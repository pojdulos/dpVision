#ifdef DPVISION_DLL_API
#	undef DPVISION_DLL_API
#endif

#ifndef DPVISION_STATIC
# if defined(DPVISION_PLUGIN_DLL)
#  define DPVISION_DLL_API __declspec(dllimport)
# else
#  define DPVISION_DLL_API __declspec(dllexport)
# endif
#else
# define DPVISION_DLL_API
#endif

