project("genie-util")

set(source_files
        bitreader.cc
        bitwriter.cc
        data-block.cc
        date.cc
        exceptions.cc
        log.cc
        ordered-lock.cc
        ordered-section.cc
        perf-stats.cc
        string-helpers.cc
        stringview.cc
        thread-manager.cc
        tokenize.cc
        )

add_library(genie-util ${source_files})
find_package(Threads)
target_link_libraries(genie-util Threads::Threads)