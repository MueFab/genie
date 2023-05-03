project("annotationParameterSet-tests")

set(source_files
        RandomRecordFillIn.cc
        annotationParameterSet-test.cc
        main.cc
)

add_executable(annotationParameterSet-tests ${source_files})

target_link_libraries(annotationParameterSet-tests PRIVATE gtest_main)
target_link_libraries(annotationParameterSet-tests PRIVATE genie-core)
target_link_libraries(annotationParameterSet-tests PRIVATE genie-util)
