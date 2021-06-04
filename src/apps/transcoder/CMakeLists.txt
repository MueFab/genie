project("transcoder")

set(
    source_files
    main.cc
    transcoding.h
    utils.cc
    utils.h
    sam/sam_to_mgrec/program-options.cc
    sam/sam_to_mgrec/sam_record.h
    sam/sam_to_mgrec/sam_record.cc
    sam/sam_to_mgrec/transcoder.cc
    sam/sam_to_mgrec/transcoder.h
    sam/sam_to_mgrec/sorter.cc
    sam/sam_to_mgrec/sorter.h
    sam/sam_to_mgrec/sam_reader.h
    sam/sam_to_mgrec/sam_reader.cc
    sam/sam_to_mgrec/sam_group.h
    sam/sam_to_mgrec/sam_group.cc)

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