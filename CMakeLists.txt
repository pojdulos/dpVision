cmake_minimum_required(VERSION 3.20)

project(dpvision-root LANGUAGES C CXX)

include(CTest)

add_subdirectory(src)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
