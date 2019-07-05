set(source_files main.cpp)

add_executable(tests ${source_files})

target_link_libraries(tests examples)
