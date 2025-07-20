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

#include "../api/UI.h"
#include "FileConnector.h"
#include "ParserWRL.h"

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
	UI::MESSAGEBOX::information( L"Nothing to do at now !!!\n\nThis is a parser plugin. Look for it's extentions in OpenFile dialog." );
}
