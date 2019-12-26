project("util")

set(source_files
        bitreader.cc
        bitwriter.cc
        exceptions.cc
        log.cc
        perf-stats.cc
        ../format/sam/sam-file-reader.cc
        ../format/sam/sam-record.cc
        string-helpers.cc
        thread-manager.cc
        ordered-lock.cc
        ordered-section.cc
)


find_package (Threads)

add_library(util ${source_files})
target_link_libraries (util Threads::Threads)