#include "StdAfx.h"
#include "CommonDLL.h"
#include "ConcretePlugin.h"

#include "AP.h"

ConcretePlugin::ConcretePlugin(void)
{
	setName( PLUGIN_NAME );
    CommonDLL::Instance().AddName( PLUGIN_NAME );
}

ConcretePlugin::~ConcretePlugin(void) {}

#include "UI.h"
#include "FileConnector.h"
#include "ParserTXT.h"
#include "ParserVCN.h"
#include "ParserVC2.h"
#include "ParserPLY.h"
#include "ParserPAT.h"
#include "ParserM.h"
#include "ParserTKD.h"
#include "ParserXYB.h"
#include "ParserPTS.h"
#include "ParserATMDL.h"
//#include "ParserDPV.h"

int ConcretePlugin::isHidden() { return 1; }

void ConcretePlugin::onLoad()
{
	CFileConnector::regParser(new CParserTXT());
	CFileConnector::regParser(new CParserVCN());
	CFileConnector::regParser(new CParserVC2());
	CFileConnector::regParser(new CParserPLY());
	//CFileConnector::regParser(new CParserPAT());
	CFileConnector::regParser(new CParserM());
	CFileConnector::regParser(new CParserTKD());
	CFileConnector::regParser(new CParserXYB());
	CFileConnector::regParser(new CParserPTS());
	CFileConnector::regParser(new CParserATMDL());
	//CFileConnector::regParser(new CParserDPVISION());
}

void ConcretePlugin::onUnload()
{
	CFileConnector::unregParser("txt");
	CFileConnector::unregParser("vc2");
	CFileConnector::unregParser("vcn");
	CFileConnector::unregParser("ply");
	//CFileConnector::unregParser("pat");
	CFileConnector::unregParser("m");
	//CFileConnector::unregParser("tkd");
	CFileConnector::unregParser("xyb");
	CFileConnector::unregParser("pts");
	CFileConnector::unregParser("atmdl");
	//CFileConnector::unregParser("dpvision");
}

void ConcretePlugin::MarchingTetrahedron(void)
{
	UI::MESSAGEBOX::information( L"Nothing to do at now !!!\n\nThis is a parser plugin. Look for it's extentions in OpenFile dialog." );
}
