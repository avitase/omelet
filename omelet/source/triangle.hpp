#pragma once

#include <glm/ext/matrix_float4x4.hpp>

#include "glsl_program.hpp"
#include "state.hpp"

namespace omelet
{
class Triangle final
{
    glsl::Program m_shader_program;

  public:
    Triangle();

    void draw(const WorldState &world_state, const glm::mat4 &vp);
};

}  // namespace omelet
