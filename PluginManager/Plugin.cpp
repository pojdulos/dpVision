#include "StdAfx.h"
#include "CommonDLL.h"
#include "Plugin.h"

Plugin::Plugin(void)
{
    setUUID(PLUGIN_UUID);
    setName(PLUGIN_NAME);
    CommonDLL::Instance().AddName( PLUGIN_NAME );
}

Plugin::~Plugin(void)
{
}

inline std::string Plugin::uuid() const
{
    return m_UUID;
}

std::wstring Plugin::name() const
{
    return m_Name;
}

void Plugin::setName( const std::wstring& name )
{
    m_Name = name;
}

void Plugin::setUUID(const std::string& uuid)
{
    m_UUID = uuid;
}


std::wstring Plugin::getPath() const
{
    return m_Path;
}

bool Plugin::hasUUID(std::string strUUID)
{
    return (0 == m_UUID.compare(strUUID));
}

void Plugin::setPath( const std::wstring& path )
{
    m_Path = path;
}

