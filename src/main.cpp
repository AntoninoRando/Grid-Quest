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
        Redis::get().log("Ask-user-a-nickname", CLIENT, OK);

        std::cout << "Enter a nickname: ";
        std::getline(std::cin, nickname);
        std::cout << "\n";

        error = GlobalSettings::profileInfo->Change("Nickname = " + nickname);
        if (error.length() > 0)
        {
            Redis::get().log("Invalid-user-nickname-prompted", CLIENT, BAD, error);
            std::cout << error << "\n";
        }
        else
        {
            pqxx::result result = addUserWork->exec("SELECT add_user('" + nickname + "', CURRENT_DATE)");
            bool added = result[0][0].as<bool>();
            if (!added)
            {
                Redis::get().log("Invalid-user-nickname-prompted", CLIENT, BAD, "Nickname-already-in-use");
                std::cout << "This nickname is already in use. Please choose a different one.\n";
            }
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

    Redis::get().log("Game-start", CLIENT, OK);

    std::string error = GlobalSettings::load();
    if (error.length() > 0)
        Redis::get().log("Load-settings", CLIENT, BAD, error);
    else
        Redis::get().log("Load-settings", CLIENT, OK);

    pqxx::connection sqlConn("postgresql://postgres:postgres@localhost/gridquest");
    pqxx::work fetchProfile(sqlConn);
    std::string nickname("");
    try
    {
        pqxx::result fetchResult = fetchProfile.exec("SELECT * FROM last_profile_to_play()");
        nickname = fetchResult[0][0].as<std::string>();
        Redis::get().log("Load-profile-nickname", CLIENT, OK, "Profile-found");

        std::string errorMessage = GlobalSettings::profileInfo->Change("Nickname = " + nickname);
        Redis::get().log("Change-initial-nickname", CLIENT, errorMessage.length() > 0 ? BAD : OK, errorMessage);
        fetchProfile.commit();
    }
    catch (const pqxx::sql_error &e)
    {
        Redis::get().log("Load-profile-nickname", CLIENT, OK, "No-profile-found");
        fetchProfile.abort();
        pqxx::work addUserWork(sqlConn);
        addNewUser(nickname, &addUserWork);
        addUserWork.commit();
    }
    catch (const std::exception &e)
    {
        Redis::get().log("Load-profile-nickname", CLIENT, OK, "No-profile-found");
        fetchProfile.abort();
        pqxx::work addUserWork(sqlConn);
        addNewUser(nickname, &addUserWork);
        addUserWork.commit();
    }

    GlobalSettings::loadProfile();

    std::cout << "\u001b[" + BG_COL + "m";

    Context game;
    game.transitionTo(new Opening);

    // Need this call to display special characters on console, e.g. colors.
    system("chcp 65001");

    while (true)
    {
        game.show();
        Redis::get().log("Show-game-scene", CLIENT, OK);

        char input = _getch();
        Redis::get().log("Receive-player-input", CLIENT, OK);

        game.processInput(input);
    }

    // We trim the stream at the end of the code to ensure that the stream is
    // trimmed at a point that won't affect the Monitors.
    StreamParser::runMonitors({}, 0);
}