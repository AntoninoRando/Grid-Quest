#include "grid.h"
#include <gtest/gtest.h>

TEST(GridFillAndCount, NegativeFill)
{
    Grid grid;
    for (int i = -1; i > -10; i--)
    {
        grid.fill(i);
        EXPECT_EQ(0, grid.contRemaining());
    }
}

TEST(GridFillAndCount, ZeroFill)
{
    Grid grid;
    EXPECT_EQ(0, grid.contRemaining());

    grid.fill(0);
    EXPECT_EQ(0, grid.contRemaining());
}

TEST(GridFillAndCount, PositiveFill)
{
    Grid grid;
    for (int i = 1; i <= 100; i++)
    {
        grid.fill(i);
        EXPECT_EQ(i, grid.contRemaining());
    }
}

TEST(GridFillAndCount, OverFill)
{
    Grid grid;
    for (int i = 101; i <= 200; i++)
    {
        grid.fill(i);
        EXPECT_EQ(100, grid.contRemaining());
    }
}

TEST(GridString, EmptyGrid)
{
    Grid grid;
    EXPECT_EQ("", grid.toString());
}

TEST(GridString, SingleRowFill)
{
    Grid grid;
    grid.setCell(0, 9, 1);
    grid.setCell(1, 9, 2);
    grid.setCell(2, 9, 3);
    EXPECT_EQ("1.2.3.", grid.toString());
}

TEST(GridString, MultipleRowFill)
{
    Grid grid;
    for (int i = 0; i < 10; i++)
    {
        grid.setCell(i, 9, i);
    }
    grid.setCell(0, 8, 0);
    EXPECT_EQ("0.1.2.3.4.5.6.7.8.9..0.", grid.toString());
}

TEST(GridStringCursor, EmptyGridOutsideCursor)
{
    Grid grid;
    std::string gr("");
    for (int i = 0; i < 10; i++)
    {
        gr.append("..........:");
    }
    EXPECT_EQ(gr, grid.toString(10, 10, 10, 10));
}

TEST(GridStringCursor, InsideCursor)
{
    Grid grid;
    grid.setCell(0, 9, 1);
    grid.setCell(1, 9, 2);
    grid.setCell(2, 9, 3);

    std::string gr("");
    for (int i = 0; i < 9; i++)
    {
        gr.append("..........:");
    }

    EXPECT_EQ(gr + "*1.>2.3........:", grid.toString(0, 9, 1, 9));
    EXPECT_EQ(gr + ">1.*2.3........:", grid.toString(1, 9, 0, 9));
    EXPECT_EQ(gr + "1.*2.>3........:", grid.toString(1, 9, 2, 9));
    EXPECT_EQ(gr + "1.>2.*3........:", grid.toString(2, 9, 1, 9));
    EXPECT_EQ(gr + "*1.2.>3........:", grid.toString(0, 9, 2, 9));
    EXPECT_EQ(gr + ">1.2.*3........:", grid.toString(2, 9, 0, 9));
}

TEST(GridStringCursor, MultipleRowCursor)
{
    Grid grid;
    grid.setCell(0, 9, 1);
    grid.setCell(1, 9, 2);
    grid.setCell(2, 9, 3);
    
    std::string gr("");
    for (int i = 0; i < 7; i++)
    {
        gr.append("..........:");
    }

    EXPECT_EQ(gr + "..........:>..........:*1.2.3........:", grid.toString(0, 9, 0, 8));
    EXPECT_EQ(gr + "..........:.>.........:*1.2.3........:", grid.toString(0, 9, 1, 8));
    EXPECT_EQ(gr + ">..........:..........:*1.2.3........:", grid.toString(0, 9, 0, 7));
}

TEST(GridStringCursor, EqualStartEndCursor)
{
    Grid grid;
    grid.setCell(0, 9, 1);
    grid.setCell(1, 9, 2);
    grid.setCell(2, 9, 3);

    std::string gr("");
    for (int i = 0; i < 9; i++)
    {
        gr.append("..........:");
    }
    EXPECT_EQ(gr + "*>1.2.3........:", grid.toString(0, 9, 0, 9));
}