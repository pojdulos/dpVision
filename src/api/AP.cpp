#include "../api/AP.h"

#include "../core/LegacyAppRuntime.h"
#include "../core/LegacyObjectTransferService.h"

#include "adapters/AppAPIAdapter.h"
#include "adapters/ModelAPIAdapter.h"
#include "adapters/ModelLoadAPIAdapter.h"
#include "adapters/ObjectAPIAdapter.h"
#include "adapters/WorkspaceActivationAPIAdapter.h"
#include "adapters/WorkspaceBulkAPIAdapter.h"
#include "adapters/WorkspaceDuplicationAPIAdapter.h"
#include "adapters/WorkspaceImportAPIAdapter.h"
#include "adapters/WorkspaceAPIAdapter.h"
#include "adapters/WorkspaceSelectionAPIAdapter.h"


namespace AP
{
	namespace {
		WorkspaceAPIAdapter& workspaceApi()
		{
			static WorkspaceAPIAdapter api;
			return api;
		}

		ObjectAPIAdapter& objectApi()
		{
			static ObjectAPIAdapter api;
			return api;
		}

		ModelAPIAdapter& modelApi()
		{
			static ModelAPIAdapter api;
			return api;
		}

		ModelLoadAPIAdapter& modelLoadApi()
		{
			static ModelLoadAPIAdapter api;
			return api;
		}

		AppAPIAdapter& appApi()
		{
			static AppAPIAdapter api;
			return api;
		}

		WorkspaceSelectionAPIAdapter& workspaceSelectionApi()
		{
			static WorkspaceSelectionAPIAdapter api;
			return api;
		}

		WorkspaceActivationAPIAdapter& workspaceActivationApi()
		{
			static WorkspaceActivationAPIAdapter api;
			return api;
		}

		WorkspaceBulkAPIAdapter& workspaceBulkApi()
		{
			static WorkspaceBulkAPIAdapter api;
			return api;
		}

		WorkspaceImportAPIAdapter& workspaceImportApi()
		{
			static WorkspaceImportAPIAdapter api;
			return api;
		}

		WorkspaceDuplicationAPIAdapter& workspaceDuplicationApi()
		{
			static WorkspaceDuplicationAPIAdapter api;
			return api;
		}
	}

	void processEvents(bool immediate)
	{
		LegacyAppRuntime::processEvents(immediate);
	}


	const QString& getExeFilePath(void) { return appApi().exeFilePath(); }

	void adjustForCurrentFile(const QString& filePath)
	{
		appApi().adjustForCurrentFile(filePath);
	}


	int addAnnotation(int parentId, std::shared_ptr<CAnnotation> an)
	{
		return objectApi().addChild(workspaceApi().findId(parentId), an);
	}


	int addAnnotation(std::shared_ptr<CModel3D> obj, std::shared_ptr<CAnnotation> an)
	{
		return modelApi().addAnnotation(obj, an);
	}

	namespace MODEL {
		std::shared_ptr<CModel3D> load(std::wstring path, bool synchronous)
		{
			return modelLoadApi().load(std::move(path), synchronous);
		}

		void removeChild(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> child) { modelApi().removeChild(obj, child); }
		void removeChild(int parentid, int childid) { workspaceApi().removeModel(childid); }
		void removeAnnotation(int parentid, int id) { workspaceApi().removeModel(id); }


		int addChild(std::shared_ptr<CModel3D> obj, std::shared_ptr<CBaseObject> child)
		{
			return modelApi().addChild(obj, child);
		}
	
		int addAnnotation(std::shared_ptr<CModel3D> obj, std::shared_ptr<CAnnotation> an)
		{
			return modelApi().addAnnotation(obj, an);
		}
	}



	namespace WORKSPACE {
		std::shared_ptr<CModel3D> loadModel(const QString fext, const QString& path, bool synchronous, bool setItCurrent)
		{
			return workspaceImportApi().loadModel(fext, path, synchronous, setItCurrent);
		}

		std::shared_ptr<CModel3D> loadModel(const QString& path, bool synchronous, bool setItCurrent, std::shared_ptr<IProgressListener> prg)
		{
			return workspaceImportApi().loadModel(path, synchronous, setItCurrent, prg);
		}

		std::shared_ptr<CModel3D> loadModel(const std::wstring& path, bool synchronous, bool setItCurrent )
		{
			return workspaceImportApi().attachLoadedModel(modelLoadApi().load(path, synchronous), setItCurrent);
		}

		void setAllModelsVisible(bool visibility)
		{
			workspaceBulkApi().setAllVisible(visibility);
		}

		bool addModel(std::shared_ptr<CModel3D> obj, bool setItCurrent)
		{
			return workspaceApi().addModel(obj, setItCurrent);
		}

		bool addObject(std::shared_ptr<CBaseObject> obj, bool setItCurrent)
		{
			return workspaceApi().addObject(obj, setItCurrent);
		}

		void notifyObjectStateChanged(int id)
		{
			if (id != NO_CURRENT_MODEL)
			{
				CWorkspace::instance()->notifyObjectStateChanged(id);
			}
		}

		void notifyStructureChanged()
		{
			CWorkspace::instance()->notifyStructureChanged();
		}

		bool addImage(std::shared_ptr<CImage> im, bool showViewer, bool show3d)
		{
			if (im == nullptr)
			{
				return false;
			}

			im->setSelfVisibility(show3d);
			im->setShowViewer(showViewer);
			return workspaceApi().addModel(im, false);
		}


		bool removeModel(int id) { return workspaceApi().removeModel(id); };
		bool removeModel(std::shared_ptr<CModel3D> obj) { return objectApi().remove(obj); };

		bool removeImage(int id) { return workspaceApi().removeModel(id); }
		bool removeImage(std::shared_ptr<CImage> im) { return objectApi().remove(im); };


		bool removeCurrentModel()
		{
			int id = workspaceApi().getCurrentModelId();
			if (NO_CURRENT_MODEL == id) return false;

			return workspaceApi().removeModel(id);
		}

		bool removeAllModels()
		{
			return workspaceBulkApi().removeAll();
		}

		bool removeSelectedModels()
		{
			return workspaceBulkApi().removeSelected();
		}

		int setCurrentModel( int id )
		{
			return workspaceActivationApi().setCurrentObject(id);
		}

		std::shared_ptr<CModel3D> duplicateModel(std::shared_ptr<CModel3D> orginal)
		{
			return workspaceDuplicationApi().duplicateModel(orginal);
		}

		std::shared_ptr<CModel3D> duplicateModel(int id)
		{
			return workspaceDuplicationApi().duplicateModel(id);
		}

		std::shared_ptr<CModel3D> duplicateCurrentModel()
		{
			return workspaceDuplicationApi().duplicateCurrentModel();
		}

		std::shared_ptr<CModel3D> getModel(int id)
		{
			return workspaceApi().getModel(id);
		}

		std::shared_ptr<CModel3D> getCurrentModel()
		{
			return workspaceApi().getCurrentModel();
		}

		int getCurrentModelId()
		{
			return workspaceApi().getCurrentModelId();
		}

		size_t size()
		{
			return workspaceApi().size();
		}

		std::shared_ptr<CBaseObject> findId(int id)
		{
			return workspaceApi().findId(id);
		}

		////OBSOLETE FUNCTIONS

		//// obsolete, use setCurrentModel(int)
		//int AP::WORKSPACE::select(int id) { return setCurrentModel(id); };

		//// obsolete, use getModel(int)
		//CModel3D * AP::WORKSPACE::get(int id) { return getModel(id); };

		//// obsolete, use removeCurrentModel()
		//int AP::WORKSPACE::delSelected() { return removeCurrentModel(); };

		//// obsolete, use removeModel(int)
		//int AP::WORKSPACE::close(int id) { return removeModel(id); };

		//// obsolete, use getCurrentModel()
		//CModel3D * AP::WORKSPACE::getSelected() { return getCurrentModel(); };

		//// obsolete, use getCurrentModelId()
		//int AP::WORKSPACE::getSelectedId() { return getCurrentModelId(); };


		namespace SELECTION {
			void selectModel(int id)
			{
				workspaceSelectionApi().select(id);
			}
			void unselectModel(int id)
			{
				workspaceSelectionApi().unselect(id);
			}

			bool isModelSelected(int id)
			{
				return workspaceSelectionApi().contains(id);
			}

			void clear()
			{
				workspaceSelectionApi().clear();
			}

			std::list<int> getList()
			{
				const std::vector<int> ids = workspaceSelectionApi().ids();
				return std::list<int>(ids.begin(), ids.end());
			}

			std::list<int> getList(std::set<CBaseObject::Type> types, std::shared_ptr<CObject> obj)
			{
				return workspaceSelectionApi().ids(std::move(types), std::move(obj));
			}

			std::list<std::shared_ptr<CBaseObject>> getObjList(std::set<CBaseObject::Type> types, std::shared_ptr<CObject> obj)
			{
				return workspaceSelectionApi().objectList(std::move(types), std::move(obj));
			}

			void setModelsVisible(bool visibility)
			{
				workspaceSelectionApi().setSelectedVisible(visibility);
			}
		}
	}

	namespace OBJECT {
		bool remove(std::shared_ptr<CBaseObject> obj) { return objectApi().remove(obj); };
		bool removeChild(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> child) { return objectApi().removeChild(obj, child); };

		int addChild(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> child)
		{
			return objectApi().addChild(obj, child);
		}

		void moveTo(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> newParent, bool keep_pos)
		{
			LegacyObjectTransferService::moveTo(obj, newParent, keep_pos);
		}

		void copyTo(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> newParent, bool keep_pos)
		{
			LegacyObjectTransferService::copyTo(obj, newParent, keep_pos);
		}
	}
}








