cmake_minimum_required(VERSION 3.14)

project(omelet-superbuild)

include(ExternalProject)

ExternalProject_Add(
    glbinding
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/3rd-party/glbinding
    INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/install
    CMAKE_ARGS
        -DBUILD_SHARED_LIBS=OFF
        -DCMAKE_BUILD_TYPE=Release
        -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
        -DCMAKE_PREFIX_PATH:PATH=<INSTALL_DIR>
        -DOPTION_BUILD_EXAMPLES=OFF
        -DOPTION_BUILD_TOOLS=OFF
        -DOPTION_BUILD_OWN_KHR_HEADERS=ON
        -Wno-dev
)

ExternalProject_Add(
    glm
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/3rd-party/glm
    INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/install
    CMAKE_ARGS
        -DBUILD_SHARED_LIBS=OFF
        -DCMAKE_BUILD_TYPE=Release
        -DGLM_BUILD_TESTS=OFF
        -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
        -DCMAKE_PREFIX_PATH:PATH=<INSTALL_DIR>
)

ExternalProject_Add(
    imgui
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/3rd-party/imgui
    INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/install
    CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=Release
        -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
        -DCMAKE_PREFIX_PATH:PATH=<INSTALL_DIR>
)

ExternalProject_Add(
    omelet
    DEPENDS glbinding glm imgui
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/omelet
    INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/install
    CMAKE_ARGS
        -DBUILD_SHARED_LIBS=ON
        -DCMAKE_BUILD_TYPE=Release
        -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
        -DCMAKE_PREFIX_PATH:PATH=<INSTALL_DIR>
)
