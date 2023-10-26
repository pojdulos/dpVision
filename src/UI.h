#ifndef INTERFEJSUZYTKOWNIKA_H
#define INTERFEJSUZYTKOWNIKA_H

#include <string>
#include <vector>
#include "Point3.h"
#include <QtCore/QString>

class QPushButton;
class CHistogram;
class QObject;
class CBaseObject;
class ProgressIndicator;
class QWidget;
class DockWidgetPluginPanel;
class DockWidgetWorkspace;
class CTransform;
class CModel3D;
class GLViewer;

namespace UI
{
	__declspec(dllexport) bool timeElapsed(int mst);
	__declspec(dllexport) void show();
	__declspec(dllexport) void updateView(bool repaintAll = true, bool buffered = true);
	__declspec(dllexport) void updateAllViews( bool buffered = true );
	__declspec(dllexport) void updateCurrentView( bool buffered = true );
	__declspec(dllexport) void changeMenuAfterSelect();

	__declspec(dllexport) void updateSelection(int id);

	__declspec(dllexport) std::string utf8_encode(const std::wstring& wstr);
	__declspec(dllexport) std::wstring utf8_decode(const std::string& str);
	__declspec(dllexport) std::string unicode2ansi(const std::wstring& wstr);
	__declspec(dllexport) std::wstring ansi2unicode(const std::string& str);

	__declspec(dllexport) std::wstring utf8wstr(std::string s);

	__declspec(dllexport) std::wstring str2wstr( std::string s );
	__declspec(dllexport) std::string wstr2str( std::wstring s );
	__declspec(dllexport) std::string stream2string( std::wstring fname );

	__declspec(dllexport) std::string getNativePath(std::string path);
	__declspec(dllexport) std::wstring getNativePath( std::wstring path );
	__declspec(dllexport) bool fileExists(std::string path);
	__declspec(dllexport) bool fileExists(std::wstring path);

	namespace FILESYSTEM {
		__declspec(dllexport) bool deleteFile(std::string path);
		__declspec(dllexport) bool deleteFile(std::wstring path);
	}

	namespace CAMERA {
		__declspec(dllexport) void move( float mx, float my, float mz );
		__declspec(dllexport) void rotate( float ax, float ay, float az );
		__declspec(dllexport) void setFloating( bool f );

		__declspec(dllexport) bool convertWinToWorld( CPoint3d winCoords, CPoint3d &worldCoords);
		__declspec(dllexport) bool convertWorldToWin( CPoint3d worldCoords, CPoint3d &winCoords);
		__declspec(dllexport) bool convertCoords(double winX, double winY, CPoint3d& pkt0, CPoint3d& pkt1);
		__declspec(dllexport) CPoint3d camPos();
		__declspec(dllexport) CTransform* transform();

		/* odpowiednik sekwencji klawiszy ctrl+L ctrl+.. 
		* dir => kierunek z którego patrzymy
		*	1 - front, 2 - back,
		*	3 - left, 4 - right,
		*	5 - top, 6 - bottom
		* obj != nullptr => centruje na obiekcie
		* obj == nullptr (domyślnie) => centruje na całej scenie
		*/
		__declspec(dllexport) void setView(int dir, CModel3D* obj = nullptr);

		__declspec(dllexport) GLViewer* currentViewer();

		__declspec(dllexport) void screenshot(QString path);
		__declspec(dllexport) inline void screenshot(const char* path) { screenshot(QString::fromStdString(path)); };
		__declspec(dllexport) inline void screenshot(std::string path) { screenshot(QString::fromStdString(path)); };
		__declspec(dllexport) inline void screenshot(std::wstring path) { screenshot(QString::fromStdWString(path)); };
	}

	namespace PICVIEWER {
		__declspec(dllexport) void reloadImage(int id, bool create = false);
	}

	namespace DOCK {
		__declspec(dllexport) void updateAll( int id );

		namespace PROPERTIES {
			__declspec(dllexport) void show(bool b);
			__declspec(dllexport) void selectionChanged( int id );
			__declspec(dllexport) void updateProperties();
		}

		namespace WORKSPACE {
			__declspec(dllexport) DockWidgetWorkspace* instance();
			__declspec(dllexport) void show(bool b);
			__declspec(dllexport) void rebuildTree();
			__declspec(dllexport) void update();
			
			__declspec(dllexport) void addItem(int id, int parentId = -1);
			__declspec(dllexport) void addItem(CBaseObject* obj);
			__declspec(dllexport) void removeItem(int id);
			__declspec(dllexport) void selectItem( int id);

			__declspec(dllexport) void setItemCheckedById(int id, bool b);
			__declspec(dllexport) void setItemVisibleById(int id, bool b);
			__declspec(dllexport) void setItemKidsVisibleById(int id, bool b);
			__declspec(dllexport) void setItemLockedById(int id, bool b);
			__declspec(dllexport) void setItemLabelById(int id, std::string s);
			__declspec(dllexport) void setItemLabelById(int id, std::wstring s);
		}

		namespace HISTOGRAM {
			__declspec(dllexport) void show(bool b);
			__declspec(dllexport) void setHistogram(CHistogram *histogram);
			__declspec(dllexport) CHistogram * getHistogram();
			__declspec(dllexport) void repaint();
		}
	}

	namespace PLUGINPANEL {
		__declspec(dllexport) DockWidgetPluginPanel* mainPanel();
		__declspec(dllexport) QWidget* instance(unsigned int pluginId);
		
		__declspec(dllexport) void create(unsigned int pluginId, const QString &label = "");
		inline __declspec(dllexport) void create(unsigned int pluginId, const char* label = "") { create(pluginId, QString::fromUtf8(label)); };
		inline __declspec(dllexport) void create(unsigned int pluginId, const std::string &label = "") { create(pluginId, QString::fromUtf8(label.c_str())); };
		inline __declspec(dllexport) void create(unsigned int pluginId, const std::wstring &label = L"") { create(pluginId, QString::fromWCharArray(label.c_str())); };

		__declspec(dllexport) void clear(unsigned int pluginId);
		__declspec(dllexport) void setEnabled(unsigned int pluginId, bool b);

		__declspec(dllexport) void removeWidget(unsigned int pluginId, const QString &name);
		inline __declspec(dllexport) void removeWidget(unsigned int pluginId, const char* name) { removeWidget(pluginId, QString::fromUtf8(name)); };
		inline __declspec(dllexport) void removeWidget(unsigned int pluginId, const std::string &name) { removeWidget(pluginId, QString::fromUtf8(name.c_str())); };
		inline __declspec(dllexport) void removeWidget(unsigned int pluginId, const std::wstring &name) { removeWidget(pluginId, QString::fromWCharArray(name.c_str())); };

		// BUTTON

		__declspec(dllexport) QPushButton* addButton(unsigned int pluginId, QString label, QObject* receiver,const char* slot, int row, int col, int rspan = 0, int cspan = 0);

		__declspec(dllexport) QPushButton* addButton(unsigned int pluginId, std::string buttonName, std::string label, int row, int col, int rspan = 0, int cspan = 0);
		__declspec(dllexport) QPushButton* addButton( unsigned int pluginId, std::wstring buttonName, std::wstring label, int row, int col, int rspan=0, int cspan=0 );
		
		__declspec(dllexport) void setButtonText(unsigned int pluginId, const QString &name, const QString &value);
		inline __declspec(dllexport) void setButtonText(unsigned int pluginId, const char* name, const char* value) { setButtonText(pluginId, QString::fromUtf8(name), QString::fromUtf8(value)); };
		inline __declspec(dllexport) void setButtonText(unsigned int pluginId, const std::string &name, const std::string &value) { setButtonText(pluginId, QString::fromUtf8(name.c_str()), QString::fromUtf8(value.c_str())); };
		inline __declspec(dllexport) void setButtonText(unsigned int pluginId, const std::wstring &name, const std::wstring &value) { setButtonText(pluginId, QString::fromWCharArray(name.c_str()), QString::fromWCharArray(value.c_str())); };

		// SLIDER

		__declspec(dllexport) void addSlider(unsigned int pluginId, const QString &buttonName, int min, int max, int row, int col, int rspan = 0, int cspan = 0);
		inline __declspec(dllexport) void addSlider(unsigned int pluginId, const char* buttonName, int min, int max, int row, int col, int rspan = 0, int cspan = 0) { addSlider(pluginId, QString::fromUtf8(buttonName), min, max, row, col, rspan, cspan); };
		inline __declspec(dllexport) void addSlider(unsigned int pluginId, const std::string &buttonName, int min, int max, int row, int col, int rspan = 0, int cspan = 0) { addSlider(pluginId, QString::fromUtf8(buttonName.c_str()), min, max, row, col, rspan, cspan); };
		inline __declspec(dllexport) void addSlider(unsigned int pluginId, const std::wstring &buttonName, int min, int max, int row, int col, int rspan = 0, int cspan = 0) { addSlider(pluginId, QString::fromWCharArray(buttonName.c_str()), min, max, row, col, rspan, cspan); };

		__declspec(dllexport) int getSliderValue(unsigned int pluginId, const QString &name);
		inline __declspec(dllexport) int getSliderValue(unsigned int pluginId, const char* name) { return getSliderValue(pluginId, QString::fromUtf8(name)); };
		inline __declspec(dllexport) int getSliderValue(unsigned int pluginId, const std::string &name) { return getSliderValue(pluginId, QString::fromUtf8(name.c_str())); };
		inline __declspec(dllexport) int getSliderValue(unsigned int pluginId, const std::wstring &name) { return getSliderValue(pluginId, QString::fromWCharArray(name.c_str())); };

		__declspec(dllexport) int setSliderValue(unsigned int pluginId, const QString &name, int value);
		inline __declspec(dllexport) int setSliderValue(unsigned int pluginId, const char* name, int value) { return setSliderValue(pluginId, QString::fromUtf8(name), value); };
		inline __declspec(dllexport) int setSliderValue(unsigned int pluginId, const std::string &name, int value) { return setSliderValue(pluginId, QString::fromUtf8(name.c_str()), value); };
		inline __declspec(dllexport) int setSliderValue(unsigned int pluginId, const std::wstring &name, int value) { return setSliderValue(pluginId, QString::fromWCharArray(name.c_str()), value); };

		__declspec(dllexport) void setSliderRange(unsigned int pluginId, const QString &name, int min, int max);
		inline __declspec(dllexport) void setSliderRange(unsigned int pluginId, const char* name, int min, int max) { setSliderRange(pluginId, QString::fromUtf8(name), min, max); };
		inline __declspec(dllexport) void setSliderRange(unsigned int pluginId, const std::string& name, int min, int max) { setSliderRange(pluginId, QString::fromUtf8(name.c_str()), min, max); };
		inline __declspec(dllexport) void setSliderRange(unsigned int pluginId, const std::wstring& name, int min, int max) { setSliderRange(pluginId, QString::fromWCharArray(name.c_str()), min, max); };

		// EDITBOX

		__declspec(dllexport) void addEditBox(unsigned int pluginId, const QString &name, const QString &label, const QString &value, int row, int col, int rspan = 0, int cspan = 0);
		inline __declspec(dllexport) void addEditBox(unsigned int pluginId, const char* name, const char* label, const char* value, int row, int col, int rspan = 0, int cspan = 0) { addEditBox(pluginId, QString::fromUtf8(name), QString::fromUtf8(label), QString::fromUtf8(value), row, col, rspan, cspan); };
		inline __declspec(dllexport) void addEditBox(unsigned int pluginId, const std::string& name, const std::string& label, const std::string& value, int row, int col, int rspan = 0, int cspan = 0) { addEditBox(pluginId, QString::fromUtf8(name.c_str()), QString::fromUtf8(label.c_str()), QString::fromUtf8(value.c_str()), row, col, rspan, cspan); };
		inline __declspec(dllexport) void addEditBox(unsigned int pluginId, const std::wstring& name, const std::wstring& label, const std::wstring& value, int row, int col, int rspan = 0, int cspan = 0) { addEditBox(pluginId, QString::fromWCharArray(name.c_str()), QString::fromWCharArray(label.c_str()), QString::fromWCharArray(value.c_str()), row, col, rspan, cspan); };
		
		__declspec(dllexport) QString getEditBoxValue(unsigned int pluginId, const QString &name);
		inline __declspec(dllexport) char* getEditBoxValue(unsigned int pluginId, const char* name) { return getEditBoxValue(pluginId, QString::fromUtf8(name)).toUtf8().data(); };
		inline __declspec(dllexport) std::string getEditBoxValue(unsigned int pluginId, const std::string& name) { return getEditBoxValue(pluginId, QString::fromUtf8(name.c_str())).toUtf8().toStdString(); };
		inline __declspec(dllexport) std::wstring getEditBoxValue(unsigned int pluginId, const std::wstring& name) { return getEditBoxValue(pluginId, QString::fromWCharArray(name.c_str())).toStdWString(); };
		
		__declspec(dllexport) void setEditBoxValue(unsigned int pluginId, const QString &name, const QString &value);
		inline __declspec(dllexport) void setEditBoxValue(unsigned int pluginId, const char* name, const char* value) { setEditBoxValue(pluginId, QString::fromUtf8(name), QString::fromUtf8(value)); };
		inline __declspec(dllexport) void setEditBoxValue(unsigned int pluginId, const std::string& name, const std::string& value) { setEditBoxValue(pluginId, QString::fromUtf8(name.c_str()), QString::fromUtf8(value.c_str())); };
		inline __declspec(dllexport) void setEditBoxValue(unsigned int pluginId, const std::wstring& name, const std::wstring& value) { setEditBoxValue(pluginId, QString::fromWCharArray(name.c_str()), QString::fromWCharArray(value.c_str())); };

		// COMBOBOX

		__declspec(dllexport) void addComboBox(unsigned int pluginId, const QString &name, int row, int col, int rspan = 0, int cspan = 0);
		inline __declspec(dllexport) void addComboBox(unsigned int pluginId, const char* name, int row, int col, int rspan = 0, int cspan = 0) { addComboBox(pluginId, QString::fromUtf8(name), row, col, rspan, cspan); }
		inline __declspec(dllexport) void addComboBox(unsigned int pluginId, const std::string& name, int row, int col, int rspan = 0, int cspan = 0) { addComboBox(pluginId, QString::fromUtf8(name.c_str()), row, col, rspan, cspan); }
		inline __declspec(dllexport) void addComboBox(unsigned int pluginId, const std::wstring& name, int row, int col, int rspan = 0, int cspan = 0) { addComboBox(pluginId, QString::fromWCharArray(name.c_str()), row, col, rspan, cspan); }
		
		__declspec(dllexport) QString getComboBoxCurrentItemText(unsigned int pluginId, const QString &name);
		inline __declspec(dllexport) char* getComboBoxCurrentItemText(unsigned int pluginId, const char* name) { return getComboBoxCurrentItemText(pluginId, QString::fromUtf8(name)).toUtf8().data(); }
		inline __declspec(dllexport) std::string getComboBoxCurrentItemText(unsigned int pluginId, const std::string& name) { return getComboBoxCurrentItemText(pluginId, QString::fromUtf8(name.c_str())).toUtf8().toStdString(); }
		inline __declspec(dllexport) std::wstring getComboBoxCurrentItemText(unsigned int pluginId, const std::wstring& name) { return getComboBoxCurrentItemText(pluginId, QString::fromWCharArray(name.c_str())).toStdWString(); }
		
		__declspec(dllexport) void setComboBoxItems(unsigned int pluginId, const QString &name, QStringList items);
		__declspec(dllexport) void setComboBoxItems(unsigned int pluginId, const char* name, std::initializer_list<const char*> items);
		__declspec(dllexport) void setComboBoxItems(unsigned int pluginId, const std::string &name, std::vector<std::string> items);
		__declspec(dllexport) void setComboBoxItems(unsigned int pluginId, const std::wstring &name, std::vector<std::wstring> items);

		// LABEL

		__declspec(dllexport) void addLabel(unsigned int pluginId, const QString &name, const QString &text, int row, int col, int rspan = 0, int cspan = 0);
		inline __declspec(dllexport) void addLabel(unsigned int pluginId, const char* name, const char* text, int row, int col, int rspan = 0, int cspan = 0) { addLabel(pluginId, QString::fromUtf8(name), QString::fromUtf8(text), row, col, rspan, cspan); }
		inline __declspec(dllexport) void addLabel(unsigned int pluginId, const std::string &name, const std::string &text, int row, int col, int rspan = 0, int cspan = 0) { addLabel(pluginId, QString::fromUtf8(name.c_str()), QString::fromUtf8(text.c_str()), row, col, rspan, cspan); }
		inline __declspec(dllexport) void addLabel(unsigned int pluginId, const std::wstring &name, const std::wstring &text, int row, int col, int rspan = 0, int cspan = 0) { addLabel(pluginId, QString::fromWCharArray(name.c_str()), QString::fromWCharArray(text.c_str()), row, col, rspan, cspan); }

		__declspec(dllexport) void setLabel(unsigned int pluginId, const QString &name, const QString &text);
		inline __declspec(dllexport) void setLabel(unsigned int pluginId, const char* name, const char* text) { setLabel(pluginId, QString::fromUtf8(name), QString::fromUtf8(text)); }
		inline __declspec(dllexport) void setLabel(unsigned int pluginId, const std::string &name, const std::string &text) { setLabel(pluginId, QString::fromUtf8(name.c_str()), QString::fromUtf8(text.c_str())); }
		inline __declspec(dllexport) void setLabel(unsigned int pluginId, const std::wstring &name, const std::wstring &text) { setLabel(pluginId, QString::fromWCharArray(name.c_str()), QString::fromWCharArray(text.c_str())); }
	}

	//--------------------------------------------------------------------------------
	// PROGRESS BAR
	namespace PROGRESSBAR {
		__declspec(dllexport) ProgressIndicator* instance();

		__declspec(dllexport) void init( int min, int max, int val );
		__declspec(dllexport) void setValue( int val );
		__declspec(dllexport) void hide();

		__declspec(dllexport) void setText(const QString text);
		inline __declspec(dllexport) void setText(const char* text) { setText(QString::fromUtf8(text)); };
		inline __declspec(dllexport) void setText(const std::string text) { setText(QString::fromUtf8(text.c_str())); };
		inline __declspec(dllexport) void setText(const std::wstring text) { setText(QString::fromWCharArray(text.c_str())); };
	};


	//--------------------------------------------------------------------------------
	// STATUS BAR
	namespace STATUSBAR {
		__declspec(dllexport) void printf(const char *format, ...);
		__declspec(dllexport) void printf(const wchar_t *format, ...);

		__declspec(dllexport) void printfTimed( int mst, const char *format, ...);
		__declspec(dllexport) void printfTimed( int mst, const wchar_t *format, ...);

		__declspec(dllexport) void setText(const QString msg);
		inline __declspec(dllexport) void setText(const char* msg) { setText(QString::fromUtf8(msg)); }
		inline __declspec(dllexport) void setText(const std::string msg) { setText(QString::fromUtf8(msg.c_str())); }
		inline __declspec(dllexport) void setText(const std::wstring msg) { setText(QString::fromWCharArray(msg.c_str())); }
		
	};


	//--------------------------------------------------------------------------------
	// MESSAGE BOX
	namespace MESSAGEBOX {
		__declspec(dllexport) void information( const QString &msg, const QString &tittle = "");
		__declspec(dllexport) void information( const char* msg, const char* tittle = "");
		__declspec(dllexport) void information( const std::string &msg, const std::string &tittle="" );
		__declspec(dllexport) void information( const std::wstring &msg, const std::wstring &tittle = L"");

		__declspec(dllexport) void warning(const QString &msg, const QString &tittle = "");
		__declspec(dllexport) void warning(const char* msg, const char* tittle = "");
		__declspec(dllexport) void warning(const std::string &msg, const std::string &tittle = "");
		__declspec(dllexport) void warning(const std::wstring &msg, const std::wstring &tittle = L"");

		__declspec(dllexport) void error(const QString &msg, const QString &tittle = "");
		__declspec(dllexport) void error(const char* msg, const char* tittle = "");
		__declspec(dllexport) void error(const std::string &msg, const std::string &tittle = "");
		__declspec(dllexport) void error(const std::wstring &msg, const std::wstring &tittle=L"" );

		__declspec(dllexport) int question(const QString& msg, const QString& tittle = "", const QString& b0 = "Yes", const QString& b1 = "No", const QString& b2 = "");
		__declspec(dllexport) int question(const char* msg, const char* tittle = "", const char* b0 = "Yes", const char* b1 = "No", const char* b2 = "");
		__declspec(dllexport) int question(const std::string& msg, const std::string& tittle = "", const std::string& b0 = "Yes", const std::string& b1 = "No", const std::string& b2 = "");
		__declspec(dllexport) int question(const std::wstring& msg, const std::wstring& tittle = L"", const std::wstring& b0 = L"Yes", const std::wstring& b1 = L"No", const std::wstring& b2 = L"");
	};


	//--------------------------------------------------------------------------------
	// FILE CHOOSER DIALOG
	namespace FILECHOOSER {
		__declspec(dllexport) QString getOpenFileName(QString title, QString dir, QString filter);
		__declspec(dllexport) std::wstring getOpenFileName( std::wstring title, std::wstring dir, std::wstring filter );
		__declspec(dllexport) std::string getOpenFileName( std::string title, std::string dir, std::string filter );

		__declspec(dllexport) QString getSaveFileName(QString title, QString dir, QString filter);
		__declspec(dllexport) std::wstring getSaveFileName( std::wstring title, std::wstring dir, std::wstring filter );
		__declspec(dllexport) std::string getSaveFileName( std::string title, std::string dir, std::string filter );
	};


	namespace IMAGEVIEWER {
		__declspec(dllexport) void setImage(std::string fname, bool show=true);
	}
};

#endif
