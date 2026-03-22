if(NOT DEFINED MY_PROJECT_ARCH OR NOT DEFINED MY_PROJECT_RUNTIME)
    include(cmake/ArchAndRuntime.cmake)
endif()

message("\nConfiguring OpenCV:\n")

set(_dpvision_opencv_components core imgproc imgcodecs highgui features2d)

if(NOT TARGET dpVisionOpenCV)
    add_library(dpVisionOpenCV INTERFACE)
    add_library(dpVision::OpenCV ALIAS dpVisionOpenCV)
endif()

find_package(OpenCV CONFIG QUIET COMPONENTS ${_dpvision_opencv_components})
if(NOT OpenCV_FOUND)
    find_package(OpenCV QUIET COMPONENTS ${_dpvision_opencv_components})
endif()

if(OpenCV_FOUND)
    message("-- OpenCV ${OpenCV_VERSION} package found")

    set(_dpvision_opencv_link_libs "${OpenCV_LIBS}")
    if(NOT _dpvision_opencv_link_libs)
        foreach(_dpvision_component IN LISTS _dpvision_opencv_components)
            if(TARGET "opencv_${_dpvision_component}")
                list(APPEND _dpvision_opencv_link_libs "opencv_${_dpvision_component}")
            endif()
        endforeach()
    endif()

    target_include_directories(dpVisionOpenCV INTERFACE "${OpenCV_INCLUDE_DIRS}")
    target_link_libraries(dpVisionOpenCV INTERFACE ${_dpvision_opencv_link_libs})
else()
    message("-- OpenCV package not found. Falling back to bundled source build")

    include(ExternalProject)

    set(_dpvision_opencv_source_candidates
        "${CMAKE_SOURCE_DIR}/3rdParty/opencv-4.10.0"
        "${PROJECT_SOURCE_DIR}/3rdParty/opencv-4.10.0"
    )

    foreach(_dpvision_opencv_source_dir IN LISTS _dpvision_opencv_source_candidates)
        set(_dpvision_version_header "${_dpvision_opencv_source_dir}/modules/core/include/opencv2/core/version.hpp")
        if(EXISTS "${_dpvision_version_header}")
            set(OPENCV_SOURCE_DIR "${_dpvision_opencv_source_dir}")
            break()
        endif()
    endforeach()

    if(NOT DEFINED OPENCV_SOURCE_DIR)
        message(FATAL_ERROR "OpenCV not found. Install it via vcpkg/system packages or place sources in 3rdParty/opencv-4.10.0")
    endif()

    file(READ "${OPENCV_SOURCE_DIR}/modules/core/include/opencv2/core/version.hpp" _version_header)
    string(REGEX MATCH "#define[ \t]+CV_VERSION_MAJOR[ \t]+([0-9]+)" _major_match "${_version_header}")
    string(REGEX MATCH "#define[ \t]+CV_VERSION_MINOR[ \t]+([0-9]+)" _minor_match "${_version_header}")
    string(REGEX MATCH "#define[ \t]+CV_VERSION_REVISION[ \t]+([0-9]+)" _revision_match "${_version_header}")
    string(REGEX REPLACE ".*CV_VERSION_MAJOR[ \t]+([0-9]+).*" "\\1" OPENCV_VERSION_MAJOR "${_major_match}")
    string(REGEX REPLACE ".*CV_VERSION_MINOR[ \t]+([0-9]+).*" "\\1" OPENCV_VERSION_MINOR "${_minor_match}")
    string(REGEX REPLACE ".*CV_VERSION_REVISION[ \t]+([0-9]+).*" "\\1" OPENCV_VERSION_REVISION "${_revision_match}")
    set(OPENCV_VERSION "${OPENCV_VERSION_MAJOR}${OPENCV_VERSION_MINOR}${OPENCV_VERSION_REVISION}")

    set(_dpvision_opencv_install_dir "${CMAKE_BINARY_DIR}/opencv")
    set(_dpvision_opencv_cmake_args
        -DCMAKE_INSTALL_PREFIX=${_dpvision_opencv_install_dir}
        -DBUILD_opencv_world=ON
        -DBUILD_LIST=core,imgproc,imgcodecs,highgui,features2d
        -DBUILD_DOCS=OFF
        -DBUILD_EXAMPLES=OFF
        -DBUILD_TESTS=OFF
        -DBUILD_SHARED_LIBS=ON
    )

    if(CMAKE_GENERATOR MATCHES "Visual Studio")
        list(APPEND _dpvision_opencv_cmake_args -DCMAKE_GENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM})
    endif()

    ExternalProject_Add(
        opencv
        SOURCE_DIR "${OPENCV_SOURCE_DIR}"
        CMAKE_GENERATOR "${CMAKE_GENERATOR}"
        CMAKE_ARGS ${_dpvision_opencv_cmake_args}
        UPDATE_COMMAND ""
        PATCH_COMMAND ""
        TEST_COMMAND ""
        INSTALL_COMMAND ${CMAKE_COMMAND} --build . --config $<CONFIG> --target install
    )

    if(CMAKE_GENERATOR MATCHES "Visual Studio")
        set(_dpvision_opencv_lib_dir "${_dpvision_opencv_install_dir}/${MY_PROJECT_ARCH}/${MY_PROJECT_RUNTIME}/lib")
        set(_dpvision_opencv_bin_dir "${_dpvision_opencv_install_dir}/${MY_PROJECT_ARCH}/${MY_PROJECT_RUNTIME}/bin")
        set(_dpvision_opencv_world_lib "$<$<CONFIG:Debug>:${_dpvision_opencv_lib_dir}/opencv_world${OPENCV_VERSION}d.lib>;$<$<CONFIG:Release>:${_dpvision_opencv_lib_dir}/opencv_world${OPENCV_VERSION}.lib>")
    else()
        set(_dpvision_opencv_lib_dir "${_dpvision_opencv_install_dir}/lib")
        set(_dpvision_opencv_bin_dir "${_dpvision_opencv_install_dir}/bin")
        set(_dpvision_opencv_world_lib "${_dpvision_opencv_lib_dir}/libopencv_world.so")
    endif()

    target_include_directories(dpVisionOpenCV INTERFACE "${_dpvision_opencv_install_dir}/include")
    target_link_libraries(dpVisionOpenCV INTERFACE "${_dpvision_opencv_world_lib}")
    add_dependencies(dpVisionOpenCV opencv)

    if(WIN32)
        add_custom_command(TARGET opencv
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${_dpvision_opencv_bin_dir}" "${CMAKE_BINARY_DIR}/$<CONFIG>"
        )
    endif()
endif()
