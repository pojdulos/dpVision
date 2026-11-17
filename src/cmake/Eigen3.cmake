message("\nConfiguring Eigen3:\n")

find_package(Eigen3 CONFIG QUIET NO_MODULE)
if(NOT Eigen3_FOUND)
    find_package(Eigen3 QUIET)
endif()

if(NOT Eigen3_FOUND)
    message("-- Eigen3 not found in package manager paths. Looking in bundled locations")

    set(_dpvision_eigen_candidates
        "${CMAKE_SOURCE_DIR}/3rdParty/Eigen3"
        "${PROJECT_SOURCE_DIR}/3rdParty/Eigen3"
    )

    foreach(_dpvision_eigen_dir IN LISTS _dpvision_eigen_candidates)
        if(EXISTS "${_dpvision_eigen_dir}/share/eigen3/Eigen3Config.cmake")
            set(Eigen3_DIR "${_dpvision_eigen_dir}/share/eigen3" CACHE PATH "Eigen3 config directory" FORCE)
            find_package(Eigen3 CONFIG QUIET NO_MODULE)
            if(Eigen3_FOUND)
                break()
            endif()
        elseif(EXISTS "${_dpvision_eigen_dir}/Eigen/Core")
            add_library(Eigen3::Eigen INTERFACE IMPORTED)
            set_target_properties(Eigen3::Eigen PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${_dpvision_eigen_dir}"
            )
            set(Eigen3_FOUND ON)
            break()
        endif()
    endforeach()
endif()

if(Eigen3_FOUND)
    message("-- Eigen3 found")
else()
    message(FATAL_ERROR "Eigen3 not found. Install it via vcpkg/system packages or place it in 3rdParty/Eigen3")
endif()
