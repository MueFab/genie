project("sam")

set(source_files
        sam-exporter.cc
        sam-header.cc
        sam-header-info.cc
        sam-importer.cc
        sam-reader.cc
        sam-record.cc
        sam-tag.cc
        sam-writer.cc
        tag-number.cc
        tag-string.cc
        tag-string-list.cc
        tag-version.cc)

add_library(sam ${source_files})


target_link_libraries(sam PUBLIC backbone)
