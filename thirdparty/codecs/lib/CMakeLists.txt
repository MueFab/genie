cmake_minimum_required(VERSION 2.9.0)
project(mpeggCodecs)

set(SOURCE_LIB_TARGETS
        Api
        Libbsc
        Lzma
        Zstd
        jbigkit
        jbig-interface
        ) # SOURCE_LIB_TARGETS

include_directories(
        ${MPEGG_CODECS_INCLUDE_DIR})

set(SOURCE_LIB_OBJECTS)
foreach(source_lib_target ${SOURCE_LIB_TARGETS})
    set(SOURCE_LIB_OBJECTS ${SOURCE_LIB_OBJECTS} $<TARGET_OBJECTS:${source_lib_target}>)
endforeach(source_lib_target)

set(MPEGG_CODECS_LIB_TARGETS
        mpeggCodecs-static
        mpeggCodecs-shared)

foreach(mpeggCodecs_lib_target ${MPEGG_CODECS_LIB_TARGETS})
    string(REGEX REPLACE "^.*-" "" lib_type ${mpeggCodecs_lib_target})
    string(TOUPPER ${lib_type} lib_type)

    add_library(${mpeggCodecs_lib_target} ${lib_type} ${SOURCE_LIB_OBJECTS})
    # set_target_properties(${mpeggCodecs_lib_target} PROPERTIES PREFIX "")
    set_target_properties(${mpeggCodecs_lib_target} PROPERTIES OUTPUT_NAME "mpeggCodecs")
    if(NOT WIN32)
      target_link_libraries(${mpeggCodecs_lib_target} m)
    else()
      target_link_libraries(${mpeggCodecs_lib_target})

    endif()
endforeach(mpeggCodecs_lib_target)