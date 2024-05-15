include(FetchContent)

FetchContent_Declare(
    xtensor
    GIT_REPOSITORY https://www.github.com/xtensor-stack/xtensor
    GIT_TAG d9c3782ed51027b2d00be3c26288b2f74e4dbe94 # v0.25.0
)

FetchContent_MakeAvailable(xtensor)

add_library(xtensor::optimize ALIAS xtensor)