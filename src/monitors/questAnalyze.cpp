#include "monitors.h"
#include <iostream>
#include <conio.h>
#include <string.h>
#include <optional>

using std::string;

// Below we define the MonitorState classes that implement the state-transaction
// function for the monitor `QuestAnalyze`.

class AddQuest : public MonitorState
{
    string player_;
    string questStartTime_;
    int questOrd_;
    string grid_;
    string goal_;
    string hp_;
    std::optional<string> result_;

public:
    AddQuest(string player,
             string questStartTime,
             int questOrd,
             string grid, string goal,
             string hp,
             std::optional<string> result = {})
    {
        player_ = player;
        questStartTime_ = questStartTime;
        questOrd_ = questOrd;
        grid_ = grid;
        goal_ = goal;
        hp_ = hp;
        result_ = result;
    }

    bool execCommitQueries(pqxx::work transaction) override
    {
        query_ << "SELECT add_quest("
               << "'" << player_ << "', "
               << questStartTime_ << ", "
               << questOrd_ << ", "
               << "'" << grid_ << "', "
               << goal_ << ", "
               << hp_;
        if (result_.has_value())
            query_ << ", " << result_.value();
        query_ << ")";

        string query = prettyPrintQuery();

        try
        {
            pqxx::result res = transaction.exec(query);
            prettyPrintResult(res[0][0].as<bool>());
            transaction.commit();
            return true;
        }
        catch (const pqxx::sql_error &e)
        {
            prettyPrintResult(e.what(), true);
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

class QuestAnalyze : public Monitor
{
    string player_;
    string questStartTime_;
    int questOrd_;
    string grid_;
    string goal_;
    string hp_;
    std::optional<string> result_;

public:
    using Monitor::Monitor; // Inherit constructor.

    void stateTransition(const string id, const string key, const string value) override
    {
        // id is written in the form: "epoch-serialId", where epoch is the time
        // the entry was written in the stream and serialId is a serial number
        // that Redis uses to differentiate entries written in the same epoch.
        // We are only interest in epoch.
        string entryTime = id.substr(0, id.find("-"));

        if (key == "game-start")
            questOrd_ = 0;
        else if (key == "use-nickname")
            player_ = value;
        else if (key == "enter-state" && value == "Quest")
            questStartTime_ = entryTime;
        else if (key == "quest-grid")
            grid_ = value;
        else if (key == "quest-goal")
            goal_ = value;
        else if (key == "quest-result")
            result_ = value;
        else if (key == "quest-hp") // If we have quest-hp, we know it ended.
            hp_ = value;
        else if (key == "quest-end")
        {
            setState(new AddQuest(player_, questStartTime_, questOrd_, grid_, goal_, hp_, result_));
            executeStateQuery();
            questOrd_++;
        }
    }
};