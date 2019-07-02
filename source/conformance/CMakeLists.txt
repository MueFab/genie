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
        program_options.cpp)

add_executable(conformance ${source_files})
target_include_directories(conformance PRIVATE ${CMAKE_SOURCE_DIR}/source)
target_include_directories(conformance PRIVATE ${CMAKE_SOURCE_DIR}/source/coding/gabac)
target_include_directories(conformance PRIVATE ${CMAKE_SOURCE_DIR}/source/format)
target_link_libraries(conformance format)
target_link_libraries(conformance gabac)
