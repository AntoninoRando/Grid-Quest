#include "monitors.h"
#include <iostream>
#include <conio.h>
#include <cassert>
#include <string.h>
#include <optional>
#include <map>
#include <list>
#include <pqxx/pqxx>
#include <format>

using std::map, std::optional, std::string, std::list, std::pair;
using opChar = optional<const char *>;

class ParserSession : public ParserState
{
    std::string player;
    long long startTime;

public:
    int id = -1;

    ParserSession(std::string player, long long startTime)
    {
        this->player = player;
        this->startTime = startTime;
    }

    void execCommitQueries(pqxx::work transaction) override
    {
        query_ << "SELECT add_game_session('"
               << player << "', "
               << startTime << ")";
        std::string query = prettyPrintQuery();

        try
        {
            pqxx::result res = transaction.exec(query);
            id = res[0][0].as<int>();
            std::cout << "Query executed successfully\n"
                      << "\t(result) Session ID: " << id << "\n";
        }
        catch (const pqxx::sql_error &e)
        {
            std::cerr << "SQL Error: " << e.what() << '\n';
            transaction.abort();
            return;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << '\n';
            transaction.abort();
            return;
        }

        transaction.commit();
    }
};

class ParserScene : public ParserState
{
    std::string sceneName;
    long long startTime;

public:
    int ord;
    int session;
    long long endTime;
    float maxWait = 0;
    float avgWait = 0;
    int count = 0;

    ParserScene(int session, std::string sceneName, long long startTime, int ord)
    {
        this->session = session;
        this->sceneName = sceneName;
        this->startTime = startTime;
        this->ord = ord;
    }
    void execCommitQueries(pqxx::work transaction) override
    {
        query_ << "SELECT add_game_scene("
               << session << ", "
               << ord << ", "
               << "'" << sceneName << "', "
               << (endTime - startTime) << ", "
               << maxWait << ", "
               << avgWait << ")";
        std::string query = prettyPrintQuery();

        try
        {
            pqxx::result res = transaction.exec(query);
            std::cout << "Query executed successfully\n"
                      << "\t(result) " << res[0][0].as<bool>() << "\n";
        }
        catch (const pqxx::sql_error &e)
        {
            std::cerr << "SQL Error: " << e.what() << '\n';
            transaction.abort();
            return;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << '\n';
            transaction.abort();
            return;
        }

        transaction.commit();
    }
};

float calculateNewAverage(float oldAverage, float newElement, int count)
{
    return (oldAverage * count + newElement) / (count + 1);
}

void readStreamString(const char *str,
                      ParserContext &context,
                      long long entryTime,
                      opChar timeType,
                      bool isScene)
{
    if (isScene && context.state() != nullptr)
    {
        int sessionID;
        int sceneOrd = 0;
        context.complete();
        ParserSession *currentState = dynamic_cast<ParserSession *>(context.state());
        if (currentState != nullptr)
            sessionID = currentState->id;
        else
        {   
            ParserScene *sc = dynamic_cast<ParserScene *>(context.state());
            sessionID = sc->session;
            sceneOrd = sc->ord + 1;
            sc->endTime = entryTime;
        }
        context.transitionTo(new ParserScene(sessionID, str, entryTime, sceneOrd));
    }
    else if (timeType.has_value() && context.state() != nullptr)
    {
        float time = std::stof(str);
        ParserScene *sc = dynamic_cast<ParserScene *>(context.state());

        if (sc == nullptr)
            return;

        sc->count++;
        sc->maxWait = std::max(sc->maxWait, time);
        sc->avgWait = calculateNewAverage(sc->avgWait, time, sc->count);
    }
    else if (strcmp(str, "game-start") == 0)
    {
        context.transitionTo(new ParserSession("debug_user", entryTime));
    }
}

int parseStream(redisReply *r,
                ParserContext &context,
                int level = 0,
                long long entryTime = 0,
                opChar timeType = {},
                bool isScene = false)
{
    bool entryTimePassed = level > 1;
    string elStr;
    map<string, float> infos;
    float time;

    switch (r->type)
    {
    case REDIS_REPLY_STRING:
        readStreamString(r->str, context, entryTime, timeType, isScene);
        break;
    case REDIS_REPLY_ARRAY:
        for (size_t i = 0; i < r->elements; i++)
        {
            auto el = r->element[i];

            // This entry is the time at which all following entries were put
            if (!entryTimePassed && el->type == REDIS_REPLY_STRING)
            {
                elStr = el->str;
                entryTime = std::stoll(elStr.substr(0, elStr.find("-")));
                entryTimePassed = true;
            }

            if (parseStream(el, context, level + 1, entryTime, timeType, isScene) == -1)
                return -1;

            if (el->type == REDIS_REPLY_STRING)
            {
                elStr = el->str;
                if (elStr.rfind("time(ms):", 0) == 0) // Next element is a time
                    timeType = elStr.substr(9).c_str();
                else
                    timeType.reset();

                isScene = (elStr == "enter-state");
            }
            else
            {
                timeType.reset();
                isScene = false;
            }
        }
        break;
    case REDIS_REPLY_STATUS:
    case REDIS_REPLY_INTEGER:
    case REDIS_REPLY_NIL:
    case REDIS_REPLY_ERROR:
        break;
    default:
        std::cout << "(unknown-type) " << r->type << "\n";
        return -1;
    }
    return 0;
}

int main()
{
    if (!Redis::get().connect("localhost", 6379))
    {
        std::cout << "ERROR: Couldn't connect to the redis server.\n"
                  << "Make sure the Redis server is ready "
                  << "(to start it: sudo systemctl start redis-server)";
        return 1;
    }

    ParserContext context("postgresql://postgres:postgres@localhost/gridquest");

    auto *reply = (redisReply *)Redis::get().run("XRANGE gridquest - +");
    int error = parseStream(reply, context);
    assert(error != -1 && "Unexpected reply type.");
    context.complete();
    std::cout << "Process Completed!";
}