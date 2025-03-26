#pragma once

#include <cstdint>
#include <string>

#include "utilities.hpp"

struct SDL_Window;

namespace omelet
{
[[nodiscard]] std::string get_sdl_error(const std::string &prefix = "");

class Window final
{
    SDL_Window *m_window{nullptr};
    void *m_context{nullptr};

  public:
    Window(const char *title,
           Size2D<int> size,
           uint32_t flags = 0U,
           bool multisampling = true);

    Window(const Window &) = delete;
    Window(Window &&) = delete;
    Window &operator=(const Window &) = delete;
    Window &operator=(Window &&) = delete;
    ~Window();

    void swap_window() const;
};
}  // namespace omelet
