#include "monitors.h"
#include <iostream>
#include <conio.h>
#include <cassert>
#include <string.h>
#include <map>

int parseStream(redisReply *r, std::map<char, int> &inputsCount, bool isInput = false)
{
    int eq = 1;
    int count = 0;
    
    switch (r->type)
    {
    case REDIS_REPLY_STRING:
        std::cout << "(string) " << r->str;
        if (isInput)
        {
            count++;
            inputsCount[r->str[0]]++;
        }
        break;
    case REDIS_REPLY_STATUS:
        std::cout << "(status) " << r->str;
        break;
    case REDIS_REPLY_INTEGER:
        std::cout << "(integer) " << r->integer;
        break;
    case REDIS_REPLY_NIL:
        std::cout << "(nill)";
        break;
    case REDIS_REPLY_ERROR:
        std::cout << "(error) " << r->str;
        break;
    case REDIS_REPLY_ARRAY:
        for (size_t i = 0; i < r->elements; i++)
        {
            auto el = r->element[i];
            count += parseStream(el, inputsCount, eq == 0);
            eq = el->type == REDIS_REPLY_STRING ? strcmp(el->str, "input") : 1;
        }
        break;
    default:
        std::cout << "(unknown-type) " << r->type;
        return -1;
    }
    std::cout << "\n";
    return count;
}

int main()
{
    if (!Redis::get().connect("localhost", 6379))
    {
        std::cout << "ERROR: Couldn't connect to the redis server.\n"
                  << "Make sure the Redis server is ready " 
                  << "(to start it: sudo systemctl start redis-server)";
        return 1;
    }

    std::map<char, int> inputsCounts;

    auto *reply = (redisReply *)Redis::get().run("XRANGE gridquest - +");
    int count = parseStream(reply, inputsCounts);
    assert(count != -1 && "Unexpected reply type.");

    std::stringstream command;
    command << "INCRBY inputs " << count;
    Redis::get().run(command.str().c_str());

    std::map<char, int>::iterator itr;
    for (itr = inputsCounts.begin(); itr != inputsCounts.end(); ++itr) {
        command.str("");
        command << "INCRBY inputs:" << itr->first << " " << itr->second;
        Redis::get().run(command.str().c_str());
    }
    std::cout << "Process completed | Counted " << count << " inputs" << std::endl;
    return 0;
}