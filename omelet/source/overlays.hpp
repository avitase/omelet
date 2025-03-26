#pragma once

#include "state.hpp"

namespace omelet
{
class Overlay final
{
  public:
    void draw(const WorldState &world_state);
};
}  // namespace omelet
