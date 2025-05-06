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
        hex_string.h
        hex_string.cc
)

add_library(genie-util ${source_files})
find_package(Threads)
target_link_libraries(genie-util Threads::Threads)
get_filename_component(TOP_DIR ../../ ABSOLUTE)
target_include_directories(genie-util PUBLIC "${TOP_DIR}")
