project("annotationAccessUnit-tests")

set(source_files
        annotationAccesUnit-test.cc
        main.cc
)

add_executable(annotationAccessUnit-tests ${source_files})

target_link_libraries(annotationAccessUnit-tests PRIVATE gtest_main)
