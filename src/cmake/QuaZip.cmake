find_path(QUAZIP_INCLUDE_DIR quazip.h
    PATH_SUFFIXES quazip5
    PATHS /usr/include /usr/local/include
)
find_library(QUAZIP_LIBRARY NAMES quazip5)

if(QUAZIP_INCLUDE_DIR AND QUAZIP_LIBRARY)
    message(STATUS "Found QuaZip: ${QUAZIP_LIBRARY} with includes in ${QUAZIP_INCLUDE_DIR}")
    add_library(QuaZip::QuaZip UNKNOWN IMPORTED)
    set_target_properties(QuaZip::QuaZip PROPERTIES
        IMPORTED_LOCATION ${QUAZIP_LIBRARY}
        INTERFACE_INCLUDE_DIRECTORIES ${QUAZIP_INCLUDE_DIR}
    )
else()
    message(FATAL_ERROR "QuaZip not found. Please install libquazip5-dev (Ubuntu) or set paths manually.")
endif()

target_link_libraries(${EXE_NAME} QuaZip::QuaZip)
