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
