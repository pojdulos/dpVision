#include "MainApplication.h"

#include "AP.h"

#include "MainWindow.h"

__declspec(dllexport) CMainApplication * CMainApplication::theApp = nullptr;

//#include <QStyleFactory>

CMainApplication::CMainApplication(int& argc, char** argv) : QApplication(argc, argv)
{
	isInitialised = false;

	setOrganizationName("IITiS PAN");
	setOrganizationDomain("iitis.pl");
	setApplicationName("dpVision");

	
	sExeDir = applicationDirPath();

	sAppDir = QString(qgetenv("APPDATA")) + QDir::separator() + APPNAME;
	if (!QDir(sAppDir).exists()) QDir().mkdir(sAppDir);

	sTmpDir = QString(qgetenv("TEMP")) + QDir::separator() + APPNAME;
	if (!QDir(sTmpDir).exists()) QDir().mkdir(sTmpDir);

	settings = new QSettings();

	bGlobalPicking = false;
	bPickSnap = false;

	activePlugin = NULL;

	m_lastObjectId = MODEL_ID_OFFSET;
}



Plugin* CMainApplication::getPlugin( unsigned int id )
{
	QMap<unsigned int, Plugin*>::iterator pit = plugins.find(id);

	if ( pit != plugins.end() )
	{
		return pit.value();
	}
	return nullptr;
}

Plugin* CMainApplication::getPlugin(const char* strUUID)
{
	for (Plugin* p : plugins)
	{
		if (p->hasUUID(strUUID))
		{
			return p;
		}
	}
	return nullptr;
}



void CMainApplication::loadPlugin( const QString pluginPath )
{
	Plugin *plugin = PluginManager::Instance().LoadPlugin( pluginPath.toStdWString() );
	CMainWindow* win = AP::mainWinPtr();

	if (win == nullptr)
	{
		return;
	}

	if ( plugin != NULL ) {
		unsigned int newID = PLUGIN_ID_OFFSET;
		while ( plugins.find( newID ) != plugins.end() ) {
			newID++;
		}

		plugin->setID( newID );
		plugins[ newID ] = plugin;
		
		plugin->onLoad();
		if ( ! plugin->isHidden() )
		{
			win->addPluginToListView( newID, QString::fromStdWString(plugin->name()) + " (" + QString::fromStdWString(plugin->getPath()) + ")" );
		}
	}
	else {
		UI::STATUSBAR::printf( "PluginAdd: ERROR" );
	}
}

void CMainApplication::unloadPlugin( const unsigned int id )
{
	QMap<unsigned int,Plugin*>::iterator pit = plugins.find( id );

	if ( pit != plugins.end() )
	{
		pit.value()->onUnload();

		PluginManager::Instance().UnloadPlugin( pit.value()->getPath().c_str() );
		plugins.erase( pit );

		activePlugin = NULL;
	}
}

void CMainApplication::unloadPlugin( const QString pluginPath )
{
	//unsigned int menuId;

	for ( QMap<unsigned int,Plugin*>::iterator pit = plugins.begin(); pit != plugins.end(); pit++ )
    {
		if ( 0 == pit.value()->getPath().compare( pluginPath.toStdWString() ) ) {
			//DeleteMenu( GetSubMenu( pOkno->hMenu, 5 ), (*pit)->GetMenuID(), MF_BYCOMMAND );
			PluginManager::Instance().UnloadPlugin( pluginPath.toStdWString() );
			plugins.erase( pit );
			break;
		}
	}
}

void CMainApplication::UnloadAllPlugins()
{
//	unsigned int menuId;

	for ( QMap<unsigned int, Plugin*>::iterator pit = plugins.begin(); pit != plugins.end(); pit++ )
    {
			//DeleteMenu( GetSubMenu( pOkno->hMenu, 5 ), (*pit)->GetMenuID(), MF_BYCOMMAND );
		PluginManager::Instance().UnloadPlugin( pit.value()->getPath().c_str() );
	}

	plugins.clear();
}


void CMainApplication::LoadAllPlugins()
{
	QString path = sExeDir + "/plugins/";
	QStringList dlls = QDir(path).entryList(QStringList() << "*.dll" << "*.DLL", QDir::Files);

	for ( QString filename : dlls )
	{
		loadPlugin(path + filename);
	}
}


void CMainApplication::RunPlugin( const unsigned int id )
{
	QMap<unsigned int,Plugin*>::iterator pit = plugins.find( id );

	if ( pit != plugins.end() )
	{
		pit.value()->onRun();
	}
}

bool CMainApplication::runPlugin(const char * strUUID)
{
	for (Plugin *p : plugins)
	{
		std::string uuid = p->uuid();
		if (p->hasUUID(strUUID))
		{
			p->onRun();
			return true;
		}
		UI::STATUSBAR::setText(uuid);
	}
	return false;
}


int CMainApplication::getUniqueId()
{
	m_lastObjectId++;
	return m_lastObjectId;
}


void CMainApplication::preExec()
{
	Q_INIT_RESOURCE(dpVision);

	if (AP::mainApp().settings->value("mainwindow/maximized", false).toBool())
	{
		CMainWindow::instance()->showMaximized();
	}
	else
	{
		CMainWindow::instance()->show();
	}

	UI::STATUSBAR::printf("loading plugins...");

	LoadAllPlugins();


	//wczytujê pliki z linii poleceñ
	vListaPlikow = this->arguments();
	vListaPlikow.pop_front(); // pierwsza jest zawsze œcie¿ka do programu
	if (!vListaPlikow.empty())
	{
		for (QString &plik : vListaPlikow)
		{
			if (QFile(plik).exists())
			{
				UI::STATUSBAR::setText(L"loading model: " + plik.toStdWString());
				AP::WORKSPACE::loadModel(plik);
			}
		}

	}
	CWorkspace::instance()->_setCurrentModel(NO_CURRENT_MODEL);

	isInitialised = true;
	
	UI::STATUSBAR::printf("starting tcp server...");

	CMainWindow::instance()->startServer();

	//FreeConsole();
	//AllocConsole();
	//AttachConsole(GetCurrentProcessId());
	//freopen("CON", "w", stdout); 
	//freopen("CON", "w", stderr);
	//freopen("CON", "r", stdin);

	UI::STATUSBAR::printf("ready...");
}


void CMainApplication::postExec()
{
	UnloadAllPlugins();
	CWorkspace::instance()->clear();
}
