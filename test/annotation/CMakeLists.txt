project("genie-test-annotation")

set(source_files
        annotation-test.cc
		annotation-contact-test.cc
        helpers.cc
        main.cc

)

add_executable(${PROJECT_NAME} ${source_files})

target_link_libraries(${PROJECT_NAME} PRIVATE gtest_main)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-core)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-util)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-annotation)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-vsite)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-genotype)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-likelihood)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-bsc)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-lzma)
target_link_libraries(${PROJECT_NAME} PRIVATE genie-zstd)
target_link_libraries(${PROJECT_NAME} PRIVATE mpeggCodecs-static)
