#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  int width;
  int height;
} SDL_Context;

SDL_Context* sdl_context_new(const char* window_name, int width, int height);
void sdl_context_delete(SDL_Context *ctx);

void draw_text(SDL_Renderer *renderer, TTF_Font *font, const char *str, int x, int y, SDL_Color color, bool center_anchor);
