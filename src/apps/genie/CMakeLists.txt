project("genie")

set(source_files
        dsg.cc
        generation.cc
        program-options.cc
        stream-saver.cc
        utilities.cc
        )

add_executable(genie ${source_files})

target_link_libraries(genie PRIVATE genie-gabac)
target_link_libraries(genie PRIVATE genie-spring)
target_link_libraries(genie PRIVATE genie-util)
target_link_libraries(genie PRIVATE pthread)

if (${GENIE_USE_OPENMP})
    target_compile_definitions(genie PRIVATE GENIE_USE_OPENMP)
    target_link_libraries(genie PRIVATE ${OpenMP_CXX_LIBRARIES})
endif ()
