project("util")

set(source_files
        bitreader.cc
        bitwriter.cc
        exceptions.cc
        ../format/fasta/fasta-file-reader.cc
        ../format/fasta/fasta-record.cc
        ../format/fastq/fastq-file-reader.cc
        ../format/fastq/fastq-record.cc
        file-reader.cc
        log.cc
        perf-stats.cc
        ../format/sam/sam-file-reader.cc
        ../format/sam/sam-record.cc
        string-helpers.cc
)

add_library(util ${source_files})
