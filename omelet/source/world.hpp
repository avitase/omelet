#pragma once

#include <SDL3/SDL_events.h>

#include "overlays.hpp"
#include "state.hpp"
#include "triangle.hpp"
#include "window.hpp"

namespace omelet
{
class World final
{
    Window m_window;
    WorldState m_world_state;
    Overlay m_overlay{};
    Triangle m_triangle;

  public:
    struct Config
    {
        int width;
        int height;
    };

    explicit World(const Config &cfg);

    World(const World &) = delete;
    World(World &&) = delete;
    World &operator=(const World &) = delete;
    World &operator=(World &&) = delete;

    ~World() = default;

    void handle_event(const SDL_Event &event);
    const WorldState &tick(double ticks);
};
}  // namespace omelet
