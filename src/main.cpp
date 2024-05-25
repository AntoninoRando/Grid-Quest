#include "state.h"
#include "settings.h"
#include "monitors.h"
#include <conio.h>

// We use W-A-S-D for movement because the _getchr() return twice with arrows keys:
// https://learn.microsoft.com/en-us/previous-versions/visualstudio/visual-studio-2010/078sfkak(v=vs.100)?redirectedfrom=MSDN

int main()
{
    if (!Redis::get().connect("localhost", 6379))
    {
        std::cout << "ERROR: Couldn't connect to the redis server.\n"
                  << "Make sure the Redis server is ready "
                  << "(to start it: sudo systemctl start redis-server)\n\n"
                  << "Press any key to exit...";
        _getch();
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
        game.show();
        char input = _getch();
        game.processInput(input);
    }
}