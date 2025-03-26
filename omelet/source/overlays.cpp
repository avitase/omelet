#include "overlays.hpp"

#include <imgui.h>

#include "state.hpp"

namespace omelet
{
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Overlay::draw(const WorldState &world_state)
{
    ImGui::SetNextWindowSize(ImVec2(180, 70));

    ImGui::Begin("Info");

    {
        const auto x = static_cast<double>(world_state.mouse.position.x);
        const auto y = static_cast<double>(world_state.mouse.position.y);
        ImGui::Text("Mouse position: (%.0f, %.0f)", x, y);
    }

    {
        const auto fps = static_cast<double>(ImGui::GetIO().Framerate);
        ImGui::Text("FPS: %.0f", fps);
    }

    ImGui::End();
}
}  // namespace omelet
