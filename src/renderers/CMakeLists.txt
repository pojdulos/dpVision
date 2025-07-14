# renderers/CMakeLists.txt

file(GLOB RENDERERS_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)
add_library(dpVisionRenderers STATIC ${RENDERERS_SRC})

add_library(dpVision::Renderers ALIAS dpVisionRenderers)

target_include_directories(dpVisionRenderers
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${dpVision_INCLUDE_DIR}
        $<BUILD_INTERFACE:${AUTOGEN_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
)

# do not link target to dpVision::Core,
# only add as include path
target_link_libraries(dpVisionRenderers
    Qt5::OpenGL
    OpenGL::GL
    Eigen3::Eigen
)
