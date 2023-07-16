#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "raylib.h"

// Constants
#define SCREEN_HEIGHT 480
#define SCREEN_WIDTH 720
#define TITLE "Blackhat"

// Structs
typedef struct Player {
    Rectangle rect;
    float speed;
    float health;
    char* direction;
} Player;
typedef struct Tile {
    Rectangle rect;
    Color color;
    char* type;
} Tile;

// Forward Definition
void init();
void updateCanvas();
void updateHUD();
void updateRelativeToCamera();
void playerMovement();
void drawMap();
void prepareTerrain();
void drawObjects();
void drawPlayer();

// Globals
Camera2D camera = { 0 };
Tile tiles[100000];
int tile_count = 0;
Texture2D terrain_textures;
Texture2D player_textures;
Player player = {
    { SCREEN_WIDTH/2-25, SCREEN_HEIGHT/2-25, 16, 16 },
    1.5f,
    100.0f,
    "down"
};

// Main function
int main(void){
    init();
    while(!WindowShouldClose()){
        playerMovement();
        updateCanvas();
    }
    return 0;
}

// Player movement
void playerMovement(){
    if(IsKeyDown(KEY_LEFT)){
        player.rect.x -= player.speed;
        player.direction = "left";
    }else if(IsKeyDown(KEY_RIGHT)){
        player.rect.x += player.speed;
        player.direction = "right";
    }else if(IsKeyDown(KEY_UP)){
        player.rect.y -= player.speed;
        player.direction = "up";
    }else if(IsKeyDown(KEY_DOWN)){
        player.rect.y += player.speed;
        if(player.direction == "up"){
            player.direction = "down";
        }
    }
}

// Draw Map
void drawMap(){
    // Draw the terrain
    for(int t=0; t<tile_count; t++){
        if(tiles[t].type == "grass"){
            DrawTextureRec(terrain_textures, (Rectangle){128, 16, 16, 16}, (Vector2){tiles[t].rect.x, tiles[t].rect.y}, tiles[t].color);
        }else if(tiles[t].type == "water"){
            DrawTextureRec(terrain_textures, (Rectangle){64, 16, 16, 16}, (Vector2){tiles[t].rect.x, tiles[t].rect.y}, tiles[t].color);
        }else if(tiles[t].type == "path"){
            DrawTextureRec(terrain_textures, (Rectangle){224, 96, 16, 16}, (Vector2){tiles[t].rect.x, tiles[t].rect.y}, tiles[t].color);
        }
    }
    drawObjects();
}

// Prepare map terrain tiles
void prepareTerrain(){
    // Prepare textures
    terrain_textures = LoadTexture("assets/images/modern-city-assets/street.png");
    // Read map data from file and create tiles
    int col = -800;
    int row = -800;
    char* terrain_data = LoadFileText("assets/maps/lake-house.txt");
    for(int i=0;i<strlen(terrain_data);i++){
        if(terrain_data[i] == '\n'){
            row += 16;
            col = -800;
            continue;
        }else if(terrain_data[i] == 'G'){
            tiles[tile_count].rect= (Rectangle){SCREEN_WIDTH/2+col, SCREEN_HEIGHT/2+row, 50, 50};
            tiles[tile_count].color = GREEN;
            tiles[tile_count].type = "grass";
            col += 16;
        }else if(terrain_data[i] == 'W'){
            tiles[tile_count].rect= (Rectangle){SCREEN_WIDTH/2+col, SCREEN_HEIGHT/2+row, 50, 50};
            tiles[tile_count].color = BLUE;
            tiles[tile_count].type = "water";
            col += 16;
        }else if(terrain_data[i] == 'P'){
            tiles[tile_count].rect= (Rectangle){SCREEN_WIDTH/2+col, SCREEN_HEIGHT/2+row, 50, 50};
            tiles[tile_count].color = BROWN;
            tiles[tile_count].type = "path";
            col += 16;
        }
        tile_count += 1;
    }
}

// Draw Map
void drawObjects(){

}

// Update HUD Elements
void updateHUD(){
    DrawFPS(25, 25);
}

// Draw player
void drawPlayer(){
    if(player.direction == "up"){
        DrawTextureRec(
            player_textures,
            (Rectangle){ 0, 32, 32, 32 },
            (Vector2){player.rect.x, player.rect.y},
            WHITE
        );
    }else if(player.direction == "down"){
        DrawTextureRec(
            player_textures,
            (Rectangle){ 0, 0, 32, 32 },
            (Vector2){player.rect.x, player.rect.y},
            WHITE
        );
    }else if(player.direction == "left"){
        DrawTextureRec(
            player_textures,
            (Rectangle){ 32, 0, 32, 32 },
            (Vector2){player.rect.x, player.rect.y},
            WHITE
        );
    }else if(player.direction == "right"){
        DrawTextureRec(
            player_textures,
            (Rectangle){ 0, 0, 32, 32 },
            (Vector2){player.rect.x, player.rect.y},
            WHITE
        );
    }
}

// Update Elements Relative to Camera
void updateRelativeToCamera(){
    camera.target = (Vector2){player.rect.x, player.rect.y};
    BeginMode2D(camera);
    drawMap();
    drawPlayer();
    EndMode2D();
}

// Draw to window
void updateCanvas(){
    BeginDrawing();
    ClearBackground(WHITE);
    updateHUD();
    updateRelativeToCamera(); 
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
    camera.target = (Vector2){player.rect.x, player.rect.y};
    camera.offset = (Vector2){ SCREEN_WIDTH/2, SCREEN_HEIGHT/2 };
    camera.rotation = 0.0f;
    camera.zoom = 1.5f;
    // Load map data
    prepareTerrain();
    // Load textures
    player_textures = LoadTexture("assets/images/hacker.png");
}
