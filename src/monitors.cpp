#include "monitors.h"

redisContext *Redis::context;

bool Redis::connect(const char *ip, int port)
{
    context = redisConnect(ip, port);
    if (context != nullptr && context->err)
        return false;
    return true;
}

void *Redis::runCommand(const char *format, ...)
{
    void *r = redisCommand(context, format);
    freeReplyObject(r);
    return r;
    return nullptr;
}
