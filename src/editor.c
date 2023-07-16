#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "raylib.h"

// Constants
#define SCREEN_HEIGHT 720
#define SCREEN_WIDTH 1080
#define TITLE "Blackhat - Editor"

// Structs
typedef struct Player {
    Rectangle rect;
    float speed;
    float health;
    char* direction;
} Player;
typedef struct Sprite {
    Rectangle rect;
    Texture2D texture_map;
    Color color;
    char* type;
} Sprite;
typedef struct Tile {
    Vector2 position;
    Sprite sprite;
    Color color;
    char* type;
} Tile;

// Forward Definition
void init();
void updateCanvas();
void updateHUD();
void updateRelativeToCamera();
void drawMap();
void loadTextureMaps();
void drawGrid();
void drawTerrain();
void mouseMovement();

// Globals
Camera2D camera = { 0 };
Tile tiles[10000];
int tile_count = 10000;
Sprite sprites[1000];
int sprite_count = 0;
Texture2D terrain_textures;

// Main function
int main(void){
    init();
    while(!WindowShouldClose()){
        mouseMovement();
        updateCanvas();
    }
    return 0;
}

void mouseMovement(){
    float wheel = GetMouseWheelMove();
    if (wheel != 0)
    {
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
        //camera.offset = GetMousePosition();
        const float zoomIncrement = 0.1f;
        camera.zoom += (wheel*zoomIncrement);
        if (camera.zoom < zoomIncrement) camera.zoom = zoomIncrement;
    }
    if(IsMouseButtonPressed(2)){
        camera.zoom = 1.0f;
    }
    if(IsMouseButtonDown(1)){
        float mouseX = GetMousePosition().x-SCREEN_WIDTH/2;
        float mouseY = GetMousePosition().y-SCREEN_HEIGHT/2;
        camera.target = (Vector2){ camera.target.x+mouseX/50, camera.target.y+mouseY/50 };
        camera.offset = (Vector2){ 0.0f, 0.0f };
    }
    printf("MOUSE: %f, %f\n", GetMousePosition().x, GetMousePosition().y);
}

// Draw Map
void drawMap(){
    // Draw the terrain
    drawTerrain();
    // Draw Mouse Grid
    drawGrid();
}

void drawTerrain(){
    for(int t=0;t<tile_count;t++){
        if(tiles[t].sprite.type != NULL){
            DrawTextureRec(tiles[t].sprite.texture_map, tiles[t].sprite.rect, tiles[t].position, tiles[t].color);
        }else{
            DrawRectangle(tiles[t].position.x, tiles[t].position.y, 32, 32, tiles[t].color);
        }
        
    }
}

// Prepare map terrain tiles
void loadTextureMaps(){
    // Prepare textures
    terrain_textures = LoadTexture("assets/images/modern-ext/32px+VXACE/Modern_Outside_A2.png");
    // Read map data from file and create tiles
    int row = 0;
    int col = 0;
    int sprite_size = 32;
    int num_rows = 12;
    int num_cols = 16;
    
    for(int y=0;y<num_rows;y++){
        row += sprite_size;
        col = 0;
        for(int x=0;x<num_cols;x++){
            sprites[sprite_count].texture_map = terrain_textures;
            sprites[sprite_count].rect= (Rectangle){row, col, sprite_size, sprite_size};
            sprites[sprite_count].color = WHITE;
            sprites[sprite_count].type = "terrain";
            col += sprite_size;
            sprite_count++;
        }
    }
}

// Draw Map Grid
void drawGrid(){
    float mouseX = (GetMousePosition().x/camera.zoom)+camera.target.x;
    float mouseY = (GetMousePosition().y/camera.zoom)+camera.target.y;
    printf("GRID MOUSE: %f, %f\n", mouseX, mouseY);
    int tileX = ((int)(mouseX / 32)*32);
    int tileY = ((int)(mouseY / 32)*32);
    DrawRectangleLines(tileX, tileY, 32, 32, WHITE);
    if(IsMouseButtonDown(0)){
        for(int t=0;t<tile_count;t++){
            if(tiles[t].position.x == tileX & tiles[t].position.y == tileY){
                tiles[t].sprite = sprites[0];
                tiles[t].color = WHITE;
            }
        }
    }
}

// Update HUD Elements
void updateHUD(){
    DrawFPS(25, 25);
}

// Update Elements Relative to Camera
void updateRelativeToCamera(){
    BeginMode2D(camera);
    drawMap();
    EndMode2D();
}

// Draw to window
void updateCanvas(){
    BeginDrawing();
    ClearBackground(BLACK);
    updateRelativeToCamera();
    updateHUD();
    EndDrawing();
}

// Initialise program
void init(){
    // Settings
    SetTargetFPS(60);
    // Init window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE);
    if(!IsWindowReady()){
        printf("Error: Window init failed.\n");
        exit(1);
    }
    // Init audio
    InitAudioDevice();
    if(!IsAudioDeviceReady()){
        printf("Error: Audio init failed.\n");
        exit(1);
    }
    // Init camera
    camera.target = (Vector2){ 0.0f, 0.0f };
    camera.offset = (Vector2){ 0.0f, 0.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    // Load map data
    loadTextureMaps();
    // Set Tile Positions
    int num_rows = 100;
    int num_cols = 100;
    int row = -1600;
    int col = -1600;
    for(int t=0;t<tile_count;t++){
        if(col == 1600){
            col = -1600;
            row += 32;
        }
        tiles[t].position = (Vector2){col, row};
        tiles[t].color = BLACK;
        tiles[t].type = "terrain";
        col+=32;
    }
}
