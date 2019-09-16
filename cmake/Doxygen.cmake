find_package(Doxygen)

if(DOXYGEN_FOUND)
    message(STATUS "Doxygen is available")
    message(STATUS "Issue 'make doc' to build documentation")

    if(NOT DOXYGEN_DOT_FOUND)
        message(
            WARNING
            "'dot' tool from Graphviz not found; "
            "Doxygen documentation will not include graphs"
        )
    endif()

    # For forward compatibility with newer versions of CMake
    if(NOT DEFINED DOXYGEN_HAVE_DOT)
        set(DOXYGEN_HAVE_DOT ${DOXYGEN_DOT_FOUND})
    endif()

    set(DOCUMENTATION_DIRECTORY "${CMAKE_BINARY_DIR}/doc")
    set(DOXYFILE_IN "${CMAKE_SOURCE_DIR}/Doxyfile.in")
    set(DOXYFILE_OUT "${DOCUMENTATION_DIRECTORY}/Doxyfile")

    configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

    add_custom_target(
        doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
        WORKING_DIRECTORY ${DOCUMENTATION_DIRECTORY}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
else()
    message(WARNING "Doxygen not found; target 'doc' not available")
endif()
