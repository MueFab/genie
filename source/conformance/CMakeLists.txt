set(source_files
        encoding.cpp
        exceptions.cpp
        fastq_file_reader.cpp
        fastq_record.cpp
        file_reader.cpp
        genie_gabac_output_stream.cc
        log.cpp
        main.cpp
        program_options.cpp)

add_executable(conformance ${source_files})
target_include_directories(conformance PRIVATE ${CMAKE_SOURCE_DIR}/source)
target_include_directories(conformance PRIVATE ${CMAKE_SOURCE_DIR}/source/format)
target_include_directories(conformance PRIVATE ${CMAKE_BINARY_DIR}/gabac-src/source)
target_link_libraries(conformance format)
target_link_libraries(conformance gabac)
