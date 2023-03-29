project("VariantGenotypeRecord-tests")

set(source_files
        variantGenotypeRecord-test.cc
        main.cc
)

add_executable(VariantGenotypeRecord-tests ${source_files})

target_link_libraries(VariantGenotypeRecord-tests PRIVATE gtest_main)
target_link_libraries(VariantGenotypeRecord-tests PRIVATE genie-core)
target_link_libraries(VariantGenotypeRecord-tests PRIVATE genie-util)
