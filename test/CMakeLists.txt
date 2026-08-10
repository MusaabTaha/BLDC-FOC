cmake_minimum_required(VERSION 3.16)

project(BLDC_FOC_tests LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.7.1
)

FetchContent_MakeAvailable(Catch2)

add_library(
    foc_math
    ../src/systems/foc_math.c
)

target_include_directories(
    foc_math
    PUBLIC ../include
)

add_executable(
    foc_math_tests
    test_foc_math.cpp
)

target_link_libraries(
    foc_math_tests
    PRIVATE
    foc_math
    Catch2::Catch2WithMain
)

include(CTest)
include(Catch)
catch_discover_tests(foc_math_tests)
