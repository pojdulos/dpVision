set (_NAME PolyVox)
message("Configuring ${_NAME}")

set (${_NAME}_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include" PARENT_SCOPE)

file(GLOB ${_NAME}_SRC
	${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/Impl/*.cpp
)

file(GLOB ${_NAME}_HDR
	${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
)

add_library (${_NAME} STATIC
    ${${_NAME}_SRC}
    ${${_NAME}_HDR}
)

target_link_libraries (${_NAME}
#	Qt5::Core
#	Qt5::OpenGL
#	Eigen3::Eigen
)

target_include_directories (${_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include/
#	${dpVision_INCLUDE_DIR}
)

set_target_properties(${_NAME} PROPERTIES
	LINKER_LANGUAGE CXX
)
