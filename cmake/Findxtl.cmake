# 1. Look for xtl locally on the system/conda prefix first
find_path(XTL_INCLUDE_DIR
    NAMES xtl/xbasic_meta_style.hpp
    HINTS
        $ENV{CONDA_PREFIX}/include
    PATH_SUFFIXES .
)

if(XTL_INCLUDE_DIR)
    set(xtl_FOUND TRUE)
    set(xtl_INCLUDE_DIRS ${XTL_INCLUDE_DIR})
    if(NOT TARGET xtl)
        add_library(xtl INTERFACE IMPORTED)
        set_target_properties(xtl PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${XTL_INCLUDE_DIR}"
        )
    endif()
else()
    # Clean cache variables to avoid NOTFOUND pollution
    unset(XTL_INCLUDE_DIR CACHE)

    # 2. Check if thirdparty folder exists
    if(EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/xtl")
        set(xtl_FOUND TRUE)
        set(xtl_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/thirdparty/xtl/include")
        if(NOT TARGET xtl)
            add_library(xtl INTERFACE IMPORTED)
            set_target_properties(xtl PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/thirdparty/xtl/include"
            )
        endif()
    else()
        # 3. Conditional download fallback via FetchContent
        set(XTENSOR_SELECTED FALSE)
        if(GENIE_BACKEND STREQUAL "xtensor" OR GENIE_CONTACT_BACKEND STREQUAL "xtensor" OR GENIE_GENOTYPE_BACKEND STREQUAL "xtensor" OR GENIE_LIKELIHOOD_BACKEND STREQUAL "xtensor")
            set(XTENSOR_SELECTED TRUE)
        endif()

        if(xtl_FIND_REQUIRED OR BUILD_TESTS OR XTENSOR_SELECTED)
            include(FetchContent)
            FetchContent_Declare(
                xtl
                GIT_REPOSITORY https://www.github.com/xtensor-stack/xtl
                GIT_TAG fea39142693fbbc2ef19d75012bc6b46ef0a5f8c # v0.7.5
            )
            set(BUILD_TESTS OFF)
            FetchContent_MakeAvailable(xtl)
            set(xtl_FOUND TRUE)
            set(xtl_INCLUDE_DIRS "${xtl_SOURCE_DIR}/include")
        else()
            message(STATUS "xtl not found, but is not required by active backend configuration.")
        endif()
    endif()
endif()