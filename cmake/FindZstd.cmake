find_path(ZSTD_INCLUDE_DIR zstd.h)
find_library(ZSTD_LIBRARY NAMES zstd)

if(ZSTD_INCLUDE_DIR AND ZSTD_LIBRARY)
    set(ZSTD_FOUND TRUE)
    set(Zstd_LIBRARIES ${ZSTD_LIBRARY})
    set(Zstd_INCLUDE_DIRS ${ZSTD_INCLUDE_DIR})
else()
    set(ZSTD_FOUND FALSE)
endif()

message(STATUS "   ZSTD include dirs: ${Zstd_INCLUDE_DIRS}")
message(STATUS "   ZSTD libraries: ${Zstd_LIBRARIES}")

mark_as_advanced(ZSTD_INCLUDE_DIR ZSTD_LIBRARY)
