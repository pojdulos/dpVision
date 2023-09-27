cmake_minimum_required(VERSION 3.20)

set(EXE_NAME dpVision)

project(${EXE_NAME})

set (CMAKE_INCLUDE_CURRENT_DIR ON)
set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTOUIC ON)
set (CMAKE_AUTORCC ON)

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED True)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#############################################################################
# ZEWNETRZNE BIBLIOTEKI
#############################################################################

find_package (Qt5Core REQUIRED)
find_package (Qt5Gui REQUIRED)
find_package (Qt5Widgets REQUIRED)
find_package (Qt5OpenGL REQUIRED)
find_package (Qt5Xml REQUIRED)
find_package (Qt5Network REQUIRED)

#############################################################################
# biblioteki dpVision
#############################################################################

set (dpVision_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src)

#############################################################################
# glowny program dpVision
#############################################################################

file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB HDR_FILES ${PROJECT_SOURCE_DIR}/src/*.h)
file(GLOB UI_FILES ${PROJECT_SOURCE_DIR}/src/*.ui)

qt5_add_resources(QRC_FILE ${PROJECT_SOURCE_DIR}/src/${EXE_NAME}.qrc)

add_executable (${EXE_NAME}
	${PROJECT_SOURCE_DIR}/src/WinMain.cpp
	${SRC_FILES}
	${PROJECT_SOURCE_DIR}/src/tinyspline.c
	${HDR_FILES}
	${UI_FILES}
	${QRC_FILE}
)

target_include_directories (${EXE_NAME} PUBLIC
	${dpVision_INCLUDE_DIR}
)


set_target_properties(${EXE_NAME} PROPERTIES
    LINKER_LANGUAGE CXX
    ENABLE_EXPORTS ON
)

target_compile_definitions(${EXE_NAME} PRIVATE NO_OLD_BYTE_DEFINITION)

set(CMAKE_INCLUDE_CURRENT_DIR ON)


target_compile_features(${EXE_NAME} PUBLIC
  cxx_nonstatic_member_init
)

target_link_libraries ( ${EXE_NAME}
	opengl32
	glu32
	Qt5::Core
	Qt5::Gui
	Qt5::Widgets
	Qt5::OpenGL
	Qt5::Xml
	Qt5::Network
)

add_library(dpVision::Core INTERFACE IMPORTED)
set_target_properties(dpVision::Core PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES ${dpVision_INCLUDE_DIR}
	INTERFACE_LINK_LIBRARIES dpVision
)

if (MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${EXE_NAME})
endif (MSVC)

# get_cmake_property(_variableNames VARIABLES)
# list(SORT _variableNames)
# foreach (_variableName ${_variableNames})
   # message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()



message( "\nConfiguring external libraries:\n" )

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
	set (SUBDIRLIST "")
    foreach(subdir ${subdirectories})
        if(IS_DIRECTORY ${parentDir}/${subdir})
            string(TOLOWER ${subdir} subdir_lower)
            string(FIND ${subdir_lower} ${keyword_lower} position)
            if(NOT ${position} EQUAL -1)
                list (APPEND SUBDIRLIST ${subdir})
            endif()
        endif()
    endforeach()
	set (SUBDIRS ${SUBDIRLIST} PARENT_SCOPE)
endfunction()


list(APPEND CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/3rdparty")
#set(CMAKE_FIND_USE_CMAKE_PATH FALSE)

	set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


#############################################################################
# ZLIB
#############################################################################

message( "\nConfiguring ZLIB:\n" )

find_package (Qt5Zlib QUIET)
if (Qt5Zlib_FOUND)
    message( "-- Qt::Zlib found" )
    target_link_libraries( ${EXE_NAME}
		Qt::Zlib
	)
    set (QUAZIP_USE_QT_ZLIB ON)
else (Qt5Zlib_FOUND)
	find_package (ZLIB)
	if (ZLIB_FOUND)
		message( "-- ZLIB found" )
		target_link_libraries( ${EXE_NAME}
			z
		)
	else (ZLIB_FOUND)
		message( "-- Qt::Zlib not found, looking for another zlib" )
	endif (ZLIB_FOUND)
endif (Qt5Zlib_FOUND)

#############################################################################
# QUAZIP
#############################################################################

message( "\nConfiguring QuaZip:\n" )

find_package (QuaZip-Qt5 QUIET)
if (QuaZip-Qt5_FOUND)
	message( "-- QuaZip-Qt5 found" )
	target_link_libraries(${EXE_NAME}
		QuaZip::QuaZip
	)
else (QuaZip-Qt5_FOUND)
	message( "-- QuaZip-Qt5 not found. Trying to find sources in 3rdParty folder" )
	
	#set (BUILD_SHARED_LIBS OFF)
	set ( QUAZIP_INSTALL OFF )
	add_subdir_if_contains(${CMAKE_CURRENT_SOURCE_DIR}/3rdParty "QuaZip")

	if (QuaZip_Library_SOURCE_DIR)
		message( "-- QuaZip found in ${QuaZip_Library_SOURCE_DIR}")
	
		target_link_libraries ( ${EXE_NAME}
			QuaZip::QuaZip
			# quazip1-qt5
		)

		# target_include_directories( ${EXE_NAME} PUBLIC
			# ${QuaZip_Library_SOURCE_DIR}
		# )
	else (QuaZip_Library_SOURCE_DIR)
		message( "-- QuaZip not found")
	endif (QuaZip_Library_SOURCE_DIR)
endif (QuaZip-Qt5_FOUND)

#############################################################################

message( "\nConfiguring Eigen3:\n" )

find_package(Eigen3 QUIET)
if (Eigen3_FOUND)
	message( "-- Eigen3 found")
	target_link_libraries(${EXE_NAME}
		Eigen3::Eigen
	)
else (Eigen3_FOUND)
	message( "-- Eigen3 not found. Trying to find sources in 3rdParty folder")
	add_subdir_if_contains(${CMAKE_CURRENT_SOURCE_DIR}/3rdParty "Eigen")
	if (TARGET Eigen3::Eigen)
		message( "-- Eigen3 found in 3rdParty folder.")
		target_link_libraries(${EXE_NAME}
			Eigen3::Eigen
		)
	else (TARGET Eigen3::Eigen)
		message( "-- Eigen3 not found.")
	endif (TARGET Eigen3::Eigen)
endif (Eigen3_FOUND)


#############################################################################

message( "\nConfiguring Boost:\n" )
set (Boost_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/boost}")
find_package(Boost QUIET)
if (Boost_FOUND)
	message( "-- Boost found")
else (Boost_FOUND)
	message( "-- Boost not found. Set Boost_ROOT by hand please...")
	set (Boost_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/boost")
	set (Boost_LIBRARY_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/boost/stage/lib")
	add_library(Boost::boost INTERFACE IMPORTED)
	set_target_properties(Boost::boost PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS}
		# INTERFACE_LINK_LIBRARIES "${Boost_LIBRARY_DIRS}/library.a"
	)
	set(Boost_FOUND TRUE)
	target_link_libraries(${EXE_NAME}
		Boost::boost
	)
endif (Boost_FOUND)



message( "\nConfiguring internal libraries...\n" )

add_subdirectory(libs/PMFactory)
add_subdirectory(libs/PluginManager)

add_library(dpVision::PluginManager INTERFACE IMPORTED)
set_target_properties(dpVision::PluginManager PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES ${PluginManager_INCLUDE_DIR}
	INTERFACE_LINK_LIBRARIES PluginManager
)

target_link_libraries ( ${EXE_NAME}
	PluginManager
	PMFactory
)

target_include_directories (${EXE_NAME} PUBLIC
	${PluginManager_INCLUDE_DIR}
	${PMFactory_INCLUDE_DIR}
)

message( "Configuring libICP\n" )
add_subdirectory(libs/libICP)

add_library(dpVision::libICP INTERFACE IMPORTED)
set_target_properties(dpVision::libICP PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/libs/libICP/src
	INTERFACE_LINK_LIBRARIES libICP
)


#############################################################################



message( "\nConfiguring plugins:\n" )

set (SUBDIRS "")
set (DPV_PLUGINS "")

find_subdirs( ${CMAKE_CURRENT_SOURCE_DIR} "plugins")
foreach(SUBDIR ${SUBDIRS})
	# message( "Subdirectory \"${SUBDIR}\" found..." )
	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR}/CMakeLists.txt)
		message( "\nSubdirectory \"${SUBDIR}\" contains CMakeLists.txt. Looking for plugins:\n" )
		add_subdirectory(${SUBDIR})
	endif()
endforeach()

message( "\nEnding configuration process...\n" )


#############################################################################
# SKRYPTY DLA: make install
# SKRYPTY DLA: make bin
#############################################################################

set(BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin)

install (TARGETS ${EXE_NAME} PluginManager DESTINATION ${BINARY_DIR})
install (TARGETS ${DPV_PLUGINS} DESTINATION ${BINARY_DIR}/plugins)

if (UNIX OR MINGW)
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/run_ldd.sh
        "#!/bin/sh\n"
        "echo \"Running ldd...\"\n"
        "ldd $1 | grep -iv 'system32' | grep -iv 'windows' | grep -iv ':$' | cut -f2 -d '>' | cut -f1 -d '(' | tr '\\\\' '/' | while read a; do ! [ -e \"${BINARY_DIR}/`basename $a`\" ] && cp -v \"$a\" ${BINARY_DIR}/; done\n"
        "echo \"End ldd...\"\n"
    )
endif()

add_custom_target(bin
    COMMAND ${CMAKE_COMMAND} --build . --target install
    # COMMAND ${CMAKE_COMMAND} -E remove ${BINARY_DIR}/*.a
    # COMMAND ${CMAKE_COMMAND} -E remove ${BINARY_DIR}/plugins/*.a
)

if (UNIX OR MINGW)
    add_custom_command(TARGET bin POST_BUILD
        COMMAND /bin/sh ${CMAKE_CURRENT_BINARY_DIR}/run_ldd.sh ${BINARY_DIR}/${EXE_NAME}.exe
    )
endif()

if (MSVC OR MINGW)
    add_custom_command(TARGET bin POST_BUILD
        COMMAND windeployqt.exe ${BINARY_DIR}/${EXE_NAME}.exe
    )
endif()
