#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "SDL.h"

typedef struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
} sdl_t;

typedef struct {
	uint32_t window_width;
	uint32_t window_height;
	uint32_t fg_color;
	uint32_t bg_color;
	uint32_t scale_factor;
} config_t;

typedef enum {
	QUIT,
	RUNNING,
	PAUSED,

} emulator_state_t;

typedef struct {
	emulator_state_t state;
} chip8_t;

// Initialize SDL
bool init_sdl(sdl_t *sdl, const config_t config) {
	if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0 ) {
		SDL_Log("Could not initialize SDL subsystems! %s\n", SDL_GetError()); 
		return false;
	}

	sdl->window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, config.window_width * config.scale_factor, config.window_height * config.scale_factor, 0);
	if(!sdl->window) {
		SDL_Log("Could not create SDL window %s\n", SDL_GetError());
		return false;
	}

	sdl->renderer = SDL_CreateRenderer(sdl->window, -1, SDL_RENDERER_ACCELERATED);
	if(!sdl->renderer) {
		SDL_Log("Could not create SDL Renderer %s\n", SDL_GetError());
		return false;
	}

	return true;
}



// Set up initial emulator  configuration
bool set_config_from_args(config_t *config, const int argc, char **argv) {
	
	// Set defaults
	*config = (config_t){
		.window_width = 64,
		.window_height = 32,
		.fg_color = 0xFFFFFFFF,
		.bg_color = 0x00000000,
		.scale_factor = 20,
	};

	// 
	for(int i = 0; i < argc; i++){
		(void)argv[i];
	}

	return true;
}

bool init_chip8(chip8_t *chip8){
	chip8->state = RUNNING;
	return true;
}

// Final cleanup
bool final_cleanup(const sdl_t sdl) {
	SDL_DestroyRenderer(sdl.renderer);
	SDL_DestroyWindow(sdl.window);
	SDL_Quit();

	return true;
}

void clear_screen(const sdl_t sdl, const config_t config) {
	const uint8_t r = (config.bg_color >> 24) & 0xFF;
	const uint8_t g = (config.bg_color >> 16) & 0xFF;
	const uint8_t b = (config.bg_color >>  8) & 0xFF;
	const uint8_t a = (config.bg_color >>  0) & 0xFF;

	SDL_SetRenderDrawColor(sdl.renderer, r, g, b, a);
	SDL_RenderClear(sdl.renderer);
}

void update_screen(const sdl_t sdl){
	SDL_RenderPresent(sdl.renderer);
}

void handle_input(chip8_t *chip8){
	SDL_Event event;

	while(SDL_PollEvent(&event)) {
		switch(event.type){
			case SDL_QUIT:
				chip8->state = QUIT;
				return;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:
						chip8->state = QUIT;
						return;

					default:
						break;

				}
				break;
			case SDL_KEYUP:

				break;
			default:
				break;
		}
	}
}

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;
	
	// Initialize emulator config.
	config_t config = {0};
	if( !set_config_from_args(&config, argc, argv)) exit(EXIT_FAILURE);

	// Initialize SDL
	sdl_t sdl = {0};
	if( !init_sdl(&sdl, config) ) exit(EXIT_FAILURE);
	
	chip8_t chip8 = {0};
	if( !init_chip8(&chip8) ) exit(EXIT_FAILURE);

	// Initial Screen clear to background color
	clear_screen(sdl, config);
	
	SDL_RenderClear(sdl.renderer);
	
	while(chip8.state != QUIT){

		handle_input(&chip8);
		
		SDL_Delay(16);
		update_screen(sdl);
	}

	// Final cleanup
	final_cleanup(sdl);
	
	exit(EXIT_SUCCESS);
}
