#pragma once
#ifndef INTERFEJSAPLIKACJI_H
#define INTERFEJSAPLIKACJI_H

#include "dll_global.h"

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
	DPVISION_EXPORT CMainApplication &mainApp();
	DPVISION_EXPORT CMainWindow &mainWin();
	DPVISION_EXPORT CMainWindow* mainWinPtr();
	DPVISION_EXPORT void leaveSelectionMode();

	DPVISION_EXPORT int getUniqueId();

	DPVISION_EXPORT void processEvents( bool immediate = false );

	// returns CWorkspace handler - use it only as a last resort.
	// If possible, use AP::WORKSPACE namespace !!!
	// Let me know what function you need is missing in this namespace.
	DPVISION_EXPORT CWorkspace* getWorkspace(void);

	DPVISION_EXPORT const QString& getExeFilePath(void);

	// OBSOLETE !!! use AP::MODEL::addAnnotation(int parentid, CAnnotation *an)
	DPVISION_EXPORT int addAnnotation(int parentid, CAnnotation *an);

	// OBSOLETE !!! use AP::MODEL::addAnnotation(CModel3D* obj, CAnnotation *an)
	DPVISION_EXPORT int addAnnotation(CModel3D* obj, CAnnotation *an);

	namespace PLUGIN {
		DPVISION_EXPORT bool loadPlugin(const QString &pluginPath);
		DPVISION_EXPORT void unloadPlugin(const unsigned int id);

		DPVISION_EXPORT PluginInterface* getPlugin(unsigned int id);
		//DPVISION_EXPORT PluginInterface* getPlugin(const char* strUUID);

		DPVISION_EXPORT bool runPlugin(const char* strUUID);
	};

	namespace OBJECT {
		DPVISION_EXPORT bool remove(CBaseObject* obj, bool deleteIt = false);
		DPVISION_EXPORT int addChild(CBaseObject* obj, CBaseObject* child);
		DPVISION_EXPORT bool removeChild(CBaseObject* obj, CBaseObject* child, bool deleteIt = false);
		DPVISION_EXPORT void moveTo(CBaseObject* obj, CBaseObject* newParent);
		DPVISION_EXPORT void copyTo(CBaseObject* obj, CBaseObject* newParent);
	};

	namespace MODEL {
		DPVISION_EXPORT CModel3D * load(std::wstring path, bool synchronous=true);

		DPVISION_EXPORT int addChild(CModel3D* obj, CBaseObject* child);
		DPVISION_EXPORT int addChild(int parentId, CBaseObject* child);
		DPVISION_EXPORT void removeChild(CBaseObject* obj, CBaseObject* child);
		DPVISION_EXPORT void removeChild(int parentid, int childid);

		DPVISION_EXPORT int addAnnotation(CModel3D* obj, CAnnotation *an);
		DPVISION_EXPORT int addAnnotation(int parentid, CAnnotation* an);
		DPVISION_EXPORT void removeAnnotation(int parentid, int id);
	};

	namespace WORKSPACE {
		DPVISION_EXPORT CWorkspace* instance(void);
		DPVISION_EXPORT size_t size();
		DPVISION_EXPORT CBaseObject* findId(int id);
		DPVISION_EXPORT bool addModel(CModel3D *obj, bool setItCurrent = false);
		DPVISION_EXPORT bool addImage(CImage* im, bool showViewer = true, bool show3d = false);

		DPVISION_EXPORT CModel3D* loadModel(const QString fext, const QString& path, bool synchronous = true, bool setItCurrent = false);
		DPVISION_EXPORT CModel3D* loadModel(const QString& path, bool synchronous = true, bool setItCurrent = false);
		DPVISION_EXPORT CModel3D* loadModel(const std::wstring& path, bool synchronous = true, bool setItCurrent = false );

		DPVISION_EXPORT void setAllModelsVisible(bool visibility = true);

		// returns copy of model
		DPVISION_EXPORT CModel3D * duplicateModel(CModel3D * obj);
		DPVISION_EXPORT CModel3D * duplicateModel(int id);
		DPVISION_EXPORT CModel3D * duplicateCurrentModel();

		// returns pointer or nullptr if id not exists
		DPVISION_EXPORT CModel3D * getModel(int id);

		// returns pointer or nullptr if none selected
		DPVISION_EXPORT CModel3D * getCurrentModel();

		// returns Id or -1 if none selected
		DPVISION_EXPORT int getCurrentModelId();

		// remove model by id
		DPVISION_EXPORT bool removeModel(int id, bool deleteIt = true);
		// remove model by handle
		DPVISION_EXPORT bool removeModel(CModel3D* obj, bool deleteIt = true);
		// remove image by id
		DPVISION_EXPORT bool removeImage(int id, bool deleteIt = true);
		// remove image by handle
		DPVISION_EXPORT bool removeImage(CImage* im, bool deleteIt = true);


		// the same as removeModel( getCurrentModelId() )
		DPVISION_EXPORT bool removeCurrentModel();

		DPVISION_EXPORT bool removeAllModels();
		DPVISION_EXPORT bool removeSelectedModels();

		//DPVISION_EXPORT void resetAllTransformations();
		//DPVISION_EXPORT void resetSelectedTransformations();

		// select model by Id or unselect if id==-1 or not exists
		DPVISION_EXPORT int setCurrentModel(int id);

		namespace SELECTION {
			DPVISION_EXPORT void selectModel(int id);
			DPVISION_EXPORT void unselectModel(int id);
			DPVISION_EXPORT bool isModelSelected(int id);
			DPVISION_EXPORT void clear();
			DPVISION_EXPORT std::list<int> getList();
			DPVISION_EXPORT std::list<int> getList(std::set<CBaseObject::Type> types, CObject* obj = nullptr);
			DPVISION_EXPORT std::list<CBaseObject*> getObjList(std::set<CBaseObject::Type> types, CObject* obj = nullptr);
			DPVISION_EXPORT void setModelsVisible(bool visibility = true);
		}
	};

	namespace EVENTS {
		DPVISION_EXPORT void modelIndicationEvent( int objId );
		DPVISION_EXPORT void workspaceTreeClicked( int objId );
	};
};

#endif
