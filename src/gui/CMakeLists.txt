# api/CMakeLists.txt

file(GLOB GUI_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/adapters/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/events/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/adapters/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/events/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.ui
)

# Dodanie zasobów Qt
qt5_add_resources(QRC_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}.qrc)

set(DPVISION_GUI_SRC
    ${GUI_SRC}
    ${QRC_FILE}
    ${CMAKE_CURRENT_SOURCE_DIR}/../api/AP.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../api/UI.cpp
)

add_library(dpVisionGui SHARED
    ${DPVISION_GUI_SRC}
)

set(GUI_AUTOGEN_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/dpVisionGui_autogen/include_$<CONFIG>")

target_include_directories(dpVisionGui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
    ${CMAKE_CURRENT_SOURCE_DIR}/../libs/PluginManager
    $<BUILD_INTERFACE:${GUI_AUTOGEN_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(dpVisionGui
    OpenGL::GL
    dpVision::GLU
    dpVision::Core
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::OpenGL
    Eigen3::Eigen
)

add_library(dpVision::Gui ALIAS dpVisionGui)

