set(source_files main.cpp)

add_executable(lae ${source_files})
target_include_directories(lae PRIVATE ${CMAKE_SOURCE_DIR}/source)
