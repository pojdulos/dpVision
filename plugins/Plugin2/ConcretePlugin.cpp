#include "StdAfx.h"
#include "CommonDLL.h"
#include "ConcretePlugin.h"

#include "AP.h"
#include "UI.h"

#include "Model3D.h"
#include "GroupObject.h"

ConcretePlugin::ConcretePlugin(void)
{
	setUUID(PLUGIN_UUID);
	setName(PLUGIN_NAME);
    CommonDLL::Instance().AddName( PLUGIN_NAME );

	//ShowMessage( L"I'm LOADED" );
}

ConcretePlugin::~ConcretePlugin(void)
{
	//ShowMessage( L"I'm UNLOADED" );
}

void ConcretePlugin::onActivate() { UI::MESSAGEBOX::information(L"I'm activated", PLUGIN_NAME); };
void ConcretePlugin::onDeactivate() { UI::MESSAGEBOX::information(L"I'm deactivated", PLUGIN_NAME); };
void ConcretePlugin::onRun() { UI::MESSAGEBOX::information(L"Nothing to do...", PLUGIN_NAME); };

void ConcretePlugin::onLoad()
{
	UI::PLUGINPANEL::create( m_ID, L"Automatyzacja PM" );

	UI::PLUGINPANEL::addLabel( m_ID, L"label00", L"start:", 0, 0 );
	UI::PLUGINPANEL::addLabel( m_ID, L"label01", L"step:", 1, 0 );
	UI::PLUGINPANEL::addLabel( m_ID, L"label02", L"stop:", 2, 0 );
	UI::PLUGINPANEL::addEditBox( m_ID, L"editBoxStart", L"First value of reduction", L"50000", 0, 1 );
	UI::PLUGINPANEL::addEditBox( m_ID, L"editBoxStep", L"Step of reduction", L"5000", 1, 1 );
	UI::PLUGINPANEL::addEditBox( m_ID, L"editBoxStop", L"Last value of reduction", L"5000", 2, 1 );
	UI::PLUGINPANEL::addButton( m_ID, L"objButton", L"create OBJ's", 3, 0, 1, 2 );
	UI::PLUGINPANEL::addButton( m_ID, L"pmtButton", L"create PMT's", 4, 0, 1, 2 );
	UI::PLUGINPANEL::addLabel( m_ID, L"label03", L"                                  ", 5, 0, 1, 2 );
}

void ConcretePlugin::onUnload()
{
}

void ConcretePlugin::akcja( std::wstring ext )
{
		CModel3D *obj = AP::WORKSPACE::getCurrentModel();

		int start = std::stoi( UI::PLUGINPANEL::getEditBoxValue( m_ID, L"editBoxStart" ) );
		int step = std::stoi( UI::PLUGINPANEL::getEditBoxValue( m_ID, L"editBoxStep" ) );
		int stop = std::stoi( UI::PLUGINPANEL::getEditBoxValue( m_ID, L"editBoxStop" ) );

		if ( NULL != obj )
		{
			int lb = start;
	
			QFileInfo f( obj->path() );
			
			while ( lb >=stop )
			{
				if ( lb < ((CMesh*)obj->getChild())->size() )
				{
					UI::PLUGINPANEL::setLabel( m_ID, L"label03", L"please wait...\nreducing to " + std::to_wstring( (long long ) lb ) + L" vertices" );
					
					CModel3D *tmp = obj->getCopy();

					tmp->PMeshEcoll( lb );

					QString fname( f.baseName() + "_" + QString::number( lb ) + "." + QString::fromStdWString(ext) );

					UI::PLUGINPANEL::setLabel( m_ID, L"label03", L"saving as:\n" + fname.toStdWString() );
					//UI::PLUGINPANEL::setLabel( m_ID, L"label03", L"Saving" );

					tmp->save( f.absolutePath() + "/" + fname );

					delete tmp;
				}
				lb -= step ;
			}
			UI::PLUGINPANEL::setLabel( m_ID, L"label03", L"Done!" );
		}
		else
		{
				UI::MESSAGEBOX::error( L"No mesh selected!!!" );
		}
}

#include <limits>

#undef min
#undef max

void ConcretePlugin::onButton( std::wstring name )
{
	if ( 0 == name.compare( L"objButton" ) )
	{
		akcja( L"obj" );
	}
	else if ( 0 == name.compare( L"pmtButton" ) )
	{
		akcja( L"pmt" );
	}
}