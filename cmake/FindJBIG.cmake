# Look for the jbig header and library in the conda environment/system first
find_path(JBIG_INCLUDE_DIR
    NAMES jbig.h
    HINTS
        $ENV{CONDA_PREFIX}/include
    PATH_SUFFIXES .
)

find_library(JBIG_LIBRARY
    NAMES jbig
    HINTS
        $ENV{CONDA_PREFIX}/lib
    PATH_SUFFIXES .
)

# Check if the library and header were found
if(JBIG_INCLUDE_DIR AND JBIG_LIBRARY)
    set(JBIG_FOUND TRUE)
    set(JBIG_LIBRARIES ${JBIG_LIBRARY})
    set(JBIG_INCLUDE_DIRS ${JBIG_INCLUDE_DIR})

    if(NOT TARGET jbigkit)
        add_library(jbigkit INTERFACE IMPORTED)
        set_target_properties(jbigkit PROPERTIES
            INTERFACE_LINK_LIBRARIES "${JBIG_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${JBIG_INCLUDE_DIR}"
        )
    endif()
else()
    # Clean cache variables to avoid NOTFOUND pollution
    unset(JBIG_INCLUDE_DIR CACHE)
    unset(JBIG_LIBRARY CACHE)

    # 2. Check if thirdparty folder exists
    if(EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/jbigkit")
        if(NOT TARGET jbigkit)
            add_subdirectory("${CMAKE_SOURCE_DIR}/thirdparty/jbigkit" "${CMAKE_BINARY_DIR}/thirdparty/jbigkit")
        endif()
        set(JBIG_FOUND TRUE)
        set(JBIG_LIBRARIES jbigkit)
        set(JBIG_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/thirdparty/jbigkit")
    else()
        # 3. Download fallback via FetchContent
        include(FetchContent)
        FetchContent_Declare(
            jbigkit
            URL      https://www.cl.cam.ac.uk/~mgk25/jbigkit/download/jbigkit-2.1.tar.gz
        )
        FetchContent_MakeAvailable(jbigkit)
        set(JBIG_FOUND TRUE)
        set(JBIG_LIBRARIES jbigkit)
        set(JBIG_INCLUDE_DIRS "${jbigkit_SOURCE_DIR}/libjbig")
    endif()
endif()

# Output the status messages
message(STATUS "   JBIG include dirs: ${JBIG_INCLUDE_DIRS}")
message(STATUS "   JBIG libraries: ${JBIG_LIBRARIES}")

# Mark the found paths as advanced
mark_as_advanced(JBIG_INCLUDE_DIR JBIG_LIBRARY)

