#include "MainApplication.h"

#include "AP.h"

#include "MainWindow.h"

DPVISION_EXPORT CMainApplication * CMainApplication::theApp = nullptr;

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



PluginInterface* CMainApplication::getPlugin( unsigned int id )
{
	QMap<unsigned int, PluginInterface*>::iterator pit = plugins.find(id);

	if ( pit != plugins.end() )
	{
		return pit.value();
	}
	return nullptr;
}

PluginInterface* CMainApplication::getPlugin(const char* strUUID)
{
	for (PluginInterface* p : plugins)
	{
		if (p->hasUUID(strUUID))
		{
			return p;
		}
	}
	return nullptr;
}

#include <QPluginLoader>

bool CMainApplication::loadQtPlugin(const QString &pluginPath)
{
	QPluginLoader loader(pluginPath); // lub .so lub .dylib w zależności od systemu
	
	QObject* pluginObject = loader.instance();

	if (pluginObject)
	{
		PluginInterface* plugin = qobject_cast<PluginInterface*>(pluginObject);
		if (plugin)
		{
			unsigned int newID = PLUGIN_ID_OFFSET;
			while (plugins.find(newID) != plugins.end()) {
				newID++;
			}
			plugin->setID(newID);
			plugins[newID] = plugin;

			plugin->setPath(pluginPath);


			QJsonObject metadata = loader.metaData().value("MetaData").toObject();

			//QString version = metadata.value("version").toString();
			//QString description = metadata.value("description").toString();
			QString name = metadata.value("name").toString();
			if (name != "") plugin->setName(name);

			if (!plugin->isHidden())
			{
				CMainWindow* win = AP::mainWinPtr();

				if (win != nullptr)
				{
					win->addPluginToListView(newID, plugin->name() + " (" + plugin->path() + ")");
				}
			}

			plugin->onLoad();

			return true;
		}
	}
	return false;
}

bool CMainApplication::loadWinPlugin( const QString &pluginPath )
{
	PluginInterface *plugin = PluginManager::Instance().LoadPlugin( pluginPath );
	if (plugin)
	{
		unsigned int newID = PLUGIN_ID_OFFSET;
		while ( plugins.find( newID ) != plugins.end() ) {
			newID++;
		}

		plugin->setID( newID );
		plugins[ newID ] = plugin;
		
		if ( ! plugin->isHidden() )
		{
			CMainWindow* win = AP::mainWinPtr();

			if (win != nullptr)
			{
				win->addPluginToListView( newID, plugin->name() + " (" + plugin->path() + ")" );
			}
		}

		plugin->onLoad();

		return true;
	}
	return false;
}

bool CMainApplication::loadPlugin( const QString &pluginPath )
{
	qInfo() << "Trying to load plugin: " << pluginPath;
	if (loadQtPlugin(pluginPath))
	{
		qInfo() << "-- has been identified as a plugin compatible with QPluginLoader";
		return true;
	}
	if (loadWinPlugin(pluginPath))
	{
		qInfo() << "-- has been identified as an old-style dpVision plugin";
		return true;
	}
	else
	{
		qInfo() << "-- probably this is not plugin";
		return false;
	}
}

void CMainApplication::unloadPlugin( const unsigned int id )
{
	QMap<unsigned int,PluginInterface*>::iterator pit = plugins.find( id );

	if ( pit != plugins.end() )
	{
		pit.value()->onUnload();

		if (!PluginManager::Instance().UnloadPlugin(pit.value()))
		{
			delete pit.value();
		}
		plugins.erase( pit );

		activePlugin = NULL;
	}
}

void CMainApplication::unloadPlugin( const QString pluginPath )
{
	//unsigned int menuId;

	for ( QMap<unsigned int,PluginInterface*>::iterator pit = plugins.begin(); pit != plugins.end(); pit++ )
    {
		if ( 0 == pit.value()->path().compare( pluginPath ) ) {
			//DeleteMenu( GetSubMenu( pOkno->hMenu, 5 ), (*pit)->GetMenuID(), MF_BYCOMMAND );
			if (!PluginManager::Instance().UnloadPlugin(pluginPath))
			{
				delete pit.value();
			}
			plugins.erase( pit );
			break;
		}
	}
}

void CMainApplication::UnloadAllPlugins()
{
//	unsigned int menuId;

	for ( QMap<unsigned int, PluginInterface*>::iterator pit = plugins.begin(); pit != plugins.end(); pit++ )
    {
			//DeleteMenu( GetSubMenu( pOkno->hMenu, 5 ), (*pit)->GetMenuID(), MF_BYCOMMAND );
		if (!PluginManager::Instance().UnloadPlugin(pit.value()))
		{
			delete pit.value();
		}
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
	QMap<unsigned int,PluginInterface*>::iterator pit = plugins.find( id );

	if ( pit != plugins.end() )
	{
		pit.value()->onRun();
	}
}

bool CMainApplication::runPlugin(const char * strUUID)
{
	for (PluginInterface *p : plugins)
	{
		//std::string uuid = p->uuid();
		if (p->hasUUID(strUUID))
		{
			p->onRun();
			return true;
		}
		UI::STATUSBAR::setText(p->uuid());
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


	//wczytuję pliki z linii poleceń
	vListaPlikow = this->arguments();
	vListaPlikow.pop_front(); // pierwsza jest zawsze ścieżka do programu
	if (!vListaPlikow.empty())
	{
		for (QString &plik : vListaPlikow)
		{
			if (QFile(plik).exists())
			{
				UI::STATUSBAR::setText("loading model: " + plik);
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
