# core/CMakeLists.txt

file(GLOB CORE_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tinyspline.c
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/events/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/events/*.h
)

set(DPVISION_CORE_SRC
    ${CORE_SRC}
)

add_library(dpVisionCore SHARED
    ${DPVISION_CORE_SRC}
)

target_include_directories(dpVisionCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../gui
    ${dpVision_INCLUDE_DIR}
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/gui/dpVisionGui_autogen/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(dpVisionCore
    Qt5::Xml
    Qt5::Network
    OpenMP::OpenMP_CXX
    Eigen3::Eigen
    QuaZip::QuaZip
    dpVision::Renderers
    dpVision::PluginManager
    PMFactory
)

add_library(dpVision::Core ALIAS dpVisionCore)

target_include_directories ( dpVisionCore PUBLIC ${OpenCV_INCLUDE_DIRS} )
target_link_libraries ( dpVisionCore ${OpenCV_LIBS} )
