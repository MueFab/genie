project("genie-bsc")

set(source_files
        decoder.cc
        encoder.cc)

if (${GABAC_BUILD_SHARED_LIB})
    add_library(genie-bsc SHARED ${source_files})
else ()
    add_library(genie-bsc STATIC ${source_files})
endif ()

target_link_libraries(genie-bsc PUBLIC genie-core)

get_filename_component(THIS_DIR ./ ABSOLUTE)
get_filename_component(TOP_DIR ../../../ ABSOLUTE)
target_include_directories(genie-bsc PUBLIC "${THIS_DIR}")
target_include_directories(genie-bsc PUBLIC "${TOP_DIR}")