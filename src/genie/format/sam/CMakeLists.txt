project("genie-sam")

set(source_files
        exporter.cc
        importer.cc
        sam_reader.cc
        sam_record.cc
        sam_sorter.cc
        pair_matcher.h
        pair_matcher.cc
        unmapped_record_matcher.h
        unmapped_record_matcher.cc
        record_queue.impl.h
)

add_library(genie-sam ${source_files})

target_link_libraries(genie-sam PUBLIC genie-core)