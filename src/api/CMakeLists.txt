# api/CMakeLists.txt

file(GLOB API_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/adapters/*.cpp
)
add_library(dpVisionApi SHARED ${API_SRC})

add_library(dpVision::Api ALIAS dpVisionApi)

set(AUTOGEN_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/dpVisionCore_autogen/include")

target_include_directories(dpVisionApi PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${dpVision_INCLUDE_DIR}
    ${AUTOGEN_INCLUDE_DIR}
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/core/dpVisionCore_autogen/include>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/gui/dpVisionGui_autogen/include>
    $<BUILD_INTERFACE:${AUTOGEN_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(dpVisionApi
    dpVision::Core
    dpVision::Gui
    Qt5::Core
    Qt5::Gui
    Qt5::OpenGL
    Eigen3::Eigen
)
