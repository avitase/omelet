#pragma once

#include "utilities.hpp"

namespace omelet
{
struct WindowState
{
    Point2D<float> origin{.x = 0.F, .y = 0.F};
    Size2D<float> size{.width = 0.F, .height = 0.F};
    bool dragging = false;
};

struct MouseState
{
    Point2D<float> position{.x = 0.F, .y = 0.F};
    bool left_button_pressed = false;
    bool right_button_pressed = false;
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
