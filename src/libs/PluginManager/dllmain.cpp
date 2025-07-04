// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#ifdef _WIN32
#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
#else
// Called when the library is loaded
__attribute__((constructor))
static void onLibraryLoad(void)
{
    // You can put initialization code here
}

// Called when the library is unloaded
__attribute__((destructor))
static void onLibraryUnload(void)
{
    // You can put cleanup code here
}
#endif
