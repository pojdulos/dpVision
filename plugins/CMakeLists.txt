MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

# W CMakeLists.txt w subdir1

set (TMP_LIST "")

FOREACH(subdir ${SUBDIRS})
    set (PLUGIN_NAME "")

    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/CMakeLists.txt)
		# message( "Subdirectory \"${subdir}\" contains CMakeLists.txt..." )
		add_subdirectory(${subdir})
	endif()

    list (APPEND TMP_LIST ${PLUGIN_NAME})
ENDFOREACH()

set (DPV_PLUGINS ${TMP_LIST} PARENT_SCOPE)
