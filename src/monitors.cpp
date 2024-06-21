#include "monitors.h"
#include <iostream>

bool Redis::connect(const char *ip, int port)
{
    context = redisConnect(ip, port);
    if (context != nullptr && context->err)
        return false;
    return true;
}

void *Redis::run(const char *format, ...)
{
    freeReplyObject(lastReply);
    lastReply = redisCommand(context, format);
    if (lastReply == NULL)
    {
        std::string error("ERROR:BAD-COMMAND ");
        error.append(context->errstr);
        putInStream(error);
    }
    return lastReply;
}

void *Redis::putInStream(std::string command)
{
    std::string s("XADD ");
    s.append(STREAM_NAME).append(" * ").append(command);
    return run(s.c_str());
}

void *Redis::push()
{
    void *result = putInStream(streamCommand_.str());
    streamCommand_.str(""); // Clear the stream
    streamCommand_.clear(); // Clear any error flags
    return result;
}

std::string ParserState::prettyPrintQuery()
{
    std::string query = query_.str();
    std::string sep(query.length(), '-');
    std::cout << "SQL ------------------------" << sep << "\n"
              << ">>> Executing postgreSQL query: " << query << "\n";

    return query;
}

void ParserContext::complete()
{
    if (lastStateCompleted)
        return;

    state_->execCommitQueries(pqxx::work(*connection));
    lastStateCompleted = true;
}

void ParserContext::transitionTo(ParserState *state)
{
    if (!lastStateCompleted)
        complete();
    state_ = state;
    state_->setContext(this);
    lastStateCompleted = false;
}

std::string MonitorState::prettyPrintQuery()
{
    std::string query = query_.str();
    std::cout << "\033[93mSQL executing >>> \033[94m" << query << "\033[0m\n";
    return query;
}

void StreamParser::runMonitors_(
    redisReply *reply,
    std::vector<Monitor *> monitors,
    int replyLevel,
    std::optional<const char *> entryId)
{
    if (reply->type != REDIS_REPLY_ARRAY)
        return;

    for (size_t i = 0; i < reply->elements; i++)
    {
        auto e1 = reply->element[i];

        // if e1 was not a string, it is an array (if it is not, the recursive
        // call will ignore it) and we need to repeat the parsing.
        if (e1->type != REDIS_REPLY_STRING)
        {
            runMonitors_(e1, monitors, replyLevel + 1, entryId);
            continue;
        }

        // e1 is a string and the stream entry id has not been found yet, thus
        // e1 is the entry id.
        if (!entryId.has_value())
        {
            entryId = {e1->str};
            continue;
        }

        // e1 is a string and the stream entry has been found, thus e1 is a key
        // and e2 is its value.
        i++;
        auto e2 = reply->element[i];
        for (auto monitor : monitors)
            monitor->stateTransition(entryId.value(), e1->str, e2->str);
    }
}

void StreamParser::runMonitors(std::vector<Monitor *> monitors)
{
    auto *reply = (redisReply *)Redis::get().run("XRANGE gridquest - +");
    runMonitors_(reply, monitors);
}
