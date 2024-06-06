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