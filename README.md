# Omlete 🍳

This is a highly opinionated template using *contemporary* C++ to wrap "modern"
OpenGL, [SDL2][1], [Dear ImGui][2], [glbindings][3], and [GLM][4].

## How to use it?

Checkout the repository **including its various submodules** and build it with [CMake][5]. A layman approach (on Linux🐧)
is to type:

```bash
git clone --recurse-submodules https://github.com/avitase/omelet
mkdir -p omelet/build && cd omelet/build/
cmake .. && make -j
```

If you have installed [SDL2][1] and OpenGL on your system, this will build [Dear ImGui][2], [glbindings][3],
and [GLM][4] and finally compile the [template in omelet/](omelet/). (If not, it will fail spectacularly💥)

Compiling the dependencies does take some time but will eventually create an executable under `build/install/bin`. Run
it, it's safe!✋

```
./install/bin/omelete
```

You should see a spinning triangle and a [Dear ImGui window][2] with the position of your cursor and an FPS counter.

## How to use it properly?

This template will give you a CMake Superbuild that builds [Dear ImGui][2], [glbindings][3], and [GLM][4] as
dependencies and the actual SDL2/OpenGL template. Typically, building the Superbuild only has to be done once, you can
then navigate to [`omelete/`](omelet/) (the subdirectory) and start fouling around here.

The subproject [`omelet/`](omelet/) expects to find its dependencies under `build/install`. This happens automatically
if you use our [CMakeUserPresets.json.EXAMPLE][6] (click on the link to learn more about what this is), e.g., by copying
it:

```bash
# project root (not omelet/omelet!)
cp CMakeUserPresets.json.EXAMPLE CMakeUserPresets.json
```

This will make a new preset `release` available that you can use to build the Superbuild:

```bash
cmake --preset=release
cmake --build --preset=release
```

Similar to before, this should install everything under `build/install` and you can now navigate to the subproject [
`omelet/`](omelet/).
Here, you'll find again a `CMakeUserPresets.json.EXAMPLE` that sets `CMAKE_PREFIX_PATH` for you and exposes two
presets `dev` and `dev-clang-tidy`. Feel free to change these presets to your own needs and then run, e.g.,

```bash
# run this in omelet/omelet
cmake --preset=dev
cmake --build --preset=dev
```

## Next steps

Everything you find under [`omelet/`](omelet) is nothing but a template, and you might have to change all files
eventually. However, consider starting your journey in [`omelet/source/triangle.cpp`](omelet/source/triangle.cpp) and [
`omelet/source/world.cpp`](omelet/source/world.cpp), and then gradually work yourself through the includes, when needed.

When you feel lost, have a look into [`omelet/source/main.cpp`](omelet/source/main.cpp) where we define
`int main(int, char**)` and start the life of *the window*.

[1]: https://libsdl.org/

[2]: https://github.com/ocornut/imgui

[3]: https://github.com/cginternals/glbinding

[4]: https://github.com/g-truc/glm

[5]: https://cmake.org/

[6]: https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html
