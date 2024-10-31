message( "\nConfiguring QuaZip:\n" )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

message("CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
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

#if (MSVC)
#	install(TARGETS ${QUAZIP_LIB} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/bin/")
#	install(FILES "${CMAKE_BINARY_DIR}/quazip1-qt5.dll" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/bin/")
#endif (MSVC)
