project("gabac-app")

set(source_files
        analyze.cpp
        code.cpp
        main.cpp
        program_options.cpp
)

add_executable(gabac-app ${source_files})

target_link_libraries(gabac-app PRIVATE gabac)
