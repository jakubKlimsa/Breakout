#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "sdl.h"

#define blocks_cols 17
#define blocks_rows 4

typedef struct{ 
  int size;
  int dir;
  SDL_Point pos;
} Paddle;

typedef struct{ 
  int lives;
  SDL_Point dir;
  SDL_Point pos;
} Ball;

typedef struct{
  int rows;
  int cols;
  int tile_size;
  int score;
  Paddle paddle;
  Ball ball; 
} Game;

typedef struct{
  int r;
  int g;
  int b;
} Line;

typedef struct{
  int lives;
  SDL_Point pos;
} Block;

typedef struct{
  Line line[blocks_rows];
  Block block[blocks_rows * blocks_cols];
} Blocks;

void print_playfield (Game *game, SDL_Context *ctx);
void print_paddle (Game *game, SDL_Context *ctx);
void print_ball (Game *game, SDL_Context *ctx);
void ball_change_dir (Game *game);
void ball_lose_life (Game *game);
void load_blocks (Game *game ,Blocks *blocks, char *path);
void print_blocks (Game *game ,Blocks *blocks, SDL_Context *ctx);
void ball_break_blocks (Game *game, Blocks *blocks);
void reset_game(Game *game);