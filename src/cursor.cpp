#include "cursor.h"

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

void Cursor::updateCursor(int xAdd, int yAdd)
{
    xS_ = xS_ + xAdd;
    yS_ = yS_ + yAdd;
    auto xyE = cursorType.cursorEnd(xS_, yS_);
    xE_ = std::get<0>(xyE);
    yE_ = std::get<1>(xyE);
}