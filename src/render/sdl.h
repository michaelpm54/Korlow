#ifndef KORLOW_SDL_H
#define KORLOW_SDL_H

#include <SDL2/SDL.h>

#include <unordered_map>

struct KeyState {
    bool pressed;
    //bool prev;
};

struct Window {
    SDL_Window* handle {nullptr};
    SDL_GLContext context {nullptr};
    std::unordered_map<SDL_Keycode, KeyState> bindings;
    std::unordered_map<int, KeyState*> actions;
};

void sdl_init();
void sdl_free();
bool sdl_open(Window* window);
void sdl_close(Window* window);

void sdl_bind(Window* window, SDL_Keycode keycode, int action);
bool sdl_get_action(Window* window, int action);

/* Returns true when quit triggered. */
bool sdl_poll(Window* window);

void sdl_flip(Window* window);

#endif    // KORLOW_SDL_H
