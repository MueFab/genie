# CMake module to apply dependencies defined in INC_LIBRARIES and LINK_LIBRARIES
# variables.
# The variable LOCAL_INC_DIR can be set before including this module to override the
# local include directory for current CMakeLists.txt. If not overridden, the local include
# directory defaults to 'include' if a sub-directory with that name exists, otherwise it
# defaults to '.'.
#
# WARNING  : To prevent INC_LIBRARIES and LINK_LIBRARIES from being erroneously propagated
#            to sub-directories added with add_subdirectory() and then being processed by
#            this module, the add_subdirectory() statements should always appear before
#            including this module.

# List the include directories from required INC_LIBRARIES and LINK_LIBRARIES
set(INC_DIRS)
foreach(inc_library ${INC_LIBRARIES} ${LINK_LIBRARIES})
    set(INC_DIRS
        ${INC_DIRS}
        $<TARGET_PROPERTY:${inc_library},INCLUDE_DIRECTORIES>)
endforeach(inc_library)

# Detect the default LOCAL_INC_DIR if not specified by the including CMakeLists.txt file.
if(NOT LOCAL_INC_DIR)
    foreach(default_local_inc_dir
            include
            .)
        if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${default_local_inc_dir})
            set(LOCAL_INC_DIR ${default_local_inc_dir})
            break()
        endif(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${default_local_inc_dir})
    endforeach(default_local_inc_dir)
endif(NOT LOCAL_INC_DIR)

include_directories(
        ${LOCAL_INC_DIR}
        ${INC_DIRS}
    )

unset(INC_DIR)
unset(LOCAL_INC_DIR)
