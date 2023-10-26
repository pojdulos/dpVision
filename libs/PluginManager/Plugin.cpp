#include "StdAfx.h"
#include "CommonDLL.h"
#include "Plugin.h"

Plugin::Plugin(void)
{
    PluginInterface::setUUID(PLUGIN_UUID);
    PluginInterface::setName(PLUGIN_NAME);
    CommonDLL::Instance().AddName( QString(PLUGIN_NAME).toStdWString() );
}


inline std::string Plugin::uuid() const
{
    return m_UUID.toStdString();
}

std::wstring Plugin::name() const
{
    return m_Name.toStdWString();
}

void Plugin::setName( const std::wstring& name )
{
    m_Name = QString::fromWCharArray(name.c_str());
}

void Plugin::setUUID(const std::string& uuid)
{
    m_UUID = QString::fromUtf8(uuid.c_str());
}


std::wstring Plugin::getPath() const
{
    return m_Path.toStdWString();
}

bool Plugin::hasUUID(std::string strUUID)
{
    return (m_UUID == QString::fromUtf8(strUUID.c_str()));
}

void Plugin::setPath( const std::wstring& path )
{
    m_Path = QString::fromWCharArray(path.c_str());
}

