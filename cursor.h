/// @brief The user cursor on the scheme.
class Cursor
{
    int xS_ = 0;
    int yS_ = 0;
    int xE_ = 1;
    int yE_ = 0;
    int xMax;
    int yMax;
    int clampX(int);
    int clampY(int);

public:
    /// @brief Create the user cursor placed at the start of the scheme.
    /// @param schemeW Scheme max row size.
    /// @param schemeH scheme max column size.
    Cursor(int schemeW, int schemeH)
    {
        xMax = schemeW;
        yMax = schemeH;
    }
    int xS() { return xS_; }
    int yS() { return yS_; }
    int xE() { return xE_; }
    int yE() { return yE_; }
    void updateCursor(int, int);
};

/// @brief Add (xAdd, yAdd) to the cursor start coordinate, then adjust where
/// the cursor ends.
void Cursor::updateCursor(int xAdd, int yAdd)
{
    xS_ = clampX(xS_ + xAdd);
    yS_ = clampY(yS_ + yAdd);
    xE_ = clampX(xS_ + 1);
    yE_ = yS_;
}

int Cursor::clampX(int x)
{
    if (x >= xMax)
    {
        return x = 0;
    }
    if (x < 0)
    {
        return x = xMax - 1;
    }
    return x;
}

int Cursor::clampY(int y)
{
    if (y >= yMax)
    {
        return y = 0;
    }
    if (y < 0)
    {
        return y = yMax - 1;
    }
    return y;
}