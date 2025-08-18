cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    imgui
    VERSION 1.92.2
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)

add_library(
    imgui_imgui
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_opengl3.cpp
    imgui/backends/imgui_impl_sdl3.cpp
)
add_library(imgui::imgui ALIAS imgui_imgui)

target_compile_definitions(imgui_imgui PUBLIC IMGUI_STATIC_DEFINE)

set_target_properties(
    imgui_imgui PROPERTIES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME imgui
    OUTPUT_NAME imgui
    POSITION_INDEPENDENT_CODE ON
)

target_include_directories(
    imgui_imgui 
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/imgui>"
)

target_compile_features(imgui_imgui PUBLIC cxx_std_17)

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-static)
target_link_libraries(
        imgui_imgui
        PUBLIC
        SDL3::SDL3-static
)

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()
