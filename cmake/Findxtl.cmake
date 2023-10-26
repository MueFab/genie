include(FetchContent)

FetchContent_Declare(
    xtl
    GIT_REPOSITORY https://www.github.com/xtensor-stack/xtl
    GIT_TAG fea39142693fbbc2ef19d75012bc6b46ef0a5f8c # v0.7.5
)

set(BUILD_TESTS OFF)

FetchContent_MakeAvailable(xtl)