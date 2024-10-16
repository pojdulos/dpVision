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
