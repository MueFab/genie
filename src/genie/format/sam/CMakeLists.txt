project("genie-sam")

set(source_files
        exporter.cc
        importer.cc
        sam_parameter.cc
        sam_to_mgrec/sam_group.cc
        sam_to_mgrec/sam_reader.cc
        sam_to_mgrec/sam_record.cc
        sam_to_mgrec/sorter.cc
        sam_to_mgrec/transcoder.cc
)

add_library(genie-sam ${source_files})

target_link_libraries(genie-sam PUBLIC genie-core)