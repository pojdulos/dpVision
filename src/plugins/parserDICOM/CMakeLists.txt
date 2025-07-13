set(PLUGIN_NAME parserDICOM)
set(PLUGIN_NAME ${PLUGIN_NAME} PARENT_SCOPE)
message("Configuring ${PLUGIN_NAME}")

message("Path: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB ${PLUGIN_NAME}_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB ${PLUGIN_NAME}_HDR ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB ${PLUGIN_NAME}_IMEBRA_SRC ${CMAKE_CURRENT_SOURCE_DIR}/imebra/src/*.cpp)
file(GLOB ${PLUGIN_NAME}_IMEBRA_IMPL ${CMAKE_CURRENT_SOURCE_DIR}/imebra/implementation/*.cpp)

add_library (${PLUGIN_NAME} SHARED
    ${${PLUGIN_NAME}_SRC}
    ${${PLUGIN_NAME}_HDR}
	${${PLUGIN_NAME}_IMEBRA_SRC}
	${${PLUGIN_NAME}_IMEBRA_IMPL}
)

if(WIN32)
    target_compile_definitions(${PLUGIN_NAME} PRIVATE IMEBRA_WINDOWS)
else()
    target_compile_definitions(${PLUGIN_NAME} PRIVATE IMEBRA_POSIX)
endif()

target_compile_definitions(${PLUGIN_NAME} PRIVATE
	DPVISION_PLUGIN_DLL
	IMEBRA_DLL
	IMEBRA_DLL_EXPORTS
	NOMINMAX
	DISABLE_DIMSE_INTEROPERABILITY_TEST
)

target_link_libraries (${PLUGIN_NAME}
	dpVision::Exe
	dpVision::Core
	#dpVision::Api
	dpVision::PluginManager
	Qt5::Core
	Eigen3::Eigen
	#opencv_world
)

target_include_directories (${PLUGIN_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/imebra/include
	${CMAKE_CURRENT_SOURCE_DIR}/imebra/implementation
)

message("Imebra include path: ${CMAKE_CURRENT_SOURCE_DIR}/imebra/include")
file(GLOB IMEBRA_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/imebra/include/*.h)
message("Imebra headers: ${IMEBRA_HEADERS}")



if (MSVC)
	set_target_properties( ${PLUGIN_NAME} PROPERTIES
		FOLDER "plugins"
		LINKER_LANGUAGE CXX
	)
	add_custom_command(TARGET ${PLUGIN_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PLUGIN_NAME}>/plugins/
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PLUGIN_NAME}> $<TARGET_FILE_DIR:${PLUGIN_NAME}>/plugins/
	)
	
	target_compile_options(${PLUGIN_NAME} PRIVATE /bigobj)
	
else (MSVC)
	set_target_properties(${PLUGIN_NAME} PROPERTIES
		LINKER_LANGUAGE CXX
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
	)
	if (WIN32)
	target_link_libraries (${PLUGIN_NAME}
		ws2_32
	)
	endif (WIN32)
endif (MSVC)

