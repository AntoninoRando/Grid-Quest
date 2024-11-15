#include "settings.h"
#include "monitors.h"
#include <fstream>
#include <istream>
#include <iostream>
#include <conio.h>
#include <regex>
#include <sstream>
#include <pqxx/pqxx>

KeyBind::KeyBind(std::string name, char key)
{
    name_ = name;
    value_ = key;
}

std::string KeyBind::validate(std::string newValue)
{
    bool isSpecial = false;
    for (auto s : {"ESC", "DEL", "SPACE", "ENTER"})
    {
        if (s == newValue)
        {
            isSpecial = true;
            break;
        }
    }

    if (newValue.size() != 1 && !isSpecial)
        return "New-value-is-not-a-character";

    Setting *topParent = parent_;

    if (topParent == nullptr)
        return "";

    while (topParent->GetParent() != nullptr)
        topParent = topParent->GetParent();

    for (auto c : topParent->GetChildrenList())
    {
        if (c == this || c->type() != KEYBIND)
            continue;
        if (c->GetValue() == newValue)
            return "Key-already-in-use";
    }

    return "";
}

std::string KeyBind::Change(std::string key)
{
    std::string nameNoSpace = std::regex_replace(name_, std::regex(" "), "-");
    std::string keyNoSpace = std::regex_replace(key, std::regex(" "), "-");
    Redis::get() << "key-bind-change " << nameNoSpace << " "
                 << "new-value " << keyNoSpace << " ";

    std::string error = validate(key);
    if (error.length() > 0)
    {
        Redis::get() << "error " << error;
        Redis::get().push();
        return error;
    }

    if (key == "ESC")
        value_ = ESC;
    else if (key == "DEL")
        value_ = DEL;
    else if (key == "SPACE")
        value_ = SPACE;
    else if (key == "ENTER")
        value_ = ENTER;
    else
        value_ = key[0];
    
    Redis::get().push();
    return "";
}

std::string KeyBind::ChangeWithInput()
{
    char newKey = _getch();
    return Change(std::string(1, newKey));
}

std::string KeyBind::ToString() const
{ 
    std::string repr(name_);
    repr.append(": ");

    if (value_[0] == ESC) // "" + char converts char to string for comparison
        return repr + "ESC";
    if (value_[0] == DEL)
        return repr + "DEL";
    if (value_[0] == SPACE)
        return repr + "SPACE";
    if (value_[0] == ENTER)
        return repr + "ENTER";
    return name_ + ": " + value_; 
}

Decoration::Decoration(std::string name, std::string code)
{
    name_ = name;
    value_ = code;
}

std::string Decoration::validate(std::string newValue)
{
    if (newValue.size() < 1)
        return "Empty-value";

    std::stringstream ss(newValue);
    std::string token;
    std::string adjustedCode = "";

    while (getline(ss, token, ';'))
    {
        if (token.size() == 0)
            return "Two-semicolons-attached";

        try
        {
            int code = std::stoi(token);
            if (code < 0 || code > 255)
                return "Invalid-code-in-the-sequence";
            adjustedCode += std::to_string(code) + ";";
        }
        catch (const std::invalid_argument &_)
        {
            return "Non-digit-characters";
        }
        catch (const std::out_of_range &_)
        {
            return "Invalid-code-in-the-sequence";
        }
    }

    adjustedCode.pop_back(); // Remove last ; character, added in the while
    return "";
}

std::string Decoration::Change(std::string newValue)
{
    std::string nameNoSpace = std::regex_replace(name_, std::regex(" "), "-");
    std::string valueNoSpace = std::regex_replace(newValue, std::regex(" "), "-");

    Redis::get() << "decoration-change " << nameNoSpace << " "
                 << "new-value " << valueNoSpace << " ";

    std::string error = validate(newValue);
    if (error.length() > 0)
    {
        Redis::get() << "error " << error;
        Redis::get().push();
        return error;
    }

    value_ = newValue;
    Redis::get().push();
    return "";
}

std::string Decoration::ChangeWithInput()
{
    std::string line;
    std::getline(std::cin, line);
    return Change(line);
}

std::string Decoration::ToString() const { return name_ + ": " + value_ + "m"; }

ProfileInfo::ProfileInfo(std::string name, std::string value)
{
    name_ = name;
    value_ = value;
}

std::string ProfileInfo::validate(std::string newValue)
{
    if (name_ != "Nickname")
        return "";

    int newValueSize = newValue.size();
    if (newValueSize == 0 || newValueSize > 100)
        return "Nickname-must-be-between-1-and-100-characters-long";

    for (auto c : newValue)
    {
        if (!std::isalnum(c) && c != '_')
            return "Nickname-must-contain-only-alphanumeric-characters";
    }

    return "";
}

std::string ProfileInfo::Change(std::string newValue)
{
    if (name_ != "Nickname")
    {
        value_ = newValue;
        return "";
    }

    std::string error = validate(newValue);
    if (error.length() > 0)
        return error;

    Redis::get() << "use-nickname " << newValue;
    Redis::get().push();
    value_ = newValue;
    return "";
}

std::string ProfileInfo::ChangeWithInput() { return "Cannot-modify"; }

std::string ProfileInfo::ToString() const { return name_ + ": " + value_; }

std::string Button::ChangeWithInput()
{
    std::cout << "Confirm? (y/n) ";
    char input = _getch();
    if (input == 'y')
        return Change(value_);
    return "";
}

std::string DeleteProfile::Change(std::string _)
{
    pqxx::connection sqlConn("postgresql://postgres:postgres@localhost/gridquest");
    pqxx::work delProfile(sqlConn);
    std::string profile = CURRENT_PROFILE;

    try
    {
        pqxx::result res = delProfile.exec("SELECT delete_user('" + profile + "')");
        delProfile.commit();

        if (res[0][0].as<bool>()) // Profile successfully deleted
            exit(0);              // @todo: Implement restart instead of exit

        return "";
    }
    catch (const pqxx::sql_error &e)
    {
        delProfile.abort();
    }
    catch (const std::exception &e)
    {
        delProfile.abort();
    }

    return "Could-not-delete-profile";
}

std::string Button::ToString() const { return name_; }

std::string Category::Change(std::string newOption)
{
    size_t i = newOption.find('=');
    if (i == std::string::npos)
        return "newOption-does-not-have-a-=-character";

    std::string settingName = newOption.substr(0, i);
    std::string value = newOption.substr(i + 1);

    if (settingName.empty())
        return "newOption-does-not-contain-the-setting";
    if (value.empty())
        return "newOption-does-not-contain-a-value";

    std::optional<Setting *> setting = findSetting(trim(settingName));

    if (!setting.has_value())
        return "this-category-has-not-the-desired-setting";

    std::string changeResult = setting.value()->Change(ltrim(value));
    return changeResult;
}

std::string Category::ChangeWithInput()
{
    std::string line;
    std::getline(std::cin, line);
    return Change(line);
}

std::string Category::ToString() const
{
    std::string result = name_ + "\n\n";
    for (const auto &[_, c] : children_)
        result += c->ToString() + "\n";
    return result;
}

std::optional<Setting *> Category::findSetting(std::string settingName)
{
    if (children_.count(settingName))
        return std::optional<Setting *>{children_[settingName]};

    for (auto &[_, c] : children_)
    {
        if (c->type() == CATEGORY)
        {
            std::optional<Setting *> result = c->findSetting(settingName);
            if (result.has_value())
                return result;
        }
    }
    return std::optional<Setting *>{};
}

Category *DefaultControls()
{
    Category *controls = new Category("Controls");
    Category *movement = new Category("Movement");
    Category *operations = new Category("Operations");
    Category *others = new Category("Others");

    controls->add(movement);
    controls->add(operations);
    controls->add(others);

    movement->add(new KeyBind("Move-up", 'w'));
    movement->add(new KeyBind("Move-left", 'a'));
    movement->add(new KeyBind("Move-down", 's'));
    movement->add(new KeyBind("Move-right", 'd'));
    movement->add(new KeyBind("Rotate-left", 'q'));
    movement->add(new KeyBind("Rotate-right", 'e'));

    operations->add(new KeyBind("Add", '+'));
    operations->add(new KeyBind("Subtract", '-'));
    operations->add(new KeyBind("Multiply", '*'));
    operations->add(new KeyBind("Divide", '/'));
    operations->add(new KeyBind("Module", '%'));
    operations->add(new KeyBind("Concat", '|'));

    operations->add(new KeyBind("Quit-quest", ESC));
    operations->add(new KeyBind("Drop-quest", DEL));
    return controls;
}

Category *DefaultGraphic()
{
    Category *graphic = new Category("Graphic");
    graphic->add(new Decoration("Selection", "7"));
    graphic->add(new Decoration("Background", "0"));
    graphic->add(new Decoration("Primary-cell", "4;41;1"));
    graphic->add(new Decoration("Secondary-cell", "31;1"));
    graphic->add(new Decoration("Even-cells", "7"));
    graphic->add(new Decoration("Odd-cells", "0"));
    return graphic;
}

Category *DefaultProfile()
{
    Category *pinfo = new Category("Profile");
    pinfo->add(new ProfileInfo("Nickname", "Main"));
    pinfo->add(new ProfileInfo("Quest Played", "0"));
    pinfo->add(new ProfileInfo("Quest Won", "0"));
    pinfo->add(new ProfileInfo("Quest Lost", "0"));
    pinfo->add(new ProfileInfo("Win Rate", "0%"));
    pinfo->add(new DeleteProfile("Delete Profile"));
    return pinfo;
}

std::string parseSettings(Category *settings, std::string filePath)
{
    std::string line;
    std::ifstream settingsFile(filePath);
    if (!settingsFile.is_open())
        return "Cannot-open-file";

    std::string section;
    while (getline(settingsFile, line))
    {
        std::string error = settings->Change(line);
        if (error.length() > 0)
            return error; // throw std::invalid_argument(error);
    }
    settingsFile.close();
    return "";
}

Category *GlobalSettings::controls;
std::map<char, std::string> *GlobalSettings::actionsOfKey;
Category *GlobalSettings::graphic;
Category *GlobalSettings::profileInfo;

std::string GlobalSettings::load()
{
    // Use default settings
    GlobalSettings::controls = DefaultControls();
    GlobalSettings::graphic = DefaultGraphic();
    GlobalSettings::profileInfo = DefaultProfile();
    GlobalSettings::actionsOfKey = new std::map<char, std::string>();

    // Replace with saved settings
    std::string error = parseSettings(GlobalSettings::controls, "../etc/savedSettings/controls.txt");
    if (error.length() > 0)
        return error;
    error = parseSettings(GlobalSettings::graphic, "../etc/savedSettings/graphic.txt");
    if (error.length() > 0)
        return error;

    // Sets the actionsOfKey map
    Setting *movements = GlobalSettings::controls->GetChildren()["Movement"];
    Setting *operations = GlobalSettings::controls->GetChildren()["Operations"];
    for (auto const &[action, setting] : movements->GetChildren())
    {
        char key = setting->GetValue()[0];
        if (GlobalSettings::actionsOfKey->find(key) != GlobalSettings::actionsOfKey->end())
            return "same-key-used-more-than-once";
        GlobalSettings::actionsOfKey->insert({key, action});
    }
    for (auto const &[action, setting] : operations->GetChildren())
    {
        char key = setting->GetValue()[0];
        if (GlobalSettings::actionsOfKey->find(key) != GlobalSettings::actionsOfKey->end())
            return "same-key-used-more-than-once";
        GlobalSettings::actionsOfKey->insert({key, action});
    }
    return "";
}

int GlobalSettings::loadProfile()
{
    pqxx::connection sqlConn("postgresql://postgres:postgres@localhost/gridquest");
    pqxx::work fetchInfo(sqlConn);

    std::string nickname = CURRENT_PROFILE;

    try
    {
        std::stringstream ss;

        pqxx::result pR = fetchInfo.exec("SELECT * FROM quest_played('" + nickname + "')");
        pqxx::result wR = fetchInfo.exec("SELECT * FROM quest_won('" + nickname + "')");
        int p = pR[0][0].as<int>();
        int w = wR[0][0].as<int>();

        Redis::get().log("Fetch-profile-info", CLIENT, OK);

        ss << "Quest Played = " << p;
        GlobalSettings::profileInfo->Change(ss.str());
        ss.str("");
        ss << "Quest Won = " << w;
        GlobalSettings::profileInfo->Change(ss.str());
        ss.str("");
        ss << "Quest Lost = " << p - w;
        GlobalSettings::profileInfo->Change(ss.str());
        ss.str("");
        ss << "Win Rate = " << (int)(w * 100 / p) << "%";
        GlobalSettings::profileInfo->Change(ss.str());

        fetchInfo.commit();
    }
    catch (const pqxx::sql_error &e)
    {
        fetchInfo.abort();
        Redis::get().log("Fetch-profile-info", CLIENT, BAD);
        std::cout << "An error occurred!"
                  << "We couldn't fetch your profile information.\n\n"
                  << "Try re-open the app.";
        _getch();
        return 1;
    }
    catch (const std::exception &e)
    {
        fetchInfo.abort();
        Redis::get().log("Fetch-profile-info", CLIENT, BAD);
        std::cout << "An error occurred!"
                  << "We couldn't fetch your profile information.\n\n"
                  << "Try re-open the app.";
        _getch();
        return 1;
    }

    return 0;
}

char GlobalSettings::getKey(std::string keyName)
{
    Setting *movements = GlobalSettings::controls->GetChildren()["Movement"];
    Setting *operations = GlobalSettings::controls->GetChildren()["Operations"];

    Setting *key = movements->GetChildren()[keyName];
    if (!key)
        key = operations->GetChildren()[keyName];
    if (!key)
    {
        std::string error = "Received-a-non-existing-key-name:" + keyName;
        throw std::invalid_argument(error);
    }
    return key->GetValue()[0];
}

std::string GlobalSettings::getActionOfKey(char key)
{
    if (GlobalSettings::actionsOfKey->find(key) == GlobalSettings::actionsOfKey->end())
        return "nothing";
    return (*GlobalSettings::actionsOfKey)[key];
}

std::string GlobalSettings::getDecoration(std::string decName)
{
    Setting *decoration = GlobalSettings::graphic->GetChildren()[decName];
    if (!decoration)
    {
        std::string error = "Received-a-non-existing-decoration-name:" + decName;
        throw std::invalid_argument(error);
    }
    return decoration->GetValue();
}

std::string &ltrim(std::string &str, std::string const &whitespace)
{
    str.erase(0, str.find_first_not_of(whitespace));
    return str;
}

std::string &rtrim(std::string &str, std::string const &whitespace)
{
    str.erase(str.find_last_not_of(whitespace) + 1);
    return str;
}

std::string &trim(std::string &str, std::string const &whitespace)
{
    return ltrim(rtrim(str, whitespace), whitespace);
}