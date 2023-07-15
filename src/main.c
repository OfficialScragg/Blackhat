#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "raylib.h"

// Constants
#define SCREEN_WIDTH 720
#define SCREEN_HEIGHT 480
#define TITLE "Blackhat"
#define FPS 60

// Forward Definitions
void spawnBox();

int main(){
	printf("---Blackhat---\n");
	// Creating a window
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE);
	if(!IsWindowReady()){
		printf("Error: Window initialisation failed.");
		return 1;
	}
	// Initialise Audio
	InitAudioDevice();
	if(!IsAudioDeviceReady()){
		printf("Error: Audio device initialisation failed.");
		return 1;
	}
	// Load Sounds	
	Sound blip = LoadSound("assets/sounds/jump.wav");
	Sound victory = LoadSound("assets/sounds/victory.wav");
	// Game Options
	SetTargetFPS(FPS);
	// Camera
	Camera2D camera = { 0 };
	camera.target = (Vector2){ 335, 215 };
	camera.offset = (Vector2){ SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f };
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;
	// Objects
	int platform_count = 1;
	Rectangle platforms[10] = {
		{ 1000, 200, 250, 50 },
		{ 500, 0, 250, 50 },
		{ 1000, -200, 250, 50 },
		{ 1500, -400, 250, 50 },
		{ 2000, -600, 250, 50 }
	};	
	Rectangle ground = { SCREEN_WIDTH-5000, SCREEN_HEIGHT, 10000, 500 };
	Rectangle rec = { 335, 215, 50, 50 };
	Rectangle platform = { 1000, 200, 250, 50};
	float speed = 15.0f;
	float max_speed = 20.0f;
	Vector2 velocity = { 0.0f, 0.0f };
	float acceleration = 0.0f;
	float gravity = 15.0f;
	float friction = 3.0f;
	float jump = -8.0f;
	bool grounded = true;
	bool moving = false;
	bool finished = false;
	bool played = false;
	double start = GetTime();
	double end = 0;
	// Game Loop
	while (!WindowShouldClose()){
		// Handle Input
		if(IsKeyDown(KEY_RIGHT)){
			velocity.x += (speed*GetFrameTime());
			moving = true;
		}else if(velocity.x > 0){
			velocity.x -= (speed*GetFrameTime()*friction);
			moving = false;
		}
		if(IsKeyDown(KEY_LEFT)){
			velocity.x -= (speed*GetFrameTime());
			moving = true;
		}else if(velocity.x < 0){
			velocity.x += (speed*GetFrameTime()*friction);
			moving = false;
		}
		if(IsKeyDown(KEY_SPACE) & grounded){
			PlaySound(blip);
			velocity.y = jump;
		}

		// Physics
		velocity.y += (acceleration*GetFrameTime());
		
		if(velocity.x < (speed*GetFrameTime()*friction) & velocity.x > -(speed*GetFrameTime()*friction) & !moving){
			velocity.x = 0.0f;
		}
		if(!grounded){
			acceleration += (gravity*GetFrameTime());
		}else{
			acceleration = 0.0f;
		}

		// Movement
		if(velocity.x > max_speed){
			velocity.x = max_speed;
		}else if(-velocity.x > max_speed){
			velocity.x = -max_speed;
		}
		rec.x += velocity.x;
		rec.y += velocity.y;
		camera.target = (Vector2){ rec.x , rec.y };

		// Collisions
		// Ground
		if(rec.y >= SCREEN_HEIGHT-rec.height){
			rec.y = SCREEN_HEIGHT-rec.height;
			grounded = true;
		}else{
			grounded = false;
		}

		// Platform
		for(int i=0; i< platform_count; i++){
			if(CheckCollisionRecs(rec, platforms[i])){
				if(rec.y-(rec.height/2) < platforms[i].y){
					rec.y = platforms[i].y-rec.height;
					grounded = true;
					if(i == 4 & !finished){
						finished = true;
						end = GetTime();
					}else if(i == platform_count-1){
						platform_count+=1;
					}
				}else if(rec.y > platforms[i].y){
					rec.y = platforms[i].y+platforms[i].height;
					acceleration = 0.0f;
					velocity.y = 0.0f;
				}
			}
		}
		
		// Prepare Canvas
		BeginDrawing();
		// Background
		ClearBackground(BLACK);
		// FPS Counter
		DrawFPS(25, 25);
		// Camera
		BeginMode2D(camera);
		// Rectangle
		for(int i=0;i<platform_count;i++){
			DrawRectangleRec(platforms[i], DARKGRAY);
		}
		DrawRectangleRec(rec, GREEN);
		DrawRectangleRec(ground, DARKGRAY);
		// End Camera
		EndMode2D();
		// Finished Text
		if(finished){
			DrawText("You Win!", SCREEN_WIDTH/2-100, SCREEN_HEIGHT/2-100, 60, GREEN);
			if(!played){
				PlaySound(victory);
				played = true;
				printf("END: %f\n", end);
				printf("START: %f\n", start);
				printf("Time: %fsec\n", end-start);
			}	
		}
		// Draw to screen
		EndDrawing();
		
	}
	CloseWindow();
	return 0;
}
