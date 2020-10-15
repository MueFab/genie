project("gabac-app")

set(source_files
        code.cc
        main.cc
        program-options.cc
        )

add_executable(gabac-app ${source_files})

target_link_libraries(gabac-app PRIVATE genie-gabac)
