#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <map>
#include <string>
#include <optional>
#include <list>
#include <iterator>

#define MOVE_UP GlobalSettings::getKey("Move up")
#define MOVE_DOWN GlobalSettings::getKey("Move down")
#define MOVE_LEFT GlobalSettings::getKey("Move left")
#define MOVE_RIGHT GlobalSettings::getKey("Move right")
#define ROTATE_LEFT GlobalSettings::getKey("Rotate left")
#define ROTATE_RIGHT GlobalSettings::getKey("Rotate right")
#define ADD GlobalSettings::getKey("Add")
#define SUB GlobalSettings::getKey("Subtract")
#define MUL GlobalSettings::getKey("Multiply")
#define MOD GlobalSettings::getKey("Module")
#define MRG GlobalSettings::getKey("Concat")
#define DIV GlobalSettings::getKey("Divide")
#define SEL_COL GlobalSettings::getDecoration("Selection")
#define BG_COL GlobalSettings::getDecoration("Background")
#define PCELL_COL GlobalSettings::getDecoration("Primary cell")
#define SCELL_COL GlobalSettings::getDecoration("Secondary cell")
#define ECELLS_COL GlobalSettings::getDecoration("Even cells")
#define OCELLS_COL GlobalSettings::getDecoration("Odd cells")
#define ENTER '\r'
#define ESC 27

class Setting
{
protected:
    Setting *parent_;
    std::string name_;
    std::string value_;

public:
    virtual ~Setting() {}
    void SetParent(Setting *parent)
    {
        this->parent_ = parent;
    }
    Setting *GetParent() const
    {
        return this->parent_;
    }
    std::string getName() const
    {
        return this->name_;
    }
    std::string getValue() const
    {
        return this->value_;
    }
    virtual void Add(Setting *Setting) {}
    virtual void Remove(Setting *Setting) {}
    virtual std::map<std::string, Setting *> getChildren()
    {
        return std::map<std::string, Setting *>();
    }
    /// @brief Get all this setting children as a single list. If the setting is
    /// composed of multiple nested categories, every inner setting is grouped
    /// into a single list.
    /// @return
    virtual std::list<Setting *> GetChildrenList()
    {
        std::list<Setting *> l = std::list<Setting *>();
        l.push_back(this);
        return l;
    }
    virtual bool IsCategory() const
    {
        return false;
    }
    virtual std::optional<Setting *> findSetting(std::string settingName)
    {
        return std::optional<Setting *>{};
    }
    /// @brief Change the current setting (e.g., the key for a KeyBind).
    /// @param newOption A string describing the new Setting option.
    /// @return An empty string if the change was performed without problems. If
    /// the change wasn't possible (e.g., invalid key for a KeyBind), then the
    /// return string is not empty and it contains an error message. If the
    /// change was possible but with some interferences (e.g., the new key for
    /// a KeyBind replaced another KeyBind), then the return string is not empty
    /// and it contains a warning message.
    virtual std::string Change(std::string newOption) = 0;
    /// @brief Change the current setting by reading the user input. Each
    /// setting defines its own way of reading the input (e.g. KeyBind a single
    /// char, Decoration an entire string).
    /// @return A string describing the result of the operation, as in  
    /// @ref Change function.
    virtual std::string ChangeWithInput() = 0;
    virtual std::string ToString() const = 0;
};

class Category : public Setting
{
protected:
    std::map<std::string, Setting *> children_;

public:
    Category(std::string name);

    /// @brief Add a new setting in this category (key binding, decoration,
    /// another category, ...).
    /// @param component The setting to add.
    void Add(Setting *component) override
    {
        children_[component->getName()] = component;
        component->SetParent(this);
    }
    /// @brief Removes a setting from this category. The setting is removed from
    /// the top level of this category. In other words, if the setting is a
    /// child of a category that is a child of this, the setting won't be
    /// removed.
    /// @param component The setting to remove.
    void Remove(Setting *component) override
    {
        children_.erase(component->getName());
        component->SetParent(nullptr);
    }
    /// @brief A getter for this category children (i.e., settings).
    /// @return This category children.
    std::map<std::string, Setting *> getChildren() override
    {
        return children_;
    }
    std::list<Setting *> GetChildrenList() override
    {
        std::list<Setting *> childrenList = std::list<Setting *>();
        for (auto c : getChildren())
            childrenList.splice(childrenList.end(), c.second->GetChildrenList());

        return childrenList;
    }
    bool IsCategory() const override
    {
        return true;
    }
    std::string Change(std::string newOption) override;
    std::string ChangeWithInput() override;
    std::string ToString() const override;
    std::optional<Setting *> findSetting(std::string settingName) override;
};

class KeyBind : public Setting
{
public:
    KeyBind(std::string name, char key);
    std::string Change(std::string) override;
    std::string ChangeWithInput() override;
    std::string ToString() const override;
};

class Decoration : public Setting
{
public:
    Decoration(std::string name, std::string code);
    std::string Change(std::string) override;
    std::string ChangeWithInput() override;
    std::string ToString() const override;
};

Category *DefaultControls();

Category *DefaultGraphic();

int parseSettings(Category *settings, std::string filePath);

class GlobalSettings
{
public:
    static Category *controls;
    static Category *graphic;
    /// @brief Return the value of the KeyBind named `keyName`. If a KeyBind
    // with that name does not exist, an 'invalid_argument' exception is raised.
    /// @param keyName The name associated with the wanted KeyBind.
    /// @return The character binded to the key.
    static char getKey(std::string keyName);
    static std::string getDecoration(std::string decName);
    static void load();
};

std::string &ltrim(std::string &str, std::string const &whitespace = " \r\n\t\v\f");

std::string &rtrim(std::string &str, std::string const &whitespace = " \r\n\t\v\f");

std::string &trim(std::string &str, std::string const &whitespace = " \r\n\t\v\f");

#endif