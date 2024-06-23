#include "questAnalyze.cpp"

int main()
{
    if (!Redis::get().connect("localhost", 6379))
    {
        std::cout << "ERROR: Couldn't connect to the redis server.\n"
                  << "Make sure the Redis server is ready "
                  << "(to start it: sudo systemctl start redis-server)";
        return 1;
    }

    QuestAnalyze *monitor = new QuestAnalyze("postgresql://postgres:postgres@localhost/gridquest");
    StreamParser::runMonitors({monitor});
    std::cout << "\n\nProcess Completed!\n";
}