# This is modified version of libICP CMakeLists.txt created for dpVision project

cmake_minimum_required (VERSION 3.20)
project (libICP)


# directories
set (LIBICP_SRC_DIR src)

# include directory
include_directories(
	${LIBICP_SRC_DIR}
	${Boost_INCLUDE_DIRS}
)

# determine if OpenMP can/should be used
# option(USE_OPENMP "Enable OpenMP?" ON) # set to OFF to disable
# if(USE_OPENMP)
 # FIND_PACKAGE(OpenMP)
 # if(OPENMP_FOUND OR OpenMP_FOUND)
  # message(STATUS "OpenMP flags = ${OpenMP_CXX_FLAGS}")
  # set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
 # else(OPENMP_FOUND OR OpenMP_FOUND)
  # message(STATUS "OpenMP not supported")
 # endif(OPENMP_FOUND OR OpenMP_FOUND)
# endif(USE_OPENMP)

# sources
FILE(GLOB LIBICP_SRC_FILES "src/*.cpp")

add_library(libICP STATIC ${LIBICP_SRC_FILES})

target_link_libraries (libICP
	Eigen3::Eigen
	Boost::boost
	dpVision
)