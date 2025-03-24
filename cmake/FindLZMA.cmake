# Look for the libbsc header and library
find_path(LZMA_INCLUDE_DIR libbsc.h)
find_library(LZMA_LIBRARY NAMES lzma liblzma)

# Check if the library and header were found
if(LZMA_INCLUDE_DIR AND LZMA_LIBRARY)
    set(LZMA_FOUND TRUE)
    set(LZMA_LIBRARIES ${LZMA_LIBRARY})
    set(LZMA_INCLUDE_DIRS ${LZMA_INCLUDE_DIR})
else()
    set(LZMA_FOUND FALSE)
endif()

# Output the status messages
message(STATUS "   LZMA include dirs: ${LZMA_INCLUDE_DIRS}")
message(STATUS "   LZMA libraries: ${LZMA_LIBRARIES}")

# If REQUIRED is passed and LZMA is not found, fail with an error
if(NOT LZMA_FOUND AND REQUIRED)
    message(FATAL_ERROR "Could not find libbsc. Please make sure the library and headers are installed.")
endif()

# Mark the found paths as advanced (optional)
mark_as_advanced(LZMA_INCLUDE_DIR LZMA_LIBRARY)
