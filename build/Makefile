# build the program with mingw64 version g++ for cpp file or gcc for c file
# C:\mingw-64\mingw32\bin\g++
# For Platform WEB Build Follow emscripten instruction to compile raylib for web and setup then customize the makefile according to your system path.
DESKTOP:
	C:\mingw-64\mingw32\bin\g++ ../src/main.cpp -g -o breakout.exe -O3 -I../include/ -L../lib/ -lraylib -lopengl32 -lgdi32 -lwinmm && breakout.exe

WASM:
	emcc ../wasm/main.cpp -o ../wasm/index.html -O3 -Wall -std=c++17 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result -Os -I. -I C:/raylib/src -I C:/raylib/src/external -L. -L C:/raylib/src -s USE_GLFW=3 -s ASYNCIFY -s TOTAL_MEMORY=67108864 -s FORCE_FILESYSTEM=1 --preload-file ../resources --shell-file ../wasm/shell.html c:/raylib/src/web/libraylib.a -DPLATFORM_WEB -s EXPORTED_FUNCTIONS=["_free","_malloc","_main"] -s EXPORTED_RUNTIME_METHODS=ccall