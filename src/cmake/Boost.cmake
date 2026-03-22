message("\nConfiguring Boost headers:\n")

set(_dpvision_boost_candidates
    "${CMAKE_SOURCE_DIR}/3rdParty"
    "${PROJECT_SOURCE_DIR}/3rdParty"
)

find_path(Boost_INCLUDE_DIRS
    NAMES boost/version.hpp
)

if(NOT Boost_INCLUDE_DIRS)
    foreach(_dpvision_boost_root IN LISTS _dpvision_boost_candidates)
        if(EXISTS "${_dpvision_boost_root}")
            file(GLOB _dpvision_boost_subdirs RELATIVE "${_dpvision_boost_root}" "${_dpvision_boost_root}/*")
            foreach(_dpvision_boost_subdir IN LISTS _dpvision_boost_subdirs)
                set(_dpvision_boost_include_dir "${_dpvision_boost_root}/${_dpvision_boost_subdir}")
                if(EXISTS "${_dpvision_boost_include_dir}/boost/version.hpp")
                    set(Boost_INCLUDE_DIRS "${_dpvision_boost_include_dir}")
                    break()
                endif()
            endforeach()
        endif()

        if(Boost_INCLUDE_DIRS)
            break()
        endif()
    endforeach()
endif()

if(NOT Boost_INCLUDE_DIRS)
    message(FATAL_ERROR "Boost headers not found. Install boost-array and boost-multi-array via vcpkg/system packages or place Boost headers in 3rdParty")
endif()

if(NOT TARGET Boost::boost)
    add_library(Boost::boost INTERFACE IMPORTED)
    set_target_properties(Boost::boost PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIRS}"
    )
endif()

message("-- Using Boost headers from ${Boost_INCLUDE_DIRS}")
