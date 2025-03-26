install(
        TARGETS omelet_exe
        RUNTIME COMPONENT omelet_Runtime
)

if (PROJECT_IS_TOP_LEVEL)
    include(CPack)
endif ()
