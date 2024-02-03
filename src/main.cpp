#include <raylib.h>
#include <iostream>
#include <vector>
#include <utility>

using namespace std;

#define TILESROW 10
#define TILESCOL 10

//! constants
int WIDTH = 1200;
int HEIGHT = 700;
const char *TITLE = "BREAKOUT";

//! variables
Color backgroundColor = BLACK;
int newWidth = GetScreenWidth();
int newHeight = GetScreenHeight();

//! Player lifes
int livesLeft = 3;
bool ballReset = false;
float delayTime = 3.0f;
float elapsedTime = 0.0f;
float currentTime = 0.0f;

//! paddle variables
float paddleWidth = 100.0;
float paddleHeight = 15.0;
float paddlePosX = WIDTH / 2 - paddleWidth / 2;
float paddlePosY = HEIGHT - 50;
Color paddleColor = DARKBLUE;
float paddleSpeed = 300.0f;
bool isPaddleDirLeft = false;
bool isPaddleDirRight = false;

//! ball variables
float ballWidth = 20.0;
float ballHeight = 20.0;
float ballPosX = 400.0;
float ballPosY = 700.0;
Color ballColor = GOLD;
float ballSpeed = -300.0f;
float ballSpeedX = ballSpeed;
float ballSpeedY = ballSpeed;
int ballGap = 3;

//! Tiles variables
struct Tile
{
    bool isAlive;
    Vector2 pos;
    Color color;
};

vector<vector<Tile>> tiles(TILESROW);
int tileWidth = 70;
int tileHeight = 15;

//!  function declarations
void ScreenResized();
void userInput();
void Paddle();
void BallReset();
void Ball();
void BallMovement();
void BallCollisionWithPaddle();
void BallCollisionWithTiles();
void InitializeTiles();
void DrawTiles();
void UpdateDrawFrame(void);

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    //* Window initialization
    InitWindow(WIDTH, HEIGHT, TITLE);

    //* set Frames per second
    const int FPS = 60;
    SetTargetFPS(FPS);

    InitializeTiles();

    //* game loop
    while (!WindowShouldClose())
    {
        UpdateDrawFrame();
    }

    //* freeing up resources
    CloseWindow();
    return 0;
}

void userInput()
{
    if (IsKeyDown(KEY_LEFT) && paddlePosX > 0)
    {
        paddlePosX -= paddleSpeed * GetFrameTime();
        isPaddleDirLeft = true;
        isPaddleDirRight = false;
    }

    if (IsKeyDown(KEY_RIGHT) && paddlePosX < WIDTH - paddleWidth)
    {
        paddlePosX += paddleSpeed * GetFrameTime();
        isPaddleDirLeft = false;
        isPaddleDirRight = true;
    }
}

void Paddle()
{
    DrawRectangle(paddlePosX, paddlePosY, paddleWidth, paddleHeight, paddleColor);
}

void BallReset()
{
    if (ballReset)
    {
        ballPosX = paddlePosX + paddleWidth / 2 - ballWidth / 2;
        ballPosY = paddlePosY - paddleHeight / 2 - ballHeight / 2 - ballGap;
        currentTime = currentTime + elapsedTime;
        if (currentTime >= delayTime)
        {
            ballReset = false;
            currentTime = 0.0f;
        }
    }
    else
    {
        BallMovement();
    }
}

void Ball()
{
    DrawRectangle(ballPosX, ballPosY, ballWidth, ballHeight, ballColor);
}

void BallMovement()
{
    ballPosX += ballSpeedX * GetFrameTime();
    ballPosY += ballSpeedY * GetFrameTime();

    if (ballPosX < 0 || ballPosX + ballWidth > WIDTH)
    {
        ballSpeedX = -ballSpeedX;
    }

    if (ballPosY < 0)
    {
        ballSpeedY = -ballSpeedY;
    }

    if (ballPosY + ballHeight > HEIGHT /*paddlePosY + paddleHeight + 50*/)
    {
        livesLeft--;
        ballReset = true;
        ballPosX = (paddlePosX + paddleWidth / 2 - ballWidth / 2);
        ballPosY = (paddlePosY - paddleHeight / 2 - ballHeight / 2) - ballGap;
    }
}

void BallCollisionWithPaddle()
{
    // checking ball collision with paddle
    if (ballPosX + ballWidth >= paddlePosX &&
        ballPosX <= paddlePosX + paddleWidth &&
        ballPosY + ballHeight >= paddlePosY &&
        ballPosY <= paddlePosY + paddleHeight)
    {
        if ((isPaddleDirLeft && ballSpeedX > 0) || (isPaddleDirRight && ballSpeedX < 0))
        {
            ballSpeedX = -ballSpeedX;
        }

        ballSpeedY = -ballSpeedY;
    }
}

void BallCollisionWithTiles()
{
    for (int i = 0; i < TILESROW; i++)
    {
        for (int j = 0; j < TILESCOL; j++)
        {
            if (tiles[i][j].isAlive)
            {
                Vector2 tilePos = tiles[i][j].pos;
                if ((ballPosX + ballWidth >= tilePos.x && ballPosX <= tilePos.x + tileWidth) && (ballPosY + ballWidth >= tilePos.y && ballPosY <= tilePos.y + tileHeight))
                {
                    tiles[i][j].isAlive = false;
                    ballSpeedY = -ballSpeedY;
                }
            }
        }
    }
}

void InitializeTiles()
{
    float posX = (WIDTH - ((tileWidth * TILESCOL) + 20.0f * (TILESCOL - 1))) / 2;
    float posY = 50.0f;

    vector<Color> brickRowColor = {
        {255, 46, 46, 255},
        {232, 130, 46, 255},
        {205, 173, 46, 255},
        {173, 205, 46, 255},
        {130, 232, 46, 255},
        {46, 255, 46, 255},
        {46, 232, 130, 255},
        {46, 205, 173, 255},
        {46, 173, 205, 255},
        {46, 130, 232, 255}
    };

    for (int i = 0; i < TILESROW; i++)
    {
        for (int j = 0; j < TILESCOL; j++)
        {
            Vector2 tilePos = {posX, posY};
            bool tilePresent = true;
            Tile tile = {tilePresent, tilePos, brickRowColor[i]};
            tiles[i].push_back(tile);
            posX += tileWidth + 20.0f;
        }

        // brickRowColor++;
        posX = (WIDTH - ((tileWidth * TILESCOL) + 20.0f * (TILESCOL - 1))) / 2;
        posY += tileHeight + 15.0f;
    }
}

void DrawTiles()
{
    for (int i = 0; i < TILESROW; i++)
    {
        for (int j = 0; j < TILESCOL; j++)
        {
            if (tiles[i][j].isAlive)
            {
                DrawRectangle(tiles[i][j].pos.x, tiles[i][j].pos.y, tileWidth, tileHeight, tiles[i][j].color);
            }
        }
    }
}

void ScreenResized()
{
    if (IsWindowResized() && GetScreenWidth() >= 700)
    {
        int newWidth = GetScreenWidth();
        int newHeight = GetScreenHeight();

        paddlePosX = paddlePosX / WIDTH * newWidth;
        paddlePosY = paddlePosY / HEIGHT * newHeight;

        ballPosX = ballPosX / WIDTH * newWidth;
        ballPosY = ballPosY / HEIGHT * newHeight;

        for (int i = 0; i < TILESROW; i++)
        {
            for (int j = 0; j < TILESCOL; j++)
            {
                if (tiles[i][j].isAlive)
                {
                    tiles[i][j].pos.x = tiles[i][j].pos.x / WIDTH * newWidth;
                    tiles[i][j].pos.y = tiles[i][j].pos.y / HEIGHT * newHeight;
                }
            }
        }

        WIDTH = newWidth;
        HEIGHT = newHeight;
    }
}

void UpdateDrawFrame(void)
{
    BeginDrawing();

    ClearBackground(backgroundColor);
    DrawRectangleGradientV(0, 0, WIDTH, HEIGHT, {50,50,230}, {200, 122, 255, 255});

    ScreenResized();
    userInput();

    BallReset(); //* BallReset() function is calling BallMovement() function
    BallCollisionWithPaddle();
    BallCollisionWithTiles();

    Ball();
    Paddle();
    DrawTiles();
    DrawFPS(0, 0);
    EndDrawing();
    elapsedTime = GetFrameTime();
}