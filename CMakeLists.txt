cmake_minimum_required(VERSION 3.20)

project(dpvision-root LANGUAGES C CXX)

# Let MSBuild schedule individual compiler invocations across all projects while
# respecting the process limit supplied by cmake --build --parallel.
if(MSVC)
    if(NOT CMAKE_VS_GLOBALS MATCHES "(^|;)UseMultiToolTask=")
        list(APPEND CMAKE_VS_GLOBALS "UseMultiToolTask=true")
    endif()
    if(NOT CMAKE_VS_GLOBALS MATCHES "(^|;)EnforceProcessCountAcrossBuilds=")
        list(APPEND CMAKE_VS_GLOBALS "EnforceProcessCountAcrossBuilds=true")
    endif()
endif()

include(CTest)

add_subdirectory(src)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
