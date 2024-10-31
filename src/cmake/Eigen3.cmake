message( "\nConfiguring Eigen3:\n" )

find_package(Eigen3 QUIET)
if (Eigen3_FOUND)
	message( "-- Eigen3 found")
	target_link_libraries(${EXE_NAME}
		Eigen3::Eigen
	)
else (Eigen3_FOUND)
	message( "-- Eigen3 not found. Looking for Eigen3 directory in 3rdParty folder")
	set(Eigen3_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/Eigen3/" CACHE PATH "Eigen3 directory" FORCE)
	set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/Eigen3/" ${CMAKE_PREFIX_PATH})

	find_package(Eigen3 REQUIRED)

	if (TARGET Eigen3::Eigen)
		message( "-- Eigen3 found in 3rdParty folder.")
		target_link_libraries(${EXE_NAME}
			Eigen3::Eigen
		)
	else (TARGET Eigen3::Eigen)
		message( "-- Eigen3 not found.")
	endif (TARGET Eigen3::Eigen)
endif (Eigen3_FOUND)
