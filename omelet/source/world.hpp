#pragma once

#include <cstdint>

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

    const WorldState &tick(uint32_t fps);
};
}  // namespace omelet
