project("genie-fasta")

set(source_files
        exporter.cc
        fai_file.cc
        fasta_source.cc
        manager.cc
        reader.cc
        reference.cc
        sha256File.cc
        ../sam/sam_sorter.h)

add_library(genie-fasta ${source_files})

target_link_libraries(genie-fasta PUBLIC genie-core)

