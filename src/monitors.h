#ifndef MONITORS_H_
#define MONITORS_H_

#include <hiredis.h>

class Redis
{
public:
    static redisContext *context;
    static bool connect(const char *ip, int port);
    static void *runCommand(const char *format, ...);
};

#endif