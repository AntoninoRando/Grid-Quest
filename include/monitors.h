#ifndef MONITORS_H_
#define MONITORS_H_

#include <hiredis.h>
#include <string>
#include <sstream> 

#define STREAM_NAME "gridquest"

// Singleton: https://stackoverflow.com/questions/1008019/how-do-you-implement-the-singleton-design-pattern?answertab=scoredesc#tab-top
class Redis
{
    Redis() = default;  // Private constructor to prevent instantiation
    ~Redis() = default; // Private destructor

    std::ostringstream streamCommand_;

public:
    Redis(Redis const &) = delete;
    void operator=(Redis const &) = delete;

    static Redis &get()
    {
        static Redis instance; // Guaranteed to be destroyed.
                               // Instantiated on first use.
        return instance;
    }

    void *lastReply;
    redisContext *context;
    bool connect(const char *ip, int port);
    void *run(const char *format, ...);
    void *putInStream(std::string command);
    void *push();

    // Overload the << operator for various types
    template <typename T>
    Redis &operator<<(const T &value)
    {
        streamCommand_ << value;
        return get();
    }
};

#endif