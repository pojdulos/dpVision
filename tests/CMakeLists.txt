set(DPVISION_TESTS_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/TestMain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ApiBoundaryTests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StatusBarManagerTests.cpp
)

add_executable(dpVisionApiTests ${DPVISION_TESTS_SRC})

target_include_directories(dpVisionApiTests PRIVATE
    ${CMAKE_SOURCE_DIR}/src/api
    ${CMAKE_SOURCE_DIR}/src/core
)

target_link_libraries(dpVisionApiTests PRIVATE
    dpVision::Core
    dpVision::LegacyApi
)

add_test(NAME dpVisionApiTests COMMAND dpVisionApiTests)
set_tests_properties(dpVisionApiTests PROPERTIES
    ENVIRONMENT "PATH=$<TARGET_FILE_DIR:dpVisionApiTests>;$<TARGET_FILE_DIR:dpVisionCore>;${CMAKE_BINARY_DIR}/src/libs/PluginManager/$<CONFIG>;${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/debug/bin;${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/bin;$ENV{PATH}")

if(WIN32)
    add_custom_command(TARGET dpVisionApiTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:dpVisionApiTests>
            $<TARGET_FILE_DIR:dpVisionApiTests>
        COMMAND_EXPAND_LISTS
    )
endif()
