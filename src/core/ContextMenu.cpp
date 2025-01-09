#include "ContextMenu.h"

#include "AP.h"
#include "UI.h"
#include "MainWindow.h"
#include "Workspace.h"
#include "BaseObject.h"
#include "Object.h"

CContextMenu::CContextMenu(CBaseObject *obj, QWidget *parent) : QMenu(parent), m_obj(obj)
{
	addAction("expand all", this, SLOT(slotExpandAll()));
	addAction("collapse all", this, SLOT(slotCollapseAll()));
	addSeparator();
	addMenu(createAddObjectMenu());
	addSeparator();
	if (m_obj == nullptr)
	{
		//addAction("Create empty model", this, SLOT(slotCreateEmptyModel()));
		//addSeparator();
		addMenu(AP::mainWin().ui.menuWorkspace);
		addMenu(AP::mainWin().ui.menuCamera);
	}
	else
	{
		addSeparator();
		addAction("show all under this", this, SLOT(slotShowAll()));
		addAction("hide all under this", this, SLOT(slotHideAll()));
		switch (m_obj->category())
		{
		case CBaseObject::Category::ANNOTATION:
			addMenu(createCopyMenu());
			addMenu(createMoveMenu());
			addSeparator();
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
			case CBaseObject::Type::PLANE:
				addAction(QIcon(":/icons/Save.ico"), "to Mesh...", this, SLOT(slotPlaneToMesh()));
				addSeparator();
				addAction(QIcon(":/icons/Visible.ico"), "show/hide", this, SLOT(pointHide()));
				addAction(QIcon(":/icons/Erase.ico"), "delete", this, SLOT(slotDeleteObject()));
				break;
			case CBaseObject::Type::TRIPLE:
				addAction(QIcon(":/icons/Save.ico"), "to Plane...", this, SLOT(slotTriangleToPlane()));
				addSeparator();
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
				addSeparator();
				addAction(QIcon(":/icons/Save.ico"), "Save as...", this, SLOT(saveObjAs()));
				addSeparator();
				addAction("apply last transformation and move up", this, SLOT(slot_apply_last_transform()));
				addAction("create reverse transformation", this, SLOT(slot_create_inversed_transform()));
				addSeparator();
				addMenu(createCopyMenu());
				addMenu(createMoveMenu());
				addSeparator();
				addMenu(createAddAnnoMenu());
				addSeparator();
				addMenu(AP::mainWin().ui.menuModel);
				addSeparator();
				addAction(QIcon(":/icons/Erase.ico"), "delete and keep childeren", this, SLOT(slot_delete_and_keep_children()));
				addAction(QIcon(":/icons/Erase.ico"), "delete all below", this, SLOT(slotDeleteObject()));
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
			case CBaseObject::Type::VOLUMETRIC_NEW:
				addMenu(createCopyMenu());
				addMenu(createMoveMenu());
				addSeparator();
				addMenu(createVolumetricMenu());
				addSeparator();
				addAction(QIcon(":/icons/Erase.ico"), "delete", this, SLOT(slotDeleteObject()));
				break;
			//case CBaseObject::Type::VOLTK:
			//	addAction("show as images", this, SLOT(volTKshowImagesDlg()));
			//	addSeparator();
			//	addMenu(createCopyMenu());
			//	addAction(QIcon(":/icons/Save.ico"), "convert to cloud of points", this, SLOT(volTKtoCloud()));
			//	addAction(QIcon(":/icons/Save.ico"), "convert to mesh", this, SLOT(volTKtoMesh()));
			//	//addAction(QIcon(":/icons/Visible.ico"), "show/hide", this, SLOT(pointHide()));
			//	addSeparator();
			//	addAction(QIcon(":/icons/Erase.ico"), "delete", this, SLOT(slotDeleteObject()));
			//	break;
			case CBaseObject::Type::CLOUD:
			case CBaseObject::Type::MESH:
				addAction(QIcon(":/icons/Save.ico"), "export as...", this, SLOT(saveObjAs()));
				addSeparator();
				addAction("apply last transformation and move up", this, SLOT(slot_apply_last_transform()));
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
	menu->addAction("...new model", this, SLOT(copyTo()))->setData(QVariant::fromValue(nullptr));
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
	menu->addAction("...new model", this, SLOT(moveTo()))->setData(QVariant::fromValue(nullptr));
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


#include "PointCloud.h"
#include "Model3D.h"

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

	AP::OBJECT::moveTo(m_obj, newParent);
}


void CContextMenu::copyTo()
{
	QAction* action = (QAction*)QObject::sender();

	CBaseObject* newParent = (CBaseObject*)action->data().value<void*>();

	AP::OBJECT::copyTo(m_obj, newParent);
}


#include <QFileDialog>
#include "FileConnector.h"

void CContextMenu::saveObjAs()
{
	CModel3D* tmpObj;
	if (m_obj->hasType(CBaseObject::Type::MODEL))
	{
		tmpObj = (CModel3D*) m_obj;
	}
	else
	{
		tmpObj = new CModel3D;
		tmpObj->addChild(m_obj);
	}

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
}

void CContextMenu::slot_apply_last_transform()
{
	// Kopiujemy obiekt z rodzica do dziadka,
	// przekształcając go tak by zachował pozycję
	
	

	if (m_obj == nullptr)
		return; 	// obiekt nie istnieje

	CBaseObject* parent = m_obj->getParent();
	

	if ( (parent == nullptr) || (!parent->hasType(CBaseObject::Type::MODEL)) )
		return;	// rodzicem obiektu nie jest przekształcenie

	CTransform parent_transform = ((CModel3D*)parent)->transform();

	CBaseObject* grandpa = parent->getParent();
	
	if ( (grandpa != nullptr) && (!grandpa->hasType(CBaseObject::Type::MODEL)) )
		return;	// dziadkiem obiektu nie jest przekształcenie lub workspace

	CTransform grandpa_transform; // zerowe przekształcenie 

	if (m_obj->hasType(CBaseObject::Type::MODEL))
	{
		CModel3D* mdl = (CModel3D*)m_obj;
		Eigen::Matrix4d T = mdl->transform().toEigenMatrix4d();
		Eigen::Matrix4d Tp = parent_transform.toEigenMatrix4d();

		T = T * Tp;

		mdl->transform().fromEigenMatrix4d(T);

		AP::OBJECT::removeChild(parent, mdl);
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
		CPointCloud* mesh = (CPointCloud*)m_obj;
		mesh->applyTransformation(parent_transform, grandpa_transform);

		if (grandpa == nullptr)
		{
			grandpa = new CModel3D();
			AP::WORKSPACE::addModel((CModel3D*)grandpa);
		}

		AP::OBJECT::removeChild(parent, mesh);
		AP::OBJECT::addChild(grandpa, mesh);
	}
}

void CContextMenu::slot_delete_and_keep_children()
{
	if (m_obj->hasType(CBaseObject::Type::MODEL))
	{
		CBaseObject* p = m_obj->getParent();
		if (p != nullptr)
		{
			for (auto d : ((CObject*)m_obj)->children())
			{
				CBaseObject* c = d.second;
				AP::OBJECT::addChild(p, c);
			}
			((CObject*)m_obj)->children().clear();

			for (auto d : ((CObject*)m_obj)->annotations())
			{
				AP::OBJECT::addChild(p, (CBaseObject*)d.second);
			}
			((CObject*)m_obj)->annotations().clear();
			AP::WORKSPACE::removeModel((CModel3D*)m_obj, true);
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
		CModel3D* obj = new CModel3D();
		obj->transform().fromEigenMatrix4d( ((CModel3D*)m_obj)->transform().toEigenMatrix4d().inverse() );
		obj->setLabel(m_obj->getLabel() + ".inv");
		obj->resetBoundingBox(CBoundingBox::NullBB);

		CBaseObject *p = m_obj->getParent();
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

	if (m_obj && m_obj->hasCategory(CBaseObject::Category::OBJECT) ) {
		AP::OBJECT::addChild(m_obj, m);
	}
	else {

		AP::WORKSPACE::addModel(m);
	}

}

void CContextMenu::slot_mesh_create()
{
	CMesh* m = new CMesh();
	if (m != nullptr) AP::OBJECT::addChild(m_obj, m);
}

/*********************************************************************************************/

#include "Volumetric.h"

void CContextMenu::slot_volum_create()
{
	Volumetric *m = Volumetric::create();
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
		VolumetricImageDialog* dialog = new VolumetricImageDialog((Volumetric*)m_obj);

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
		parser->save({ (Volumetric*)m_obj }, file_path);
	}
}

void CContextMenu::slot_volumetric_set_metadata()
{
	float* origin = ((Volumetric*)m_obj)->metadata[0].image_position_patient;

	float vsizeX = ((Volumetric*)m_obj)->metadata[1].pixel_spacing[0];
	float vsizeY = ((Volumetric*)m_obj)->metadata[1].pixel_spacing[1];
	float vsizeZ = ((Volumetric*)m_obj)->metadata[1].slice_distance;

	int shape[] = {
	((Volumetric*)m_obj)->layers(),
	((Volumetric*)m_obj)->rows(),
	((Volumetric*)m_obj)->columns()
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

		for (int i=0; i<((Volumetric*)m_obj)->metadata.size(); i++)
		{
			auto& md = ((Volumetric*)m_obj)->metadata[i];

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

		CPointCloud *cloud = ((Volumetric*)m_obj)->sift_cloud(nfeatures, nOctaveLayers, contrastThreshold, edgeThreshold, sigma, factor);

		AP::OBJECT::addChild((Volumetric*)m_obj, cloud);

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

		CMesh* mesh = ((Volumetric*)m_obj)->marching_cube(f);
		
		AP::OBJECT::addChild((Volumetric*)m_obj, mesh);
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

		CMesh *mesh = ((Volumetric*)m_obj)->marching_tetrahedron(f);

		AP::OBJECT::addChild((Volumetric*)m_obj, mesh);

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
	AP::mainWin().createPicViewer((CImage*)m_obj);
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


void CContextMenu::slotPlaneToMesh()
{
	CMesh* plane = ((CAnnotationPlane*)m_obj)->getMesh(100.0, 100, 100);
	AP::OBJECT::addChild(m_obj->getParent(), plane);
}

//CTransform globalTransformation(CBaseObject* obj)
//{
//	return CTransform(obj->getGlobalTransformationMatrix());
//}

#include "AnnotationTriangle.h"

void CContextMenu::slotTriangleToPlane()
{
	CAnnotationTriangle* tri = (CAnnotationTriangle*)m_obj;
	CAnnotationPlane* plane = new CAnnotationPlane(CPlane(tri->A(), tri->B(), tri->C()));
	CBaseObject* p = m_obj->getParent();
	if (p != nullptr)
		AP::OBJECT::addChild(p, plane);
}