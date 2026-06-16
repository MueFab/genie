# Look for the zstd header and library in the conda environment/system first
find_path(ZSTD_INCLUDE_DIR
    NAMES zstd.h
    HINTS
        $ENV{CONDA_PREFIX}/include
    PATH_SUFFIXES .
)

find_library(ZSTD_LIBRARY
    NAMES zstd
    HINTS
        $ENV{CONDA_PREFIX}/lib
    PATH_SUFFIXES .
)

if(ZSTD_INCLUDE_DIR AND ZSTD_LIBRARY)
    set(ZSTD_FOUND TRUE)
    set(Zstd_LIBRARIES ${ZSTD_LIBRARY})
    set(Zstd_INCLUDE_DIRS ${ZSTD_INCLUDE_DIR})
else()
    # Clean cache variables to avoid NOTFOUND pollution
    unset(ZSTD_INCLUDE_DIR CACHE)
    unset(ZSTD_LIBRARY CACHE)

    # 2. Check if thirdparty folder exists
    if(EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/zstd")
        if(NOT TARGET zstd)
            add_subdirectory("${CMAKE_SOURCE_DIR}/thirdparty/zstd" "${CMAKE_BINARY_DIR}/thirdparty/zstd")
        endif()
        set(ZSTD_FOUND TRUE)
        set(Zstd_LIBRARIES libzstd_static)
        set(Zstd_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/thirdparty/zstd/lib")
    else()
        # 3. Download fallback via FetchContent
        include(FetchContent)
        FetchContent_Declare(
            zstd
            GIT_REPOSITORY https://github.com/facebook/zstd.git
            GIT_TAG        v1.5.5
            SOURCE_SUBDIR  build/cmake
        )
        # Configure options for zstd build
        set(ZSTD_BUILD_PROGRAMS OFF CACHE BOOL "Disable zstd programs" FORCE)
        set(ZSTD_BUILD_TESTS OFF CACHE BOOL "Disable zstd tests" FORCE)
        set(ZSTD_BUILD_SHARED OFF CACHE BOOL "Disable zstd shared lib" FORCE)
        set(ZSTD_BUILD_STATIC ON CACHE BOOL "Enable zstd static lib" FORCE)

        FetchContent_MakeAvailable(zstd)

        set(ZSTD_FOUND TRUE)
        set(Zstd_LIBRARIES libzstd_static)
        set(Zstd_INCLUDE_DIRS "${zstd_SOURCE_DIR}/lib")
    endif()
endif()

message(STATUS "   ZSTD include dirs: ${Zstd_INCLUDE_DIRS}")
message(STATUS "   ZSTD libraries: ${Zstd_LIBRARIES}")

mark_as_advanced(ZSTD_INCLUDE_DIR ZSTD_LIBRARY)
