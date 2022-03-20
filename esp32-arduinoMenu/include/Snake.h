#ifndef _SNAKE_H
#define _SNAKE_H
#include <Arduino.h>
#include <Config.h>

class Snake
{
public:
    int snakeLen = 3;

    int *snakeLenX = new int[200];
    int *snakeLenY = new int[200];

    int hx = 3, hy = 3;
    int tx = 1, ty = 3;
    int appleX, appleY;

    bool IsSnakeCollision(int hx, int hy);
    void updateSnakePosition(bool appleFlag, int tx, int ty);
    bool Key_Scan();
    void DirectionAndCount();
    String keyDirection; //"upper" "lower" "right" "left"
    String hdirection;   //"_A" "_B"
    void CreatApple(int *snakeX, int *snakeY, int snakeLen);
    bool IfAppleEat(int _hx, int _hy, int _appleX, int _appleY);
    void AutoWalk();
    void DisplaySnake();
    Snake();
    ~Snake();
};

bool Snake::IsSnakeCollision(int hx, int hy)
{
    if (hx > 63 || hx < 0 || hy > 31 || hy < 0)
        return true;
    for (int t = 0; t < snakeLen - 1; t++)
    {
        if (hx == snakeLenX[t])
            for (int i = 0; i < snakeLen - 1; i++)
            {
                if (hy == snakeLenY[t])
                    return true;
            }
    }
    return false;
};
void Snake::updateSnakePosition(bool appleFlag, int tx, int ty)
{
    if (!appleFlag)
    {
        for (int t = 0; t < snakeLen - 1; t++)
        {
            snakeLenX[t] = snakeLenX[t + 1];
            snakeLenY[t] = snakeLenY[t + 1];
        }
        snakeLenX[snakeLen - 1] = hx;
        snakeLenY[snakeLen - 1] = hy;
    }
    else
    {
        for (int t = (snakeLen - 2); t > 0; t--)
        {
            snakeLenX[t] = snakeLenX[t - 1];
            snakeLenY[t] = snakeLenY[t - 1];
        }
        snakeLenX[snakeLen - 1] = hx;
        snakeLenY[snakeLen - 1] = hy;
        snakeLenX[0] = tx;
        snakeLenY[0] = ty;
    }
};

bool Snake::Key_Scan()
{
    keyDirection = "_S";
    if (serial_read == "l")
    {
        keyDirection = "_A";
        serial_read = "";
        return true;
    }
    else if (serial_read == "j")
    {
        keyDirection = "_B";
        serial_read = "";
        return true;
    }
    else
    {
        return false;
    }
};

void Snake::DirectionAndCount()
{
    if (hdirection == "right")
    {
        if (keyDirection == "_A")
        {
            hy += 1;
            hdirection = "lower";
        }
        else if (keyDirection == "_B")
        {
            hy -= 1;
            hdirection = "upper";
        }
    }
    else if (hdirection == "left")
    {
        if (keyDirection == "_A")
        {
            hy -= 1;
            hdirection = "upper";
        }
        else if (keyDirection == "_B")
        {
            hy += 1;
            hdirection = "lower";
        }
    }
    else if (hdirection == "upper")
    {
        if (keyDirection == "_A")
        {
            hx += 1;
            hdirection = "right";
        }
        else if (keyDirection == "_B")
        {
            hx -= 1;
            hdirection = "left";
        }
    }
    else if (hdirection == "lower")
    {
        if (keyDirection == "_A")
        {
            hx -= 1;
            hdirection = "left";
        }
        else if (keyDirection == "_B")
        {
            hx += 1;
            hdirection = "right";
        }
    }
};

Snake::Snake()
{
    snakeLenX[0] = 1;
    snakeLenX[1] = 2;
    snakeLenX[2] = 3;
    snakeLenY[0] = 3;
    snakeLenY[1] = 3;
    snakeLenY[2] = 3;
};

void Snake::CreatApple(int *snakeX, int *snakeY, int snakeLen)
{
    int posx;
    int posy;
    int f = 1;
    do
    {
        f = 1;
        posx = random(1, 62);
        posy = random(1, 30);
        for (int i = 0; i < snakeLen; i++)
        {
            if (posx == snakeX[i])
            {
                for (i = 0; i < snakeLen; i++)
                {
                    if (posy == snakeY[i])
                    {
                        f = 0;
                    }
                }
            }
        }
        appleX = posx;
        appleY = posy;
    } while (!f);
    display.drawPixel(posx, posy, 0xF800);
}

bool Snake::IfAppleEat(int _hx, int _hy, int _appleX, int _appleY)
{
    if (_hx == _appleX && _hy == _appleY)
    {
        return true;
    }
    return false;
}

void Snake::AutoWalk()
{
    if (hdirection == "upper")
    {
        hy -= 1;
    }
    else if (hdirection == "lower")
    {
        hy += 1;
    }
    else if (hdirection == "left")
    {
        hx -= 1;
    }
    else if (hdirection == "right")
    {
        hx += 1;
    }
}
void Snake::DisplaySnake()
{
    for (int i = 0; i < snakeLen; i++)
    {
        display.drawPixel(snakeLenX[i], snakeLenY[i], 0xF800); //
    }
    display.drawPixel(snakeLenX[snakeLen - 1], snakeLenY[snakeLen - 1], 0x07E0);
}
Snake::~Snake()
{
    delete[] snakeLenX;
    delete[] snakeLenY;
}
#endif // _Snake_h_
