# 1. Look for xtensor locally on the system/conda prefix first
find_path(XTENSOR_INCLUDE_DIR
    NAMES xtensor/xtensor.hpp
    HINTS
        $ENV{CONDA_PREFIX}/include
    PATH_SUFFIXES .
)

if(XTENSOR_INCLUDE_DIR)
    set(xtensor_FOUND TRUE)
    set(xtensor_INCLUDE_DIRS ${XTENSOR_INCLUDE_DIR})
    if(NOT TARGET xtensor)
        add_library(xtensor INTERFACE IMPORTED)
        set_target_properties(xtensor PROPERTIES
            INTERFACE_LINK_LIBRARIES xtl
            INTERFACE_INCLUDE_DIRECTORIES "${XTENSOR_INCLUDE_DIR}"
        )
    endif()
    if(NOT TARGET xtensor::optimize)
        add_library(xtensor::optimize ALIAS xtensor)
    endif()
else()
    # Clean cache variables to avoid NOTFOUND pollution
    unset(XTENSOR_INCLUDE_DIR CACHE)

    # 2. Check if thirdparty folder exists
    if(EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/xtensor")
        set(xtensor_FOUND TRUE)
        set(xtensor_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/thirdparty/xtensor/include")
        if(NOT TARGET xtensor)
            add_library(xtensor INTERFACE IMPORTED)
            set_target_properties(xtensor PROPERTIES
                INTERFACE_LINK_LIBRARIES xtl
                INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/thirdparty/xtensor/include"
            )
        endif()
        if(NOT TARGET xtensor::optimize)
            add_library(xtensor::optimize ALIAS xtensor)
        endif()
    else()
        # 3. Conditional download fallback via FetchContent
        set(XTENSOR_SELECTED FALSE)
        if(GENIE_BACKEND STREQUAL "xtensor" OR GENIE_CONTACT_BACKEND STREQUAL "xtensor" OR GENIE_GENOTYPE_BACKEND STREQUAL "xtensor" OR GENIE_LIKELIHOOD_BACKEND STREQUAL "xtensor")
            set(XTENSOR_SELECTED TRUE)
        endif()

        if(xtensor_FIND_REQUIRED OR BUILD_TESTS OR XTENSOR_SELECTED)
            include(FetchContent)
            FetchContent_Declare(
                xtensor
                GIT_REPOSITORY https://www.github.com/xtensor-stack/xtensor
                GIT_TAG d9c3782ed51027b2d00be3c26288b2f74e4dbe94 # v0.25.0
            )
            FetchContent_MakeAvailable(xtensor)
            set(xtensor_FOUND TRUE)
            set(xtensor_INCLUDE_DIRS "${xtensor_SOURCE_DIR}/include")
            if(NOT TARGET xtensor::optimize)
                add_library(xtensor::optimize ALIAS xtensor)
            endif()
        else()
            message(STATUS "xtensor not found, but is not required by active backend configuration.")
        endif()
    endif()
endif()