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

    GlobalSettings::load();

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

    pqxx::work fetchInfo(sqlConn);
    try
    {
        std::stringstream ss;

        pqxx::result pR = fetchInfo.exec("SELECT * FROM quest_played('" + nickname + "')");
        pqxx::result wR = fetchInfo.exec("SELECT * FROM quest_won('" + nickname + "')");
        int p = pR[0][0].as<int>();
        int w = wR[0][0].as<int>();

        ss << "Quest Played = " << p;
        GlobalSettings::profileInfo->Change(ss.str());
        ss.str("");
        ss << "Quest Won = " << w;
        GlobalSettings::profileInfo->Change(ss.str());
        ss.str("");
        ss << "Quest Lost = " << p - w;
        GlobalSettings::profileInfo->Change(ss.str());
        ss.str("");
        ss << "Win Rate = " << w / p;
        GlobalSettings::profileInfo->Change(ss.str());

        fetchInfo.commit();
    }
    catch (const pqxx::sql_error &e)
    {
        fetchInfo.abort();
        std::cout << "An error occurred!"
                  << "We couldn't fetch your profile information.\n\n"
                  << "Try re-open the app.";
        _getch();
        return 1;
    }
    catch (const std::exception &e)
    {
        fetchInfo.abort();
        std::cout << "An error occurred!"
                  << "We couldn't fetch your profile information.\n\n"
                  << "Try re-open the app.";
        _getch();
        return 1;
    }

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