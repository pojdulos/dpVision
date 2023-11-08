#include "ContextMenu.h"

#include "AP.h"
#include "UI.h"
#include "MainWindow.h"
#include "Workspace.h"
#include "BaseObject.h"

CContextMenu::CContextMenu(CBaseObject *obj, QWidget *parent) : QMenu(parent), m_obj(obj)
{
	if (m_obj == nullptr)
	{
		addAction("Create empty model", this, SLOT(slotCreateEmptyModel()));
		addSeparator();
		addMenu(AP::mainWin().ui.menuWorkspace);
		addMenu(AP::mainWin().ui.menuCamera);
	}
	else
	{
		switch (m_obj->category())
		{
		case CBaseObject::Category::ANNOTATION:
			switch (m_obj->type())
			{
			case CBaseObject::Type::HISTOGRAM:
				addAction(QIcon(":/icons/Save.ico"), "export to csv/plt", this, SLOT(histogramSave()));
				addAction(QIcon(":/icons/Visible.ico"), "show/hide", this, SLOT(pointHide()));
				addAction(QIcon(":/icons/Erase.ico"), "delete", this, SLOT(slotDeleteObject()));
				break;

			case CBaseObject::Type::SETOFFACES:
				addAction(QIcon(":/icons/Save.ico"), "export to mesh", this, SLOT(setOfFacesToMesh()));
				break;
			default:
				addAction(QIcon(":/icons/Visible.ico"), "show/hide", this, SLOT(pointHide()));
				addAction(QIcon(":/icons/Erase.ico"), "delete", this, SLOT(slotDeleteObject()));
				break;
			}
			break;
		case CBaseObject::Category::OBJECT:
			switch (m_obj->type())
			{
			case CBaseObject::Type::MODEL:
				AP::WORKSPACE::setCurrentModel(m_obj->id());
				addMenu(createCopyMenu());
				addMenu(createMoveMenu());
				addSeparator();
				addMenu(createAddAnnoMenu());
				addSeparator();
				addMenu(AP::mainWin().ui.menuModel);
				addSeparator();
				addAction(QIcon(":/icons/Erase.ico"), "delete", this, SLOT(slotDeleteObject()));
				break;
			case CBaseObject::Type::IMAGE:
				AP::WORKSPACE::setCurrentModel(m_obj->id());
				//addMenu(createCopyMenu());
				//addMenu(createMoveMenu());
				//addSeparator();
				addMenu(AP::mainWin().ui.menuImage);
				addSeparator();
				addAction(QIcon(":/icons/Erase.ico"), "delete", this, SLOT(slotDeleteObject()));
				break;
			case CBaseObject::Type::VOLTK:
				addAction("show as images", this, SLOT(volTKshowImagesDlg()));
				addSeparator();
				addMenu(createCopyMenu());
				addAction(QIcon(":/icons/Save.ico"), "convert to cloud of points", this, SLOT(volTKtoCloud()));
				addAction(QIcon(":/icons/Save.ico"), "convert to mesh", this, SLOT(volTKtoMesh()));
				//addAction(QIcon(":/icons/Visible.ico"), "show/hide", this, SLOT(pointHide()));
				addSeparator();
				addAction(QIcon(":/icons/Erase.ico"), "delete", this, SLOT(slotDeleteObject()));
				break;
			case CBaseObject::Type::CLOUD:
			case CBaseObject::Type::MESH:
				addAction(QIcon(":/icons/Save.ico"), "export as...", this, SLOT(saveObjAs()));
				addSeparator();
				addMenu(createCopyMenu());
				addMenu(createMoveMenu());
				addSeparator();
				addAction("invert normals", this, SLOT(meshAction()))->setData(QVariant::fromValue((int)0));
				addSeparator();
				//addAction(QIcon(":/icons/Visible.ico"), "show/hide", this, SLOT(pointHide()));
				addAction(QIcon(":/icons/Erase.ico"), "delete", this, SLOT(slotDeleteObject()));
				break;
			default:
				addMenu(createCopyMenu());
				addMenu(createMoveMenu());
				//addAction(QIcon(":/icons/Visible.ico"), "show/hide", this, SLOT(pointHide()));
				addSeparator();
				addAction(QIcon(":/icons/Erase.ico"), "delete", this, SLOT(slotDeleteObject()));
				break;
			}
			break;
		}
	}
}

CContextMenu::~CContextMenu() {}

QMenu* CContextMenu::createCopySubMenu(QString label, CObject* obj)
{
	QMenu* menu2 = new QMenu(label);
	menu2->addAction(".. here ..", this, SLOT(copyTo()))->setData(QVariant::fromValue((void*)obj));
	menu2->addSeparator();

	for (const auto& m : obj->children())
	{
		if (((CObject*)m.second)->children().empty())
		{
			menu2->addAction(m.second->getLabel(), this, SLOT(copyTo()))->setData(QVariant::fromValue((void*)m.second));
		}
		else
		{
			menu2->addMenu(createCopySubMenu(m.second->getLabel(), (CObject*)m.second));
		}
	}
	return menu2;
}

QMenu* CContextMenu::createCopyMenu()
{
	QMenu* menu = new QMenu("copy to...");
	menu->setIcon(QIcon(":/icons/CopyTo.ico"));
	menu->addAction("...new model", this, SLOT(copyToNew()));
	menu->addSeparator();

	for (const auto& m : *AP::getWorkspace())
	{
		if (m.second->children().empty())
		{
			menu->addAction(m.second->getLabel(), this, SLOT(copyTo()))->setData(QVariant::fromValue((void*)m.second));
		}
		else
		{
			menu->addMenu(createCopySubMenu(m.second->getLabel(), m.second));
		}
	}
	return menu;
}


QMenu* CContextMenu::createMoveSubMenu(QString label, CObject* obj)
{
	QMenu* menu2 = new QMenu(label);
	menu2->addAction(".. here ..", this, SLOT(moveTo()))->setData(QVariant::fromValue((void*)obj));
	menu2->addSeparator();

	for (const auto& m : obj->children())
	{
		if (((CObject*)m.second)->children().empty())
		{
			menu2->addAction(m.second->getLabel(), this, SLOT(moveTo()))->setData(QVariant::fromValue((void*)m.second));
		}
		else
		{
			menu2->addMenu(createMoveSubMenu(m.second->getLabel(), (CObject*)m.second));
		}
	}
	return menu2;
}

QMenu* CContextMenu::createMoveMenu()
{
	QMenu* menu = new QMenu("move to...");
	menu->setIcon(QIcon(":/icons/MoveTo.ico"));
	menu->addAction("...new model", this, SLOT(moveToNew()));
	menu->addSeparator();

	for (const auto& m : *AP::getWorkspace())
	{
		if (m.second->children().empty())
		{
			menu->addAction(m.second->getLabel(), this, SLOT(moveTo()))->setData(QVariant::fromValue((void*)m.second));
		}
		else
		{
			menu->addMenu(createMoveSubMenu(m.second->getLabel(), m.second));
		}
	}
	return menu;
}

QMenu* CContextMenu::createAddAnnoMenu()
{
	QMenu* menu = new QMenu("add Annotation");

	if (menu == nullptr) return nullptr;

	menu->addAction("point", this, SLOT(slotAddAnnotation()))->setData(QVariant::fromValue((int)CBaseObject::Type::POINT));
	menu->addAction("plane", this, SLOT(slotAddAnnotation()))->setData(QVariant::fromValue((int)CBaseObject::Type::PLANE));
	menu->addAction("sphere", this, SLOT(slotAddAnnotation()))->setData(QVariant::fromValue((int) CBaseObject::Type::SPHERE));

	return menu;
}


#include "VolTK.h"
#include "PointCloud.h"
#include "Model3D.h"

extern void DPVISION_EXPORT testVolTKimageDialog(CVolTK*);
void CContextMenu::volTKshowImagesDlg()
{
	if (m_obj->hasType(CBaseObject::VOLTK))
	{
		testVolTKimageDialog((CVolTK*)m_obj);
	}
}

void CContextMenu::volTKtoCloud()
{
	if (m_obj->hasType(CBaseObject::VOLTK))
	{
		CVolTK* vol = (CVolTK*)m_obj;
		CModel3D* newModel = vol->toCloud();

		AP::WORKSPACE::addModel(newModel);

		UI::STATUSBAR::setText("Done!");
	}
}

void CContextMenu::volTKtoMesh()
{
	if (m_obj->hasType(CBaseObject::VOLTK))
	{
		CVolTK* vol = (CVolTK*)m_obj;
		CModel3D* newModel = vol->toMesh();

		AP::WORKSPACE::addModel(newModel);

		UI::STATUSBAR::setText("Done!");
	}
}

void CContextMenu::meshAction()
{
	QAction* action = (QAction*)QObject::sender();
	int whatToDo = action->data().value<int>();

	switch (whatToDo)
	{
	case 0:
		((CMesh*)m_obj)->invertNormals();
		break;
	default:
		break;
	}
}

void CContextMenu::moveTo()
{
	QAction* action = (QAction*)QObject::sender();

	CBaseObject* newParent = (CBaseObject*)action->data().value<void*>();

	if (newParent != nullptr)
	{
		CBaseObject* oldParent = m_obj->getParent();

		if (oldParent != nullptr)
		{
			AP::OBJECT::removeChild(oldParent, m_obj, false); // false => usun ale nie kasuj
		}
		else 
		{
			if (m_obj == AP::WORKSPACE::getModel(m_obj->id()))
			{
				AP::WORKSPACE::removeModel(m_obj->id(), false);  // false => usun ale nie kasuj
			}
		}

		AP::OBJECT::addChild(newParent, m_obj);
		UI::updateAllViews();
	}
}

void CContextMenu::moveToNew()
{
	CBaseObject* oldParent = m_obj->getParent();

	if (oldParent != nullptr)
	{
		AP::OBJECT::removeChild(m_obj->getParent(), m_obj, false); // false => usun ale nie kasuj
	}
	else
	{
		if (m_obj == AP::WORKSPACE::getModel(m_obj->id()))
		{
			AP::WORKSPACE::removeModel(m_obj->id(), false);  // false => usun ale nie kasuj
		}
	}

	CModel3D* tmpObj = new CModel3D;
	tmpObj->addChild(m_obj);
	tmpObj->importChildrenGeometry();

	AP::WORKSPACE::addModel(tmpObj);
}

void CContextMenu::copyTo()
{
	QAction* action = (QAction*)QObject::sender();

	CBaseObject* newParent = (CBaseObject*)action->data().value<void*>();

	if (newParent != nullptr)
	{
		AP::OBJECT::addChild(newParent, m_obj->getCopy());
		UI::updateAllViews();
	}
}

void CContextMenu::copyToNew()
{
	if (m_obj->hasType(CObject::Type::MODEL))
	{
		AP::WORKSPACE::addModel((CModel3D*)m_obj->getCopy());
	}
	else if (m_obj->hasType(CObject::Type::IMAGE))
	{
		AP::WORKSPACE::addImage((CImage*)m_obj->getCopy());
	}
	else
	{
		CModel3D* tmpObj = new CModel3D;
		tmpObj->addChild(m_obj->getCopy());
		tmpObj->importChildrenGeometry();
		AP::WORKSPACE::addModel(tmpObj);
	}
}

#include <QFileDialog>
#include "FileConnector.h"

void CContextMenu::saveObjAs()
{
	//if (m_obj->hasType(CBaseObject::CLOUD))
	//{
		//CPointCloud* cloud = (CPointCloud*)m_obj;
		
		CModel3D* tmpObj = new CModel3D;
		tmpObj->addChild(m_obj);

		QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", CFileConnector::getSaveExts());

		if (!fileName.isEmpty())
		{
			if (tmpObj->save(fileName))
			{
				UI::STATUSBAR::setText(L"Saved: " + fileName.toStdWString());
			}
		}

		tmpObj->removeChild(m_obj->id());
		delete tmpObj;
	//}
}

#include "Histogram.h"
#include <QFileInfo>
void CContextMenu::histogramSave()
{
	QString window_title("");
	QString filter("Histogram data (*.csv);;Plot files (*.plt)");
	QString initial_path("");

	QString path = UI::FILECHOOSER::getSaveFileName(window_title, initial_path, filter);

	if (path == "") return;

	QString fdir = QFileInfo(path).absolutePath();
	QString fname = QFileInfo(path).completeBaseName();

	((CHistogram *)m_obj)->save(fdir.toStdWString(), fname.toStdWString());
}


#include "AnnotationSetOfFaces.h"

void CContextMenu::setOfFacesToMesh()
{
	if (m_obj->hasType(CBaseObject::SETOFFACES))
	{
		CMesh* mesh = ((CAnnotationSetOfFaces*)m_obj)->toMesh();

		CModel3D* obj = new CModel3D();
		obj->addChild(mesh);
		obj->importChildrenGeometry();
		AP::WORKSPACE::addModel(obj);
	}
}


void CContextMenu::slotDeleteObject()
{
	AP::OBJECT::remove(m_obj, true);
}

void CContextMenu::slotCreateEmptyModel()
{
	CModel3D *m = new CModel3D();

	if (m == nullptr) return;

	m->reset(CBoundingBox::InitialValues::NullBB);

	AP::WORKSPACE::addModel(m);
}


void CContextMenu::pointHide()
{
	m_obj->switchSelfVisibility();
	//m_obj->setVisible(!m_obj->isVisible());
	
	UI::updateAllViews();
}

#include "MainWindow.h"

void CContextMenu::newPicWindow()
{
	AP::mainWin().createPicViewer((CImage*)m_obj);
}


#include "AnnotationPoint.h"
#include "AnnotationPlane.h"
#include "AnnotationSphere.h"

void CContextMenu::slotAddAnnotation()
{
	QAction* action = (QAction*)QObject::sender();

	CBaseObject::Type type = (CBaseObject::Type)action->data().value<int>();

	switch (type)
	{
	case CBaseObject::Type::POINT:
		AP::OBJECT::addChild(m_obj, new CAnnotationPoint());
		break;

	case CBaseObject::Type::PLANE:
		AP::OBJECT::addChild(m_obj, new CAnnotationPlane());
		break;

	case CBaseObject::Type::SPHERE:
		AP::OBJECT::addChild(m_obj, new CAnnotationSphere());
		break;
	}
}


