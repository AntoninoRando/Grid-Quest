#include "monitors.h"
#include <iostream>
#include <conio.h>
#include <string.h>

using std::string;

// Below we define the MonitorState classes that implement the state-transaction
// function for the monitor `InputsObserve`.

class AddInput : public MonitorState
{
    string input_;
    string action_;

public:
    AddInput(string input, string action)
    {
        input_ = input;
        action_ = action;
    }

    bool execCommitQueries(pqxx::work transaction) override
    {
        query_ << "INCRBY inputs:" << input_ << ":" << action_ << " 1";
        string query = prettyPrintQuery();
        // Executing a REDIS command with Redis class fill free the previous reply.
        // This is a problem is we're using the old reply to read the stream.
        // Thus, we use runNoFree to avoid freeing the last reply.
        auto reply = Redis::get().runNoFree(query.c_str());
        freeReplyObject(reply);
        transaction.commit();
        return true;
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
class InputsObserve : public Monitor
{
    string input_;

public:
    using Monitor::Monitor; // Inherit constructor.

    void stateTransition(const string id, const string key, const string value) override
    {
        if (key == "input")
            input_ = value;
        else if (key == "action")
        {
            setState(new AddInput(input_, value));
            executeStateQuery();
        }
    }
};