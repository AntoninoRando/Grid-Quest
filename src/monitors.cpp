#include "monitors.h"

redisContext *Redis::context;

bool Redis::connect(const char *ip, int port)
{
    context = redisConnect(ip, port);
    if (context != nullptr && context->err)
        return false;
    return true;
}

void *Redis::run(const char *format, ...)
{
    void *r = redisCommand(context, format);
    freeReplyObject(r);
    return r;
}

void TimePlayed::update()
{
    Redis::run("INCRBY playtime %d", totalSeconds);
}

void GamesPlayed::update()
{
    Redis::run("INCRBY started %d", started);
    Redis::run("INCRBY won %d", won);
    Redis::run("INCRBY lost %d", lost);
    Redis::run("INCRBY interrupted %d", interrupted);
    started = 0;
    won = 0;
    lost = 0;
    interrupted = 0;
}
