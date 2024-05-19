#include "settings.h"
#include "monitors.h"
#include <fstream>
#include <istream>
#include <iostream>
#include <conio.h>
#include <regex>
#include <sstream>

KeyBind::KeyBind(std::string name, char key)
{
    name_ = name;
    value_ = key;
}

std::string KeyBind::Change(std::string key)
{
    std::string nameNoSpace = std::regex_replace(name_, std::regex(" "), "-");
    std::string keyNoSpace = std::regex_replace(key, std::regex(" "), "-");
    Redis::get() << "key-bind-change " << nameNoSpace << " "
                 << "new-value " << keyNoSpace << " ";

    if (key.size() != 1)
    {
        std::string error("New-value-is-not-a-character");
        Redis::get() << "error " << error;
        Redis::get().push();
        return "E: " + error;
    }

    Setting *topParent = parent_;

    if (topParent == nullptr)
    {
        value_ = key[0];
        Redis::get().push();
        return "";
    }

    while (topParent->GetParent() != nullptr)
        topParent = topParent->GetParent();

    for (auto c : topParent->GetChildrenList())
    {
        if (c == this)
            continue;
        ///@todo ADD CHECK THAT c IS A KEYBINDING
        if (c->getValue() == key)
        {
            std::string error("Key-already-in-use");
            // error.append(c->getName());
            Redis::get() << "error " << error;
            Redis::get().push();
            return "E: ";
        }
    }

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
    return name_ + ": " + value_;
}

Decoration::Decoration(std::string name, std::string code)
{
    name_ = name;
    value_ = code;
}

std::string Decoration::Change(std::string newValue)
{
    std::string nameNoSpace = std::regex_replace(name_, std::regex(" "), "-");
    std::string valueNoSpace = std::regex_replace(newValue, std::regex(" "), "-");

    Redis::get() << "decoration-change " << nameNoSpace << " "
                 << "new-value " << valueNoSpace << " ";

    if (newValue.size() < 1)
    {
        std::string error("Empty-value");
        Redis::get() << "error " << error;
        Redis::get().push();
        return "E: " + error;
    }

    std::stringstream ss(newValue);
    std::string token;
    std::string adjustedCode = "";

    while (getline(ss, token, ';'))
    {
        if (token.size() == 0)
        {
            std::string error("Two-semicolons-attached");
            Redis::get() << "error " << error;
            Redis::get().push();
            return "E: " + error;
        }
        try
        {
            int code = std::stoi(token);
            if (code < 0 || code > 255)
            {
                std::string error("Invalid-code-in-the-sequence");
                Redis::get() << "error " << error;
                Redis::get().push();
                return "E: " + error;
            }
            adjustedCode += std::to_string(code) + ";";
        }
        catch (const std::invalid_argument &_)
        {
            std::string error("Non-digit-characters");
            Redis::get() << "error " << error;
            Redis::get().push();
            return "E: " + error;
        }
    }

    adjustedCode.pop_back(); // Remove last ; character, added in the while
    value_ = adjustedCode;
    Redis::get().push();
    return "";
}

std::string Decoration::ChangeWithInput()
{
    std::string line;
    std::getline(std::cin, line);
    return Change(line);
}

std::string Decoration::ToString() const
{
    return name_ + ": " + value_ + "m";
}

Category::Category(std::string name)
{
    name_ = name;
}

std::string Category::Change(std::string newOption)
{
    size_t i = newOption.find('=');
    if (i == std::string::npos)
        return "E: newOption does not have an = character!";

    std::string settingName = newOption.substr(0, i);
    std::string value = newOption.substr(i + 1);

    if (settingName.empty())
        return "E: newOption does not contain the setting!";
    if (value.empty())
        return "E: newOption does not contain a value!";

    std::optional<Setting *> setting = findSetting(trim(settingName));

    if (!setting.has_value())
        return "E: this category hasn't the desired setting!";

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
        if (c->IsCategory())
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
    controls->Add(movement);
    controls->Add(operations);
    movement->Add(new KeyBind("Move up", 'w'));
    movement->Add(new KeyBind("Move left", 'a'));
    movement->Add(new KeyBind("Move down", 's'));
    movement->Add(new KeyBind("Move right", 'd'));
    movement->Add(new KeyBind("Rotate left", 'q'));
    movement->Add(new KeyBind("Rotate right", 'e'));
    operations->Add(new KeyBind("Add", '+'));
    operations->Add(new KeyBind("Subtract", '-'));
    operations->Add(new KeyBind("Multiply", '*'));
    operations->Add(new KeyBind("Divide", '/'));
    operations->Add(new KeyBind("Module", '%'));
    operations->Add(new KeyBind("Concat", '|'));
    return controls;
}

Category *DefaultGraphic()
{
    Category *graphic = new Category("Graphic");
    graphic->Add(new Decoration("Selection", "7"));
    graphic->Add(new Decoration("Background", "0"));
    graphic->Add(new Decoration("Primary cell", "4;41;1"));
    graphic->Add(new Decoration("Secondary cell", "31;1"));
    graphic->Add(new Decoration("Even cells", "7"));
    graphic->Add(new Decoration("Odd cells", "0"));
    return graphic;
}

int parseSettings(Category *settings, std::string filePath)
{
    std::string line;
    std::ifstream settingsFile(filePath);
    if (settingsFile.is_open())
    {
        std::string section;
        while (getline(settingsFile, line))
        {
            std::string error = settings->Change(line);

            if (error.length() > 0 && error[0] == 'E')
                throw std::invalid_argument(error);
        }
        settingsFile.close();
    }
    else
        return 1;
    return 0;
}

Category *GlobalSettings::controls;
Category *GlobalSettings::graphic;

void GlobalSettings::load()
{
    GlobalSettings::controls = DefaultControls();
    GlobalSettings::graphic = DefaultGraphic();
    parseSettings(GlobalSettings::controls, "etc/savedSettings/controls.txt");
    parseSettings(GlobalSettings::graphic, "etc/savedSettings/graphic.txt");
}

char GlobalSettings::getKey(std::string keyName)
{
    Setting *movements = GlobalSettings::controls->getChildren()["Movement"];
    Setting *operations = GlobalSettings::controls->getChildren()["Operations"];

    Setting *key = movements->getChildren()[keyName];
    if (!key)
        key = operations->getChildren()[keyName];
    if (!key)
    {
        std::string error = "Received a non-existing key name: " + keyName;
        throw std::invalid_argument(error);
    }
    return key->getValue()[0];
}

std::string GlobalSettings::getDecoration(std::string decName)
{
    Setting *decoration = GlobalSettings::graphic->getChildren()[decName];
    if (!decoration)
    {
        std::string error = "Received a non-existing decoration name: " + decName;
        throw std::invalid_argument(error);
    }
    return decoration->getValue();
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
