#include "settings.h"
#include <gtest/gtest.h>

TEST(KeyBindChange, LettersAndNumbers)
{
    KeyBind key("test", 'a');
    for (auto c : "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789")
    {
        EXPECT_EQ("", key.validate(std::string(1, c)));
    }
}

TEST(KeyBindChange, MultipleLengthKeys)
{
    KeyBind key("test", 'a');
    for (auto c : {"ab", "abc", "abcd", "abcde", "abcdef", "abcdefg"})
    {
        EXPECT_EQ("New-value-is-not-a-character", key.validate(c));
    }
}

TEST(KeyBindChange, EmptyKey)
{
    KeyBind key("test", 'a');
    EXPECT_EQ("New-value-is-not-a-character", key.validate(""));
}

TEST(KeyBindChange, KeyAlreadyBinded)
{
    KeyBind sourceKey("source", 'a');
    KeyBind targetKey("target", 'b');
    Category cat("test-category");

    cat.add(&sourceKey);
    EXPECT_EQ("", sourceKey.validate(targetKey.value()));

    cat.add(&targetKey);
    EXPECT_EQ("Key-already-in-use", sourceKey.validate(targetKey.value()));
    EXPECT_EQ("Key-already-in-use", targetKey.validate(sourceKey.value()));

    cat.remove(&targetKey);
    EXPECT_EQ("", sourceKey.validate(targetKey.value()));
}

TEST(DecorationChange, EmptyCode)
{
    Decoration dec("test", "1");
    EXPECT_EQ("Empty-value", dec.validate(""));
}

TEST(DecorationChange, Letters)
{
    Decoration dec("test", "1");
    for (auto c : {"ab", "abc", "abcd", "abcde", "abcdef", "abcdefg"})
    {
        EXPECT_EQ("Non-digit-characters", dec.validate(c));
    }
}

TEST(DecorationChange, WrongSemicolons)
{
    Decoration dec("test", "1");
    for (auto c : {";;", ";", "1;;1", ";1", ";;1", ";;;;"})
    {
        EXPECT_EQ("Two-semicolons-attached", dec.validate(c));
    }
}

TEST(DecorationChange, InvalidCodes)
{
    Decoration dec("test", "1");
    for (auto c : {"10;0;299", "-1", "256", "0;1;2;256", "1129829812012370981"})
    {
        EXPECT_EQ("Invalid-code-in-the-sequence", dec.validate(c));
    }
}

TEST(DecorationChange, OKFormats)
{
    Decoration dec("test", "1");
    for (auto c : {"10;0;255", "1;23;1", "255", "0", "12;9;1"})
    {
        EXPECT_EQ("", dec.validate(c));
    }
}

TEST(NicknameChange, EmptyNick)
{
    ProfileInfo pinfo("Nickname", "test");
    EXPECT_EQ("E: Nickname-must-be-between-1-and-100-characters-long", pinfo.validate(""));
}

TEST(NicknameChange, TooLongNick)
{
    ProfileInfo pinfo("Nickname", "test");
    for (int i = 101; i < 200; i++)
    {
        EXPECT_EQ("E: Nickname-must-be-between-1-and-100-characters-long", pinfo.validate(std::string(i, 'a')));
    }
}

TEST(NicknameChange, WhiteSpaces)
{
    ProfileInfo pinfo("Nickname", "test");
    for (auto c : {" ", "  ", "   ", "a ", " a", "a b", "\t", "\n", "\b", "\v", "\r"})
    {
        EXPECT_EQ("E: Nickname-must-contain-only-alphanumeric-characters", pinfo.validate(c));
    }
}

TEST(NicknameChange, UnicodeWhiteSpaces)
{
    // https://stackoverflow.com/questions/18169006/all-the-whitespace-characters-is-it-language-independent
    ProfileInfo pinfo("Nickname", "test");
    for (auto c : {"\u0020", "\u00A0", "\u1680", "\u2000", "\u2001", "\u2002", "\u2003", "\u2004", "\u2005", "\u2006", "\u2007", "\u2008", "\u2009", "\u200A", "\u202F", "\u205F", "\u3000"})
    {
        EXPECT_EQ("E: Nickname-must-contain-only-alphanumeric-characters", pinfo.validate(c));
    }
}

TEST(NicknameChange, OKFormats)
{
    ProfileInfo pinfo("Nickname", "test");
    for (int i = 1; i <= 100; i++)
    {
        for (auto c : "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789")
        {
            if (c == '\0')
                continue;
            EXPECT_EQ("", pinfo.validate(std::string(i, c)));
        }
    }

    EXPECT_EQ("", pinfo.validate("test_123"));
    EXPECT_EQ("", pinfo.validate("test_"));
    EXPECT_EQ("", pinfo.validate("______"));
}