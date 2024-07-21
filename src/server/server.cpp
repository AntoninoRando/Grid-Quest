#include "server_quest.h"
#include "monitors.h"
#include <conio.h>

class InputReader : public Monitor
{
    QuestGame *quest_;
    bool inGame_ = false;
    int replyCheck_ = 0;
    std::string lastId_ = "0-0";

public:
    using Monitor::Monitor; // Inherit constructor.

    int replyCheck() { return replyCheck_; }
    std::string retrieveLastId() { return lastId_; }

    void createQuest()
    {
        Redis::get().log("quest-creation", SERVER, OK);
        quest_ = new QuestGame();
        pushQuestInfos();
    }

    void pushQuestInfos()
    {
        auto reply = Redis::get().runNoFree(("SET gridquest:gridString " + quest_->gridString()).c_str());
        freeReplyObject(reply);
        reply = Redis::get().runNoFree("SET gridquest:remaining %d", quest_->remaining());
        freeReplyObject(reply);
        reply = Redis::get().runNoFree("SET gridquest:hp %d", quest_->hp());
        freeReplyObject(reply);
        reply = Redis::get().runNoFree("SET gridquest:nextHp %d", quest_->nextHp());
        freeReplyObject(reply);
        reply = Redis::get().runNoFree("SET gridquest:quest %d", quest_->quest());
        freeReplyObject(reply);
    }

    void stateTransition(const std::string id, const std::string key, const std::string value) override
    {
        lastId_ = id;
        replyCheck_++;
        std::string endStatus;

        if (key != "action")
            return;

        if (value == "Quit-quest")
        {
            inGame_ = false;
        }
        else if (value == "Start-quest")
        {
            inGame_ = true;
            replyCheck_ = 0;
            auto reply = (redisReply *)Redis::get().runNoFree("SET gridquest:endStatus none");
            freeReplyObject(reply);
        }
        else if (value == "Drop-quest")
        {
            inGame_ = false;
            quest_ = new QuestGame();
        }
        else if (inGame_)
        {
            Redis::get().log("process-action-of-input", SERVER, OK, "action:" + value);
            quest_->processAction(value);

            endStatus = quest_->endStatus();
            if (endStatus != "none")
            {
                inGame_ = false;
                quest_ = new QuestGame();
                auto reply = (redisReply *)Redis::get().runNoFree(("SET gridquest:endStatus " + endStatus).c_str());
                freeReplyObject(reply);
                return;
            }
        }
    }
};

int main()
{
    if (!Redis::get().connect("localhost", 6379))
        return 1;

    Redis::get().log("server-start", SERVER, OK);

    std::string command = "XREAD STREAMS gridquest:inputs ";
    std::string lastEntryId = "0-0";
    InputReader *inputReader = new InputReader();
    inputReader->createQuest();

    /*
    1) 1) "gridquest:inputs"
       2) 1) 1) "1720989246984-0"
             2) 1) "input"
                2) "27"
          2) 1) "1720989247092-0"
             2) 1) "input"
                2) "\r"
          ...

    - 1° reply is an array of streams. In this case, there is only 1 element and
      it is another array; call this element the 2° reply.
    - 2° reply is an array with 2 elements. The first is the stream name and the
      second is an array of stream entries; call this element the 3° reply.
    - 3° reply is the array we need to parse.
    */

    auto reply = Redis::get().runNoFree("SET gridquest:replyCheck 0");
    freeReplyObject(reply);
    reply = (redisReply *)Redis::get().runNoFree("SET gridquest:endStatus none");
    freeReplyObject(reply);

    Redis::get().log("read-stream-start", SERVER, OK);
    while (true)
    {
        // STEP 1: READ FROM STREAM REDIS
        redisReply *reply;
        try
        {
            reply = (redisReply *)Redis::get().runNoFree((command + lastEntryId).c_str());
        }
        catch (const std::exception &e)
        {
            Redis::get().log("get-stream-elements:1", SERVER, BAD, "redis-run-exception");
            freeReplyObject(reply);
            return 1;
        }

        // STEP 2: PERFORM COMPUTATIONS
        if (reply->type == REDIS_REPLY_NIL)
        {
            freeReplyObject(reply);
            continue;
        }

        if (reply->type != REDIS_REPLY_ARRAY)
        {
            Redis::get().log("get-stream-elements:2", SERVER, BAD, "reply-type-not-array");
            freeReplyObject(reply);
            return 1;
        }

        reply = reply->element[0];
        if (reply->type != REDIS_REPLY_ARRAY)
        {
            Redis::get().log("get-stream-elements:3", SERVER, BAD, "reply-type-not-array");
            freeReplyObject(reply);
            return 1;
        }

        reply = reply->element[1];
        if (reply->type != REDIS_REPLY_ARRAY)
        {
            Redis::get().log("get-stream-elements:4", SERVER, BAD, "reply-type-not-array");
            freeReplyObject(reply);
            return 1;
        }

        Redis::get().log("parse-stream-entries", SERVER, OK);
        StreamParser::runMonitors(reply, {inputReader});

        // Redis::get().log("update-last-entry-id", SERVER, OK, "id:" + lastEntryId);
        // lastEntryId = inputReader->retrieveLastId();
        freeReplyObject(reply);

        inputReader->pushQuestInfos();
        reply = (redisReply *)Redis::get().runNoFree("SET gridquest:replyCheck %d", inputReader->replyCheck());
        freeReplyObject(reply);

        // Trim stream
        reply = (redisReply *)Redis::get().runNoFree("XTRIM gridquest:inputs MAXLEN 0");
        freeReplyObject(reply);
    }
}