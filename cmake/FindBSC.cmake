# Look for the libbsc header and library
find_path(BSC_INCLUDE_DIR libbsc.h)
find_library(BSC_LIBRARY NAMES bsc libbsc)

# Check if the library and header were found
if(BSC_INCLUDE_DIR AND BSC_LIBRARY)
    set(BSC_FOUND TRUE)
    set(BSC_LIBRARIES ${BSC_LIBRARY})
    set(BSC_INCLUDE_DIRS ${BSC_INCLUDE_DIR})
else()
    set(BSC_FOUND FALSE)
endif()

# Output the status messages
message(STATUS "   BSC include dirs: ${BSC_INCLUDE_DIRS}")
message(STATUS "   BSC libraries: ${BSC_LIBRARIES}")

# If REQUIRED is passed and BSC is not found, fail with an error
if(NOT BSC_FOUND AND REQUIRED)
    message(FATAL_ERROR "Could not find libbsc. Please make sure the library and headers are installed.")
endif()

# Mark the found paths as advanced (optional)
mark_as_advanced(BSC_INCLUDE_DIR BSC_LIBRARY)
