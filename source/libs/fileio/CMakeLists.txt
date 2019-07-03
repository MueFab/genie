project("fileio")

set(source_files
        fasta_file_reader.cc
        fasta_record.cc
        fastq_file_reader.cc
        fastq_record.cc
        file_reader.cc
        sam_file_reader.cc
        sam_record.cc
)

add_library(fileio ${source_files})
