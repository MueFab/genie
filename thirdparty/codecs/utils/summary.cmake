macro(summary)
STRING(TOUPPER "${CMAKE_BUILD_TYPE}" buildtype)

MESSAGE(STATUS "")
MESSAGE(STATUS "MPEG-G Codecs configuration summary:")
MESSAGE(STATUS "")
MESSAGE(STATUS "  System name ........: ${CMAKE_SYSTEM_NAME}")
MESSAGE(STATUS "  Build type .........: ${CMAKE_BUILD_TYPE}")
MESSAGE(STATUS "")
MESSAGE(STATUS "  Install prefix .....: ${CMAKE_INSTALL_PREFIX}")
MESSAGE(STATUS "  C compiler .........: ${CMAKE_C_COMPILER}")
MESSAGE(STATUS "  C compiler flags ...: ${CMAKE_C_FLAGS}")
MESSAGE(STATUS "  C buildtype flags ..: ${CMAKE_C_FLAGS_${buildtype}}")
MESSAGE(STATUS "")

MESSAGE(STATUS "")

endmacro(summary)
