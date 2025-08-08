#include <cstdint>

#include "world.hpp"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_video.h>
#include <glbinding/gl/bitfield.h>
#include <glbinding/gl/functions.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>

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
        case SDL_EVENT_QUIT: {
            world_state.running = false;
        } break;

        case SDL_EVENT_WINDOW_RESIZED:
        case SDL_EVENT_WINDOW_MOUSE_ENTER:
        case SDL_EVENT_WINDOW_MOUSE_LEAVE: {
            on_window_event(event.window, world_state);
        } break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN: {
            on_mouse_button_down(event.button, world_state);
        } break;

        case SDL_EVENT_MOUSE_BUTTON_UP: {
            on_mouse_button_up(event.button, world_state);
        } break;

        case SDL_EVENT_MOUSE_MOTION: {
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
          {.window = {.origin = {.x = 0.F, .y = 0.F},
                      .size = {.width = static_cast<float>(cfg.width),
                               .height = static_cast<float>(cfg.height)},
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
    style.WindowRounding = 0.F;
}

const WorldState &World::tick(const uint32_t fps)
{
    if (fps > 0) {
        m_world_state.t += 1. / static_cast<double>(fps);
    }

    m_world_state.mouse.button_pressed = MouseButton::none;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL3_ProcessEvent(&event);
        m_world_state.mouse.inside_any_overlay =
            ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);

        handle_event(event, m_world_state);
    }

    {
        auto &mouse = m_world_state.mouse;
        SDL_GetMouseState(&mouse.position.x, &mouse.position.y);

        mouse.position.y = m_world_state.window.size.height - mouse.position.y;
    }

    if (m_world_state.running) {
        {
            ::gl::glClearColor(1.F, 1.F, 1.F, 1.F);
            ::gl::glClear(::gl::GL_COLOR_BUFFER_BIT);
            ::gl::glClear(::gl::GL_DEPTH_BUFFER_BIT);
        }

        {
            const auto w = m_world_state.window.size.width;
            const auto h = m_world_state.window.size.height;
            const auto x0 = m_world_state.window.origin.x;
            const auto y0 = m_world_state.window.origin.y;

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
            ImGui_ImplSDL3_NewFrame();
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
