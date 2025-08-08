cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
        omelet
        VERSION 0.1.0
        LANGUAGES C CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)


# ---- Declare library ----

add_library(
        omelet_lib OBJECT
        source/events.cpp
        source/fonts.cpp
        source/glsl_program.cpp
        source/triangle.cpp
        source/window.cpp
        source/world.cpp
        source/overlays.cpp
)

target_include_directories(
        omelet_lib ${warning_guard}
        PUBLIC
        "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source>"
)

target_compile_features(omelet_lib PUBLIC cxx_std_23)


# ---- Dependencies ----

find_package(glbinding REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-static)
find_package(imgui 1.91.9 EXACT REQUIRED)

target_link_libraries(
        omelet_lib
        PUBLIC
        glbinding::glbinding
        glbinding::glbinding-aux
        glm::glm
        OpenGL::GL
        SDL3::SDL3-static
        imgui::imgui
)


# ---- Declare executable ----

add_executable(omelet_exe source/main.cpp)
add_executable(omelet::exe ALIAS omelet_exe)

set_property(TARGET omelet_exe PROPERTY OUTPUT_NAME omelet)

target_compile_features(omelet_exe PRIVATE cxx_std_23)
target_link_libraries(omelet_exe PRIVATE omelet_lib)


# ---- Install rules ----

if (NOT CMAKE_SKIP_INSTALL_RULES)
    include(cmake/install-rules.cmake)
endif ()


# ---- Developer mode ----

if (NOT omelet_DEVELOPER_MODE)
    return()
elseif (NOT PROJECT_IS_TOP_LEVEL)
    message(
            AUTHOR_WARNING
            "Developer mode is intended for developers of omelet"
    )
endif ()

include(cmake/dev-mode.cmake)
