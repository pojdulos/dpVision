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
		auto wksp = CWorkspace::instance();
		return wksp->_objectAdd(an, wksp->getSomethingWithId(parentId));
	}


	int addAnnotation(std::shared_ptr<CModel3D> obj, std::shared_ptr<CAnnotation> an)
	{
		auto wksp = CWorkspace::instance();
		return wksp->_objectAdd(an, obj);
	}

	namespace MODEL {
		std::shared_ptr<CModel3D> load(std::wstring path, bool synchronous)
		{
			return CModel3D::load(path, synchronous);
		}

		void removeChild(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> child) { CWorkspace::instance()->_objectRemove(child); }
		void removeChild(int parentid, int childid) { CWorkspace::instance()->_objectRemove(childid); }
		void removeAnnotation(int parentid, int id) { CWorkspace::instance()->_objectRemove(id); }


		int addChild(std::shared_ptr<CModel3D> obj, std::shared_ptr<CBaseObject> child)
		{
			if (child == nullptr) return NO_CURRENT_MODEL;
			if (obj == nullptr) return NO_CURRENT_MODEL;

			auto wksp = CWorkspace::instance();
			return wksp->_objectAdd(child, obj);
		}
	
		int addAnnotation(std::shared_ptr<CModel3D> obj, std::shared_ptr<CAnnotation> an)
		{
			if (an == nullptr) return NO_CURRENT_MODEL;
			if (obj == nullptr) return NO_CURRENT_MODEL;

			auto wksp = CWorkspace::instance();
			return wksp->_objectAdd(an, obj);
		}
	}



	namespace WORKSPACE {
		std::shared_ptr<CModel3D> loadModel(const QString fext, const QString& path, bool synchronous, bool setItCurrent)
		{
			std::shared_ptr<CModel3D> obj = CModel3D::load(fext, path, synchronous);

			if (nullptr != obj)
			{
				if (AP::WORKSPACE::addModel(obj, setItCurrent))
				{
					return obj;
				}
			}
			return nullptr;
		}

		std::shared_ptr<CModel3D> loadModel(const QString& path, bool synchronous, bool setItCurrent, std::shared_ptr<IProgressListener> prg)
		{
			std::shared_ptr<CModel3D> obj = CModel3D::load(path, synchronous, prg);

			if (nullptr != obj)
			{
				if (AP::WORKSPACE::addModel(obj, setItCurrent))
				{
					return obj;
				}
			}
			return nullptr;
		}

		std::shared_ptr<CModel3D> loadModel(const std::wstring& path, bool synchronous, bool setItCurrent )
		{
			std::shared_ptr<CModel3D> obj = CModel3D::load(path, synchronous);

			if (nullptr != obj)
			{
				if (AP::WORKSPACE::addModel(obj, setItCurrent))
				{
					return obj;
				}
			}
			return nullptr;
		}

		void setAllModelsVisible(bool visibility)
		{
			for (std::map<int, std::shared_ptr<CModel3D>>::iterator it = CWorkspace::instance()->begin(); it != CWorkspace::instance()->end(); it++)
			{
				it->second->setSelfVisibility(visibility);
				it->second->setKidsVisibility(visibility);

				WorkspacePanelManager::setWorkspaceItemVisible(it->first, visibility);
				WorkspacePanelManager::setWorkspaceItemKidsVisible(it->first, visibility);
			}
			AppStateManager::changeMenuAfterSelect();
			AppStateManager::updateProperties();
			AppStateManager::updateAllViews();
		}

		bool addModel(std::shared_ptr<CModel3D> obj, bool setItCurrent)
		{
			auto wksp = CWorkspace::instance();
			auto result = wksp->_objectAdd(obj);
			if (setItCurrent) wksp->_objectActivate(obj->id());
			return result!=-1;
		}

		bool addObject(std::shared_ptr<CBaseObject> obj, bool setItCurrent)
		{
			auto wksp = CWorkspace::instance();
			auto result = wksp->_objectAdd(obj);
			if (setItCurrent) wksp->_objectActivate(obj->id());
			return result!=-1;
		}

		bool addImage(std::shared_ptr<CImage> im, bool showViewer, bool show3d)
		{
			im->setSelfVisibility(show3d);

			if (showViewer && mainWindow() != nullptr) MdiChild::create(im.get(), mainWindow()->ui.mdiArea);

			auto wksp = CWorkspace::instance();
			auto result = wksp->_objectAdd(im);
			return result!=-1;
		}


		bool removeModel(int id) { return CWorkspace::instance()->_objectRemove(id); };
		bool removeModel(std::shared_ptr<CModel3D> obj) { return CWorkspace::instance()->_objectRemove(obj); };

		bool removeImage(int id) { return CWorkspace::instance()->_objectRemove(id); }
		bool removeImage(std::shared_ptr<CImage> im) { return CWorkspace::instance()->_objectRemove(im); };


		bool removeCurrentModel()
		{
			auto wksp = CWorkspace::instance();

			int id = wksp->_getCurrentModelId();

			if (NO_CURRENT_MODEL == id) return false;

			return wksp->_objectRemove(id);
		}

		bool removeAllModels()
		{
			WorkspacePanelManager::propertiesSelectionChanged(NO_CURRENT_MODEL);
			CWorkspace::instance()->_removeAllModels();
			WorkspacePanelManager::rebuildWorkspaceTree();
			AppStateManager::updateAllViews();

			return true;
		}

		bool removeSelectedModels()
		{
			auto wksp = CWorkspace::instance();
			std::list<int> sel = wksp->getSelection();
			for (std::list<int>::reverse_iterator it = sel.rbegin(); it != sel.rend(); it++)
			{
				wksp->_objectRemove(*it);
			}
			return true;
		}

		int setCurrentModel( int id )
		{
			auto wksp = CWorkspace::instance();
			
			wksp->_objectActivate(id);
			return wksp->_getCurrentModelId();
		}

		std::shared_ptr<CModel3D> duplicateModel(std::shared_ptr<CModel3D> orginal)
		{
			if (nullptr != orginal)
			{
				std::shared_ptr<CModel3D> kopia = std::dynamic_pointer_cast<CModel3D>(orginal->getCopy());

				if (nullptr != kopia)
				{
					if (AP::WORKSPACE::addModel(kopia))
					{
						return kopia;
					}
					//else
					//{
					//	delete kopia;
					//}
				}
			}
			return nullptr;
		}

		std::shared_ptr<CModel3D> duplicateModel(int id)
		{
			return AP::WORKSPACE::duplicateModel(AP::WORKSPACE::getModel(id));
		}

		std::shared_ptr<CModel3D> duplicateCurrentModel()
		{
			return AP::WORKSPACE::duplicateModel(AP::WORKSPACE::getCurrentModel());
		}

		std::shared_ptr<CModel3D> getModel(int id)
		{
			return CWorkspace::instance()->_getModel(id);
		}

		std::shared_ptr<CModel3D> getCurrentModel()
		{
			return CWorkspace::instance()->_getModel( CWorkspace::instance()->_getCurrentModelId() );
		}

		int getCurrentModelId()
		{
			return CWorkspace::instance()->_getCurrentModelId();
		}

		size_t size()
		{
			return CWorkspace::instance()->size();
		}

		std::shared_ptr<CBaseObject> findId(int id)
		{
			return CWorkspace::instance()->getSomethingWithId(id);
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
				//CModel3D *obj = AP::getWorkspace()->_getModel(id);

				CWorkspace::instance()->addToSelection(id);
				WorkspacePanelManager::setWorkspaceItemChecked(id, true);
				AppStateManager::updateProperties();
				AppStateManager::updateAllViews();
			}
			void unselectModel(int id)
			{
				//CModel3D *obj = AP::getWorkspace()->_getModel(id);

				CWorkspace::instance()->removeFromSelection(id);
				WorkspacePanelManager::setWorkspaceItemChecked(id, false);
				AppStateManager::updateProperties();
				AppStateManager::updateAllViews();
			}

			bool isModelSelected(int id)
			{
				return CWorkspace::instance()->inSelection(id);
			}

			void clear()
			{
				CWorkspace::instance()->clearSelection();
			}

			std::list<int> getList()
			{
				return CWorkspace::instance()->getSelection();
			}

			std::list<int> getList(std::set<CBaseObject::Type> types, std::shared_ptr<CObject> obj)
			{
				return CWorkspace::instance()->getSelection(types, obj);
			}

			std::list<std::shared_ptr<CBaseObject>> getObjList(std::set<CBaseObject::Type> types, std::shared_ptr<CObject> obj)
			{
				return CWorkspace::instance()->getSelected(types, obj);
			}

			void setModelsVisible(bool visibility)
			{
				std::list<int> sel = CWorkspace::instance()->getSelection();
				for (std::list<int>::iterator it = sel.begin(); it != sel.end(); it++)
				{
					std::shared_ptr<CModel3D> m = AP::WORKSPACE::getModel(*it);
					if (m != nullptr)
					{
						m->setSelfVisibility(visibility);
						WorkspacePanelManager::setWorkspaceItemVisible(*it, visibility);
					}
				}

				AppStateManager::changeMenuAfterSelect();
				AppStateManager::updateProperties();
				AppStateManager::updateAllViews();
			}
		}
	}

	namespace OBJECT {
		bool remove(std::shared_ptr<CBaseObject> obj) { return CWorkspace::instance()->_objectRemove(obj); };
		bool removeChild(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> child) { return CWorkspace::instance()->_objectRemove(child); };

		int addChild(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> child)
		{
			if (child == nullptr) return NO_CURRENT_MODEL;
			if (obj == nullptr) return NO_CURRENT_MODEL;

			auto wksp = CWorkspace::instance();
			auto result = wksp->_objectAdd(child, obj);
			return result;
		}

		void moveTo(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> newParent, bool keep_pos)
		{
			auto wksp = CWorkspace::instance();

			if (obj != nullptr)
			{
				if ((newParent != nullptr) && newParent->hasCategory(CBaseObject::Category::ANNOTATION) && obj->hasCategory(CBaseObject::Category::OBJECT))
				{
					MessageBoxManager::error("regular object cannot be moved as a descendant of annotation");
					return;
				}

				std::shared_ptr<CBaseObject> oldParent = obj->getParentPtr();

				CTransform t0;

				if (oldParent != nullptr)
				{
					t0 = CTransform(CBaseObject::getGlobalTransformationMatrix(oldParent));
					AP::OBJECT::removeChild(oldParent, obj);
				}
				else
				{
					wksp->_objectRemove(obj->id());
				}

				if (keep_pos)
				{
					std::shared_ptr<CModel3D> newmodel = std::make_shared<CModel3D>();
					newmodel->setLabel("<=>");
					newmodel->setDescr("Macierz dopasowania, wygenerowana podczas przenoszenia obiektu");

					if (obj->hasCategory(CBaseObject::OBJECT))
					{
						newmodel->addChild(newmodel, obj);
						newmodel->importChildrenGeometry();
					}
					else if (obj->hasCategory(CBaseObject::ANNOTATION))
					{
						newmodel->addAnnotation(newmodel, std::dynamic_pointer_cast<CAnnotation>(obj));
					}

					if (newParent == nullptr) // copyToNew
					{
						newmodel->setTransform(t0);

						AP::WORKSPACE::addModel(newmodel);
					}
					else // copyTo existing
					{
						CTransform t1(CBaseObject::getGlobalTransformationMatrix(newParent));
						CTransform ft = CTransform::fromTo(t0, t1);

						newmodel->setTransform(ft);

						AP::OBJECT::addChild(newParent, newmodel);
						//((CObject*)newParent)->importChildrenGeometry();
					}
				}
				else
				{
					if (newParent == nullptr) // moveToNew
					{
						AP::WORKSPACE::addObject(obj);
					}
					else // moveTo existing
					{
						AP::OBJECT::addChild(newParent, obj);
					}
				}

				AppStateManager::updateAllViews();
			}
		}

		void copyTo(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> newParent, bool keep_pos)
		{
			if (obj != nullptr)
			{
				if ((newParent != nullptr) && newParent->hasCategory(CBaseObject::Category::ANNOTATION) && obj->hasCategory(CBaseObject::Category::OBJECT))
				{
					MessageBoxManager::error("regular object cannot be copied as a descendant of annotation");
					return;
				}

				std::shared_ptr<CBaseObject> kopia = obj->getCopy();

				if (keep_pos)
				{
					CTransform t0;
					if (obj->getParent() != nullptr)
						t0 = CTransform(CBaseObject::getGlobalTransformationMatrix(obj->getParentPtr()));

					std::shared_ptr<CModel3D> newmodel = std::make_shared<CModel3D>();
					newmodel->setLabel("<=>");
					newmodel->setDescr("Macierz dopasowania, wygenerowana podczas kopiowania obiektu");

					if (newParent == nullptr) // copyToNew
					{
						if (kopia->hasCategory(CBaseObject::OBJECT))
						{
							newmodel->addChild(newmodel, kopia);
							newmodel->importChildrenGeometry();
							newmodel->setTransform(t0);
							AP::WORKSPACE::addModel(newmodel);
						}
						else if (kopia->hasCategory(CBaseObject::ANNOTATION))
						{
							newmodel->addAnnotation(newmodel, std::dynamic_pointer_cast<CAnnotation>(kopia));
							newmodel->setTransform(t0);
							AP::WORKSPACE::addModel(newmodel);
						}
					}
					else // copyTo existing
					{
						CTransform t1(CBaseObject::getGlobalTransformationMatrix(newParent));
						CTransform ft = CTransform::fromTo(t0, t1);

						if (ft.toQMatrix4x4().isIdentity())
						{
							AP::OBJECT::addChild(newParent, kopia);
							//delete newmodel;
						}
						else
						{
							newmodel->addChild(newmodel, kopia);
							newmodel->importChildrenGeometry();
							newmodel->setTransform(ft);

							AP::OBJECT::addChild(newParent, newmodel);
						}

						//((CObject*)newParent)->importChildrenGeometry();
					}
				}
				else
				{
					if (newParent == nullptr) // copyToNew
					{
						AP::WORKSPACE::addObject(kopia);
					}
					else // copyTo existing
					{
						AP::OBJECT::addChild(newParent, kopia);
					}
				}
				AppStateManager::updateAllViews();
			}
		}
	}
}








