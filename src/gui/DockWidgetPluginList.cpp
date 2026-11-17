#include "DockWidgetPluginList.h"

#include "DockWidgetPluginPanel.h"
#include "MainApplication.h"
#include "../core/PluginRuntimeManager.h"

//#include "Plugin.h"

#include <QListWidgetItem>

#include "MainWindow.h"

#include <QMessageBox>

namespace
{
	CMainApplication* mainApplication()
	{
		return static_cast<CMainApplication*>(QApplication::instance());
	}

	CMainWindow* mainWindow()
	{
		return CMainWindow::instance();
	}

	QString pluginsDir()
	{
		if (auto app = mainApplication())
		{
			return app->appExecDir() + "/plugins";
		}
		return QString();
	}
}

DockWidgetPluginList::DockWidgetPluginList(QWidget *parent)	: QDockWidget(parent)
{
	ui.setupUi(this);

	setObjectName("DockWidgetPluginsList");

	//QObject::connect( ui.spinRotX, SIGNAL(valueChanged(double)), this, SLOT(updateModelRotX(double)) );
	ui.dockWidgetContents->layout()->setAlignment( Qt::Alignment::enum_type::AlignTop );
}

DockWidgetPluginList::~DockWidgetPluginList()
{
}


void DockWidgetPluginList::runSelectedPlugin( QListWidgetItem *item )
{
	if (auto app = mainApplication())
	{
		app->RunPlugin(item->data(Qt::UserRole).toUInt());
	}
}

void DockWidgetPluginList::removeSelectedPlugin()
{
	QMessageBox::warning(0, "Unload Plugin", "Function temporarily unavailable..." );
	/*
	QList<QListWidgetItem*> lista = ui.listPlugins->selectedItems();

	if ( lista.size() > 0 )
	{
		int i = lista.at(0)->data(Qt::UserRole).toUInt();

		delete lista.at(0);
		AP::unloadPlugin( i );
		removePluginPanel( i );
	}
	*/
}


#include <QFileDialog>

void DockWidgetPluginList::loadPlugin()
{
#ifdef _WIN32
	QString fileName = QDir::toNativeSeparators(
		QFileDialog::getOpenFileName(
			this,
			tr("Load plugin"),
			pluginsDir(),
			tr("Plugin DLL (*.dll)")) );
#else
	QString fileName = QDir::toNativeSeparators(
		QFileDialog::getOpenFileName(
			this,
			tr("Load plugin"),
			pluginsDir(),
			tr("Plugin DLL (*.so)")) );
#endif
	if (auto app = mainApplication())
	{
		app->loadPlugin(fileName);
	}
}

void DockWidgetPluginList::addPluginToList( int id, QString txt )
{
	QListWidgetItem *i = new QListWidgetItem( txt, ui.listPlugins );
	i->setData(Qt::UserRole, id);
		
	ui.listPlugins->addItem( i );
}


void DockWidgetPluginList::currentItemChanged( QListWidgetItem *curr, QListWidgetItem *prev )
{
	if ( NULL != prev )
	{
		unsigned int prevId = prev->data(Qt::UserRole ).toUInt();
		
		if (auto win = mainWindow())
		{
			win->dockPluginPanel->showPanel(prevId, false);
		}
		if (auto app = mainApplication())
		{
			if (PluginInterface* plugin = app->getPlugin(prevId))
			{
				plugin->onDeactivate();
			}
		}
		PluginRuntimeManager::setActivePlugin(nullptr);
	}

	if ( NULL != curr )
	{
		unsigned int currId = curr->data(Qt::UserRole ).toUInt();
		
		if (auto win = mainWindow())
		{
			win->dockPluginPanel->showPanel( currId, true );
		}
		PluginInterface* plugin = mainApplication() ? mainApplication()->getPlugin(currId) : nullptr;
		PluginRuntimeManager::setActivePlugin(plugin);
		if (plugin != nullptr)
		{
			plugin->onActivate();
		}
	}
}

void DockWidgetPluginList::deactivateCurrentPlugin()
{
	PluginInterface* plugin = PluginRuntimeManager::activePlugin();
	if (plugin == nullptr)
	{
		return;
	}

	unsigned int id = plugin->id();

	if (auto win = mainWindow())
	{
		win->dockPluginPanel->showPanel(id, false);
	}
	if (auto app = mainApplication())
	{
		if (PluginInterface* registeredPlugin = app->getPlugin(id))
		{
			registeredPlugin->onDeactivate();
		}
	}
	PluginRuntimeManager::setActivePlugin(nullptr);
}
