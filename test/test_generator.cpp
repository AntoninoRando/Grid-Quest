#include "state.h"
#include "settings.h"
#include "monitors.h"
#include <conio.h>
#include <unistd.h>

int main(int argc, char * argv[])
{
    if (!Redis::get().connect("localhost", 6379))
        return 1;
    
    int tests = 1;
    if (argc > 1)
    {
        try
        {
            tests = std::stoi(argv[1]);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return 1;
        }
    }

    Redis::get().log("Game-start", TEST, OK);

    std::string error = GlobalSettings::load();
    if (error.length() > 0)
        Redis::get().log("Load-settings", TEST, BAD, error);
    else
        Redis::get().log("Load-settings", TEST, OK);

    char inputs[] = "wasdqe+-*/m|";
    int n = 12;
    srand(time(nullptr));

    for (int i = 0; i < tests; i++)
    {
        Context game;
        Quest *quest = new Quest;
        quest->pingTolerance = 10;
        game.transitionTo(quest);

        while (!quest->isEnd())
        {
            char input = inputs[rand() % n];
            std::string detailMessage("input:");
            detailMessage.push_back(input);
            Redis::get().log("generate-random-input", TEST, OK, detailMessage);
            game.processInput(input);
        }

        std::string detailMessage("test-number:");
        detailMessage.append(std::to_string(i));
        Redis::get().log("test-ended", TEST, OK, detailMessage);
    }
    Redis::get().log("all-test-ended", TEST, OK);
}