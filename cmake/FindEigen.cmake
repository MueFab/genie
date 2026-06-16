# 1. Look for Eigen3 locally on the system/conda prefix first
find_path(EIGEN3_INCLUDE_DIR
    NAMES signature_of_eigen3_matrix_library
    HINTS
        $ENV{CONDA_PREFIX}/include
        $ENV{CONDA_PREFIX}/include/eigen3
    PATH_SUFFIXES eigen3 .
)

if(EIGEN3_INCLUDE_DIR)
    set(Eigen3_FOUND TRUE)
    set(EIGEN3_INCLUDE_DIRS ${EIGEN3_INCLUDE_DIR})
    if(NOT TARGET Eigen3::Eigen)
        add_library(Eigen3::Eigen INTERFACE IMPORTED)
        set_target_properties(Eigen3::Eigen PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${EIGEN3_INCLUDE_DIR}"
        )
    endif()
else()
    # Clean cache variables to avoid NOTFOUND pollution
    unset(EIGEN3_INCLUDE_DIR CACHE)

    # 2. Check if thirdparty folder exists
    if(EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/eigen")
        set(Eigen3_FOUND TRUE)
        set(EIGEN3_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/thirdparty/eigen")
        if(NOT TARGET Eigen3::Eigen)
            add_library(Eigen3::Eigen INTERFACE IMPORTED)
            set_target_properties(Eigen3::Eigen PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/thirdparty/eigen"
            )
        endif()
    else()
        # 3. Conditional download fallback via FetchContent
        set(EIGEN_SELECTED FALSE)
        if(GENIE_BACKEND STREQUAL "eigen" OR GENIE_CONTACT_BACKEND STREQUAL "eigen" OR GENIE_GENOTYPE_BACKEND STREQUAL "eigen" OR GENIE_LIKELIHOOD_BACKEND STREQUAL "eigen")
            set(EIGEN_SELECTED TRUE)
        endif()

        if(Eigen3_FIND_REQUIRED OR BUILD_TESTS OR EIGEN_SELECTED)
            include(FetchContent)
            FetchContent_Declare(
              Eigen
              GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
              GIT_TAG        3.4.0
              GIT_SHALLOW    TRUE
            )
            set(EIGEN_BUILD_TESTING OFF CACHE BOOL "Disable Eigen tests" FORCE)
            set(EIGEN_BUILD_DOC OFF CACHE BOOL "Disable Eigen documentation" FORCE)
            set(EIGEN_BUILD_PKGCONFIG OFF CACHE BOOL "Disable Eigen pkgconfig" FORCE)

            FetchContent_MakeAvailable(Eigen)
            
            set(Eigen3_FOUND TRUE)
            set(EIGEN3_INCLUDE_DIRS "${eigen_SOURCE_DIR}")
        else()
            message(STATUS "Eigen3 not found, but is not required by active backend configuration.")
        endif()
    endif()
endif()
