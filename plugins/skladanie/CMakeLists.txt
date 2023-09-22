set(PLUGIN_NAME skladanie)
set(PLUGIN_NAME ${PLUGIN_NAME} PARENT_SCOPE)
message("Configuring ${PLUGIN_NAME}")

file(GLOB ${PLUGIN_NAME}_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB ${PLUGIN_NAME}_HDR ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library (${PLUGIN_NAME} SHARED
    ${${PLUGIN_NAME}_SRC}
    ${${PLUGIN_NAME}_HDR}
)

if (MSVC)
	set_target_properties( ${PLUGIN_NAME} PROPERTIES FOLDER "plugins")
endif (MSVC)

target_include_directories (${PLUGIN_NAME} PUBLIC
	${dpVision_INCLUDE_DIR}
	${PluginManager_INCLUDE_DIR}
	${CMAKE_SOURCE_DIR}/3rdParty/libICP/src
)

target_compile_definitions(${PLUGIN_NAME} PRIVATE DPVISION_PLUGIN_DLL)

set_target_properties(${PLUGIN_NAME} PROPERTIES
	LINKER_LANGUAGE CXX
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
)

target_link_libraries (${PLUGIN_NAME} 
	libICP
	dpVision
	PluginManager
	Eigen3::Eigen
)
