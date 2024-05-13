#include "monitors.h"

redisContext *Redis::context;
void *Redis::lastReply;

bool Redis::connect(const char *ip, int port)
{
    context = redisConnect(ip, port);
    if (context != nullptr && context->err)
        return false;
    return true;
}

void *Redis::run(const char *format, ...)
{
    freeReplyObject(Redis::lastReply);
    Redis::lastReply = redisCommand(context, format);
    return Redis::lastReply;
}

void *Redis::putInStream(std::string command)
{
    std::string s("XADD ");
    s.append(STREAM_NAME).append(" * ").append(command);
    return Redis::run(s.c_str());
}