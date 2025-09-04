#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "raylib.h"

// Constants
#define INIT_SCREEN_HEIGHT 480
#define INIT_SCREEN_WIDTH 720
#define TITLE "Blackhat"

// Structs
typedef struct Player {
    Rectangle rect;
    bool blocked[4];
    float speed;
    float health;
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
void updateWindowSize();
void updatePlayerCollisions(Tile t);
void resetPlayerCollisions();

// Globals
Camera2D camera = { 0 };
Tile tiles[100000];
Texture2D terrain_textures;
int tile_count = 0;
int SCREEN_HEIGHT = INIT_SCREEN_HEIGHT;
int SCREEN_WIDTH = INIT_SCREEN_WIDTH;
Player player = {
    { INIT_SCREEN_WIDTH/2-424, INIT_SCREEN_HEIGHT/2-400, 16, 16 },
    { false, false, false, false },
    0.5f,
    100.0f
};

// Main function
int main(void){
    init();
    while(!WindowShouldClose()){
        updateWindowSize();
        resetPlayerCollisions();
        updateCanvas();
        playerMovement();
    }
    return 0;
}
// Update Player Collisions States
void resetPlayerCollisions(){
    player.blocked[0] = false;
    player.blocked[1] = false;
    player.blocked[2] = false;
    player.blocked[3] = false;
}

// Update Player Collisions States (Sketchy, should redo it)
void updatePlayerCollisions(Tile t){
    // Check if Water Tile
    if (t.type == "water"){
        // Check if aligned
        if ((t.rect.x < player.rect.x && t.rect.x+16 > player.rect.x) || (t.rect.x < player.rect.x+16 && t.rect.x+16 > player.rect.x+16)){
            if ((t.rect.y <= player.rect.y+16) && (t.rect.y - player.rect.y <= 16)){
                // Blocked above
                player.blocked[0] = true;
            } else if ((t.rect.y+16 <= player.rect.y) && (player.rect.y - t.rect.y <= 16)){
                // Blocked below
                player.blocked[1] = true;
            }
        }else if ((t.rect.y < player.rect.y && t.rect.y+16 > player.rect.y) || (t.rect.y < player.rect.y+16 && t.rect.y+16 > player.rect.y+16)){
            if ((t.rect.x+16 <= player.rect.x) && (player.rect.x - t.rect.x <= 16)){
                // Blocked left
                player.blocked[2] = true;
            } else if ((t.rect.x <= player.rect.x+16) && (t.rect.x - player.rect.x <= 16)){
                // Blocked right
                player.blocked[3] = true;
            }
        }
    }
}

// Player movement
void playerMovement(){
    // Update Player Position
    if(!player.blocked[0] && (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))) player.rect.y -= player.speed;
    if(!player.blocked[1] && (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))) player.rect.y += player.speed;
    if(!player.blocked[2] && (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))) player.rect.x -= player.speed;
    if(!player.blocked[3] && (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))) player.rect.x += player.speed;
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
        // Update Player Collisions
        updatePlayerCollisions(tiles[t]);
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

// Update Elements Relative to Camera
void updateRelativeToCamera(){
    camera.target = (Vector2){player.rect.x, player.rect.y};
    BeginMode2D(camera);
    drawMap();
    DrawRectangleRec(player.rect, BLACK);
    EndMode2D();
}

// Update Window Size
void updateWindowSize(){
    if(IsWindowResized()){
        SCREEN_HEIGHT = GetScreenHeight();
        SCREEN_WIDTH = GetScreenWidth();
        camera.offset = (Vector2){ SCREEN_WIDTH/2-16, SCREEN_HEIGHT/2-16 };
    }
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
    // Init window
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
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
    camera.offset = (Vector2){ SCREEN_WIDTH/2-16, SCREEN_HEIGHT/2-16 };
    camera.rotation = 0.0f;
    camera.zoom = 2.0f;
    // Load map data
    prepareTerrain();
}
