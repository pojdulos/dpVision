#include "stdafx.h"
#include "CommonDLL.h"
#include "ConcretePlugin.h"

#include "../api/AP.h"

ConcretePlugin::ConcretePlugin(void)
{
	setName( PLUGIN_NAME );
    CommonDLL::Instance().AddName( PLUGIN_NAME );
}

ConcretePlugin::~ConcretePlugin(void)
{
}

#include "../api/adapters/PluginUIAPIAdapter.h"
#include "FileConnector.h"
#include "ParserWRL.h"

namespace {
PluginUIAPIAdapter& uiApi()
{
    static PluginUIAPIAdapter api;
    return api;
}
}

void ConcretePlugin::onLoad()
{
	CFileConnector::regParser( new CParserWRL() );
}

void ConcretePlugin::onUnload()
{
	CFileConnector::unregParser( "wrl" );
}

void ConcretePlugin::MarchingTetrahedron(void)
{
	uiApi().messageBox().information(QString::fromWCharArray(L"Nothing to do at now !!!\n\nThis is a parser plugin. Look for it's extentions in OpenFile dialog."));
}
