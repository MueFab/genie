project("genie-sam")

set(source_files
        exporter.cc
        header/header.cc
        header/header-info.cc
        importer.cc
        reader.cc
        record.cc
        header/tag.cc
        writer.cc
        header/tag-number.cc
        header/tag-string.cc
        header/tag-string-list.cc
        header/tag-version.cc)

add_library(genie-sam ${source_files})


target_link_libraries(genie-sam PUBLIC genie-core)
