project("fastq")

set(source_files
        fastq-exporter.cc
        fastq-importer.cc)

add_library(fastq ${source_files})

target_link_libraries(fastq PUBLIC backbone)