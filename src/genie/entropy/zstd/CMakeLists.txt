project("genie-zstd")

set(source_files
        decoder.cc
        encoder.cc)

if (${GABAC_BUILD_SHARED_LIB})
    add_library(genie-zstd SHARED ${source_files})
else ()
    add_library(genie-zstd STATIC ${source_files})
endif ()

target_link_libraries(genie-zstd PUBLIC genie-core)

get_filename_component(THIS_DIR ./ ABSOLUTE)
get_filename_component(TOP_DIR ../../../ ABSOLUTE)
target_include_directories(genie-zstd PUBLIC "${THIS_DIR}")
target_include_directories(genie-zstd PUBLIC "${TOP_DIR}")