#include "CommonDLL.h"
#include "ConcretePlugin.h"

#include "AP.h"
#include "UI.h"
#include "FileConnector.h"
#include "ParserDICOM.h"

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
	UI::MESSAGEBOX::information( L"Nothing to do at now !!!\n\nThis is a parser plugin. Look for it's extentions in OpenFile dialog." );
}
