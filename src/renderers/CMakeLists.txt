# renderers/CMakeLists.txt

file(GLOB RENDERERS_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)
add_library(dpVisionRenderers STATIC ${RENDERERS_SRC})

add_library(dpVision::Renderers ALIAS dpVisionRenderers)

target_include_directories(dpVisionRenderers
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        $<BUILD_INTERFACE:${AUTOGEN_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(dpVisionRenderers
    dpVision::Core
    # Qt5::Core
    # Qt5::Gui
    # Qt5::Widgets
    Qt5::OpenGL
    # Qt5::Xml
    # Qt5::Network
    OpenGL::GL
    # ${GLU_LIBRARY}
    # OpenMP::OpenMP_CXX
    Eigen3::Eigen
    #dpVision::PluginManager # (jeśli PluginManager to alias do realnej biblioteki)
)
