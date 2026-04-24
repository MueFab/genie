project("genie-backend")

set(XTENSOR_USE_XSIMD 0)
set(XTENSOR_USE_TBB 0)

# Root source files
set(source_files
        backend.cc
        backend_std.cc
)

# Optional XTensor Backend
find_package(xtl QUIET)
find_package(xtensor QUIET)
set(HAS_XTENSOR OFF)
if(xtensor_FOUND OR GENIE_BACKEND STREQUAL "xtensor")
    if (NOT xtensor_FOUND)
        find_package(xtensor REQUIRED)
    endif()
    list(APPEND source_files backend_xtensor.cc)
    message(STATUS "genie-backend: XTensor backend enabled")
    set(HAS_XTENSOR ON)
endif()

# Optional Eigen Backend
find_package(Eigen3 QUIET)
set(HAS_EIGEN OFF)
if(Eigen3_FOUND OR GENIE_BACKEND STREQUAL "eigen")
    if (NOT Eigen3_FOUND)
        find_package(Eigen3 REQUIRED)
    endif()
    list(APPEND source_files backend_eigen.cc)
    message(STATUS "genie-backend: Eigen backend enabled")
    set(HAS_EIGEN ON)
endif()

add_library(${PROJECT_NAME} STATIC ${source_files})

if(HAS_XTENSOR)
    target_compile_definitions(${PROJECT_NAME} PUBLIC GENIE_HAS_XTENSOR_BACKEND)
endif()
if(HAS_EIGEN)
    target_compile_definitions(${PROJECT_NAME} PUBLIC GENIE_HAS_EIGEN_BACKEND)
endif()


target_include_directories(${PROJECT_NAME} PUBLIC .)
target_link_libraries(${PROJECT_NAME} PUBLIC genie-util)
target_link_libraries(${PROJECT_NAME} PRIVATE mpeggCodecs-static)
target_link_libraries(${PROJECT_NAME} PUBLIC genie-jbig)

# Link optional dependencies and aggressively export include paths
if(HAS_XTENSOR)
    target_link_libraries(${PROJECT_NAME} PUBLIC xtensor)
    if (xtensor_INCLUDE_DIRS)
        target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${xtensor_INCLUDE_DIRS})
    endif()
endif()
if(HAS_EIGEN)
    target_link_libraries(${PROJECT_NAME} PUBLIC Eigen3::Eigen)
    if (EIGEN3_INCLUDE_DIRS)
        target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${EIGEN3_INCLUDE_DIRS})
    endif()
endif()

# Define global default if needed, but the library now supports all
if(GENIE_BACKEND STREQUAL "xtensor")
    target_compile_definitions(${PROJECT_NAME} PUBLIC GENIE_BACKEND_XTENSOR)
elseif(GENIE_BACKEND STREQUAL "eigen")
    target_compile_definitions(${PROJECT_NAME} PUBLIC GENIE_BACKEND_EIGEN)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC GENIE_BACKEND_STD)
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE "-pedantic" "-fno-common" "-Wall" "-Wshadow" "-Wextra" "-Wundef" "-Wconversion" "-Wdouble-promotion")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wno-error=conversion")
endif()
