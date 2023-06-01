cmake_minimum_required (VERSION 2.9.0)
project (mpeggCodecs)

if (CMAKE_VERSION VERSION_LESS "3.1")
  if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set (CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")
  endif ()
else ()
  set (CMAKE_C_STANDARD 99)
endif ()

string(TOUPPER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_U)
if("${CMAKE_BUILD_TYPE_U}" STREQUAL "RELEASE")
  set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
endif()

if(NOT WIN32)
  set (CMAKE_C_FLAGS                "-fPIC -Wc++11-extensions -Wc++11-long-long -Wall ${CMAKE_C_FLAGS}")
else()
  set (CMAKE_C_FLAGS                "/D_CRT_SECURE_NO_WARNINGS /D_CRT_NONSTDC_NO_WARNINGS /wd4018 /wd4028 /wd4244 /wd4267 /wd4100 /wd4133 /wd4189 /wd4996 /WX- ${CMAKE_C_FLAGS}")
  set (CMAKE_CXX_FLAGS                "/D_CRT_SECURE_NO_WARNINGS /D_CRT_NONSTDC_NO_WARNINGS /wd4018 /wd4028 /wd4244 /wd4267 /wd4100 /wd4133 /wd4189 /wd4996 /WX- ${CMAKE_C_FLAGS}")
  add_compile_options("/WX-")
endif()

set (CMAKE_C_FLAGS_RELEASE        "-Werror ${CMAKE_C_FLAGS_RELEASE}")
set (CMAKE_C_FLAGS_RELWITHDEBINFO "-Werror ${CMAKE_C_FLAGS_RELWITHDEBINFO}")

set (CMAKE_VERBOSE_MAKEFILE       OFF)
set (CMAKE_COLOR_MAKEFILE         ON)
set (CMAKE_MACOSX_RPATH 1)

set (CMAKE_MODULE_PATH         "${PROJECT_SOURCE_DIR}/utils")
set (MPEGG_CODECS_INCLUDE_DIR  "${PROJECT_SOURCE_DIR}/include")
set (MPEGG_CODECS_ROOT_DIR     "${PROJECT_SOURCE_DIR}")
set (MPEGG_CODECS_STATIC       "mpeggCodecs-static")
set (MPEGG_CODECS_SHARED       "mpeggCodecs-shared")

include_directories(${MPEGG_CODECS_INCLUDE_DIR})

add_subdirectory ("api")
add_subdirectory ("libbsc")
add_subdirectory ("lzma")
# add_subdirectory ("procrustes")
add_subdirectory ("zstd")
add_subdirectory ("lib")

#add_subdirectory ("debug")
######################################################################################################################
## C-UNIT TEST CONFIGURATION
######################################################################################################################
include(CTest)
#if (BUILD_TESTING)
  add_subdirectory("test")
#endif ()


## Output global build information
include(summary)
summary()



