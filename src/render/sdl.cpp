#include "render/sdl.h"

#include <glad/glad.h>
#include <imgui/imgui_impl_sdl.h>

#include <cstdio>

void sdl_init()
{
    SDL_Init(SDL_INIT_VIDEO);
}

void sdl_free()
{
    SDL_Quit();
}

bool sdl_open(Window* window)
{
    window->handle = SDL_CreateWindow("DMG Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
    if (!window->handle) {
        fprintf(stderr, "Failed to create SDL window: %s\n", SDL_GetError());
        return false;
    }

    // Vsync
    SDL_GL_SetSwapInterval(1);

    // OpenGL context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    window->context = SDL_GL_CreateContext(window->handle);
    if (!window->context) {
        fprintf(stderr, "Failed to create SDL context: %s\n", SDL_GetError());
        SDL_DestroyWindow(window->handle);
        return false;
    }

    // OpenGL functions
    if (!gladLoadGL()) {
        fprintf(stderr, "(gladLoadGL) Failed to load OpenGL functions\n");
        SDL_GL_DeleteContext(window->context);
        SDL_DestroyWindow(window->handle);
        return false;
    }
    else {
        fprintf(stdout, "Loaded OpenGL: %s\n", glGetString(GL_VERSION));
    }

    return true;
}

void sdl_close(Window* window)
{
    if (window->context)
        SDL_GL_DeleteContext(window->context);
    if (window->handle)
        SDL_DestroyWindow(window->handle);
    window->context = nullptr;
    window->handle = nullptr;
}

bool sdl_poll(Window* window)
{
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        ImGui_ImplSDL2_ProcessEvent(&ev);
        switch (ev.type) {
            case SDL_QUIT:
                printf("SDL_QUIT\n");
                return true;
            case SDL_WINDOWEVENT:
                if (ev.window.event == SDL_WINDOWEVENT_CLOSE && ev.window.windowID == SDL_GetWindowID(window->handle)) {
                    printf("SDL_WINDOWEVENT_CLOSE\n");
                    return true;
                }
                break;
            default:
                break;
        }
    }
    const unsigned char* keys = SDL_GetKeyboardState(nullptr);
    for (auto& [keycode, binding] : window->bindings) {
        /* Set pressed to false if so. */
        if (!keys[keycode]) {
            if (binding.pressed)
                binding.pressed = false;
            continue;
        }

        /* Key is pressed. */
        //binding.prev = binding.pressed;
        binding.pressed = true;
    }
    return false;
}

void sdl_flip(Window* window)
{
    SDL_GL_SwapWindow(window->handle);
}

void sdl_bind(Window* window, SDL_Keycode keycode, int action)
{
    window->bindings[keycode] = {false};
    window->actions[action] = &window->bindings[keycode];
}

bool sdl_get_action(Window* window, int action)
{
    if (window->actions.count(action))
        return window->actions[action]->pressed;
    return false;
}
