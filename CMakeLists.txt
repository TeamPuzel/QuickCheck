cmake_minimum_required(VERSION 3.28)
project(QuickCheck VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_CXX_FLAGS "-Wno-dynamic-exception-spec -Wno-shift-op-parentheses")

add_library(QuickCheck)
add_library(QuickCheck::QuickCheck ALIAS QuickCheck)

file(GLOB_RECURSE QUICK_CHECK_MODULES "src/*.cppm")

target_sources(QuickCheck
    PUBLIC FILE_SET CXX_MODULES FILES
    ${QUICK_CHECK_MODULES}
)
