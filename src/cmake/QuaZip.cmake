message("\nConfiguring QuaZip:\n")

option(DPVISION_FETCH_MISSING_QUAZIP "Fetch QuaZip from Git when it is not available locally" OFF)
set(DPVISION_QUAZIP_GIT_TAG "v1.4" CACHE STRING "QuaZip git tag used when fetching sources")

function(_dpvision_set_quazip_target)
    if(TARGET QuaZip::QuaZip)
        set(_dpvision_quazip_target QuaZip::QuaZip PARENT_SCOPE)
    elseif(TARGET QuaZip::QuaZip-Qt5)
        set(_dpvision_quazip_target QuaZip::QuaZip-Qt5 PARENT_SCOPE)
    elseif(TARGET QuaZip::quazip)
        set(_dpvision_quazip_target QuaZip::quazip PARENT_SCOPE)
    elseif(TARGET quazip)
        set(_dpvision_quazip_target quazip PARENT_SCOPE)
    else()
        set(_dpvision_quazip_target "" PARENT_SCOPE)
    endif()
endfunction()

set(_dpvision_quazip_target "")

if(NOT _dpvision_quazip_target)
    set(_dpvision_quazip_candidates
        "${CMAKE_SOURCE_DIR}/3rdParty/quazip"
        "${CMAKE_SOURCE_DIR}/3rdParty/QuaZip"
        "${PROJECT_SOURCE_DIR}/3rdParty/quazip"
        "${PROJECT_SOURCE_DIR}/3rdParty/QuaZip"
    )

    file(GLOB _dpvision_quazip_globbed_candidates
        "${CMAKE_SOURCE_DIR}/3rdParty/quazip*"
        "${CMAKE_SOURCE_DIR}/3rdParty/QuaZip*"
        "${PROJECT_SOURCE_DIR}/3rdParty/quazip*"
        "${PROJECT_SOURCE_DIR}/3rdParty/QuaZip*"
    )
    list(APPEND _dpvision_quazip_candidates ${_dpvision_quazip_globbed_candidates})
    list(REMOVE_DUPLICATES _dpvision_quazip_candidates)

    foreach(_dpvision_quazip_dir IN LISTS _dpvision_quazip_candidates)
        if(EXISTS "${_dpvision_quazip_dir}/CMakeLists.txt")
            message("-- Using QuaZip sources from ${_dpvision_quazip_dir}")
            set(QT_DEFAULT_MAJOR_VERSION 5 CACHE STRING "Default Qt major version" FORCE)
            set(CMAKE_DISABLE_FIND_PACKAGE_Qt6 ON CACHE BOOL "Disable Qt6 lookup for bundled QuaZip" FORCE)
            set(QUAZIP_QT_MAJOR_VERSION 5 CACHE STRING "Qt major version for QuaZip" FORCE)
            set(QUAZIP_ENABLE_TESTS OFF CACHE BOOL "Build QuaZip tests" FORCE)
            set(QUAZIP_INSTALL OFF CACHE BOOL "Install QuaZip from bundled source build" FORCE)
            set(QUAZIP_USE_QT_ZLIB OFF CACHE BOOL "Use Qt bundled zlib in QuaZip" FORCE)
            add_subdirectory("${_dpvision_quazip_dir}" "${CMAKE_BINARY_DIR}/_deps/quazip-local")
            _dpvision_set_quazip_target()
            if(_dpvision_quazip_target)
                break()
            endif()
        endif()
    endforeach()
endif()

if(NOT _dpvision_quazip_target AND DPVISION_FETCH_MISSING_QUAZIP)
    include(FetchContent)

    message("-- Fetching QuaZip from GitHub (${DPVISION_QUAZIP_GIT_TAG})")
    set(QT_DEFAULT_MAJOR_VERSION 5 CACHE STRING "Default Qt major version" FORCE)
    set(CMAKE_DISABLE_FIND_PACKAGE_Qt6 ON CACHE BOOL "Disable Qt6 lookup for fetched QuaZip" FORCE)
    set(QUAZIP_QT_MAJOR_VERSION 5 CACHE STRING "Qt major version for QuaZip" FORCE)
    set(QUAZIP_ENABLE_TESTS OFF CACHE BOOL "Build QuaZip tests" FORCE)
    set(QUAZIP_INSTALL OFF CACHE BOOL "Install QuaZip from fetched source build" FORCE)
    set(QUAZIP_USE_QT_ZLIB OFF CACHE BOOL "Use Qt bundled zlib in QuaZip" FORCE)

    FetchContent_Declare(
        quazip
        GIT_REPOSITORY https://github.com/stachenov/quazip.git
        GIT_TAG ${DPVISION_QUAZIP_GIT_TAG}
    )
    FetchContent_MakeAvailable(quazip)
    _dpvision_set_quazip_target()
endif()

if(NOT _dpvision_quazip_target)
    find_package(QuaZip CONFIG QUIET)
    if(NOT QuaZip_FOUND)
        find_package(QuaZip-Qt5 CONFIG QUIET)
    endif()
    _dpvision_set_quazip_target()
endif()

if(NOT _dpvision_quazip_target)
    message("-- QuaZip package not found. Falling back to manual detection")

    find_path(QUAZIP_INCLUDE_DIR
        NAMES quazip.h
        PATH_SUFFIXES quazip quazip5
    )
    find_library(QUAZIP_LIBRARY
        NAMES quazip quazip5 quazip1-qt5 quazipqt5
    )

    if(QUAZIP_INCLUDE_DIR AND QUAZIP_LIBRARY)
        if(NOT TARGET dpVision::QuaZip)
            add_library(dpVision::QuaZip UNKNOWN IMPORTED)
            set_target_properties(dpVision::QuaZip PROPERTIES
                IMPORTED_LOCATION "${QUAZIP_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${QUAZIP_INCLUDE_DIR}"
                INTERFACE_LINK_LIBRARIES "Qt5::Core;dpVision::ZLIB"
            )
        endif()
    else()
        message(FATAL_ERROR "QuaZip not found. Use vcpkg, provide 3rdParty/quazip, or configure -DDPVISION_FETCH_MISSING_QUAZIP=ON")
    endif()
endif()

if(_dpvision_quazip_target AND NOT TARGET dpVision::QuaZip)
    message("-- Using QuaZip target: ${_dpvision_quazip_target}")
    if(QUAZIP_QT_MAJOR_VERSION EQUAL 5)
        if(TARGET QuaZip)
            set_property(TARGET QuaZip APPEND PROPERTY
                INTERFACE_COMPILE_DEFINITIONS QUAZIP_CAN_USE_QTEXTCODEC)
        elseif(NOT _dpvision_quazip_target MATCHES "^QuaZip::")
            set_property(TARGET ${_dpvision_quazip_target} APPEND PROPERTY
                INTERFACE_COMPILE_DEFINITIONS QUAZIP_CAN_USE_QTEXTCODEC)
        endif()
    endif()
    add_library(dpVision::QuaZip INTERFACE IMPORTED)
    set_target_properties(dpVision::QuaZip PROPERTIES
        INTERFACE_LINK_LIBRARIES "${_dpvision_quazip_target}"
    )
endif()
