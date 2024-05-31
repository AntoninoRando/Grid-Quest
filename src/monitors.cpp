#include "monitors.h"

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
