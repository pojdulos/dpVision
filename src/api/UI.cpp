#include "../api/UI.h"

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
#include <QCoreApplication>

#include "GLViewer.h"

#include "../core/AppStateManager.h"
#include "../core/MessageBoxManager.h"
#include "../core/StatusBarManager.h"
#include "../core/WorkspacePanelManager.h"
#include "adapters/DockHistogramAPIAdapter.h"
#include "adapters/DockWorkspaceAPIAdapter.h"
#include "adapters/FileDialogAPIAdapter.h"
#include "adapters/GuiCameraAPIAdapter.h"
#include "adapters/GuiPluginPanelAPIAdapter.h"
#include "adapters/MessageBoxAPIAdapter.h"
#include "adapters/ProgressAPIAdapter.h"
#include "adapters/StatusBarAPIAdapter.h"

namespace
{
	CMainWindow* mainWindow()
	{
		return CMainWindow::instance();
	}

	DockWidgetPluginPanel* pluginPanel()
	{
		if (auto win = mainWindow())
		{
			return win->dockPluginPanel;
		}
		return nullptr;
	}

	void processUiEvents(bool immediate = false)
	{
		static QElapsedTimer timer;
		if (!timer.isValid())
		{
			timer.start();
		}

		if (immediate || timer.elapsed() > 1000)
		{
			QCoreApplication::processEvents();
			timer.restart();
		}
	}

	DockWorkspaceAPIAdapter& dockWorkspaceApi()
	{
		static DockWorkspaceAPIAdapter api;
		return api;
	}

	DockHistogramAPIAdapter& dockHistogramApi()
	{
		static DockHistogramAPIAdapter api;
		return api;
	}

	GuiPluginPanelAPIAdapter& pluginPanelApi()
	{
		static GuiPluginPanelAPIAdapter api;
		return api;
	}

	GuiCameraAPIAdapter& cameraApi()
	{
		static GuiCameraAPIAdapter api;
		return api;
	}

	ProgressAPIAdapter& progressApi()
	{
		static ProgressAPIAdapter api;
		return api;
	}

	StatusBarAPIAdapter& statusBarApi()
	{
		static StatusBarAPIAdapter api;
		return api;
	}

	MessageBoxAPIAdapter& messageBoxApi()
	{
		static MessageBoxAPIAdapter api;
		return api;
	}

	FileDialogAPIAdapter& fileDialogApi()
	{
		static FileDialogAPIAdapter api;
		return api;
	}
}


//void UI::adjustGroupBoxHeight(QGroupBox* groupBox, bool checked)
//{
//	// Uzyskaj wysokość checkboxa i tytułu
//	QCheckBox* ch = groupBox->findChild<QCheckBox*>();
//	int checkBoxHeight = (ch) ? groupBox->findChild<QCheckBox*>()->sizeHint().height() : 0;
//	int titleHeight = groupBox->fontMetrics().height();  // Wysokość tytułu na podstawie czcionki
//
//	int heightForChecked = groupBox->sizeHint().height();  // Wysokość dla zaznaczonego stanu
//	//int heightForUnchecked = checkBoxHeight + titleHeight + 10;  // Wysokość dla odznaczonego stanu (z dodatkowym odstępem)
//
//	int heightForUnchecked = std::max(checkBoxHeight, titleHeight);
//
//	if (checked) {
//		groupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//		groupBox->setMinimumHeight(0);
//		groupBox->setMaximumHeight(QWIDGETSIZE_MAX);  // Brak limitu wysokości
//		groupBox->adjustSize();
//	}
//	else {
//		groupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
//		groupBox->setMinimumHeight(heightForUnchecked);  // Ustaw wysokość na widoczność checkboxa i tytułu
//		groupBox->setMaximumHeight(heightForUnchecked);  // Ustaw wysokość na widoczność checkboxa i tytułu
//	}
//}

// bool UI::timeElapsed(int mst)
// {
// 	static DWORD t0001 = 0;

// 	DWORD t1 = GetTickCount();
// 	if ( ( t0001 + mst ) < t1 )
// 	{
// 		t0001 = t1;
// 		processUiEvents();
// 		return true;
// 	}
// 	return false;
// }


//bool UI::timeElapsed(int mst)
//{
//    static QElapsedTimer timer;
//    if (!timer.isValid())
//        timer.start();
//
//    if (timer.elapsed() >= mst)
//    {
//        timer.restart();
//        processUiEvents();
//        return true;
//    }
//    return false;
//}

#ifndef DISABLE_OLD
//for later use
#endif


#include "MainWindow.h"

//void UI::show()
//{
//	if (auto win = CMainWindow::instance())
//	{
//		win->show();
//	}
//}

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
	AppStateManager::changeMenuAfterSelect();
}


// WORKSPACE, MODEL, VIEWER PANELS

void UI::updateSelection(int id)
{
	UI::DOCK::WORKSPACE::selectItem(id);

	UI::DOCK::PROPERTIES::selectionChanged(id);

	UI::changeMenuAfterSelect();
	UI::updateAllViews();
}

void UI::DOCK::PROPERTIES::selectionChanged( int id )
{
	WorkspacePanelManager::propertiesSelectionChanged(id);
}

void UI::DOCK::PROPERTIES::updateProperties()
{
	AppStateManager::updateProperties();
}

DockWidgetWorkspace* UI::DOCK::WORKSPACE::instance()
{
	if (auto win = mainWindow())
	{
		return win->dockWorkspace;
	}
	return nullptr;
}

void UI::DOCK::WORKSPACE::update()
{
	dockWorkspaceApi().rebuildTree();
}

void UI::DOCK::WORKSPACE::selectItem( int id)
{
	CMainWindow* win = mainWindow();

	if (win != nullptr && win->dockWorkspace != nullptr)
	{
		win->dockWorkspace->selectItem(id);
	}
}

std::shared_ptr<CBaseObject> UI::DOCK::WORKSPACE::currentItem()
{
	CMainWindow* win = mainWindow();

	if (win != nullptr && win->dockWorkspace != nullptr)
	{
		return win->dockWorkspace->getCurrentItemObj();
	}

	return nullptr;
}

QVector<std::shared_ptr<CBaseObject>> UI::DOCK::WORKSPACE::selectedObjects()
{
	CMainWindow* win = mainWindow();

	if (win != nullptr && win->dockWorkspace != nullptr)
	{
		return win->dockWorkspace->getSelectedObjects();
	}

	return {};
}


void UI::DOCK::WORKSPACE::setItemCheckedById(int id, bool b)
{
	WorkspacePanelManager::setWorkspaceItemChecked(id, b);
}

void UI::DOCK::WORKSPACE::setItemVisibleById(int id, bool b)
{
	dockWorkspaceApi().setItemVisibleById(id, b);
}

void UI::DOCK::WORKSPACE::setItemKidsVisibleById(int id, bool b)
{
	WorkspacePanelManager::setWorkspaceItemKidsVisible(id, b);
}


void UI::DOCK::WORKSPACE::setItemLockedById(int id, bool b)
{
	dockWorkspaceApi().setItemLockedById(id, b);
}

void UI::DOCK::WORKSPACE::setItemLabelById(int id, std::string s)
{
	CMainWindow* win = mainWindow();

	if (win != nullptr && win->dockWorkspace != nullptr)
	{
		win->dockWorkspace->setItemLabelById(id, QString::fromStdString(s));
	}
}

void UI::DOCK::WORKSPACE::setItemLabelById(int id, std::wstring s)
{
	CMainWindow* win = mainWindow();

	if (win != nullptr && win->dockWorkspace != nullptr)
	{
		win->dockWorkspace->setItemLabelById(id, QString::fromStdWString(s));
	}
}


void UI::DOCK::HISTOGRAM::repaint()
{
	dockHistogramApi().repaint();
}

//#include "childwindow.h"

GLViewer * UI::CAMERA::currentViewer()
{
	return cameraApi().currentViewer();
}

void UI::CAMERA::screenshot(QString path, void *v)
{
	cameraApi().screenshot(path, v);
}

void UI::CAMERA::move( float mx, float my, float mz )
{
	cameraApi().move(mx, my, mz);
}

void UI::CAMERA::rotate( float ax, float ay, float az )
{
	cameraApi().rotate(ax, ay, az);
}

void UI::CAMERA::setFloating( bool f )
{
	cameraApi().setFloating(f);
}

bool UI::CAMERA::convertWinToWorld(CPoint3d winCoords, CPoint3d & worldCoords)
{
	return cameraApi().convertWinToWorld(winCoords, worldCoords);
}

bool UI::CAMERA::convertWorldToWin(CPoint3d worldCoords, CPoint3d & winCoords)
{
	return cameraApi().convertWorldToWin(worldCoords, winCoords);
}

bool UI::CAMERA::convertCoords(double winX, double winY, CPoint3d& pkt0, CPoint3d& pkt1)
{
	return cameraApi().convertCoords(winX, winY, pkt0, pkt1);
}

CPoint3d UI::CAMERA::camPos()
{
	return cameraApi().camPos();
}

#include "Transform.h"

CTransform* UI::CAMERA::transform()
{
	return cameraApi().transform();
}


void UI::CAMERA::setView(int dir, std::shared_ptr<CModel3D> obj)
{
	cameraApi().setView(dir, std::move(obj));
}


// PLUGINPANEL
//#include <QtWidgets/QGroupBox>

QWidget* UI::PLUGINPANEL::instance(unsigned int pluginId)
{
	return pluginPanelApi().panel(pluginId);
}

void UI::PLUGINPANEL::create(unsigned int pluginId, const QString &label)
{
	pluginPanelApi().create(pluginId, label);
}

void UI::PLUGINPANEL::clear(unsigned int pluginId)
{
	if (DockWidgetPluginPanel* panel = pluginPanel())
	{
		if (QWidget* pluginWidget = panel->getPanel(pluginId))
		{
			while (QWidget* w = pluginWidget->findChild<QWidget*>())
			{
				delete w;
			}
		}
	}
}

void UI::PLUGINPANEL::setEnabled(unsigned int pluginId, bool b)
{
	pluginPanelApi().setEnabled(pluginId, b);
}

void UI::PLUGINPANEL::removeWidget(unsigned int pluginId, const QString &name)
{
	pluginPanelApi().removeWidget(pluginId, name);
}

// PLUGINPANEL - PUSH BUTTON
QPushButton* UI::PLUGINPANEL::addButton(unsigned int pluginId, QString label, QObject* receiver, const char* slot, int row, int col, int rspan, int cspan)
{
	CMainWindow* win = mainWindow();

	if (win != nullptr && win->dockPluginPanel != nullptr)
	{
		return win->dockPluginPanel->addButton(pluginId, label, receiver, slot, row, col, rspan, cspan);
	}

	return nullptr;
}

QPushButton* UI::PLUGINPANEL::addButton(unsigned int pluginId, std::string buttonName, std::string label, int row, int col, int rspan, int cspan)
{
	return pluginPanelApi().addButton(pluginId, QString::fromStdString(buttonName), QString::fromUtf8(label.c_str()), row, col, rspan, cspan);
}

QPushButton* UI::PLUGINPANEL::addButton( unsigned int pluginId, std::wstring buttonName, std::wstring label, int row, int col, int rspan, int cspan )
{
	return pluginPanelApi().addButton(pluginId, QString::fromStdWString(buttonName), QString::fromStdWString(label), row, col, rspan, cspan);
}

void UI::PLUGINPANEL::setButtonText(unsigned int pluginId, const QString &name, const QString &value)
{
	pluginPanelApi().setButtonText(pluginId, name, value);
}

void UI::PLUGINPANEL::addSlider(unsigned int pluginId, const QString &buttonName, int min, int max, int row, int col, int rspan, int cspan)
{
	pluginPanelApi().addSlider(pluginId, buttonName, min, max, row, col, rspan, cspan);
}

int UI::PLUGINPANEL::getSliderValue(unsigned int pluginId, const QString &name)
{
	return pluginPanelApi().getSliderValue(pluginId, name);
}

int UI::PLUGINPANEL::setSliderValue(unsigned int pluginId, const QString &name, int value)
{
	return pluginPanelApi().setSliderValue(pluginId, name, value);
}

void UI::PLUGINPANEL::setSliderRange(unsigned int pluginId, const QString &name, int min, int max)
{
	pluginPanelApi().setSliderRange(pluginId, name, min, max);
}


// PLUGINPANEL - EDITBOX

void UI::PLUGINPANEL::addEditBox(unsigned int pluginId, const QString &name, const QString &label, const QString &value, int row, int col, int rspan, int cspan)
{
	pluginPanelApi().addEditBox(pluginId, name, label, value, row, col, rspan, cspan);
}

QString UI::PLUGINPANEL::getEditBoxValue(unsigned int pluginId, const QString &name)
{
	return pluginPanelApi().getEditBoxValue(pluginId, name);
}

void UI::PLUGINPANEL::setEditBoxValue(unsigned int pluginId, const QString &name, const QString &value)
{
	pluginPanelApi().setEditBoxValue(pluginId, name, value);
}

void UI::PLUGINPANEL::addComboBox(unsigned int pluginId, const QString &name, int row, int col, int rspan, int cspan)
{
	pluginPanelApi().addComboBox(pluginId, name, row, col, rspan, cspan);
}

QString UI::PLUGINPANEL::getComboBoxCurrentItemText(unsigned int pluginId, const QString &name)
{
	return pluginPanelApi().getComboBoxCurrentItemText(pluginId, name);
}

void UI::PLUGINPANEL::setComboBoxItems(unsigned int pluginId, const QString &name, QStringList items)
{
	pluginPanelApi().setComboBoxItems(pluginId, name, items);
}

void UI::PLUGINPANEL::setComboBoxItems(unsigned int pluginId, const char* name, std::initializer_list<const char*> items)
{
	QStringList qItems;
	for (const char* item : items)
	{
		qItems.append(QString::fromUtf8(item));
	}
	setComboBoxItems(pluginId, QString::fromUtf8(name), qItems);
}

void UI::PLUGINPANEL::setComboBoxItems(unsigned int pluginId, const std::string &name, std::vector<std::string> items)
{
	QStringList qItems;
	for (const std::string& item : items)
	{
		qItems.append(QString::fromUtf8(item.c_str()));
	}
	setComboBoxItems(pluginId, QString::fromUtf8(name.c_str()), qItems);
}

void UI::PLUGINPANEL::setComboBoxItems(unsigned int pluginId, const std::wstring &name, std::vector<std::wstring> items)
{
	QStringList qItems;
	for (const std::wstring& item : items)
	{
		qItems.append(QString::fromWCharArray(item.c_str()));
	}
	setComboBoxItems(pluginId, QString::fromWCharArray(name.c_str()), qItems);
}


// PLUGINPANEL - LABEL

void UI::PLUGINPANEL::addLabel(unsigned int pluginId, const QString &name, const QString &text, int row, int col, int rspan, int cspan)
{
	pluginPanelApi().addLabel(pluginId, name, text, row, col, rspan, cspan);
}

void UI::PLUGINPANEL::setLabel(unsigned int pluginId, const QString &name, const QString &text)
{
	pluginPanelApi().setLabel(pluginId, name, text);
}

// PROGRESSBAR

ProgressIndicator* UI::PROGRESSBAR::instance()
{
	return progressApi().instance();
}

void UI::PROGRESSBAR::init( int min, int max, int val )
{
	progressApi().init(min, max, val);
	processUiEvents(true);
}

void UI::PROGRESSBAR::setValue( int val )
{
	progressApi().setValue(val);
	processUiEvents(true);
}


void UI::PROGRESSBAR::setText(const QString text)
{
	progressApi().setText(text);
	processUiEvents(true);
}

void UI::PROGRESSBAR::hide()
{
	progressApi().hide();
	processUiEvents(true);
}



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
	statusBarApi().setText(msg);
	processUiEvents(true);
}



// MESSAGEBOX

void UI::MESSAGEBOX::information(const QString &msg, const QString &tittle) { messageBoxApi().information(msg, tittle); }
void UI::MESSAGEBOX::information(const char* msg, const char* tittle) { MessageBoxManager::information(QString::fromUtf8(msg).toStdString(), QString::fromUtf8(tittle).toStdString(), UserMessageChannel::Modal); }
void UI::MESSAGEBOX::information(const std::string &msg, const std::string &tittle) { MessageBoxManager::information(msg, tittle, UserMessageChannel::Modal); }
void UI::MESSAGEBOX::information(const std::wstring &msg, const std::wstring &tittle ) { MessageBoxManager::information(QString::fromWCharArray(msg.c_str()).toStdString(), QString::fromWCharArray(tittle.c_str()).toStdString(), UserMessageChannel::Modal); }

void UI::MESSAGEBOX::warning(const QString &msg, const QString &tittle) { messageBoxApi().warning(msg, tittle); }
void UI::MESSAGEBOX::warning(const char* msg, const char* tittle) { MessageBoxManager::warning(QString::fromUtf8(msg).toStdString(), QString::fromUtf8(tittle).toStdString(), UserMessageChannel::Modal); }
void UI::MESSAGEBOX::warning(const std::string &msg, const std::string &tittle) { MessageBoxManager::warning(msg, tittle, UserMessageChannel::Modal); }
void UI::MESSAGEBOX::warning(const std::wstring &msg, const std::wstring &tittle ) { MessageBoxManager::warning(QString::fromWCharArray(msg.c_str()).toStdString(), QString::fromWCharArray(tittle.c_str()).toStdString(), UserMessageChannel::Modal); }

void UI::MESSAGEBOX::error(const QString &msg, const QString &tittle) { messageBoxApi().error(msg, tittle); }
void UI::MESSAGEBOX::error(const char* msg, const char* tittle) { MessageBoxManager::error(QString::fromUtf8(msg).toStdString(), QString::fromUtf8(tittle).toStdString(), UserMessageChannel::Modal); }
void UI::MESSAGEBOX::error(const std::string &msg, const std::string &tittle) { MessageBoxManager::error(msg, tittle, UserMessageChannel::Modal); }
void UI::MESSAGEBOX::error(const std::wstring &msg, const std::wstring &tittle ) { MessageBoxManager::error(QString::fromWCharArray(msg.c_str()).toStdString(), QString::fromWCharArray(tittle.c_str()).toStdString(), UserMessageChannel::Modal); }

int UI::MESSAGEBOX::question(const QString &msg, const QString &tittle, const QString &b0, const QString &b1, const QString &b2) { return messageBoxApi().question(msg, tittle, b0, b1, b2); }
int UI::MESSAGEBOX::question(const char* msg, const char* tittle, const char* b0, const char* b1, const char* b2) { return MessageBoxManager::question(QString::fromUtf8(msg).toStdString(), QString::fromUtf8(tittle).toStdString(), QString::fromUtf8(b0).toStdString(), QString::fromUtf8(b1).toStdString(), QString::fromUtf8(b2).toStdString()); }
int UI::MESSAGEBOX::question(const std::string &msg, const std::string &tittle, const std::string &b0, const std::string &b1, const std::string &b2) { return MessageBoxManager::question(msg, tittle, b0, b1, b2); }
int UI::MESSAGEBOX::question(const std::wstring &msg, const std::wstring &tittle, const std::wstring &b0, const std::wstring &b1, const std::wstring &b2 ) { return MessageBoxManager::question(QString::fromWCharArray(msg.c_str()).toStdString(), QString::fromWCharArray(tittle.c_str()).toStdString(), QString::fromWCharArray(b0.c_str()).toStdString(), QString::fromWCharArray(b1.c_str()).toStdString(), QString::fromWCharArray(b2.c_str()).toStdString()); }


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
	return fileDialogApi().getOpenFileName(title, dir, filter);
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
	return fileDialogApi().getSaveFileName(title, dir, filter);
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

std::string UI::wstr2str( std::wstring ws ) { return QString::fromWCharArray( ws.c_str() ).toUtf8().toStdString(); }


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

