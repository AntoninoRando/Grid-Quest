#include "monitors.h"
#include <iostream>
#include <conio.h>
#include <cassert>
#include <string.h>
#include <optional>
#include <map>

using std::map, std::optional, std::string;

float calculateNewAverage(float oldAverage, float newElement, int count)
{
    return (oldAverage * count + newElement) / (count + 1);
}

int parseStream(redisReply *r, map<string, map<string, float>> &timeInfos, optional<const char *> key = {})
{
    string value;
    map<string, float> infos;
    float time;

    switch (r->type)
    {
    case REDIS_REPLY_STRING:
        std::cout << "(string) " << r->str;

        if (!key.has_value())
            break;

        infos = timeInfos[key.value()];
        time = std::stof(r->str);
        infos["count"]++;
        infos["avg"] = calculateNewAverage(infos["avg"], time, infos["count"]);
        infos["max"] = std::max(infos["max"], time);
        infos["min"] = std::min(infos["min"], time);
        infos["sum"] += time;
        timeInfos[key.value()] = infos;
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
            if (parseStream(el, timeInfos, key) == -1)
                return -1;

            if (el->type == REDIS_REPLY_STRING)
            {
                value = el->str;
                if (value.rfind("time(ms):", 0) == 0)
                {
                    string timeType = value.substr(9);
                    key = timeType.c_str();
                }
                else
                    key.reset();
            }
            else
                key.reset();
        }
        break;
    default:
        std::cout << "(unknown-type) " << r->type;
        return -1;
    }
    std::cout << "\n";
    return 0;
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

    map<string, map<string, float>> timeInfos;

    auto *reply = (redisReply *)Redis::get().run("XRANGE gridquest - +");
    int error = parseStream(reply, timeInfos);
    assert(error != -1 && "Unexpected reply type.");

    int count = 0;
    std::stringstream command;
    for (auto &[timeName, infos] : timeInfos)
    {
        for (auto &[key, value] : infos)
        {
            command.str("");
            command << "SET " << timeName << ":" << key << " " << value;
            std::cout << "Running command: " << command.str() << "\n";
            Redis::get().run(command.str().c_str());
            count++;
        }
    }
    std::cout << "Process completed | Added " << count << " entries" << std::endl;
    return 0;
}