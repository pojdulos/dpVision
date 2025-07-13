set (_NAME PMFactory)
message("Configuring ${_NAME}")

#find_package (Qt5Core)
#find_package (Qt5OpenGL)

set (PMFactory_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

file(GLOB ${_NAME}_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB ${_NAME}_HDR ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library (${_NAME} STATIC
    ${${_NAME}_SRC}
    ${${_NAME}_HDR}
)

target_link_libraries (${_NAME}
	Qt5::Core
	Qt5::OpenGL
	Eigen3::Eigen
	# dpVision::Api
)

target_include_directories (${_NAME} PUBLIC
	${dpVision_INCLUDE_DIR}
)

target_include_directories (${_NAME} PUBLIC .)

set_target_properties(${_NAME} PROPERTIES
	LINKER_LANGUAGE CXX
)
