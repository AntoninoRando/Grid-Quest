#ifndef MONITORS_H_
#define MONITORS_H_

#include <hiredis.h>
#include <string>
#include <sstream>
#include <pqxx/pqxx>

#define STREAM_NAME "gridquest"

// Singleton: https://stackoverflow.com/questions/1008019/how-do-you-implement-the-singleton-design-pattern?answertab=scoredesc#tab-top

/**
 * @brief A singleton class representing the connection with the Redis server.
 */
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

class ParserContext;

class ParserState
{
protected:
    ParserContext *context_;
    std::stringstream query_;

public:
    /**
     * @brief Change the current context.
     */
    void setContext(ParserContext *context) { context_ = context; }
    
    /**
     * @brief Execute the SQL query to commit the information parsed in this
     * state.
     */
    virtual void execCommitQueries(pqxx::work) = 0;
    std::string prettyPrintQuery();
};

class ParserContext
{
    ParserState *state_ = nullptr;
    pqxx::connection *connection;
    bool lastStateCompleted = true;

public:
    ParserContext(const char *db_url)
    {
        connection = new pqxx::connection(db_url);
    }
    ParserState *state() { return state_; }
    void complete();
    void transitionTo(ParserState *state);
};

#endif