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

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Możliwe konfiguracje" FORCE)

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

find_package (Qt5Core REQUIRED)
find_package (Qt5Gui REQUIRED)
find_package (Qt5Widgets REQUIRED)
find_package (Qt5OpenGL REQUIRED)
find_package (Qt5Xml REQUIRED)
find_package (Qt5Network REQUIRED)

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

#############################################################################
# ZEWNETRZNE BIBLIOTEKI
#############################################################################

# Funkcja do sprawdzenia, czy pakiet jest znaleziony
function(find_or_add_subdirectory package subdir)
    find_package(${package} QUIET)
    if (NOT ${package}_FOUND)
        message(STATUS "${package} not found, adding subdirectory ${subdir}")
        add_subdirectory(${subdir})
        # find_package(${package} REQUIRED)
    endif()
endfunction()


#find_package( OpenCV REQUIRED )
#add_subdirectory(libs/opencv)

#find_or_add_subdirectory(OpenCV libs/opencv)

# find_package(OpenCV REQUIRED)
# target_link_libraries (${EXE_NAME}
# 	${OpenCV_LIBRARIES}
# )

find_package(OpenCV QUIET)
if (NOT OpenCV_FOUND)
    include(ExternalProject)

	file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/opencv/include)
    
	# Ustawienia architektury 64-bitowej
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /DWIN64 /D_WIN64 /DAMD64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DWIN64 /D_WIN64 /DAMD64")

    #set(OpenCV_BUILD_LIST "world" CACHE STRING "List of modules to build")
    #set(OpenCV_BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries (DLLs)")


	set(OPENCV_SOURCE_DIR ${CMAKE_SOURCE_DIR}/3rdParty/opencv-4.10.0)
	set(VERSION_HEADER_PATH "${OPENCV_SOURCE_DIR}/modules/core/include/opencv2/core/version.hpp")

if(EXISTS "${VERSION_HEADER_PATH}")
    file(READ "${VERSION_HEADER_PATH}" _version_header)

    string(REGEX MATCHALL "#define[ \t]+CV_VERSION_MAJOR[ \t]+([0-9]+)" _major_match "${_version_header}")
    string(REGEX MATCHALL "#define[ \t]+CV_VERSION_MINOR[ \t]+([0-9]+)" _minor_match "${_version_header}")
    string(REGEX MATCHALL "#define[ \t]+CV_VERSION_REVISION[ \t]+([0-9]+)" _revision_match "${_version_header}")

    message(STATUS "Major match: ${_major_match}")
    message(STATUS "Minor match: ${_minor_match}")
    message(STATUS "Revision match: ${_revision_match}")

    if (_major_match)
        string(REGEX REPLACE ".*#define[ \t]+CV_VERSION_MAJOR[ \t]+([0-9]+).*" "\\1" OPENCV_VERSION_MAJOR "${_major_match}")
    else()
        set(OPENCV_VERSION_MAJOR "0")
    endif()

    if (_minor_match)
        string(REGEX REPLACE ".*#define[ \t]+CV_VERSION_MINOR[ \t]+([0-9]+).*" "\\1" OPENCV_VERSION_MINOR "${_minor_match}")
    else()
        set(OPENCV_VERSION_MINOR "0")
    endif()

    if (_revision_match)
        string(REGEX REPLACE ".*#define[ \t]+CV_VERSION_REVISION[ \t]+([0-9]+).*" "\\1" OPENCV_VERSION_REVISION "${_revision_match}")
    else()
        set(OPENCV_VERSION_REVISION "0")
    endif()

    set(OPENCV_VERSION "${OPENCV_VERSION_MAJOR}${OPENCV_VERSION_MINOR}${OPENCV_VERSION_REVISION}")

    message(STATUS "OpenCV version: ${OPENCV_VERSION_MAJOR}.${OPENCV_VERSION_MINOR}.${OPENCV_VERSION_REVISION}")
    message(STATUS "OpenCV version combined: ${OPENCV_VERSION}")
	
    ExternalProject_Add(
        opencv
        SOURCE_DIR ${OPENCV_SOURCE_DIR}
#        BINARY_DIR ${CMAKE_BINARY_DIR}/opencv/build
		CMAKE_GENERATOR ${gen}
        CMAKE_ARGS
#			-DCMAKE_PREFIX_PATH=${CMAKE_BINARY_DIR}/opencv
			-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/opencv
            -DBUILD_opencv_world=ON
            -DBUILD_LIST=core,imgproc,imgcodecs,highgui,features2d#,videoio,video,calib3d,objdetect,dnn,ml,flann,photo,stitching,gapi
            -DBUILD_DOCS=OFF
            -DBUILD_EXAMPLES=OFF
            -DBUILD_TESTS=OFF
            -DBUILD_SHARED_LIBS=ON
            -DCMAKE_SYSTEM_PROCESSOR=AMD64
            -DCMAKE_GENERATOR_PLATFORM=x64
		UPDATE_COMMAND ""
        PATCH_COMMAND ""
        TEST_COMMAND ""
        INSTALL_COMMAND ${CMAKE_COMMAND} --build . --config $<CONFIG> --target install
    )
	
else()
	set(OPENCV_VERSION 4100)

    ExternalProject_Add(
        opencv
        GIT_REPOSITORY https://github.com/opencv/opencv.git
        GIT_TAG 4.10.0
#        BINARY_DIR ${CMAKE_BINARY_DIR}/opencv/build
		CMAKE_GENERATOR ${gen}
        CMAKE_ARGS
#			-DCMAKE_PREFIX_PATH=${CMAKE_BINARY_DIR}/opencv
			-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/opencv
            -DBUILD_opencv_world=ON
            -DBUILD_LIST=core,imgproc,imgcodecs,highgui,features2d#,videoio,video,calib3d,objdetect,dnn,ml,flann,photo,stitching,gapi
            -DBUILD_DOCS=OFF
            -DBUILD_EXAMPLES=OFF
            -DBUILD_TESTS=OFF
            -DBUILD_SHARED_LIBS=ON
            -DCMAKE_SYSTEM_PROCESSOR=AMD64
            -DCMAKE_GENERATOR_PLATFORM=x64
		UPDATE_COMMAND ""
        PATCH_COMMAND ""
        TEST_COMMAND ""
        INSTALL_COMMAND ${CMAKE_COMMAND} --build . --config $<CONFIG> --target install
    )
endif()


    # Dodajemy ścieżki do nagłówków i bibliotek po zbudowaniu OpenCV
    # ExternalProject_Get_Property(opencv install_dir)
	set(install_dir ${CMAKE_BINARY_DIR}/opencv)
	
    # Dynamiczne uzyskanie architektury i kompilatora
    if(CMAKE_GENERATOR MATCHES "Visual Studio")
        #set(MY_VS_PREFIX /${CMAKE_VS_PLATFORM_NAME}/${CMAKE_VS_PLATFORM_TOOLSET})
        set(MY_VS_PREFIX /${CMAKE_VS_PLATFORM_NAME}/vc17)
    else()
        set(MY_VS_PREFIX "")
    endif()
    
    set(OPENCV_BIN_DIR ${install_dir}${MY_VS_PREFIX}/bin)
    set(OPENCV_LIB_DIR ${install_dir}${MY_VS_PREFIX}/lib)
    set(OPENCV_INCLUDE_DIR ${install_dir}/include)	
	
    set(OpenCV_INCLUDE_DIRS ${OPENCV_INCLUDE_DIR})

    link_directories(${OPENCV_LIB_DIR})

    # Ustawienie OpenCV_LIBS w zależności od konfiguracji
    set(OpenCV_LIBS
        $<$<CONFIG:Debug>:${OPENCV_LIB_DIR}/opencv_world${OPENCV_VERSION}d.lib>
        $<$<CONFIG:Release>:${OPENCV_LIB_DIR}/opencv_world${OPENCV_VERSION}.lib>
    )

	# target_link_libraries(${PROJECT_NAME}
		# $<$<CONFIG:Debug>:${OPENCV_LIB_DIR}/opencv_world${OPENCV_VERSION}d.lib>
		# $<$<CONFIG:Release>:${OPENCV_LIB_DIR}/opencv_world${OPENCV_VERSION}.lib>
	# )

    add_dependencies(${EXE_NAME} opencv)
	
	# Custom command to copy DLLs
    add_custom_command(TARGET opencv
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${OPENCV_BIN_DIR} ${CMAKE_BINARY_DIR}/$<CONFIG>
	# COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PLUGIN_NAME}> $<TARGET_FILE_DIR:${PLUGIN_NAME}>/plugins/ 
    )	
else()
    message("OpenCV found: ${OpenCV_VERSION_MAJOR}.${OPENCV_VERSION_MINOR}.${OPENCV_VERSION_REVISION}")
endif()


include_directories(${OpenCV_INCLUDE_DIRS})

target_include_directories ( ${EXE_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS} )
target_link_libraries ( ${EXE_NAME} ${OpenCV_LIBS} )


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
find_package(Boost QUIET)
if (Boost_FOUND)
	message( "-- Boost found.")
else (Boost_FOUND)
	message( "-- Boost not found in system path. Looking in 3rdParty folder...")

	find_subdirs( ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty "boost")
	
	list(LENGTH SUBDIRS SUBDIR_COUNT)
	while((SUBDIR_COUNT GREATER 0) AND (NOT Boost_FOUND))
		list(POP_FRONT SUBDIRS SUBDIR)
		set (PTH "${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/${SUBDIR}")
		if(IS_DIRECTORY ${PTH} AND EXISTS "${PTH}/boost/version.hpp")
			set(Boost_INCLUDE_DIR ${PTH} CACHE PATH "Boost directory" FORCE)
			find_package(Boost QUIET)
		endif()
		list(LENGTH SUBDIRS SUBDIR_COUNT)
	endwhile()
	
	if (Boost_FOUND)
		message( "-- Boost found in ${PTH}" )
	else (Boost_FOUND)
		message( "-- Boost not found. Specify Boost_INCLUDE_DIR variable and reconfigure, please...")
	endif (Boost_FOUND)
endif (Boost_FOUND)

# message( "Configuring DicomHero6 (Imebra)\n" )
# add_subdirectory(3rdParty/DicomHero6)
# target_link_libraries(dicomhero6 ws2_32)

message( "\nConfiguring internal libraries...\n" )

# set ( xerces-c_INSTALL OFF )
# add_subdirectory(libs/xerces-c-3.2.4)
# add_subdirectory(libs/libE57)
add_subdirectory(libs/PMFactory)
add_subdirectory(libs/PluginManager)
# add_subdirectory(libs/PolyVox)
# add_subdirectory(libs/Volumetric)
# add_subdirectory(libs/Lidar)

# get_cmake_property(_variableNames VARIABLES)
# list(SORT _variableNames)
# foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()


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
