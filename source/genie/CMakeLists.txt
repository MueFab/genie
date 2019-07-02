#==============================================================================
# genie
#==============================================================================

# Set the paths
set(genie "genie")
set(genie_source_dir ${CMAKE_SOURCE_DIR}/source )
set(genie_header_dir ${CMAKE_SOURCE_DIR}/source ${CMAKE_CURRENT_BINARY_DIR}/gabac-src/source)
set(genie_include_dir ${CMAKE_SOURCE_DIR}/source ${CMAKE_CURRENT_BINARY_DIR}/gabac-src/source)

# List all source files
set(genie_source_files ${genie_source_files} ${genie_source_dir}/fileio/fasta_file_reader.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/fileio/fasta_record.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/fileio/fastq_file_reader.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/fileio/fastq_record.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/fileio/file_reader.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/fileio/sam_file_reader.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/fileio/sam_record.cc)
#
set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/dsg.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/exceptions.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/gabac_integration.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/generation.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/genie_file_format.cc)
set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/ProgramOptions.cc)
if (NOT ${GENIE_USE_OPENMP})
    set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/thread_pool.cc)
endif ()
set(genie_source_files ${genie_source_files} ${genie_source_dir}/genie/utilities.cc)

# List all header files (alphabetically)
# TBD

# Group the source and header files
source_group("genie Source Files" FILES ${genie_source_files})
# source_group("genie Header Files" FILES ${genie_header_files})

# Set up the target
add_executable(${genie} ${genie_source_files})
if (${GENIE_USE_OPENMP})
    target_compile_definitions(${genie} PRIVATE GENIE_USE_OPENMP)
endif ()
target_include_directories(${genie} PRIVATE ${genie_include_dir})
# target_include_directories(${genie} PRIVATE ${CMAKE_BINARY_DIR}/gabac-src/source)
target_include_directories(genie PRIVATE ${CMAKE_SOURCE_DIR}/source/coding/gabac)

target_link_libraries(${genie} format)
target_link_libraries(${genie} ${file})
target_link_libraries(${genie} MPEGGFileCreation)
target_link_libraries(${genie} coding)
target_link_libraries(${genie} ${file})
target_link_libraries(${genie} gabac)
