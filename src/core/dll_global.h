//#ifdef DPVISION_DLL_API
//#undef DPVISION_DLL_API
//#endif
//
//#ifdef DPVISION_IMPORT
//#undef DPVISION_IMPORT
//#endif
//
//#ifdef DPVISION_EXPORT
//#undef DPVISION_EXPORT
//#endif
//
//#ifdef DPVISION_EXPORT2
//#undef DPVISION_EXPORT2
//#endif
//
//#include <QtCore/qglobal.h>
//
//#ifdef DPVISION_PLUGIN_DLL
//#	define DPVISION_DLL_API Q_DECL_EXPORT
////#	define DPVISION_IMPORT Q_DECL_EXPORT
//#	define DPVISION_EXPORT2 Q_DECL_IMPORT
//#else
//#	define DPVISION_DLL_API Q_DECL_IMPORT
////#	define DPVISION_IMPORT Q_DECL_IMPORT
//#	define DPVISION_EXPORT2 Q_DECL_EXPORT
//#endif
//
//#define DPVISION_EXPORT Q_DECL_EXPORT

#pragma once

#ifdef DPVISION_DLL_API
#  undef DPVISION_DLL_API
#endif

#ifdef DPVISION_IMPORT
#  undef DPVISION_IMPORT
#endif

#ifdef DPVISION_EXPORT
#  undef DPVISION_EXPORT
#endif

#ifdef DPVISION_EXPORT2
#  undef DPVISION_EXPORT2
#endif

// Detekcja kompilatora
#if defined(_WIN32) || defined(_WIN64)
#  define DP_DECL_EXPORT __declspec(dllexport)
#  define DP_DECL_IMPORT __declspec(dllimport)
#else
#  define DP_DECL_EXPORT __attribute__((visibility("default")))
#  define DP_DECL_IMPORT __attribute__((visibility("default")))
#endif

#ifdef DPVISION_PLUGIN_DLL
#  define DPVISION_DLL_API DP_DECL_EXPORT
// #define DPVISION_IMPORT DP_DECL_EXPORT
#  define DPVISION_EXPORT2 DP_DECL_IMPORT
#else
#  define DPVISION_DLL_API DP_DECL_IMPORT
// #define DPVISION_IMPORT DP_DECL_IMPORT
#  define DPVISION_EXPORT2 DP_DECL_EXPORT
#endif

#define DPVISION_EXPORT DP_DECL_EXPORT
