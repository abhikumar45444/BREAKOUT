# BREAKOUT GAME IN C++ USING RAYLIB

 Breakout is an arcade game in which the player controls a paddle on the screen knocking the ball back and forth breaking the bricks.
 
 ![thumbnail](./Breakout.png)

## DESCRIPTION
 A player moves the paddle back and forth horizontally to hit and ricochet the ball to cause it to break the bricks on top of the screen.
 If the ball reaches the bottom of the screen it costs one life.
 The bricks are in color of yellow, green, orange, and red from bottom to top.

## PLAY GAME ON WEB
Checkout Game Here: [BREAKOUT](https://abhikumar45444.github.io/BREAKOUT/).

## PLAY (WINDOWS - EXE)
 Run the [breakout.exe](./build) file.

## BUILD: If you want to make changes feel free to do so
***
Open Makefile
Make necessary adjustments like Compiler for raylib c++ <code>[raylib github windows setup](https://github.com/raysan5/raylib/wiki/Working-on-Windows)</code>.</br>
You can find raylib library in <code>[library folder](./lib)</code> or else you need to compile the library by yourself.
***

## DEPENDENCIES
```console
raylib
opengl32 
gdi32 
winmm
```

## HOW-TO-PLAY
|keys|description|
|---|---|
|Player|
|<kbd>←</kbd>/<kbd>→</kbd>|move Paddle LEFT/RIGHT|
|<kbd>SPACE</kbd>|Pause/Play|

## REFERENCES
[Collision Detection - Axis Aligned Bounding Box (AABB)](https://medium.com/@hemalatha.psna/collision-detection-in-javascript-efafe8bba2c0) </br>
[Breakout Game Wiki](https://en.wikipedia.org/wiki/Breakout_(video_game))