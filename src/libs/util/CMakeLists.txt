project("util")

set(source_files
        bitwriter.cc
        data-block.cc
        exceptions.cc
        bitreader.cc
        string-helpers.cc
        ../backbone/locus.cc
        log.cc
        date.cc
        ordered-lock.cc
        thread-manager.cc
        ordered-section.cc
        perf-stats.cc
        tokenize.cc
        )


find_package(Threads)

add_library(util ${source_files})
target_link_libraries(util Threads::Threads)