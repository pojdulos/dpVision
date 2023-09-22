#ifndef DOCKWIDGETPLUGINPANEL_H
#define DOCKWIDGETPLUGINPANEL_H

#include <QtWidgets/QDockWidget>
#include "ui_dockWidgetPluginPanel.h"

class QGroupBox;
class QPushButton;

class DockWidgetPluginPanel : public QDockWidget
{
	Q_OBJECT

public:
	explicit DockWidgetPluginPanel(QWidget *parent = 0);
	~DockWidgetPluginPanel();

	QGroupBox* getPanel(unsigned int pluginId);

	void showPanel(unsigned int pluginId, bool b = true );

	void addPluginPanel(unsigned int pluginId, QString label);
	void removePluginPanel(unsigned int pluginId);

	void removeWidget(unsigned int pluginId, QString name);

	QPushButton* addButton(unsigned int pluginId, QString label, QObject* receiver, const char* slot, int row, int col, int rspan = 0, int cspan = 0);
	QPushButton* addButton(unsigned int pluginId, QString buttonName, QString label, int row, int col, int rspan = 0, int cspan = 0);

	void addEditBox(unsigned int pluginId, QString name, QString label, QString value, int row, int col, int rspan = 0, int cspan = 0);
	void addSlider(unsigned int pluginId, QString buttonName, int min, int max, int row, int col, int rspan = 0, int cspan = 0);
	void addComboBox(unsigned int pluginId, QString name, int row, int col, int rspan = 0, int cspan = 0);

	void setButtonText(unsigned int pluginId, QString name, QString value);

	QString getEditBoxValue(unsigned int pluginId, QString name);
	void setEditBoxValue(unsigned int pluginId, QString name, QString value);

	int getSliderValue(unsigned int pluginId, QString name);
	int setSliderValue(unsigned int pluginId, QString name, int value );
	void setSliderRange(unsigned int pluginId, QString name, int min, int max );

	QString getComboBoxCurrentItemText(unsigned int pluginId, QString name);
	void setComboBoxItems(unsigned int pluginId, QString name, QVector<QString> items);
	

	void addLabel(unsigned int pluginId, QString name, QString label, int row, int col, int rspan = 0, int cspan = 0);
	void setLabel(unsigned int pluginId, QString name, QString text);

public slots:
	void userButton();
	void userValueChanged(int);
	void comboTextChanged(const QString &text);

private:
	Ui::DockWidgetPluginPanel ui;

	std::map<unsigned int, QGroupBox*> pluginUI;
};

#endif // DOCKWIDGETPLUGINPANEL_H
