#ifndef INTERFEJSUZYTKOWNIKA_H
#define INTERFEJSUZYTKOWNIKA_H

#include "dll_global.h"

#include <string>
#include <vector>
#include "Point3.h"
#include <QtCore/QString>

// Forward declarations
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
class QGroupBox;

/**
 * @namespace UI
 * @brief Main UI namespace containing interface management functionality
 */
namespace UI
{
    /**
     * @brief Adjusts the height of a QGroupBox based on its checked state
     * @param groupBox The group box to adjust
     * @param checked The checked state of the group box
     */
	DPVISION_EXPORT void adjustGroupBoxHeight(QGroupBox* groupBox, bool checked);

	/**
     * @brief Checks if specified time has elapsed
     * @param mst Time in milliseconds
     * @return True if specified time has elapsed
     */
	DPVISION_EXPORT bool timeElapsed(int mst);

    /**
     * @brief Shows the main UI
     */
    DPVISION_EXPORT void show();

    /**
     * @brief Updates the current view
     * @param repaintAll If true, repaints all elements
     * @param buffered If true, uses buffered rendering
     */
    DPVISION_EXPORT void updateView(bool repaintAll = true, bool buffered = true);

    /**
     * @brief Updates all views in the application
     * @param buffered If true, uses buffered rendering
     */
    DPVISION_EXPORT void updateAllViews(bool buffered = true);

    /**
     * @brief Updates only the current view
     * @param buffered If true, uses buffered rendering
     */
    DPVISION_EXPORT void updateCurrentView(bool buffered = true);

    /**
     * @brief Updates the menu after selection change
     */
    DPVISION_EXPORT void changeMenuAfterSelect();

    /**
     * @brief Updates the selection state for specified ID
     * @param id Object identifier
     */
    DPVISION_EXPORT void updateSelection(int id);

    /**
     * @brief String encoding conversion functions
     */
    DPVISION_EXPORT std::string utf8_encode(const std::wstring& wstr);
    DPVISION_EXPORT std::wstring utf8_decode(const std::string& str);
    DPVISION_EXPORT std::string unicode2ansi(const std::wstring& wstr);
    DPVISION_EXPORT std::wstring ansi2unicode(const std::string& str);
    DPVISION_EXPORT std::wstring utf8wstr(std::string s);
    DPVISION_EXPORT std::wstring str2wstr(std::string s);
    DPVISION_EXPORT std::string wstr2str(std::wstring s);
    DPVISION_EXPORT std::string stream2string(std::wstring fname);

    /**
     * @brief Path and file management functions
     */
    DPVISION_EXPORT std::string getNativePath(std::string path);
    DPVISION_EXPORT std::wstring getNativePath(std::wstring path);
    DPVISION_EXPORT bool fileExists(std::string path);
    DPVISION_EXPORT bool fileExists(std::wstring path);

    /**
     * @namespace FILESYSTEM
     * @brief File system operations namespace
     */
    namespace FILESYSTEM {
        /**
         * @brief Deletes a file at specified path
         * @param path Path to the file
         * @return True if deletion was successful
         */
 		DPVISION_EXPORT bool deleteFile(std::string path);
		DPVISION_EXPORT bool deleteFile(std::wstring path);
	}

    /**
     * @namespace CAMERA
     * @brief Camera control and manipulation namespace
     */
    namespace CAMERA {
        /**
         * @brief Moves camera by specified offset
         * @param mx X-axis movement
         * @param my Y-axis movement
         * @param mz Z-axis movement
         */
        DPVISION_EXPORT void move(float mx, float my, float mz);

        /**
         * @brief Rotates camera by specified angles
         * @param ax X-axis rotation angle
         * @param ay Y-axis rotation angle
         * @param az Z-axis rotation angle
         */
        DPVISION_EXPORT void rotate(float ax, float ay, float az);

        /**
         * @brief Sets camera floating state
         * @param f Floating state
         */
        DPVISION_EXPORT void setFloating(bool f);

        /**
         * @brief Coordinate conversion functions
         */
		DPVISION_EXPORT bool convertWinToWorld( CPoint3d winCoords, CPoint3d &worldCoords);
		DPVISION_EXPORT bool convertWorldToWin( CPoint3d worldCoords, CPoint3d &winCoords);
		DPVISION_EXPORT bool convertCoords(double winX, double winY, CPoint3d& pkt0, CPoint3d& pkt1);

        /**
         * @brief Gets current camera position
         * @return Camera position as CPoint3d
         */
		DPVISION_EXPORT CPoint3d camPos();

        /**
         * @brief Gets camera transform
         * @return Pointer to camera transform
         */
		DPVISION_EXPORT CTransform* transform();

		/* odpowiednik sekwencji klawiszy ctrl+L ctrl+.. 
		* dir => kierunek z którego patrzymy
		*	1 - front, 2 - back,
		*	3 - left, 4 - right,
		*	5 - top, 6 - bottom
		* obj != nullptr => centruje na obiekcie
		* obj == nullptr (domyślnie) => centruje na całej scenie
		*/

        /**
         * @brief Sets predefined camera view
         * @param dir Direction to view from (1-front, 2-back, 3-left, 4-right, 5-top, 6-bottom)
         * @param obj Optional object to center on (nullptr centers on entire scene)
         */
		DPVISION_EXPORT void setView(int dir, CModel3D* obj = nullptr);

        /**
         * @brief Gets current GLViewer instance
         * @return Pointer to current GLViewer
         */
		DPVISION_EXPORT GLViewer* currentViewer();

        /**
         * @brief Takes screenshot of viewer window
         * @param path Path to save screenshot
         * @param v Optional viewer pointer (nullptr uses active viewer)
         */
		DPVISION_EXPORT void screenshot(QString path, void* v = nullptr);
		DPVISION_EXPORT inline void screenshot(const char* path, void* v = nullptr) { screenshot(QString::fromStdString(path), v); };
		DPVISION_EXPORT inline void screenshot(std::string path, void* v = nullptr) { screenshot(QString::fromStdString(path), v); };
		DPVISION_EXPORT inline void screenshot(std::wstring path, void* v = nullptr) { screenshot(QString::fromStdWString(path), v); };
	}

    /**
     * @namespace PICVIEWER
     * @brief Picture viewer functionality namespace
     */
	namespace PICVIEWER {
        /**
         * @brief Reloads image with specified ID
         * @param id Image identifier
         * @param create If true, creates new image if not exists
         */
		DPVISION_EXPORT void reloadImage(int id, bool create = false);
	}

    /**
     * @namespace DOCK
     * @brief Dock windows management namespace
     */
	namespace DOCK {
        /**
         * @brief Updates all dock windows for specified ID
         * @param id Object identifier
         */
		DPVISION_EXPORT void updateAll( int id );

        /**
         * @namespace PROPERTIES
         * @brief Properties dock window functionality
         */
		namespace PROPERTIES {
			DPVISION_EXPORT void show(bool b);
			DPVISION_EXPORT void selectionChanged( int id );
			DPVISION_EXPORT void updateProperties();
		}

        /**
         * @namespace WORKSPACE
         * @brief Workspace dock window functionality
         */
		namespace WORKSPACE {
			DPVISION_EXPORT DockWidgetWorkspace* instance();
			DPVISION_EXPORT void show(bool b);
			DPVISION_EXPORT void rebuildTree();
			DPVISION_EXPORT void update();
			
            /**
             * @brief Item management functions
             */
			DPVISION_EXPORT void addItem(int id, int parentId = -1);
			DPVISION_EXPORT void addItem(CBaseObject* obj);
			DPVISION_EXPORT void removeItem(int id);
			DPVISION_EXPORT void selectItem( int id);

            /**
             * @brief Gets list of selected objects
             * @return Vector of selected object pointers
             */
			DPVISION_EXPORT QVector<CBaseObject*> getSelectedObjects();
			
			
			DPVISION_EXPORT CBaseObject* getCurrentItemObj();

            /**
             * @brief Item property management functions
             */
			DPVISION_EXPORT void setItemCheckedById(int id, bool b);
			DPVISION_EXPORT void setItemVisibleById(int id, bool b);
			DPVISION_EXPORT void setItemKidsVisibleById(int id, bool b);
			DPVISION_EXPORT void setItemLockedById(int id, bool b);
			DPVISION_EXPORT void setItemLabelById(int id, std::string s);
			DPVISION_EXPORT void setItemLabelById(int id, std::wstring s);
		}

        /**
         * @namespace HISTOGRAM
         * @brief Histogram dock window functionality
         */
		namespace HISTOGRAM {
			DPVISION_EXPORT void show(bool b);
			DPVISION_EXPORT void setHistogram(CHistogram *histogram);
			DPVISION_EXPORT CHistogram * getHistogram();
			DPVISION_EXPORT void repaint();
		}
	}

    /**
     * @namespace PLUGINPANEL
     * @brief Plugin panel functionality namespace
     */
	namespace PLUGINPANEL {
		DPVISION_EXPORT DockWidgetPluginPanel* mainPanel();
		DPVISION_EXPORT QWidget* instance(unsigned int pluginId);
		
        /**
         * @brief Creates plugin panel
         * @param pluginId Plugin identifier
         * @param label Optional panel label
         */
		DPVISION_EXPORT void create(unsigned int pluginId, const QString &label = "");
		inline DPVISION_EXPORT void create(unsigned int pluginId, const char* label = "") { create(pluginId, QString::fromUtf8(label)); };
		inline DPVISION_EXPORT void create(unsigned int pluginId, const std::string &label = "") { create(pluginId, QString::fromUtf8(label.c_str())); };
		inline DPVISION_EXPORT void create(unsigned int pluginId, const std::wstring &label = L"") { create(pluginId, QString::fromWCharArray(label.c_str())); };

		DPVISION_EXPORT void clear(unsigned int pluginId);
		DPVISION_EXPORT void setEnabled(unsigned int pluginId, bool b);

        /**
         * @brief Widget management functions
         */
		DPVISION_EXPORT void removeWidget(unsigned int pluginId, const QString &name);
		inline DPVISION_EXPORT void removeWidget(unsigned int pluginId, const char* name) { removeWidget(pluginId, QString::fromUtf8(name)); };
		inline DPVISION_EXPORT void removeWidget(unsigned int pluginId, const std::string &name) { removeWidget(pluginId, QString::fromUtf8(name.c_str())); };
		inline DPVISION_EXPORT void removeWidget(unsigned int pluginId, const std::wstring &name) { removeWidget(pluginId, QString::fromWCharArray(name.c_str())); };

		// BUTTON

		DPVISION_EXPORT QPushButton* addButton(unsigned int pluginId, QString label, QObject* receiver,const char* slot, int row, int col, int rspan = 0, int cspan = 0);

		DPVISION_EXPORT QPushButton* addButton(unsigned int pluginId, std::string buttonName, std::string label, int row, int col, int rspan = 0, int cspan = 0);
		DPVISION_EXPORT QPushButton* addButton( unsigned int pluginId, std::wstring buttonName, std::wstring label, int row, int col, int rspan=0, int cspan=0 );
		
		DPVISION_EXPORT void setButtonText(unsigned int pluginId, const QString &name, const QString &value);
		inline DPVISION_EXPORT void setButtonText(unsigned int pluginId, const char* name, const char* value) { setButtonText(pluginId, QString::fromUtf8(name), QString::fromUtf8(value)); };
		inline DPVISION_EXPORT void setButtonText(unsigned int pluginId, const std::string &name, const std::string &value) { setButtonText(pluginId, QString::fromUtf8(name.c_str()), QString::fromUtf8(value.c_str())); };
		inline DPVISION_EXPORT void setButtonText(unsigned int pluginId, const std::wstring &name, const std::wstring &value) { setButtonText(pluginId, QString::fromWCharArray(name.c_str()), QString::fromWCharArray(value.c_str())); };


		// SLIDER

		DPVISION_EXPORT void addSlider(unsigned int pluginId, const QString &buttonName, int min, int max, int row, int col, int rspan = 0, int cspan = 0);
		inline DPVISION_EXPORT void addSlider(unsigned int pluginId, const char* buttonName, int min, int max, int row, int col, int rspan = 0, int cspan = 0) { addSlider(pluginId, QString::fromUtf8(buttonName), min, max, row, col, rspan, cspan); };
		inline DPVISION_EXPORT void addSlider(unsigned int pluginId, const std::string &buttonName, int min, int max, int row, int col, int rspan = 0, int cspan = 0) { addSlider(pluginId, QString::fromUtf8(buttonName.c_str()), min, max, row, col, rspan, cspan); };
		inline DPVISION_EXPORT void addSlider(unsigned int pluginId, const std::wstring &buttonName, int min, int max, int row, int col, int rspan = 0, int cspan = 0) { addSlider(pluginId, QString::fromWCharArray(buttonName.c_str()), min, max, row, col, rspan, cspan); };

		DPVISION_EXPORT int getSliderValue(unsigned int pluginId, const QString &name);
		inline DPVISION_EXPORT int getSliderValue(unsigned int pluginId, const char* name) { return getSliderValue(pluginId, QString::fromUtf8(name)); };
		inline DPVISION_EXPORT int getSliderValue(unsigned int pluginId, const std::string &name) { return getSliderValue(pluginId, QString::fromUtf8(name.c_str())); };
		inline DPVISION_EXPORT int getSliderValue(unsigned int pluginId, const std::wstring &name) { return getSliderValue(pluginId, QString::fromWCharArray(name.c_str())); };

		DPVISION_EXPORT int setSliderValue(unsigned int pluginId, const QString &name, int value);
		inline DPVISION_EXPORT int setSliderValue(unsigned int pluginId, const char* name, int value) { return setSliderValue(pluginId, QString::fromUtf8(name), value); };
		inline DPVISION_EXPORT int setSliderValue(unsigned int pluginId, const std::string &name, int value) { return setSliderValue(pluginId, QString::fromUtf8(name.c_str()), value); };
		inline DPVISION_EXPORT int setSliderValue(unsigned int pluginId, const std::wstring &name, int value) { return setSliderValue(pluginId, QString::fromWCharArray(name.c_str()), value); };

		DPVISION_EXPORT void setSliderRange(unsigned int pluginId, const QString &name, int min, int max);
		inline DPVISION_EXPORT void setSliderRange(unsigned int pluginId, const char* name, int min, int max) { setSliderRange(pluginId, QString::fromUtf8(name), min, max); };
		inline DPVISION_EXPORT void setSliderRange(unsigned int pluginId, const std::string& name, int min, int max) { setSliderRange(pluginId, QString::fromUtf8(name.c_str()), min, max); };
		inline DPVISION_EXPORT void setSliderRange(unsigned int pluginId, const std::wstring& name, int min, int max) { setSliderRange(pluginId, QString::fromWCharArray(name.c_str()), min, max); };

		// EDITBOX

		DPVISION_EXPORT void addEditBox(unsigned int pluginId, const QString &name, const QString &label, const QString &value, int row, int col, int rspan = 0, int cspan = 0);
		inline DPVISION_EXPORT void addEditBox(unsigned int pluginId, const char* name, const char* label, const char* value, int row, int col, int rspan = 0, int cspan = 0) { addEditBox(pluginId, QString::fromUtf8(name), QString::fromUtf8(label), QString::fromUtf8(value), row, col, rspan, cspan); };
		inline DPVISION_EXPORT void addEditBox(unsigned int pluginId, const std::string& name, const std::string& label, const std::string& value, int row, int col, int rspan = 0, int cspan = 0) { addEditBox(pluginId, QString::fromUtf8(name.c_str()), QString::fromUtf8(label.c_str()), QString::fromUtf8(value.c_str()), row, col, rspan, cspan); };
		inline DPVISION_EXPORT void addEditBox(unsigned int pluginId, const std::wstring& name, const std::wstring& label, const std::wstring& value, int row, int col, int rspan = 0, int cspan = 0) { addEditBox(pluginId, QString::fromWCharArray(name.c_str()), QString::fromWCharArray(label.c_str()), QString::fromWCharArray(value.c_str()), row, col, rspan, cspan); };
		
		DPVISION_EXPORT QString getEditBoxValue(unsigned int pluginId, const QString &name);
		inline DPVISION_EXPORT char* getEditBoxValue(unsigned int pluginId, const char* name) { return getEditBoxValue(pluginId, QString::fromUtf8(name)).toUtf8().data(); };
		inline DPVISION_EXPORT std::string getEditBoxValue(unsigned int pluginId, const std::string& name) { return getEditBoxValue(pluginId, QString::fromUtf8(name.c_str())).toUtf8().toStdString(); };
		inline DPVISION_EXPORT std::wstring getEditBoxValue(unsigned int pluginId, const std::wstring& name) { return getEditBoxValue(pluginId, QString::fromWCharArray(name.c_str())).toStdWString(); };
		
		DPVISION_EXPORT void setEditBoxValue(unsigned int pluginId, const QString &name, const QString &value);
		inline DPVISION_EXPORT void setEditBoxValue(unsigned int pluginId, const char* name, const char* value) { setEditBoxValue(pluginId, QString::fromUtf8(name), QString::fromUtf8(value)); };
		inline DPVISION_EXPORT void setEditBoxValue(unsigned int pluginId, const std::string& name, const std::string& value) { setEditBoxValue(pluginId, QString::fromUtf8(name.c_str()), QString::fromUtf8(value.c_str())); };
		inline DPVISION_EXPORT void setEditBoxValue(unsigned int pluginId, const std::wstring& name, const std::wstring& value) { setEditBoxValue(pluginId, QString::fromWCharArray(name.c_str()), QString::fromWCharArray(value.c_str())); };

		// COMBOBOX

		DPVISION_EXPORT void addComboBox(unsigned int pluginId, const QString &name, int row, int col, int rspan = 0, int cspan = 0);
		inline DPVISION_EXPORT void addComboBox(unsigned int pluginId, const char* name, int row, int col, int rspan = 0, int cspan = 0) { addComboBox(pluginId, QString::fromUtf8(name), row, col, rspan, cspan); }
		inline DPVISION_EXPORT void addComboBox(unsigned int pluginId, const std::string& name, int row, int col, int rspan = 0, int cspan = 0) { addComboBox(pluginId, QString::fromUtf8(name.c_str()), row, col, rspan, cspan); }
		inline DPVISION_EXPORT void addComboBox(unsigned int pluginId, const std::wstring& name, int row, int col, int rspan = 0, int cspan = 0) { addComboBox(pluginId, QString::fromWCharArray(name.c_str()), row, col, rspan, cspan); }
		
		DPVISION_EXPORT QString getComboBoxCurrentItemText(unsigned int pluginId, const QString &name);
		inline DPVISION_EXPORT char* getComboBoxCurrentItemText(unsigned int pluginId, const char* name) { return getComboBoxCurrentItemText(pluginId, QString::fromUtf8(name)).toUtf8().data(); }
		inline DPVISION_EXPORT std::string getComboBoxCurrentItemText(unsigned int pluginId, const std::string& name) { return getComboBoxCurrentItemText(pluginId, QString::fromUtf8(name.c_str())).toUtf8().toStdString(); }
		inline DPVISION_EXPORT std::wstring getComboBoxCurrentItemText(unsigned int pluginId, const std::wstring& name) { return getComboBoxCurrentItemText(pluginId, QString::fromWCharArray(name.c_str())).toStdWString(); }
		
		DPVISION_EXPORT void setComboBoxItems(unsigned int pluginId, const QString &name, QStringList items);
		DPVISION_EXPORT void setComboBoxItems(unsigned int pluginId, const char* name, std::initializer_list<const char*> items);
		DPVISION_EXPORT void setComboBoxItems(unsigned int pluginId, const std::string &name, std::vector<std::string> items);
		DPVISION_EXPORT void setComboBoxItems(unsigned int pluginId, const std::wstring &name, std::vector<std::wstring> items);

		// LABEL

		DPVISION_EXPORT void addLabel(unsigned int pluginId, const QString &name, const QString &text, int row, int col, int rspan = 0, int cspan = 0);
		inline DPVISION_EXPORT void addLabel(unsigned int pluginId, const char* name, const char* text, int row, int col, int rspan = 0, int cspan = 0) { addLabel(pluginId, QString::fromUtf8(name), QString::fromUtf8(text), row, col, rspan, cspan); }
		inline DPVISION_EXPORT void addLabel(unsigned int pluginId, const std::string &name, const std::string &text, int row, int col, int rspan = 0, int cspan = 0) { addLabel(pluginId, QString::fromUtf8(name.c_str()), QString::fromUtf8(text.c_str()), row, col, rspan, cspan); }
		inline DPVISION_EXPORT void addLabel(unsigned int pluginId, const std::wstring &name, const std::wstring &text, int row, int col, int rspan = 0, int cspan = 0) { addLabel(pluginId, QString::fromWCharArray(name.c_str()), QString::fromWCharArray(text.c_str()), row, col, rspan, cspan); }

		DPVISION_EXPORT void setLabel(unsigned int pluginId, const QString &name, const QString &text);
		inline DPVISION_EXPORT void setLabel(unsigned int pluginId, const char* name, const char* text) { setLabel(pluginId, QString::fromUtf8(name), QString::fromUtf8(text)); }
		inline DPVISION_EXPORT void setLabel(unsigned int pluginId, const std::string &name, const std::string &text) { setLabel(pluginId, QString::fromUtf8(name.c_str()), QString::fromUtf8(text.c_str())); }
		inline DPVISION_EXPORT void setLabel(unsigned int pluginId, const std::wstring &name, const std::wstring &text) { setLabel(pluginId, QString::fromWCharArray(name.c_str()), QString::fromWCharArray(text.c_str())); }
	}

    /**
     * @namespace PROGRESSBAR
     * @brief Progress bar functionality namespace
     */
	namespace PROGRESSBAR {
		DPVISION_EXPORT ProgressIndicator* instance();

		DPVISION_EXPORT void init( int min, int max, int val );
		DPVISION_EXPORT void setValue( int val );
		DPVISION_EXPORT void hide();

		DPVISION_EXPORT void setText(const QString text);
		inline DPVISION_EXPORT void setText(const char* text) { setText(QString::fromUtf8(text)); };
		inline DPVISION_EXPORT void setText(const std::string text) { setText(QString::fromUtf8(text.c_str())); };
		inline DPVISION_EXPORT void setText(const std::wstring text) { setText(QString::fromWCharArray(text.c_str())); };
	};


    /**
     * @namespace STATUSBAR
     * @brief Status bar functionality namespace
     */
	namespace STATUSBAR {
		DPVISION_EXPORT void printf(const char *format, ...);
		DPVISION_EXPORT void printf(const wchar_t *format, ...);

		DPVISION_EXPORT void printfTimed( int mst, const char *format, ...);
		DPVISION_EXPORT void printfTimed( int mst, const wchar_t *format, ...);

		DPVISION_EXPORT void setText(const QString msg);
		inline DPVISION_EXPORT void setText(const char* msg) { setText(QString::fromUtf8(msg)); }
		inline DPVISION_EXPORT void setText(const std::string msg) { setText(QString::fromUtf8(msg.c_str())); }
		inline DPVISION_EXPORT void setText(const std::wstring msg) { setText(QString::fromWCharArray(msg.c_str())); }
		
	};


    /**
     * @namespace MESSAGEBOX
     * @brief Message box display functionality namespace
     */
	namespace MESSAGEBOX {
		DPVISION_EXPORT void information( const QString &msg, const QString &tittle = "");
		DPVISION_EXPORT void information( const char* msg, const char* tittle = "");
		DPVISION_EXPORT void information( const std::string &msg, const std::string &tittle="" );
		DPVISION_EXPORT void information( const std::wstring &msg, const std::wstring &tittle = L"");

		DPVISION_EXPORT void warning(const QString &msg, const QString &tittle = "");
		DPVISION_EXPORT void warning(const char* msg, const char* tittle = "");
		DPVISION_EXPORT void warning(const std::string &msg, const std::string &tittle = "");
		DPVISION_EXPORT void warning(const std::wstring &msg, const std::wstring &tittle = L"");

		DPVISION_EXPORT void error(const QString &msg, const QString &tittle = "");
		DPVISION_EXPORT void error(const char* msg, const char* tittle = "");
		DPVISION_EXPORT void error(const std::string &msg, const std::string &tittle = "");
		DPVISION_EXPORT void error(const std::wstring &msg, const std::wstring &tittle=L"" );

		DPVISION_EXPORT int question(const QString& msg, const QString& tittle = "", const QString& b0 = "Yes", const QString& b1 = "No", const QString& b2 = "");
		DPVISION_EXPORT int question(const char* msg, const char* tittle = "", const char* b0 = "Yes", const char* b1 = "No", const char* b2 = "");
		DPVISION_EXPORT int question(const std::string& msg, const std::string& tittle = "", const std::string& b0 = "Yes", const std::string& b1 = "No", const std::string& b2 = "");
		DPVISION_EXPORT int question(const std::wstring& msg, const std::wstring& tittle = L"", const std::wstring& b0 = L"Yes", const std::wstring& b1 = L"No", const std::wstring& b2 = L"");
	};


    /**
     * @namespace FILECHOOSER
     * @brief File Open/Save window display functionality namespace
     */
	namespace FILECHOOSER {
		DPVISION_EXPORT QString getOpenFileName(QString title, QString dir, QString filter);
		DPVISION_EXPORT std::wstring getOpenFileName( std::wstring title, std::wstring dir, std::wstring filter );
		DPVISION_EXPORT std::string getOpenFileName( std::string title, std::string dir, std::string filter );

		DPVISION_EXPORT QString getSaveFileName(QString title, QString dir, QString filter);
		DPVISION_EXPORT std::wstring getSaveFileName( std::wstring title, std::wstring dir, std::wstring filter );
		DPVISION_EXPORT std::string getSaveFileName( std::string title, std::string dir, std::string filter );
	};


    /**
     * @namespace IMAGEVIEWER
     * @brief ImageViewer box display functionality namespace
     */
	namespace IMAGEVIEWER {
		DPVISION_EXPORT void setImage(std::string fname, bool show=true);
	}
};

#endif
