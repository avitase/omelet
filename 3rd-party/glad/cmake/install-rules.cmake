if(PROJECT_IS_TOP_LEVEL)
  set(
      CMAKE_INSTALL_INCLUDEDIR "include/glad-${PROJECT_VERSION}"
      CACHE STRING ""
  )
  set_property(CACHE CMAKE_INSTALL_INCLUDEDIR PROPERTY TYPE PATH)
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package glad)

install(
    DIRECTORY
    include/
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT glad_Development
)

install(
    TARGETS glad_glad
    EXPORT gladTargets
    RUNTIME #
    COMPONENT glad_Runtime
    LIBRARY #
    COMPONENT glad_Runtime
    NAMELINK_COMPONENT glad_Development
    ARCHIVE #
    COMPONENT glad_Development
    INCLUDES #
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
    glad_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${package}"
    CACHE STRING "CMake package config location relative to the install prefix"
)
set_property(CACHE glad_INSTALL_CMAKEDIR PROPERTY TYPE PATH)
mark_as_advanced(glad_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${glad_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT glad_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${glad_INSTALL_CMAKEDIR}"
    COMPONENT glad_Development
)

install(
    EXPORT gladTargets
    NAMESPACE glad::
    DESTINATION "${glad_INSTALL_CMAKEDIR}"
    COMPONENT glad_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
