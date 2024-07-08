#include "propPointCloud.h"
#include "PointCloud.h"
#include "Mesh.h"
#include "UI.h"

#include <QColorDialog>
#include <QPushButton>
#include <QVBoxLayout>

bool PropPointCloud::group_visible = true;

PropPointCloud::PropPointCloud(CPointCloud*mesh, QWidget *parent) : PropWidget( parent )
{
	obj = mesh; 

	// Usuñ istniej¹cy layout
	QLayout* oldLayout = layout();
	if (oldLayout) {
		QLayoutItem* item;
		while ((item = oldLayout->takeAt(0)) != nullptr) {
			delete item->widget();
			delete item;
		}
		delete oldLayout;
	}

	ui.setupUi(this);

	connect(ui.cloud, &QGroupBox::toggled, this, &PropPointCloud::adjustGroupHeight);

	//cd = new QColorDialog(this);
	//cd->setOptions(QColorDialog::DontUseNativeDialog | QColorDialog::ShowAlphaChannel);
	//QObject::connect(cd, SIGNAL(currentColorChanged(QColor)), this, SLOT(meshColorChanged(QColor)));
}

PropPointCloud::~PropPointCloud()
{
	//delete cd;
}

PropWidget* PropPointCloud::create(CPointCloud* m, QWidget* parent)
{
	PropWidget* widget = new PropWidget(parent);
	QVBoxLayout* layout = new QVBoxLayout(widget);

	layout->addWidget(new PropBaseObject(m, widget));
	layout->addWidget(new PropMaterial(m, widget));
	layout->addWidget(new PropPointCloud(m, widget));
	//layout->addWidget(new PropTransform(&m->getTransform(), widget));

	widget->resize(layout->sizeHint());
	widget->setMinimumSize(layout->sizeHint());
	widget->setMaximumSize(layout->sizeHint());

	widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	return widget;
}

QVector<PropWidget*> PropPointCloud::create_and_get_subwidgets(CBaseObject* obj)
{
	CPointCloud* m = (CPointCloud*)obj;
	return QVector<PropWidget*>({ new PropBaseObject(m), new PropMaterial(m), new PropPointCloud(m) });
}

//void PropPointCloud::updateSliders(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
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
//void PropPointCloud::updateEditBox(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
//{
//	std::string ss = CRGBA( r, g, b, a ).asHexRGBA();
//
//	QString s = QString::fromStdString(ss);
//
//	ui.editRGBA->blockSignals(true);
//	ui.editRGBA->setText(s);
//	ui.editRGBA->blockSignals(false);
//}


void PropPointCloud::updateProperties()
{
	//PropMaterial::updateProperties();


	switch (obj->type())
	{
		case CBaseObject::ORDEREDCLOUD:
			ui.cloud->setTitle("Ordered Point Cloud");
			break;

		case CBaseObject::CLOUD:
		default:
			ui.cloud->setTitle("Point Cloud");

			break;
	}

	//QString s("background-color: rgb(" + QString::number(a->getColor().R()) + ", " + QString::number(a->getColor().G()) + ", " + QString::number(a->getColor().B()) + ");");
	//ui.colorGroupBox->setStyleSheet(s);

	ui.spinPointSize->blockSignals(true);
	ui.spinPointSize->setValue(CPointCloud::m_pointSize);
	ui.spinPointSize->blockSignals(false);

	QString info = "Vertices: " + QString::number(((CPointCloud*)obj)->vertices().size()) + "\n";
	 info += "V-Colors: " + QString::number(((CPointCloud*)obj)->vcolors().size()) + "\n";
	 info += "V-Normals: " + QString::number(((CPointCloud*)obj)->vnormals().size()) + "\n";

	ui.info->blockSignals(true);
	ui.info->setText(info); // QString::fromStdWString(obj->infoRow()) );
	ui.info->blockSignals(false);
}


void PropPointCloud::adjustGroupHeight(bool checked) {
	PropPointCloud::group_visible = checked;
	UI::adjustGroupBoxHeight(ui.cloud, PropPointCloud::group_visible);
	this->adjustSize();
	this->parentWidget()->adjustSize();
}



void PropPointCloud::pointSizeChanged(int s)
{
	CPointCloud::m_pointSize = s;

	UI::updateAllViews();
}


//void PropPointCloud::rValueChanged(int r)
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
//void PropPointCloud::gValueChanged(int g)
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
//void PropPointCloud::bValueChanged(int b)
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
//void PropPointCloud::aValueChanged(int a)
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
//void PropPointCloud::rgbaTextChanged(QString s)
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
//void PropPointCloud::meshColorChanged(QColor c)
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
//void PropPointCloud::colorButtonClicked()
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

//void PropPointCloud::testButtonClicked()
//{
//	CRGBA c = ((CMesh*)obj)->getMaterial().FrontColor.ambient;
//	QColor color = QColor::fromRgb(c.red(), c.green(), c.blue(), c.alpha());
//	
//	cd->setCurrentColor(color);
//	cd->setVisible(true);
//}
