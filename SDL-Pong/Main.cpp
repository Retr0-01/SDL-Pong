#include <iostream>
#include <SDL.h>
#include <SDL_mixer.h>
#include "consts.h"

// Global variables
int game_is_running = false;
int last_frame_time = 0;
float delta_time = 0;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
Mix_Chunk* hit_sound;

// Our core game object struct
struct game_object {
	float x;
	float y;
	float width;
	float height;
	float vel_x;
	float vel_y;
} ball, paddle;
typedef struct game_object GameObject;

typedef enum {
	CHANNEL_NONE,
	CHANNEL_GENERIC,
	CHANNEL_BALL,
	CHANNEL_GAME_EVENT
} SoundChannels;

/// Forward declerations
int Initialize_Window(void);
void Destroy_Window(void);

void Setup(void);
void Process_Input(void);
void Update(void);
void Render(void);

void MoveBall(void);
///

// Showtime.
int main(int argc, char* args[])
{
	game_is_running = Initialize_Window();

	Setup();

	while (game_is_running)
	{
		Process_Input();
		Update();
		Render();
	}

	Destroy_Window();

	return 0;
}

// Initialize SDL, our window, the renderer and any extensions.
int Initialize_Window(void)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fprintf(stderr, "Error initializing SDL.\n");
		return false;
	}

	window = SDL_CreateWindow(
		"SDL Ping-Pong",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		SDL_WINDOW_FULLSCREEN_DESKTOP
	);
	if (!window)
	{
		fprintf(stderr, "Error creating SDL Window.\n");
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	if (!renderer)
	{
		fprintf(stderr, "Error creating SDL Renderer.\n");
		return false;
	}

	// Initialize SDL_mixer with our audio format.
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0)
	{
		fprintf(stderr, "Error creating SDL Mixer.\n");
		return false;
	}

	return true;
}

// This function runs once at the start of the program.
void Setup(void)
{
	// Initialize the ball object moving down at a constant velocity
	ball.x = 10;
	ball.y = 20;
	ball.width = 20;
	ball.height = 20;
	ball.vel_x = 180;
	ball.vel_y = 140;

	// Cache sounds.
	hit_sound = Mix_LoadWAV("ball_hit.wav");
}

// Poll SDL events and process keyboard input.
void Process_Input(void)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			game_is_running = false;
			break;
		case SDL_KEYDOWN:

			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				game_is_running = false;
			default:
				break;
			}

			break;
		}
	}
}

// Update function with a fixed time step
void Update(void)
{
	// Get delta_time factor converted to seconds to be used to update objects.
	delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0;

	// Store the milliseconds of the current frame to be used in the next one
	last_frame_time = SDL_GetTicks();

	MoveBall();
}

void MoveBall(void)
{
	// Move ball as a function of delta time
	ball.x += ball.vel_x * delta_time;
	ball.y += ball.vel_y * delta_time;

	// Check for ball collision with the window borders
	if (ball.x < 0) {
		ball.x = 0;
		ball.vel_x = -ball.vel_x;
		Mix_PlayChannel(CHANNEL_BALL, hit_sound, false);
	}
	if (ball.x + ball.height > WINDOW_WIDTH) {
		ball.x = WINDOW_WIDTH - ball.width;
		ball.vel_x = -ball.vel_x;
		Mix_PlayChannel(CHANNEL_BALL, hit_sound, false);
	}
	if (ball.y < 0) {
		ball.y = 0;
		ball.vel_y = -ball.vel_y;
		Mix_PlayChannel(CHANNEL_BALL, hit_sound, false);
	}
	if (ball.y + ball.height > WINDOW_HEIGHT) {
		ball.y = WINDOW_HEIGHT - ball.height;
		ball.vel_y = -ball.vel_y;
		Mix_PlayChannel(CHANNEL_BALL, hit_sound, false);
	}

}

// Render function to draw game objects in the SDL window.
void Render(void)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	// Draw a rectangle for the ball object
	SDL_Rect ball_rect = {
		(int)ball.x,
		(int)ball.y,
		(int)ball.width,
		(int)ball.height
	};
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(renderer, &ball_rect);

	SDL_RenderPresent(renderer);
}

// Function to destroy SDL window and renderer
void Destroy_Window(void)
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
