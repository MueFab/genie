project("genie-fastq")

set(source_files
        exporter.cc
        importer.cc)

add_library(genie-fastq ${source_files})

target_link_libraries(genie-fastq PUBLIC genie-core)