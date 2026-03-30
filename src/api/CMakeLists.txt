# api/CMakeLists.txt

set(DPVISION_LEGACY_API_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/AP.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BoundaryContractChecks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LegacyApiStatics.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/UI.cpp
)

add_library(dpVisionLegacyApi STATIC ${DPVISION_LEGACY_API_SRC})
add_library(dpVision::LegacyApi ALIAS dpVisionLegacyApi)

target_compile_definitions(dpVisionLegacyApi PRIVATE DPVISION_LEGACY_API_BUILD)

set(API_AUTOGEN_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/dpVisionLegacyApi_autogen/include_$<CONFIG>")

target_include_directories(dpVisionLegacyApi PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${dpVision_INCLUDE_DIR}
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src/core/dpVisionCore_autogen/include_$<CONFIG>>
    $<BUILD_INTERFACE:${API_AUTOGEN_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(dpVisionLegacyApi
    PUBLIC
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        dpVision::Core
)

add_library(dpVisionApi INTERFACE)
add_library(dpVision::Api ALIAS dpVisionApi)

target_include_directories(dpVisionApi INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${dpVision_INCLUDE_DIR}
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src/core/dpVisionCore_autogen/include_$<CONFIG>>
    $<INSTALL_INTERFACE:include>
)
