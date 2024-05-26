#include "monitors.h"
#include <iostream>
#include <conio.h>
#include <cassert>
#include <string.h>
#include <map>

int parseStream(redisReply *r, std::map<std::string, int> &gameInfos)
{
    switch (r->type)
    {
    case REDIS_REPLY_STRING:
        std::cout << "(string) " << r->str;
        if (strcmp(r->str, "quest-quit") == 0)
            gameInfos["quest-quit"]++;
        else if (strcmp(r->str, "quest-won") == 0)
            gameInfos["quest-won"]++;
        else if (strcmp(r->str, "no-hp") == 0)
            gameInfos["quest-lost-no-hp"]++;
        else if (strcmp(r->str, "no-match") == 0)
            gameInfos["quest-lost-no-match"]++;
        break;
    case REDIS_REPLY_STATUS:
        std::cout << "(status) " << r->str;
        break;
    case REDIS_REPLY_INTEGER:
        std::cout << "(integer) " << r->integer;
        break;
    case REDIS_REPLY_NIL:
        std::cout << "(nill)";
        break;
    case REDIS_REPLY_ERROR:
        std::cout << "(error) " << r->str;
        break;
    case REDIS_REPLY_ARRAY:
        for (size_t i = 0; i < r->elements; i++)
        {
            auto el = r->element[i];
            int error = parseStream(el, gameInfos);
            if (error == -1)
                return -1;
        }
        break;
    default:
        std::cout << "(unknown-type) " << r->type;
        return -1;
    }
    std::cout << "\n";
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

    std::map<std::string, int> gameInfos;

    auto *reply = (redisReply *)Redis::get().run("XRANGE gridquest - +");
    assert(parseStream(reply, gameInfos) != -1 && "Unexpected reply type.");

    int count = gameInfos["quest-quit"] + gameInfos["quest-won"] + gameInfos["quest-lost-no-hp"] + gameInfos["quest-lost-no-match"];

    std::stringstream command;
    command << "INCRBY quest-played " << count;
    Redis::get().run(command.str().c_str());

    std::map<std::string, int>::iterator itr;
    for (itr = gameInfos.begin(); itr != gameInfos.end(); ++itr)
    {
        command.str("");
        command << "INCRBY " << itr->first << " " << itr->second;
        Redis::get().run(command.str().c_str());
    }
    std::cout << "Process completed | Counted " << count << " games" << std::endl;
    return 0;
}