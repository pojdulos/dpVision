message("\nDetecting Compiler version:")
if(DEFINED MY_PROJECT_ARCH AND DEFINED MY_PROJECT_RUNTIME)
  # custom overridden values
elseif(MSVC)
  # see Modules/CMakeGenericSystem.cmake
  if("${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
	set(MY_PROJECT_ARCH "x64")
  elseif("${CMAKE_GENERATOR_PLATFORM}" MATCHES "ARM64")
	set(MY_PROJECT_ARCH "ARM64")
  elseif("${CMAKE_GENERATOR}" MATCHES "ARM")
	set(MY_PROJECT_ARCH "ARM")
  elseif("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
	set(MY_PROJECT_ARCH "x64")
  else()
	set(MY_PROJECT_ARCH x86)
  endif()

  if(MSVC_VERSION EQUAL 1400)
	set(MY_PROJECT_RUNTIME vc8)
  elseif(MSVC_VERSION EQUAL 1500)
	set(MY_PROJECT_RUNTIME vc9)
  elseif(MSVC_VERSION EQUAL 1600)
	set(MY_PROJECT_RUNTIME vc10)
  elseif(MSVC_VERSION EQUAL 1700)
	set(MY_PROJECT_RUNTIME vc11)
  elseif(MSVC_VERSION EQUAL 1800)
	set(MY_PROJECT_RUNTIME vc12)
  elseif(MSVC_VERSION EQUAL 1900)
	set(MY_PROJECT_RUNTIME vc14)
  elseif(MSVC_VERSION MATCHES "^191[0-9]$")
	set(MY_PROJECT_RUNTIME vc15)
  elseif(MSVC_VERSION MATCHES "^192[0-9]$")
	set(MY_PROJECT_RUNTIME vc16)
  elseif(MSVC_VERSION MATCHES "^19[34][0-9]$")
	set(MY_PROJECT_RUNTIME vc17)
  else()
	message(WARNING "dpVision does not recognize MSVC_VERSION \"${MSVC_VERSION}\". Cannot set MY_PROJECT_RUNTIME")
  endif()
elseif(MINGW)
  set(MY_PROJECT_RUNTIME mingw)

  if(CMAKE_SYSTEM_PROCESSOR MATCHES "amd64.*|x86_64.*|AMD64.*")
	set(MY_PROJECT_ARCH x64)
  else()
	set(MY_PROJECT_ARCH x86)
  endif()
endif()
message("-- Runtime: ${MY_PROJECT_RUNTIME}")
message("-- Architecture: ${MY_PROJECT_ARCH}")
