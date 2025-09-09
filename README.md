# Omlete 🍳

This is a highly opinionated [CMake][5] template using *contemporary* C++ to wrap "modern" OpenGL, [SDL3][1], [Dear ImGui][2], [glbindings][3], and [GLM][4].

## Template or Library?

Consider this repository as a template that helps you creating a canonical *Hello World* in the context of OpenGL programming: a colorful rotating triangle!

![A colorful rotation triangle](https://github.com/avitase/omelet/blob/main/screenshot.png?raw=true)

In contrast to a library or a full-fledged game engine, this template does note even try to hide OpenGL details from you but instead provides you with a *good* (this is the part that is highly opinionated) starting point with convenient helper functions that wraps, for instance, the annoying VAO/VBO fiddling.
However, none of the files under [`omlete/`](omlete/) should be considered out of bounds: Open them, read them, change them! (Maybe except for [`omelet/source/glsl_program.cpp`](omelet/source/glsl_program.cpp); here be dragons🐉)

## How to use it?

Check out the repository **including its various submodules** and build it with [CMake][5].
A layman's approach (on Linux🐧) is to type:

```bash
git clone --recurse-submodules https://github.com/avitase/omelet
mkdir -p omelet/build && cd omelet/build/
cmake .. && make -j
```

If you have installed OpenGL on your system, this will build [SDL3][1], [Dear ImGui][2], [glbindings][3], and [GLM][4] and finally compile the [template in omelet/](omelet/). (If not, it will fail spectacularly💥)

Compiling the dependencies does take some time but will eventually create an executable under `build/install/bin`.
Run it, it's safe!✋

```
./install/bin/omelete  # in omelet/build/
```

You should see a spinning triangle and a [Dear ImGui window][2] with the position of your cursor and an FPS counter. 

## How to use it properly?

This template will give you a CMake Superbuild that builds [SDL3][1], [Dear ImGui][2], [glbindings][3], and [GLM][4] as dependencies and the actual SDL3/OpenGL template.
Typically, building the Superbuild only has to be done once, you can then navigate to [`omelete/`](omelet/) (the subdirectory) and start fooling around here.

The subproject [`omelet/`](omelet/) expects to find its dependencies under `build/install`.
This happens automatically if you use our [CMakeUserPresets.json.EXAMPLE][6] (click on the link to learn more about what this is), e.g., by copying it: 

```bash
# project root (not omelet/omelet!)
cp CMakeUserPresets.json.EXAMPLE CMakeUserPresets.json
```

This will make a new preset `release` available that you can use to build the Superbuild:

```bash
cmake --preset=release
cmake --build --preset=release
```

As before, this should install everything under `build/install` and you can now navigate to the subproject [`omelet/`](omelet/).
Here, you'll find again a `CMakeUserPresets.json.EXAMPLE` that sets `CMAKE_PREFIX_PATH` for you and exposes two presets `dev` and `dev-clang-tidy`.
Feel free to change these presets to your own needs and then run, e.g.,

```bash
# run this in omelet/omelet
cmake --preset=dev
cmake --build --preset=dev
```

## Next steps

Everything you find under [`omelet/`](omelet) is nothing but a template, and you might have to change all files eventually.
However, consider starting your journey in [`omelet/source/triangle.cpp`](omelet/source/triangle.cpp) and [`omelet/source/world.cpp`](omelet/source/world.cpp), and then gradually work yourself through the includes, when needed.

When you feel lost, have a look into [`omelet/source/main.cpp`](omelet/source/main.cpp) where we define
`int main(int, char**)` and start the life of *the window*.

## Seeing link-time errors?

The super build script is not bullet-proof and assumes that all your dependencies that are build during the process are linked statically.
If this is not possible, one has to adopt the `rpath`, e.g., by adding the following the build recipe:

```
ExternalProject_Add(
    omelet
    [...]
    CMAKE_ARGS
        [...]
        -DCMAKE_INSTALL_RPATH:STRING=<INSTALL_DIR>/lib;<INSTALL_DIR>/lib64
        -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON
```

[1]: https://libsdl.org/

[2]: https://github.com/ocornut/imgui

[3]: https://github.com/cginternals/glbinding

[4]: https://github.com/g-truc/glm

[5]: https://cmake.org/

[6]: https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html
