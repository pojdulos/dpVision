#include "propMaterial.h"
#include "PointCloud.h"
#include "UI.h"

#include <QColorDialog>
#include <QPushButton>
#include <QVBoxLayout>


PropMaterial::PropMaterial(CBaseObject *mesh, QWidget *parent) : PropWidget( parent )
{
	obj = mesh;
	ui.setupUi(this);

	//cd = new QColorDialog(this);
	//cd->setOptions(QColorDialog::DontUseNativeDialog | QColorDialog::ShowAlphaChannel);
	//QObject::connect(cd, SIGNAL(currentColorChanged(QColor)), this, SLOT(meshColorChanged(QColor)));
}

PropMaterial::~PropMaterial()
{
	//delete cd;
}

PropWidget* PropMaterial::create(CBaseObject* m, QWidget* parent)
{
	PropWidget* widget = new PropWidget(parent);
	QVBoxLayout* layout = new QVBoxLayout(widget);

	layout->addWidget(new PropBaseObject(m, widget));
	layout->addWidget(new PropMaterial(m, widget));
	//layout->addWidget(new PropTransform(&m->getTransform(), widget));

	widget->resize(layout->sizeHint());
	widget->setMinimumSize(layout->sizeHint());
	widget->setMaximumSize(layout->sizeHint());

	widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	return widget;
}

void PropMaterial::updateSliders(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	ui.sliderR->blockSignals(true);
	ui.sliderR->setValue(r);
	ui.sliderR->blockSignals(false);

	ui.sliderG->blockSignals(true);
	ui.sliderG->setValue(g);
	ui.sliderG->blockSignals(false);

	ui.sliderB->blockSignals(true);
	ui.sliderB->setValue(b);
	ui.sliderB->blockSignals(false);

	ui.sliderA->blockSignals(true);
	ui.sliderA->setValue(a);
	ui.sliderA->blockSignals(false);
}

void PropMaterial::updateEditBox(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	std::string ss = CRGBA( r, g, b, a ).asHexRGBA();

	QString s = QString::fromStdString(ss);

	ui.editRGBA->blockSignals(true);
	ui.editRGBA->setText(s);
	ui.editRGBA->blockSignals(false);
}


void PropMaterial::updateProperties()
{
	//PropBaseObject::updateProperties();

		CRGBA c = ((CPointCloud*)obj)->getMaterial(0).FrontColor.ambient;

		updateSliders(c.red(), c.green(), c.blue(), c.alpha());
		updateEditBox(c.red(), c.green(), c.blue(), c.alpha());

		ui.checkShow->blockSignals(true);
		ui.checkShow->setChecked(((CPointCloud*)obj)->getMaterial(0).m_force);

		ui.checkShow->setEnabled(((CPointCloud*)obj)->hasVertexColors());
		ui.checkShow->setToolTip("Use this checkbox to hide original vertex colors and use selected material.\nIf your model has a texture instead of vertex colors, the checkbox is grayed.\nYou should to hide the texture (menu or key \"T\") to use the material.");
		//ui.checkShow->setToolTip(AP::::GetResourceString(IDS_PROP_MATERIAL_CHECK));
		
		ui.checkShow->blockSignals(false);
}

void PropMaterial::rValueChanged(int r)
{
	((CPointCloud*)obj)->getMaterial(0).FrontColor.ambient.setRed(r);
	((CPointCloud*)obj)->getMaterial(0).FrontColor.diffuse.setRed(r);

	CRGBA c = ((CPointCloud*)obj)->getMaterial(0).FrontColor.ambient;
	updateEditBox(c.red(), c.green(), c.blue(), c.alpha());

	UI::updateAllViews();
}

void PropMaterial::gValueChanged(int g)
{
	((CPointCloud*)obj)->getMaterial(0).FrontColor.ambient.setGreen(g);
	((CPointCloud*)obj)->getMaterial(0).FrontColor.diffuse.setGreen(g);

	CRGBA c = ((CPointCloud*)obj)->getMaterial(0).FrontColor.ambient;
	updateEditBox(c.red(), c.green(), c.blue(), c.alpha());

	UI::updateAllViews();
}

void PropMaterial::bValueChanged(int b)
{
	((CPointCloud*)obj)->getMaterial(0).FrontColor.ambient.setBlue(b);
	((CPointCloud*)obj)->getMaterial(0).FrontColor.diffuse.setBlue(b);

	CRGBA c = ((CPointCloud*)obj)->getMaterial(0).FrontColor.ambient;
	updateEditBox(c.red(), c.green(), c.blue(), c.alpha());

	UI::updateAllViews();
}

void PropMaterial::aValueChanged(int a)
{
	((CPointCloud*)obj)->getMaterial(0).FrontColor.ambient.setAlpha(a);
	((CPointCloud*)obj)->getMaterial(0).FrontColor.diffuse.setAlpha(a);

	CRGBA c = ((CPointCloud*)obj)->getMaterial(0).FrontColor.ambient;
	updateEditBox(c.red(), c.green(), c.blue(), c.alpha());

	UI::updateAllViews();
}

void PropMaterial::rgbaTextChanged(QString s)
{
	CRGBA c = CRGBA::fromHexRGBA( s.toStdString() );

	((CPointCloud*)obj)->getMaterial(0).FrontColor.ambient = c;
	((CPointCloud*)obj)->getMaterial(0).FrontColor.diffuse = c;

	updateSliders(c.red(), c.green(), c.blue(), c.alpha());

	UI::updateAllViews();
}

void PropMaterial::meshColorChanged(QColor c)
{
	if (c.isValid())
	{
		((CPointCloud*)obj)->getMaterial(0).FrontColor.ambient.Set(c.red(), c.green(), c.blue(), c.alpha());
		((CPointCloud*)obj)->getMaterial(0).FrontColor.diffuse.Set(c.red(), c.green(), c.blue(), c.alpha());

		updateSliders(c.red(), c.green(), c.blue(), c.alpha());
		updateEditBox(c.red(), c.green(), c.blue(), c.alpha());

		UI::updateAllViews();
	}
}

void PropMaterial::colorButtonClicked()
{
	QPushButton *s = (QPushButton *)sender();

	QColor c = s->palette().button().color();
	c.setHsv(c.hsvHue(), c.hsvSaturation(), c.value() / 2, c.alpha());

	meshColorChanged( c );
}

void PropMaterial::checkShowClicked(int i)
{
	((CPointCloud*)obj)->getMaterial(0).m_force = i!=0;
	UI::updateAllViews();
}

