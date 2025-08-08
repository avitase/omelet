#pragma once

#include <SDL3/SDL_events.h>

#include "state.hpp"

namespace omelet
{
void on_window_event(const SDL_WindowEvent &event, WorldState &world_state);

void on_mouse_button_down(const SDL_MouseButtonEvent &event,
                          WorldState &world_state);

void on_mouse_button_up(const SDL_MouseButtonEvent &event,
                        WorldState &world_state);

void on_mouse_motion(const SDL_MouseMotionEvent &event,
                     WorldState &world_state);
}  // namespace omelet
