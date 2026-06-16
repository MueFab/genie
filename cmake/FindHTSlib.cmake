# https://github.com/genome/build-common/blob/master/cmake/FindHTSlib.cmake
# - Try to find htslib
# Once done, this will define
#
#  htslib_FOUND - system has htslib
#  htslib_INCLUDE_DIRS - the htslib include directories
#  htslib_LIBRARIES - link these to use htslib

set(HTSLIB_SEARCH_DIRS
        ${HTSLIB_SEARCH_DIRS}
        $ENV{HTLSIB_ROOT}
        $ENV{CONDA_PREFIX}
        /gsc/pkg/bio/htslib
        /usr
        /usr/local
        )

set(_htslib_ver_path "htslib-${htslib_FIND_VERSION}")
include(LibFindMacros)

# Dependencies
libfind_package(HTSlib ZLIB)

# Include dir
find_path(HTSlib_INCLUDE_DIR
        NAMES ${HTSLIB_ADDITIONAL_HEADERS} sam.h
        PATHS ${HTSLIB_SEARCH_DIRS}
        PATH_SUFFIXES
        include include/htslib htslib/${_htslib_ver_path}/htslib
        HINTS ENV HTSLIB_ROOT
        )

# Finally the library itself
find_library(HTSlib_LIBRARY
        NAMES hts libhts.a hts.a
        PATHS ${HTSlib_INCLUDE_DIR} ${HTSLIB_SEARCH_DIRS}
        NO_DEFAULT_PATH
        PATH_SUFFIXES lib lib64 lib/x86_64-linux-gnu ${_htslib_ver_path}
        HINTS ENV HTSLIB_ROOT
        )

if(HTSlib_INCLUDE_DIR AND HTSlib_LIBRARY)
    # Set the include dir variables and the libraries and let libfind_process do the rest.
    set(HTSlib_PROCESS_INCLUDES HTSlib_INCLUDE_DIR ZLIB_INCLUDE_DIR)
    set(HTSlib_PROCESS_LIBS HTSlib_LIBRARY ZLIB_LIBRARIES)
    libfind_process(HTSlib)
else()
    # Clean cache variables to avoid NOTFOUND pollution
    unset(HTSlib_INCLUDE_DIR CACHE)
    unset(HTSlib_LIBRARY CACHE)

    # 2. Check if thirdparty folder exists
    if(EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/htslib")
        if(NOT TARGET hts)
            add_subdirectory("${CMAKE_SOURCE_DIR}/thirdparty/htslib" "${CMAKE_BINARY_DIR}/thirdparty/htslib")
        endif()
        set(HTSlib_FOUND TRUE)
        set(HTSlib_LIBRARIES hts)
        set(HTSlib_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/thirdparty/htslib")
    else()
        # 3. Download fallback via FetchContent
        include(FetchContent)
        FetchContent_Declare(
            htslib
            GIT_REPOSITORY https://github.com/samtools/htslib.git
            GIT_TAG        1.19.1
            GIT_SHALLOW    TRUE
        )
        FetchContent_MakeAvailable(htslib)
        set(HTSlib_FOUND TRUE)
        set(HTSlib_LIBRARIES hts)
        set(HTSlib_INCLUDE_DIRS "${htslib_SOURCE_DIR}")
    endif()
endif()

message(STATUS "   HTSlib include dirs: ${HTSlib_INCLUDE_DIRS}")
message(STATUS "   HTSlib libraries: ${HTSlib_LIBRARIES}")
