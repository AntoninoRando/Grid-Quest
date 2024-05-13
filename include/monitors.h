#ifndef MONITORS_H_
#define MONITORS_H_

#include <hiredis.h>
#include <string>
#include <ostream>

#define STREAM_NAME "gridquest"

class Redis
{
    
public:
    static void *lastReply;
    static redisContext *context;
    static bool connect(const char *ip, int port);
    static void *run(const char *format, ...);
    static void *putInStream(std::string command);
};

#endif