project("genie-fasta")

set(source_files
        reader.cc
        record.cc)

add_library(genie-fasta ${source_files})

target_link_libraries(genie-fasta PUBLIC genie-core)

