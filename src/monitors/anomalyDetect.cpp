#include "monitors.h"
#include <iostream>
#include <conio.h>
#include <string.h>

using std::string;

// Below we define the MonitorState classes that implement the state-transaction
// function for the monitor `AnomalyDetect`.

class CountError : public MonitorState
{
    string errorDetail_;

public:
    CountError(string errorDetail)
    {
        errorDetail_ = errorDetail;
    }

    bool execCommitQueries(pqxx::work transaction) override
    {
        query_ << "INCRBY error:" << errorDetail_ << " 1";
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

class AnomalyDetect : public Monitor
{
    bool error_ = false;
    string input_;
    int errorCount_ = 0;

public:
    using Monitor::Monitor; // Inherit constructor.

    int countErrors() { return errorCount_; }

    void stateTransition(const string id, const string key, const string value) override
    {
        if (key == "message")
            error_ = false; // Reset error flag when a new message is read.
        else if (key == "result" && value == "1")
        {
            error_ = true;
            errorCount_++;
        }
        else if (error_ && key == "details")
        {
            setState(new CountError(value));
            executeStateQuery();
        }
    }
};