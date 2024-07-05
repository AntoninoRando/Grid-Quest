#include "anomalyDetect.cpp"

int main()
{
    if (!Redis::get().connect("localhost", 6379))
    {
        std::cout << "ERROR: Couldn't connect to the redis server.\n"
                  << "Make sure the Redis server is ready "
                  << "(to start it: sudo systemctl start redis-server)";
        return 1;
    }

    AnomalyDetect *monitor = new AnomalyDetect("postgresql://postgres:postgres@localhost/gridquest");
    StreamParser::runMonitors({monitor}, -1, LOG_STREAM);
    std::cout << "\n\nProcess Completed!\n"
              << "Detected anomalies: " << monitor->countErrors() << "\n";
}