set (_NAME PluginManager)
message("Configuring ${_NAME}")

file(GLOB ${_NAME}_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB ${_NAME}_HDR ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library (${_NAME} SHARED
    ${${_NAME}_SRC}
    ${${_NAME}_HDR}
)

set (PluginManager_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

target_include_directories (${_NAME} PUBLIC
    ${dpVision_INCLUDE_DIR}
)

target_compile_definitions(${_NAME} PRIVATE
    DPVISION_PLUGIN_DLL
)

set_target_properties(${_NAME} PROPERTIES
    LINKER_LANGUAGE CXX
)
