#include "state.h"
#include "settings.h"
#include "monitors.h"
#include <conio.h>
#include <pqxx/pqxx>

// We use W-A-S-D for movement because the _getchr() return twice with arrows keys:
// https://learn.microsoft.com/en-us/previous-versions/visualstudio/visual-studio-2010/078sfkak(v=vs.100)?redirectedfrom=MSDN

void addNewUser(std::string nickname, pqxx::work *addUserWork)
{
    std::string error;

    while (true)
    {
        std::cout << "Enter a nickname: ";
        std::getline(std::cin, nickname);
        std::cout << "\n";

        error = GlobalSettings::profileInfo->Change("Nickname = " + nickname);

        if (error.length() > 0)
            std::cout << error << "\n";
        else
        {
            pqxx::result result = addUserWork->exec("SELECT add_user('" + nickname + "', CURRENT_DATE)");
            bool added = result[0][0].as<bool>();
            if (!added)
                std::cout << "E: Nickname-already-in-use.\n";
            else
                return;
        }
    }
}

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

    Redis::get(LOG_STREAM) << "message Game-start author player result 0";
    Redis::get(LOG_STREAM).push();

    int error = GlobalSettings::load();
    Redis::get(LOG_STREAM) << "message Load-settings author player result " << error;
    Redis::get(LOG_STREAM).push();

    pqxx::connection sqlConn("postgresql://postgres:postgres@localhost/gridquest");
    pqxx::work fetchProfile(sqlConn);
    std::string nickname("");
    try
    {
        pqxx::result fetchResult = fetchProfile.exec("SELECT * FROM last_profile_to_play()");
        nickname = fetchResult[0][0].as<std::string>();
        GlobalSettings::profileInfo->Change("Nickname = " + nickname);
        fetchProfile.commit();
    }
    catch (const pqxx::sql_error &e)
    {
        fetchProfile.abort();
        pqxx::work addUserWork(sqlConn);
        addNewUser(nickname, &addUserWork);
        addUserWork.commit();
    }
    catch (const std::exception &e)
    {
        fetchProfile.abort();
        pqxx::work addUserWork(sqlConn);
        addNewUser(nickname, &addUserWork);
        addUserWork.commit();
    }

    GlobalSettings::loadProfile();

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

    // We trim the stream at the end of the code to ensure that the stream is
    // trimmed at a point that won't affect the Monitors.
    StreamParser::runMonitors({}, 0);
}