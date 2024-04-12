#include "settings.h"
#include <iostream>

KeyBind::KeyBind(std::string name, char key)
{
    this->name_ = name;
    this->value_ = key;
}

std::string KeyBind::Change(std::string newKey)
{
    if (newKey.size() != 1)
    {
        return "E: The new value for " + name_ + " is not a character!";
    }

    value_ = newKey[0];
    return "";
}

std::string KeyBind::ToString() const
{
    return name_ + ": " + value_;
}

Decoration::Decoration(std::string name, std::string code)
{
    this->name_ = name;
    this->value_ = code;
}

std::string Decoration::Change(std::string newCode)
{
    value_ = newCode;
    return "";
}

std::string Decoration::ToString() const
{
    return name_ + ": " + value_ + "m";
}

Category::Category(std::string name)
{
    this->name_ = name;
}

std::string Category::Change(std::string newOption)
{
    size_t i = newOption.find('=');
    if (i == std::string::npos)
    {
        return "E: newOption does not have an = character!";
    }

    std::string settingName = newOption.substr(0, i);
    std::string value = newOption.substr(i + 1);

    if (settingName.empty())
    {
        return "E: newOption does not contain the setting!";
    }
    if (value.empty())
    {
        return "E: newOption does not contain a value!";
    }

    std::optional<Setting *> setting = findSetting(trim(settingName));

    if (!setting.has_value())
    {
        return "E: this category hasn't the desired setting!";
    }

    std::string changeResult = setting.value()->Change(ltrim(value));
    return changeResult;
}

std::string Category::ToString() const
{
    std::string result = name_ + "\n\n";
    for (const auto &[_, c] : children_)
    {
        result += c->ToString() + "\n";
    }
    return result;
}

std::optional<Setting *> Category::findSetting(std::string settingName)
{
    if (children_.count(settingName))
    {
        return std::optional<Setting *>{children_[settingName]};
    }

    for (auto &[_, c] : children_)
    {
        if (c->IsCategory())
        {
            std::optional<Setting *> result = c->findSetting(settingName);
            if (result.has_value())
            {
                return result;
            }
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
    return graphic;
}

char GlobalSettings::getKey(std::string keyName)
{
    Setting *controls = getInstance()->getChildren()["Controls"];
    Setting *movements = controls->getChildren()["Movement"];
    Setting *operations = controls->getChildren()["Operations"];

    Setting* key = movements->getChildren()[keyName];
    if (!key)
    {
        key = operations->getChildren()[keyName];
    }
    if (!key)
    {
        return '<';
    }
    return key->getValue()[0];
};

int parseSettings(Category *settings, std::string filePath)
{
    std::string line;
    std::ifstream settingsFile(filePath);
    if (settingsFile.is_open())
    {
        std::string section;
        while (getline(settingsFile, line))
        {
            settings->Change(line);
        }
        settingsFile.close();
    }
    else
    {
        return 1;
    }
    return 0;
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

std::map<std::string, Setting *> Setting::getChildren()
{
    return std::map<std::string, Setting *>();
}
