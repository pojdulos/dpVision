# api/CMakeLists.txt

file(GLOB API_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/adapters/*.cpp
)
add_library(dpVisionApi STATIC ${API_SRC})

add_library(dpVision::Api ALIAS dpVisionApi)

target_include_directories(dpVisionApi
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        $<BUILD_INTERFACE:${AUTOGEN_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(dpVisionApi
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::OpenGL
    Qt5::Xml
    Qt5::Network
    OpenGL::GL
    ${GLU_LIBRARY}
    OpenMP::OpenMP_CXX
    Eigen3::Eigen
    dpVision::PluginManager # (jeśli PluginManager to alias do realnej biblioteki)
)
