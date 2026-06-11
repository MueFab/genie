project("genie-util")

set(source_files
        block_stepper.cc
        bit_reader.cc
        bit_writer.cc
        data_block.cc
        exception.cc
        ordered_lock.cc
        ordered_section.cc
        runtime_exception.cc
        string_helpers.cc
        thread_manager.cc
        stop_watch.cc
        log.cc
        dynamic_scheduler.cc
        zlib/streambuffer.cc
        zlib/istream.cc
        zlib/ostream.cc
)

add_library(genie-util ${source_files})
find_package(Threads)
find_package(ZLIB REQUIRED)
message(STATUS "DEBUG: ZLIB_INCLUDE_DIRS = '${ZLIB_INCLUDE_DIRS}'")
message(STATUS "DEBUG: ZLIB_LIBRARIES = '${ZLIB_LIBRARIES}'")
target_include_directories(genie-util PRIVATE ${ZLIB_INCLUDE_DIRS})
target_link_libraries(genie-util Threads::Threads ${ZLIB_LIBRARIES})
get_filename_component(TOP_DIR ../../ ABSOLUTE)
target_include_directories(genie-util PUBLIC "${TOP_DIR}")
