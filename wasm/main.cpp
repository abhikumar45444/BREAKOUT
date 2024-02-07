#include <raylib.h>
#include <iostream>
#include <vector>
#include <utility>
#include <string>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

using namespace std;

#define TILESROW 10
#define TILESCOL 10

//! constants
int WIDTH = 1200;
int HEIGHT = 650;
const char *TITLE = "BREAKOUT";

//! variables
Color backgroundColor = BLACK;
int newWidth = GetScreenWidth();
int newHeight = GetScreenHeight();
bool isGameStarted = false;
bool isGameOver = false;
bool isGameWon = false;
int highscore = 0;

//! Player lifes
int livesLeft = 4;
struct LifeRects
{
    Vector2 pos;
    Color color;
};

vector<LifeRects> livesrects;

bool ballReset = false;
float delayTime = 3.0f;
float elapsedTime = 0.0f;
float currentTime = 0.0f;

//! paddle variables
float paddleWidth = 100.0;
float paddleHeight = 15.0;
float paddlePosX = WIDTH * 0.5f - paddleWidth * 0.5f;
float paddlePosY = HEIGHT - 50;
Color paddleColor = DARKBLUE;
float paddleSpeed = 400.0f;
bool isPaddleDirLeft = false;
bool isPaddleDirRight = false;

//! ball variables
int ballGap = 3;
float ballWidth = 18.0;
float ballHeight = 18.0;
float ballPosX = paddlePosX + paddleWidth * 0.5f - ballWidth * 0.5f;
float ballPosY = paddlePosY - paddleHeight * 0.5f - ballHeight * 0.5f - ballGap;
Color ballColor = GOLD;
float ballSpeed = -250.0f;
float ballSpeedX = ballSpeed;
float ballSpeedY = ballSpeed;

//! Tiles variables
struct Tile
{
    bool isAlive;
    Vector2 pos;
    Color color;
    int hitsLeft;
};

vector<vector<Tile>> tiles(TILESROW);
int tileWidth = 70;
int tileHeight = 12;

//! Game Score
int score = 0;


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
void RenderScore();
void RenderHighScore();
void GameStartScreen();
void GameOverScreen();
void GameWonScreen();
void IsGameWon();
void GameReset();
void Lives();
void InitializeLifeRects();
void DrawLiveRects();

int main()
{

    //* Window initialization
    InitWindow(WIDTH, HEIGHT, TITLE);

    //* set Frames per second
    const int FPS = 60;
    SetTargetFPS(FPS);

    InitializeTiles();
    InitializeLifeRects();

    #if defined(PLATFORM_WEB)
    InitializeTiles();
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
    #else
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    //* game loop
    while (!WindowShouldClose())
    {
        UpdateDrawFrame();
    }
    #endif

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
        ballPosX = paddlePosX + paddleWidth * 0.5f - ballWidth * 0.5f;
        ballPosY = paddlePosY - paddleHeight * 0.5f - ballHeight * 0.5f - ballGap;
        
        if (!isGameStarted)
        {
            return;
        }

        currentTime = currentTime + elapsedTime;
        if (currentTime >= delayTime)
        {
            ballReset = false;
            currentTime = 0.0f;
        }
    }
    else
    {
        if(!isGameStarted)
            return;
        
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
        if(livesLeft > 0)
        {
            livesrects.pop_back();
            ballReset = true;
            ballPosX = (paddlePosX + paddleWidth * 0.5f - ballWidth * 0.5f);
            ballPosY = (paddlePosY - paddleHeight * 0.5f - ballHeight * 0.5f) - ballGap;
        }
        else
        {
            isGameOver = true;
        }
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
                if ((ballPosX + ballWidth >= tilePos.x 
                    && ballPosX <= tilePos.x + tileWidth) 
                    && (ballPosY + ballWidth >= tilePos.y 
                    && ballPosY <= tilePos.y + tileHeight))
                {
                    if(tiles[i][j].hitsLeft < 2)
                    {
                        tiles[i][j].isAlive = false;

                        if(i >= 0 && i <= 2)
                            score += 300;
                        else if(i >= 3 && i <= 6)
                            score += 200;
                        else
                            score += 100;
                    }
                    ballSpeedY = -ballSpeedY;
                    tiles[i][j].color.a -= 55;
                    tiles[i][j].hitsLeft--;
                }
            }
        }
    }
}

void InitializeTiles()
{
    float posX = (WIDTH - ((tileWidth * TILESCOL) + 15.0f * (TILESCOL - 1))) * 0.5f;
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
        {46, 130, 232, 255}};

    for (int i = 0; i < TILESROW; i++)
    {
        for (int j = 0; j < TILESCOL; j++)
        {
            Vector2 tilePos = {posX, posY};
            bool tilePresent = true;
            int hits = 0;
            if(i >= 0 && i <= 2)
            {
                hits = 3;
            }
            else if(i >= 3 && i <= 6)
            {
                hits = 2;
            }
            else
            {
                hits = 1;
            }
            Tile tile = {tilePresent, tilePos, brickRowColor[i], hits};
            tiles[i].push_back(tile);
            posX += tileWidth + 15.0f;
        }

        posX = (WIDTH - ((tileWidth * TILESCOL) + 15.0f * (TILESCOL - 1))) * 0.5f;
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

        for (int i = 0; i < livesrects.size(); i++)
        {
            livesrects[i].pos.x = livesrects[i].pos.x / WIDTH * newWidth;
            livesrects[i].pos.y = livesrects[i].pos.y / HEIGHT * newHeight;
        }

        WIDTH = newWidth;
        HEIGHT = newHeight;
    }
}


void RenderScore()
{
    DrawText(TextFormat("Score: %i", score), 10, 10, 20, RED);
}

void RenderHighScore()
{
    const char *text = "Highscore: ";
    int textWidth =  MeasureText(text, 20);
    DrawText(TextFormat("%s%i", text, highscore), WIDTH - textWidth - 60, 10, 20, YELLOW);
}


void GameStartScreen()
{  
    DrawRectangle(0, 0, WIDTH, HEIGHT, {220, 220, 220, 230});
    const char *text = "Press Space Key to Start";
    int textWidth =  MeasureText(text, 40);
    DrawText(TextFormat(text), WIDTH * 0.5f-textWidth * 0.5f, HEIGHT * 0.5f, 40, RED);
}


void GameOverScreen()
{  
    DrawRectangleGradientV(0, 0, WIDTH, HEIGHT, {50, 50, 230, 50}, {200, 122, 255, 255});
    const char *gameOverText = "Game Over";
    int gameOverTextWidth =  MeasureText(gameOverText, 40);
    DrawText(TextFormat(gameOverText), WIDTH * 0.5f-gameOverTextWidth * 0.5f, HEIGHT * 0.5f - 200, 40, RED);
    const char *scoreText = "Your Score :";
    int scoreTextWidth =  MeasureText(scoreText, 40);
    int scoreWidth = MeasureText(to_string(score).c_str(), 40);
    DrawText(TextFormat("%s %d", scoreText, score), WIDTH * 0.5f-scoreTextWidth * 0.5f - scoreWidth * 0.5f, HEIGHT * 0.5f - 100, 40, RED);
    const char *restartText = "Press 'Enter' Key to Restart Game";
    int restartTextWidth =  MeasureText(restartText, 40);
    DrawText(TextFormat(restartText), WIDTH * 0.5f-restartTextWidth * 0.5f, HEIGHT * 0.5f, 40, RED);
}

void GameWonScreen()
{  
    DrawRectangleGradientV(0, 0, WIDTH, HEIGHT, {50, 50, 230, 50}, {200, 122, 255, 255});
    const char *gameOverText = "YOU WIN";
    int gameOverTextWidth =  MeasureText(gameOverText, 40);
    DrawText(TextFormat(gameOverText), WIDTH * 0.5f-gameOverTextWidth * 0.5f, HEIGHT * 0.5f - 200, 40, RED);
    const char *scoreText = "Your Score :";
    int scoreTextWidth =  MeasureText(scoreText, 40);
    int scoreWidth = MeasureText(to_string(score).c_str(), 40);
    DrawText(TextFormat("%s %d", scoreText, score), WIDTH * 0.5f-scoreTextWidth * 0.5f - scoreWidth * 0.5f, HEIGHT * 0.5f - 100, 40, RED);
    const char *restartText = "Press 'Enter' Key to Restart Game";
    int restartTextWidth =  MeasureText(restartText, 40);
    DrawText(TextFormat(restartText), WIDTH * 0.5f-restartTextWidth * 0.5f, HEIGHT * 0.5f, 40, RED);
}


void IsGameWon()
{
    for (int i = 0; i < TILESROW; i++)
    {
        for (int j = 0; j < TILESCOL; j++)
        {
            if (tiles[i][j].isAlive)
            {
                isGameWon = false;
                return;
            }
        }
    }

    isGameWon = true;
}


void GameReset()
{
    livesLeft = 4;

    if(highscore < score)
    {
        highscore = score;
    }

    score = 0;

    ballPosX = paddlePosX + paddleWidth * 0.5f - ballWidth * 0.5f;
    ballPosY = paddlePosY - paddleHeight * 0.5f - ballHeight * 0.5f - ballGap;

    livesrects.clear();

    for (int i = 0; i < TILESROW; i++)
    {
        tiles[i].clear();
    }

    InitializeLifeRects();
    InitializeTiles();
    isGameStarted = false;
}

void Lives()
{
    int fontSize = 20;
    const char *text = "Lives : ";
    DrawText(TextFormat(text), 20, HEIGHT - 30, fontSize, MAROON);
    DrawLiveRects();
}

void InitializeLifeRects()
{
    float posX = (250 + ballWidth - ((ballWidth * 3) + 15.0f * (3 - 1))) * 0.5f;
    float posY = HEIGHT - 30;

    for (int i = 0; i < 3; i++)
    {
        Vector2 liveRectPos = {posX, posY};
        LifeRects liveRect = {liveRectPos, BLACK};
        livesrects.push_back(liveRect);
        posX += ballWidth + 15.0f;
    }
}

void DrawLiveRects()
{
    for (int i = 0; i < livesrects.size(); i++)
    {
        DrawRectangle(livesrects[i].pos.x, livesrects[i].pos.y, ballWidth, ballHeight, livesrects[i].color);
    }
}


void UpdateDrawFrame(void)
{
    BeginDrawing();

    ClearBackground(backgroundColor);

    if(IsKeyPressed(KEY_SPACE))
    {
        isGameStarted = !isGameStarted;
    }

    if(isGameOver)
    {
        GameOverScreen();
        if(IsKeyPressed(KEY_ENTER))
        {
            isGameOver = false;
            GameReset();
        }
        EndDrawing();
        return;
    }

    if(isGameWon)
    {
        GameWonScreen();
        if(IsKeyPressed(KEY_ENTER))
        {
            isGameWon = false;
            GameReset();
        }
        EndDrawing();
        return;
    }

    if (isGameStarted)
    {
        DrawRectangleGradientV(0, 0, WIDTH, HEIGHT, {50, 50, 230, 50}, {200, 122, 255, 255});

        IsGameWon();

        ScreenResized();
        userInput();

        BallReset(); //* BallReset() function is calling BallMovement() function
        BallCollisionWithPaddle();
        BallCollisionWithTiles();
        RenderScore();
        RenderHighScore();
        Ball();
        Paddle();
        DrawTiles();
        Lives();
        DrawFPS(WIDTH - 75, HEIGHT - 20);
    }
    else
    {
        DrawRectangleGradientV(0, 0, WIDTH, HEIGHT, {50, 50, 230}, {200, 122, 255, 255});
        ScreenResized();
        BallReset(); //* BallReset() function is calling BallMovement() function
        RenderScore();
        RenderHighScore();
        Ball();
        Paddle();
        DrawTiles();
        Lives();
        DrawFPS(WIDTH - 75, HEIGHT - 20);
        GameStartScreen();
    }

    EndDrawing();
    elapsedTime = GetFrameTime();
}