set(_NAME example)
set(PLUGIN_NAME ${_NAME} PARENT_SCOPE)
message("Configuring ${_NAME}")

file(GLOB ${_NAME}_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB ${_NAME}_HDR ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library (${_NAME} SHARED
    ${${_NAME}_SRC}
    ${${_NAME}_HDR}
)

target_compile_definitions(${_NAME} PRIVATE
	DPVISION_PLUGIN_DLL
)

target_link_libraries (${_NAME} 
	dpVision::Core
	dpVision::PluginManager
	Qt5::Core
)

target_include_directories (${_NAME} PUBLIC
	${dpVision_INCLUDE_DIR}
)

if (MSVC)
	set_target_properties( ${_NAME} PROPERTIES
		FOLDER "plugins"
		LINKER_LANGUAGE CXX
	)
	add_custom_command(TARGET ${_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${_NAME}>/plugins/
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${_NAME}> $<TARGET_FILE_DIR:${_NAME}>/plugins/
	)
else (MSVC)
	set_target_properties(${_NAME} PROPERTIES
		LINKER_LANGUAGE CXX
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
	)
endif (MSVC)


