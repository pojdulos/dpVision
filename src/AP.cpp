#include "AP.h"

#include "MainApplication.h"
#include "MainWindow.h"

#include "DockWidgetWorkspace.h"
//#include "DockWidgetPluginList.h"
#include "Annotation.h"
#include "MdiChild.h"
#include "Image.h"

#include <QtCore/QString>

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

	void processEvents( bool immediate )
	{
		static unsigned long t475634 = 0;
		unsigned long t1;

		t1 = GetTickCount();
		
		//processEvents calls slows down the program, so no more than once per second
		if ( immediate || ((int)(t1 - t475634) > 1000) )
		{
			QCoreApplication::processEvents();
			t475634 = t1;
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


	int addAnnotation(int parentId, CAnnotation *an)
	{
		return AP::MODEL::addAnnotation(parentId, an);
	}


	int addAnnotation(CModel3D* obj, CAnnotation *an)
	{
		return AP::MODEL::addAnnotation(obj, an);
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
		CModel3D * load(std::wstring path, bool synchronous)
		{
			return CModel3D::load(path, synchronous);
		}


		int addChild(CModel3D* obj, CBaseObject* child)
		{
			if (child == nullptr) return NO_CURRENT_MODEL;
			if (obj == nullptr) return NO_CURRENT_MODEL;

			int id = -1;
			if (child->hasCategory(CBaseObject::Category::ANNOTATION))
			{
				id = obj->addAnnotation((CAnnotation*)child);
			}
			else if (child->hasCategory(CBaseObject::Category::OBJECT))
			{
				id = obj->addChild(child);
			}

			UI::DOCK::WORKSPACE::addItem(id, obj->id());
			UI::updateAllViews();
			return id;
		}

		int addChild(int parentId, CBaseObject* child)
		{
			return addChild(AP::WORKSPACE::getModel(parentId), child);
		}

		int addAnnotation(CModel3D* obj, CAnnotation *an)
		{
			if (an == nullptr) return NO_CURRENT_MODEL;
			if (obj == nullptr) return NO_CURRENT_MODEL;

			int id = obj->addAnnotation(an);

			UI::DOCK::WORKSPACE::addItem(id, obj->id());
			UI::updateAllViews();
			return id;
		}

		int addAnnotation(int parentId, CAnnotation* an)
		{
			return addAnnotation(AP::WORKSPACE::getModel(parentId), an);
		}

		void removeChild(CBaseObject *obj, CBaseObject *child)
		{
			if (child == nullptr) return;
			if (obj == nullptr) return;
			if (child->getParent() != obj) return;

			if (obj->hasCategory(CBaseObject::Category::OBJECT))
			{
				if (child->hasCategory(CBaseObject::Category::OBJECT))
				{
					((CObject*)obj)->removeChild(child->id());
				}
				else if (child->hasCategory(CBaseObject::Category::ANNOTATION))
				{
					((CObject*)obj)->removeAnnotation(child->id());
				}
			}
			else if (obj->hasCategory(CBaseObject::Category::ANNOTATION))
			{
				((CAnnotation*)obj)->removeAnnotation(child->id());
			}
			else
			{
				return;
			}

			UI::DOCK::WORKSPACE::removeItem(child->id());
			UI::DOCK::PROPERTIES::selectionChanged(obj->id());
			delete child;

			UI::updateAllViews();
		}

		void removeChild(int parentid, int childid)
		{
			CBaseObject* obj = AP::WORKSPACE::findId(parentid);
			CBaseObject* child = AP::WORKSPACE::findId(childid);

			removeChild(obj, child);
		}

		void removeAnnotation(int parentid, int id)
		{
			UI::DOCK::WORKSPACE::removeItem(id);
			UI::DOCK::PROPERTIES::selectionChanged(parentid);

			CBaseObject* an = AP::WORKSPACE::findId(id);

			if (an == nullptr) return;

			CBaseObject* parent = an->getParent();
			
			if (parent != nullptr)
			{
				if (parent->hasType(CBaseObject::MODEL))
				{
					((CModel3D*)parent)->removeAnnotation(id);
				}
				else if (parent->hasCategory(CBaseObject::ANNOTATION))
				{
					((CAnnotation*)parent)->removeAnnotation(id);
				}
			}
			delete an;

			UI::updateAllViews();
		}
	}


	namespace WORKSPACE {
		CWorkspace* instance()
		{
			return CWorkspace::instance();
		}

		CModel3D* loadModel(const QString fext, const QString& path, bool synchronous, bool setItCurrent)
		{
			CModel3D* obj = CModel3D::load(fext, path, synchronous);

			if (nullptr != obj)
			{
				if (AP::WORKSPACE::addModel(obj, setItCurrent))
				{
					return obj;
				}
				else
				{
					delete obj;
				}
			}
			return nullptr;
		}

		CModel3D* loadModel(const QString& path, bool synchronous, bool setItCurrent)
		{
			CModel3D* obj = CModel3D::load(path, synchronous);

			if (nullptr != obj)
			{
				if (AP::WORKSPACE::addModel(obj, setItCurrent))
				{
					return obj;
				}
				else
				{
					delete obj;
				}
			}
			return nullptr;
		}

		CModel3D * loadModel(const std::wstring& path, bool synchronous, bool setItCurrent )
		{
			CModel3D *obj = CModel3D::load(path, synchronous);

			if (nullptr != obj)
			{
				if (AP::WORKSPACE::addModel(obj, setItCurrent))
				{
					return obj;
				}
				else
				{
					delete obj;
				}
			}
			return nullptr;
		}

		void setAllModelsVisible(bool visibility)
		{
			for (std::map<int, CModel3D*>::iterator it = AP::getWorkspace()->begin(); it != AP::getWorkspace()->end(); it++)
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

		bool addModel( CModel3D *obj, bool setItCurrent )
		{
			if ( AP::getWorkspace()->_addModel(obj) )
			{
				UI::DOCK::WORKSPACE::addItem(obj->id());
				
				if (setItCurrent) {
					AP::WORKSPACE::setCurrentModel(obj->id());
				}
				else
				{
					UI::DOCK::WORKSPACE::selectItem(obj->id() );
					UI::DOCK::PROPERTIES::selectionChanged(obj->id());
					UI::changeMenuAfterSelect();
					UI::updateAllViews();
				}
				return true;
			}
			return false;
		}

		bool addImage(CImage* im, bool showViewer, bool show3d)
		{
			im->setSelfVisibility(show3d);

			if (showViewer) MdiChild::create(im, AP::mainWin().ui.mdiArea);

			return AP::WORKSPACE::addModel((CModel3D*)im);
		}

		bool removeImage(int id, bool deleteIt)
		{
			AP::mainWin().closePicViewers(id);
			return removeModel(id, deleteIt);
		}

		bool removeImage(CImage* im, bool deleteIt)
		{
			AP::mainWin().closePicViewers(im->id());
			return removeModel(im->id(), deleteIt);
		}

		bool removeModel( int id, bool deleteIt)
		{
			UI::DOCK::WORKSPACE::removeItem(id);

			if (getWorkspace()->_removeModel(id, deleteIt))
			{
				CModel3D *obj = AP::WORKSPACE::getCurrentModel();
				if (nullptr != obj)
				{
					UI::DOCK::WORKSPACE::selectItem(obj->id());
					UI::DOCK::PROPERTIES::selectionChanged(obj->id());
				}
				else
				{
					UI::DOCK::WORKSPACE::selectItem(NO_CURRENT_MODEL);
					UI::DOCK::PROPERTIES::selectionChanged(NO_CURRENT_MODEL);
				}
					
				UI::changeMenuAfterSelect();
				UI::updateAllViews();
			}

			return false;
		}

		bool removeModel(CModel3D* obj, bool deleteIt)
		{
			return removeModel(obj->id(), deleteIt);
		}

		bool removeCurrentModel()
		{
			int id = AP::WORKSPACE::getCurrentModelId();

			if (NO_CURRENT_MODEL == id) return false;

			return AP::WORKSPACE::removeModel(id);
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
			std::list<int> sel = AP::getWorkspace()->getSelection();
			for (std::list<int>::reverse_iterator it = sel.rbegin(); it != sel.rend(); it++)
			{
				AP::WORKSPACE::removeModel(*it);
			}
			return true;
		}

		int setCurrentModel( int id )
		{
			int newId = getWorkspace()->_setCurrentModel(id);

			//UI::DOCK::WORKSPACE::selectItem( id );
			//UI::DOCK::PROPERTIES::selectionChanged( id );
			//UI::changeMenuAfterSelect();

			//UI::updateAllViews();
			return newId;
		}

		CModel3D * duplicateModel(CModel3D * orginal)
		{
			if (nullptr != orginal)
			{
				CModel3D *kopia = orginal->getCopy();

				if (nullptr != kopia)
				{
					if (AP::WORKSPACE::addModel(kopia))
					{
						return kopia;
					}
					else
					{
						delete kopia;
					}
				}
			}
			return nullptr;
		}

		CModel3D * duplicateModel(int id)
		{
			return AP::WORKSPACE::duplicateModel( AP::WORKSPACE::getModel(id) );
		}

		CModel3D * duplicateCurrentModel()
		{
			return AP::WORKSPACE::duplicateModel( AP::WORKSPACE::getCurrentModel() );
		}

		CModel3D * getModel( int id )
		{
			return getWorkspace()->_getModel( id );
		}

		CModel3D * getCurrentModel()
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

		CBaseObject* findId(int id)
		{
			return getWorkspace()->getSomethingWithId(id);
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

			std::list<int> getList(std::set<CBaseObject::Type> types, CObject* obj)
			{
				return AP::getWorkspace()->getSelection(types, obj);
			}

			std::list<CBaseObject*> getObjList(std::set<CBaseObject::Type> types, CObject* obj)
			{
				return AP::getWorkspace()->getSelected(types, obj);
			}

			void setModelsVisible(bool visibility)
			{
				std::list<int> sel = AP::getWorkspace()->getSelection();
				for (std::list<int>::iterator it = sel.begin(); it != sel.end(); it++)
				{
					CModel3D* m = AP::WORKSPACE::getModel(*it);
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
					UI::STATUSBAR::printf(L"Selected (%d) %s", objId, AP::WORKSPACE::getCurrentModel()->getLabelW().c_str());
				}
				else
				{
					UI::STATUSBAR::printf(L"None selected (%d)", objId);
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

bool AP::OBJECT::remove(CBaseObject* obj, bool deleteIt)
{
	if (obj == nullptr) return false;

	bool isRemoved = false;

	CBaseObject* parent = obj->getParent();
	if (parent == nullptr)
	{
		if (obj == AP::WORKSPACE::getModel(obj->id()))
		{
			if (obj->hasType(CObject::Type::MODEL))
				return AP::WORKSPACE::removeModel(obj->id(), deleteIt);
			else if (obj->hasType(CObject::Type::IMAGE))
				return AP::WORKSPACE::removeImage(obj->id(), deleteIt);
		}
	}
	else
	{
		if (obj->hasType(CAnnotation::HISTOGRAM))
		{
			CHistogram* h = (CHistogram*) obj;

			if (UI::DOCK::HISTOGRAM::getHistogram() == h)
			{
				h->setSelfVisibility(false);
				UI::DOCK::HISTOGRAM::show(false);
				UI::DOCK::HISTOGRAM::setHistogram(NULL);
			}
		}

		return AP::OBJECT::removeChild(parent, obj, deleteIt);
	}

	return false;
}

int AP::OBJECT::addChild(CBaseObject* obj, CBaseObject* child)
{
	if (child == nullptr) return NO_CURRENT_MODEL;
	if (obj == nullptr) return NO_CURRENT_MODEL;

	int id = NO_CURRENT_MODEL;

	if (obj->hasCategory(CBaseObject::Category::ANNOTATION))
	{
		if (child->hasCategory(CBaseObject::Category::ANNOTATION))
		{
			id = ((CAnnotation*)obj)->addAnnotation((CAnnotation*)child);
		}
	}
	else if (obj->hasCategory(CBaseObject::Category::OBJECT))
	{
		if (child->hasCategory(CBaseObject::Category::ANNOTATION))
		{
			id = ((CObject*)obj)->addAnnotation((CAnnotation*)child);
		}
		else if (child->hasCategory(CBaseObject::Category::OBJECT))
		{
			id = ((CObject*)obj)->addChild(child);
		}
	}

	if (id > 0)
	{
		UI::DOCK::WORKSPACE::addItem(child);
		UI::updateAllViews();
	}

	return id;
}

bool AP::OBJECT::removeChild(CBaseObject* obj, CBaseObject* child, bool deleteIt)
{
	if (child == nullptr) return false;
	if (obj == nullptr) return false;
	if (child->getParent() != obj) return false;

	bool isRemoved = false;

	if (obj->hasCategory(CBaseObject::Category::OBJECT))
	{
		if (child->hasCategory(CBaseObject::Category::OBJECT))
		{
			isRemoved = ((CObject*)obj)->removeChild(child->id());
		}
		else if (child->hasCategory(CBaseObject::Category::ANNOTATION))
		{
			isRemoved = ((CObject*)obj)->removeAnnotation((CAnnotation*)child);
		}
	}
	else if (obj->hasCategory(CBaseObject::Category::ANNOTATION))
	{
		isRemoved = (nullptr != ((CAnnotation*)obj)->removeAnnotation(child->id()));
	}

	if (isRemoved)
	{
		UI::DOCK::WORKSPACE::removeItem(child->id());
		UI::DOCK::PROPERTIES::selectionChanged(obj->id());

		if (deleteIt)
		{
			delete child;
		}

		UI::updateAllViews();

		return true;
	}

	return false;
}

void AP::OBJECT::moveTo(CBaseObject *obj, CBaseObject* newParent)
{
	if (obj != nullptr)
	{
		CBaseObject* oldParent = obj->getParent();

		CTransform t0;

		if (oldParent != nullptr)
		{
			t0 = CTransform(oldParent->getGlobalTransformationMatrix());
			AP::OBJECT::removeChild(oldParent, obj, false); // false => usun ale nie kasuj
		}
		else
		{
			if (obj == AP::WORKSPACE::getModel(obj->id()))
			{
				AP::WORKSPACE::removeModel(obj->id(), false);  // false => usun ale nie kasuj
			}
		}

		CModel3D* newmodel = new CModel3D();
		newmodel->setLabel("<=>");
		newmodel->setDescr("Macierz dopasowania, wygenerowana podczas przenoszenia obiektu");
		newmodel->addChild(obj);
		newmodel->importChildrenGeometry();

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

		UI::updateAllViews();
	}
}

void AP::OBJECT::copyTo(CBaseObject* obj, CBaseObject* newParent)
{
	if (obj != nullptr)
	{
		CBaseObject* kopia = obj->getCopy();

		CTransform t0;
		if (obj->getParent() != nullptr)
			t0 = CTransform(obj->getParent()->getGlobalTransformationMatrix());

		CModel3D* newmodel = new CModel3D();
		newmodel->setLabel("<=>");
		newmodel->setDescr("Macierz dopasowania, wygenerowana podczas kopiowania obiektu");

		if (newParent == nullptr) // copyToNew
		{
			newmodel->addChild(kopia);
			newmodel->importChildrenGeometry();
			newmodel->setTransform(t0);

			AP::WORKSPACE::addModel(newmodel);
		}
		else // copyTo existing
		{
			CTransform t1(newParent->getGlobalTransformationMatrix());
			CTransform ft = CTransform::fromTo(t0, t1);

			if (ft.toQMatrix4x4().isIdentity())
			{
				AP::OBJECT::addChild(newParent, kopia);
				delete newmodel;
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
		UI::updateAllViews();
	}
}