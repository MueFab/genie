project("transcoder")

set(
    source_files
    main.cc
    program-options.cc
    sam_record.h
    sam_record.cc
    sam_transcoder.cc
    sam_transcoder.h
    sam_sorter.cc
    sam_sorter.h
    sam_reader.h
    sam_reader.cc sam_group.h sam_group.cc)
add_executable(transcoder ${source_files})

include_directories(${LIBLZMA_INCLUDE_DIRS})
include_directories(${BZIP2_INCLUDE_DIRS})
include_directories(${ZLIB_INCLUDE_DIRS})

target_link_libraries(transcoder ${LIBLZMA_LIBRARIES})
target_link_libraries(transcoder ${BZIP2_LIBRARIES})
target_link_libraries(transcoder ${ZLIB_LIBRARIES})

add_dependencies(transcoder htslib)
include_directories(${CMAKE_BINARY_DIR}/htslib-build/include)
target_link_libraries(transcoder ${CMAKE_BINARY_DIR}/htslib-build/lib/libhts.so)

add_dependencies(transcoder genie-util)
target_link_libraries(transcoder genie-util)

add_dependencies(transcoder genie-core)
target_link_libraries(transcoder genie-core)