#include <cstdint>

#include "world.hpp"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_video.h>
#include <glbinding/gl/bitfield.h>
#include <glbinding/gl/functions.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>

#include "events.hpp"
#include "fonts.hpp"
#include "overlays.hpp"
#include "state.hpp"
#include "triangle.hpp"
#include "utilities.hpp"
#include "window.hpp"

namespace omelet
{
namespace
{
void handle_event(const SDL_Event &event, WorldState &world_state)
{
    switch (event.type) {
        case SDL_QUIT: {
            world_state.running = false;
        } break;

        case SDL_WINDOWEVENT: {
            on_window_event(event.window, world_state);
        } break;

        case SDL_MOUSEBUTTONDOWN: {
            on_mouse_button_down(event.button, world_state);
        } break;

        case SDL_MOUSEBUTTONUP: {
            on_mouse_button_up(event.button, world_state);
        } break;

        case SDL_MOUSEMOTION: {
            on_mouse_motion(event.motion, world_state);
        } break;

        default:;
    }
}
}  // namespace

World::World(const Config &cfg)
    : m_window("Omelet",
               Size2D{.width = cfg.width, .height = cfg.height},
               SDL_WINDOW_RESIZABLE)
    , m_world_state(
          {.window = {.origin = {.x = 0.0, .y = 0.0},
                      .size = {.width = cfg.width, .height = cfg.height},
                      .dragging = false},
           .mouse = {},
           .t = 0.0,
           .running = true})
{
    auto &io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.Fonts->AddFontFromMemoryCompressedTTF(
        fonts::hack.data, fonts::hack.size, 12);

    ImGuiStyle &style = ImGui::GetStyle();
    style.WindowRounding = 0.0F;
}

const WorldState &World::tick(const uint32_t fps)
{
    if (fps > 0) {
        m_world_state.t += 1.0 / static_cast<double>(fps);
    }

    m_world_state.mouse.button_pressed = MouseButton::none;

    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        m_world_state.mouse.inside_any_overlay =
            ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);

        handle_event(event, m_world_state);
    }

    {
        int x_pxl = 0;
        int y_pxl = 0;
        SDL_GetMouseState(&x_pxl, &y_pxl);

        auto &mouse = m_world_state.mouse;
        mouse.position.x = static_cast<double>(x_pxl);
        mouse.position.y =
            static_cast<double>(m_world_state.window.size.height - y_pxl);
    }

    if (m_world_state.running) {
        {
            ::gl::glClearColor(1.0F, 1.0F, 1.0F, 1.0F);
            ::gl::glClear(::gl::GL_COLOR_BUFFER_BIT);
            ::gl::glClear(::gl::GL_DEPTH_BUFFER_BIT);
        }

        {
            const auto w = static_cast<float>(m_world_state.window.size.width);
            const auto h = static_cast<float>(m_world_state.window.size.height);
            const auto x0 = static_cast<float>(m_world_state.window.origin.x);
            const auto y0 = static_cast<float>(m_world_state.window.origin.y);

            const auto vp = glm::ortho(/*left=*/-w / 2.F,
                                       /*right=*/w / 2.F,
                                       /*bottom=*/-h / 2.F,
                                       /*top=*/h / 2.F,
                                       /*zNear=*/0.F,
                                       /*zFar=*/1.F)
                * glm::lookAt(/*eye=*/glm::vec3(x0, y0, 1.F),
                              /*center=*/glm::vec3(x0, y0, 0.F),
                              /*up=*/glm::vec3(0.F, 1.F, 0.F));

            m_triangle.draw(m_world_state, vp);
        }

        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();

            m_overlay.draw(m_world_state);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        m_window.swap_window();
    }

    return m_world_state;
}
}  // namespace omelet
