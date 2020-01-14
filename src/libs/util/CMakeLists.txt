project("util")

set(source_files
        bitreader.cc
        bitwriter.cc
        exceptions.cc
        log.cc
        perf-stats.cc
        string-helpers.cc
        thread-manager.cc
        ordered-lock.cc
        ordered-section.cc
        date.cc
        tokenize.cc
        locus.cc
        )


find_package(Threads)

add_library(util ${source_files})
target_link_libraries(util Threads::Threads)