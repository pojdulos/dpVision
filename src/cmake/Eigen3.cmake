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
