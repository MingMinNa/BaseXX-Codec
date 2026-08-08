cmake_minimum_required(VERSION 3.20)

project(BaseXX LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Warning Flag
add_library(warnings INTERFACE)

target_compile_options(warnings
    INTERFACE
        -Wall
        -Wextra
        -Wpedantic
)

# BaseXX Library
file(GLOB LIB_SRCS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

add_library(basexx STATIC ${LIB_SRCS})

target_include_directories(basexx
    PUBLIC
        "${CMAKE_SOURCE_DIR}/include"
)

target_link_libraries(basexx
    PRIVATE
        warnings
)

# Example 
add_executable(example example.cpp)

target_link_libraries(example
    PRIVATE
        basexx
        warnings
)

# Tests
include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.15.3
)

FetchContent_MakeAvailable(Catch2)

file(GLOB TEST_SRCS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/tests/*.cpp"
)

add_executable(tests ${TEST_SRCS})

target_link_libraries(tests
    PRIVATE
        warnings
        basexx
        Catch2::Catch2WithMain
)