project("genie-sam")

set(source_files
        exporter.cc
        importer.cc
        sam_reader.cc
        sam_record.cc
        sam_sorter.cc
        pair_matcher.cc
        unmapped_record_matcher.cc
        sam_tag.cc
)

add_library(genie-sam ${source_files})

target_link_libraries(genie-sam PUBLIC genie-core)

find_package(HTSlib REQUIRED)  # NOLINT
target_include_directories(genie-sam PUBLIC ${HTSlib_INCLUDE_DIRS})
target_link_libraries(genie-sam PUBLIC ${HTSlib_LIBRARIES})