project("gabac-app")

set(source_files
        analyze.cpp
        code.cpp
        main.cpp
        program_options.cpp
)

add_executable(gabac-app ${source_files})
target_include_directories(gabac-app PRIVATE "${CMAKE_SOURCE_DIR}/source")
target_include_directories(gabac-app PRIVATE "${CMAKE_SOURCE_DIR}/source/coding/gabac")
target_link_libraries(gabac-app gabac)
