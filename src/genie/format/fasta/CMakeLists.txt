project("genie-fasta")

set(source_files
        fai-file.cc
        manager.cc
        reader.cc
        reference.cc
        )

add_library(genie-fasta ${source_files})

target_link_libraries(genie-fasta PUBLIC genie-core)

