project("genie")

set(source_files
        main.cc
        cli_handler.cc
        run/main.cc
        run/program-options.cc
        gabac/code.cc
        gabac/main.cc
        gabac/program-options.cc
        annotation/main.cc
        annotation/program-options.cc
        annotation/code.cc
        )

add_executable(${PROJECT_NAME} ${source_files})

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang") # GNU Compiler Collection
    target_compile_options(${PROJECT_NAME} PRIVATE "-pedantic")
    target_compile_options(${PROJECT_NAME} PRIVATE "-fno-common")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wall")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wshadow")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wextra")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wundef")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wconversion")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wdouble-promotion")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wno-error=conversion")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wno-error=maybe-uninitialized")
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE genie-core)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-util)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-annotation)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-bsc)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-lzma)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-zstd)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-module)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-gabac)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-jbig)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-vsite)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-genotype-local)
target_link_libraries(${PROJECT_NAME} PRIVATE mpeggCodecs-static)

#target_link_libraries(${PROJECT_NAME} PRIVATE libbsc)
#target_link_libraries(${PROJECT_NAME} PRIVATE Api)
# TODO: To be implemented later
#target_link_libraries(${PROJECT_NAME} PRIVATE genie-mgrec)
#target_link_libraries(${PROJECT_NAME} PRIVATE genie-mgg)
#target_link_libraries(${PROJECT_NAME} PRIVATE genie-mgb)

#target_link_libraries(${PROJECT_NAME} PRIVATE genie-spring)
#target_link_libraries(${PROJECT_NAME} PRIVATE genie-lowlatency)
#target_link_libraries(${PROJECT_NAME} PRIVATE genie-fastq)
#target_link_libraries(${PROJECT_NAME} PRIVATE genie-localassembly)
#target_link_libraries(${PROJECT_NAME} PRIVATE genie-qvwriteout)
#target_link_libraries(${PROJECT_NAME} PRIVATE genie-calq)

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION "usr/bin")
