include(ExternalProject)
#include(CHECK_INCLUDE_FILE_CXX)

SET(HTSLIB_PREFIX ${CMAKE_BINARY_DIR}/htslib-build)

ExternalProject_Add(htslib
#    PREFIX              ${HTSLIB_PREFIX}
    GIT_REPOSITORY      https://github.com/samtools/htslib.git
    GIT_TAG             1.11
    SOURCE_DIR          ${CMAKE_BINARY_DIR}/htslib-source
    BUILD_IN_SOURCE     1
    CONFIGURE_COMMAND   autoheader
        COMMAND         autoconf
        COMMAND         ./configure --prefix=${HTSLIB_PREFIX}
    BUILD_COMMAND       make -j
        COMMAND         make install
    INSTALL_COMMAND     ""
)

include_directories(${HTSLIB_PREFIX}/include)

## verify that the TBB header files can be included
#set(CMAKE_REQUIRED_INCLUDES_SAVE ${CMAKE_REQUIRED_INCLUDES})
#set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES} ${HTSLIB_INCLUDE_DIRS})
#
#CHECK_INCLUDE_FILE_CXX("<htslib/sam.h>" HAVE_HTSLIB)
#set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES_SAVE})
#if (NOT HAVE_HTSLIB)
#    message(ERROR "Did not build TBB correctly as cannot find tbb.h. Will build it.")
#endif (NOT HAVE_HTSLIB)