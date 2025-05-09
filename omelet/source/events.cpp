#include "events.hpp"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_video.h>

#include "state.hpp"

namespace omelet
{
void on_window_event(const SDL_WindowEvent &event, WorldState &world_state)
{
    auto &window = world_state.window;
    auto &mouse = world_state.mouse;

    switch (event.event) {
        case SDL_WINDOWEVENT_RESIZED: {
            window.size.width = event.data1;
            window.size.height = event.data2;
        } break;
        case SDL_WINDOWEVENT_ENTER: {
            mouse.inside_window = true;
        } break;
        case SDL_WINDOWEVENT_LEAVE: {
            mouse.inside_window = false;
        } break;
        default:;
    }
}

void on_mouse_button_down(const SDL_MouseButtonEvent &event,
                          WorldState &world_state)
{
    auto &window = world_state.window;
    auto &mouse = world_state.mouse;

    switch (event.button) {
        case SDL_BUTTON_LEFT: {
            window.dragging = not mouse.inside_any_overlay;
            mouse.button_pressed = MouseButton::left;
            mouse.double_click = event.clicks > 1;
        } break;
        case SDL_BUTTON_RIGHT: {
            mouse.button_pressed = MouseButton::right;
            mouse.double_click = event.clicks > 1;
        } break;
        default:;
    }
}

void on_mouse_button_up(const SDL_MouseButtonEvent &event,
                        WorldState &world_state)
{
    switch (event.button) {
        case SDL_BUTTON_LEFT: {
            world_state.window.dragging = false;
        } break;
        default:;
    }
}

void on_mouse_motion(const SDL_MouseMotionEvent &event, WorldState &world_state)
{
    auto &window = world_state.window;
    if (window.dragging) {
        window.origin.x -= event.xrel;
        window.origin.y += event.yrel;
    }
}
}  // namespace omelet