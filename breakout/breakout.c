#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "sdl.h"
#include "breakout.h"

void print_playfield (Game *game, SDL_Context *ctx) {

  SDL_SetRenderDrawColor(ctx->renderer, 255, 255, 255, 255); 
  SDL_Rect rect; 

  rect.w = rect.h = game->tile_size;
  rect.y = 2 * game->tile_size;

  //Top border of the playfield
  for (int i = 1; i <= game->cols - 2; i++)
  {
    rect.x = i * game->tile_size;
    SDL_RenderFillRect(ctx-> renderer, &rect);
  }
  
  //Side borders of the playfield
  for (int i = 3; i < game->rows - 2; i++)
  {   
    rect.y = i * game->tile_size;

    rect.x = 1 * game->tile_size;
    SDL_RenderFillRect(ctx-> renderer, &rect);
    
    rect.x = (game->cols - 2) * game->tile_size;
    SDL_RenderFillRect(ctx-> renderer, &rect);
  }
      
}

void print_paddle (Game *game, SDL_Context *ctx) {
   
  SDL_SetRenderDrawColor(ctx->renderer, 255, 0, 0, 255);  
  SDL_Rect rect; 

  rect.w = game->tile_size * game->paddle.size; 
  rect.h = game->tile_size;
  rect.x = game->paddle.pos.x * game->tile_size;
  rect.y = game->paddle.pos.y * game->tile_size;

  SDL_RenderFillRect(ctx-> renderer, &rect);
}

void print_ball (Game *game, SDL_Context *ctx) {
   
  SDL_SetRenderDrawColor(ctx->renderer, 255, 255, 255, 255);  
  SDL_Rect rect; 

  rect.h = rect.w = game->tile_size;
  rect.x = game->ball.pos.x * game->tile_size;
  rect.y = game->ball.pos.y * game->tile_size;

  SDL_RenderFillRect(ctx-> renderer, &rect);

}  

void ball_change_dir (Game *game) {

  int difference = game->ball.pos.x - (game->paddle.pos.x + (game->paddle.size - 1)/2);
   
  if (game->ball.pos.x == game->cols - 3 || game->ball.pos.x == 2) {
    game->ball.dir.x = game->ball.dir.x * -1;
  }

  if (game->ball.pos.y == 3) {
    game->ball.dir.y = 1;
  }

  else if (game->ball.pos.y == game->rows - 4 && difference <= (game->paddle.size - 1)/2 && difference >= -(game->paddle.size - 1)/2 )
  {
    game->ball.dir.y = -1;
  } 

}  

void ball_lose_life (Game *game) {

  game->ball.lives -= 1;

  game->ball.dir.x = 0;
  game->ball.dir.y = 0;

  game->ball.pos.x = game->paddle.pos.x + (game->paddle.size -1)/2;
  game->ball.pos.y = game->rows - 4;

}

void load_blocks (Game *game ,Blocks *blocks, char *path) {

  FILE *f = fopen(path, "r");
    if(f == NULL) {
      printf("Error opening file!\n");
      exit(1);
    }

  int i = 0;
  while(fscanf(f,"%d %d %d ", &blocks->line[i].r,&blocks->line[i].g,&blocks->line[i].b) == 3) {   
    
    i++;  
  } 

  fclose(f);

  for (int j = 0;j < sizeof(blocks->line)/sizeof(Line) ;j++) {
    for(i = 0;i < game->cols - 4;i++) {
      int current_block = i + j * (game->cols - 4);

      blocks->block[current_block].lives = 1;
      blocks->block[current_block].pos.x = 2 + i;
      blocks->block[current_block].pos.y = 3 + j; 
    }
  }
}

void print_blocks (Game *game ,Blocks *blocks, SDL_Context *ctx) {

  SDL_Rect rect; 
  rect.w = rect.h = game->tile_size;

  for(int j = 0;j < sizeof(blocks->line)/sizeof(Line) ;j++) {

    SDL_SetRenderDrawColor(ctx->renderer, blocks->line[j].r, blocks->line[j].g, blocks->line[j].b, 255);  
    
    for(int i = 0;i < game->cols - 4;i++) {
      
      int current_block = i + j * (game->cols - 4);
      rect.x = blocks->block[current_block].pos.x * game->tile_size;
      rect.y = blocks->block[current_block].pos.y * game->tile_size;
      
      if(blocks->block[current_block].lives != 0) {
        SDL_RenderFillRect(ctx-> renderer, &rect);
      }
      
    }
  }

}

void ball_break_blocks (Game *game, Blocks *blocks) {

  for(int j = 0;j < sizeof(blocks->line)/sizeof(Line) ;j++) {
    
    for(int i = 0;i < game->cols - 4;i++) {
      
      int current_block = i + j * (game->cols - 4);
      
      if(blocks->block[current_block].lives != 0 && abs(blocks->block[current_block].pos.x - game->ball.pos.x) == 1 && blocks->block[current_block].pos.y - game->ball.pos.y == 0) {

        blocks->block[current_block].lives = 0;
        game->ball.dir.x = game->ball.dir.x * -1;   
        game->score += 10;  
        return;  
      }
      else if(blocks->block[current_block].lives != 0 && abs(blocks->block[current_block].pos.y - game->ball.pos.y) == 1 && blocks->block[current_block].pos.x - game->ball.pos.x == 0) {

        blocks->block[current_block].lives = 0;
        game->ball.dir.y = game->ball.dir.y * -1;
        game->score += 10; 
        return;       
      }
      
    }
  } 

  for(int j = 0;j < sizeof(blocks->line)/sizeof(Line) ;j++) {
      
    for(int i = 0;i < game->cols - 4;i++) {
        
      int current_block = i + j * (game->cols - 4);
        
      if(blocks->block[current_block].lives != 0 && blocks->block[current_block].pos.y - (game->ball.pos.y + game->ball.dir.y) == 0 && blocks->block[current_block].pos.x - (game->ball.pos.x + game->ball.dir.x) == 0) {

        blocks->block[current_block].lives = 0;
        
        game->ball.dir.y = game->ball.dir.y * -1;  
        game->ball.dir.x = game->ball.dir.x * -1;  
        
        game->score += 10;
      }     
    }
  }
  
}

void reset_game(Game *game) {

  game->paddle.pos.x = game->cols / 2 - 1;
  game->paddle.pos.y = game->rows - 3;

  game->ball.pos.x = game->cols / 2;
  game->ball.pos.y = game->rows - 4;
    
  game->ball.dir.x = 0;
  game->ball.dir.y = 0;

  game->ball.lives = 3;   
  game->score = 0;  

}