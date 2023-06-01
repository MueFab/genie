# CMake module to handle external static libraries compiled in the project

DEFINE_PROPERTY(GLOBAL PROPERTY EXT_LIBS
        BRIEF_DOCS "Contains linkable libraries"
        FULL_DOCS  "Contains paths to libraries for linking")


function(include_ext_library target name)
    target_include_directories(${target} PUBLIC $<TARGET_PROPERTY:${name},INTERFACE_INCLUDE_DIRECTORIES>)
    add_dependencies(${target} ${name})
endfunction(include_ext_library)

function(add_ext_library name buildTarget buildType include lib)
    string(TOUPPER "${buildType}"  UC_BUILD_TYPE)

    # add library
    if (UC_BUILD_TYPE STREQUAL "STATIC")
        add_library(${name} STATIC  IMPORTED GLOBAL)
    elseif(UC_BUILD_TYPE STREQUAL "SHARED")
        add_library(${name} SHARED  IMPORTED GLOBAL)
    else()
        message(FATAL_ERROR "${name} lib undefined type: ${buildType}")
    endif()

    # register library
    set_property(TARGET ${name} PROPERTY IMPORTED_LOCATION ${lib})
    set_property(TARGET ${name} PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${include})
    include_directories(${include})

    ## register library to EXT_LIBS
    get_property(extLibs GLOBAL PROPERTY EXT_LIBS)
    set(extLibs "${extLibs};${name}")
    set_property(GLOBAL PROPERTY EXT_LIBS "${extLibs}")

    # add build dependency
    if(NOT ${buildTarget} STREQUAL "")
        add_dependencies(${name} ${buildTarget})
    endif()

endfunction(add_ext_library)