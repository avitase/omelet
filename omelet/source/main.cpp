#define SDL_MAIN_USE_CALLBACKS 1

#include <exception>
#include <iostream>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_timer.h>

#include "world.hpp"

extern "C" SDL_AppResult SDL_AppInit(void **appstate,
                                     int /*argc*/,
                                     char ** /*argv*/)
{
    try {
        *appstate = new omelet::World{{.width = 800, .height = 600}};
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

extern "C" SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    static_cast<omelet::World *>(appstate)->handle_event(*event);

    return event->type == SDL_EVENT_QUIT ? SDL_APP_SUCCESS : SDL_APP_CONTINUE;
}

extern "C" SDL_AppResult SDL_AppIterate(void *appstate)
{
    static_cast<omelet::World *>(appstate)->tick(
        static_cast<double>(SDL_GetTicksNS()) / 1e9);

    return SDL_APP_CONTINUE;
}

extern "C" void SDL_AppQuit(void *appstate, SDL_AppResult /*result*/)
{
    delete static_cast<omelet::World *>(appstate);
}