// #pragma once // This header file has been included in multiple file, thus this avoid error C2011
#ifndef CURSOR_H_
#define CURSOR_H_

#include <tuple>
/**
 * @brief The cursor direction (right, bottom, left and top). They are defined
 * in clockwise order.
 */
enum CursorDirection
{
    RGT,
    BOT,
    LFT,
    TOP,
};

/**
 * @brief The type of the user cursor. It defines how the cursor behaves.
 *
 * This is NOT the actual cursor that the user moves on the grid. This has to
 * be attached to the actual cursor, which works as a wrapper. The actual
 * cursor will assume the behavior specified by this class.
 */
class CursorType
{
protected:
    CursorDirection direction = CursorDirection::RGT;

public:
    virtual std::tuple<int, int> cursorEnd(int xS, int yS);
    void rotateLeft();
    void rotateRight();
};

/**
 * @brief The user cursor on the Grid. By default, it spawn at the bottom-left
 * corner of the Grid and takes two cells.
 */
class Cursor
{
    int xS_ = 0;
    int yS_ = 9;
    int xE_ = 1;
    int yE_ = 9;
    CursorType cursorType_;

public:
    /**
     * @brief The x coordinate on the grid of where the cursor starts.
     */
    int xS() const { return xS_; }
    /**
     * @brief The y coordinate on the grid of where the cursor starts.
     */
    int yS() const { return yS_; }
    /**
     * @brief The x coordinate on the grid of where the cursor ends.
     */
    int xE() const { return xE_; }
    /**
     * @brief The y coordinate on the grid of where the cursor ends.
     */
    int yE() const { return yE_; }

    /**
     * @brief Changes the cursor type (i.e., its behavior).
     * 
     * @param t The new cursor type.
     */
    void setType(CursorType t) { cursorType_ = t; }

    /**
     * @brief Shift the cursor.
     *
     * @param xAdd Amount to add to the x coordinate of the cursor start.
     * @param yAdd Amount to add to the y coordinate of the cursor end.
     */
    void updateCursor(int xAdd, int yAdd);

    /**
     * @brief Rotate the cursor counterclockwise.
     */
    void rotateLeft();

    /**
     * @brief Rotate the cursor clockwise.
     */
    void rotateRight();
};

#endif