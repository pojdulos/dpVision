cmake_minimum_required(VERSION 3.20)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Wybór typu budowania: Debug, Release, RelWithDebInfo, MinSizeRel")
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Możliwe konfiguracje" FORCE)

# Ustawienie nazwy projektu
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


# Biblioteki dpVision
set(dpVision_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/core)

# Znajdowanie pakietów
find_package(OpenMP REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5 COMPONENTS OpenGL REQUIRED)

# Dodanie zasobów Qt
qt5_add_resources(QRC_FILE ${PROJECT_SOURCE_DIR}/core/${EXE_NAME}.qrc)

# Pliki źródłowe, nagłówkowe i interfejsu użytkownika
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/core/*.cpp)
file(GLOB HDR_FILES ${PROJECT_SOURCE_DIR}/core/*.h)
file(GLOB UI_FILES ${PROJECT_SOURCE_DIR}/core/*.ui)

# Tworzenie pliku wykonywalnego
add_executable(${EXE_NAME}
    ${PROJECT_SOURCE_DIR}/core/WinMain.cpp
    ${SRC_FILES}
    ${PROJECT_SOURCE_DIR}/core/tinyspline.c
    ${HDR_FILES}
    ${UI_FILES}
    ${QRC_FILE}
)

# Dodanie katalogów do przeszukania przez kompilator
target_include_directories(${EXE_NAME} PUBLIC
    ${dpVision_INCLUDE_DIR}
    $<BUILD_INTERFACE:${AUTOGEN_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Właściwości targetu
set_target_properties(${EXE_NAME} PROPERTIES
    LINKER_LANGUAGE CXX
    ENABLE_EXPORTS ON
)

# Definicje preprocesora
target_compile_definitions(${EXE_NAME} PRIVATE NO_OLD_BYTE_DEFINITION)

# Funkcje do obsługi zewnętrznych bibliotek
function(find_or_add_subdirectory package subdir)
    find_package(${package} QUIET)
    if (NOT ${package}_FOUND)
        message(STATUS "${package} not found, adding subdirectory ${subdir}")
        add_subdirectory(${subdir})
    endif()
endfunction()

function(add_subdir_if_contains parentDir keyword)
    file(GLOB subdirectories RELATIVE ${parentDir} ${parentDir}/*)
    string(TOLOWER ${keyword} keyword_lower)
    foreach(subdir ${subdirectories})
        if(IS_DIRECTORY ${parentDir}/${subdir})
            string(TOLOWER ${subdir} subdir_lower)
            string(FIND ${subdir_lower} ${keyword_lower} position)
            if(NOT ${position} EQUAL -1)
                add_subdirectory(${parentDir}/${subdir})
            endif()
        endif()
    endforeach()
endfunction()

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

# Łączenie bibliotek
# target_link_libraries(${EXE_NAME}
#     opengl32
#     glu32
#     Qt5::Core
#     Qt5::Gui
#     Qt5::Widgets
#     Qt5::OpenGL
#     Qt5::Xml
#     Qt5::Network
#     OpenMP::OpenMP_CXX
# )

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
find_library(GLU_LIBRARY NAMES GLU glu)

if (NOT GLU_LIBRARY)
    message(FATAL_ERROR "GLU library not found. Please install libglu1-mesa-dev (Ubuntu)")
endif()


target_link_libraries(${EXE_NAME}
    OpenGL::GL
    ${GLU_LIBRARY}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::OpenGL
    Qt5::Xml
    Qt5::Network
    OpenMP::OpenMP_CXX
)


# Zewnętrzne biblioteki
add_library(dpVision::Core INTERFACE IMPORTED)
set_target_properties(dpVision::Core PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${dpVision_INCLUDE_DIR}
    INTERFACE_LINK_LIBRARIES dpVision
)

# detekcja wersji kompilatora i architektury
include(cmake/ArchAndRuntime.cmake)

# Konfiguracja zewnętrznych bibliotek
include(cmake/getOpenCV.cmake)
include(cmake/Zlib.cmake)
include(cmake/QuaZip.cmake)

# QuaZip używa Qt5 i nie dostarcza własnego configa, więc musisz dodać to ręcznie:
include_directories(/usr/include/quazip5)
# include_directories(/usr/include/QtQuaZip)  # niektóre wersje


include(cmake/Eigen3.cmake)
include(cmake/Boost.cmake)

message("\nConfiguring internal libraries...\n")
add_subdirectory(libs/PMFactory)
add_subdirectory(libs/PluginManager)
add_subdirectory(libs/libICP)

add_library(dpVision::PluginManager INTERFACE IMPORTED)
set_target_properties(dpVision::PluginManager PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${PluginManager_INCLUDE_DIR}
    INTERFACE_LINK_LIBRARIES PluginManager
)


add_subdirectory(api)

target_link_libraries(${EXE_NAME}
    dpVision::Api
    PluginManager
    PMFactory
)

target_include_directories(${EXE_NAME} PUBLIC
    ${PluginManager_INCLUDE_DIR}
    ${PMFactory_INCLUDE_DIR}
)

add_library(dpVision::libICP INTERFACE IMPORTED)
set_target_properties(dpVision::libICP PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/libs/libICP/src
    INTERFACE_LINK_LIBRARIES libICP
)

# Konfiguracja pluginów
message("\nConfiguring plugins:\n")
set(SUBDIRS "")
set(DPV_PLUGINS "")

find_subdirs(${CMAKE_CURRENT_SOURCE_DIR} "plugins")
foreach(SUBDIR ${SUBDIRS})
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR}/CMakeLists.txt)
        message("\nSubdirectory \"${SUBDIR}\" contains CMakeLists.txt. Looking for plugins:\n")
        add_subdirectory(${SUBDIR})
    endif()
endforeach()

message("Plugin list: ${DPV_PLUGINS}")
message("\nEnding configuration process...\n")

if (MSVC)
	add_custom_command(TARGET ${EXE_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${EXE_NAME}> $<TARGET_FILE_DIR:${EXE_NAME}>/
	)
else (MSVC)
	set_target_properties(${EXE_NAME} PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
	)
endif (MSVC)

# Skrypty instalacyjne
set(BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin)
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
        COMMAND /bin/sh ${CMAKE_CURRENT_BINARY_DIR}/run_ldd.sh ${BINARY_DIR}/${EXE_NAME}.exe
    )
endif()

if(MSVC OR MINGW)
    add_custom_command(TARGET bin POST_BUILD
        COMMAND windeployqt.exe ${BINARY_DIR}/${EXE_NAME}.exe
    )
endif()
