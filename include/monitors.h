#ifndef MONITORS_H_
#define MONITORS_H_

#include <hiredis.h>
#include <string>
#include <sstream>
#include <iostream>
#include <pqxx/pqxx>

#define LOG_STREAM "gridquest:logs"

// Singleton: https://stackoverflow.com/questions/1008019/how-do-you-implement-the-singleton-design-pattern?answertab=scoredesc#tab-top

/**
 * @brief A singleton class representing the connection with the Redis server.
 */
class Redis
{
    Redis() = default;  // Private constructor to prevent instantiation
    ~Redis() = default; // Private destructor

    /**
     * @brief The current command to send to the Redis stream.
     */
    std::ostringstream streamCommand_;
    std::string currentStream_;

public:
    Redis(Redis const &) = delete;
    void operator=(Redis const &) = delete;

    static Redis &get(std::string stream = "gridquest")
    {
        static Redis instance; // Guaranteed to be destroyed.
                               // Instantiated on first use.
        instance.currentStream_ = stream;
        return instance;
    }

    void *lastReply;
    redisContext *context;
    bool connect(const char *ip, int port);
    void *run(const char *format, ...);
    void *runNoFree(const char *format, ...);
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

// Monitor and MonitorState implement the State pattern:
// https://refactoring.guru/design-patterns/state

class Monitor;

class MonitorState
{
protected:
    Monitor *monitor_;
    std::stringstream query_;
    bool printQuery_ = true;

public:
    void setMonitor(Monitor *monitor) { monitor_ = monitor_; }

    /**
     * @brief Execute and commit the SQL query to save the information parsed
     * in this state.
     *
     *
     * @param transaction The SQL transaction that will commit changes to the
     * database.
     * @return true If the SQL query was committed successfully.
     * @return false If the SQL query encountered an error.
     */
    virtual bool execCommitQueries(pqxx::work transaction) = 0;

    void disablePrint() { printQuery_ = false; }

    std::string prettyPrintQuery();

    template <typename T>
    void prettyPrintResult(T result, bool isError = false)
    {
        if (!printQuery_)
            return;
        std::cout << "\033[" << (isError ? "91m" : "32m") << "(RESULT)\033[0m " << result << '\n';
    }
};

/**
 * A finite-state machine that reads from a Redis stream and run some SQL
 * queries based on the information parsed. This is equivalent to the concept of
 * finite-state machine in Automata Theory. Thus, the Monitor class is the
 * 5-tuple
 *
 * (Q, Σ, δ, q0, F)
 *
 * where:
 * - Σ is the alphabet of input, i.e. the Redis stream;
 * - δ is the transition function, i.e. the `stateTransition` method;
 * - q0 is the initial state, i.e. the `state_` attribute.
 */
class Monitor
{
    MonitorState *state_ = nullptr;

protected:
    pqxx::connection *connection_;
    bool printQuery_ = true;

public:
    Monitor(const char *db_url) { connect(db_url); }
    void disablePrint() { printQuery_ = false; }
    MonitorState *state() { return state_; }
    void setState(MonitorState *state)
    {
        state_ = state;
        if (!printQuery_)
            state_->disablePrint();
    }
    void connect(const char *db_url) { connection_ = new pqxx::connection(db_url); }

    /**
     * @brief Execute the SQL query of this MonitorState, thus committing to the
     * database the information parsed from the Redis stream until this method
     * call.
     *
     * @return true No error occurred.
     * @return false An error occurred.
     */
    bool executeStateQuery() { return state_->execCommitQueries(pqxx::work(*connection_)); }

    /**
     * @brief The heart of the Monitor class. This function defines how the
     * monitor changes state (i.e., run SQL queries) based on what it reads from
     * the Redis stream.
     *
     * The Monitor reads the Redis stream sequentially. For every key-value
     * entry of the stream, this function is called with the id of that entry,
     * the key and the value.
     *
     * Inside this function, the state of the Monitor should be set (if changed)
     * using the `setExecState` method.
     *
     * @param id The Redis stream entry id.
     * @param key The key of the Redis stream entry.
     * @param value The value of the Redis stream entry.
     */
    virtual void stateTransition(const std::string id, const std::string key, const std::string value) = 0;

    /**
     * @brief Implement the last step for the `stateTransition` method, i.e. the
     * action to take when the stream parsing ends.
     *
     * If not overridden, this method does nothing.
     */
    virtual void stateTransitionEnd() {};
};

/**
 * @brief Reads a Redis stream of and runs a set of monitors that use that
 * stream as input.
 */
class StreamParser
{
    static void runMonitors_(redisReply *reply,
                             std::vector<Monitor *> monitors,
                             int replyLevel = 0,
                             std::optional<const char *> entryId = {});

public:
    static void runMonitors(std::vector<Monitor *> monitors, int trimStream = -1);
};

#endif