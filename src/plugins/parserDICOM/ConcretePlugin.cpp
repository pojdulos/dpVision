#include "CommonDLL.h"
#include "ConcretePlugin.h"

#include "../api/AP.h"
#include "../api/adapters/PluginUIAPIAdapter.h"
#include "FileConnector.h"
#include "ParserDICOM.h"

namespace {
PluginUIAPIAdapter& uiApi()
{
    static PluginUIAPIAdapter api;
    return api;
}
}

ConcretePlugin::ConcretePlugin(void)
{
	setName( PLUGIN_NAME );
    CommonDLL::Instance().AddName( PLUGIN_NAME );
}

ConcretePlugin::~ConcretePlugin(void)
{
}

void ConcretePlugin::onLoad()
{
	CFileConnector::regParser(new CParserDICOM());
}

void ConcretePlugin::onUnload()
{
	CFileConnector::unregParser("dcm");
}

void ConcretePlugin::MarchingTetrahedron(void)
{
	uiApi().messageBox().information(QString::fromWCharArray(L"Nothing to do at now !!!\n\nThis is a parser plugin. Look for it's extentions in OpenFile dialog."));
}
