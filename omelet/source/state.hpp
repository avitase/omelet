#pragma once

#include <cstdint>

#include "utilities.hpp"

namespace omelet
{
struct WindowState
{
    Point2D<double> origin{.x = 0.0, .y = 0.0};
    Size2D<int> size{.width = 0, .height = 0};
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
    Point2D<double> position{.x = 0.0, .y = 0.0};
    MouseButton button_pressed = MouseButton::none;
    bool double_click = false;
    bool inside_window = false;
    bool inside_any_overlay = false;
};

struct WorldState
{
    WindowState window{};
    MouseState mouse{};
    double t = 0.0;
    bool running = true;
};
}  // namespace omelet
