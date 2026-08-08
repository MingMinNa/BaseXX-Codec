cmake_minimum_required(VERSION 3.20)

project(BaseXX
    VERSION 1.0.0
    LANGUAGES CXX
    HOMEPAGE_URL "https://github.com/MingMinNa/BaseXX-Codec"
    DESCRIPTION "C++ implementations of Base16, Base32, and Base64"
)

option(BASEXX_BUILD_EXAMPLES
    "Build examples" OFF
)

option(BASEXX_BUILD_TESTS
    "Build tests" OFF
)

# Warning flags
add_library(basexx_warnings INTERFACE)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(basexx_warnings
        INTERFACE
            -Wall
            -Wextra
            -Wpedantic
    )
elseif(MSVC)
    target_compile_options(basexx_warnings
        INTERFACE
            /W4
    )
endif()

# BaseXX Library
file(GLOB LIB_SRCS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(basexx STATIC ${LIB_SRCS})

target_compile_features(basexx
    PUBLIC
        cxx_std_20
)

target_include_directories(basexx
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(basexx
    PRIVATE
        basexx_warnings
)

# Alias
add_library(BaseXX::basexx ALIAS basexx)

# Examples
if(BASEXX_BUILD_EXAMPLES)

    add_executable(example
        example.cpp
    )

    target_link_libraries(example
        PRIVATE
            BaseXX::basexx
            basexx_warnings
    )

endif()

# Tests
if(BASEXX_BUILD_TESTS)

    include(FetchContent)

    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.15.3
    )

    FetchContent_MakeAvailable(Catch2)

    file(GLOB TEST_SRCS CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
    )

    add_executable(tests
        ${TEST_SRCS}
    )

    target_link_libraries(tests
        PRIVATE
            basexx_warnings
            BaseXX::basexx
            Catch2::Catch2WithMain
    )

endif()