#include "DockWidgetPluginPanel.h"

#include "../api/UI.h"
#include "../api/AP.h"

#include "Plugin.h"

#include <QtWidgets>

DockWidgetPluginPanel::DockWidgetPluginPanel(QWidget *parent) : QDockWidget(parent)
{
	setObjectName("DockWidgetPluginPanel");
	setWindowTitle("Plugin Panel");

	scrollArea = new QScrollArea(this);
	scrollArea->setWidgetResizable(true);

	// Tworzymy wewn�trzny widget
	innerWidget = new QWidget();
	QVBoxLayout* layout = new QVBoxLayout(innerWidget);

	innerWidget->setLayout(layout);
	scrollArea->setWidget(innerWidget);

	setWidget(scrollArea);
}

DockWidgetPluginPanel::~DockWidgetPluginPanel()
{
}

QGroupBox* DockWidgetPluginPanel::getPanel(unsigned int pluginId)
{
	if (pluginUI.find(pluginId) != pluginUI.end()) return pluginUI[pluginId];
	return nullptr;
}

void DockWidgetPluginPanel::showPanel(unsigned int pluginId, bool b)
{
	if (pluginUI.find(pluginId) != pluginUI.end()) pluginUI[pluginId]->setVisible(b);
}

void DockWidgetPluginPanel::userButton()
{
	QPushButton *s = (QPushButton *)sender();
	QString n = s->objectName();

	QStringList l = n.split("@");

	unsigned int id = l[0].toUInt();

	PluginInterface *p = AP::PLUGIN::getPlugin(id);
	if (p != nullptr)
	{
		p->onButton(l[1]);
	}
}

void DockWidgetPluginPanel::addPluginPanel(unsigned int pluginId, QString label)
{
	QGroupBox *userGroupBox = new QGroupBox();
	userGroupBox->setObjectName(QString::fromStdWString(std::to_wstring((long long)pluginId)));

	if (label.compare("") != 0)
	{
		userGroupBox->setTitle( label );
	}

	QGridLayout *userLayout = new QGridLayout(userGroupBox);
	userLayout->setObjectName(QStringLiteral("userLayout"));
	userLayout->setAlignment(Qt::Alignment::enum_type::AlignTop);
	userGroupBox->setLayout(userLayout);

	QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(userGroupBox->sizePolicy().hasHeightForWidth());
	userGroupBox->setSizePolicy(sizePolicy);

	userGroupBox->setVisible(false);

	pluginUI[pluginId] = userGroupBox;
	
	//ui.dockWidgetContents->layout()->addWidget(userGroupBox);
	innerWidget->layout()->addWidget(userGroupBox);
}

void DockWidgetPluginPanel::removePluginPanel(unsigned int pluginId)
{
	if (pluginUI.find(pluginId) != pluginUI.end())
	{
		delete pluginUI[pluginId];
	}
}

void DockWidgetPluginPanel::removeWidget(unsigned int pluginId, QString name)
{
	QWidget *w = pluginUI[pluginId]->findChild<QWidget*>( QString::number(pluginId) + "@" + name, Qt::FindChildrenRecursively );

	if (w != NULL) delete w;
}

void DockWidgetPluginPanel::addEditBox(unsigned int pluginId, QString name, QString label, QString value, int row, int col, int rspan, int cspan)
{
	QLineEdit *testBox;
	testBox = new QLineEdit( value, pluginUI[pluginId]);

	testBox->setToolTip( label );
	testBox->setObjectName( QString::number(pluginId) + "@" + name );

	if ((rspan == 0) && (cspan == 0))
	{
		((QGridLayout*)pluginUI[pluginId]->layout())->addWidget(testBox, row, col);
	}
	else
	{
		((QGridLayout*)pluginUI[pluginId]->layout())->addWidget(testBox, row, col, rspan, cspan, Qt::AlignCenter);
	}
}

void DockWidgetPluginPanel::addLabel(unsigned int pluginId, QString name, QString text, int row, int col, int rspan, int cspan)
{
	QLabel *qlabel = new QLabel( text, pluginUI[pluginId]);

	qlabel->setToolTip( text );
	qlabel->setObjectName(QString::number(pluginId) + "@" + name );

	if ((rspan == 0) && (cspan == 0))
	{
		((QGridLayout*)pluginUI[pluginId]->layout())->addWidget(qlabel, row, col);
	}
	else
	{
		((QGridLayout*)pluginUI[pluginId]->layout())->addWidget(qlabel, row, col, rspan, cspan, Qt::AlignLeft);
	}
}

void DockWidgetPluginPanel::setLabel(unsigned int pluginId, QString name, QString text)
{
	QLabel *qlabel = (QLabel *)pluginUI[pluginId]->findChild<QWidget*>( QString::number(pluginId) + "@" + name, Qt::FindChildrenRecursively );

	if (qlabel != NULL)
	{
		qlabel->setText(text);
		qlabel->setToolTip(text);
		qlabel->adjustSize();
	}
}

void DockWidgetPluginPanel::userValueChanged(int val)
{
	QSlider *s = (QSlider *)sender();
	QString n = s->objectName();

	QStringList l = n.split("@");

	unsigned int id = l[0].toUInt();

	PluginInterface *p = AP::PLUGIN::getPlugin(id);
	if (NULL != p)
	{
		p->onButton(l[1]);
	}

}

void DockWidgetPluginPanel::comboTextChanged(const QString & text)
{
	QComboBox *s = (QComboBox *)sender();
	QString n = s->objectName();

	QStringList l = n.split("@");

	unsigned int id = l[0].toUInt();

	PluginInterface *p = AP::PLUGIN::getPlugin(id);
	if (NULL != p)
	{
		p->onButton(l[1]);
	}
}


QString DockWidgetPluginPanel::getEditBoxValue(unsigned int pluginId, QString name)
{
	QLineEdit *ed = (QLineEdit *)pluginUI[pluginId]->findChild<QWidget*>(QString::number(pluginId) + "@" + name, Qt::FindChildrenRecursively);

	if (ed != NULL)
	{
		return ed->text();
	}

	return QString("");
}

void DockWidgetPluginPanel::setEditBoxValue(unsigned int pluginId, QString name, QString value)
{
	QLineEdit *ed = (QLineEdit *)pluginUI[pluginId]->findChild<QWidget*>(QString::number(pluginId) + "@" + name, Qt::FindChildrenRecursively);

	if (ed != NULL)
	{
		ed->setText(value);
		ed->repaint();
	}
}

int DockWidgetPluginPanel::getSliderValue(unsigned int pluginId, QString name)
{
	QSlider *s = (QSlider *)pluginUI[pluginId]->findChild<QWidget*>(QString::number(pluginId) + "@" + name, Qt::FindChildrenRecursively);

	if (s != NULL)
	{
		return s->value();
	}

	return 0;
}

int DockWidgetPluginPanel::setSliderValue(unsigned int pluginId, QString name, int value)
{
	QSlider *s = (QSlider *)pluginUI[pluginId]->findChild<QWidget*>(QString::number(pluginId) + "@" + name, Qt::FindChildrenRecursively);

	if (s != NULL)
	{
		if (value < s->minimum()) value = s->minimum();
		if (value > s->maximum()) value = s->maximum();
		
		s->blockSignals(true);
		s->setValue(value);
		s->blockSignals(false);

		s->repaint();
		return value;
	}

	return 0;
}

void DockWidgetPluginPanel::setSliderRange(unsigned int pluginId, QString name, int min, int max)
{
	QSlider *s = (QSlider *)pluginUI[pluginId]->findChild<QWidget*>(QString::number(pluginId) + "@" + name, Qt::FindChildrenRecursively);

	if (s != NULL)
	{
		int v = s->value();
		if ( v < min ) min = v;
		if ( v > max ) max = v;

		s->blockSignals(true);
		s->setRange(min, max);
		s->blockSignals(false);
	}
}

QString DockWidgetPluginPanel::getComboBoxCurrentItemText(unsigned int pluginId, QString name)
{
	QComboBox *ed = (QComboBox *)pluginUI[pluginId]->findChild<QWidget*>(QString::number(pluginId) + "@" + name, Qt::FindChildrenRecursively);

	if (ed != NULL)
	{
		return ed->currentText();
	}

	return QString("");
}

void DockWidgetPluginPanel::setComboBoxItems(unsigned int pluginId, QString name, QStringList items)
{
	QComboBox *ed = (QComboBox *)pluginUI[pluginId]->findChild<QWidget*>(QString::number(pluginId) + "@" + name, Qt::FindChildrenRecursively);

	if (ed != NULL)
	{ 
		ed->blockSignals(true);
		ed->clear();
		ed->addItems(items);
		ed->blockSignals(false);
	}
}

QPushButton* DockWidgetPluginPanel::addButton(unsigned int pluginId, QString label, QObject* receiver, const char* slot, int row, int col, int rspan, int cspan)
{
	QPushButton* testButton;
	testButton = new QPushButton;

	testButton->setText(label);
	testButton->setStyleSheet("color: blue; background-color: yellow");

	if ((rspan == 0) && (cspan == 0))
	{
		((QGridLayout*)pluginUI[pluginId]->layout())->addWidget(testButton, row, col);
	}
	else
	{
		((QGridLayout*)pluginUI[pluginId]->layout())->addWidget(testButton, row, col, rspan, cspan, Qt::AlignCenter);
	}

	QObject::connect(testButton, SIGNAL(clicked()), receiver, slot);

	return testButton;
}

QPushButton* DockWidgetPluginPanel::addButton(unsigned int pluginId, QString name, QString label, int row, int col, int rspan, int cspan)
{
	QPushButton *testButton;
	testButton = new QPushButton(pluginUI[pluginId]);

	testButton->setObjectName(QString::number(pluginId) + "@" + name);
	testButton->setText(label);

	//testButton->setStyleSheet("color: blue; background-color: yellow");

	if ((rspan == 0) && (cspan == 0))
	{
		((QGridLayout*)pluginUI[pluginId]->layout())->addWidget(testButton, row, col);
	}
	else
	{
		((QGridLayout*)pluginUI[pluginId]->layout())->addWidget(testButton, row, col, rspan, cspan, Qt::AlignCenter);
	}

	QObject::connect(testButton, SIGNAL(clicked()), this, SLOT(userButton()));

	return testButton;
}

void DockWidgetPluginPanel::addSlider(unsigned int pluginId, QString name, int min, int max, int row, int col, int rspan, int cspan)
{
	QSlider *slider;
	slider = new QSlider( Qt::Orientation::Horizontal, pluginUI[pluginId] );

	slider->setObjectName(QString::number(pluginId) + "@" + name);
	slider->setRange(min, max);

	//testButton->setStyleSheet("color: blue; background-color: yellow");

	if ((rspan == 0) && (cspan == 0))
	{
		((QGridLayout*)pluginUI[pluginId]->layout())->addWidget(slider, row, col);
	}
	else
	{
		((QGridLayout*)pluginUI[pluginId]->layout())->addWidget(slider, row, col, rspan, cspan);
	}

	QObject::connect(slider, SIGNAL(valueChanged(int)), this, SLOT(userValueChanged(int)));
}

void DockWidgetPluginPanel::addComboBox(unsigned int pluginId, QString name, int row, int col, int rspan, int cspan)
{
	QComboBox *combo;
	combo = new QComboBox(pluginUI[pluginId]);

	combo->setObjectName(QString::number(pluginId) + "@" + name);

	//testButton->setStyleSheet("color: blue; background-color: yellow");

	if ((rspan == 0) && (cspan == 0))
	{
		((QGridLayout*)pluginUI[pluginId]->layout())->addWidget(combo, row, col);
	}
	else
	{
		((QGridLayout*)pluginUI[pluginId]->layout())->addWidget(combo, row, col, rspan, cspan);
	}

	QObject::connect(combo, SIGNAL(currentTextChanged(QString)), this, SLOT(comboTextChanged(QString)));
}

void DockWidgetPluginPanel::setButtonText(unsigned int pluginId, QString name, QString value)
{
	QPushButton *b = (QPushButton *)pluginUI[pluginId]->findChild<QWidget*>(QString::number(pluginId) + "@" + name, Qt::FindChildrenRecursively);

	if (b != NULL)
	{
		b->setText( value );
		b->repaint();
	}
}
