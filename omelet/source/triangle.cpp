#include <array>
#include <span>

#include "triangle.hpp"

#include <glbinding/gl/boolean.h>
#include <glbinding/gl/enum.h>
#include <glbinding/gl/functions.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "glsl_program.hpp"
#include "state.hpp"

namespace
{
const char *vertex_shader = R"(
#version 300 es

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
#version 300 es
precision mediump float;

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
          glsl::Program::create_vbo(
              {glsl::Program::FloatingPointAttribute{.idx = 0,
                                                     .size = 2,
                                                     .type = ::gl::GL_FLOAT,
                                                     .normalized = false,
                                                     .relative_offset = 0},
               glsl::Program::FloatingPointAttribute{.idx = 1,
                                                     .size = 3,
                                                     .type = ::gl::GL_FLOAT,
                                                     .normalized = false,
                                                     .relative_offset = 8}},
              glsl::Program::VBOLayout{.stride = 20, .offset = 0}),
          ::gl::GL_TRIANGLES)
{
}

void Triangle::draw(const WorldState &world_state, const glm::mat4 &vp)
{
    ::gl::glViewport(0,
                     0,
                     static_cast<int>(world_state.window.size.width),
                     static_cast<int>(world_state.window.size.height));

    {
        const auto phi = static_cast<float>(world_state.t);
        const glm::mat4 model =
            glm::rotate(glm::mat4{1.F}, phi, glm::vec3(0.F, 0.F, 1.F));

        const auto mvp = vp * model;
        m_shader_program.set_uniform("mvp",
                                     ::gl::glProgramUniformMatrix4fv,
                                     1,
                                     ::gl::GL_FALSE,
                                     &mvp[0][0]);
    }

    constexpr auto n_elements = 3UZ;
    constexpr std::array vertices{-100.F, -100.F, 0.F, 100.F, 100.F, -100.F};
    constexpr std::array color{1.F, 0.F, 0.F, 0.F, 1.F, 0.F, 0.F, 0.F, 1.F};

    std::array<float, vertices.size() + color.size()> data_zipped{0.F};
    for (auto i = 0UZ; i < n_elements; i++) {
        data_zipped[5UZ * i] = vertices[2UZ * i];
        data_zipped[(5UZ * i) + 1UZ] = vertices[(2UZ * i) + 1UZ];
        data_zipped[(5UZ * i) + 2UZ] = color[3UZ * i];
        data_zipped[(5UZ * i) + 3UZ] = color[(3UZ * i) + 1UZ];
        data_zipped[(5UZ * i) + 4UZ] = color[(3UZ * i) + 2UZ];
    }

    m_shader_program.fill_vbo(
        0, std::span{data_zipped.data(), data_zipped.size()});
    m_shader_program.draw(n_elements);
}

}  // namespace omelet
