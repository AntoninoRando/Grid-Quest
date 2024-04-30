#ifndef MONITORS_H_
#define MONITORS_H_

#include <hiredis.h>

class Redis
{
public:
    static redisContext *context;
    static bool connect(const char *ip, int port);
    static void *run(const char *format, ...);
};

class TimePlayed
{
    int totalSeconds = 0;
    int lastGameSeconds = 0;
public:
    void update();
};

class GamesPlayed
{
    int won = 0;
    int lost = 0;
    int interrupted = 0;
    int started = 0;

public:
    void update();
};

class GameDetails
{

};

#endif