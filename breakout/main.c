#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "sdl.h"
#include "breakout.h"

#define blocks_cols 17
#define blocks_rows 4

int main(int argc, char **argv) {
  int width = 672;
  int height = 800;
  srand(time(NULL));

  if(argc >= 2) {
    if(sscanf(argv[1], "%dx%d", &width, &height) != 2) {
      fprintf(stderr, "Usage: %s WxH\n", argv[0]);
      exit(1);
    }
  }

  // initialize SDL and create window (for details see sdl.h/sdl.c)
  SDL_Context *ctx = sdl_context_new("game", width, height);

  Game game = {0};
  game.tile_size = 32;
  game.rows = ctx->height / game.tile_size; 
  game.cols = ctx->width / game.tile_size;

  game.paddle.size = 3;
  game.paddle.pos.x = game.cols / 2 - 1;
  game.paddle.pos.y = game.rows - 3;

  game.ball.pos.x = game.cols / 2;
  game.ball.pos.y = game.rows - 4;
  game.ball.lives = 3;

  Blocks blocks = {0};
  load_blocks(&game,&blocks, "resources/blocks.txt");

  char line_score[16];
  char line_lives[16];
  SDL_Color white = {255,255,255,255};

  TTF_Font *font = TTF_OpenFont("resources/OpenSans-Regular.ttf", 16);
  if(font == NULL) {
      printf("Error opening font!\n");
      exit(1);
  }

  snprintf(line_score,sizeof(line_score),"score:%d",game.score);
  snprintf(line_lives,sizeof(line_lives),"lives:%d",game.ball.lives);
  
  bool quit = false;
  Uint64 prevCounter = SDL_GetPerformanceCounter();
  double remaining = 250;
  while (!quit) {
    Uint64 now = SDL_GetPerformanceCounter(); 
    double elapsed_ms = (now - prevCounter) * 1000 / (double) SDL_GetPerformanceFrequency();
    prevCounter = now;

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        quit = true;
        break;
      }
      else if (e.type == SDL_KEYDOWN) {
        // key is pressed DOWN
        switch (e.key.keysym.sym) {
          case SDLK_ESCAPE:
          case SDLK_q:
            quit = true;
            break;
          case SDLK_LEFT:
            game.paddle.dir = -1;
            if(game.ball.dir.y == 0){
              game.ball.dir.x = -1;
            }
            break;
          case SDLK_RIGHT:
            game.paddle.dir = 1;
            if(game.ball.dir.y == 0){
              game.ball.dir.x = 1;
            }
            break;  
          case SDLK_SPACE:
            if (game.ball.dir.y == 0) {
              game.ball.dir.y = -1;
              game.ball.dir.x = -1; 
            }
            break;   
        }
      }  
      else if (e.type == SDL_KEYUP) {
        // key is released
        switch (e.key.keysym.sym) {
          case SDLK_LEFT:
            game.paddle.dir = 0;
            if(game.ball.dir.y == 0){
              game.ball.dir.x = 0;
            }
            break;
          case SDLK_RIGHT:
            game.paddle.dir = 0;
            if(game.ball.dir.y == 0){
              game.ball.dir.x = 0;
            }
            break;  
        }  
      } else if(e.type == SDL_MOUSEMOTION) {
        // mouse move
        //printf("mouse x=%d y=%d\n", e.motion.x, e.motion.y);
      } else if(e.type == SDL_MOUSEBUTTONUP) {
        // e.button.button: SDL_BUTTON_LEFT, SDL_BUTTON_MIDDLE, SDL_BUTTON_RIGHT
        //printf("mouse x=%d y=%d button=%d\n", e.button.x, e.button.y, e.button.button);
      }
    }

    // clear the buffer
    SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
    SDL_RenderClear(ctx->renderer);

    // your code here
    print_playfield(&game, ctx);

    if(remaining < 0) {
      
      //game win
      if(game.score == 10 * blocks_cols * blocks_rows) {

        reset_game(&game);
        load_blocks(&game,&blocks, "resources/blocks.txt");

      }
      
      if(game.ball.pos.y == game.rows - 3){
        //game over
        if(game.ball.lives == 1) {
          
        reset_game(&game);
        load_blocks(&game,&blocks, "resources/blocks.txt");

        }
        //lose life
        else {
          ball_lose_life(&game);
        }
      } 

      if(game.ball.dir.y != 0) {
        
        ball_change_dir(&game);
        ball_break_blocks(&game,&blocks);

        game.ball.pos.x += game.ball.dir.x;
        game.ball.pos.y += game.ball.dir.y; 

      }

      if(game.paddle.pos.x > 2 && game.paddle.dir < 0) {
        game.paddle.pos.x += game.paddle.dir;
        
        if(game.ball.dir.y == 0) {
          game.ball.pos.x += game.ball.dir.x;
        }
      }
      if(game.paddle.pos.x <= (game.cols - 3 - game.paddle.size) && game.paddle.dir > 0) {
        game.paddle.pos.x += game.paddle.dir;
        
        if(game.ball.dir.y == 0) {
          game.ball.pos.x += game.ball.dir.x;
        }
      }

      snprintf(line_score,sizeof(line_score),"score: %d",game.score);
      snprintf(line_lives,sizeof(line_lives),"lives: %d",game.ball.lives);

      remaining = 250;
    } 
    else {
      remaining -= elapsed_ms;
    }

    draw_text(ctx->renderer, font, line_score, 1.5 * game.tile_size, 1 * game.tile_size, white, 0);
    draw_text(ctx->renderer, font, line_lives, (game.cols - 3) * game.tile_size, 1 * game.tile_size, white, 0);

    print_blocks(&game,&blocks,ctx);
    print_paddle(&game,ctx);
    print_ball(&game,ctx);

    // show the buffer
    SDL_RenderPresent(ctx->renderer);
  }


  // cleanup all resources
  sdl_context_delete(ctx);
  return 0;
}
