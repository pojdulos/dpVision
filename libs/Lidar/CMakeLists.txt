set (_NAME Lidar)
message("Configuring ${_NAME}")

set (${_NAME}_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" PARENT_SCOPE)

file(GLOB ${_NAME}_SRC
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

file(GLOB ${_NAME}_HDR
	${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

add_library (${_NAME} STATIC
    ${${_NAME}_SRC}
    ${${_NAME}_HDR}
)

target_link_libraries (${_NAME}
	dpVision::Core
	Qt5::Core
	Qt5::OpenGL
#	Eigen3::Eigen
)

target_include_directories (${_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
#	${dpVision_INCLUDE_DIR}
)

set_target_properties(${_NAME} PROPERTIES
	LINKER_LANGUAGE CXX
)
