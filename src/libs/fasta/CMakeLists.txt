project("fasta")

set(source_files
        fasta-file-reader.cc
        fasta-record.cc)

add_library(fasta ${source_files})

target_link_libraries(fasta PUBLIC backbone)

