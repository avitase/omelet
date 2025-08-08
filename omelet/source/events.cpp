#include "events.hpp"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_mouse.h>

#include "state.hpp"

namespace omelet
{
void on_window_event(const SDL_WindowEvent &event, WorldState &world_state)
{
    auto &window = world_state.window;
    auto &mouse = world_state.mouse;

    switch (event.type) {
        case SDL_EVENT_WINDOW_RESIZED: {
            window.size.width = static_cast<float>(event.data1);
            window.size.height = static_cast<float>(event.data2);
        } break;
        case SDL_EVENT_WINDOW_MOUSE_ENTER: {
            mouse.inside_window = true;
        } break;
        case SDL_EVENT_WINDOW_MOUSE_LEAVE: {
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
            mouse.left_button_pressed = true;
            mouse.double_click = event.clicks > 1;

            window.dragging = not mouse.inside_any_overlay;
        } break;
        case SDL_BUTTON_RIGHT: {
            mouse.right_button_pressed = true;
            mouse.double_click = event.clicks > 1;
        } break;
        default:;
    }
}

void on_mouse_button_up(const SDL_MouseButtonEvent &event,
                        WorldState &world_state)
{
    auto &window = world_state.window;
    auto &mouse = world_state.mouse;

    switch (event.button) {
        case SDL_BUTTON_LEFT: {
            mouse.left_button_pressed = false;
            window.dragging = false;
        } break;
        case SDL_BUTTON_RIGHT: {
            mouse.right_button_pressed = false;
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
