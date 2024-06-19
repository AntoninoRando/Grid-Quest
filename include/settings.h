#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <map>
#include <string>
#include <optional>
#include <list>
#include <iterator>

/**
 * @brief The key to move up the cursor on the grid.
 */
#define MOVE_UP GlobalSettings::getKey("Move up")
/**
 * @brief The key to move down the cursor on the grid.
 */
#define MOVE_DOWN GlobalSettings::getKey("Move down")
/**
 * @brief The key to move left the cursor on the grid.
 */
#define MOVE_LEFT GlobalSettings::getKey("Move left")
/**
 * @brief The key to move right the cursor on the grid.
 */
#define MOVE_RIGHT GlobalSettings::getKey("Move right")
/**
 * @brief The key to rotate counterclockwise the cursor on the grid.
 */
#define ROTATE_LEFT GlobalSettings::getKey("Rotate left")
/**
 * @brief The key to rotate clockwise the cursor on the grid.
 */
#define ROTATE_RIGHT GlobalSettings::getKey("Rotate right")
/**
 * @brief The key to add the two numbers in the grid cursor.
 */
#define ADD GlobalSettings::getKey("Add")
/**
 * @brief The key to subtract the two numbers in the grid cursor.
 */
#define SUB GlobalSettings::getKey("Subtract")
/**
 * @brief The key to multiple the two numbers in the grid cursor.
 */
#define MUL GlobalSettings::getKey("Multiply")
/**
 * @brief The key to perform the module between the two numbers in the grid
 * cursor.
 */
#define MOD GlobalSettings::getKey("Module")
/**
 * @brief The key to concatenate the two numbers in the grid cursor.
 */
#define MRG GlobalSettings::getKey("Concat")
/**
 * @brief The key to divide the two numbers in the grid cursor.
 */
#define DIV GlobalSettings::getKey("Divide")
/**
 * @brief The decoration for selected menu options.
 */
#define SEL_COL GlobalSettings::getDecoration("Selection")
/**
 * @brief The decoration for the background.
 */
#define BG_COL GlobalSettings::getDecoration("Background")
/**
 * @brief The decoration for the primary cell of the grid cursor.
 */
#define PCELL_COL GlobalSettings::getDecoration("Primary cell")
/**
 * @brief The decoration for the secondary cell of the grid cursor.
 */
#define SCELL_COL GlobalSettings::getDecoration("Secondary cell")
/**
 * @brief The decoration for the grid cells that are in an event position.
 */
#define ECELLS_COL GlobalSettings::getDecoration("Even cells")
/**
 * @brief The decoration for the grid cells that are in an odd index.
 */
#define OCELLS_COL GlobalSettings::getDecoration("Odd cells")

#define ENTER '\r'
#define ESC 27

enum SettingType
{
    KEYBIND,
    DECORATION,
    CATEGORY,
    PROFILEINFO
};

/**
 * @brief The abstract class representing any kind of setting, even a group of
 * settings.
 *
 * Every settings is a name associated with a value. The value represent as a
 * string the configuration of this setting.
 */
class Setting
{
protected:
    Setting *parent_ = nullptr;
    std::string name_;
    std::string value_;

public:
    virtual ~Setting() {}
    void SetParent(Setting *parent) { parent_ = parent; }
    Setting *GetParent() const { return parent_; }
    std::string GetName() const { return name_; }
    std::string name() const { return name_; }
    std::string GetValue() const { return value_; }
    std::string value() const { return value_; }
    virtual std::map<std::string, Setting *> GetChildren()
    {
        return std::map<std::string, Setting *>();
    }

    /**
     * @brief Add another setting as a child of this setting.
     *
     * @param Setting The setting to add as a child.
     */
    virtual void add(Setting *Setting) {}

    /**
     * @brief Try to remove a setting from this setting's childrens. If the
     * setting to remove is not a child of this setting, a call to this function
     * will have no effect.
     *
     * @param Setting The child setting to remove.
     */
    virtual void remove(Setting *Setting) {}

    /**
     * @brief Get all this setting children as a single list. If the setting is
     * composed of multiple nested categories, every inner setting is grouped
     * into a single list.
     *
     * @return
     */
    virtual std::list<Setting *> GetChildrenList()
    {
        std::list<Setting *> l = std::list<Setting *>();
        l.push_back(this);
        return l;
    }

    /**
     * @brief Get the type of this setting as a SettingType value.
     *
     * @return SettingType The type of this setting.
     */
    virtual SettingType type() const = 0;

    /**
     * @brief Search a setting called `settingName` in this setting's childrens
     * and return an optional with it, if found, or an empty optional otherwise.
     *
     * @param settingName The name of the setting to search.
     * @return std::optional<Setting *> An optional with the setting, it it was
     * found; an empty optional otherwise.
     */
    virtual std::optional<Setting *> findSetting(std::string settingName)
    {
        return std::optional<Setting *>{};
    }

    /**
     * @brief Check if `newValue` is a valid new value for this setting.
     *
     * @param newValue The new value to validate.
     * @return std::string An error message if the new value is invalid, an
     * empty string otherwise.
     */
    virtual std::string validate(std::string newValue) { return ""; }

    /**
     * @brief Change the current setting (e.g., the key for a KeyBind).
     *
     * @param newOption A string describing the new Setting option.
     * @return An empty string if the change was performed without problems. If
     * the change wasn't possible (e.g., invalid key for a KeyBind), then the
     * return string is not empty and it contains an error message. If the
     * change was possible but with some interferences (e.g., the new key for
     * a KeyBind replaced another KeyBind), then the return string is not empty
     * and it contains a warning message.
     */
    virtual std::string Change(std::string newOption) = 0;

    /**
     * @brief Change the current setting by reading the user input. Each
     * setting defines its own way of reading the input (e.g. KeyBind a single
     * char, Decoration an entire string).
     *
     * @return A string describing the result of the operation, as in
     */
    virtual std::string ChangeWithInput() = 0;

    /**
     * @brief Return a pretty string that can be printed to show this setting,
     * with both its name and value.
     *
     * @return std::string A string describing this setting.
     */
    virtual std::string ToString() const = 0;
};

/**
 * @brief A group of settings.
 */
class Category : public Setting
{
protected:
    std::map<std::string, Setting *> children_;

public:
    Category(std::string name);

    std::map<std::string, Setting *> GetChildren() override { return children_; }

    std::list<Setting *> GetChildrenList() override
    {
        std::list<Setting *> childrenList = std::list<Setting *>();
        for (auto c : GetChildren())
            childrenList.splice(childrenList.end(), c.second->GetChildrenList());

        return childrenList;
    }

    void add(Setting *component) override
    {
        children_[component->GetName()] = component;
        component->SetParent(this);
    }

    void remove(Setting *component) override
    {
        children_.erase(component->GetName());
        component->SetParent(nullptr);
    }

    SettingType type() const override { return CATEGORY; }

    std::string Change(std::string newOption) override;
    std::string ChangeWithInput() override;
    std::string ToString() const override;
    std::optional<Setting *> findSetting(std::string settingName) override;
};

/**
 * @brief An input mapping.
 */
class KeyBind : public Setting
{
public:
    KeyBind(std::string name, char key);
    std::string validate(std::string) override;
    std::string Change(std::string) override;
    std::string ChangeWithInput() override;
    std::string ToString() const override;
    SettingType type() const override { return KEYBIND; }
};

/**
 * @brief A style of some object of the game.
 */
class Decoration : public Setting
{
public:
    Decoration(std::string name, std::string code);
    std::string validate(std::string) override;
    std::string Change(std::string) override;
    std::string ChangeWithInput() override;
    std::string ToString() const override;
    SettingType type() const override { return DECORATION; }
};

/**
 * @brief An information about the user profile (e.g., name, score, etc.).
 */
class ProfileInfo : public Setting
{
public:
    ProfileInfo(std::string name, std::string value);
    std::string validate(std::string) override;
    std::string Change(std::string) override;
    std::string ChangeWithInput() override;
    std::string ToString() const override;
    SettingType type() const override { return PROFILEINFO; }
};

/**
 * @brief Create a category containing all controls settings and configure them
 * with default values.
 *
 * @return Category* The category containing the default controls settings.
 */
Category *DefaultControls();

/**
 * @brief Create a category containing all graphic settings and configure them
 * with default values.
 *
 * @return Category* The category containing the default graphic settings.
 */
Category *DefaultGraphic();

/**
 * @brief Create a category containing all profile settings and configure them
 * with default values.
 *
 * @return Category* The category containing the default profile settings.
 */
Category *DefaultProfile();

/**
 * @brief Parse the file at `filePath` and fill the `settings` category with the
 * parsed settings.
 *
 * @param settings The category of settings where the configuration is stored.
 * @param filePath The path to the file containing the settings.
 * @return int 1 if an error occurred, 0 otherwise.
 */
int parseSettings(Category *settings, std::string filePath);

/**
 * @brief The current settings configuration.
 */
class GlobalSettings
{
public:
    /**
     * @brief The category that groups input mapping settings.
     */
    static Category *controls;

    /**
     * @brief The category that groups decoration settings.
     */
    static Category *graphic;

    /**
     * @brief The category that groups profile info.
     */
    static Category *profileInfo;

    /**
     * @brief Return the value of the KeyBind named `keyName`. If a KeyBind
     * with that name does not exist, an 'invalid_argument' exception is raised.
     *
     * @param keyName The name associated with the wanted KeyBind.
     * @return The character binded to the key.
     */
    static char getKey(std::string keyName);

    /**
     * @brief Return the value of the Decoration named `decName`. If a
     * Decoration with that name does not exist, an 'invalid_argument' exception
     * is raised.
     *
     * @param decName The name associated with the wanted Decoration.
     * @return A terminal color code that is the value of the `decName` setting.
     */
    static std::string getDecoration(std::string decName);

    /**
     * @brief Read and parse the file containing the saved user setting in order
     * to configure the game settings.
     */
    static void load();
};

/**
 * @brief Remove every white-space at the start of a string.
 *
 * @param str The string from which the white-spaces are removed.
 * @param whitespace The white-space characters.
 * @return std::string& The string without starting white-spaces.
 */
std::string &ltrim(std::string &str, std::string const &whitespace = " \r\n\t\v\f");

/**
 * @brief Remove every white-space at the end of a string.
 *
 * @param str The string from which the white-spaces are removed.
 * @param whitespace The white-space characters.
 * @return std::string& The string without ending white-spaces.
 */
std::string &rtrim(std::string &str, std::string const &whitespace = " \r\n\t\v\f");

/**
 * @brief Remove every white-space at the start and end of a string.
 *
 * @param str The string from which the white-spaces are removed.
 * @param whitespace The white-space characters.
 * @return std::string& The string without starting and ending white-spaces.
 */
std::string &trim(std::string &str, std::string const &whitespace = " \r\n\t\v\f");

#endif