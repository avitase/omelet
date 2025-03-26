#pragma once

#include "glsl_program.hpp"
#include "state.hpp"

namespace omelet
{
class Triangle final
{
    glsl::Program m_shader_program;

  public:
    Triangle();

    void draw(const WorldState &world_state);
};

}  // namespace omelet
