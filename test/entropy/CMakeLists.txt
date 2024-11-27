project("entropy-tests")

set(source_files
        bsc-test.cc
        lzma-test.cc
        zstd-test.cc
        jbig-test.cc
        rans-test.cc
        main.cc
)

add_executable(entropy-tests ${source_files})

target_link_libraries(${PROJECT_NAME} PRIVATE gtest_main)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-core)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-util)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-bsc)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-lzma)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-jbig)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-zstd)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-rans)

