project("genie-lzma")

set(source_files
        decoder.cc
        encoder.cc
        param_decoder.cc
        subsequence.cc)

if (${GABAC_BUILD_SHARED_LIB})
    add_library(genie-lzma SHARED ${source_files})
else ()
    add_library(genie-lzma STATIC ${source_files})
endif ()

find_package(LZMA REQUIRED)

target_link_libraries(genie-lzma PUBLIC genie-core)
target_link_libraries(genie-lzma PUBLIC ${LZMA_LIBRARIES})

get_filename_component(THIS_DIR ./ ABSOLUTE)
get_filename_component(TOP_DIR ../../../ ABSOLUTE)
target_include_directories(genie-lzma PUBLIC "${THIS_DIR}")
target_include_directories(genie-lzma PUBLIC "${TOP_DIR}")