#pragma once

#include <cstdint>

#include "utilities.hpp"

namespace omelet
{
struct WindowState
{
    Point2D<float> origin{.x = 0.F, .y = 0.F};
    Size2D<float> size{.width = 0.F, .height = 0.F};
    bool dragging = false;
};

enum class MouseButton : int8_t
{
    none = 0,
    left = 1,
    right = 2,
    middle = 3,
};

struct MouseState
{
    Point2D<float> position{.x = 0.F, .y = 0.F};
    MouseButton button_pressed = MouseButton::none;
    bool double_click = false;
    bool inside_window = false;
    bool inside_any_overlay = false;
};

struct WorldState
{
    WindowState window{};
    MouseState mouse{};
    double t = 0.;
    bool running = true;
};
}  // namespace omelet
