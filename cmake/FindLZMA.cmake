# Look for the lzma header and library in the conda environment/system first
find_path(LZMA_INCLUDE_DIR
    NAMES lzma.h
    HINTS
        $ENV{CONDA_PREFIX}/include
    PATH_SUFFIXES .
)

find_library(LZMA_LIBRARY
    NAMES lzma liblzma
    HINTS
        $ENV{CONDA_PREFIX}/lib
    PATH_SUFFIXES .
)

# Check if the library and header were found
if(LZMA_INCLUDE_DIR AND LZMA_LIBRARY)
    set(LZMA_FOUND TRUE)
    set(LZMA_LIBRARIES ${LZMA_LIBRARY})
    set(LZMA_INCLUDE_DIRS ${LZMA_INCLUDE_DIR})
else()
    # Clean cache variables to avoid NOTFOUND pollution
    unset(LZMA_INCLUDE_DIR CACHE)
    unset(LZMA_LIBRARY CACHE)

    # 2. Check if thirdparty folder exists
    if(EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/lzma")
        if(NOT TARGET lzma)
            add_subdirectory("${CMAKE_SOURCE_DIR}/thirdparty/lzma" "${CMAKE_BINARY_DIR}/thirdparty/lzma")
        endif()
        set(LZMA_FOUND TRUE)
        set(LZMA_LIBRARIES liblzma)
        set(LZMA_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/thirdparty/lzma/src/liblzma/api")
    else()
        # 3. Download fallback via FetchContent
        include(FetchContent)
        FetchContent_Declare(
            lzma
            GIT_REPOSITORY https://github.com/tukaani-project/xz.git
            GIT_TAG        v5.6.1
        )
        set(BUILD_TESTING OFF CACHE BOOL "Disable testing" FORCE)

        FetchContent_MakeAvailable(lzma)

        set(LZMA_FOUND TRUE)
        set(LZMA_LIBRARIES liblzma)
        set(LZMA_INCLUDE_DIRS "${lzma_SOURCE_DIR}/src/liblzma/api")
    endif()
endif()

# Output the status messages
message(STATUS "   LZMA include dirs: ${LZMA_INCLUDE_DIRS}")
message(STATUS "   LZMA libraries: ${LZMA_LIBRARIES}")

# If REQUIRED is passed and LZMA is not found, fail with an error
if(NOT LZMA_FOUND AND LZMA_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find LZMA. Please make sure the library and headers are installed.")
endif()

# Mark the found paths as advanced
mark_as_advanced(LZMA_INCLUDE_DIR LZMA_LIBRARY)
