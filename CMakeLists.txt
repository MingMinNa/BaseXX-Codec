cmake_minimum_required(VERSION 3.20)

project(BaseXX LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_library(warnings INTERFACE)

target_compile_options(warnings
    INTERFACE
        -Wall
        -Wextra
        -Wpedantic
)

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

add_executable(example example.cpp)

target_link_libraries(example
    PRIVATE
        basexx
        warnings
)