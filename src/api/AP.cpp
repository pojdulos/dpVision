#include "AP.h"

#include "UI.h"

#include "MainApplication.h"
#include "MainWindow.h"

#include "DockWidgetWorkspace.h"
//#include "DockWidgetPluginList.h"
#include "Annotation.h"
#include "MdiChild.h"
#include "Image.h"
#include "Workspace.h"

#include <QtCore/QString>
#include "StatusBarManager.h"

#include <QElapsedTimer>


namespace AP
{
	CMainApplication& mainApp() {
		//return *CMainApplication::theApp;
		return (CMainApplication&)*QApplication::instance();
	}
	
	CMainWindow& mainWin() { return *CMainWindow::instance(); }
	
	CMainWindow* mainWinPtr() { return CMainWindow::instance(); }

	void leaveSelectionMode()
	{
		CMainWindow *win = CMainWindow::instance();
		if ( win != nullptr )
			win->leaveSelectionMode();
	};

	int getUniqueId()
	{
		int i = AP::mainApp().getUniqueId();
		
		return i;
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


	CWorkspace* getWorkspace(void)
	{
		return CWorkspace::instance();
	}

	const QString& getExeFilePath(void)
	{
		return AP::mainApp().appExecDir();
	}


	int addAnnotation(int parentId, std::shared_ptr<CAnnotation> an)
	{
		auto wksp = CWorkspace::instance();
		return wksp->_objectAdd(an, wksp->getSomethingWithId(parentId));
		//return AP::MODEL::addAnnotation(parentId, an);
	}


	int addAnnotation(std::shared_ptr<CModel3D> obj, std::shared_ptr<CAnnotation> an)
	{
		auto wksp = CWorkspace::instance();
		return wksp->_objectAdd(an, obj);
		//return AP::MODEL::addAnnotation(obj, an);
	}


	namespace PLUGIN {
		bool loadPlugin(const QString &pluginPath)
		{
			return AP::mainApp().loadPlugin(pluginPath);
		}

		void unloadPlugin(const unsigned int id)
		{
			AP::mainApp().unloadPlugin(id);
		}


		PluginInterface* getPlugin(unsigned int id)
		{
			return AP::mainApp().getPlugin(id);
		}

		// PluginInterface* getPlugin(const char* strUUID)
		// {
		// 	return AP::mainApp().getPlugin(strUUID);
		// }

		bool runPlugin(const char* strUUID)
		{
			return AP::mainApp().runPlugin(strUUID);
		}
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

		//int addChild(CModel3D* obj, CBaseObject* child) { return addChild(std::shared_ptr<CModel3D>(obj), std::shared_ptr<CBaseObject>(child)); }

		// int addChild(int parentId, std::shared_ptr<CBaseObject> child)
		// {
		// 	return addChild(AP::WORKSPACE::getModel(parentId), child);
		// }

		// int addChild(int parentId, CBaseObject* child)
		// {
		// 	return addChild(AP::WORKSPACE::getModel(parentId), std::shared_ptr<CBaseObject>(child));
		// }

		int addAnnotation(std::shared_ptr<CModel3D> obj, std::shared_ptr<CAnnotation> an)
		{
			if (an == nullptr) return NO_CURRENT_MODEL;
			if (obj == nullptr) return NO_CURRENT_MODEL;

			auto wksp = CWorkspace::instance();
			return wksp->_objectAdd(an, obj);

			// int id = obj->addAnnotation(an);

			// UI::DOCK::WORKSPACE::addItem(id, obj->id());
			// UI::updateAllViews();
			// return id;
		}

		// int addAnnotation(int parentId, std::shared_ptr<CAnnotation> an)
		// {
		// 	return addAnnotation(AP::WORKSPACE::getModel(parentId), an);
		// }
	}



	namespace WORKSPACE {
		CWorkspace* instance()
		{
			return CWorkspace::instance();
		}

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
				//else
				//{
				//	delete obj;
				//}
			}
			return nullptr;
		}

		void setAllModelsVisible(bool visibility)
		{
			for (std::map<int, std::shared_ptr<CModel3D>>::iterator it = AP::getWorkspace()->begin(); it != AP::getWorkspace()->end(); it++)
			{
				it->second->setSelfVisibility(visibility);
				it->second->setKidsVisibility(visibility);

				UI::DOCK::WORKSPACE::setItemVisibleById(it->first, visibility);
				UI::DOCK::WORKSPACE::setItemKidsVisibleById(it->first, visibility);
			}
			UI::changeMenuAfterSelect();
			UI::DOCK::PROPERTIES::updateProperties();
			UI::updateAllViews();
		}

		bool addModel(std::shared_ptr<CModel3D> obj, bool setItCurrent)
		{
			auto wksp = CWorkspace::instance();
			auto result = wksp->_objectAdd(obj);
			if (setItCurrent) wksp->_objectActivate(obj->id());
			return result!=-1;
			// if (AP::getWorkspace()->_addModel(obj))
			// {
			// 	UI::DOCK::WORKSPACE::addItem(obj);

			// 	int id = (setItCurrent) ? obj->id() : -1;

			// 	if (id != AP::WORKSPACE::getCurrentModelId())
			// 	{
			// 		AP::WORKSPACE::setCurrentModel(id);

			// 		UI::DOCK::WORKSPACE::selectItem(id);
			// 		UI::DOCK::PROPERTIES::selectionChanged(id);

			// 		UI::changeMenuAfterSelect();
			// 	}
			// 	UI::updateAllViews();
			// 	return true;
			// }
			// return false;
		}

		bool addModel(CModel3D* obj, bool setItCurrent) { return addModel(std::shared_ptr<CModel3D>(obj), setItCurrent); }

		bool addObject(std::shared_ptr<CBaseObject> obj, bool setItCurrent)
		{
			auto wksp = CWorkspace::instance();
			auto result = wksp->_objectAdd(obj);
			if (setItCurrent) wksp->_objectActivate(obj->id());
			return result!=-1;

		// 	if (obj->hasType(CBaseObject::Type::MODEL))
		// 	{
		// 		addModel(std::dynamic_pointer_cast<CModel3D>(obj), setItCurrent);
		// 		return true;
		// 	}
		// 	else if (obj->hasType(CBaseObject::Type::IMAGE))
		// 	{
		// 		addImage(std::dynamic_pointer_cast<CImage>(obj), false, true);
		// 		return true;
		// 	}
		// 	else
		// 	{
		// 		std::shared_ptr<CModel3D> mdl = std::make_shared<CModel3D>();
		// 		if (obj->hasCategory(CBaseObject::Category::OBJECT))
		// 		{
		// 			mdl->addChild(obj);
		// 			mdl->importChildrenGeometry();
		// 			addModel(mdl, setItCurrent);
		// 			return true;
		// 		}
		// 		else if (obj->hasCategory(CBaseObject::Category::ANNOTATION))
		// 		{
		// 			mdl->addAnnotation(std::dynamic_pointer_cast<CAnnotation>(obj));
		// 			addModel(mdl, setItCurrent);
		// 			return true;
		// 		}
		// 	}
		// 	return false;
		}

		bool addImage(std::shared_ptr<CImage> im, bool showViewer, bool show3d)
		{
			im->setSelfVisibility(show3d);

			if (showViewer) MdiChild::create(im.get(), AP::mainWin().ui.mdiArea);

			return AP::WORKSPACE::addModel(std::dynamic_pointer_cast<CModel3D>(im));
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
			UI::DOCK::PROPERTIES::selectionChanged(NO_CURRENT_MODEL);
			AP::getWorkspace()->_removeAllModels();
			UI::DOCK::WORKSPACE::rebuildTree();
			UI::updateAllViews();

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
			return getWorkspace()->_getModel(id);
		}

		std::shared_ptr<CModel3D> getCurrentModel()
		{
			return getWorkspace()->_getModel( getWorkspace()->_getCurrentModelId() );
		}

		int getCurrentModelId()
		{
			return getWorkspace()->_getCurrentModelId();
		}

		size_t size()
		{
			return getWorkspace()->size();
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

				AP::getWorkspace()->addToSelection(id);
				UI::DOCK::WORKSPACE::setItemCheckedById(id, true);
				UI::DOCK::PROPERTIES::updateProperties();
				UI::updateAllViews();
			}
			void unselectModel(int id)
			{
				//CModel3D *obj = AP::getWorkspace()->_getModel(id);

				AP::getWorkspace()->removeFromSelection(id);
				UI::DOCK::WORKSPACE::setItemCheckedById(id, false);
				UI::DOCK::PROPERTIES::updateProperties();
				UI::updateAllViews();
			}

			bool isModelSelected(int id)
			{
				return AP::getWorkspace()->inSelection(id);
			}

			void clear()
			{
				AP::getWorkspace()->clearSelection();
			}

			std::list<int> getList()
			{
				return AP::getWorkspace()->getSelection();
			}

			std::list<int> getList(std::set<CBaseObject::Type> types, std::shared_ptr<CObject> obj)
			{
				return AP::getWorkspace()->getSelection(types, obj);
			}

			std::list<std::shared_ptr<CBaseObject>> getObjList(std::set<CBaseObject::Type> types, std::shared_ptr<CObject> obj)
			{
				return AP::getWorkspace()->getSelected(types, obj);
			}

			void setModelsVisible(bool visibility)
			{
				std::list<int> sel = AP::getWorkspace()->getSelection();
				for (std::list<int>::iterator it = sel.begin(); it != sel.end(); it++)
				{
					std::shared_ptr<CModel3D> m = AP::WORKSPACE::getModel(*it);
					if (m != nullptr)
					{
						m->setSelfVisibility(visibility);
						UI::DOCK::WORKSPACE::setItemVisibleById(*it, visibility);
					}
				}

				UI::changeMenuAfterSelect();
				UI::DOCK::PROPERTIES::updateProperties();
				UI::updateAllViews();
			}
		}
	}

	namespace EVENTS {
		void modelIndicationEvent( int objId )
		{
			bool usedInPlugin = false;

			if (nullptr != AP::mainApp().activePlugin)
				usedInPlugin = AP::mainApp().activePlugin->onModelIndication(objId);

			if (!usedInPlugin)
			{
				AP::WORKSPACE::setCurrentModel(objId);

				if (NO_CURRENT_MODEL != objId)
				{
					StatusBarManager::setText(QString("Selected (%1) %2").arg(objId).arg(AP::WORKSPACE::getCurrentModel()->getLabel()));
				}
				else
				{
					StatusBarManager::setText(QString("None selected (%1)").arg(objId));
				}

				//UI::DOCK::WORKSPACE::selectItem(objId);
				//UI::DOCK::PROPERTIES::selectionChanged();
				//UI::changeMenuAfterSelect();
			}
		}

		DPVISION_EXPORT void workspaceTreeClicked( int objId )
		{
			bool usedInPlugin = false;

			if (nullptr != AP::mainApp().activePlugin)
				usedInPlugin = AP::mainApp().activePlugin->onModelIndication(objId);

			if (!usedInPlugin)
			{
				//DO SOMETHING ELSE
				;
			}
		}
	}
}

#include "Histogram.h"


bool AP::OBJECT::remove(std::shared_ptr<CBaseObject> obj) { return CWorkspace::instance()->_objectRemove(obj); };
bool AP::OBJECT::removeChild(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> child) { return CWorkspace::instance()->_objectRemove(child); };


int AP::OBJECT::addChild(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> child)
{
	if (child == nullptr) return NO_CURRENT_MODEL;
	if (obj == nullptr) return NO_CURRENT_MODEL;

	auto wksp = CWorkspace::instance();
	auto result = wksp->_objectAdd(child, obj);
	return result;

	// int id = NO_CURRENT_MODEL;

	// if (obj->hasCategory(CBaseObject::Category::ANNOTATION))
	// {
	// 	if (child->hasCategory(CBaseObject::Category::ANNOTATION))
	// 	{
	// 		id = std::dynamic_pointer_cast<CAnnotation>(obj)->addAnnotation(std::dynamic_pointer_cast<CAnnotation>(child));
	// 	}
	// }
	// else if (obj->hasCategory(CBaseObject::Category::OBJECT))
	// {
	// 	if (child->hasCategory(CBaseObject::Category::ANNOTATION))
	// 	{
	// 		id = std::dynamic_pointer_cast<CObject>(obj)->addAnnotation(std::dynamic_pointer_cast<CAnnotation>(child));
	// 	}
	// 	else if (child->hasCategory(CBaseObject::Category::OBJECT))
	// 	{
	// 		id = std::dynamic_pointer_cast<CObject>(obj)->addChild(child);
	// 	}
	// }

	// if (id > 0)
	// {
	// 	UI::DOCK::WORKSPACE::addItem(child);
	// 	UI::updateAllViews();
	// }

	// return id;
}


void AP::OBJECT::moveTo(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> newParent, bool keep_pos)
{
	auto wksp = CWorkspace::instance();

	if (obj != nullptr)
	{
		if ((newParent != nullptr) && newParent->hasCategory(CBaseObject::Category::ANNOTATION) && obj->hasCategory(CBaseObject::Category::OBJECT))
		{
			UI::MESSAGEBOX::error("regular object cannot be moved as a descendant of annotation");
			return;
		}

		std::shared_ptr<CBaseObject> oldParent = obj->getParentPtr();

		CTransform t0;

		if (oldParent != nullptr)
		{
			t0 = CTransform(oldParent->getGlobalTransformationMatrix());
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
				newmodel->addChild(obj);
				newmodel->importChildrenGeometry();
			}
			else if (obj->hasCategory(CBaseObject::ANNOTATION))
			{
				newmodel->addAnnotation(std::dynamic_pointer_cast<CAnnotation>(obj));
			}

			if (newParent == nullptr) // copyToNew
			{
				newmodel->setTransform(t0);

				AP::WORKSPACE::addModel(newmodel);
			}
			else // copyTo existing
			{
				CTransform t1(newParent->getGlobalTransformationMatrix());
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

		UI::updateAllViews();
	}
}

void AP::OBJECT::copyTo(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> newParent, bool keep_pos)
{
	if (obj != nullptr)
	{
		if ((newParent != nullptr) && newParent->hasCategory(CBaseObject::Category::ANNOTATION) && obj->hasCategory(CBaseObject::Category::OBJECT))
		{
			UI::MESSAGEBOX::error("regular object cannot be copied as a descendant of annotation");
			return;
		}

		std::shared_ptr<CBaseObject> kopia = obj->getCopy();

		if (keep_pos)
		{
			CTransform t0;
			if (obj->getParent() != nullptr)
				t0 = CTransform(obj->getParent()->getGlobalTransformationMatrix());

			std::shared_ptr<CModel3D> newmodel = std::make_shared<CModel3D>();
			newmodel->setLabel("<=>");
			newmodel->setDescr("Macierz dopasowania, wygenerowana podczas kopiowania obiektu");

			if (newParent == nullptr) // copyToNew
			{
				if (kopia->hasCategory(CBaseObject::OBJECT))
				{
					newmodel->addChild(kopia);
					newmodel->importChildrenGeometry();
					newmodel->setTransform(t0);
					AP::WORKSPACE::addModel(newmodel);
				}
				else if (kopia->hasCategory(CBaseObject::ANNOTATION))
				{
					newmodel->addAnnotation(std::dynamic_pointer_cast<CAnnotation>(kopia));
					newmodel->setTransform(t0);
					AP::WORKSPACE::addModel(newmodel);
				}
			}
			else // copyTo existing
			{
				CTransform t1(newParent->getGlobalTransformationMatrix());
				CTransform ft = CTransform::fromTo(t0, t1);

				if (ft.toQMatrix4x4().isIdentity())
				{
					AP::OBJECT::addChild(newParent, kopia);
					//delete newmodel;
				}
				else
				{
					newmodel->addChild(kopia);
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
		UI::updateAllViews();
	}
}



