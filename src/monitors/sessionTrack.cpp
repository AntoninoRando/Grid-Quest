#include "monitors.h"
#include <iostream>
#include <conio.h>
#include <string.h>
#include <pqxx/pqxx>

using std::string;

// Below we define the MonitorState classes that implement the state-transaction
// function for the monitor `SessionTracker`.

/**
 * @brief The monitor state in which a new user has been read in the Redis
 * stream and it should now be added to the database.
 */
class AddUser : public MonitorState
{
    string player_;
    string addTime_;

public:
    AddUser(string player, string addTime)
    {
        player_ = player;
        addTime_ = addTime;
    }

    bool execCommitQueries(pqxx::work transaction) override
    {
        // The SQL add_user function will do nothing if this user already
        // exists, thus it is safe to use without checks.
        query_ << "SELECT add_user('"
               << player_ << "', "
               << addTime_ << ")";
        string query = prettyPrintQuery();

        try
        {
            pqxx::result res = transaction.exec(query);
            bool userAdded = res[0][0].as<bool>();
            std::cout << "\033[32m(RESULT)\033[0m " << userAdded << "\n";
            transaction.commit();
            return true;
        }
        catch (const pqxx::sql_error &e)
        {
            std::cerr << "\033[91m(RESULT)\033[0m " << e.what() << '\n';
            transaction.abort();
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << '\n';
            transaction.abort();
        }
        return false;
    }
};

/**
 * @brief The monitor state in which a new user session has been read on the
 * Redis stream and it should be now added to the database.
 */
class AddSession : public MonitorState
{
    string player_;
    string addTime_;
    int id_ = -2;

public:
    AddSession(string player, string addTime)
    {
        player_ = player;
        addTime_ = addTime;
    }

    int id() { return id_; }

    bool execCommitQueries(pqxx::work transaction) override
    {
        // The SQL add_user function will do nothing if this session already
        // exists (-1 is returned), thus it is safe to use without checks.
        query_ << "SELECT add_game_session('"
               << player_ << "', "
               << addTime_ << ")";
        string query = prettyPrintQuery();

        try
        {
            pqxx::result res = transaction.exec(query);
            int queryReturn = res[0][0].as<int>();
            std::cout << "\033[32m(RESULT)\033[0m " << queryReturn << "\n";
            id_ = queryReturn;
            transaction.commit();
            return true;
        }
        catch (const pqxx::sql_error &e)
        {
            std::cerr << "\033[91m(RESULT)\033[0m " << e.what() << '\n';
            transaction.abort();
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << '\n';
            transaction.abort();
        }
        return false;
    }
};

/**
 * @brief The monitor state in which a new user session has been read on the
 * Redis stream and it should be now added to the database.
 */
class AddScene : public MonitorState
{
    string sceneName_;
    string startTime_;
    int ord_;
    int session_;
    string endTime_;
    float maxWait_ = 0;
    float avgWait_ = 0;
    int waitCount_ = 0;

public:
    AddScene(int session, string sceneName, string startTime, int ord)
    {
        session_ = session;
        sceneName_ = sceneName;
        startTime_ = startTime;
        ord_ = ord;
    }

    void newTime(float time)
    {
        waitCount_++;
        maxWait_ = std::max(maxWait_, time);
        avgWait_ = (avgWait_ * (waitCount_ - 1) + time) / waitCount_;
    }

    void addEndTime(string endTime) { endTime_ = endTime; }

    bool execCommitQueries(pqxx::work transaction) override
    {
        query_ << "SELECT add_game_scene("
               << session_ << ", "
               << ord_ << ", "
               << "'" << sceneName_ << "', "
               << startTime_ << ", "
               << endTime_ << ", "
               << maxWait_ << ", "
               << avgWait_ << ")";
        string query = prettyPrintQuery();

        try
        {
            pqxx::result res = transaction.exec(query);
            std::cout << "\033[32m(RESULT)\033[0m " << res[0][0].as<bool>() << "\n";
            transaction.commit();
            return true;
        }
        catch (const pqxx::sql_error &e)
        {
            std::cerr << "\033[91m(RESULT)\033[0m " << e.what() << '\n';
            transaction.abort();
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << '\n';
            transaction.abort();
        }

        return false;
    }
};

// The monitor

/**
 * @brief A monitor that tracks the users that play the game and when they play.
 *
 * This monitor is used to track:
 * - New users;
 * - Users playing sessions;
 * - Game scenes in which users spend their time during the playing sessions;
 * - Time users spent waiting for a game scene to load.
 *
 * This monitor uses 3 states to perform its job: AddUser, AddSession, AddScene.
 */
class SessionTracker : public Monitor
{
    string player_;
    int sessionId_;
    int sceneOrd_;
    AddScene *scene_ = nullptr;

public:
    using Monitor::Monitor; // Inherit constructor.

    void stateTransition(const string id, const string key, const string value) override
    {
        // id is written in the form: "epoch-serialId", where epoch is the time
        // the entry was written in the stream and serialId is a serial number
        // that Redis uses to differentiate entries written in the same epoch.
        // We are only interest in epoch.
        string entryTime = id.substr(0, id.find("-"));

        if (key == "use-nickname")
        {
            // Commit last scene if it exists.
            if (scene_ != nullptr)
            {
                scene_->addEndTime(entryTime);
                executeStateQuery();
                scene_ = nullptr;
            }

            player_ = value;
            setState(new AddUser(player_, entryTime));
            executeStateQuery();
        }
        else if (key == "game-start")
        {
            // Commit last scene if it exists.
            if (scene_ != nullptr)
            {
                scene_->addEndTime(entryTime);
                executeStateQuery();
                scene_ = nullptr;
            }

            AddSession *as = new AddSession(player_, entryTime);
            setState(as);
            executeStateQuery();
            sessionId_ = as->id();
            sceneOrd_ = 0; // Reset scene order for following scenes.
        }
        else if (key == "enter-state")
        {
            // Commit last scene if it exists.
            if (scene_ != nullptr)
            {
                scene_->addEndTime(entryTime);
                executeStateQuery();
            }

            scene_ = new AddScene(sessionId_, value, entryTime, sceneOrd_);
            setState(scene_);
            sceneOrd_++; // Increase ord for next scene.
        }
        else if (scene_ != nullptr && key.rfind("time(ms):", 0) == 0)
        {
            // string timeType = key.substr(9);
            float milliseconds = std::stof(value);
            scene_->newTime(milliseconds);
        }
        else if (scene_ != nullptr && key == "game-end")
        {
            scene_->addEndTime(entryTime);
            executeStateQuery();
            scene_ = nullptr;
        }
    }
};