#include "propMesh.h"
#include "Mesh.h"
#include "UI.h"

#include <QColorDialog>
#include <QPushButton>
#include <QVBoxLayout>

bool PropMesh::group_visible = true;

PropMesh::PropMesh(CMesh *mesh, QWidget *parent) : PropWidget( parent )
{
	obj = mesh; 

	treeItemLabel = "Mesh properties";

	ui.setupUi(this);
}

PropMesh::~PropMesh()
{
	//delete cd;
}

PropWidget* PropMesh::create(CMesh* m, QWidget* parent)
{
	return PropWidget::build(PropMesh::create_and_get_subwidgets(m), parent);
}

QVector<PropWidget*> PropMesh::create_and_get_subwidgets(CBaseObject* obj)
{
	CMesh* m = (CMesh*)obj;
	return QVector<PropWidget*>({
		new PropBaseObject(m, nullptr),
		new PropMaterial(m, nullptr),
		new PropPointCloud(m, nullptr),
		new PropMesh(m, nullptr) });
}

//void PropMesh::updateSliders(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
//{
//	ui.sliderR->blockSignals(true);
//	ui.sliderR->setValue(r);
//	ui.sliderR->blockSignals(false);
//
//	ui.sliderG->blockSignals(true);
//	ui.sliderG->setValue(g);
//	ui.sliderG->blockSignals(false);
//
//	ui.sliderB->blockSignals(true);
//	ui.sliderB->setValue(b);
//	ui.sliderB->blockSignals(false);
//
//	ui.sliderA->blockSignals(true);
//	ui.sliderA->setValue(a);
//	ui.sliderA->blockSignals(false);
//}
//
//void PropMesh::updateEditBox(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
//{
//	std::string ss = CRGBA( r, g, b, a ).asHexRGBA();
//
//	QString s = QString::fromStdString(ss);
//
//	ui.editRGBA->blockSignals(true);
//	ui.editRGBA->setText(s);
//	ui.editRGBA->blockSignals(false);
//}



void PropMesh::updateProperties()
{
	QString info = "Faces: " + QString::number(((CMesh*)obj)->faces().size()) + "\n";
	info += "F-Colors: " + QString::number(((CMesh*)obj)->fcolors().size()) + "\n";
	info += "F-Normals: " + QString::number(((CMesh*)obj)->fnormals().size()) + "\n";

	ui.info->blockSignals(true);
	ui.info->setText(info);// QString::fromStdWString(obj->infoRow()) );
	ui.info->blockSignals(false);
}

//void PropMesh::rValueChanged(int r)
//{
//	if (obj->type() != CBaseObject::MESH) return;
//
//	((CMesh*)obj)->getMaterial(0).FrontColor.ambient.setRed(r);
//	((CMesh*)obj)->getMaterial(0).FrontColor.diffuse.setRed(r);
//
//	CRGBA c = ((CMesh*)obj)->getMaterial(0).FrontColor.ambient; 
//	updateEditBox(c.red(), c.green(), c.blue(), c.alpha());
//
//	UI::updateAllViews();
//}
//
//void PropMesh::gValueChanged(int g)
//{
//	if (obj->type() != CBaseObject::MESH) return;
//
//	((CMesh*)obj)->getMaterial(0).FrontColor.ambient.setGreen(g);
//	((CMesh*)obj)->getMaterial(0).FrontColor.diffuse.setGreen(g);
//
//	CRGBA c = ((CMesh*)obj)->getMaterial(0).FrontColor.ambient;
//	updateEditBox(c.red(), c.green(), c.blue(), c.alpha());
//
//	UI::updateAllViews();
//}
//
//void PropMesh::bValueChanged(int b)
//{
//	if (obj->type() != CBaseObject::MESH) return;
//
//	((CMesh*)obj)->getMaterial(0).FrontColor.ambient.setBlue(b);
//	((CMesh*)obj)->getMaterial(0).FrontColor.diffuse.setBlue(b);
//
//	CRGBA c = ((CMesh*)obj)->getMaterial(0).FrontColor.ambient;
//	updateEditBox(c.red(), c.green(), c.blue(), c.alpha());
//
//	UI::updateAllViews();
//}
//
//void PropMesh::aValueChanged(int a)
//{
//	if (obj->type() != CBaseObject::MESH) return;
//
//	((CMesh*)obj)->getMaterial(0).FrontColor.ambient.setAlpha(a);
//	((CMesh*)obj)->getMaterial(0).FrontColor.diffuse.setAlpha(a);
//
//	CRGBA c = ((CMesh*)obj)->getMaterial(0).FrontColor.ambient;
//	updateEditBox(c.red(), c.green(), c.blue(), c.alpha());
//
//	UI::updateAllViews();
//}
//
//void PropMesh::rgbaTextChanged(QString s)
//{
//	if (obj->type() != CBaseObject::MESH) return;
//
//	CRGBA c = CRGBA::fromHexRGBA( s.toStdString() );
//
//	((CMesh*)obj)->getMaterial(0).FrontColor.ambient = c;
//	((CMesh*)obj)->getMaterial(0).FrontColor.diffuse = c;
//
//	updateSliders(c.red(), c.green(), c.blue(), c.alpha());
//
//	UI::updateAllViews();
//}
//
//void PropMesh::meshColorChanged(QColor c)
//{
//	if (obj->type() != CBaseObject::MESH) return;
//
//	if (c.isValid())
//	{
//		((CMesh*)obj)->getMaterial(0).FrontColor.ambient.Set(c.red(), c.green(), c.blue(), c.alpha());
//		((CMesh*)obj)->getMaterial(0).FrontColor.diffuse.Set(c.red(), c.green(), c.blue(), c.alpha());
//
//		updateSliders(c.red(), c.green(), c.blue(), c.alpha());
//		updateEditBox(c.red(), c.green(), c.blue(), c.alpha());
//
//		UI::updateAllViews();
//	}
//}
//
//void PropMesh::colorButtonClicked()
//{
//	if (obj->type() != CBaseObject::MESH) return;
//
//	QPushButton *s = (QPushButton *)sender();
//
//	QColor c = s->palette().button().color();
//	c.setHsv(c.hsvHue(), c.hsvSaturation(), c.value() / 2, c.alpha());
//
//	meshColorChanged( c );
//}

//void PropMesh::testButtonClicked()
//{
//	CRGBA c = ((CMesh*)obj)->getMaterial().FrontColor.ambient;
//	QColor color = QColor::fromRgb(c.red(), c.green(), c.blue(), c.alpha());
//	
//	cd->setCurrentColor(color);
//	cd->setVisible(true);
//}
