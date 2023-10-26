set(PLUGIN_NAME plugin2)
set(PLUGIN_NAME ${PLUGIN_NAME} PARENT_SCOPE)
message("Configuring ${PLUGIN_NAME}")

file(GLOB ${PLUGIN_NAME}_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB ${PLUGIN_NAME}_HDR ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library (${PLUGIN_NAME} SHARED
    ${${PLUGIN_NAME}_SRC}
    ${${PLUGIN_NAME}_HDR}
)

target_compile_definitions(${PLUGIN_NAME} PRIVATE DPVISION_PLUGIN_DLL)

target_link_libraries (${PLUGIN_NAME}
	dpVision::Core
	dpVision::PluginManager
	Eigen3::Eigen
)

if (MSVC)
	set_target_properties( ${PLUGIN_NAME} PROPERTIES
		FOLDER "plugins"
		LINKER_LANGUAGE CXX
	)
	add_custom_command(TARGET ${PLUGIN_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PLUGIN_NAME}>/plugins/
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PLUGIN_NAME}> $<TARGET_FILE_DIR:${PLUGIN_NAME}>/plugins/
	)
else (MSVC)
	set_target_properties(${PLUGIN_NAME} PROPERTIES
		LINKER_LANGUAGE CXX
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
	)
endif (MSVC)

