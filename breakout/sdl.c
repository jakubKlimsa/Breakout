#include <SDL2/SDL_image.h>
#include "sdl.h"

SDL_Context* sdl_context_new(const char* window_name, int width, int height) {
  SDL_Context *ctx = malloc(sizeof(SDL_Context));
  if(ctx == NULL) {
    fprintf(stderr, "Could not allocate memory.");
    exit(1);
  }

  // Init SDL with video and audio
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
    fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
    exit(1);
  }

  // Init TrueType fonts
  if(TTF_Init() != 0) {
    fprintf(stderr, "TTF_Init Error: %s\n", TTF_GetError());
    exit(1);
  }

  bool enableFullscreen = width <= 0 || height <= 0;
  ctx->window = SDL_CreateWindow(
    window_name,
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    width, height,
    SDL_WINDOW_SHOWN | (enableFullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0)
  );
  if (ctx->window == NULL) {
    fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
    exit(1);
  }

  // create GPU accelerated renderer and use vertical sync
  ctx->renderer = SDL_CreateRenderer(ctx->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (ctx->renderer == NULL) {
    fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
    exit(1);
  }

  // enable transparency
  if(SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_BLEND) != 0) {
    fprintf(stderr, "SDL_SetRenderDrawBlendMode Error: %s\n", SDL_GetError());
    exit(1);
  }

  if(enableFullscreen) {
    if(SDL_GetRendererOutputSize(ctx->renderer, &width, &height) != 0) {
      fprintf(stderr, "SDL_GetRendererOutputSize Error: %s\n", SDL_GetError());
      exit(1);
    }
  }

  ctx->width = width;
  ctx->height = height;
  return ctx;
}

void sdl_context_delete(SDL_Context *ctx) {
  SDL_DestroyRenderer(ctx->renderer);
  SDL_DestroyWindow(ctx->window);
  TTF_Quit();
  SDL_Quit();
}

void draw_text(SDL_Renderer *renderer, TTF_Font *font, const char *str, int x, int y, SDL_Color color, bool center_anchor) {
  // Render the text with a font into the RAM memory
  SDL_Surface *surface = TTF_RenderText_Solid(font, str, color);
  if(surface == NULL) {
    fprintf(stderr, "TTF_RenderText_Solid Error: %s\n", SDL_GetError());
    exit(1);
  }

  // Convert surface from the RAM memory to the GPU memory
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  if(texture == NULL) {
    fprintf(stderr, "SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
    exit(1);
  }

  // render the texture on x, y with width and height
  SDL_Rect rect;
  rect.x = x - surface->w / 2 * center_anchor;
  rect.y = y;
  rect.w = surface->w;
  rect.h = surface->h;
  SDL_RenderCopy(renderer, texture, NULL, &rect);

  // free the surface from the CPU memory
  SDL_FreeSurface(surface);

  // Texture needs to be freed from the GPU
  SDL_DestroyTexture(texture);
}
