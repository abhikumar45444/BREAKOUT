# BREAKOUT GAME IN C++ USING RAYLIB

 Breakout is an arcade game in which the player controls a paddle on the screen knocking the ball back and forth breaking the bricks.
 
 ![thumbnail](./Breakout.png)

## DESCRIPTION
 A player moves the paddle back and forth horizontally to hit and ricochet the ball to cause it to break the bricks on top of the screen.
 If the ball reaches the bottom of the screen it costs one life.
 The bricks are in color of yellow, green, orange, and red from bottom to top.

## PLAY (WINDOWS - EXE)
 Run the [breakout.exe](./build/) file.

## BUILD: If you want to make changes feel free to do so
***
Open Makefile
Make necessary adjustments like Compiler for raylib c++ <pre><code>[raylib github windows setup](https://github.com/raysan5/raylib/wiki/Working-on-Windows)</code></pre>.</br>
You can find raylib library in <pre><code>[library folder](./lib/)</code></pre> or else you need to compile the library by yourself.
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
