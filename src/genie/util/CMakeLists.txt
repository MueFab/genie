project("genie-util")

set(source_files
        block-stepper.cc
        bitreader.cc
        bitwriter.cc
        data-block.cc
        date.cc
        exception.cc
        ordered-lock.cc
        ordered-section.cc
        runtime-exception.cc
        string-helpers.cc
        stringview.cc
        thread-manager.cc
        watch.cc
        )

add_library(genie-util ${source_files})
find_package(Threads)
target_link_libraries(genie-util Threads::Threads)
get_filename_component(TOP_DIR ../../ ABSOLUTE)
target_include_directories(genie-util PUBLIC "${TOP_DIR}")
