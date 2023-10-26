set (_NAME Volumetric)
message("Configuring ${_NAME}")

set (Volumetric_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

file(GLOB ${_NAME}_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB ${_NAME}_HDR ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library (${_NAME} SHARED
    ${${_NAME}_SRC}
    ${${_NAME}_HDR}
)

target_link_libraries (${_NAME}
	dpVision::Core
	PolyVox
	Qt5::Core
	Qt5::OpenGL
	Eigen3::Eigen
)

target_include_directories (${_NAME} PUBLIC
	${dpVision_INCLUDE_DIR}
    ${PolyVox_INCLUDE_DIR}
)

set_target_properties(${_NAME} PROPERTIES
	LINKER_LANGUAGE CXX
)
