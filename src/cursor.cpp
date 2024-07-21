#include "cursor.h"

std::tuple<int, int> CursorType::cursorEnd(int xS, int yS)
{
    switch (direction)
    {
    case CursorDirection::RGT:
        return std::make_tuple(xS + 1, yS);
    case CursorDirection::LFT:
        return std::make_tuple(xS - 1, yS);
    case CursorDirection::TOP:
        return std::make_tuple(xS, yS - 1);
    case CursorDirection::BOT:
        return std::make_tuple(xS, yS + 1);
    default:
        break;
    }

    return std::make_tuple(xS + 1, yS);
}

void CursorType::rotateLeft()
{
    switch (direction)
    {
    case RGT:
        direction = TOP;
        break;
    case TOP:
        direction = LFT;
        break;
    case LFT:
        direction = BOT;
        break;
    case BOT:
        direction = RGT;
        break;
    }
}

void CursorType::rotateRight()
{
    switch (direction)
    {
    case RGT:
        direction = BOT;
        break;
    case BOT:
        direction = LFT;
        break;
    case LFT:
        direction = TOP;
        break;
    case TOP:
        direction = RGT;
        break;
    }
}

void Cursor::updateCursor(int xAdd, int yAdd)
{
    xS_ = xS_ + xAdd;
    yS_ = yS_ + yAdd;
    auto xyE = cursorType_.cursorEnd(xS_, yS_);
    xE_ = std::get<0>(xyE);
    yE_ = std::get<1>(xyE);
}

void Cursor::rotateLeft()
{
    cursorType_.rotateLeft();
    updateCursor(0, 0);
}

void Cursor::rotateRight()
{
    cursorType_.rotateRight();
    updateCursor(0, 0);
}

void Cursor::modOnGrid(Grid grid)
{
    std::tuple<int, int, int, int> xyMod = grid.modCursor(xS_, yS_, xE_, yE_);
    xS_ = std::get<0>(xyMod);
    yS_ = std::get<1>(xyMod);
    xE_ = std::get<2>(xyMod);
    yE_ = std::get<3>(xyMod);
    return;
}
