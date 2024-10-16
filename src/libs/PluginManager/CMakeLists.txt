set (_NAME PluginManager)
message("Configuring ${_NAME}")

file(GLOB ${_NAME}_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB ${_NAME}_HDR ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library (${_NAME} SHARED
    ${${_NAME}_SRC}
    ${${_NAME}_HDR}
)

set (PluginManager_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

target_link_libraries (${_NAME}
	Qt5::Core
)

target_include_directories (${_NAME} PUBLIC
    ${dpVision_INCLUDE_DIR}
)

target_compile_definitions(${_NAME} PRIVATE
    DPVISION_PLUGIN_DLL
	"PLUGIN_NAME=L\"Plugin wzorcowy\""
)

if (MSVC)
	set_target_properties( ${_NAME} PROPERTIES
		LINKER_LANGUAGE CXX
	)
	add_custom_command(TARGET ${_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${_NAME}> $<TARGET_FILE_DIR:${_NAME}>/
	)
else (MSVC)
	set_target_properties(${_NAME} PROPERTIES
		LINKER_LANGUAGE CXX
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
	)
endif (MSVC)
