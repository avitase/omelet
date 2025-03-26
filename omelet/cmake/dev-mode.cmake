include(cmake/folders.cmake)

add_custom_target(
        run-exe
        COMMAND omelet_exe
        VERBATIM
)
add_dependencies(run-exe omelet_exe)

include(cmake/lint-targets.cmake)

add_folders(Project)
