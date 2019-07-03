project("ureads-encoder")

set(source_files
        encoding.cpp
        exceptions.cpp
        fastq_file_reader.cpp
        fastq_record.cpp
        file.cpp
        file_reader.cpp
        genie_gabac_output_stream.cpp
        input_file.cpp
        log.cpp
        main.cpp
        output_file.cpp
        program_options.cpp
)

add_executable(ureads-encoder ${source_files})

target_link_libraries(ureads-encoder PRIVATE format)
target_link_libraries(ureads-encoder PRIVATE gabac)
