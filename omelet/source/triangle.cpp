#include <array>
#include <span>

#include "triangle.hpp"

#include <glbinding/gl/boolean.h>
#include <glbinding/gl/enum.h>
#include <glbinding/gl/functions.h>

// NOLINTBEGIN(misc-include-cleaner)
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
// NOLINTEND(misc-include-cleaner)

#include "glsl_program.hpp"
#include "state.hpp"

namespace
{
const char *vertex_shader = R"(
#version 460 core

layout(location = 0) in vec2 in_xy;
layout(location = 1) in vec3 in_rgb;

uniform mat4 mvp;

out vec4 v_color;

void main()
{
    gl_Position = mvp * vec4(in_xy.x, in_xy.y, 0.5, 1.0);
    v_color = vec4(in_rgb, 1.0);
}
)";

const char *fragment_shader = R"(
#version 460 core

in vec4 v_color;
out vec4 f_color;

void main()
{
    f_color = v_color;
}
)";
}  // namespace

namespace omelet
{

Triangle::Triangle()
    : m_shader_program(
          {{.type = ::gl::GL_VERTEX_SHADER, .source = vertex_shader},
           {.type = ::gl::GL_FRAGMENT_SHADER, .source = fragment_shader}},
          {glsl::Program::create_vbo(
               {glsl::Program::FloatingPointAttribute{.name = "in_xy",
                                                      .idx = 0,
                                                      .size = 2,
                                                      .type = ::gl::GL_FLOAT,
                                                      .normalized = false,
                                                      .buffer_offset = 0,
                                                      .buffer_stride = 8}}),
           glsl::Program::create_vbo(
               {glsl::Program::FloatingPointAttribute{.name = "in_rgb",
                                                      .idx = 1,
                                                      .size = 3,
                                                      .type = ::gl::GL_FLOAT,
                                                      .normalized = false,
                                                      .buffer_offset = 0,
                                                      .buffer_stride = 12}})},
          ::gl::GL_TRIANGLES)
{
}

void Triangle::draw(const WorldState &world_state)
{
    ::gl::glViewport(
        0, 0, world_state.window.size.width, world_state.window.size.height);

    {
        const auto w = static_cast<float>(world_state.window.size.width);
        const auto h = static_cast<float>(world_state.window.size.height);

        // NOLINTNEXTLINE(misc-include-cleaner)
        const glm::mat4 projection = glm::ortho(/*left=*/-w / 2.F,
                                                /*right=*/w / 2.F,
                                                /*bottom=*/-h / 2.F,
                                                /*top=*/h / 2.F,
                                                /*zNear=*/0.F,
                                                /*zFar=*/1.F);

        const auto x0 = static_cast<float>(world_state.window.origin.x);
        const auto y0 = static_cast<float>(world_state.window.origin.y);

        const glm::mat4 view =
            // NOLINTNEXTLINE(misc-include-cleaner)
            glm::lookAt(/*eye=*/glm::vec3(x0, y0, 1.F),
                        /*center=*/glm::vec3(x0, y0, 0.F),
                        /*up=*/glm::vec3(0.F, 1.F, 0.F));

        const auto phi = static_cast<float>(world_state.t);
        const glm::mat4 model =
            // NOLINTNEXTLINE(misc-include-cleaner)
            glm::rotate(glm::mat4{1.F}, phi, glm::vec3(0.F, 0.F, 1.F));

        const auto mvp = projection * view * model;

        m_shader_program.set_uniform("mvp",
                                     ::gl::glProgramUniformMatrix4fv,
                                     1,
                                     ::gl::GL_FALSE,
                                     &mvp[0][0]);
    }

    constexpr std::array vertices{-100.F, -100.F, 0.F, 100.F, 100.F, -100.F};
    m_shader_program.fill_vbo(0, std::span{vertices.data(), vertices.size()});

    constexpr std::array color{1.F, 0.F, 0.F, 0.F, 1.F, 0.F, 0.F, 0.F, 1.F};
    m_shader_program.fill_vbo(1, std::span{color.data(), color.size()});

    m_shader_program.draw(3);
}

}  // namespace omelet