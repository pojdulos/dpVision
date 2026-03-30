#include "../api/UI.h"

#include "../core/AppStateManager.h"
#include "../core/CameraControlManager.h"
#include "../core/LegacyUiSelectionWorkflow.h"
#include "../core/LegacyAppRuntime.h"
#include "../core/interfaces/IProgressListener.h"
#include "../core/UserMessageManager.h"

//#include <Windows.h> // definiuje GetTickCount()

#include <QtWidgets>

#include "../core/PluginPanelManager.h"
#include "../core/StatusBarManager.h"
#include "../core/WorkspacePanelManager.h"
#include "LegacyUiPrivilegedAccess.h"
#include "adapters/DockHistogramAPIAdapter.h"
#include "adapters/DockWorkspaceAPIAdapter.h"

namespace
{
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
}

void UI::updateView(bool repaintAll, bool buffered)
{
	AppStateManager::updateView(repaintAll, buffered);
}

void UI::updateAllViews(bool buffered)
{
	AppStateManager::updateAllViews(buffered);
}

void UI::updateCurrentView(bool buffered)
{
	AppStateManager::updateView(false, buffered);
}

void UI::changeMenuAfterSelect()
{
	LegacyUiSelectionWorkflow::changeMenuAfterSelect();
}


// WORKSPACE, MODEL, VIEWER PANELS

void UI::updateSelection(int id)
{
	UI::DOCK::WORKSPACE::selectItem(id);
	LegacyUiSelectionWorkflow::updateSelection(id);
}

void UI::DOCK::PROPERTIES::selectionChanged( int id )
{
	LegacyUiSelectionWorkflow::propertiesSelectionChanged(id);
}

void UI::DOCK::PROPERTIES::updateProperties()
{
	LegacyUiSelectionWorkflow::updateProperties();
}

DockWidgetWorkspace* UI::DOCK::WORKSPACE::instance()
{
	return LegacyUiPrivilegedAccess::workspaceDock();
}

void UI::DOCK::WORKSPACE::update()
{
	dockWorkspaceApi().rebuildTree();
}

void UI::DOCK::WORKSPACE::selectItem( int id)
{
	WorkspacePanelManager::selectWorkspaceItem(id);
}

std::shared_ptr<CBaseObject> UI::DOCK::WORKSPACE::currentItem()
{
	return LegacyUiPrivilegedAccess::currentWorkspaceItem();
}

QVector<std::shared_ptr<CBaseObject>> UI::DOCK::WORKSPACE::selectedObjects()
{
	return LegacyUiPrivilegedAccess::selectedWorkspaceObjects();
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
	WorkspacePanelManager::setWorkspaceItemLabel(id, QString::fromStdString(s));
}

void UI::DOCK::WORKSPACE::setItemLabelById(int id, std::wstring s)
{
	WorkspacePanelManager::setWorkspaceItemLabel(id, QString::fromStdWString(s));
}


void UI::DOCK::HISTOGRAM::repaint()
{
	dockHistogramApi().repaint();
}


GLViewer * UI::CAMERA::currentViewer()
{
	return LegacyUiPrivilegedAccess::currentViewer();
}

void UI::CAMERA::screenshot(QString path, void *v)
{
	LegacyUiPrivilegedAccess::screenshot(path, v);
}

void UI::CAMERA::move( float mx, float my, float mz )
{
	CameraControlManager::move(mx, my, mz);
}

void UI::CAMERA::rotate( float ax, float ay, float az )
{
	CameraControlManager::rotate(ax, ay, az);
}

void UI::CAMERA::setFloating( bool f )
{
	CameraControlManager::setFloating(f);
}

bool UI::CAMERA::convertWinToWorld(CPoint3d winCoords, CPoint3d & worldCoords)
{
	return CameraControlManager::convertWinToWorld(winCoords, worldCoords);
}

bool UI::CAMERA::convertWorldToWin(CPoint3d worldCoords, CPoint3d & winCoords)
{
	return CameraControlManager::convertWorldToWin(worldCoords, winCoords);
}

bool UI::CAMERA::convertCoords(double winX, double winY, CPoint3d& pkt0, CPoint3d& pkt1)
{
	return CameraControlManager::convertCoords(winX, winY, pkt0, pkt1);
}

CPoint3d UI::CAMERA::camPos()
{
	return CameraControlManager::camPos();
}

#include "Transform.h"

CTransform* UI::CAMERA::transform()
{
	return LegacyUiPrivilegedAccess::currentCameraTransform();
}


void UI::CAMERA::setView(int dir, std::shared_ptr<CModel3D> obj)
{
	CameraControlManager::setView(dir, std::move(obj));
}


// PLUGINPANEL

DockWidgetPluginPanel* UI::PLUGINPANEL::mainPanel()
{
	return LegacyUiPrivilegedAccess::pluginPanelHost();
}

QWidget* UI::PLUGINPANEL::instance(unsigned int pluginId)
{
	return LegacyUiPrivilegedAccess::pluginPanelInstance(pluginId);
}

void UI::PLUGINPANEL::create(unsigned int pluginId, const QString &label)
{
	PluginPanelManager::create(pluginId, label);
}

void UI::PLUGINPANEL::clear(unsigned int pluginId)
{
	PluginPanelManager::clear(pluginId);
}

void UI::PLUGINPANEL::setEnabled(unsigned int pluginId, bool b)
{
	PluginPanelManager::setEnabled(pluginId, b);
}

void UI::PLUGINPANEL::removeWidget(unsigned int pluginId, const QString &name)
{
	PluginPanelManager::removeWidget(pluginId, name);
}

// PLUGINPANEL - PUSH BUTTON
QPushButton* UI::PLUGINPANEL::addButton(unsigned int pluginId, QString label, QObject* receiver, const char* slot, int row, int col, int rspan, int cspan)
{
	return LegacyUiPrivilegedAccess::addPluginPanelButton(pluginId, label, receiver, slot, row, col, rspan, cspan);
}

QPushButton* UI::PLUGINPANEL::addButton(unsigned int pluginId, std::string buttonName, std::string label, int row, int col, int rspan, int cspan)
{
	return LegacyUiPrivilegedAccess::addPluginPanelButton(
		pluginId,
		QString::fromStdString(buttonName),
		QString::fromUtf8(label.c_str()),
		row,
		col,
		rspan,
		cspan);
}

QPushButton* UI::PLUGINPANEL::addButton( unsigned int pluginId, std::wstring buttonName, std::wstring label, int row, int col, int rspan, int cspan )
{
	return LegacyUiPrivilegedAccess::addPluginPanelButton(
		pluginId,
		QString::fromStdWString(buttonName),
		QString::fromStdWString(label),
		row,
		col,
		rspan,
		cspan);
}

void UI::PLUGINPANEL::setButtonText(unsigned int pluginId, const QString &name, const QString &value)
{
	PluginPanelManager::setButtonText(pluginId, name, value);
}

void UI::PLUGINPANEL::addSlider(unsigned int pluginId, const QString &buttonName, int min, int max, int row, int col, int rspan, int cspan)
{
	PluginPanelManager::addSlider(pluginId, buttonName, min, max, row, col, rspan, cspan);
}

int UI::PLUGINPANEL::getSliderValue(unsigned int pluginId, const QString &name)
{
	return PluginPanelManager::getSliderValue(pluginId, name);
}

int UI::PLUGINPANEL::setSliderValue(unsigned int pluginId, const QString &name, int value)
{
	return PluginPanelManager::setSliderValue(pluginId, name, value);
}

void UI::PLUGINPANEL::setSliderRange(unsigned int pluginId, const QString &name, int min, int max)
{
	PluginPanelManager::setSliderRange(pluginId, name, min, max);
}


// PLUGINPANEL - EDITBOX

void UI::PLUGINPANEL::addEditBox(unsigned int pluginId, const QString &name, const QString &label, const QString &value, int row, int col, int rspan, int cspan)
{
	PluginPanelManager::addEditBox(pluginId, name, label, value, row, col, rspan, cspan);
}

QString UI::PLUGINPANEL::getEditBoxValue(unsigned int pluginId, const QString &name)
{
	return PluginPanelManager::getEditBoxValue(pluginId, name);
}

void UI::PLUGINPANEL::setEditBoxValue(unsigned int pluginId, const QString &name, const QString &value)
{
	PluginPanelManager::setEditBoxValue(pluginId, name, value);
}

void UI::PLUGINPANEL::addComboBox(unsigned int pluginId, const QString &name, int row, int col, int rspan, int cspan)
{
	PluginPanelManager::addComboBox(pluginId, name, row, col, rspan, cspan);
}

QString UI::PLUGINPANEL::getComboBoxCurrentItemText(unsigned int pluginId, const QString &name)
{
	return PluginPanelManager::getComboBoxCurrentItemText(pluginId, name);
}

void UI::PLUGINPANEL::setComboBoxItems(unsigned int pluginId, const QString &name, QStringList items)
{
	PluginPanelManager::setComboBoxItems(pluginId, name, items);
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
	PluginPanelManager::addLabel(pluginId, name, text, row, col, rspan, cspan);
}

void UI::PLUGINPANEL::setLabel(unsigned int pluginId, const QString &name, const QString &text)
{
	PluginPanelManager::setLabel(pluginId, name, text);
}

// PROGRESSBAR

void UI::PROGRESSBAR::init( int min, int max, int val )
{
	if (auto listener = IProgressListener::getDefault())
	{
		listener->init(min, max, val);
	}
}

void UI::PROGRESSBAR::setValue( int val )
{
	if (auto listener = IProgressListener::getDefault())
	{
		listener->setValue(val);
	}
}


void UI::PROGRESSBAR::setText(const QString text)
{
	if (auto listener = IProgressListener::getDefault())
	{
		listener->setText(text.toStdString());
	}
}

void UI::PROGRESSBAR::hide()
{
	if (auto listener = IProgressListener::getDefault())
	{
		listener->hide();
	}
}



// STATUSBAR

void UI::STATUSBAR::printf(const char *format, ...)
{
    va_list paramList;
    va_start(paramList, format);
    StatusBarManager::vprintf(format, paramList);
    va_end(paramList);
    LegacyAppRuntime::processEvents(true);
}

void UI::STATUSBAR::printf(const wchar_t *format, ...)
{
    va_list paramList;
    va_start(paramList, format);
    StatusBarManager::vprintf(format, paramList);
    va_end(paramList);
    LegacyAppRuntime::processEvents(true);
}


void UI::STATUSBAR::printfTimed(int mst, const char* format, ...)
{
    va_list paramList;
    va_start(paramList, format);
    StatusBarManager::vprintfTimed(mst, format, paramList);
    va_end(paramList);
    LegacyAppRuntime::processEvents(true);
}



void UI::STATUSBAR::printfTimed(int mst, const wchar_t* format, ...)
{
    va_list paramList;
    va_start(paramList, format);
    StatusBarManager::vprintfTimed(mst, format, paramList);
    va_end(paramList);
    LegacyAppRuntime::processEvents(true);
}

void UI::STATUSBAR::setText(const QString msg)
{
	StatusBarManager::setText(msg);
	LegacyAppRuntime::processEvents(true);
}



// MESSAGEBOX

void UI::MESSAGEBOX::information(const QString &msg, const QString &tittle) { UserMessageManager::information(msg.toStdString(), tittle.toStdString(), UserMessageChannel::Modal); }
void UI::MESSAGEBOX::information(const char* msg, const char* tittle) { information(QString::fromUtf8(msg), QString::fromUtf8(tittle)); }
void UI::MESSAGEBOX::information(const std::string &msg, const std::string &tittle) { information(QString::fromUtf8(msg.c_str()), QString::fromUtf8(tittle.c_str())); }
void UI::MESSAGEBOX::information(const std::wstring &msg, const std::wstring &tittle ) { information(QString::fromWCharArray(msg.c_str()), QString::fromWCharArray(tittle.c_str())); }

void UI::MESSAGEBOX::warning(const QString &msg, const QString &tittle) { UserMessageManager::warning(msg.toStdString(), tittle.toStdString(), UserMessageChannel::Modal); }
void UI::MESSAGEBOX::warning(const char* msg, const char* tittle) { warning(QString::fromUtf8(msg), QString::fromUtf8(tittle)); }
void UI::MESSAGEBOX::warning(const std::string &msg, const std::string &tittle) { warning(QString::fromUtf8(msg.c_str()), QString::fromUtf8(tittle.c_str())); }
void UI::MESSAGEBOX::warning(const std::wstring &msg, const std::wstring &tittle ) { warning(QString::fromWCharArray(msg.c_str()), QString::fromWCharArray(tittle.c_str())); }

void UI::MESSAGEBOX::error(const QString &msg, const QString &tittle) { UserMessageManager::error(msg.toStdString(), tittle.toStdString(), UserMessageChannel::Modal); }
void UI::MESSAGEBOX::error(const char* msg, const char* tittle) { error(QString::fromUtf8(msg), QString::fromUtf8(tittle)); }
void UI::MESSAGEBOX::error(const std::string &msg, const std::string &tittle) { error(QString::fromUtf8(msg.c_str()), QString::fromUtf8(tittle.c_str())); }
void UI::MESSAGEBOX::error(const std::wstring &msg, const std::wstring &tittle ) { error(QString::fromWCharArray(msg.c_str()), QString::fromWCharArray(tittle.c_str())); }

int UI::MESSAGEBOX::question(const QString &msg, const QString &tittle, const QString &b0, const QString &b1, const QString &b2) { return UserMessageManager::question(msg.toStdString(), tittle.toStdString(), b0.toStdString(), b1.toStdString(), b2.toStdString(), UserMessageChannel::Modal); }
int UI::MESSAGEBOX::question(const char* msg, const char* tittle, const char* b0, const char* b1, const char* b2) { return question(QString::fromUtf8(msg), QString::fromUtf8(tittle), QString::fromUtf8(b0), QString::fromUtf8(b1), QString::fromUtf8(b2)); }
int UI::MESSAGEBOX::question(const std::string &msg, const std::string &tittle, const std::string &b0, const std::string &b1, const std::string &b2) { return question(QString::fromUtf8(msg.c_str()), QString::fromUtf8(tittle.c_str()), QString::fromUtf8(b0.c_str()), QString::fromUtf8(b1.c_str()), QString::fromUtf8(b2.c_str())); }
int UI::MESSAGEBOX::question(const std::wstring &msg, const std::wstring &tittle, const std::wstring &b0, const std::wstring &b1, const std::wstring &b2 ) { return question(QString::fromWCharArray(msg.c_str()), QString::fromWCharArray(tittle.c_str()), QString::fromWCharArray(b0.c_str()), QString::fromWCharArray(b1.c_str()), QString::fromWCharArray(b2.c_str())); }


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
	return QDir::toNativeSeparators(QFileDialog::getOpenFileName(nullptr, title, dir, filter));
}

std::wstring UI::FILECHOOSER::getOpenFileName( std::wstring title, std::wstring dir, std::wstring filter )
{
	return getOpenFileName(
		QString::fromWCharArray(title.c_str()),
		QString::fromWCharArray(dir.c_str()),
		QString::fromWCharArray(filter.c_str())).toStdWString();
}

std::string UI::FILECHOOSER::getOpenFileName( std::string title, std::string dir, std::string filter )
{
	return getOpenFileName(
		QString::fromStdString(title),
		QString::fromStdString(dir),
		QString::fromStdString(filter)).toStdString();
}

QString UI::FILECHOOSER::getSaveFileName(QString title, QString dir, QString filter)
{
	return QDir::toNativeSeparators(QFileDialog::getSaveFileName(nullptr, title, dir, filter));
}


std::string UI::FILECHOOSER::getSaveFileName( std::string title, std::string dir, std::string filter )
{
	return getSaveFileName(
		QString::fromStdString(title),
		QString::fromStdString(dir),
		QString::fromStdString(filter)).toStdString();
}

std::wstring UI::FILECHOOSER::getSaveFileName( std::wstring title, std::wstring dir, std::wstring filter )
{
	return getSaveFileName(
		QString::fromWCharArray(title.c_str()),
		QString::fromWCharArray(dir.c_str()),
		QString::fromWCharArray(filter.c_str())).toStdWString();
}

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
}

bool UI::FILESYSTEM::deleteFile(std::string path)
{
	return QFile(QString::fromStdString(path)).remove();
}

bool UI::FILESYSTEM::deleteFile(std::wstring path)
{
	return QFile(QString::fromWCharArray(path.c_str())).remove();
}

