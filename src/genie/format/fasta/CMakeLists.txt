project("genie-fasta")

set(source_files
        exporter.cc
        fai-file.cc
        fasta-source.cc
        manager.cc
        reader.cc
        reference.cc
        sha256File.cc)

add_library(genie-fasta ${source_files})

target_link_libraries(genie-fasta PUBLIC genie-core)

