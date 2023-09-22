// MainDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "CommonDLL.h"

class CommonDLLImpl
{
public:
    NameList m_Names;
};

// This is the constructor of a class that has been exported.
// see MainDLL.h for the class definition
CommonDLL::CommonDLL()
{
    m_pImpl = new CommonDLLImpl();
	return;
}

CommonDLL::~CommonDLL()
{
    m_pImpl->m_Names.clear();
    delete m_pImpl;
}

CommonDLL& CommonDLL::Instance()
{
    static CommonDLL instance;
    return instance;
}

void CommonDLL::AddName( const std::wstring& name )
{
    m_pImpl->m_Names.push_back( name );
}

const NameList& CommonDLL::GetNames() const
{
    return m_pImpl->m_Names;
}