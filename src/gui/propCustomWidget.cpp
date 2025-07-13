#include "propBaseObject.h"
#include "propCustomWidget.h"
#include "UI.h"

#include <QColorDialog>
#include <QPushButton>
#include <QVBoxLayout>

PropCustomWidget::PropCustomWidget(CBaseObject *mesh, QWidget *parent) : PropWidget( parent )
{
	obj = mesh; 

	if (obj->has_prop_widget()) {
		treeItemLabel = "Custom properties";

		QVBoxLayout* layout = new QVBoxLayout(this);

		QWidget* w = obj->prop_widget();

		if (w != nullptr) {
			layout->addWidget(w);
		}

		this->resize(layout->sizeHint());
		this->setMinimumSize(layout->sizeHint());
		this->setMaximumSize(layout->sizeHint());

		this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

		//ui.setupUi(this);

		//connect(ui.cloud, &QGroupBox::toggled, this, &PropCustomWidget::adjustGroupHeight);

		//cd = new QColorDialog(this);
		//cd->setOptions(QColorDialog::DontUseNativeDialog | QColorDialog::ShowAlphaChannel);
		//QObject::connect(cd, SIGNAL(currentColorChanged(QColor)), this, SLOT(meshColorChanged(QColor)));
	}
}

PropCustomWidget::~PropCustomWidget()
{
	//delete cd;
}

PropWidget* PropCustomWidget::create(CBaseObject* m, QWidget* parent)
{
	PropWidget* widget = new PropWidget(parent);
	QVBoxLayout* layout = new QVBoxLayout(widget);

	//layout->addWidget(new PropBaseObject(m, widget));
	//layout->addWidget(new PropMaterial(m, widget));
	layout->addWidget(new PropCustomWidget(m, widget));
	//layout->addWidget(new PropTransform(&m->getTransform(), widget));

	widget->resize(layout->sizeHint());
	widget->setMinimumSize(layout->sizeHint());
	widget->setMaximumSize(layout->sizeHint());

	widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	return widget;
}

QVector<PropWidget*> PropCustomWidget::create_and_get_subwidgets(CBaseObject* obj)
{
	return QVector<PropWidget*>({
	//	new PropBaseObject(obj),
		new PropCustomWidget(obj)
	});
}


void PropCustomWidget::updateProperties()
{
	obj->prop_widget_update();
}

