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
        literal.h
        log.h
        log.cc
        circular_buffer.impl.h
        dynamic_scheduler.cc
        merge_sort/chunk.h
        merge_sort/chunk_file.h
        merge_sort/chunk_memory.h
        merge_sort/builder.h
        merge_sort/merger.h
        merge_sort/chunk_file.impl.h
        merge_sort/chunk_memory.impl.h
        merge_sort/builder.impl.h
        merge_sort/merger.impl.h
)

add_library(genie-util ${source_files})
find_package(Threads)
target_link_libraries(genie-util Threads::Threads)
get_filename_component(TOP_DIR ../../ ABSOLUTE)
target_include_directories(genie-util PUBLIC "${TOP_DIR}")
