# 1. Look for xtensor-io locally on the system/conda prefix first
find_path(XTENSOR_IO_INCLUDE_DIR
    NAMES xtensor-io/xio_binary.hpp
    HINTS
        $ENV{CONDA_PREFIX}/include
    PATH_SUFFIXES .
)

if(XTENSOR_IO_INCLUDE_DIR)
    set(xtensor-io_FOUND TRUE)
    set(xtensor-io_INCLUDE_DIRS ${XTENSOR_IO_INCLUDE_DIR})
    if(NOT TARGET xtensor-io)
        add_library(xtensor-io INTERFACE IMPORTED)
        set_target_properties(xtensor-io PROPERTIES
            INTERFACE_LINK_LIBRARIES xtensor
            INTERFACE_INCLUDE_DIRECTORIES "${XTENSOR_IO_INCLUDE_DIR}"
        )
    endif()
else()
    # Clean cache variables to avoid NOTFOUND pollution
    unset(XTENSOR_IO_INCLUDE_DIR CACHE)

    # 2. Check if thirdparty folder exists
    if(EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/xtensor-io")
        set(xtensor-io_FOUND TRUE)
        set(xtensor-io_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/thirdparty/xtensor-io/include")
        if(NOT TARGET xtensor-io)
            add_library(xtensor-io INTERFACE IMPORTED)
            set_target_properties(xtensor-io PROPERTIES
                INTERFACE_LINK_LIBRARIES xtensor
                INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/thirdparty/xtensor-io/include"
            )
        endif()
    else()
        # 3. Conditional download fallback via FetchContent
        set(XTENSOR_SELECTED FALSE)
        if(GENIE_BACKEND STREQUAL "xtensor" OR GENIE_CONTACT_BACKEND STREQUAL "xtensor" OR GENIE_GENOTYPE_BACKEND STREQUAL "xtensor" OR GENIE_LIKELIHOOD_BACKEND STREQUAL "xtensor")
            set(XTENSOR_SELECTED TRUE)
        endif()

        if(xtensor-io_FIND_REQUIRED OR BUILD_TESTS OR XTENSOR_SELECTED)
            include(FetchContent)
            FetchContent_Declare(
                xtensor-io
                GIT_REPOSITORY https://github.com/xtensor-stack/xtensor-io
                GIT_TAG ffada93 # v0.13.0
            )
            FetchContent_MakeAvailable(xtensor-io)
            set(xtensor-io_FOUND TRUE)
            set(xtensor-io_INCLUDE_DIRS "${xtensor-io_SOURCE_DIR}/include")
        else()
            message(STATUS "xtensor-io not found, but is not required by active backend configuration.")
        endif()
    endif()
endif()