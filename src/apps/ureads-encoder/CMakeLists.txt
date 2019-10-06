project("ureads-encoder")

set(source_files
        encoding.cc
        fastq-file-reader.cc
        fastq-record.cc
        file.cc
        file-reader.cc
        genie-gabac-output-stream.cc
        input-file.cc
        log.cc
        main.cc
        output-file.cc
        program-options.cc
)

add_executable(ureads-encoder ${source_files})

target_link_libraries(ureads-encoder PRIVATE format)
target_link_libraries(ureads-encoder PRIVATE util)
target_link_libraries(ureads-encoder PRIVATE gabac)
