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
    float rotation;
} Tile;
typedef struct UITile {
    Rectangle rect;
    Sprite sprite;
} UITile;

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
Tile layer_one[10000];
int tile_count = 10000;
Sprite sprites[1000];
int sprite_count = 0;
Texture2D terrain_textures_one;
Texture2D terrain_textures_two;
Texture2D terrain_textures_three;
Texture2D terrain_textures_four;
int selected_sprite = 4;
UITile ui_tiles[1000];
int ui_tile_count = 0;
float rotation = 0.0f;
int layer = 0;

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
        if(CheckCollisionPointRec(GetMousePosition(), (Rectangle){SCREEN_WIDTH-64.0f, 0.0f, 64.0f, SCREEN_HEIGHT})){
            for(int i=0;i<ui_tile_count;i++){
                ui_tiles[i].rect.y += wheel*50.0f;
            }
        }else{
            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
            //camera.offset = GetMousePosition();
            const float zoomIncrement = 0.1f;
            camera.zoom += (wheel*zoomIncrement);
            if (camera.zoom < zoomIncrement) camera.zoom = zoomIncrement;
        }
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
}

// Draw Map
void drawMap(){
    // Draw the terrain
    drawTerrain();
    // Draw Mouse Grid
    drawGrid();
}

void drawTerrain(){
    // Layer Zero - Terrain
    for(int t=0;t<tile_count;t++){
        if(tiles[t].sprite.type != NULL){
            DrawTexturePro(tiles[t].sprite.texture_map, tiles[t].sprite.rect, (Rectangle){ tiles[t].position.x+16.0f, tiles[t].position.y+16.0f, 32.0f, 32.0f }, (Vector2){ 16.0f, 16.0f }, tiles[t].rotation, tiles[t].color);
        }else{
            DrawRectangle(tiles[t].position.x, tiles[t].position.y, 32, 32, tiles[t].color);
        }
        
    }
    // Layer one
    for(int t=0;t<tile_count;t++){
        if(layer_one[t].sprite.type != NULL){
            DrawTexturePro(layer_one[t].sprite.texture_map, layer_one[t].sprite.rect, (Rectangle){ layer_one[t].position.x+16.0f, layer_one[t].position.y+16.0f, 32.0f, 32.0f }, (Vector2){ 16.0f, 16.0f }, layer_one[t].rotation, layer_one[t].color);
        }else{
            DrawRectangle(layer_one[t].position.x, layer_one[t].position.y, 32, 32, layer_one[t].color);
        }
        
    }
}

// Prepare map terrain tiles
void loadTextureMaps(){
    // Prepare textures
    terrain_textures_one = LoadTexture("assets/images/modern-ext/32px+VXACE/Modern_Outside_A1.png");
    terrain_textures_two = LoadTexture("assets/images/modern-ext/32px+VXACE/Modern_Outside_A2.png");
    terrain_textures_three = LoadTexture("assets/images/modern-ext/32px+VXACE/Modern_Outside_A3.png");
    terrain_textures_four = LoadTexture("assets/images/modern-ext/32px+VXACE/Modern_Outside_A4.png");
    // Read map data from file and create tiles
    for(int i=0;i<4;i++){
        int row = 0;
        int col = 0;
        int sprite_size = 32;
        int num_rows = 12;
        int num_cols = 16;
        
        for(int y=0;y<num_rows;y++){
            row += sprite_size;
            col = 0;
            for(int x=0;x<num_cols;x++){
                if(i == 0){
                    sprites[sprite_count].texture_map = terrain_textures_one;
                }else if(i == 1){
                    sprites[sprite_count].texture_map = terrain_textures_two;
                }else if(i == 2){
                    sprites[sprite_count].texture_map = terrain_textures_three;
                }else{
                    sprites[sprite_count].texture_map = terrain_textures_four;
                }
                sprites[sprite_count].rect = (Rectangle){row, col, sprite_size, sprite_size};
                sprites[sprite_count].color = WHITE;
                sprites[sprite_count].type = "terrain";
                col += sprite_size;
                sprite_count++;
            }
        }
    }
}

// Draw Map Grid
void drawGrid(){
    float mouseX = (GetMousePosition().x/camera.zoom)+camera.target.x;
    float mouseY = (GetMousePosition().y/camera.zoom)+camera.target.y;
    int tileX = ((int)(mouseX / 32)*32);
    int tileY = ((int)(mouseY / 32)*32);
    DrawRectangleLinesEx((Rectangle){-1600, -1600, 3200, 3200}, 5, RED);
    DrawTexturePro(sprites[selected_sprite].texture_map, sprites[selected_sprite].rect, (Rectangle){tileX+16.0f, tileY+16.0f, 32, 32}, (Vector2){ 16.0f, 16.0f }, rotation, WHITE);
    DrawRectangleLinesEx((Rectangle){tileX, tileY, 32, 32}, 3, WHITE);
    if(IsKeyPressed(KEY_RIGHT)){
        rotation += 90.0f;
        if(rotation == 360.0f) rotation = 0.0f;
    }
    if(IsKeyPressed(KEY_LEFT)){
        rotation -= 90.0f;
        if(rotation == -360.0f) rotation = 0.0f;
    }
    if(IsKeyPressed(KEY_UP)){
        layer++;
    }
    if(IsKeyPressed(KEY_DOWN)){
        layer--;
        if(layer < 0) layer = 0;
    }
    if(IsMouseButtonDown(0)){
        if(CheckCollisionPointRec(GetMousePosition(), (Rectangle){SCREEN_WIDTH-64.0f, 0.0f, 64.0f, SCREEN_HEIGHT})){
            if(IsMouseButtonPressed(0)){
                for(int i=0;i<ui_tile_count;i++){
                    if(CheckCollisionPointRec(GetMousePosition(), ui_tiles[i].rect)){
                        selected_sprite = i;
                    }
                }
            }
        }else{
            if(layer == 0){
                for(int t=0;t<tile_count;t++){
                    if(tiles[t].position.x == tileX & tiles[t].position.y == tileY){
                        tiles[t].sprite = sprites[selected_sprite];
                        tiles[t].color = WHITE;
                        tiles[t].rotation = rotation;
                    }
                }
            }else{
                for(int t=0;t<tile_count;t++){
                    if(layer_one[t].position.x == tileX & layer_one[t].position.y == tileY){
                        layer_one[t].sprite = sprites[selected_sprite];
                        layer_one[t].color = WHITE;
                        layer_one[t].rotation = rotation;
                    }
                }
            }
        }
    }
}

// Update HUD Elements
void updateHUD(){
    DrawFPS(25, 25);
    DrawRectangle(SCREEN_WIDTH-64.0f, 0.0f, 64.0f, SCREEN_HEIGHT, DARKGRAY);

    for(int i=0;i<ui_tile_count;i++){
        DrawTextureRec(ui_tiles[i].sprite.texture_map, ui_tiles[i].sprite.rect, (Vector2){ui_tiles[i].rect.x, ui_tiles[i].rect.y}, WHITE);
    }
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
        tiles[t].color = (Color){10.0f, 10.0f, 10.0f, 255.0f};
        tiles[t].type = "terrain";
        layer_one[t].position = (Vector2){col, row};
        layer_one[t].color = (Color){255.0f, 255.0f, 255.0f, 0.0f};
        layer_one[t].type = "layer-1";
        col+=32;
    }
    // Setup UI Tiles
    for(int i=0;i<sprite_count;i++){
        ui_tiles[ui_tile_count].rect = (Rectangle){SCREEN_WIDTH-48, 16.0f+(i*48), 32, 32};
        ui_tiles[ui_tile_count].sprite = sprites[i];
        ui_tile_count++;
    }
}
