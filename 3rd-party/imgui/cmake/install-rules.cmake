if(PROJECT_IS_TOP_LEVEL)
  set(
      CMAKE_INSTALL_INCLUDEDIR "include/imgui-${PROJECT_VERSION}"
      CACHE STRING ""
  )
  set_property(CACHE CMAKE_INSTALL_INCLUDEDIR PROPERTY TYPE PATH)
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package imgui)

install(
    FILES
    imgui/imconfig.h
    imgui/imgui.h
    imgui/imgui_internal.h
    imgui/backends/imgui_impl_opengl3.h
    imgui/backends/imgui_impl_opengl3_loader.h
    imgui/backends/imgui_impl_sdl2.h
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT imgui_Development
)

install(
    TARGETS imgui_imgui
    EXPORT imguiTargets
    RUNTIME #
    COMPONENT imgui_Runtime
    LIBRARY #
    COMPONENT imgui_Runtime
    NAMELINK_COMPONENT imgui_Development
    ARCHIVE #
    COMPONENT imgui_Development
    INCLUDES #
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
    imgui_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${package}"
    CACHE STRING "CMake package config location relative to the install prefix"
)
set_property(CACHE imgui_INSTALL_CMAKEDIR PROPERTY TYPE PATH)
mark_as_advanced(imgui_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${imgui_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT imgui_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${imgui_INSTALL_CMAKEDIR}"
    COMPONENT imgui_Development
)

install(
    EXPORT imguiTargets
    NAMESPACE imgui::
    DESTINATION "${imgui_INSTALL_CMAKEDIR}"
    COMPONENT imgui_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
