#include "../api/AP.h"

#include "MainApplication.h"
#include "MainWindow.h"

#include "DockWidgetWorkspace.h"
//#include "DockWidgetPluginList.h"
#include "Annotation.h"
#include "MdiChild.h"
#include "Image.h"
#include "Workspace.h"
#include "Histogram.h"

#include <QtCore/QString>
#include "StatusBarManager.h"
#include "MessageBoxManager.h"
#include "AppStateManager.h"
#include "WorkspacePanelManager.h"
#include "adapters/ModelAPIAdapter.h"
#include "adapters/ModelLoadAPIAdapter.h"
#include "adapters/ObjectAPIAdapter.h"
#include "adapters/WorkspaceActivationAPIAdapter.h"
#include "adapters/WorkspaceBulkAPIAdapter.h"
#include "adapters/WorkspaceDuplicationAPIAdapter.h"
#include "adapters/WorkspaceImportAPIAdapter.h"
#include "adapters/WorkspaceAPIAdapter.h"
#include "adapters/WorkspaceSelectionAPIAdapter.h"

#include <QElapsedTimer>


namespace AP
{
	namespace {
		CMainApplication& mainApplication()
		{
			return *static_cast<CMainApplication*>(QApplication::instance());
		}

		CMainWindow* mainWindow()
		{
			return CMainWindow::instance();
		}

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

		std::shared_ptr<CModel3D> attachLoadedModel(std::shared_ptr<CModel3D> obj, bool setItCurrent)
		{
			if (obj != nullptr && workspaceApi().addModel(obj, setItCurrent))
			{
				return obj;
			}
			return nullptr;
		}

		bool validateObjectTransferTarget(
			const std::shared_ptr<CBaseObject>& object,
			const std::shared_ptr<CBaseObject>& newParent,
			const char* errorMessage)
		{
			if (object == nullptr)
			{
				return false;
			}

			if ((newParent != nullptr)
				&& newParent->hasCategory(CBaseObject::Category::ANNOTATION)
				&& object->hasCategory(CBaseObject::Category::OBJECT))
			{
				MessageBoxManager::error(errorMessage);
				return false;
			}

			return true;
		}

		CTransform parentGlobalTransform(const std::shared_ptr<CBaseObject>& object)
		{
			if (object != nullptr && object->getParent() != nullptr)
			{
				return CTransform(CBaseObject::getGlobalTransformationMatrix(object->getParentPtr()));
			}
			return CTransform();
		}

		std::shared_ptr<CModel3D> buildTransferWrapper(
			const std::shared_ptr<CBaseObject>& object,
			const char* description)
		{
			std::shared_ptr<CModel3D> wrapper = std::make_shared<CModel3D>();
			wrapper->setLabel("<=>");
			wrapper->setDescr(description);

			if (object->hasCategory(CBaseObject::OBJECT))
			{
				wrapper->addChild(wrapper, object);
				wrapper->importChildrenGeometry();
			}
			else if (object->hasCategory(CBaseObject::ANNOTATION))
			{
				wrapper->addAnnotation(wrapper, std::dynamic_pointer_cast<CAnnotation>(object));
			}

			return wrapper;
		}

		void attachAsRootOrChild(
			const std::shared_ptr<CBaseObject>& object,
			const std::shared_ptr<CBaseObject>& newParent)
		{
			if (newParent == nullptr)
			{
				AP::WORKSPACE::addObject(object);
			}
			else
			{
				AP::OBJECT::addChild(newParent, object);
			}
		}

	}

	void processEvents(bool immediate)
	{
		static QElapsedTimer timer;
		if (!timer.isValid())
			timer.start();

		if (immediate || timer.elapsed() > 1000)
		{
			QCoreApplication::processEvents();
			timer.restart();
		}
	}


	const QString& getExeFilePath(void) { return mainApplication().appExecDir(); }

	void adjustForCurrentFile(const QString& filePath)
	{
		AppStateManager::adjustForCurrentFile(filePath);
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
			return attachLoadedModel(CModel3D::load(path, synchronous), setItCurrent);
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

		bool addImage(std::shared_ptr<CImage> im, bool showViewer, bool show3d)
		{
			im->setSelfVisibility(show3d);

			if (showViewer && mainWindow() != nullptr) MdiChild::create(im.get(), mainWindow()->ui.mdiArea);

			return workspaceApi().addObject(im);
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
				return CWorkspace::instance()->getSelection(types, obj);
			}

			std::list<std::shared_ptr<CBaseObject>> getObjList(std::set<CBaseObject::Type> types, std::shared_ptr<CObject> obj)
			{
				const std::vector<std::shared_ptr<CBaseObject>> objects = workspaceSelectionApi().objects(std::move(types), std::move(obj));
				return std::list<std::shared_ptr<CBaseObject>>(objects.begin(), objects.end());
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
			auto wksp = CWorkspace::instance();

			if (!validateObjectTransferTarget(
				obj,
				newParent,
				"regular object cannot be moved as a descendant of annotation"))
			{
				return;
			}

			const std::shared_ptr<CBaseObject> oldParent = obj->getParentPtr();
			const CTransform sourceParentTransform = oldParent != nullptr
				? CTransform(CBaseObject::getGlobalTransformationMatrix(oldParent))
				: CTransform();

			if (oldParent != nullptr)
			{
				AP::OBJECT::removeChild(oldParent, obj);
			}
			else
			{
				wksp->_objectRemove(obj->id());
			}

			if (keep_pos)
			{
				std::shared_ptr<CModel3D> wrapper = buildTransferWrapper(
					obj,
					"Macierz dopasowania, wygenerowana podczas przenoszenia obiektu");

				if (newParent == nullptr)
				{
					wrapper->setTransform(sourceParentTransform);
					AP::WORKSPACE::addModel(wrapper);
				}
				else
				{
					CTransform targetParentTransform(CBaseObject::getGlobalTransformationMatrix(newParent));
					CTransform finalTransform = CTransform::fromTo(sourceParentTransform, targetParentTransform);
					wrapper->setTransform(finalTransform);
					AP::OBJECT::addChild(newParent, wrapper);
				}
			}
			else
			{
				attachAsRootOrChild(obj, newParent);
			}

			AppStateManager::updateAllViews();
		}

		void copyTo(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> newParent, bool keep_pos)
		{
			if (!validateObjectTransferTarget(
				obj,
				newParent,
				"regular object cannot be copied as a descendant of annotation"))
			{
				return;
			}

			std::shared_ptr<CBaseObject> copy = obj->getCopy();
			if (copy == nullptr)
			{
				return;
			}

			if (keep_pos)
			{
				const CTransform sourceParentTransform = parentGlobalTransform(obj);

				if (newParent == nullptr)
				{
					std::shared_ptr<CModel3D> wrapper = buildTransferWrapper(
						copy,
						"Macierz dopasowania, wygenerowana podczas kopiowania obiektu");
					wrapper->setTransform(sourceParentTransform);
					AP::WORKSPACE::addModel(wrapper);
				}
				else
				{
					CTransform targetParentTransform(CBaseObject::getGlobalTransformationMatrix(newParent));
					CTransform finalTransform = CTransform::fromTo(sourceParentTransform, targetParentTransform);

					if (finalTransform.toQMatrix4x4().isIdentity())
					{
						AP::OBJECT::addChild(newParent, copy);
					}
					else
					{
						std::shared_ptr<CModel3D> wrapper = buildTransferWrapper(
							copy,
							"Macierz dopasowania, wygenerowana podczas kopiowania obiektu");
						wrapper->setTransform(finalTransform);
						AP::OBJECT::addChild(newParent, wrapper);
					}
				}
			}
			else
			{
				attachAsRootOrChild(copy, newParent);
			}
			AppStateManager::updateAllViews();
		}
	}
}








