include(FetchContent)

FetchContent_Declare(
    xtensor-io
    GIT_REPOSITORY https://github.com/xtensor-stack/xtensor-io
#    GIT_TAG 0.13.0 # v0.13.0
    GIT_TAG ffada93 # v0.13.0
)

FetchContent_MakeAvailable(xtensor-io)

#add_library(xio)