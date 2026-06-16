# Look for the libbsc header and library in the conda environment/system first
find_path(BSC_INCLUDE_DIR
    NAMES libbsc.h
    HINTS
        $ENV{CONDA_PREFIX}/include
    PATH_SUFFIXES .
)

find_library(BSC_LIBRARY
    NAMES bsc libbsc
    HINTS
        $ENV{CONDA_PREFIX}/lib
    PATH_SUFFIXES .
)

# Check if the library and header were found
if(BSC_INCLUDE_DIR AND BSC_LIBRARY)
    set(BSC_FOUND TRUE)
    set(BSC_LIBRARIES ${BSC_LIBRARY})
    set(BSC_INCLUDE_DIRS ${BSC_INCLUDE_DIR})
else()
    # Clean cache variables to avoid NOTFOUND pollution
    unset(BSC_INCLUDE_DIR CACHE)
    unset(BSC_LIBRARY CACHE)

    # 2. Check if thirdparty folder exists
    if(EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/libbsc")
        if(NOT TARGET bsc)
            add_subdirectory("${CMAKE_SOURCE_DIR}/thirdparty/libbsc" "${CMAKE_BINARY_DIR}/thirdparty/libbsc")
        endif()
        set(BSC_FOUND TRUE)
        set(BSC_LIBRARIES bsc)
        set(BSC_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/thirdparty/libbsc")
    else()
        # 3. Download fallback via FetchContent
        include(FetchContent)
        FetchContent_Declare(
            bsc
            GIT_REPOSITORY https://github.com/shanshan-i/libbsc.git
            GIT_TAG        master
        )
        FetchContent_MakeAvailable(bsc)
        set(BSC_FOUND TRUE)
        set(BSC_LIBRARIES bsc)
        set(BSC_INCLUDE_DIRS "${bsc_SOURCE_DIR}")
    endif()
endif()

# Output the status messages
message(STATUS "   BSC include dirs: ${BSC_INCLUDE_DIRS}")
message(STATUS "   BSC libraries: ${BSC_LIBRARIES}")

# If REQUIRED is passed and BSC is not found, fail with an error
if(NOT BSC_FOUND AND BSC_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find libbsc. Please make sure the library and headers are installed.")
endif()

# Mark the found paths as advanced
mark_as_advanced(BSC_INCLUDE_DIR BSC_LIBRARY)

