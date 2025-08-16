cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    glad
    VERSION 1.0.0
    LANGUAGES C
)

include(cmake/project-is-top-level.cmake)

add_library(glad_glad src/egl.c src/gles2.c)
add_library(glad::glad ALIAS glad_glad)

set_target_properties(
    glad_glad PROPERTIES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME glad
    OUTPUT_NAME glad
    POSITION_INDEPENDENT_CODE ON
)

target_include_directories(
    glad_glad 
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()
