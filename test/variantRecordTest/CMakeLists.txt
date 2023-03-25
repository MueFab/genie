project("VariantRecord-tests")

set(source_files
        variantRecord-test.cc
        main.cc
)

add_executable(VariantRecord-tests ${source_files})

target_link_libraries(VariantRecord-tests PRIVATE gtest_main)
target_link_libraries(VariantRecord-tests PRIVATE genie-core)
target_link_libraries(VariantRecord-tests PRIVATE genie-util)
