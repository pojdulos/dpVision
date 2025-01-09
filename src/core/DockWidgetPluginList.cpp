#include "DockWidgetPluginList.h"
#include "UI.h"
#include "AP.h"

#include "DockWidgetPluginPanel.h"
#include "MainApplication.h"
#include "Plugin.h"

#include <QListWidgetItem>

#include "MainWindow.h"

DockWidgetPluginList::DockWidgetPluginList(QWidget *parent)	: QDockWidget(parent)
{
	ui.setupUi(this);

	//QObject::connect( ui.spinRotX, SIGNAL(valueChanged(double)), this, SLOT(updateModelRotX(double)) );
	ui.dockWidgetContents->layout()->setAlignment( Qt::Alignment::enum_type::AlignTop );
}

DockWidgetPluginList::~DockWidgetPluginList()
{
}


void DockWidgetPluginList::runSelectedPlugin( QListWidgetItem *item )
{
	AP::mainApp().RunPlugin(item->data(Qt::UserRole).toUInt());
	//foreach ( QListWidgetItem *it, ui.listPlugins->selectedItems() )
	//	AP::mainApp().RunPlugin( it->data(Qt::UserRole).toUInt() );
}

void DockWidgetPluginList::removeSelectedPlugin()
{
	UI::MESSAGEBOX::warning(L"Function temporarily unavailable...", L"Unload Plugin");
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
	QString fileName = QDir::toNativeSeparators(
		QFileDialog::getOpenFileName(
			this,
			tr("Load plugin"),
			QString( AP::getExeFilePath() + "/plugins" ),
			tr("Plugin DLL (*.dll)")) );

	AP::PLUGIN::loadPlugin( fileName );
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
		
		AP::mainWin().dockPluginPanel->showPanel(prevId, false);
		AP::PLUGIN::getPlugin(prevId)->onDeactivate();
		AP::mainApp().activePlugin = NULL;
	}

	if ( NULL != curr )
	{
		unsigned int currId = curr->data(Qt::UserRole ).toUInt();
		
		AP::mainWin().dockPluginPanel->showPanel( currId, true );
		AP::mainApp().activePlugin = AP::PLUGIN::getPlugin(currId);
		AP::mainApp().activePlugin->onActivate();
	}
}

void DockWidgetPluginList::deactivateCurrentPlugin()
{
	unsigned int id = AP::mainApp().activePlugin->id();

	AP::mainWin().dockPluginPanel->showPanel(id, false);
	AP::PLUGIN::getPlugin(id)->onDeactivate();
	AP::mainApp().activePlugin = NULL;
}