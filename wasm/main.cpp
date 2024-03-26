#include <raylib.h>
#include <iostream>
#include <vector>
#include <utility>
#include <string>
#include <algorithm>
#include <time.h>
#include "../include/raymath.h"


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
bool generateParticles = false;

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
float paddleWidth = 100.0f;
float paddleHeight = 15.0f;
float paddlePosX = WIDTH * 0.5f - paddleWidth * 0.5f;
float paddlePosY = HEIGHT - 50;
Color paddleColor = DARKBLUE;
float paddleSpeed = 400.0f;
bool isPaddleDirLeft = false;
bool isPaddleDirRight = false;
Color paddleColorFirst = { 255, 20, 30, 255 };
Color paddleColorSecond = BLACK;
Color paddleColorThird = { 255, 20, 30, 255 };
Color paddleColorFourth = { 180, 180, 180, 255 };

//! ball variables
int ballGap = 3.0f;
float ballWidth = 18.0f;
float ballHeight = 18.0f;
float ballPosX = paddlePosX + paddleWidth * 0.5f - ballWidth * 0.5f;
float ballPosY = paddlePosY - paddleHeight * 0.5f - ballHeight * 0.5f - ballGap;
Color ballColor = GOLD;
float ballSpeed = -300.0f;
float ballSpeedX = ballSpeed;
float ballSpeedY = ballSpeed;
float isCollidedHorizontally = false;
float ballcurrentTime = 0.0f;
float ballelapsedTime = 0.0f;
float balldelayTime = 0.1f;
float speedIncreasedFactor = 1.90f;
bool resetSpeed = false;

//! Tiles variables
struct Tile
{
    bool isAlive;
    Vector2 pos;
    Color color;
    int hitsLeft;
};

vector<vector<Tile>> tiles(TILESROW);
int tileWidth = 70.0f;
int tileHeight = 12.0f;

//! Game Score
int score = 0;

//! Game Sounds 
Sound tileBreakSound;
Sound tileHitSound;
Sound ballBreakSound;
Sound paddleHitSound;

//! Game Font
Font font;

//! Particle system
struct Size
{
    int w;
    int h;
};

const int minLifetimeValue = 0;
const int maxLifetimeValue = 2;
const int minSizeValue = 3;
const int maxSizeValue = 9;
const int particleMinAngle = 40;
const int particleMaxAngle = 150;
const int particleMinSpeed = 100;
const int particleMaxSpeed = 250;
int maxParticles = 50;

struct Particle
{
    float lifetime;
    Vector2 speed;
    Vector2 pos;
    Size size;
    Color color;
    float angle;
    bool isAlive;
    float currentLifeTime;
};

vector<Particle> particles;
//! Particle system ends


//! Animation State - starts

bool gameStartScreenAnimation = true;
bool gameStartScreenAnimationText = true;
bool gameStartedFirstTime = true;
bool isAnimationCancelled = false;

//! Animation State - ends


//!  function declarations - starts
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
void GameState();
void GameStartScreen();
void GameOverScreen();
void GameWonScreen();
void IsGameWon();
void GameReset();
void Lives();
void InitializeLifeRects();
void DrawLiveRects();
Vector4 Vector4Lerp(Vector4 a, Vector4 b, float t);
Vector4 srgb_to_linear(Vector4 color);
Vector4 linear_to_srgb(Vector4 color);
bool checkIntersectionX(const Rectangle &rect1, const Rectangle &rect2);
bool checkIntersectionY(const Rectangle &rect1, const Rectangle &rect2);
bool checkIntersection(const Rectangle &rect1, const Rectangle &rect2);
Color GetLerpedGradientColor(Color start, Color end, float t);
float randomFloat();
int randomInt(int a, int b);
float randomFloat(int a, int b);
void ParticleInit();
void DrawParticles();
void MoveParticles();
void GenerateParticles(Color tileColor, Vector2 tilePos, bool direction);
void ParticleCollisionWithTiles();
bool Animation(float &startingValue, const float &endingValue, float &progress, const float &timeToDecrease, bool &animating, Color &animColor, string animType);
void GameGradientBackground();
void BlurEffect(float posX, float posY, float width, float height, Color color);
void BlurEffect(float posX, float posY, float width, float height, Color color1, Color color2, Color color3, Color color4);
void BlurEffect(Font font, const char *text, Vector2 position, float fontSize, float spacing, Color tint);
//!  function declarations - ends


int main()
{
    srand(time(0));

    //* Window initialization
    InitWindow(WIDTH, HEIGHT, TITLE);

    //* Initialize Sound
    InitAudioDevice();

    tileBreakSound = LoadSound("../resources/tilebreak.ogg");
    tileHitSound = LoadSound("../resources/tilehit.ogg");
    ballBreakSound = LoadSound("../resources/ballbreak.ogg");
    paddleHitSound = LoadSound("../resources/paddlehit.ogg");

    SetSoundVolume(tileBreakSound, 0.5f);
    SetSoundVolume(tileHitSound, 0.5f);
    SetSoundVolume(ballBreakSound, 0.5f);
    SetSoundVolume(paddleHitSound, 0.7f);

    const char *filename = "../resources/font.ttf";
    font = LoadFontEx(filename, 50, 0, 250);

    #if defined(PLATFORM_WEB)
        const int FPS = 60;
        SetTargetFPS(FPS);
        InitializeTiles();
        InitializeLifeRects();
        ParticleInit();
        emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
    #else
        //* set Frames per second
        const int FPS = 60;
        SetTargetFPS(FPS);

        InitializeTiles();
        InitializeLifeRects();
        ParticleInit();

        //* game loop
        while (!WindowShouldClose())
        {
            UpdateDrawFrame();
        }
    #endif

    //* freeing up resources
    UnloadFont(font);
    UnloadSound(tileHitSound);
    UnloadSound(ballBreakSound);
    UnloadSound(tileBreakSound);
    UnloadSound(paddleHitSound);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}


//! Function Defintions
void GameState()
{
    if(gameStartedFirstTime)
    {
        if(IsKeyPressed(KEY_SPACE) && !gameStartScreenAnimation && !gameStartScreenAnimationText)
        {
            isGameStarted = !isGameStarted;
            gameStartedFirstTime = false;
        }
    }
    else
    {
        if(IsKeyPressed(KEY_SPACE) && !isAnimationCancelled)
        {
            isGameStarted = !isGameStarted;
            isAnimationCancelled = true;
        }
        else if(IsKeyPressed(KEY_SPACE))
        {
            isGameStarted = !isGameStarted;
        }
    }
}

void GameGradientBackground()
{
    DrawRectangleGradientV(0, 0, WIDTH, HEIGHT, {50, 50, 230, 30}, {200, 122, 240, 150});
}


void userInput()
{
    if (IsKeyDown(KEY_LEFT))
    {
        paddlePosX -= paddleSpeed * GetFrameTime();
        isPaddleDirLeft = true;
        isPaddleDirRight = false;
    }
    else
    {
        isPaddleDirLeft = false;
    }

    if (IsKeyDown(KEY_RIGHT))
    {
        paddlePosX += paddleSpeed * GetFrameTime();
        isPaddleDirLeft = false;
        isPaddleDirRight = true;
    }
    else
    {
        isPaddleDirRight = false;
    }

    paddlePosX = Clamp(paddlePosX, 0.0f, paddlePosX+paddleWidth);
    paddlePosX = Clamp(paddlePosX, paddlePosX-paddleWidth, (float)WIDTH-paddleWidth);
}

void Paddle()
{
    if(isGameStarted)
    {
        DrawRectangleGradientEx({paddlePosX, paddlePosY, paddleWidth, paddleHeight}, paddleColorFirst, paddleColorSecond, paddleColorThird, paddleColorFourth);
        return;
    }
  
    BlurEffect(paddlePosX, paddlePosY, paddleWidth, paddleHeight, paddleColorFirst, paddleColorSecond, paddleColorThird, paddleColorFourth);
}

void BallReset()
{
    if (ballReset)
    {
        ballPosX = paddlePosX + paddleWidth * 0.5f - ballWidth * 0.5f;
        ballPosY = paddlePosY - paddleHeight * 0.5f - ballHeight * 0.5f - ballGap;

        if ((isPaddleDirLeft && ballSpeedX > 0) || (isPaddleDirRight && ballSpeedX < 0))
        {
            ballSpeedX = -ballSpeedX;
        }

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
    if(isGameStarted)
    {
        DrawRectangle(ballPosX, ballPosY, ballWidth, ballHeight, ballColor);
        return;
    }

    BlurEffect(ballPosX, ballPosY, ballWidth, ballHeight, ballColor);
}

void BallMovement()
{
    if (isCollidedHorizontally)
    {
        ballcurrentTime = ballcurrentTime + ballelapsedTime;
        if (ballcurrentTime >= balldelayTime)
        {
            ballSpeedX = ballSpeedX / speedIncreasedFactor;
            ballSpeedY = ballSpeedY / speedIncreasedFactor;
            ballcurrentTime = 0.0f;
            resetSpeed = true;
        }
        else if(abs(ballSpeedX) >= 500.0f && ballSpeedX > 0)
        {
            ballSpeedX = abs(ballSpeed);
            ballSpeedY = abs(ballSpeed);
            ballcurrentTime = 0.0f;
            resetSpeed = true;
        }

        else if(abs(ballSpeedX) >= 500.0f && ballSpeedX < 0)
        {
            ballSpeedX = ballSpeed;
            ballSpeedY = abs(ballSpeed);
            ballcurrentTime = 0.0f;
            resetSpeed = true;
        }

        if (resetSpeed)
        {
            isCollidedHorizontally = false;
            resetSpeed = false;
        }
    }

    float newballPosX = ballPosX + ballSpeedX * GetFrameTime();
    if (ballPosX < 0 || ballPosX + ballWidth > WIDTH)
    {
        ballPosX = Clamp(ballPosX, 0.0f, ballPosX + ballWidth);
        ballPosX = Clamp(ballPosX, ballPosX - ballWidth, (float)WIDTH - ballWidth);
        ballSpeedX = -ballSpeedX;
        newballPosX = ballPosX + ballSpeedX * GetFrameTime();
    }
    ballPosX = newballPosX;

    float newballPosY = ballPosY + ballSpeedY * GetFrameTime();
    if (ballPosY < 0 || ballPosY + ballHeight > HEIGHT)
    {
        ballPosY = Clamp(ballPosY, 0.0f, ballPosY + ballHeight);
        ballPosY = Clamp(ballPosY, ballPosY - ballHeight, (float)HEIGHT - ballHeight);
        ballSpeedY = -ballSpeedY;
        newballPosY = ballPosY + ballSpeedY * GetFrameTime();
    }
    ballPosY = newballPosY;

    if (ballPosY + ballHeight >= HEIGHT /*paddlePosY + paddleHeight + 50*/)
    {
        livesLeft--;
        GenerateParticles(ballColor, {ballPosX, ballPosY}, false);
        if (!IsSoundPlaying(ballBreakSound))
        {
            PlaySound(ballBreakSound);
        }

        if(livesLeft > 0)
        {
            livesrects.pop_back();
            ballReset = true;
            ballPosX = (paddlePosX + paddleWidth * 0.5f - ballWidth * 0.5f);
            ballPosY = (paddlePosY - paddleHeight * 0.5f - ballHeight * 0.5f) - ballGap;
            ballSpeedX = ballSpeedY = ballSpeed;
        }
        else
        {
            isGameOver = true;
        }
    }
}


//! Checking rectangles intersections function - collision detection (AABB) - STARTS
bool checkIntersectionX(const Rectangle& rect1, const Rectangle& rect2) {
    float left1 = rect1.x;
    float right1 = rect1.x + rect1.width;

    float left2 = rect2.x;
    float right2 = rect2.x + rect2.width;

    bool intersectX = !((right1 < left2) || (right2 < left1));

    return intersectX ;
}

bool checkIntersectionY(const Rectangle& rect1, const Rectangle& rect2) {
    float top1 = rect1.y;
    float bottom1 = rect1.y + rect1.height;

    float top2 = rect2.y;
    float bottom2 = rect2.y + rect2.height;

    bool intersectY = !((bottom1 < top2) || (bottom2 < top1));

    return intersectY;
}


bool checkIntersection(const Rectangle& rect1, const Rectangle& rect2) {
    float left1 = rect1.x;
    float right1 = rect1.x + rect1.width;
    float top1 = rect1.y;
    float bottom1 = rect1.y + rect1.height;

    float left2 = rect2.x;
    float right2 = rect2.x + rect2.width;
    float top2 = rect2.y;
    float bottom2 = rect2.y + rect2.height;

    // Check for intersection along both axes
    bool intersectX = !((right1 < left2) || (right2 < left1));
    bool intersectY = !((bottom1 < top2) || (bottom2 < top1));

    // If both axes have intersection, then there is overall intersection
    return intersectX && intersectY;
}

//! Checking rectangles intersections function - collision detection (AABB) - ENDS


void BallCollisionWithPaddle()
{
    Rectangle rect1, rect2;
    rect1.x = ballPosX;
    rect1.y = ballPosY;
    rect1.width = ballWidth;
    rect1.height = ballHeight;

    rect2.x = paddlePosX;
    rect2.y = paddlePosY;
    rect2.width = paddleWidth;
    rect2.height = paddleHeight;

    float newballPosX = ballPosX + ballSpeedX * GetFrameTime();
    rect1.x = newballPosX;
    if(checkIntersection(rect1,rect2) && checkIntersectionX(rect1, rect2) 
       /* ((ballPosX + ballWidth > paddlePosX) || 
        (ballPosX < paddlePosX + paddleWidth)) */)
    {
        if((isPaddleDirLeft && ballSpeedX > 0) || (isPaddleDirRight && ballSpeedX < 0))
        {
            ballSpeedX = -(speedIncreasedFactor * ballSpeedX);
        }
        else if((isPaddleDirLeft && ballSpeedX < 0) || (isPaddleDirRight && ballSpeedX > 0))
        {
            ballSpeedX = (speedIncreasedFactor * ballSpeedX);
        }
        else
        {
            ballSpeedX = -(speedIncreasedFactor * ballSpeedX);
        }
        
        // ballSpeedX = -(speedIncreasedFactor * ballSpeedX);
        ballSpeedY = speedIncreasedFactor * ballSpeedY;
        isCollidedHorizontally = true;
        Color particleGradientColor = GetLerpedGradientColor(paddleColorFirst, paddleColorSecond, 0.12f);
        GenerateParticles(particleGradientColor, {paddlePosX, paddlePosY}, true);
        
        if (!IsSoundPlaying(paddleHitSound))
        {
            PlaySound(paddleHitSound);
        }
    }

    float newballPosY = ballPosY + ballSpeedY * GetFrameTime();
    rect1.y = newballPosY;
    if(checkIntersection(rect1, rect2) && checkIntersectionY(rect1, rect2)
       /* ((ballPosY + ballHeight > paddlePosY) || 
        (ballPosY < paddlePosY + paddleHeight)) */)
    {
        if ((isPaddleDirLeft && ballSpeedX > 0) || (isPaddleDirRight && ballSpeedX < 0))
        {
            ballSpeedX = -ballSpeedX;
        }

        ballSpeedY = -ballSpeedY;
        Color particleGradientColor = GetLerpedGradientColor(paddleColorFirst, paddleColorSecond, 0.12f);
        GenerateParticles(particleGradientColor, {paddlePosX, paddlePosY}, true);

        if (!IsSoundPlaying(paddleHitSound))
        {
            PlaySound(paddleHitSound);
        }
    }
}

void BallCollisionWithTiles()
{
    for (int i = 0; i < TILESROW; i++)
    {
        for (int j = 0; j < TILESCOL; j++)
        {
            bool isCollided = false;

            if (tiles[i][j].isAlive)
            {
                Vector2 tilePos = tiles[i][j].pos;

                Rectangle rect1, rect2;
                rect1.x = ballPosX;
                rect1.y = ballPosY;
                rect1.width = ballWidth;
                rect1.height = ballHeight;

                rect2.x = tilePos.x;
                rect2.y = tilePos.y;
                rect2.width = tileWidth;
                rect2.height = tileHeight;

                float newballPosX = ballPosX + ballSpeedX * GetFrameTime();
                rect1.x = newballPosX;
                if (checkIntersection(rect1,rect2) && 
                ((ballPosX + ballWidth > tilePos.x) || 
                 (ballPosX < tilePos.x + tileWidth)))
                {
                    ballSpeedX = -ballSpeedX;
                    isCollided = true;
                }

                float newballPosY = ballPosY + ballSpeedY * GetFrameTime();
                rect1.y = newballPosY;
                if (checkIntersection(rect1, rect2) && 
                ((ballPosY + ballHeight > tilePos.y) || 
                 (ballPosY < tilePos.y + tileHeight)))
                {
                    ballSpeedY = -ballSpeedY;
                    isCollided = true;
                }
                
                if (isCollided)
                {
                    if (tiles[i][j].hitsLeft < 2)
                    {
                        // direction -> true means downwards and false means upwards... 
                        bool direction = ballSpeedY > 0 ? false : true;
                        tiles[i][j].isAlive = false;
                        Color tileColor = tiles[i][j].color;
                        tileColor.a = 255;
                        if (!IsSoundPlaying(tileBreakSound))
                        {
                            PlaySound(tileBreakSound);
                        }
                        // Generate Particles
                        GenerateParticles(tileColor, tilePos, direction);

                        if (i >= 0 && i <= 2)
                            score += 300;
                        else if (i >= 3 && i <= 6)
                            score += 200;
                        else
                            score += 100;
                    }
                    else
                    {
                        if (!IsSoundPlaying(tileHitSound))
                        {
                            PlaySound(tileHitSound);
                        }
                    }
    
                    tiles[i][j].color.a -= 55;
                    tiles[i][j].hitsLeft--;
                }

                if(isCollided)
                {
                    return;
                }
            }
        }
    }
}

void InitializeTiles()
{
    float posX = (WIDTH - ((tileWidth * TILESCOL) + 30.0f * (TILESCOL - 1))) * 0.5f;
    float posY = 50.0f;

    vector<Color> brickRowColor;

    for (int row = 0; row < TILESROW; row++) {
        Vector4 red = srgb_to_linear(Vector4{1, 0.18f, 0.18f, 1});
        Vector4 green = srgb_to_linear(Vector4{0.18f, 1, 0.18f, 1});
        Vector4 blue = srgb_to_linear(Vector4{0.18f, 0.18f, 1, 1});
        float level = 0.5f;
        float t = (float)row / TILESROW;
        float c = (t < level) ? 1.0f : 0.0f;
        Vector4 g1 = Vector4Lerp(red, green, t / level);
        Vector4 g2 = Vector4Lerp(green, blue, (t - level) / (1 - level));

        Vector4 colorVec = linear_to_srgb(
            Vector4{
                c*g1.x+(1-c)*g2.x,
                c*g1.y+(1-c)*g2.y,
                c*g1.z+(1-c)*g2.z,
                c*g1.w+(1-c)*g2.w
            }
        );
        Color color = ColorFromNormalized(colorVec);
        brickRowColor.push_back(color);
    }

    for (int i = 0; i < TILESROW; i++)
    {
        for (int j = 0; j < TILESCOL; j++)
        {
            Vector2 tilePos = {posX, posY};
            bool tilePresent = true;
            int hits = 0;
            if (i >= 0 && i <= 2)
            {
                hits = 3;
            }
            else if (i >= 3 && i <= 6)
            {
                hits = 2;
            }
            else
            {
                hits = 1;
            }
            Tile tile = {tilePresent, tilePos, brickRowColor[i], hits};
            tiles[i].push_back(tile);
            posX += tileWidth + 30.0f;
        }

        posX = (WIDTH - ((tileWidth * TILESCOL) + 30.0f * (TILESCOL - 1))) * 0.5f;
        posY += tileHeight + 15.0f;
    }
}

void DrawTiles()
{
    if(isGameStarted)
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
        return;
    }

    for (int i = 0; i < TILESROW; i++)
    {
        for (int j = 0; j < TILESCOL; j++)
        {
            if (tiles[i][j].isAlive)
            {
                BlurEffect(tiles[i][j].pos.x, tiles[i][j].pos.y, tileWidth, tileHeight, tiles[i][j].color);
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
    const char *text = "Score: ";
    // int textWidth =  MeasureText(text, 30);
    if(isGameStarted)
    {
        // DrawText(TextFormat("Score: %i", score), 10, 10, 20, RED);
        DrawTextEx(font,TextFormat("%s%i", text, score),{20.0f, 10.0f}, 25.0f, 0, RED);
        return;
    }

    BlurEffect(font,TextFormat("%s%i", text, score),{20.0f, 10.0f}, 25.0f, 0, RED);
}

void RenderHighScore()
{
    const char *text = "Highscore: ";
    int textWidth =  MeasureText(text, 30);
    if(isGameStarted)
    {
        // DrawText(TextFormat("%s%i", text, highscore), WIDTH - textWidth - 60, 10, 20, YELLOW);
        DrawTextEx(font, TextFormat("%s%i", text, highscore), {(float)(WIDTH - textWidth - 90), 10}, 25, 0, YELLOW);
        return;
    }

    BlurEffect(font, TextFormat("%s%i", text, highscore), {(float)(WIDTH - textWidth - 90), 10.0f}, 25, 0, YELLOW); 
}


void GameStartScreen()
{
    static Color animColor = {220, 220, 220, 255};
    static float startingValue = animColor.a;
    static float endingValue = 180.0f;
    // static float currentValue = startingValue;
    static float timeToDecrease = 3.0f;
    static float progress = 0.0f;

    static Color textColor = {230, 41, 55, 0};

    static float textstartingValue = textColor.a;
    static float textendingValue = 255.0f;
    // static float textcurrentValue = startingValue;
    static float texttimeToDecrease = 2.0f;
    static float textprogress = 0.0f;
    
    static string textStr = "Press Space Key to Start";
    static const char *text = textStr.c_str();
    static int textWidth = MeasureText(text, 40);

    if(!isAnimationCancelled)
    {
        if (gameStartScreenAnimation && Animation(startingValue, endingValue, progress, timeToDecrease, gameStartScreenAnimation, animColor, "gamestart"))
        {
            DrawRectangle(0, 0, WIDTH, HEIGHT, animColor);
        }
        else
        {
            DrawRectangle(0, 0, WIDTH, HEIGHT, animColor);
            if (gameStartScreenAnimationText && Animation(textstartingValue, textendingValue, textprogress, texttimeToDecrease, gameStartScreenAnimationText, textColor, "text"))
            {
                DrawTextEx(font, TextFormat(text), {WIDTH * 0.5f - textWidth * 0.5f, HEIGHT * 0.5f}, 40, 0, textColor);
            }
            else
            {
                textColor.a = 255;
                DrawTextEx(font, TextFormat(text), {WIDTH * 0.5f - textWidth * 0.5f, HEIGHT * 0.5f}, 40, 0, textColor);
            }
        }
    }
    else
    {
        animColor.a = 200;
        DrawRectangle(0, 0, WIDTH, HEIGHT, animColor);
        textColor.a = 255;
        DrawTextEx(font, TextFormat(text), {WIDTH * 0.5f - textWidth * 0.5f, HEIGHT * 0.5f}, 40, 0, textColor); 
    }
}


void GameOverScreen()
{  
    DrawRectangleGradientV(0, 0, WIDTH, HEIGHT, {50, 50, 230, 50}, {200, 122, 255, 255});
    const char *gameOverText = "Game Over";
    int gameOverTextWidth =  MeasureText(gameOverText, 40);
    // DrawText(TextFormat(gameOverText), WIDTH * 0.5f-gameOverTextWidth * 0.5f, HEIGHT * 0.5f - 200, 40, RED);
    DrawTextEx(font, TextFormat(gameOverText), {WIDTH * 0.5f-gameOverTextWidth * 0.5f, HEIGHT * 0.5f - 200}, 40, 0, RED);
    const char *scoreText = "Your Score :";
    int scoreTextWidth =  MeasureText(scoreText, 40);
    int scoreWidth = MeasureText(to_string(score).c_str(), 40);
    // DrawText(TextFormat("%s %d", scoreText, score), WIDTH * 0.5f-scoreTextWidth * 0.5f - scoreWidth * 0.5f, HEIGHT * 0.5f - 100, 40, RED);
    DrawTextEx(font, TextFormat("%s %d", scoreText, score), {WIDTH * 0.5f-scoreTextWidth * 0.5f - scoreWidth * 0.5f, HEIGHT * 0.5f - 100}, 40, 0, RED);
    const char *restartText = "Press 'Enter' Key to Restart Game";
    int restartTextWidth =  MeasureText(restartText, 40);
    // DrawText(TextFormat(restartText), WIDTH * 0.5f-restartTextWidth * 0.5f, HEIGHT * 0.5f, 40, RED);
    DrawTextEx(font, TextFormat(restartText), {WIDTH * 0.5f-restartTextWidth * 0.5f, HEIGHT * 0.5f}, 40, 0, RED);
}

void GameWonScreen()
{  
    DrawRectangleGradientV(0, 0, WIDTH, HEIGHT, {50, 50, 230, 50}, {200, 122, 255, 255});
    const char *gameOverText = "YOU WIN";
    int gameOverTextWidth =  MeasureText(gameOverText, 40);
    // DrawText(TextFormat(gameOverText), WIDTH * 0.5f-gameOverTextWidth * 0.5f, HEIGHT * 0.5f - 200, 40, RED);
    DrawTextEx(font, TextFormat(gameOverText), {WIDTH * 0.5f-gameOverTextWidth * 0.5f, HEIGHT * 0.5f - 200}, 40, 0, RED);
    const char *scoreText = "Your Score :";
    int scoreTextWidth =  MeasureText(scoreText, 40);
    int scoreWidth = MeasureText(to_string(score).c_str(), 40);
    // DrawText(TextFormat("%s %d", scoreText, score), WIDTH * 0.5f-scoreTextWidth * 0.5f - scoreWidth * 0.5f, HEIGHT * 0.5f - 100, 40, RED);
    DrawTextEx(font, TextFormat("%s %d", scoreText, score), {WIDTH * 0.5f-scoreTextWidth * 0.5f - scoreWidth * 0.5f, HEIGHT * 0.5f - 100}, 40, 0, RED);
    const char *restartText = "Press 'Enter' Key to Restart Game";
    int restartTextWidth =  MeasureText(restartText, 40);
    // DrawText(TextFormat(restartText), WIDTH * 0.5f-restartTextWidth * 0.5f, HEIGHT * 0.5f, 40, RED);
    DrawTextEx(font, TextFormat(restartText), {WIDTH * 0.5f-restartTextWidth * 0.5f, HEIGHT * 0.5f}, 40, 0, RED);
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
    ballSpeedX = ballSpeedY = ballSpeed;

    livesrects.clear();
    particles.clear();

    for (int i = 0; i < TILESROW; i++)
    {
        tiles[i].clear();
    }

    InitializeLifeRects();
    InitializeTiles();
    ParticleInit();
    isGameStarted = false;
    generateParticles = false;
    ballReset = true;
    gameStartScreenAnimation = true;
    gameStartScreenAnimationText = true;
    isAnimationCancelled = false;
}

void Lives()
{
    int fontSize = 22;
    const char *text = "Lives : ";
    if(isGameStarted)
    {
        // DrawText(TextFormat(text), 20, HEIGHT - 30, fontSize, MAROON);
        DrawTextEx(font, TextFormat(text), {20.0f, HEIGHT - 30.0f}, fontSize, 2.0f, ORANGE);
        DrawLiveRects();
        return;
    }

    BlurEffect(font, TextFormat(text), {20.0f, (HEIGHT - 30.0f)}, fontSize, 2.0f, ORANGE);
    DrawLiveRects();
}

void InitializeLifeRects()
{
    float posX = (320 + ballWidth - ((ballWidth * 3) + 15.0f * (3 - 1))) * 0.5f;
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
    if(isGameStarted)
    {
        for (int i = 0; i < livesrects.size(); i++)
        {
            DrawRectangle(livesrects[i].pos.x, livesrects[i].pos.y, ballWidth, ballHeight, livesrects[i].color);
        }
        return;
    }

    for (int i = 0; i < livesrects.size(); i++)
    {
        BlurEffect(livesrects[i].pos.x, livesrects[i].pos.y, ballWidth, ballHeight, livesrects[i].color);
    }
}


// --------------------------------------------------------------------------------------------------------------
// ! Functions for creating Color lerp to provide smoother transition of colors between tiles - starts 

Vector4 Vector4Lerp(Vector4 a, Vector4 b, float t) {
    return Vector4{
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t,
        a.w + (b.w - a.w) * t
    };
}


Vector4 srgb_to_linear(Vector4 color) {
    return Vector4{
        (color.x <= 0.04045f) ? (color.x / 12.92f) : pow((color.x + 0.055f) / 1.055f, 2.4f),
        (color.y <= 0.04045f) ? (color.y / 12.92f) : pow((color.y + 0.055f) / 1.055f, 2.4f),
        (color.z <= 0.04045f) ? (color.z / 12.92f) : pow((color.z + 0.055f) / 1.055f, 2.4f),
        color.w
    };
}

Vector4 linear_to_srgb(Vector4 color) {
    return Vector4{
        (color.x <= 0.0031308f) ? (color.x * 12.92f) : (1.055f * pow(color.x, 1.0f / 2.4f) - 0.055f),
        (color.y <= 0.0031308f) ? (color.y * 12.92f) : (1.055f * pow(color.y, 1.0f / 2.4f) - 0.055f),
        (color.z <= 0.0031308f) ? (color.z * 12.92f) : (1.055f * pow(color.z, 1.0f / 2.4f) - 0.055f),
        color.w
    };
}

Color GetLerpedGradientColor(Color start, Color end, float t)
{
    Vector4 ColorStart = ColorNormalize(start);
    Vector4 ColorEnd = ColorNormalize(end);
    Vector4 gradientColorVec4 = Vector4Lerp(ColorStart, ColorEnd, t);
    gradientColorVec4 = srgb_to_linear(gradientColorVec4);
    Color gradientColor = ColorFromNormalized(gradientColorVec4);
    return gradientColor;
}

// ! Functions for creating Color lerp to provide smoother transition of colors between tiles - ends

// --------------------------------------------------------------------------------------------------------------

//! Particle system functions - starts
float randomFloat()
{
    return (float)(rand()) / (float)(RAND_MAX);
}
 
int randomInt(int a, int b)
{
    if (a > b)
        return randomInt(b, a);
    if (a == b)
        return a;
    return a + (rand() % (b - a));
}
 
float randomFloat(int a, int b)
{
    if (a > b)
        return randomFloat(b, a);
    if (a == b)
        return a;
 
    return (float)randomInt(a, b) + randomFloat();
}


void ParticleInit()
{
    int size = maxParticles;
    for (int i = 0; i < size; i++)
    {
        Vector2 pos = {50, 50};
        Size size = {GetRandomValue(minSizeValue,maxSizeValue), GetRandomValue(minSizeValue,maxSizeValue)};
        Color color = RED;
        float lifeTime = randomFloat(minLifetimeValue, maxLifetimeValue);
        Vector2 speed = {randomFloat(particleMinSpeed, particleMaxSpeed), randomFloat(particleMinSpeed, particleMaxSpeed)};
        float angle = 0;
        bool isAlive = false;
        Particle particle = {
            lifeTime,
            speed,
            pos,
            size,
            color,
            angle,
            isAlive,
            0.0f,
        };

        particles.push_back(particle);
    }

    particles.reserve(1024);
}

void DrawParticles()
{
    if(isGameStarted)
    {
        for (int i = 0; i < particles.size(); i++)
        {
            if(particles[i].isAlive)
            {
                DrawRectangle(particles[i].pos.x, particles[i].pos.y, particles[i].size.w, particles[i].size.h, particles[i].color);
            }
        }
        return;
    }

    for (int i = 0; i < particles.size(); i++)
    {
        if (particles[i].isAlive)
        {
            DrawRectangle(particles[i].pos.x, particles[i].pos.y, particles[i].size.w, particles[i].size.h, particles[i].color);
        }
    }
}

void MoveParticles()
{
    if(generateParticles)
    {
        if(isGameStarted)
        {
            for (int i = 0; i < particles.size(); i++)
            {
            
                if(particles[i].isAlive && particles[i].currentLifeTime < particles[i].lifetime /* particles[i].lifetime >= 0 */)
                {
                    particles[i].currentLifeTime += GetFrameTime();
                    float alpha = particles[i].currentLifeTime / particles[i].lifetime;
                    float easedAlpha = 1.0f - (alpha - 1.0f) * (alpha - 1.0f);
                    
                    particles[i].color.a = (unsigned char)(255 * (1.0f - easedAlpha));

                    float newparticlesPosX = particles[i].pos.x + particles[i].speed.x * GetFrameTime() * cosf(particles[i].angle * DEG2RAD);
                    if (particles[i].pos.x < 0 || particles[i].pos.x + particles[i].size.w > WIDTH)
                    {
                        particles[i].speed.x = -particles[i].speed.x;
                        newparticlesPosX = particles[i].pos.x + particles[i].speed.x * GetFrameTime() * cosf(particles[i].angle * DEG2RAD);
                    }

                    float newparticlesPosY = particles[i].pos.y + particles[i].speed.y * GetFrameTime() * sinf(particles[i].angle * DEG2RAD);
                    if (particles[i].pos.y < 0 || particles[i].pos.y + particles[i].size.h > HEIGHT)
                    {
                        particles[i].speed.y = -particles[i].speed.y;
                        newparticlesPosY = particles[i].pos.y + particles[i].speed.y * GetFrameTime() * sinf(particles[i].angle * DEG2RAD);
                    }

                    particles[i].pos.x = newparticlesPosX;
                    particles[i].pos.y = newparticlesPosY;
                }
                else
                {
                    particles.erase(particles.begin() + i);
                }
            }
        }

        DrawParticles();
    }
}


void GenerateParticles(Color tileColor, Vector2 tilePos, bool direction)
{
    generateParticles = true;

    for (int i = 0; i < maxParticles; i++)
    {
        Vector2 pos = tilePos;
        pos.x += 3.0f;
        pos.y += 3.0f;
        int particleSide = GetRandomValue(minSizeValue, maxSizeValue);
        Size size = {particleSide, particleSide};
        Color color = tileColor;
        float lifeTime = randomFloat(minLifetimeValue, maxLifetimeValue);
        // float particleSpeed = randomFloat(particleMinSpeed, particleMaxSpeed);
        Vector2 speed = {randomFloat(particleMinSpeed, particleMaxSpeed), randomFloat(particleMinSpeed, particleMaxSpeed)};
        float angle;
        if(direction)
        {
            int random = GetRandomValue(1, 4);
            switch (random)
            {
            case 1:
                angle = randomFloat(particleMinAngle, particleMaxAngle);
                break;
            case 2:
                angle = randomFloat(-particleMinAngle, particleMaxAngle);
                break;
            case 3:
                angle = randomFloat(particleMinAngle, -particleMaxAngle);
                break;
            case 4:
                angle = randomFloat(-particleMinAngle, -particleMaxAngle);
                break;
            } 
        }
        else
        {
            int random = GetRandomValue(1, 4);
            switch (random)
            {
            case 1:
                angle = randomFloat(particleMinAngle, particleMaxAngle);
                break;
            case 2:
                angle = randomFloat(-particleMinAngle, particleMaxAngle);
                break;
            case 3:
                angle = randomFloat(particleMinAngle, -particleMaxAngle);
                break;
            case 4:
                angle = randomFloat(-particleMinAngle, -particleMaxAngle);
                break;
            } 
        }
        bool isAlive = true;

        Particle particle = {
            lifeTime,
            speed,
            pos,
            size,
            color,
            angle,
            isAlive,
            0.0f
        };

        particles.push_back(particle);
    }
}


void ParticleCollisionWithTiles()
{
    for (int k = 0; k < particles.size(); k++)
    {
        for (int i = 0; i < TILESROW; i++)
        {
            for (int j = 0; j < TILESCOL; j++)
            {
                if (tiles[i][j].isAlive)
                {
                    Vector2 tilePos = tiles[i][j].pos;

                    Rectangle rect1, rect2;
                    rect1.x = particles[k].pos.x;
                    rect1.y = particles[k].pos.y;
                    rect1.width = particles[k].size.w;
                    rect1.height = particles[k].size.h;

                    rect2.x = tilePos.x;
                    rect2.y = tilePos.y;
                    rect2.width = tileWidth;
                    rect2.height = tileHeight;

                    float newparticlesPosX = particles[k].pos.x + particles[k].speed.x * GetFrameTime() * cosf(particles[k].angle * DEG2RAD);
                    rect1.x = newparticlesPosX;
                    if(checkIntersection(rect1,rect2) &&
                    ((particles[k].pos.x + particles[k].size.w > tilePos.x) ||
                    (particles[k].pos.x < tilePos.x + tileWidth)))
                    {
                        particles[k].speed.x = -particles[k].speed.x;
                    }


                    float newparticlesPosY = particles[k].pos.y + particles[k].speed.y * GetFrameTime() * sinf(particles[k].angle * DEG2RAD);
                    rect1.y = newparticlesPosY;
                    if(checkIntersection(rect1,rect2) &&
                    ((particles[k].pos.y + particles[k].size.h > tilePos.y) ||
                    (particles[k].pos.y < tilePos.y + tileHeight)))
                    {
                        particles[k].speed.y = -particles[k].speed.y;
                    }
                }
            }
        }
    }
}

//! Particle system functions - ends

// --------------------------------------------------------------------------------------------------------------

//! Animation system functions - starts

bool Animation(float &startingValue,const  float &endingValue, float &progress, const float &timeToDecrease, bool &animating, Color &animColor, string animType)
{
    if(animType == "gamestart")
    {
        progress += GetFrameTime() / timeToDecrease;
        progress = Clamp(progress, 0.0f, 1.0f);
        float currentValue = Lerp(startingValue, endingValue, progress);
        animColor.a = (unsigned char)(currentValue);
        if (progress >= 1.0f)
        {
            progress = 0.0f;
            currentValue = endingValue;
            animating = false;
            return false;
        }

        return true;
    }
    else if(animType == "text")
    {
        progress += GetFrameTime() / timeToDecrease;
        progress = Clamp(progress, 0.0f, 1.0f);
        float currentValue = Lerp(startingValue, endingValue, progress);
        animColor.a = (unsigned char)(currentValue);
        if (progress >= 1.0f)
        {
            progress = 0.0f;
            currentValue = endingValue;
            animating = false;
            return false;
        }

        return true;
    }

    return false;
}

void BlurEffect(float posX, float posY, float width, float height, Color color)
{
    for (int yOffset = -2; yOffset <= 2; ++yOffset)
    {
        for (int xOffset = -2; xOffset <= 2; ++xOffset)
        {
            DrawRectangle(posX + xOffset, posY + yOffset, width, height, Fade(color, 0.1f));
        }
    }
}

void BlurEffect(float posX, float posY, float width, float height, Color color1, Color color2, Color color3, Color color4)
{
    for (int yOffset = -2; yOffset <= 2; ++yOffset)
    {
        for (int xOffset = -2; xOffset <= 2; ++xOffset)
        {
            DrawRectangleGradientEx({posX + xOffset, posY + yOffset, width, height}, Fade(paddleColorFirst, 0.1f), Fade(paddleColorSecond, 0.1f), Fade(paddleColorThird, 0.1f), Fade(paddleColorFourth, 0.1f));
        }
    }
}

void BlurEffect(Font font, const char *text, Vector2 position, float fontSize, float spacing, Color tint)
{
    for (int yOffset = -2; yOffset <= 2; ++yOffset)
    {
        for (int xOffset = -2; xOffset <= 2; ++xOffset)
        {
            DrawTextEx(font,text,{position.x + xOffset, position.y + yOffset}, fontSize, spacing, Fade(tint, 0.1f));
        }
    }
}

//! Animation system functions - ends

// --------------------------------------------------------------------------------------------------------------

//! Game main loop - starts

void UpdateDrawFrame(void)
{
    BeginDrawing();

    ClearBackground(backgroundColor);

    GameState(); // game has started or paused, toggles game state

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
        GameGradientBackground();
        IsGameWon();
        ScreenResized();
        userInput();
        BallReset(); //* BallReset() function is calling BallMovement() function
        BallCollisionWithPaddle();
        BallCollisionWithTiles();
        ParticleCollisionWithTiles();
        RenderScore();
        RenderHighScore();
        Ball();
        Paddle();
        MoveParticles();
        DrawTiles();
        DrawParticles();
        Lives();
        DrawFPS(WIDTH - 75, HEIGHT - 20);
    }
    else
    {
        GameGradientBackground();
        ScreenResized();
        BallReset(); //* BallReset() function is calling BallMovement() function
        RenderScore();
        RenderHighScore();
        Ball();
        Paddle();
        MoveParticles();
        DrawTiles();
        DrawParticles();
        Lives();
        DrawFPS(WIDTH - 75, HEIGHT - 20);
        GameStartScreen();
    }

    EndDrawing();
    elapsedTime = GetFrameTime();
    ballelapsedTime = GetFrameTime();
}

//! Game main loop - ends