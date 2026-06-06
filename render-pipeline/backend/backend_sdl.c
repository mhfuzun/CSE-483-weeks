#include "backend.h"

#if __has_include(<SDL2/SDL.h>)
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#else
#include <SDL.h>
#include <SDL_ttf.h>
#endif
#include <stdint.h>
#include <stdio.h>

#include "ptime.h"

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *texture;
static int quit_requested;

static SDL_Texture *fpsTextTexture;
static TTF_Font *font;
static SDL_Color red = {255, 0, 0, 255};
static SDL_Rect fpsTextRect;
static uint64_t fpsLastTime;
static int frameCount;

static int setFPS(double fps) {
    char fpsTextBuffer[64];
    snprintf(fpsTextBuffer, sizeof(fpsTextBuffer), "FPS: %.2f", fps);

    if (fpsTextTexture) {
        SDL_DestroyTexture(fpsTextTexture);
        fpsTextTexture = NULL;
    }

    SDL_Surface *textSurface = TTF_RenderText_Blended(font, fpsTextBuffer, red);
    if (!textSurface) {
        fprintf(stderr, "TTF_RenderText_Blended failed: %s\n", TTF_GetError());
        return 0;
    }

    fpsTextTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!fpsTextTexture) {
        fprintf(stderr, "SDL_CreateTextureFromSurface failed: %s\n", SDL_GetError());
        SDL_FreeSurface(textSurface);
        return 0;
    }

    fpsTextRect.w = textSurface->w;
    fpsTextRect.h = textSurface->h;

    SDL_FreeSurface(textSurface);
    return 1;
}

static int init(int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 0;
    }

    window = SDL_CreateWindow(
        "Render Pipeline",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI
    );
    if (!window) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

    if (TTF_Init() != 0) {
        fprintf(stderr, "TTF_Init failed: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        width,
        height
    );
    if (!texture) {
        fprintf(stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

    font = TTF_OpenFont("resources/Roboto-VariableFont_wdth.ttf", 32);
    if (!font) {
        fprintf(stderr, "TTF_OpenFont failed: %s\n", TTF_GetError());
        SDL_DestroyTexture(texture);
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

    fpsTextRect.x = 20;
    fpsTextRect.y = 20;
    fpsLastTime = getCurrentTime().ns;
    frameCount = 0;

    if (!setFPS(0.0)) {
        TTF_CloseFont(font);
        SDL_DestroyTexture(texture);
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

    return 1;
}

static void poll_events(void) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            quit_requested = 1;
        } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            quit_requested = 1;
        }
    }
}

static void present(uint32_t *pixels, int width, int height) {
    poll_events();

    (void)height;

    frameCount++;
    uint64_t currentTime = getCurrentTime().ns;
    double elapsed = (double)(currentTime - fpsLastTime) / 1000000000.0;

    if (elapsed >= 1.0) {
        setFPS((double)frameCount / elapsed);
        frameCount = 0;
        fpsLastTime = currentTime;
    }

    SDL_UpdateTexture(texture, NULL, pixels, width * (int)sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderCopy(renderer, fpsTextTexture, NULL, &fpsTextRect);
    SDL_RenderPresent(renderer);
}

static int should_close(void) {
    return quit_requested;
}

static void shutdown(void) {
    if (fpsTextTexture) {
        SDL_DestroyTexture(fpsTextTexture);
        fpsTextTexture = NULL;
    }
    if (font) {
        TTF_CloseFont(font);
        font = NULL;
    }
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = NULL;
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = NULL;
    }
    TTF_Quit();
    SDL_Quit();
}

static backend_t backend = {
    .init = init,
    .present = present,
    .should_close = should_close,
    .shutdown = shutdown
};

backend_t *get_sdl_backend(void) {
    return &backend;
}
