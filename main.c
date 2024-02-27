#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_LOGIC_FPS 60

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif


//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------

const int screenWidth  = 800;
const int screenHeight = 800;
const int draw_fps     = 60;
float logic_fps        = 10.0f;

bool isRunning = false;

const int cellSize     = 1;
const int gridWidth    = screenHeight / cellSize;
const int gridHeight   = screenHeight / cellSize;
const int n_cells      = gridWidth * gridHeight;

const Color cellColor  = BLACK;
const Color bgColor    = WHITE;

typedef struct Cell
{
    int x;
    int y;
    int isAlive; // bool
    int nAliveNeig;
} CELL;

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------

static void initializeCells(CELL *cellArray, int n_cells);

static int mtoi(int x, int y);

static void UpdateCells(CELL *cellArray, int n_cells);           // Update all cells in cellArray
static void DrawCells(const CELL *cellArray, int n_cells);       // Draws all cells in cellArray

    //----------------------------------------------------------------------------------
    // Main entry point
    //----------------------------------------------------------------------------------

    int
    main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    CELL *cellArray = calloc(n_cells, sizeof(CELL));
    InitWindow(screenWidth, screenHeight, "Game of Life");

    initializeCells(cellArray, n_cells);
    //--------------------------------------------------------------------------------------

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, fps, 1);
#else
    SetTargetFPS(draw_fps);               // Set our game maximum FPS
    //--------------------------------------------------------------------------------------
    float acc;

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        if(isRunning){
            acc += GetFrameTime();
            if (acc > 1.0f/logic_fps){
                acc -= 1.0f/logic_fps;
                UpdateCells(cellArray, n_cells);
            }

        }

        if (IsKeyPressed(KEY_SPACE)) isRunning = !isRunning;
        if (IsKeyPressedRepeat(KEY_LEFT)) {
            logic_fps --;
            logic_fps = logic_fps < 1 ? 1 : logic_fps;
        }
        if (IsKeyPressedRepeat(KEY_RIGHT))
        {
            logic_fps++;
            logic_fps = logic_fps > MAX_LOGIC_FPS ? MAX_LOGIC_FPS : logic_fps;
        }
        if (IsKeyPressed(KEY_R)) initializeCells(cellArray, n_cells);

        BeginDrawing();

        ClearBackground(bgColor);
        DrawCells(cellArray, n_cells);
        DrawFPS(20, 20);

        EndDrawing();
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();                  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
}

static int mtoi(int x, int y){
    if (x < 0 || y < 0 || x >= gridWidth || y >= gridHeight)  return -1;
    return y * gridWidth + x;
}

static void UpdateCells(CELL *cellArray, int n_cells){
    for (int i = 0; i < n_cells; i++){
        if (!cellArray[i].isAlive) continue;

        int x = cellArray[i].x;
        int y = cellArray[i].y;

        int neighbours[8] = {
                mtoi(x+1, y  ),
                mtoi(x-1, y  ),
                mtoi(x,   y+1),
                mtoi(x+1, y+1),
                mtoi(x-1, y+1),
                mtoi(x,   y-1),
                mtoi(x+1, y-1),
                mtoi(x-1, y-1)
            };

        for (int i = 0; i < 8; i++) {
            if ( neighbours[i] < 0 ) continue;
            cellArray[neighbours[i]].nAliveNeig++;
        }
    }

    for (int i = 0; i < n_cells; i++)
    {
        CELL *c = &cellArray[i];
        int nAliveNeig = c->nAliveNeig;
        c->nAliveNeig = 0;
        if (nAliveNeig > 3){
            c->isAlive = 0;
            continue;
        }
        if (nAliveNeig < 2){
            c->isAlive = 0;
            continue;
        }
        if (nAliveNeig == 3) {
            c->isAlive = 1; 
            continue;
        }
    }
}

static void DrawCells(const CELL *cellArray, int n_cells)
{
    for (int i = 0; i < n_cells; i++)
    {
        CELL c = cellArray[i];
        if (c.isAlive) DrawRectangle(c.x * cellSize, c.y * cellSize, cellSize, cellSize, cellColor);
    }
}

static void initializeCells(CELL *cellArray, int n_cells)
{
    for (int y = 0; y < gridHeight; y++){
        for (int x = 0; x < gridWidth; x++)
        {
            int index = mtoi(x, y);
            cellArray[index].x = x;
            cellArray[index].y = y;
            cellArray[index].isAlive = GetRandomValue(0,1);
            cellArray[index].nAliveNeig = 0;
        }
    }
}
