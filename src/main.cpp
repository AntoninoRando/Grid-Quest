#include "state.h"
#include "settings.h"
#include <conio.h>
#include <hiredis.h>

// We use wasd for movement because the _getchr() return twice with arrows keys:
// https://learn.microsoft.com/en-us/previous-versions/visualstudio/visual-studio-2010/078sfkak(v=vs.100)?redirectedfrom=MSDN

int main()
{
    GlobalSettings::load();
    std::cout << "\u001b[" + BG_COL + "m";

    redisContext *c = redisConnect("127.0.0.1", 6379);
    if (c != nullptr && c->err)
    {
        std::cout << "ERROR Trying to connect to redis server";
        return 1;
    }

    Context game;
    game.transitionTo(new Opening);

    // Serve per visualizzare i caratteri speciali su console, e.g. i colori.
    system("chcp 65001");

    while (true)
    {
        game.show();
        game.processInput(_getch());
    }
}