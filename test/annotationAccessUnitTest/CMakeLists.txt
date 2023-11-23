project("annotationAccessUnit-tests")

set(source_files
        annotationAccessUnit-test.cc
		blockTests.cc
        main.cc
#        ../likelihood/encode-test.cc
)

add_executable(annotationAccessUnit-tests ${source_files})

target_link_libraries(annotationAccessUnit-tests PRIVATE gtest_main)
target_link_libraries(annotationAccessUnit-tests PRIVATE genie-core)
target_link_libraries(annotationAccessUnit-tests PRIVATE genie-util)
