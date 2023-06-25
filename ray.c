#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define SCREEN_HEIGHT 480
#define SCREEN_WIDTH (SCREEN_HEIGHT * 2)
#define MAP_SIZE 8
#define BLOCK_SIZE (SCREEN_HEIGHT / MAP_SIZE)
#define MAX_DEPTH (MAP_SIZE * BLOCK_SIZE)
#define FOV (M_PI / 3)
#define HALF_FOV (FOV / 2)
#define NUMBER_RAYS 120
#define STEP_ANGLE (FOV / NUMBER_RAYS)
#define RAY_WIDTH (SCREEN_HEIGHT / NUMBER_RAYS)
#define CONS 20000

typedef struct {
    double x;
    double y;
    double angle;
} Player;

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
} State;

// Map grid
const char* MAP = "########"
                  "#    # #"
                  "# #    #"
                  "#    # #"
                  "###  # #"
                  "# #    #"
                  "#      #"
                  "########";

// SDL initialization
int initSDL(State* state) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    state->window = SDL_CreateWindow("Raycasting", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (state->window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }

    state->renderer = SDL_CreateRenderer(state->window, -1, SDL_RENDERER_ACCELERATED);
    if (state->renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(state->window);
        SDL_Quit();
        return 0;
    }

    return 1;
}

// SDL cleanup
void quitSDL(State* state) {
    SDL_DestroyRenderer(state->renderer);
    SDL_DestroyWindow(state->window);
    SDL_Quit();
}

// Render map
void drawMap(State* state, const Player* player) {
    // Loop over map rows
    for (int row = 0; row < MAP_SIZE; row++) {
        // Loop over map columns
        for (int col = 0; col < MAP_SIZE; col++) {
            // calculate block index
            int index = row * MAP_SIZE + col;

            // Set block properties
            SDL_Rect rect = { col * BLOCK_SIZE, row * BLOCK_SIZE, BLOCK_SIZE - 2, BLOCK_SIZE - 2 };

            // Block color selection
            if (MAP[index] == '#') {
                SDL_SetRenderDrawColor(state->renderer, 255, 255, 255, 255);
            } else {
                SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 255);
            }
            // Render map
            SDL_RenderFillRect(state->renderer, &rect);
        }
    }

    // Draw player on 2D board
    SDL_SetRenderDrawColor(state->renderer, 0, 255, 0, 255);
    SDL_RenderDrawPoint(state->renderer, (int)player->x, (int)player->y);
}

void rayCast(State* state, const Player* player) {
    // Define the angle of the rightmost ray
    double initial_angle = player->angle - HALF_FOV;

    // Loop over cast rays
    for (int ray = 0; ray < NUMBER_RAYS; ray++) {
        // Cast ray incrementally
        for (int depth = 0; depth < MAX_DEPTH; depth++) {
            // Register hit coordinates
            double hit_x = player->x + cos(initial_angle) * depth;
            double hit_y = player->y - sin(initial_angle) * depth;

            // Locate grid coordinates of hit point
            int col = (int)(hit_x / BLOCK_SIZE);
            int row = (int)(hit_y / BLOCK_SIZE);

            // Calculate index
            int index = row * MAP_SIZE + col;

            // Check for wall collision
            if (MAP[index] == '#') {

                // Highlight hit wall 
                SDL_SetRenderDrawColor(state->renderer, 255, 0, 0, 255);
                SDL_Rect rect = { col * BLOCK_SIZE, row * BLOCK_SIZE, BLOCK_SIZE - 2, BLOCK_SIZE - 2 };
                SDL_RenderFillRect(state->renderer, &rect);

                // Render full ray
                SDL_SetRenderDrawColor(state->renderer, 0, 0, 255, 255);
                SDL_RenderDrawLine(state->renderer, (int)player->x, (int)player->y, (int)hit_x, (int)hit_y);

                // Color shading
                int color = 255 / (1 + depth * depth * 0.0001);

                // Correct fisheye effect
                depth *= cos(player->angle - initial_angle);

                // Calculate wall height using ray depth
                float wall_height = CONS / (depth + 0.00001); // Adding 0.00001 to avoid zero division

                // Render slice
                SDL_SetRenderDrawColor(state->renderer, color, color, color, 255);
                SDL_Rect slice;
                slice.x = SCREEN_HEIGHT + ray * RAY_WIDTH; // Adding SCREEN_HEIGHT to display on right window
                slice.y = (SCREEN_HEIGHT / 2) - wall_height / 2;
                slice.w = RAY_WIDTH;
                slice.h = wall_height;
                SDL_RenderFillRect(state->renderer, &slice);
                break;
            }
        }
        // Update FOV 
        initial_angle += STEP_ANGLE;
    }
}

int checkCollision(float x, float y) {
    // Convert player position to grid coordinates
    int col = (int)(x / BLOCK_SIZE);
    int row = (int)(y / BLOCK_SIZE);

    // Calculate index
    int index = row * MAP_SIZE + col;

    if (MAP[index] == '#') {
        // Collision occurred
        return 1; 
    }

    // No collision
    return 0; 
}


// Handle user input
void userInput(Player* player) {
    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    if (keystates[SDL_SCANCODE_LEFT]) {
        player->angle -= 0.1;
    }
    if (keystates[SDL_SCANCODE_RIGHT]) {
        player->angle += 0.1;
    }
    if (keystates[SDL_SCANCODE_UP]) {
        float update_x = player->x + cos(player->angle) * 5;
        float update_Y = player->y - sin(player->angle) * 5;
        if (!checkCollision(update_x, update_Y)) {
            player->x = update_x;
            player->y = update_Y;
        }
    }
    if (keystates[SDL_SCANCODE_DOWN]) {
        float update_x = player->x - cos(player->angle) * 5;
        float update_y = player->y + sin(player->angle) * 5;
        if (!checkCollision(update_x, update_y)) {
            player->x = update_x;
            player->y = update_y;
        }
    }
}

int main(void) {
    State state;
    Player player;

    if (!initSDL(&state)) {
        return 1;
    }

    // Set initial position
    player.x = SCREEN_WIDTH / 4;
    player.y = SCREEN_WIDTH / 4;
    player.angle = M_PI;

    int quit = 0;
    SDL_Event event;

    while (!quit) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            }
        }

        // Update 2D background
        SDL_SetRenderDrawColor(state.renderer, 0, 0, 0, 255);
        SDL_RenderClear(state.renderer);

        // Render ceiling (top half of "3D" side)
        SDL_SetRenderDrawColor(state.renderer, 135, 206, 235, 255);
        SDL_Rect ceiling = { SCREEN_HEIGHT, 0, SCREEN_HEIGHT, SCREEN_HEIGHT / 2 };
        SDL_RenderFillRect(state.renderer, &ceiling);

        // Render floor (lower half)
        SDL_SetRenderDrawColor(state.renderer, 124, 252, 0, 255);
        SDL_Rect floor = { SCREEN_HEIGHT, SCREEN_HEIGHT / 2, SCREEN_HEIGHT, SCREEN_HEIGHT };
        SDL_RenderFillRect(state.renderer, &floor);

        // Draw 2D map
        drawMap(&state, &player);

        // Draw 3D map
        rayCast(&state, &player);

        // Handle user input
        userInput(&player);

        // Update display
        SDL_RenderPresent(state.renderer);

        // Stabilize render
        SDL_Delay(30);
    }

    quitSDL(&state);

    return 0;
}

