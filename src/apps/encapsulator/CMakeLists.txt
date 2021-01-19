project("encapsulator")

set(source_files
        # TODO: Fix FileHandlingUtils
        #FileHandlingUtils.cc
        main.cc
        program-options.cc
)

add_executable(encapsulator ${source_files})


target_link_libraries(encapsulator PRIVATE genie-spring)
target_link_libraries(encapsulator PRIVATE genie-mpegg_p1)
target_link_libraries(encapsulator PRIVATE genie-mgb)
target_link_libraries(encapsulator PRIVATE genie-module)