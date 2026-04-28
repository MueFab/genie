add_subdirectory(core)
add_subdirectory(variant)
add_subdirectory(example)
add_subdirectory(records)
add_subdirectory(genotype)
add_subdirectory(likelihood)
add_subdirectory(contact)
add_subdirectory(entropy)
add_subdirectory(conformance)
add_subdirectory(annotation)

enable_testing()

file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*_test.cc")

foreach(SOURCE ${TEST_SOURCES})
    get_filename_component(NAME ${SOURCE} NAME_WE)
    string(REPLACE "_" "-" TARGET_NAME ${NAME})
    set(TARGET_NAME "genie-test-${TARGET_NAME}")
    add_executable(${TARGET_NAME} ${SOURCE})
    
    # Generic linking for these root tests
    target_link_libraries(${TARGET_NAME} PRIVATE gtest_main genie-core genie-util)
    
    # Register test with CTest
    add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
endforeach()
