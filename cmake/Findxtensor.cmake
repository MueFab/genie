include(FetchContent)

FetchContent_Declare(
    xtensor
    GIT_REPOSITORY https://www.github.com/xtensor-stack/xtensor
    GIT_TAG e534928cc30eb3a4a05539747d98e1d6868c2d62 # v0.24.6
)

FetchContent_MakeAvailable(xtensor)

add_library(xtensor::optimize ALIAS xtensor)