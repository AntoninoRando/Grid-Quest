#include "settings.h"
#include <gtest/gtest.h>

TEST(KeyBindChange, LettersAndNumbers)
{
    KeyBind key("test", 'a');
    for (auto c : "abcdefghijklmnopqrstuvwxyz0123456789")
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