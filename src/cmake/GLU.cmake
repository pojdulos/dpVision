message("\nConfiguring GLU:\n")

if(TARGET OpenGL::GLU)
    if(NOT TARGET dpVision::GLU)
        add_library(dpVision::GLU INTERFACE IMPORTED)
        set_target_properties(dpVision::GLU PROPERTIES
            INTERFACE_LINK_LIBRARIES OpenGL::GLU
        )
    endif()
    message("-- Using GLU target from OpenGL package")
else()
    find_library(GLU_LIBRARY NAMES GLU glu MesaGLU)
    if(NOT GLU_LIBRARY)
        message(FATAL_ERROR "GLU library not found")
    endif()

    if(NOT TARGET dpVision::GLU)
        add_library(dpVision::GLU UNKNOWN IMPORTED)
        set_target_properties(dpVision::GLU PROPERTIES
            IMPORTED_LOCATION "${GLU_LIBRARY}"
        )
    endif()
    message("-- Using manually detected GLU library: ${GLU_LIBRARY}")
endif()
