#include <cstdint>
#include <stdexcept>
#include <string>

#include "window.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_video.h>
#include <glbinding-aux/debug.h>
#include <glbinding/ProcAddress.h>
#include <glbinding/gl/enum.h>
#include <glbinding/gl/functions.h>
#include <glbinding/glbinding.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>

#include "utilities.hpp"

namespace omelet
{
std::string get_sdl_error(const std::string &prefix)
{
    auto msg = prefix;
    if (not prefix.empty()) {
        msg += ' ';
    }
    return msg + "SDL error: " + std::string(SDL_GetError());
}

Window::Window(const std::string &title,
               const Size2D<int> size,
               const uint32_t flags,
               const bool multisampling)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error(get_sdl_error("SDL could not initialize."));
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    if (multisampling) {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
    }

    m_window = SDL_CreateWindow(title.c_str(),
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                size.width,
                                size.height,
                                SDL_WINDOW_OPENGL | flags);

    if (m_window == nullptr) {
        const auto &msg = get_sdl_error("SDL could not initialize.");
        SDL_Quit();

        throw std::runtime_error(msg);
    }

    m_context = SDL_GL_CreateContext(m_window);
    if (m_context == nullptr) {
        const auto &msg = get_sdl_error("OpenGL context could not be created.");
        SDL_DestroyWindow(m_window);
        SDL_Quit();

        throw std::runtime_error(msg);
    }

    ::glbinding::initialize(
        reinterpret_cast<::glbinding::ProcAddress (*)(const char *)>(
            SDL_GL_GetProcAddress),
        /*resolve_functions=*/true);

    glbinding::aux::enableGetErrorCallback();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsLight();
    ImGui_ImplSDL2_InitForOpenGL(m_window, m_context);
    ImGui_ImplOpenGL3_Init("#version 460");

    if (multisampling) {
        ::gl::glEnable(::gl::GL_MULTISAMPLE);
    }
}

Window::~Window()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(m_context);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void Window::swap_window() const
{
    SDL_GL_SwapWindow(m_window);
}
}  // namespace omelet
