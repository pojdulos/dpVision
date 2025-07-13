#include "propDataChild.h"
#include "MainWindow.h"
#include "UI.h"
#include "AP.h"
#include <QVBoxLayout>

PropDataChild::PropDataChild(CBaseObject *m, QWidget *parent) : PropWidget( parent )
{
	obj = m;
	//setSelectionEnabled(false);
}

PropDataChild::~PropDataChild()
{
}

PropWidget* PropDataChild::create(CBaseObject* m, QWidget* parent)
{
	PropWidget* widget = new PropWidget(parent);
	QVBoxLayout* layout = new QVBoxLayout(widget);

	layout->addWidget(new PropBaseObject(m, widget));
	//layout->addWidget(new PropTransform(&m->getTransform(), widget));

	widget->resize(layout->sizeHint());
	widget->setMinimumSize(layout->sizeHint());
	widget->setMaximumSize(layout->sizeHint());

	widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	return widget;
}

void PropDataChild::updateProperties()
{
	//PropBaseObject::updateProperties();
}

