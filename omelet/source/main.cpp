#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>

#include "world.hpp"

int main(int /*argc*/, char ** /*argv*/)
{
    omelet::World world{{.width = 800, .height = 600}};

    bool running = true;
    uint32_t fps = 60.0F;
    while (running) {
        const auto t0 = std::chrono::high_resolution_clock::now();
        const auto &state = world.tick(fps);
        running = state.running;

        const auto t1 = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> dt{t1 - t0};
        fps = static_cast<uint32_t>(std::round(1. / dt.count()));
    }

    return EXIT_SUCCESS;
}
