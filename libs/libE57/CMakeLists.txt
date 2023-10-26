set (_NAME E57RefImpl)
message("Configuring ${_NAME}")

set (E57RefImpl_DIR "${CMAKE_CURRENT_SOURCE_DIR}" PARENT_SCOPE)
set (E57RefImpl_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include" PARENT_SCOPE)

add_library (${_NAME} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/E57Foundation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/E57FoundationImpl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/E57FoundationImpl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/E57Foundation.h
)

target_link_libraries (${_NAME}
	xerces-c
)

target_include_directories (${_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
	${xerces-c_SOURCE_DIR}/src
	${xerces-c_BINARY_DIR}/src
)

set_target_properties(${_NAME} PROPERTIES
	LINKER_LANGUAGE CXX
)
