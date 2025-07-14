# api/CMakeLists.txt

file(GLOB GUI_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.ui
)

# Dodanie zasobów Qt
qt5_add_resources(QRC_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}.qrc)

set(DPVISION_GUI_SRC
    ${GUI_SRC}
    ${QRC_FILE}
)

add_library(dpVisionGui SHARED
    ${DPVISION_GUI_SRC}
)


set(AUTOGEN_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/gui/dpVisionGui_autogen/include")

target_include_directories(dpVisionGui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
    ${CMAKE_CURRENT_SOURCE_DIR}/../libs/PluginManager
    ${AUTOGEN_INCLUDE_DIR}
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/gui/dpVisionGui_autogen/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(dpVisionGui
    OpenGL::GL
    ${GLU_LIBRARY}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::OpenGL
    Eigen3::Eigen
)

add_library(dpVision::Gui ALIAS dpVisionGui)

