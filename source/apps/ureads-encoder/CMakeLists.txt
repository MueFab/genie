project("ureads-encoder")

set(source_files
        encoding.cc
        exceptions.cc
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
        part2_format.h part2_format.cc part2_format_access_unit.cc part2_format_access_unit.h part2_format_parameter_set.cc part2_format_parameter_set.h part2_format_bitwriter.cc part2_format_bitwriter.h part2_format_raw_reference.cc part2_format_raw_reference.h part2_format_data_unit.cc part2_format_data_unit.h)

add_executable(ureads-encoder ${source_files})
target_link_libraries(ureads-encoder PRIVATE gabac)
