cmake_minimum_required(VERSION 3.20)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Wybór typu budowania: Debug, Release, RelWithDebInfo, MinSizeRel")
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Możliwe konfiguracje" FORCE)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(EXE_NAME dpVision)
project(${EXE_NAME})

# Ustawienia katalogów i opcji CMake
set(AUTOGEN_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}_autogen/include")
file(MAKE_DIRECTORY ${AUTOGEN_INCLUDE_DIR})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Dodaj katalogi include dla core i gui
set(dpVision_INCLUDE_DIR
    ${PROJECT_SOURCE_DIR}/core
    ${PROJECT_SOURCE_DIR}/gui
)

# Znajdowanie pakietów
find_package(OpenMP REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5 COMPONENTS OpenGL REQUIRED)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
find_library(GLU_LIBRARY NAMES GLU glu)
if (NOT GLU_LIBRARY)
    message(FATAL_ERROR "GLU library not found. Please install libglu1-mesa-dev (Ubuntu)")
endif()


# === WYKONYWALNY EXE (tylko plik wejściowy, np. WinMain.cpp) ===
add_executable(${EXE_NAME}
    ${PROJECT_SOURCE_DIR}/gui/WinMain.cpp
)

################################################################

include(cmake/ArchAndRuntime.cmake)
include(cmake/getOpenCV.cmake)
include(cmake/Zlib.cmake)
include(cmake/QuaZip.cmake)
include_directories(/usr/include/quazip5)
# include_directories(/usr/include/QtQuaZip)  # niektóre wersje

include(cmake/Eigen3.cmake)
include(cmake/Boost.cmake)

include_directories(/usr/include/quazip5)


add_subdirectory(libs/PluginManager)
#add_library(dpVision::PluginManager ALIAS PluginManager)

add_library(dpVision::PluginManager INTERFACE IMPORTED)
set_target_properties(dpVision::PluginManager PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${PluginManager_INCLUDE_DIR}
    INTERFACE_LINK_LIBRARIES PluginManager
)

add_subdirectory(libs/libICP)

add_subdirectory(core)
add_subdirectory(gui)
add_subdirectory(api)
add_subdirectory(renderers)


add_subdirectory(libs/PMFactory)

################################################################

target_link_libraries(${EXE_NAME} #PRIVATE
    dpVision::Core
    dpVision::Gui
    #dpVision::Renderers
    dpVision::PluginManager
    dpVision::Api
)


target_include_directories(${EXE_NAME} PUBLIC
    ${dpVision_INCLUDE_DIR}
    $<BUILD_INTERFACE:${AUTOGEN_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
    ${PluginManager_INCLUDE_DIR}
    ${PMFactory_INCLUDE_DIR}
)

add_library(dpVision::Exe INTERFACE IMPORTED)

set_target_properties(dpVision::Exe PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/gui
    INTERFACE_LINK_LIBRARIES dpVisionCore
)


add_library(dpVision::libICP INTERFACE IMPORTED)
set_target_properties(dpVision::libICP PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/libs/libICP/src
    INTERFACE_LINK_LIBRARIES libICP
)

# Konfiguracja pluginów
set(SUBDIRS "")
set(DPV_PLUGINS "")
function(find_subdirs parentDir keyword)
    file(GLOB subdirectories RELATIVE ${parentDir} ${parentDir}/*)
    string(TOLOWER ${keyword} keyword_lower)
    set(SUBDIRLIST "")
    foreach(subdir ${subdirectories})
        if(IS_DIRECTORY ${parentDir}/${subdir})
            string(TOLOWER ${subdir} subdir_lower)
            string(FIND ${subdir_lower} ${keyword_lower} position)
            if(NOT ${position} EQUAL -1)
                list(APPEND SUBDIRLIST ${subdir})
            endif()
        endif()
    endforeach()
    set(SUBDIRS ${SUBDIRLIST} PARENT_SCOPE)
endfunction()

find_subdirs(${CMAKE_CURRENT_SOURCE_DIR} "plugins")
foreach(SUBDIR ${SUBDIRS})
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR}/CMakeLists.txt)
        add_subdirectory(${SUBDIR})
    endif()
endforeach()

# Skrypty instalacyjne
set(BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin)
install(TARGETS ${EXE_NAME} dpVisionCore DESTINATION ${BINARY_DIR})
install(TARGETS ${EXE_NAME} dpVisionGui DESTINATION ${BINARY_DIR})
install(TARGETS ${EXE_NAME} dpVisionApi DESTINATION ${BINARY_DIR})
install(TARGETS ${EXE_NAME} PluginManager DESTINATION ${BINARY_DIR})
install(TARGETS ${DPV_PLUGINS} DESTINATION ${BINARY_DIR}/plugins)
if(MSVC)
    install(TARGETS QuaZip DESTINATION ${BINARY_DIR})
endif()

if(UNIX OR MINGW)
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/run_ldd.sh
        "#!/bin/sh\n"
        "echo \"Running ldd...\"\n"
        "ldd $1 | grep -iv 'system32' | grep -iv 'windows' | grep -iv ':$' | cut -f2 -d '>' | cut -f1 -d '(' | tr '\\\\' '/' | while read a; do ! [ -e \"${BINARY_DIR}/`basename $a`\" ] && cp -v \"$a\" ${BINARY_DIR}/; done\n"
        "echo \"End ldd...\"\n"
    )
endif()

add_custom_target(bin
    COMMAND ${CMAKE_COMMAND} --build . --target install
)

if(UNIX OR MINGW)
    add_custom_command(TARGET bin POST_BUILD
        COMMAND /bin/sh ${CMAKE_CURRENT_BINARY_DIR}/run_ldd.sh ${BINARY_DIR}/${EXE_NAME}
    )
endif()

if(MSVC OR MINGW)
    add_custom_command(TARGET bin POST_BUILD
        COMMAND windeployqt.exe ${BINARY_DIR}/${EXE_NAME}.exe
    )
endif()
