#pragma once
#include "dll_global.h"

#include <vector>
#include <string>

typedef std::vector<std::wstring> NameList;

class CommonDLLImpl;

// This class is exported from the MainDLL.dll
class DPVISION_DLL_API CommonDLL {
public:
    // Return a static instance of this class
    static CommonDLL& Instance();

    void AddName( const std::wstring& name );
    const NameList& GetNames() const;

private:
    CommonDLL(void);
    virtual ~CommonDLL(void);

    CommonDLLImpl* m_pImpl;
    // This generates warnings:
    //NameList     m_Names;
};

