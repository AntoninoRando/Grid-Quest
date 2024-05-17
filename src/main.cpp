#include "state.h"
#include "settings.h"
#include "monitors.h"
#include <conio.h>
#include <chrono>
#include <sstream>

// We use W-A-S-D for movement because the _getchr() return twice with arrows keys:
// https://learn.microsoft.com/en-us/previous-versions/visualstudio/visual-studio-2010/078sfkak(v=vs.100)?redirectedfrom=MSDN

int main()
{
    if (!Redis::get().connect("127.0.0.1", 6379))
    {
        std::cout << "ERROR: Couldn't connect to the redis server.";
        return 1;
    }

    GlobalSettings::load();
    std::cout << "\u001b[" + BG_COL + "m";

    Context game;
    game.transitionTo(new Opening);

    // Serve per visualizzare i caratteri speciali su console, e.g. i colori.
    system("chcp 65001");

    while (true)
    {
        auto start = std::chrono::system_clock::now();
        game.show();
        auto end = std::chrono::system_clock::now();
        auto showTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        Redis::get() << "show_time " << showTime << "ms";
        Redis::get().push();

        char input = _getch();
        Redis::get() << "input " << input;
        Redis::get().push();

        game.processInput(input);
    }
}