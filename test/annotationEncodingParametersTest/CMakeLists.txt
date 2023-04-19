project("annotationEncodingParameters-tests")

set(source_files
        RandomRecordFillIn.cc
        annotationEncodingParameters-test.cc
        main.cc
)

add_executable(annotationEncodingParameters-tests ${source_files})

target_link_libraries(annotationEncodingParameters-tests PRIVATE gtest_main)
target_link_libraries(annotationEncodingParameters-tests PRIVATE genie-core)
target_link_libraries(annotationEncodingParameters-tests PRIVATE genie-util)
