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
		__declspec(dllexport) void create(unsigned int pluginId, QString label = "");
		__declspec(dllexport) void create(unsigned int pluginId, std::string label = "");
		__declspec(dllexport) void create(unsigned int pluginId, std::wstring label = L"");

		__declspec(dllexport) void clear(unsigned int pluginId);
		__declspec(dllexport) void setEnabled(unsigned int pluginId, bool b);

		__declspec(dllexport) void removeWidget(unsigned int pluginId, std::string name);
		__declspec(dllexport) void removeWidget( unsigned int pluginId, std::wstring name );

		__declspec(dllexport) QPushButton* addButton(unsigned int pluginId, QString label, QObject* receiver,const char* slot, int row, int col, int rspan = 0, int cspan = 0);

		__declspec(dllexport) QPushButton* addButton(unsigned int pluginId, std::string buttonName, std::string label, int row, int col, int rspan = 0, int cspan = 0);
		__declspec(dllexport) QPushButton* addButton( unsigned int pluginId, std::wstring buttonName, std::wstring label, int row, int col, int rspan=0, int cspan=0 );
		__declspec(dllexport) void setButtonText(unsigned int pluginId, std::string name, std::string value);
		__declspec(dllexport) void setButtonText(unsigned int pluginId, std::wstring name, std::wstring value);

		__declspec(dllexport) void addSlider(unsigned int pluginId, std::string buttonName, int min, int max, int row, int col, int rspan = 0, int cspan = 0);
		__declspec(dllexport) void addSlider( unsigned int pluginId, std::wstring buttonName, int min, int max, int row, int col, int rspan = 0, int cspan = 0);
		__declspec(dllexport) int getSliderValue(unsigned int pluginId, std::string name);
		__declspec(dllexport) int getSliderValue(unsigned int pluginId, std::wstring name);
		__declspec(dllexport) int setSliderValue(unsigned int pluginId, std::string name, int value);
		__declspec(dllexport) int setSliderValue(unsigned int pluginId, std::wstring name, int value);
		__declspec(dllexport) void setSliderRange(unsigned int pluginId, std::string name, int min, int max);
		__declspec(dllexport) void setSliderRange(unsigned int pluginId, std::wstring name, int min, int max);

		__declspec(dllexport) void addEditBox(unsigned int pluginId, std::string name, std::string label, std::string value, int row, int col, int rspan = 0, int cspan = 0);
		__declspec(dllexport) void addEditBox( unsigned int pluginId, std::wstring name, std::wstring label, std::wstring value, int row, int col, int rspan=0, int cspan=0 );
		__declspec(dllexport) std::string getEditBoxValue(unsigned int pluginId, std::string name);
		__declspec(dllexport) std::wstring getEditBoxValue( unsigned int pluginId, std::wstring name );
		__declspec(dllexport) void setEditBoxValue( unsigned int pluginId, std::string name, std::string value );
		__declspec(dllexport) void setEditBoxValue(unsigned int pluginId, std::wstring name, std::wstring value);

		__declspec(dllexport) void addComboBox(unsigned int pluginId, std::string name, int row, int col, int rspan = 0, int cspan = 0);
		__declspec(dllexport) void addComboBox(unsigned int pluginId, std::wstring name, int row, int col, int rspan = 0, int cspan = 0);
		__declspec(dllexport) std::string getComboBoxCurrentItemText(unsigned int pluginId, std::string name);
		__declspec(dllexport) std::wstring getComboBoxCurrentItemText(unsigned int pluginId, std::wstring name);
		__declspec(dllexport) void setComboBoxItems(unsigned int pluginId, std::string name, std::vector<std::string> items);
		__declspec(dllexport) void setComboBoxItems(unsigned int pluginId, std::wstring name, std::vector<std::wstring> items);

		__declspec(dllexport) void addLabel(unsigned int pluginId, std::string name, std::string text, int row, int col, int rspan = 0, int cspan = 0);
		__declspec(dllexport) void addLabel( unsigned int pluginId, std::wstring name, std::wstring text, int row, int col, int rspan=0, int cspan=0 );
		__declspec(dllexport) void setLabel(unsigned int pluginId, std::string name, std::string text);
		__declspec(dllexport) void setLabel( unsigned int pluginId, std::wstring name, std::wstring text );
	}

	//--------------------------------------------------------------------------------
	// PROGRESS BAR
	namespace PROGRESSBAR {
		__declspec(dllexport) ProgressIndicator* instance();
		__declspec(dllexport) void init( int min, int max, int val );
		__declspec(dllexport) void setValue( int val );
		__declspec(dllexport) void setText(std::string text);
		__declspec(dllexport) void setText(std::wstring text);
		__declspec(dllexport) void hide();
	};


	//--------------------------------------------------------------------------------
	// STATUS BAR
	namespace STATUSBAR {
		__declspec(dllexport) void printf(const char *format, ...);
		__declspec(dllexport) void printf(const wchar_t *format, ...);

		__declspec(dllexport) void printfTimed( int mst, const char *format, ...);
		__declspec(dllexport) void printfTimed( int mst, const wchar_t *format, ...);

		//__declspec(dllexport) void setText(const QString msg);
		//__declspec(dllexport) void setText(const char* msg);
		__declspec(dllexport) void setText(const std::string msg);
		__declspec(dllexport) void setText(const std::wstring msg );
		
	};


	//--------------------------------------------------------------------------------
	// MESSAGE BOX
	namespace MESSAGEBOX {
		//__declspec(dllexport) void information( QString msg, QString tittle = "");
		//__declspec(dllexport) void information( char* msg, char* tittle = "");
		__declspec(dllexport) void information( std::string msg, std::string tittle="" );
		__declspec(dllexport) void information( std::wstring msg, std::wstring tittle = L"");

		//__declspec(dllexport) void warning(QString msg, QString tittle = "");
		//__declspec(dllexport) void warning(char* msg, char* tittle = "");
		__declspec(dllexport) void warning(std::string msg, std::string tittle = "");
		__declspec(dllexport) void warning( std::wstring msg, std::wstring tittle = L"");

		//__declspec(dllexport) void error(char* msg, char* tittle = "");
		//__declspec(dllexport) void error(QString msg, QString tittle = "");
		__declspec(dllexport) void error(std::string msg, std::string tittle = "");
		__declspec(dllexport) void error( std::wstring msg, std::wstring tittle=L"" );

		//__declspec(dllexport) int question(QString msg, QString tittle = "", QString b0 = "Yes", QString b1 = "No", QString b2 = "");
		//__declspec(dllexport) int question(char* msg, char* tittle = "", char* b0 = "Yes", char* b1 = "No", char* b2 = "");
		__declspec(dllexport) int question(std::string msg, std::string tittle = "", std::string b0 = "Yes", std::string b1 = "No", std::string b2 = "");
		__declspec(dllexport) int question( std::wstring msg, std::wstring tittle=L"", std::wstring b0=L"Yes", std::wstring b1=L"No", std::wstring b2=L"" );
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
