cmake_minimum_required(VERSION 3.1)
project(cpplazy VERSION 1.0.0 LANGUAGES CXX)

option(CPPLAZY_BUILD_DEMO "Build demo" ON)
option(CPPLAZY_BUILD_TESTS "Build tests" ON)

if(CPPLAZY_BUILD_DEMO)
    add_subdirectory(demo)
endif()

if(CPPLAZY_BUILD_TESTS)
    add_subdirectory(tests)
endif()