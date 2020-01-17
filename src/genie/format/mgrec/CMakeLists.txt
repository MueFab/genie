project("genie-mgrec")

set(source_files
        exporter.cc
        importer.cc
        )

add_library(genie-mgrec ${source_files})

target_link_libraries(genie-mgrec PUBLIC genie-core)