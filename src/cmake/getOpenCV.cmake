if(NOT DEFINED MY_PROJECT_ARCH OR NOT DEFINED MY_PROJECT_RUNTIME)
	include(cmake/ArchAndRuntime.cmake)
endif()

message( "\nConfiguring OpenCV:\n" )

find_package(OpenCV QUIET)

if (OpenCV_FOUND)
    message("-- OpenCV ${OpenCV_VERSION} package found in ${OpenCV_CONFIG_PATH}")
else()
    message("-- OpenCV not found. Trying to get and configure...")

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
	    message("-- OpenCV sources found in ${OPENCV_SOURCE_DIR}")

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
			CMAKE_GENERATOR ${gen}
			CMAKE_ARGS
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
	    message("-- Trying to clone OpenCV from repository...")
		
		set(OPENCV_VERSION 4100)

		ExternalProject_Add(
			opencv
			GIT_REPOSITORY https://github.com/opencv/opencv.git
			GIT_TAG 4.10.0
			CMAKE_GENERATOR ${gen}
			CMAKE_ARGS
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
	set(install_dir "${CMAKE_BINARY_DIR}/opencv")
	
    # Dynamiczne uzyskanie architektury i kompilatora
    if(CMAKE_GENERATOR MATCHES "Visual Studio")
        #set(MY_VS_PREFIX /${CMAKE_VS_PLATFORM_NAME}/${CMAKE_VS_PLATFORM_TOOLSET})
        # set(MY_VS_PREFIX /${CMAKE_VS_PLATFORM_NAME}/${MY_PROJECT_RUNTIME})
        set(MY_VS_PREFIX "${MY_PROJECT_ARCH}/${MY_PROJECT_RUNTIME}/")
    else()
        set(MY_VS_PREFIX "")
    endif()
    
    set(OPENCV_BIN_DIR "${CMAKE_BINARY_DIR}/opencv/${MY_VS_PREFIX}bin")
    set(OPENCV_LIB_DIR "${CMAKE_BINARY_DIR}/opencv/${MY_VS_PREFIX}lib")
    set(OPENCV_INCLUDE_DIR "${CMAKE_BINARY_DIR}/opencv/include")	
	
    set(OpenCV_INCLUDE_DIRS ${OPENCV_INCLUDE_DIR})

    #link_directories(${OPENCV_LIB_DIR})
	

    # Ustawienie OpenCV_LIBS w zależności od konfiguracji
    set(OpenCV_LIBS
        # $<$<CONFIG:Debug>:${OPENCV_LIB_DIR}/opencv_world${OPENCV_VERSION}d.lib>
        # $<$<CONFIG:Release>:${OPENCV_LIB_DIR}/opencv_world${OPENCV_VERSION}.lib>
        $<$<CONFIG:Debug>:opencv_world${OPENCV_VERSION}d>
        $<$<CONFIG:Release>:opencv_world${OPENCV_VERSION}>
    )

    add_dependencies(${EXE_NAME} opencv)
	
	# Custom command to copy DLLs
    add_custom_command(TARGET opencv
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${OPENCV_BIN_DIR} ${CMAKE_BINARY_DIR}/$<CONFIG>
	# COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PLUGIN_NAME}> $<TARGET_FILE_DIR:${PLUGIN_NAME}>/plugins/ 
    )	
endif()


if(CMAKE_GENERATOR MATCHES "Visual Studio")
	if(DEFINED OpenCV_LIB_PATH)
		target_link_directories(${EXE_NAME} PRIVATE "${OpenCV_LIB_PATH}" )
	elseif(DEFINED OPENCV_LIB_DIR)
		target_link_directories(${EXE_NAME} PRIVATE "${OPENCV_LIB_DIR}" )
		# target_link_directories(${EXE_NAME} PRIVATE "$(ProjectDir)opencv/${MY_PROJECT_ARCH}/${MY_PROJECT_RUNTIME}/lib" )
	endif()
endif()


include_directories(${OpenCV_INCLUDE_DIRS})

target_include_directories ( ${EXE_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS} )
target_link_libraries ( ${EXE_NAME} ${OpenCV_LIBS} )
