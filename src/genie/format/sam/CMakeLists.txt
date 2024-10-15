project("genie-sam")

set(source_files
        exporter.cc
        importer.cc)

add_library(genie-sam ${source_files})

target_link_libraries(genie-sam PUBLIC genie-core)