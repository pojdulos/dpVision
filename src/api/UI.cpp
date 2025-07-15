#include "UI.h"

#include "MainWindow.h"

//#include <Windows.h> // definiuje GetTickCount()

#include "DockWidgetWorkspace.h"
#include "DockWidgetProperties.h"
#include "DockWidgetModel.h"
#include "DockWidgetViewer.h"
#include "DockWidgetLights.h"
#include "DockWidgetPluginList.h"
#include "DockWidgetPluginPanel.h"
#include "DockWidgetHistogram.h"
#include "DockWidgetImageViewer.h"

#include "ProgressIndicator.h"

#include <QElapsedTimer>
#include <QtWidgets>

#include "GLViewer.h"

#include "AP.h"


void UI::adjustGroupBoxHeight(QGroupBox* groupBox, bool checked)
{
	// Uzyskaj wysokość checkboxa i tytułu
	QCheckBox* ch = groupBox->findChild<QCheckBox*>();
	int checkBoxHeight = (ch) ? groupBox->findChild<QCheckBox*>()->sizeHint().height() : 0;
	int titleHeight = groupBox->fontMetrics().height();  // Wysokość tytułu na podstawie czcionki

	int heightForChecked = groupBox->sizeHint().height();  // Wysokość dla zaznaczonego stanu
	//int heightForUnchecked = checkBoxHeight + titleHeight + 10;  // Wysokość dla odznaczonego stanu (z dodatkowym odstępem)

	int heightForUnchecked = std::max(checkBoxHeight, titleHeight);

	if (checked) {
		groupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		groupBox->setMinimumHeight(0);
		groupBox->setMaximumHeight(QWIDGETSIZE_MAX);  // Brak limitu wysokości
		groupBox->adjustSize();
	}
	else {
		groupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		groupBox->setMinimumHeight(heightForUnchecked);  // Ustaw wysokość na widoczność checkboxa i tytułu
		groupBox->setMaximumHeight(heightForUnchecked);  // Ustaw wysokość na widoczność checkboxa i tytułu
	}
}

// bool UI::timeElapsed(int mst)
// {
// 	static DWORD t0001 = 0;

// 	DWORD t1 = GetTickCount();
// 	if ( ( t0001 + mst ) < t1 )
// 	{
// 		t0001 = t1;
// 		AP::processEvents();
// 		return true;
// 	}
// 	return false;
// }


bool UI::timeElapsed(int mst)
{
    static QElapsedTimer timer;
    if (!timer.isValid())
        timer.start();

    if (timer.elapsed() >= mst)
    {
        timer.restart();
        AP::processEvents();
        return true;
    }
    return false;
}


#include "MainWindow.h"

void UI::show()
{
	if (auto win = CMainWindow::instance())
	{
		win->show();
	}
}

void UI::updateView(bool repaintAll, bool buffered)
{
	if (auto win = CMainWindow::instance())
	{
		win->updateView(repaintAll, buffered);
	}
}

void UI::updateAllViews(bool buffered)
{
	if (auto win = CMainWindow::instance())
	{
		win->updateView(true, buffered);
	}
}

void UI::updateCurrentView(bool buffered)
{
	if (auto win = CMainWindow::instance())
	{
		win->updateView(false, buffered);
	}
}

void UI::changeMenuAfterSelect()
{
	if (auto win = CMainWindow::instance())
	{
		win->changeMenuAfterSelect();
	}
}


// WORKSPACE, MODEL, VIEWER PANELS

void UI::updateSelection(int id)
{
	UI::DOCK::WORKSPACE::selectItem(id);

	UI::DOCK::PROPERTIES::selectionChanged(id);

	UI::changeMenuAfterSelect();
	UI::updateAllViews();
}


void UI::DOCK::updateAll(int id)
{
	UI::DOCK::WORKSPACE::update();

	UI::DOCK::PROPERTIES::selectionChanged( id );
}


void UI::DOCK::PROPERTIES::show(bool b)
{
	//CMainWindow &q = AP::mainWin();
	//q.dockModel->setVisible(b);
	//if (b) q.dockModel->raise();
}

void UI::DOCK::PROPERTIES::selectionChanged( int id )
{
	if (auto win = CMainWindow::instance())
		if (win->dockProperties)
		{
			win->dockProperties->raise();
			win->dockProperties->selectionChanged(id);
			win->dockProperties->update();
		}
}

void UI::DOCK::PROPERTIES::updateProperties()
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockProperties != nullptr)
	{
		win->dockProperties->updateProperties();
		win->dockProperties->update();
	}
}


DockWidgetWorkspace* UI::DOCK::WORKSPACE::instance()
{
	if (auto win = CMainWindow::instance())
		return win->dockWorkspace;
	return nullptr;
}

void UI::DOCK::WORKSPACE::show(bool b)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockWorkspace != nullptr)
	{
		win->dockWorkspace->setVisible(b);
		if (b)
		{
			win->dockWorkspace->raise();
		}
	}
}

void UI::DOCK::WORKSPACE::rebuildTree()
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockWorkspace != nullptr)
	{
		win->dockWorkspace->rebuildTree();
	}
}

void UI::DOCK::WORKSPACE::update()
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockWorkspace != nullptr)
	{
		win->dockWorkspace->rebuildTree();
	}
}

void UI::DOCK::WORKSPACE::addItem(int id, int parentId)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockWorkspace != nullptr)
	{
		win->dockWorkspace->addItem(id, parentId);
	}
}

void UI::DOCK::WORKSPACE::addItem(std::shared_ptr<CBaseObject> obj)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockWorkspace != nullptr)
	{
		win->dockWorkspace->addItem(obj);
	}
}

void UI::DOCK::WORKSPACE::removeItem(int id)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockWorkspace != nullptr)
	{
		win->dockWorkspace->removeItem(id);
	}
}

void UI::DOCK::WORKSPACE::selectItem( int id)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockWorkspace != nullptr)
	{
		win->dockWorkspace->selectItem(id);
	}
}

QVector<std::shared_ptr<CBaseObject>> UI::DOCK::WORKSPACE::getSelectedObjects()
{
	CMainWindow* win = AP::mainWinPtr();

	if (win && win->dockWorkspace) return win->dockWorkspace->getSelectedObjects();

	return QVector<std::shared_ptr<CBaseObject>>();
}

std::shared_ptr<CBaseObject> UI::DOCK::WORKSPACE::getCurrentItemObj()
{
	CMainWindow* win = AP::mainWinPtr();

	if (win && win->dockWorkspace) return win->dockWorkspace->getCurrentItemObj();

	return nullptr;
}


void UI::DOCK::WORKSPACE::setItemCheckedById(int id, bool b)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockWorkspace != nullptr)
	{
		win->dockWorkspace->setItemCheckedById(id, b);
	}
}

void UI::DOCK::WORKSPACE::setItemVisibleById(int id, bool b)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockWorkspace != nullptr)
	{
		win->dockWorkspace->setItemVisibleById(id, b);
	}
}

void UI::DOCK::WORKSPACE::setItemKidsVisibleById(int id, bool b)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockWorkspace != nullptr)
	{
		win->dockWorkspace->setItemKidsVisibleById(id, b);
	}
}


void UI::DOCK::WORKSPACE::setItemLockedById(int id, bool b)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockWorkspace != nullptr)
	{
		win->dockWorkspace->setItemLockedById(id, b);
	}
}

void UI::DOCK::WORKSPACE::setItemLabelById(int id, std::string s)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockWorkspace != nullptr)
	{
		win->dockWorkspace->setItemLabelById(id, QString::fromStdString(s));
	}
}

void UI::DOCK::WORKSPACE::setItemLabelById(int id, std::wstring s)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockWorkspace != nullptr)
	{
		win->dockWorkspace->setItemLabelById(id, QString::fromStdWString(s));
	}
}


void UI::DOCK::HISTOGRAM::show(bool b)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockHisto != nullptr)
	{
		win->dockHisto->setVisible(b);
		if (b)
		{
			win->dockHisto->raise();
		}
	}
}

//void UI::DOCK::HISTOGRAM::processData( std::vector<double> data, double min, double max )
//{
//	CMainWindow &q = AP::mainWin();
//	q.dockHisto->processData(data, min, max);
//}

void UI::DOCK::HISTOGRAM::setHistogram(CHistogram *histogram)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockHisto != nullptr)
	{
		win->dockHisto->setHistogram(histogram);
	}
}

CHistogram * UI::DOCK::HISTOGRAM::getHistogram()
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockHisto != nullptr)
	{
		return win->dockHisto->getHistogram();
	}
	return nullptr;
}

void UI::DOCK::HISTOGRAM::repaint()
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockHisto != nullptr)
	{
		win->dockHisto->update();
	}
}

//#include "childwindow.h"

GLViewer * UI::CAMERA::currentViewer()
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr)
	{
		return win->currentViewer();
	}

	return nullptr;
}

void UI::CAMERA::screenshot(QString path, void *v)
{
	if (v != nullptr)
	{
		((GLViewer*)v)->screenshot(path);
	}
	else
	{
		CMainWindow* win = AP::mainWinPtr();

		if (win != nullptr)
		{
			GLViewer* view = win->currentViewer();

			if (view != nullptr)
			{
				view->screenshot(path);
			}
		}
	}
}

void UI::CAMERA::move( float mx, float my, float mz )
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr)
	{
		QMdiArea* m = win->ui.mdiArea;
		QList<QMdiSubWindow*> l = m->subWindowList();

		if (!l.empty())
		{
			GLViewer* v = (GLViewer*)l[0]->widget();

			CVector3f w(mx, -my, mz);

			v->transform().translate(w);

			v->update();// GL();
		}
	}
}

void UI::CAMERA::rotate( float ax, float ay, float az )
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr)
	{
		QMdiArea* m = win->ui.mdiArea;
		QList<QMdiSubWindow*> l = m->subWindowList();

		if (!l.empty())
		{
			GLViewer* v = (GLViewer*)l[0]->widget();

			CVector3f w(ax, ay, az);

			//w += v->getRotation();

			//v->setRotation( w );

			v->update();// GL();
		}
	}
}

void UI::CAMERA::setFloating( bool f )
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr)
	{
		QMdiArea* m = win->ui.mdiArea;
		QList<QMdiSubWindow*> l = m->subWindowList();

		if (!l.empty())
		{
			GLViewer* v = (GLViewer*)l[0]->widget();

			if (v != nullptr)
			{
				v->setCameraFloating(f);
			}
		}
	}
}

bool UI::CAMERA::convertWinToWorld(CPoint3d winCoords, CPoint3d & worldCoords)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr)
	{
		GLViewer* view = win->currentViewer();

		if (view != nullptr)
		{
			return view->convertWinToWorld(winCoords, worldCoords);
		}
	}
	return false;
}

bool UI::CAMERA::convertWorldToWin(CPoint3d worldCoords, CPoint3d & winCoords)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr)
	{
		GLViewer* view = win->currentViewer();

		if (view != nullptr)
		{
			return view->convertWorldToWin(worldCoords, winCoords);
		}
	}
	return false;
}

bool UI::CAMERA::convertCoords(double winX, double winY, CPoint3d& pkt0, CPoint3d& pkt1)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr)
	{
		GLViewer* view = win->currentViewer();

		if (view != nullptr)
		{
			return view->convertCoords(winX, winY, pkt0, pkt1);
		}
	}
	return false;
}

CPoint3d UI::CAMERA::camPos()
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr)
	{
		GLViewer* view = win->currentViewer();

		if (view != nullptr)
		{
			return view->camPos();
		}
	}
	return CPoint3d(0,0,0);
}

#include "Transform.h"

CTransform* UI::CAMERA::transform()
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr)
	{
		GLViewer* view = win->currentViewer();

		if (view != nullptr)
		{
			return &view->transform();
		}
	}
	return nullptr;
}


void UI::CAMERA::setView(int dir, CModel3D* obj)
{
	AP::mainWin().actionLookDir(dir, obj);
}


// PLUGINPANEL
//#include <QtWidgets/QGroupBox>

DockWidgetPluginPanel* UI::PLUGINPANEL::mainPanel()
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockPluginPanel != nullptr)
	{
		return win->dockPluginPanel;
	}
	return nullptr;
}

QWidget* UI::PLUGINPANEL::instance(unsigned int pluginId)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockPluginPanel != nullptr)
	{
		return win->dockPluginPanel->getPanel(pluginId);
	}
	return nullptr;
}

void UI::PLUGINPANEL::create(unsigned int pluginId, const QString &label)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockPluginPanel != nullptr)
	{
		win->dockPluginPanel->addPluginPanel(pluginId, label);
	}
}

void UI::PLUGINPANEL::clear(unsigned int pluginId)
{
	while (QWidget* w = instance(pluginId)->findChild<QWidget*>())
		delete w;
}

void UI::PLUGINPANEL::setEnabled(unsigned int pluginId, bool b)
{
	instance(pluginId)->setEnabled(b);
}

void UI::PLUGINPANEL::removeWidget(unsigned int pluginId, const QString &name)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockPluginPanel != nullptr)
	{
		win->dockPluginPanel->removeWidget(pluginId, name);
	}
}

// PLUGINPANEL - PUSH BUTTON
QPushButton* UI::PLUGINPANEL::addButton(unsigned int pluginId, QString label, QObject* receiver, const char* slot, int row, int col, int rspan, int cspan)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockPluginPanel != nullptr)
	{
		return win->dockPluginPanel->addButton(pluginId, label, receiver, slot, row, col, rspan, cspan);
	}

	return nullptr;
}

QPushButton* UI::PLUGINPANEL::addButton(unsigned int pluginId, std::string buttonName, std::string label, int row, int col, int rspan, int cspan)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockPluginPanel != nullptr)
	{
		return win->dockPluginPanel->addButton(pluginId, QString::fromStdString(buttonName), QString::fromUtf8(label.c_str()), row, col, rspan, cspan);
	}
	return nullptr;
}

QPushButton* UI::PLUGINPANEL::addButton( unsigned int pluginId, std::wstring buttonName, std::wstring label, int row, int col, int rspan, int cspan )
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockPluginPanel != nullptr)
	{
		return win->dockPluginPanel->addButton(pluginId, QString::fromStdWString(buttonName), QString::fromStdWString(label), row, col, rspan, cspan);
	}
	return nullptr;
}

void UI::PLUGINPANEL::setButtonText(unsigned int pluginId, const QString &name, const QString &value)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockPluginPanel != nullptr)
	{
		win->dockPluginPanel->setButtonText(pluginId, name, value);
	}
}

void UI::PLUGINPANEL::addSlider(unsigned int pluginId, const QString &buttonName, int min, int max, int row, int col, int rspan, int cspan)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockPluginPanel != nullptr)
	{
		win->dockPluginPanel->addSlider(pluginId, buttonName, min, max, row, col, rspan, cspan);
	}
}

int UI::PLUGINPANEL::getSliderValue(unsigned int pluginId, const QString &name)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockPluginPanel != nullptr)
	{
		return win->dockPluginPanel->getSliderValue(pluginId, name);
	}
	return -1;
}

int UI::PLUGINPANEL::setSliderValue(unsigned int pluginId, const QString &name, int value)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockPluginPanel != nullptr)
	{
		return win->dockPluginPanel->setSliderValue(pluginId, name, value);
	}
	return -1;
}

void UI::PLUGINPANEL::setSliderRange(unsigned int pluginId, const QString &name, int min, int max)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockPluginPanel != nullptr)
	{
		win->dockPluginPanel->setSliderRange(pluginId, name, min, max);
	}
}


// PLUGINPANEL - EDITBOX

void UI::PLUGINPANEL::addEditBox(unsigned int pluginId, const QString &name, const QString &label, const QString &value, int row, int col, int rspan, int cspan)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockPluginPanel != nullptr)
	{
		win->dockPluginPanel->addEditBox(pluginId, name, label, value, row, col, rspan, cspan);
	}
}

QString UI::PLUGINPANEL::getEditBoxValue(unsigned int pluginId, const QString &name)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockPluginPanel != nullptr)
	{
		return win->dockPluginPanel->getEditBoxValue(pluginId, name);
	}
	return "";
}

void UI::PLUGINPANEL::setEditBoxValue(unsigned int pluginId, const QString &name, const QString &value)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockPluginPanel != nullptr)
	{
		win->dockPluginPanel->setEditBoxValue(pluginId, name, value);
	}
}

void UI::PLUGINPANEL::addComboBox(unsigned int pluginId, const QString &name, int row, int col, int rspan, int cspan)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockPluginPanel != nullptr)
	{
		win->dockPluginPanel->addComboBox(pluginId, name, row, col, rspan, cspan);
	}
}

QString UI::PLUGINPANEL::getComboBoxCurrentItemText(unsigned int pluginId, const QString &name)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockPluginPanel != nullptr)
	{
		return win->dockPluginPanel->getComboBoxCurrentItemText(pluginId, name);
	}
	return "";
}

void UI::PLUGINPANEL::setComboBoxItems(unsigned int pluginId, const QString &name, QStringList items)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockPluginPanel != nullptr)
	{
		//QStringList qItems;
		//for(const QString &item : items) { qItems.append(QString::fromUtf8(item.toLatin1())); }

		win->dockPluginPanel->setComboBoxItems(pluginId, name, items);
	}
}

void UI::PLUGINPANEL::setComboBoxItems(unsigned int pluginId, const char* name, std::initializer_list<const char*> items)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockPluginPanel != nullptr)
	{
		QStringList qItems;
		for(const char* item : items) { qItems.append(QString::fromUtf8(item)); }

		win->dockPluginPanel->setComboBoxItems(pluginId, QString::fromUtf8(name), qItems);
	}
}
void UI::PLUGINPANEL::setComboBoxItems(unsigned int pluginId, const std::string &name, std::vector<std::string> items)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockPluginPanel != nullptr)
	{
		QStringList qItems;
		for(const std::string &item : items) { qItems.append(QString::fromUtf8(item.c_str())); }
		
		win->dockPluginPanel->setComboBoxItems(pluginId, QString::fromUtf8(name.c_str()), qItems);
	}
}
void UI::PLUGINPANEL::setComboBoxItems(unsigned int pluginId, const std::wstring &name, std::vector<std::wstring> items)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockPluginPanel != nullptr)
	{
		QStringList qItems;
		for(const std::wstring &item : items) { qItems.append(QString::fromWCharArray(item.c_str())); }

		win->dockPluginPanel->setComboBoxItems(pluginId, QString::fromWCharArray(name.c_str()), qItems);
	}
}


// PLUGINPANEL - LABEL

void UI::PLUGINPANEL::addLabel(unsigned int pluginId, const QString &name, const QString &text, int row, int col, int rspan, int cspan)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockPluginPanel != nullptr)
	{
		win->dockPluginPanel->addLabel(pluginId, name, text, row, col, rspan, cspan);
	}
}

void UI::PLUGINPANEL::setLabel(unsigned int pluginId, const QString &name, const QString &text)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockPluginPanel != nullptr)
	{
		win->dockPluginPanel->setLabel(pluginId, name, text);
	}
}
// PROGRESSBAR

ProgressIndicator* UI::PROGRESSBAR::instance()
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr)
	{
		return win->progressIndicator;
	}
	return nullptr;
}

void UI::PROGRESSBAR::init( int min, int max, int val )
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr)
	{
		if (nullptr != win->progressIndicator)
		{
			win->progressIndicator->init(min, max, val);
			win->progressIndicator->show();
		}
	}
	AP::processEvents(true);
}

void UI::PROGRESSBAR::setValue( int val )
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr)
	{
		if (nullptr != win->progressIndicator)
		{
			win->progressIndicator->setValue(val);
		}
	}
	AP::processEvents(true);
}


void UI::PROGRESSBAR::setText(const QString text)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr)
	{
		if (nullptr != win->progressIndicator)
		{
			win->progressIndicator->setText(text);
		}
	}
	AP::processEvents(true);
}

void UI::PROGRESSBAR::hide()
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr)
	{
		if (nullptr != win->progressIndicator)
		{
			win->progressIndicator->hide();
		}
	}
	AP::processEvents(true);
}


#ifndef DISABLE_OLD

// STATUSBAR

void UI::STATUSBAR::printf(const char *format, ...)
{
	va_list paramList;
	va_start(paramList, format);

	char formatBuf[1024];
	// vsprintf_s(formatBuf, _countof(formatBuf), format, paramList);
	vsnprintf(formatBuf, sizeof(formatBuf), format, paramList);

	setText(formatBuf); 

	va_end(paramList);
}

void UI::STATUSBAR::printf(const wchar_t *format, ...)
{
	va_list paramList;
	va_start(paramList, format);

	wchar_t formatBuf[1024];
	// vswprintf_s(formatBuf, _countof(formatBuf), format, paramList);
	vswprintf(formatBuf, sizeof(formatBuf) / sizeof(formatBuf[0]), format, paramList);

	setText(formatBuf); 

	va_end(paramList);
}


#include <QElapsedTimer>

void UI::STATUSBAR::printfTimed(int mst, const char* format, ...)
{
    static QElapsedTimer timer;
    if (!timer.isValid())
        timer.start();

    if (timer.elapsed() > mst)
    {
        va_list paramList;
        va_start(paramList, format);

        char formatBuf[1024];
        vsnprintf(formatBuf, sizeof(formatBuf), format, paramList);

        va_end(paramList);

        setText(formatBuf);
        timer.restart();
    }
}



void UI::STATUSBAR::printfTimed(int mst, const wchar_t* format, ...)
{
    static QElapsedTimer timer;
    if (!timer.isValid())
        timer.start();

    if (timer.elapsed() > mst)
    {
        va_list paramList;
        va_start(paramList, format);

        wchar_t formatBuf[1024];
        vswprintf(formatBuf, sizeof(formatBuf), format, paramList);

        va_end(paramList);

        setText(formatBuf);
        timer.restart();
    }
}

void UI::STATUSBAR::setText(const QString msg)
{
	if (auto win = CMainWindow::instance())
		if (auto sb = win->statusBar())
		{
			sb->showMessage( msg );
		}

	AP::processEvents(true);
}
#endif

// MESSAGEBOX

#include <QtWidgets/QMessageBox>

void UI::MESSAGEBOX::information(const QString &msg, const QString &tittle) { QMessageBox::information(0, tittle, msg ); }
void UI::MESSAGEBOX::information(const char* msg, const char* tittle) { QMessageBox::information(0, QString::fromUtf8(tittle), QString::fromUtf8(msg)); }
void UI::MESSAGEBOX::information(const std::string &msg, const std::string &tittle) { QMessageBox::information(0, QString::fromUtf8(tittle.c_str()), QString::fromUtf8(msg.c_str())); }
void UI::MESSAGEBOX::information(const std::wstring &msg, const std::wstring &tittle ) { QMessageBox::information( 0, QString::fromWCharArray(tittle.c_str()), QString::fromWCharArray(msg.c_str()) ); }

void UI::MESSAGEBOX::warning(const QString &msg, const QString &tittle) { QMessageBox::warning( 0, tittle, msg ); }
void UI::MESSAGEBOX::warning(const char* msg, const char* tittle) { QMessageBox::warning( 0, QString::fromUtf8(tittle), QString::fromUtf8(msg) ); }
void UI::MESSAGEBOX::warning(const std::string &msg, const std::string &tittle) { QMessageBox::warning(0, QString::fromUtf8(tittle.c_str()), QString::fromUtf8(msg.c_str())); }
void UI::MESSAGEBOX::warning(const std::wstring &msg, const std::wstring &tittle ) { QMessageBox::warning( 0, QString::fromWCharArray(tittle.c_str()), QString::fromWCharArray(msg.c_str()) ); }

void UI::MESSAGEBOX::error(const QString &msg, const QString &tittle) { QMessageBox::critical(0, tittle, msg ); }
void UI::MESSAGEBOX::error(const char* msg, const char* tittle) { QMessageBox::critical(0, QString::fromUtf8(tittle), QString::fromUtf8(msg) ); }
void UI::MESSAGEBOX::error(const std::string &msg, const std::string &tittle) { QMessageBox::critical(0, QString::fromUtf8(tittle.c_str()), QString::fromUtf8(msg.c_str())); }
void UI::MESSAGEBOX::error(const std::wstring &msg, const std::wstring &tittle ) { QMessageBox::critical( 0, QString::fromWCharArray(tittle.c_str()), QString::fromWCharArray(msg.c_str())); }

int UI::MESSAGEBOX::question(const QString &msg, const QString &tittle, const QString &b0, const QString &b1, const QString &b2) { return QMessageBox::question(0, tittle, msg, b0, b1, b2); }
int UI::MESSAGEBOX::question(const char* msg, const char* tittle, const char* b0, const char* b1, const char* b2) {	return QMessageBox::question(0, QString::fromUtf8(tittle), QString::fromUtf8(msg), QString::fromUtf8(b0), QString::fromUtf8(b1), QString::fromUtf8(b2)); }
int UI::MESSAGEBOX::question(const std::string &msg, const std::string &tittle, const std::string &b0, const std::string &b1, const std::string &b2) { return QMessageBox::question(0, QString::fromUtf8(tittle.c_str()), QString::fromUtf8(msg.c_str()), QString::fromUtf8(b0.c_str()), QString::fromUtf8(b1.c_str()), QString::fromUtf8(b2.c_str())); }
int UI::MESSAGEBOX::question(const std::wstring &msg, const std::wstring &tittle, const std::wstring &b0, const std::wstring &b1, const std::wstring &b2 ) { return QMessageBox::question(0, QString::fromWCharArray(tittle.c_str()), QString::fromWCharArray(msg.c_str()), QString::fromWCharArray(b0.c_str()), QString::fromWCharArray(b1.c_str()), QString::fromWCharArray(b2.c_str())); }

// FILECHOOSER

#include <QtWidgets/QFileDialog>

std::string UI::getNativePath(std::string path)
{
	return QDir::toNativeSeparators(QString::fromStdString(path)).toStdString();
}

std::wstring UI::getNativePath( std::wstring path )
{
	return QDir::toNativeSeparators( QString::fromWCharArray(path.c_str())).toStdWString();
}

bool UI::fileExists(std::string path)
{
	return QFile::exists(QString::fromStdString(path));
}

bool UI::fileExists(std::wstring path)
{
	return QFile::exists(QString::fromWCharArray(path.c_str()));
}

QString UI::FILECHOOSER::getOpenFileName(QString title, QString dir, QString filter)
{
	return QFileDialog::getOpenFileName( 0, title, dir, filter );
}

std::wstring UI::FILECHOOSER::getOpenFileName( std::wstring title, std::wstring dir, std::wstring filter )
{
	return QDir::toNativeSeparators(
		QFileDialog::getOpenFileName(
			0,
			QString::fromWCharArray(title.c_str()),
			QString::fromWCharArray(dir.c_str()),
			QString::fromWCharArray(filter.c_str()) ) ).toStdWString();
}

std::string UI::FILECHOOSER::getOpenFileName( std::string title, std::string dir, std::string filter )
{
	return QDir::toNativeSeparators(
		QFileDialog::getOpenFileName(
			0,
			QString::fromStdString(title),
			QString::fromStdString(dir),
			QString::fromStdString(filter) ) ).toStdString();
}

QString UI::FILECHOOSER::getSaveFileName(QString title, QString dir, QString filter)
{
	return QFileDialog::getSaveFileName( 0, title, dir, filter );
}


std::string UI::FILECHOOSER::getSaveFileName( std::string title, std::string dir, std::string filter )
{
	return QDir::toNativeSeparators(
		QFileDialog::getSaveFileName(
			0,
			QString::fromStdString(title),
			QString::fromStdString(dir),
			QString::fromStdString(filter) ) ).toStdString();
}

std::wstring UI::FILECHOOSER::getSaveFileName( std::wstring title, std::wstring dir, std::wstring filter )
{
	return QDir::toNativeSeparators(
		QFileDialog::getSaveFileName(
			0,
			QString::fromWCharArray(title.c_str()),
			QString::fromWCharArray(dir.c_str()),
			QString::fromWCharArray(filter.c_str()) ) ).toStdWString();
}


// // Convert a wide Unicode string to an UTF8 string
// std::string UI::utf8_encode(const std::wstring& wstr)
// {
// 	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
// 	std::string strTo(size_needed, 0);
// 	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
// 	return strTo;
// }

// // Convert an UTF8 string to a wide Unicode String
// std::wstring UI::utf8_decode(const std::string& str)
// {
// 	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
// 	std::wstring wstrTo(size_needed, 0);
// 	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
// 	return wstrTo;
// }

// // Convert an wide Unicode string to ANSI string
// std::string UI::unicode2ansi(const std::wstring& wstr)
// {
// 	int size_needed = WideCharToMultiByte(CP_ACP, 0, &wstr[0], -1, NULL, 0, NULL, NULL);
// 	std::string strTo(size_needed, 0);
// 	WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
// 	return strTo;
// }

// // Convert an ANSI string to a wide Unicode String
// std::wstring UI::ansi2unicode(const std::string& str)
// {
// 	int size_needed = MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), NULL, 0);
// 	std::wstring wstrTo(size_needed, 0);
// 	MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
// 	return wstrTo;
// }

std::string UI::utf8_encode(const std::wstring& wstr)
{
    return QString::fromStdWString(wstr).toUtf8().constData();
}

std::wstring UI::utf8_decode(const std::string& str)
{
    return QString::fromUtf8(str.c_str()).toStdWString();
}

std::string UI::unicode2ansi(const std::wstring& wstr)
{
    return QString::fromStdWString(wstr).toLocal8Bit().constData();
}

std::wstring UI::ansi2unicode(const std::string& str)
{
    return QString::fromLocal8Bit(str.c_str()).toStdWString();
}


std::wstring UI::utf8wstr(std::string s)
{
	return QString::fromUtf8(s.c_str()).toStdWString();
}

std::wstring UI::str2wstr( std::string s )
{
	return QString::fromStdString(s).toStdWString();
}

std::string UI::wstr2str( std::wstring ws )
{
	return QString::fromWCharArray( ws.c_str() ).toUtf8().toStdString();
}


//#include <sstream>
#include <fstream>

std::string UI::stream2string( std::wstring fname )
{
	std::ifstream t(std::string(fname.begin(),fname.end()));
	if (!t.is_open()) return "";

	t.seekg(0, std::ios::end);
	size_t size = t.tellg();
	std::string buffer(size, ' ');
	t.seekg(0);
	t.read(&buffer[0], size);
	return buffer;

	//getline( t,  buffer );
	//getline( t,  buffer, '{' );
}

bool UI::FILESYSTEM::deleteFile(std::string path)
{
	return QFile(QString::fromStdString(path)).remove();
}

bool UI::FILESYSTEM::deleteFile(std::wstring path)
{
	return QFile(QString::fromWCharArray(path.c_str())).remove();
}

void UI::IMAGEVIEWER::setImage(std::string fname, bool show)
{
	CMainWindow* win = AP::mainWinPtr();

	if (win != nullptr && win->dockImage != nullptr)
	{
		win->dockImage->setImage(fname.c_str());
		win->dockImage->drawImage();

		if (show)
		{
			win->dockImage->setVisible(true);
		}
	}
}

#include "PicViewer.h"
#include "MdiChild.h"

void UI::PICVIEWER::reloadImage(int id, bool create)
{
	QMdiSubWindow* window = AP::mainWinPtr()->getPicViewerInstance(id);
	if (window != nullptr)
	{
		((PicViewer*)((MdiChild*)window->widget())->m_widget)->reloadImage();
	}
	else if (create)
	{
		AP::mainWinPtr()->activatePicViewerInstance(id);
	}
}
