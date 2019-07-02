# Set the paths
set(MPEGGFileCreation "MPEGGFileCreation")
set(MPEGGFileCreation_source_dir ${CMAKE_SOURCE_DIR}/source/utils/MPEGGFileCreation)
set(MPEGGFileCreation_header_dir ${CMAKE_SOURCE_DIR}/source/utils/MPEGGFileCreation)
set(MPEGGFileCreation_include_dir ${CMAKE_SOURCE_DIR}/source/utils/MPEGGFileCreation ${CMAKE_SOURCE_DIR}/source)

# List all source files (alphabetically)
set(MPEGGFileCreation_source_files ${MPEGGFileCreation_source_files} ${MPEGGFileCreation_source_dir}/MPEGGFileCreator.cpp)

# List all header files (alphabetically)
set(MPEGGFileCreation_header_files ${MPEGGFileCreation_header_files} ${MPEGGFileCreation_header_dir}/MPEGGFileCreator.h)

# Group the source and header files
source_group("MPEGGFileCreation Source Files" FILES ${MPEGGFileCreation_source_files})
source_group("MPEGGFileCreation Header Files" FILES ${MPEGGFileCreation_header_files})

# Set up the target
add_library(${MPEGGFileCreation} STATIC ${MPEGGFileCreation_source_files})
target_include_directories(${MPEGGFileCreation} PRIVATE ${MPEGGFileCreation_include_dir})
target_link_libraries(${MPEGGFileCreation} ${format})
