#ifndef INTERFEJSAPLIKACJI_H
#define INTERFEJSAPLIKACJI_H

#include <string>
#include <list>
#include <set>

#include "BaseObject.h"

class PluginInterface;
class CWorkspace;
class CObject;
class CModel3D;
class CAnnotation;
class CMainApplication;
class CMainWindow;
class CImage;
class QString;

namespace AP
{
	__declspec(dllexport) CMainApplication &mainApp();
	__declspec(dllexport) CMainWindow &mainWin();
	__declspec(dllexport) CMainWindow* mainWinPtr();
	__declspec(dllexport) void leaveSelectionMode();

	__declspec(dllexport) int getUniqueId();

	__declspec(dllexport) void processEvents( bool immediate = false );

	// returns CWorkspace handler - use it only as a last resort.
	// If possible, use AP::WORKSPACE namespace !!!
	// Let me know what function you need is missing in this namespace.
	__declspec(dllexport) CWorkspace* getWorkspace(void);

	__declspec(dllexport) const QString& getExeFilePath(void);

	// OBSOLETE !!! use AP::MODEL::addAnnotation(int parentid, CAnnotation *an)
	__declspec(dllexport) int addAnnotation(int parentid, CAnnotation *an);

	// OBSOLETE !!! use AP::MODEL::addAnnotation(CModel3D* obj, CAnnotation *an)
	__declspec(dllexport) int addAnnotation(CModel3D* obj, CAnnotation *an);

	namespace PLUGIN {
		__declspec(dllexport) bool loadPlugin(const QString &pluginPath);
		__declspec(dllexport) void unloadPlugin(const unsigned int id);

		__declspec(dllexport) PluginInterface* getPlugin(unsigned int id);
		//__declspec(dllexport) PluginInterface* getPlugin(const char* strUUID);

		__declspec(dllexport) bool runPlugin(const char* strUUID);
	};

	namespace OBJECT {
		__declspec(dllexport) bool remove(CBaseObject* obj, bool deleteIt = false);
		__declspec(dllexport) int addChild(CBaseObject* obj, CBaseObject* child);
		__declspec(dllexport) bool removeChild(CBaseObject* obj, CBaseObject* child, bool deleteIt = false);
	};

	namespace MODEL {
		__declspec(dllexport) CModel3D * load(std::wstring path, bool synchronous=true);

		__declspec(dllexport) int addChild(CModel3D* obj, CBaseObject* child);
		__declspec(dllexport) int addChild(int parentId, CBaseObject* child);
		__declspec(dllexport) void removeChild(CBaseObject* obj, CBaseObject* child);
		__declspec(dllexport) void removeChild(int parentid, int childid);

		__declspec(dllexport) int addAnnotation(CModel3D* obj, CAnnotation *an);
		__declspec(dllexport) int addAnnotation(int parentid, CAnnotation* an);
		__declspec(dllexport) void removeAnnotation(int parentid, int id);
	};

	namespace WORKSPACE {
		__declspec(dllexport) CWorkspace* instance(void);
		__declspec(dllexport) size_t size();
		__declspec(dllexport) CBaseObject* findId(int id);
		__declspec(dllexport) bool addModel(CModel3D *obj, bool setItCurrent = false);
		__declspec(dllexport) bool addImage(CImage* im, bool showViewer = true, bool show3d = false);

		__declspec(dllexport) CModel3D* loadModel(const QString fext, const QString& path, bool synchronous = true, bool setItCurrent = false);
		__declspec(dllexport) CModel3D* loadModel(const QString& path, bool synchronous = true, bool setItCurrent = false);
		__declspec(dllexport) CModel3D* loadModel(const std::wstring& path, bool synchronous = true, bool setItCurrent = false );

		__declspec(dllexport) void setAllModelsVisible(bool visibility = true);

		// returns copy of model
		__declspec(dllexport) CModel3D * duplicateModel(CModel3D * obj);
		__declspec(dllexport) CModel3D * duplicateModel(int id);
		__declspec(dllexport) CModel3D * duplicateCurrentModel();

		// returns pointer or nullptr if id not exists
		__declspec(dllexport) CModel3D * getModel(int id);

		// returns pointer or nullptr if none selected
		__declspec(dllexport) CModel3D * getCurrentModel();

		// returns Id or -1 if none selected
		__declspec(dllexport) int getCurrentModelId();

		// remove model by id
		__declspec(dllexport) bool removeModel(int id, bool deleteIt = true);
		// remove model by handle
		__declspec(dllexport) bool removeModel(CModel3D* obj, bool deleteIt = true);
		// remove image by id
		__declspec(dllexport) bool removeImage(int id, bool deleteIt = true);
		// remove image by handle
		__declspec(dllexport) bool removeImage(CImage* im, bool deleteIt = true);


		// the same as removeModel( getCurrentModelId() )
		__declspec(dllexport) bool removeCurrentModel();

		__declspec(dllexport) bool removeAllModels();
		__declspec(dllexport) bool removeSelectedModels();

		//__declspec(dllexport) void resetAllTransformations();
		//__declspec(dllexport) void resetSelectedTransformations();

		// select model by Id or unselect if id==-1 or not exists
		__declspec(dllexport) int setCurrentModel(int id);

		namespace SELECTION {
			__declspec(dllexport) void selectModel(int id);
			__declspec(dllexport) void unselectModel(int id);
			__declspec(dllexport) bool isModelSelected(int id);
			__declspec(dllexport) void clear();
			__declspec(dllexport) std::list<int> getList();
			__declspec(dllexport) std::list<int> getList(std::set<CBaseObject::Type> types, CObject* obj = nullptr);
			__declspec(dllexport) std::list<CBaseObject*> getObjList(std::set<CBaseObject::Type> types, CObject* obj = nullptr);
			__declspec(dllexport) void setModelsVisible(bool visibility = true);
		}
	};

	namespace EVENTS {
		__declspec(dllexport) void modelIndicationEvent( int objId );
	};
};

#endif
