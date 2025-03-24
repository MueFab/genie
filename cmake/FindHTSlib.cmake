# https://github.com/genome/build-common/blob/master/cmake/FindHTSlib.cmake
# - Try to find htslib
# Once done, this will define
#
#  htslib_FOUND - system has htslib
#  htslib_INCLUDE_DIRS - the htslib include directories
#  htslib_LIBRARIES - link these to use htslib

# Set search directories
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

# Find the include directory for HTSlib
find_path(HTSLIB_INCLUDE_DIR
    NAMES ${HTSLIB_ADDITIONAL_HEADERS} sam.h
    PATHS ${HTSLIB_SEARCH_DIRS}
    PATH_SUFFIXES include include/htslib htslib/${_htslib_ver_path}/htslib
    HINTS ENV HTSLIB_ROOT
)

# If the found include directory ends with "htslib", use its parent instead.
string(REGEX MATCH "/htslib$" _is_htslib_dir "${HTSLIB_INCLUDE_DIR}")
if(_is_htslib_dir)
    get_filename_component(HTSLIB_INCLUDE_DIR ${HTSLIB_INCLUDE_DIR} DIRECTORY)
endif()

# Find the library itself
find_library(HTSlib_LIBRARY
    NAMES hts libhts.a hts.a
    PATHS ${HTSLIB_INCLUDE_DIR} ${HTSLIB_SEARCH_DIRS}
    NO_DEFAULT_PATH
    PATH_SUFFIXES lib lib64 lib/x86_64-linux-gnu ${_htslib_ver_path}
    HINTS ENV HTSLIB_ROOT
)

# Optional: set ZLIB_LIBRARY if needed
#set(ZLIB_LIBRARY "/usr/local/lib/libz-ng.so")
#set(HTSlib_LIBRARY "/usr/local/lib/libz-ng.so")

# Process the found include dirs and libraries.
set(HTSlib_PROCESS_INCLUDES HTSLIB_INCLUDE_DIR ZLIB_INCLUDE_DIR)
set(HTSlib_PROCESS_LIBS HTSlib_LIBRARY ZLIB_LIBRARIES)
libfind_process(HTSlib)
message(STATUS "   HTSlib include dirs: ${HTSlib_INCLUDE_DIRS}")
message(STATUS "   HTSlib libraries: ${HTSlib_LIBRARIES}")
