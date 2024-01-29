#include <raylib.h>
#include <iostream>
#include <vector>
#include <utility>

using namespace std;

#define TILESROW 5
#define TILESCOL 10

//! constants
int WIDTH = 800;
int HEIGHT = 600;
const char *TITLE = "BREAKING TILES";

//! variables
Color backgroundColor = BLACK;

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
Color paddleColor = PURPLE;
float paddleSpeed = 300.0f;
bool isPaddleDirLeft = false;
bool isPaddleDirRight = false;

//! ball variables
float ballWidth = 20.0;
float ballHeight = 20.0;
float ballPosX = 400.0;
float ballPosY = 700.0;
Color ballColor = RED;
float ballSpeed = -300.0f;
float ballSpeedX = ballSpeed;
float ballSpeedY = ballSpeed;
int ballGap = 3;

//! Tiles variables
vector<vector<pair<bool, Vector2>>> tiles(TILESROW);
int tileWidth = 50;
int tileHeight = 20;

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

    int newWidth = GetScreenWidth();
    int newHeight = GetScreenHeight();

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
            if (tiles[i][j].first)
            {
                Vector2 tilePos = tiles[i][j].second;
                if ((ballPosX + ballWidth >= tilePos.x && ballPosX <= tilePos.x + tileWidth) && (ballPosY + ballWidth >= tilePos.y && ballPosY <= tilePos.y + tileHeight))
                {
                    tiles[i][j].first = false;
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
    for (int i = 0; i < TILESROW; i++)
    {
        for (int j = 0; j < TILESCOL; j++)
        {
            Vector2 tilePos = {posX, posY};
            bool tilePresent = true;
            pair<bool, Vector2> tile = make_pair(tilePresent, tilePos);
            tiles[i].push_back(tile);
            posX += tileWidth + 20.0f;
        }
        posX = (WIDTH - ((tileWidth * TILESCOL) + 20.0f * (TILESCOL - 1))) / 2;
        posY += tileHeight + 20.0f;
    }
}

void DrawTiles()
{
    for (int i = 0; i < TILESROW; i++)
    {
        for (int j = 0; j < TILESCOL; j++)
        {
            if (tiles[i][j].first)
            {
                DrawRectangle(tiles[i][j].second.x, tiles[i][j].second.y, tileWidth, tileHeight, BLUE);
            }
        }
    }
}


void ScreenResized()
{
    if(IsWindowResized())
    {
        int newWidth = GetScreenWidth();
        int newHeight = GetScreenHeight();

        paddlePosX = paddlePosX / WIDTH * newWidth;
        paddlePosY = paddlePosY / HEIGHT * newHeight;

        WIDTH = newWidth;
        HEIGHT = newHeight;
    }
}


void UpdateDrawFrame(void)
{
    BeginDrawing();

    ClearBackground(backgroundColor);
    DrawRectangleGradientV(0, 0, WIDTH, HEIGHT, GREEN, YELLOW);

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