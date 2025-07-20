#include "ContextMenu.h"

#include "AP.h"

#include "UI.h"

#include "MainWindow.h"
#include "Workspace.h"
#include "BaseObject.h"
#include "Object.h"

#include <QMessageBox>

CContextMenu::CContextMenu(std::shared_ptr<CBaseObject> obj, QWidget *parent) : QMenu(parent), m_obj(obj)
{
	auto main_window = CMainWindow::instance();

	addAction("expand all", this, SLOT(slotExpandAll()));
	addAction("collapse all", this, SLOT(slotCollapseAll()));
	addSeparator();
	if (m_obj == nullptr)
	{
		addMenu(createAddObjectMenu());
		addSeparator();
		//addMenu(AP::mainWin().ui.menuWorkspace);
		//addMenu(AP::mainWin().ui.menuCamera);
		addMenu(main_window->ui.menuWorkspace);
		addMenu(main_window->ui.menuCamera);
	}
	else
	{
		addSeparator();
		addAction("show all under this", this, SLOT(slotShowAll()));
		addAction("hide all under this", this, SLOT(slotHideAll()));
		addSeparator();
		addMenu(createAddAnnoMenu());
		switch (m_obj->category())
		{
		case CBaseObject::Category::ANNOTATION:
			switch (m_obj->type())
			{
			case CBaseObject::Type::HISTOGRAM:
				addAction(QIcon(":/icons/Save.ico"), "export to csv/plt", this, SLOT(histogramSave()));
				addAction(QIcon(":/icons/Visible.ico"), "show/hide", this, SLOT(pointHide()));
				break;

			case CBaseObject::Type::SETOFFACES:
				addAction(QIcon(":/icons/Save.ico"), "export to mesh", this, SLOT(setOfFacesToMesh()));
				break;
			case CBaseObject::Type::PLANE:
				addAction(QIcon(":/icons/Save.ico"), "to Mesh...", this, SLOT(slotPlaneToMesh()));
				addSeparator();
				addAction(QIcon(":/icons/Visible.ico"), "show/hide", this, SLOT(pointHide()));
				break;
			case CBaseObject::Type::TRIPLE:
				addAction(QIcon(":/icons/Save.ico"), "to Plane...", this, SLOT(slotTriangleToPlane()));
				break;
			default:
				addAction(QIcon(":/icons/Visible.ico"), "show/hide", this, SLOT(pointHide()));
				break;
			}
			break;
		case CBaseObject::Category::OBJECT:
			addAction("test graph", this, SLOT(slot_test_graph()));
			addSeparator();
			addMenu(createAddObjectMenu());
			addSeparator();
			switch (m_obj->type())
			{
			case CBaseObject::Type::MODEL:
				AP::WORKSPACE::setCurrentModel(m_obj->id());
				addAction(QIcon(":/icons/Save.ico"), "Save as...", this, SLOT(saveObjAs()));
				addSeparator();
				//addMenu(AP::mainWin().ui.menuModel);
				addMenu(main_window->ui.menuModel);
				addSeparator();
				addAction("create inversed transformation", this, SLOT(slot_create_inversed_transform()));
				break;
			case CBaseObject::Type::IMAGE:
				AP::WORKSPACE::setCurrentModel(m_obj->id());
				//addMenu(AP::mainWin().ui.menuImage);
				addMenu(main_window->ui.menuImage);
				break;
			case CBaseObject::Type::MOVEMENT:
				addAction("copy all frames as new models", this, SLOT(slot_copy_frames_as_models()));
				break;
			case CBaseObject::Type::VOLUMETRIC_NEW:
				addSeparator();
				addMenu(createVolumetricMenu());
				break;
			case CBaseObject::Type::CLOUD:
			case CBaseObject::Type::MESH:
				addAction(QIcon(":/icons/Save.ico"), "export as...", this, SLOT(saveObjAs()));
				addSeparator();
				addAction("invert normals", this, SLOT(meshAction()))->setData(QVariant::fromValue((int)0));
				//addSeparator();
				//addAction(QIcon(":/icons/Visible.ico"), "show/hide", this, SLOT(pointHide()));
				break;
			default:
				//addAction(QIcon(":/icons/Visible.ico"), "show/hide", this, SLOT(pointHide()));
				break;
			}
			break;
		}
		addSeparator();
		addMenu(createRepositionMenu());
		addMenu(createDeleteMenu());
	}
}

CContextMenu::~CContextMenu() {}

QMenu* CContextMenu::createAddObjectMenu()
{
	QMenu* menu = new QMenu("add Object...");

	menu->addAction("transformation", this, SLOT(slotCreateEmptyModel()));
	if (m_obj && m_obj->hasCategory(CBaseObject::Category::OBJECT)) {
		addSeparator();
		menu->addAction("mesh", this, SLOT(slot_mesh_create()));
		menu->addAction("volumetric", this, SLOT(slot_volum_create()));
	}
	return menu;
}

QMenu* CContextMenu::createAddAnnoMenu()
{
	QMenu* menu = new QMenu("add Annotation");

	if (menu == nullptr) return nullptr;

	menu->addAction("point", this, SLOT(slotAddAnnotation()))->setData(QVariant::fromValue((int)CBaseObject::Type::POINT));
	menu->addAction("plane", this, SLOT(slotAddAnnotation()))->setData(QVariant::fromValue((int)CBaseObject::Type::PLANE));
	menu->addAction("sphere", this, SLOT(slotAddAnnotation()))->setData(QVariant::fromValue((int)CBaseObject::Type::SPHERE));

	return menu;
}


QMenu* CContextMenu::createRepositionMenu()
{
	QMenu* menu = new QMenu("reposition object");
	menu->setIcon(QIcon(":/icons/Copy.ico"));

	menu->addAction("move/copy", this, SLOT(slot_repositioning()));

	if (m_obj->hasType(CBaseObject::Type::MODEL) || m_obj && m_obj->hasType(CBaseObject::Type::MESH)
		|| m_obj && m_obj->hasType(CBaseObject::Type::CLOUD) || m_obj && m_obj->hasType(CBaseObject::Type::ORDEREDCLOUD))
	{
		menu->addAction("apply last transformation and move up", this, SLOT(slot_apply_last_transform()));
	}

	menu->addSeparator();
	if (m_obj && m_obj->hasCategory(CBaseObject::Category::OBJECT)) {
		menu->addAction("set as root", this, SLOT(slot_make_me_root()));
	}

	return menu;
}

QMenu* CContextMenu::createDeleteMenu()
{
	QMenu* menu = new QMenu("delete");
	menu->setIcon(QIcon(":/icons/Erase.ico"));
	addSeparator();
	if (m_obj && m_obj->hasCategory(CBaseObject::Category::OBJECT)) {
		QAction* a = new QAction("only this object");
		connect(a, SIGNAL(triggered()), this, SLOT(slot_delete_and_keep_children()));
		a->setEnabled(m_obj->getParent());
		menu->addAction(a);
		//menu->addAction("only this object", this, SLOT(slot_delete_and_keep_children()));
	}
	menu->addAction("the entire subtree below", this, SLOT(slot_delete_object_with_subtree()));

	return menu;
}

QMenu* CContextMenu::createCopySubMenu(QString label, std::shared_ptr<CObject> obj)
{
	QMenu* menu2 = new QMenu(label);
	menu2->addAction(".. here ..", this, SLOT(copyTo()))->setData(QVariant::fromValue<std::shared_ptr<CBaseObject>>(obj));
	menu2->addSeparator();

	for (const auto& m : obj->children())
	{
		if (((CObject*)m.second.get())->children().empty())
		{
			menu2->addAction(m.second->getLabel(), this, SLOT(copyTo()))->setData(QVariant::fromValue<std::shared_ptr<CBaseObject>>(m.second));
		}
		else
		{
			menu2->addMenu(createCopySubMenu(m.second->getLabel(), std::static_pointer_cast<CObject>(m.second)));
		}
	}
	return menu2;
}

QMenu* CContextMenu::createCopyMenu()
{
	QMenu* menu = new QMenu("copy to...");
	menu->setIcon(QIcon(":/icons/CopyTo.ico"));
	menu->addAction("...new model", this, SLOT(copyTo()))->setData(QVariant::fromValue<std::shared_ptr<CBaseObject>>(nullptr));
	menu->addSeparator();

	for (const auto& m : *AP::getWorkspace())
	{
		if (m.second->children().empty())
		{
			menu->addAction(m.second->getLabel(), this, SLOT(copyTo()))->setData(QVariant::fromValue<std::shared_ptr<CBaseObject>>(m.second));
		}
		else
		{
			menu->addMenu(createCopySubMenu(m.second->getLabel(), m.second));
		}
	}
	return menu;
}


QMenu* CContextMenu::createMoveSubMenu(QString label, std::shared_ptr<CObject> obj)
{
	QMenu* menu2 = new QMenu(label);
	menu2->addAction(".. here ..", this, SLOT(moveTo()))->setData(QVariant::fromValue<std::shared_ptr<CBaseObject>>(obj));
	menu2->addSeparator();

	for (const auto& m : obj->children())
	{
		if (((CObject*)m.second.get())->children().empty())
		{
			menu2->addAction(m.second->getLabel(), this, SLOT(moveTo()))->setData(QVariant::fromValue<std::shared_ptr<CBaseObject>>(m.second));
		}
		else
		{
			menu2->addMenu(createMoveSubMenu(m.second->getLabel(), std::static_pointer_cast<CObject>(m.second)));
		}
	}
	return menu2;
}

QMenu* CContextMenu::createMoveMenu()
{
	QMenu* menu = new QMenu("move to...");
	menu->setIcon(QIcon(":/icons/MoveTo.ico"));
	menu->addAction("...new model", this, SLOT(moveTo()))->setData(QVariant::fromValue<std::shared_ptr<CBaseObject>>(nullptr));
	menu->addSeparator();

	for (const auto& m : *AP::getWorkspace())
	{
		if (m.second->children().empty())
		{
			menu->addAction(m.second->getLabel(), this, SLOT(moveTo()))->setData(QVariant::fromValue<std::shared_ptr<CBaseObject>>(m.second));
		}
		else
		{
			menu->addMenu(createMoveSubMenu(m.second->getLabel(), m.second));
		}
	}
	return menu;
}



#include "PointCloud.h"
#include "Model3D.h"

void CContextMenu::meshAction()
{
	QAction* action = (QAction*)QObject::sender();
	int whatToDo = action->data().value<int>();

	switch (whatToDo)
	{
	case 0:
		std::static_pointer_cast<CMesh>(m_obj)->invertNormals();
		break;
	default:
		break;
	}
}

void CContextMenu::moveTo()
{
	QAction* action = (QAction*)QObject::sender();

	std::shared_ptr<CBaseObject> newParent = action->data().value<std::shared_ptr<CBaseObject>>();

	AP::OBJECT::moveTo(m_obj, newParent);
}


void CContextMenu::copyTo()
{
	QAction* action = (QAction*)QObject::sender();

	std::shared_ptr<CBaseObject> newParent = action->data().value<std::shared_ptr<CBaseObject>>();

	AP::OBJECT::copyTo(m_obj, newParent);
}


#include <QFileDialog>
#include "FileConnector.h"

#include "StatusBarManager.h"

void CContextMenu::saveObjAs()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", CFileConnector::getSaveExts());

	if (!fileName.isEmpty())
	{
		std::shared_ptr<CModel3D> tmpObj;
		if (m_obj->hasType(CBaseObject::Type::MODEL))
		{
			if (std::dynamic_pointer_cast<CModel3D>(m_obj)->save(fileName))
			{
				StatusBarManager::setText("Saved: " + fileName);
			}
		}
		else
		{
			tmpObj = std::make_shared<CModel3D>();
			tmpObj->addChild(tmpObj, m_obj);
			if (tmpObj->save(fileName))
			{
				StatusBarManager::setText("Saved: " + fileName);
			}
			tmpObj->removeChild(m_obj->id());
			m_obj->setParent(nullptr);
			//delete tmpObj;
		}
	}
}

void reversePath(std::vector<std::shared_ptr<CBaseObject>>& path, std::shared_ptr<CObject> start) {
	
	std::vector<std::shared_ptr<CBaseObject>> revpath(path);
	
	std::reverse(revpath.begin(), revpath.end());
	revpath.push_back(start);
	revpath.push_back(nullptr);


	for (int i = 0; i < revpath.size()-1; i++) {
		std::shared_ptr<CObject> objD = std::dynamic_pointer_cast<CObject>(revpath[i]);		// dotychczasowy rodzic
		std::shared_ptr<CObject> objA = std::dynamic_pointer_cast<CObject>(revpath[i + 1]);	// dotychczasowy potomek

		// w najgorszym przypadku trzeba będzie skompensować aż dwie macierze transformacji:
		// macierz dotychczasowego rodzica - bo dotychczasowy potomek wyjdzie poza jej wpływ więc jego położenie wzgledem rodzica sie zmieni
		// macierz dotychczasowego potomka - bo dotychczasowy rodzic znajdzie sie teraz pod jej wpływem co zmieni jego położenie
		// dlatego dopasowanie jest złożeniem dwóch macierzy odwrotnych

		if (objA) {
			Eigen::Matrix4d mD_inv = Eigen::Matrix4d::Identity();
			if (objD->hasType(CObject::Type::MODEL))
				mD_inv = std::static_pointer_cast<CModel3D>(objD)->transform().toEigenMatrix4d().inverse();


			Eigen::Matrix4d mA_inv = Eigen::Matrix4d::Identity();
			if (objA->hasType(CObject::Type::MODEL))
				mA_inv = std::static_pointer_cast<CModel3D>(objA)->transform().toEigenMatrix4d().inverse();

			Eigen::Matrix4d mDop = mA_inv * mD_inv;

			CObject* p = (CObject*)objD->getParent();
			if (p) p->removeChild(objD->id());

			if (mDop.isIdentity()) {
				objA->addChild(objA, objD);
			}
			else {
				std::shared_ptr<CModel3D> objDop = std::make_shared<CModel3D>();
				objDop->setTransform(mDop);
				objDop->setLabel("DOPASOWANIE");
				objA->addChild(objA, objDop);
				objDop->addChild(objDop, objD);
			}

			//CModel3D* objA_inv = new CModel3D();
			//objA_inv->setTransform(mA_inv);

			//CModel3D* objD_inv = new CModel3D();
			//objD_inv->setTransform(mD_inv);

			//objA->addChild(objA_inv);
			//objA_inv->addChild(objD_inv);
			//objD_inv->addChild(objD);
		}
		else {
			CObject* p = (CObject*)objD->getParent();
			if (p) p->removeChild(objD->id());

			objD->setParent(nullptr);
		}
	}
}


// 4. Główna funkcja
bool makeObjectRoot(std::shared_ptr<CObject> m_obj)
{
	if (m_obj == nullptr)
		return false;

	std::shared_ptr<CObject> parent = std::dynamic_pointer_cast<CObject>(m_obj->getParentPtr());
	if (parent == nullptr)
		return false;

	// Zbuduj ścieżkę do korzenia
	auto path = CBaseObject::getPathToRoot(m_obj);

	// Znajdź korzeń drzewa
	if (path.empty()) // already root
		return true;
	
	//CBaseObject* top = path.back();


	// Odwróć relacje wzdłuż ścieżki (przebuduj drzewo)
	reversePath(path, m_obj);

	parent->removeChild(m_obj->id());
	m_obj->setParent(nullptr);

	return true;
}


void CContextMenu::slot_make_me_root()
{
	if (m_obj == nullptr)
		return; 	// object not exists or is not regular object

	if (!m_obj->hasCategory(CBaseObject::Category::OBJECT)) {
		QMessageBox::information(0,"","The object you have choosen probably is an annotation.\nCurrently only regular (data) object can be set as root.");
		return;
	}

	std::shared_ptr<CBaseObject> top = CBaseObject::getRoot(m_obj);

	if (top == m_obj) {
		QMessageBox::information(0,"","An object is already root,\nso you are not changing anything");
		return;
	}

	Eigen::Matrix4d globalM = CBaseObject::getGlobalTransformationMatrix(m_obj);

	auto wksp = CWorkspace::instance();

	wksp->_objectRemove(top);

	if (makeObjectRoot(std::static_pointer_cast<CObject>(m_obj)))
	{
		AP::WORKSPACE::addObject(m_obj);

		if (m_obj->hasType(CObject::Type::MODEL))
			std::static_pointer_cast<CModel3D>(m_obj)->transform().fromEigenMatrix4d(globalM);
		else
			std::static_pointer_cast<CModel3D>(m_obj->getParentPtr())->transform().fromEigenMatrix4d(globalM);
	}
}


#include "GraphViewer.h"



void CContextMenu::slot_test_graph()
{
	auto* gv = new GraphViewer(m_obj); // bez parenta!
	gv->setAttribute(Qt::WA_DeleteOnClose, true); // Qt samo usunie obiekt po zamknięciu okna
	gv->show();
}

void CContextMenu::slot_make_me_root2()
{
	if (m_obj == nullptr)
		return; 	// object not exists

	std::shared_ptr<CBaseObject> parent = m_obj->getParentPtr();

	if (parent == nullptr)
		return; 	// object is already root

	// searching for the most top object in tree
	std::shared_ptr<CObject> top = std::dynamic_pointer_cast<CObject>(m_obj);
	while (top->getParent() != nullptr)
		top = std::static_pointer_cast<CObject>(top->getParentPtr());

	Eigen::Matrix4d globalM = CBaseObject::getGlobalTransformationMatrix(m_obj);

	auto wksp = CWorkspace::instance();

	wksp->_objectRemove(m_obj);

	CWorkspace::instance()->_objectRemove(top);

	std::shared_ptr<CObject> new_parent = std::dynamic_pointer_cast<CObject>(m_obj);

	while (parent != nullptr) {
		std::shared_ptr<CObject> grandpa = std::dynamic_pointer_cast<CObject>(parent->getParentPtr());
		if (grandpa != nullptr) grandpa->removeChild(parent->id());

		if (parent->hasType(CObject::Type::MODEL)) {
			auto p = std::dynamic_pointer_cast<CModel3D>(parent);

			// tu odwrócenie transformacji
			Eigen::Matrix4Xd m = p->transform().toEigenMatrix4d();

			p->transform().reset();

			std::shared_ptr<CModel3D> tmp = std::make_shared<CModel3D>();
			tmp->transform().fromEigenMatrix4d(m.inverse());
			tmp->setLabel("DOPASOWANIE");

			p->addChild(p, tmp);

			new_parent->addChild(new_parent, parent);
			new_parent = tmp;
		}
		else {
			new_parent->addChild(new_parent, parent);
			new_parent = std::dynamic_pointer_cast<CObject>(parent);
		}
		parent = grandpa;
	}

	AP::WORKSPACE::addObject(m_obj);

	if (m_obj->hasType(CObject::Type::MODEL))
		std::static_pointer_cast<CModel3D>(m_obj)->transform().fromEigenMatrix4d(globalM);
	else
		std::static_pointer_cast<CModel3D>(m_obj->getParentPtr())->transform().fromEigenMatrix4d(globalM);
}



#include "TreeSelectDialog.h"
#include "DockWidgetWorkspace.h"

void CContextMenu::slot_repositioning()
{
	if (m_obj == nullptr)
		return; 	// obiekt nie istnieje

	QStandardItemModel* model = (QStandardItemModel*)UI::DOCK::WORKSPACE::instance()->getTreeView()->model();

	TreeSelectDialog dlg(m_obj);
	dlg.cloneModelToWidget(model);

	if (dlg.exec() == QDialog::Accepted) {
		std::shared_ptr<CBaseObject> wybranyObiekt = dlg.selectedHandle();
		QString action = dlg.getAction();
		bool keep_pos = dlg.keepPosition();

		if (action == "copy")
		{
			AP::OBJECT::copyTo(m_obj, wybranyObiekt, keep_pos);
		}
		else if (action == "move")
		{
			AP::OBJECT::moveTo(m_obj, wybranyObiekt, keep_pos);
		}
		else if (action == "rearrange")
		{
			if (!m_obj->hasCategory(CBaseObject::Category::OBJECT)) {
				QMessageBox::information(0,"","The object you have choosen probably is an annotation.\nCurrently only regular (data) object can be set as root.");
				return;
			}

			std::shared_ptr<CBaseObject> tgt_top = (wybranyObiekt)?CBaseObject::getRoot(wybranyObiekt):nullptr;
			std::shared_ptr<CBaseObject> top = CBaseObject::getRoot(m_obj);

			if (top == tgt_top) {
				QMessageBox::information(0,"","An object cannot be a descendant of its own descendant");
				return;
			}

			bool already_root = (top == m_obj);

			if (already_root && (wybranyObiekt == nullptr)) {
				QMessageBox::information(0,"","You are trying to move the root object to the workspace,\nso you are not changing anything");
				return;
			}

			Eigen::Matrix4d globalM = CBaseObject::getGlobalTransformationMatrix(m_obj);

			CWorkspace::instance()->_objectRemove(std::static_pointer_cast<CModel3D>(top));

			if (already_root || makeObjectRoot(std::static_pointer_cast<CObject>(m_obj)))
			{
				if (wybranyObiekt != nullptr)
				{
					if (keep_pos && ! m_obj->hasType(CObject::Type::MODEL)) // musze dodać macierz dopasowujacą
					{
						std::shared_ptr<CModel3D> mdl = std::make_shared<CModel3D>();
						mdl->addChild(mdl, m_obj);
						mdl->setLabel("<**>");
						mdl->importChildrenGeometry();
						AP::OBJECT::addChild(wybranyObiekt, mdl);
					}
					else
					{
						AP::OBJECT::addChild(wybranyObiekt, m_obj);
					}
				}
				else
				{
					AP::WORKSPACE::addObject(m_obj);
				}

				if (keep_pos)
				{
					if (m_obj->hasType(CObject::Type::MODEL))
						std::static_pointer_cast<CModel3D>(m_obj)->transform().fromEigenMatrix4d(globalM);
					else
						std::static_pointer_cast<CModel3D>(m_obj->getParentPtr())->transform().fromEigenMatrix4d(globalM);
				}
			}
		}
	}
}


void CContextMenu::slot_apply_last_transform()
{
	// Kopiujemy obiekt z rodzica do dziadka,
	// przekształcając go tak by zachował pozycję
	
	if (m_obj == nullptr)
		return; 	// obiekt nie istnieje

	auto wksp = CWorkspace::instance();

	std::shared_ptr<CBaseObject> parent = m_obj->getParentPtr();
	
	if ( (parent == nullptr) || (!parent->hasType(CBaseObject::Type::MODEL)) )
		return;	// rodzicem obiektu nie jest przekształcenie

	CTransform parent_transform = std::static_pointer_cast<CModel3D>(parent)->transform();

	std::shared_ptr<CBaseObject> grandpa = parent->getParentPtr();
	
	//CTransform grandpa_transform; // zerowe przekształcenie 
	CTransform null_transform; // zerowe przekształcenie 

	//if (grandpa != nullptr)
	//	grandpa_transform = grandpa->getGlobalTransformationMatrix();

	if (m_obj->hasType(CBaseObject::Type::MODEL))
	{
		std::shared_ptr<CModel3D> mdl = std::static_pointer_cast<CModel3D>(m_obj);
		Eigen::Matrix4d T = mdl->transform().toEigenMatrix4d();
		Eigen::Matrix4d Tp = parent_transform.toEigenMatrix4d();

		T = Tp * T;

		mdl->transform().fromEigenMatrix4d(T);

		wksp->_objectRemove(mdl);

		if (grandpa == nullptr)
		{
			AP::WORKSPACE::addModel(mdl);
		}
		else
		{
			AP::OBJECT::addChild(grandpa, mdl);
		}
	}
	else if (m_obj->hasType(CBaseObject::Type::MESH) || m_obj->hasType(CBaseObject::Type::CLOUD) || m_obj->hasType(CBaseObject::Type::ORDEREDCLOUD))
	{
		std::static_pointer_cast<CPointCloud>(m_obj)->applyTransformation(parent_transform, null_transform);

		if (grandpa == nullptr) // workspace root
		{
			// na razie jeszcze na najwyższym poziomie drzewa musi byc Model3D
			grandpa = std::make_shared<CModel3D>();
			AP::WORKSPACE::addModel(std::static_pointer_cast<CModel3D>(grandpa));
		}

		wksp->_objectRemove(m_obj);
		AP::OBJECT::addChild(grandpa, m_obj);
	}
}

void CContextMenu::slot_delete_and_keep_children()
{
	auto wksp = CWorkspace::instance();
	if (m_obj->hasCategory(CBaseObject::Category::OBJECT))
	{
		std::shared_ptr<CObject> p = std::dynamic_pointer_cast<CObject>(m_obj->getParentPtr());
		if (p != nullptr)
		{
			auto kids = std::static_pointer_cast<CObject>(m_obj)->children();
			for (auto d : kids)
			{
				std::shared_ptr<CBaseObject> c = d.second;
				wksp->_objectRemove(c);
				AP::OBJECT::addChild(p, c);
			}
//			((CObject*)m_obj)->children().clear();

			auto anno = std::static_pointer_cast<CObject>(m_obj)->annotations();
			for (auto d : anno)
			{
				wksp->_objectRemove(d.second);
				AP::OBJECT::addChild(p, std::static_pointer_cast<CBaseObject>(d.second));
			}
//			((CObject*)m_obj)->annotations().clear();
			
			wksp->_objectRemove(m_obj);
		}
		else
		{
			;// AP::WORKSPACE::addModel(obj);
		}
	}
}

void CContextMenu::slot_create_inversed_transform()
{
	if (m_obj->hasType(CBaseObject::Type::MODEL))
	{
		std::shared_ptr<CModel3D> obj = std::make_shared<CModel3D>();
		obj->transform().fromEigenMatrix4d( std::static_pointer_cast<CModel3D>(m_obj)->transform().toEigenMatrix4d().inverse() );
		obj->setLabel(m_obj->getLabel() + ".inv");
		obj->resetBoundingBox(CBoundingBox::NullBB);

		std::shared_ptr<CBaseObject> p = m_obj->getParentPtr();
		if (p == nullptr)
		{
			AP::WORKSPACE::addModel(obj);
		}
		else
		{
			AP::OBJECT::addChild(p, obj);
		}
	}

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

	std::static_pointer_cast<CHistogram>(m_obj)->save(fdir.toStdWString(), fname.toStdWString());
}


#include "AnnotationSetOfFaces.h"

void CContextMenu::setOfFacesToMesh()
{
	if (m_obj->hasType(CBaseObject::SETOFFACES))
	{
		std::shared_ptr<CMesh> mesh = std::static_pointer_cast<CAnnotationSetOfFaces>(m_obj)->toMesh();

		std::shared_ptr<CModel3D> obj = std::make_shared<CModel3D>();
		obj->addChild(obj, mesh);
		obj->importChildrenGeometry();
		AP::WORKSPACE::addModel(obj);
	}
}

void CContextMenu::slot_delete_object_with_subtree()
{
	CWorkspace::instance()->_objectRemove(m_obj);
}

void CContextMenu::slotCreateEmptyModel()
{
	std::shared_ptr<CModel3D> m = std::make_shared<CModel3D>();
	if (m == nullptr) return;
	m->reset(CBoundingBox::InitialValues::NullBB);

	if (m_obj && m_obj->hasCategory(CBaseObject::Category::OBJECT) ) {
		AP::OBJECT::addChild(m_obj, m);
	}
	else {

		AP::WORKSPACE::addModel(m);
	}
}

void CContextMenu::slot_mesh_create()
{
	std::shared_ptr<CMesh> m = std::make_shared<CMesh>();
	if (m != nullptr) AP::OBJECT::addChild(m_obj, m);
}

/*********************************************************************************************/

#include "Volumetric.h"

void CContextMenu::slot_volum_create()
{
	std::shared_ptr<Volumetric> m = Volumetric::create();
	if (m != nullptr) AP::OBJECT::addChild(m_obj, m);
}

QMenu* CContextMenu::createVolumetricMenu()
{
	QMenu* menu = new QMenu("volumetric...");

	menu->addAction("show as images", this, SLOT(slot_vol_show_images()));
	menu->addSeparator();
	menu->addAction("change metadata", this, SLOT(slot_volumetric_set_metadata()));
	menu->addSeparator();
	menu->addAction("create SIFT cloud", this, SLOT(slot_volumetric_sift_cloud()));
	menu->addAction("marching cube", this, SLOT(slot_volumetric_marching_cube()));
	menu->addAction("marching tetrahedron", this, SLOT(slot_volumetric_marching_tetra()));
	menu->addSeparator();
	menu->addAction("export as DICOMs...", this, SLOT(slot_volumetric_export()));
	menu->addSeparator();

	return menu;
}

#include <QtWidgets>
#include <QtGui>

#include "volumetricImageDialog.h"

void CContextMenu::slot_vol_show_images()
{
	if (m_obj->hasType(CBaseObject::VOLUMETRIC_NEW))
	{
		std::shared_ptr<VolumetricImageDialog> dialog = std::make_shared<VolumetricImageDialog>((Volumetric*)m_obj.get());

		dialog->show();
	}
}

void CContextMenu::slot_volumetric_export()
{
	QString file_path = UI::FILECHOOSER::getSaveFileName("", "", QString("*.dcm"));
	qInfo() << file_path << Qt::endl;

	if (file_path.endsWith(".dcm"))
	{
		//QString fname = file_path.chopped(4);
		//qInfo() << fname << Qt::endl;
		
		CParser *parser = CFileConnector::getSaveParser(".dcm");
		parser->save({ std::static_pointer_cast<Volumetric>(m_obj) }, file_path);
	}
}

#include "Movement.h"

void CContextMenu::slot_copy_frames_as_models()
{
	if (m_obj->hasType(CBaseObject::Type::MOVEMENT))
	{
		std::shared_ptr<CMovement> r = std::static_pointer_cast<CMovement>(m_obj);

		for (CMovement::FrameVal& fv : r->m_seqlist)
		{
			std::shared_ptr<CModel3D> mdl = std::make_shared<CModel3D>();
			mdl->setTransform(fv.t);
			mdl->setLabel(fv.getLabel());

			for (auto k : r->children()) {
				mdl->addChild(mdl, k.second->getCopy());
			}

			for (auto k : r->annotations()) {
				mdl->addAnnotation(mdl, std::static_pointer_cast<CAnnotation>(k.second->getCopy()));
			}

			std::shared_ptr<CObject> p = std::dynamic_pointer_cast<CObject>(r->getParentPtr());
			if (p)
				AP::OBJECT::addChild(p, mdl);
			else
				AP::WORKSPACE::addModel(mdl);
		}
	}
}

void CContextMenu::slot_volumetric_set_metadata()
{
	if (auto v = std::dynamic_pointer_cast<Volumetric>(m_obj))
	{
		float* origin = v->metadata[0].image_position_patient;

		float vsizeX = v->metadata[1].pixel_spacing[0];
		float vsizeY = v->metadata[1].pixel_spacing[1];
		float vsizeZ = v->metadata[1].slice_distance;

		unsigned int shape[] = {
			v->layers(),
			v->rows(),
			v->columns()
		};


		QDialog* dlg = new QDialog();

		dlg->setWindowTitle("Volumetric Metadata");

		QDoubleValidator* validator = new QDoubleValidator(-10000.0, 10000.0, 3);
		//validator->setLocale(QLocale::C);

		QLineEdit* input1 = new QLineEdit(dlg);
		QLineEdit* input2 = new QLineEdit(dlg);
		QLineEdit* input3 = new QLineEdit(dlg);
		input1->setValidator(validator);
		input2->setValidator(validator);
		input3->setValidator(validator);

		input1->setText(QString::number(origin[0]));
		input2->setText(QString::number(origin[1]));
		input3->setText(QString::number(origin[2]));

		QPushButton* ctrButton = new QPushButton("Center", dlg);

		QObject::connect(ctrButton, &QPushButton::clicked, [&]() {
			origin[0] = -(vsizeX * shape[2]) / 2;
			origin[1] = -(vsizeY * shape[1]) / 2;
			origin[2] = -(vsizeZ * shape[0]) / 2;

			input1->setText(QString::number(origin[0]));
			input2->setText(QString::number(origin[1]));
			input3->setText(QString::number(origin[2]));
			});

		QGroupBox* group1 = new QGroupBox(dlg);
		group1->setTitle("Origin:");
		group1->setToolTip("ImagePositionPatient and SliceLocation of first image");

		QFormLayout* formLayout1 = new QFormLayout();
		formLayout1->addRow("x:", input1);
		formLayout1->addRow("y:", input2);
		formLayout1->addRow("z:", input3);
		formLayout1->addRow(ctrButton);

		group1->setLayout(formLayout1);

		QLineEdit* input4 = new QLineEdit(dlg);
		QLineEdit* input5 = new QLineEdit(dlg);
		QLineEdit* input6 = new QLineEdit(dlg);
		input4->setValidator(validator);
		input5->setValidator(validator);
		input6->setValidator(validator);

		input4->setText(QString::number(vsizeX));
		input5->setText(QString::number(vsizeY));
		input6->setText(QString::number(vsizeZ));


		QGroupBox* group2 = new QGroupBox(dlg);
		group2->setTitle("Voxel spacing:");
		QFormLayout* formLayout2 = new QFormLayout();
		formLayout2->addRow("x:", input4);
		formLayout2->addRow("y:", input5);
		formLayout2->addRow("z:", input6);
		group2->setLayout(formLayout2);
		group2->setToolTip("PixelSpacing and SliceThickness of all images");

		QHBoxLayout* hboxlayout = new QHBoxLayout();

		hboxlayout->addWidget(group1);
		hboxlayout->addWidget(group2);

		QPushButton* okButton = new QPushButton("OK", dlg);
		QPushButton* cancelButton = new QPushButton("Cancel", dlg);

		QObject::connect(okButton, SIGNAL(clicked()), dlg, SLOT(accept()));
		QObject::connect(cancelButton, SIGNAL(clicked()), dlg, SLOT(reject()));

		QVBoxLayout* layout = new QVBoxLayout();

		QString crl = "Volume size: " + QString::number(shape[2]) + " x " + QString::number(shape[1]) + " x " + QString::number(shape[0]);
		crl += " (" + QString::number(vsizeX * shape[2]) + "mm x " + QString::number(vsizeY * shape[1]) + "mm x " + QString::number(vsizeZ * shape[0]) + "mm)";

		layout->addWidget(new QLabel(crl));
		layout->addLayout(hboxlayout);
		//layout->addWidget(group1);
		layout->addWidget(okButton);
		layout->addWidget(cancelButton);

		dlg->setLayout(layout);

		if (dlg->exec())
		{
			float posX = input1->text().replace(',', '.').toFloat();
			float posY = input2->text().replace(',', '.').toFloat();
			float posZ = input3->text().replace(',', '.').toFloat();

			float vsizeX = input4->text().replace(',', '.').toFloat();
			float vsizeY = input5->text().replace(',', '.').toFloat();
			float vsizeZ = input6->text().replace(',', '.').toFloat();

			float delta[] = { posX - origin[0], posY - origin[1], posZ - origin[2] };

			//origin[0] = posX;
			//origin[1] = posY;
			//origin[2] = posZ;

			for (int i = 0; i < v->metadata.size(); i++)
			{
				auto& md = v->metadata[i];

				md.image_position_patient[0] = posX; // += delta[0];
				md.image_position_patient[1] = posY; // += delta[1];
				md.image_position_patient[2] = posZ + vsizeZ * i;

				md.slice_location = md.image_position_patient[2];

				md.pixel_spacing[0] = vsizeX;
				md.pixel_spacing[1] = vsizeY;
				md.slice_distance = vsizeZ;
				md.slice_thickness = vsizeZ;
			}
		}

		delete dlg;

	}
}


void CContextMenu::slot_volumetric_sift_cloud()
{
	// Liczba kluczowych punkt�w do zachowania.Domy�lnie 0, co oznacza, �e nie ma limitu.
	int nfeatures = 0;

	// Liczba warstw w ka�dej oktawie.Domy�lnie 3
	int nOctaveLayers = 3;

	// Pr�g eliminacji kluczowych punkt�w o niskim kontra�cie.Domy�lnie 0.04
	double contrastThreshold = 0.04;

	// Pr�g eliminacji kluczowych punkt�w na kraw�dziach.Domy�lnie 10
	double edgeThreshold = 10.0;

	// Pocz�tkowa sigma dla Gaussowskiego rozmycia.Domy�lnie 1.6
	double sigma = 1.6;

	int factor = 1;

	QDialog* dlg = new QDialog();
	QFormLayout* formLayout = new QFormLayout();

	QLineEdit* input1 = new QLineEdit(dlg);
	//input1->setValidator(new QIntValidator(0, 999999999999999999));
	input1->setText(QString::number(nfeatures));
	formLayout->addRow("nfeatures:", input1);

	QLineEdit* input2 = new QLineEdit(dlg);
	//input2->setValidator(new QIntValidator(0, 999999999999999999));
	input2->setText(QString::number(nOctaveLayers));
	formLayout->addRow("nOctaveLayers:", input2);

	QLineEdit* input3 = new QLineEdit(dlg);
	//input3->setValidator(new QIntValidator(0, 999999999999999999));
	input3->setText(QString::number(contrastThreshold));
	formLayout->addRow("contrastThreshold:", input3);

	QLineEdit* input4 = new QLineEdit(dlg);
	//input3->setValidator(new QIntValidator(0, 999999999999999999));
	input4->setText(QString::number(edgeThreshold));
	formLayout->addRow("edgeThreshold:", input4);

	QLineEdit* input5 = new QLineEdit(dlg);
	//input3->setValidator(new QIntValidator(0, 999999999999999999));
	input5->setText(QString::number(sigma));
	formLayout->addRow("Sigma:", input5);

	//formLayout->addItem()>addRow("Factor:", input6);

	QLineEdit* input6 = new QLineEdit(dlg);
	//input3->setValidator(new QIntValidator(0, 999999999999999999));
	input6->setText(QString::number(factor));
	formLayout->addRow("Factor:", input6);
	input6->setDisabled(true);

	QPushButton* okButton = new QPushButton("OK", dlg);
	QPushButton* cancelButton = new QPushButton("Cancel", dlg);

	QObject::connect(okButton, SIGNAL(clicked()), dlg, SLOT(accept()));
	QObject::connect(cancelButton, SIGNAL(clicked()), dlg, SLOT(reject()));

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addLayout(formLayout);
	layout->addWidget(okButton);
	layout->addWidget(cancelButton);
	
	dlg->setLayout(layout);

	if (dlg->exec())
	{
		qInfo() << "OK";

		nfeatures = input1->text().toInt();
		nOctaveLayers = input2->text().toInt();
		contrastThreshold = input3->text().replace(',', '.').toDouble();
		edgeThreshold = input4->text().replace(',', '.').toDouble();
		sigma = input5->text().replace(',', '.').toDouble();
		factor = input6->text().toInt();

		std::shared_ptr<CPointCloud> cloud = std::static_pointer_cast<Volumetric>(m_obj)->sift_cloud(nfeatures, nOctaveLayers, contrastThreshold, edgeThreshold, sigma, factor);

		AP::OBJECT::addChild(m_obj, cloud);

		UI::updateAllViews();
	}

	delete dlg;
}

void CContextMenu::slot_volumetric_marching_cube()
{
	QDialog* dlg = new QDialog();

	QLineEdit* input1 = new QLineEdit(dlg);
	input1->setValidator(new QIntValidator(1, 16));

	input1->setText("4");

	QFormLayout* formLayout = new QFormLayout();
	formLayout->addRow("set factor:", input1);

	QPushButton* okButton = new QPushButton("OK", dlg);
	QPushButton* cancelButton = new QPushButton("Cancel", dlg);

	QObject::connect(okButton, SIGNAL(clicked()), dlg, SLOT(accept()));
	QObject::connect(cancelButton, SIGNAL(clicked()), dlg, SLOT(reject()));

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addLayout(formLayout);
	layout->addWidget(okButton);
	layout->addWidget(cancelButton);

	dlg->setLayout(layout);

	if (dlg->exec())
	{
		qInfo() << "OK";

		int f = input1->text().toInt();

		std::shared_ptr<CMesh> mesh = std::static_pointer_cast<Volumetric>(m_obj)->marching_cube(f);
		
		AP::OBJECT::addChild(m_obj, mesh);
		UI::updateAllViews();
	}

	delete dlg;
}

void CContextMenu::slot_volumetric_marching_tetra()
{
	QDialog* dlg = new QDialog();

	QLineEdit* input1 = new QLineEdit(dlg);
	input1->setValidator(new QIntValidator(1, 16));

	input1->setText("4");

	QFormLayout* formLayout = new QFormLayout();
	formLayout->addRow("set factor:", input1);

	QPushButton* okButton = new QPushButton("OK", dlg);
	QPushButton* cancelButton = new QPushButton("Cancel", dlg);

	QObject::connect(okButton, SIGNAL(clicked()), dlg, SLOT(accept()));
	QObject::connect(cancelButton, SIGNAL(clicked()), dlg, SLOT(reject()));

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addLayout(formLayout);
	layout->addWidget(okButton);
	layout->addWidget(cancelButton);

	dlg->setLayout(layout);

	if (dlg->exec())
	{
		qInfo() << "OK";

		int f = input1->text().toInt();

		std::shared_ptr<CMesh> mesh = std::static_pointer_cast<Volumetric>(m_obj)->marching_tetrahedron(f);

		AP::OBJECT::addChild(m_obj, mesh);

		UI::updateAllViews();
	}

	delete dlg;
}

/*********************************************************************************************/

void CContextMenu::pointHide()
{
	m_obj->switchSelfVisibility();
	//m_obj->setVisible(!m_obj->isVisible());
	
	UI::updateAllViews();
}

#include "MainWindow.h"

void CContextMenu::newPicWindow()
{
	AP::mainWin().createPicViewer((CImage*)m_obj.get());
}

#include "DockWidgetWorkspace.h"
void CContextMenu::slotCollapseAll()
{
	DockWidgetWorkspace* dock = UI::DOCK::WORKSPACE::instance();
	dock->collapseAll();
}

void CContextMenu::slotExpandAll()
{
	DockWidgetWorkspace* dock = UI::DOCK::WORKSPACE::instance();
	dock->expandAll();
}

void CContextMenu::slotHideAll()
{
	m_obj->showChildren(false, {}, {CBaseObject::Type::MODEL});
	UI::updateAllViews();
	DockWidgetWorkspace* dock = UI::DOCK::WORKSPACE::instance();
	dock->updateVisibilityAll();
}

void CContextMenu::slotShowAll()
{
	m_obj->showChildren(true, {}, { CBaseObject::Type::MODEL });
	UI::updateAllViews();
	DockWidgetWorkspace* dock = UI::DOCK::WORKSPACE::instance();
	dock->updateVisibilityAll();
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
		AP::OBJECT::addChild(m_obj, std::make_shared<CAnnotationPoint>());
		break;

	case CBaseObject::Type::PLANE:
		AP::OBJECT::addChild(m_obj, std::make_shared<CAnnotationPlane>());
		break;

	case CBaseObject::Type::SPHERE:
		AP::OBJECT::addChild(m_obj, std::make_shared<CAnnotationSphere>());
		break;
	}
}


void CContextMenu::slotPlaneToMesh()
{
	std::shared_ptr<CMesh> plane = std::static_pointer_cast<CAnnotationPlane>(m_obj)->getMesh(100.0, 100, 100);
	AP::OBJECT::addChild(m_obj->getParentPtr(), plane);
}

//CTransform globalTransformation(CBaseObject* obj)
//{
//	return CTransform(obj->getGlobalTransformationMatrix());
//}

#include "AnnotationTriangle.h"

void CContextMenu::slotTriangleToPlane()
{
	std::shared_ptr<CAnnotationTriangle> tri = std::static_pointer_cast<CAnnotationTriangle>(m_obj);
	std::shared_ptr<CAnnotationPlane> plane = std::make_shared<CAnnotationPlane>(CPlane(tri->A(), tri->B(), tri->C()));
	std::shared_ptr<CBaseObject> p = m_obj->getParentPtr();
	if (p != nullptr)
		AP::OBJECT::addChild(p, plane);
}