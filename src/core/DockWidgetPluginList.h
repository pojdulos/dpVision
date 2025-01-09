#ifndef DOCKWIDGETPLUGINLIST_H
#define DOCKWIDGETPLUGINLIST_H
#include "dll_global.h"

#include <QtWidgets/QGroupBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>

#include <QtWidgets/QDockWidget>
#include "ui_dockWidgetPluginList.h"

class DPVISION_EXPORT DockWidgetPluginList : public QDockWidget
{
	Q_OBJECT

public:
	explicit DockWidgetPluginList(QWidget *parent = 0);
	~DockWidgetPluginList();

	void addPluginToList( int id, QString txt );

	void deactivateCurrentPlugin();

public slots:
	void currentItemChanged( QListWidgetItem *curr, QListWidgetItem *prev );
	void runSelectedPlugin( QListWidgetItem* );
	void removeSelectedPlugin();
	void loadPlugin();

private:
	Ui::DockWidgetPluginList ui;
};

#endif // DOCKWIDGETPLUGINLIST_H
