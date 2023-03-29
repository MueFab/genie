project("VariantSiteRecord-tests")

set(source_files
        variantSiteRecord-test.cc
        main.cc
)

add_executable(VariantSiteRecord-tests ${source_files})

target_link_libraries(VariantSiteRecord-tests PRIVATE gtest_main)
target_link_libraries(VariantSiteRecord-tests PRIVATE genie-core)
target_link_libraries(VariantSiteRecord-tests PRIVATE genie-util)
