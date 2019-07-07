project("fio")

set(source_files
        fasta-file-reader.cc
        fasta-record.cc
        fastq-file-reader.cc
        fastq-record.cc
        file-reader.cc
        sam-file-reader.cc
        sam-record.cc
)

add_library(fio ${source_files})

target_link_libraries(fio utils)
