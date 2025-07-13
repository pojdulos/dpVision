#include "propAnnotationSetOfFaces.h"
#include "AnnotationSetOfFaces.h"
#include "propTransform.h"

#include "UI.h"
#include <QVBoxLayout>

PropAnnotationSetOfFaces::PropAnnotationSetOfFaces(CAnnotationSetOfFaces *an, QWidget *parent) : PropWidget(parent)//: PropAnnotation( an, parent )
{
	obj = an;
	ui.setupUi(this);
}

PropAnnotationSetOfFaces::~PropAnnotationSetOfFaces()
{
}

PropWidget* PropAnnotationSetOfFaces::create(CAnnotationSetOfFaces* m, QWidget* parent)
{
	PropWidget* widget = new PropWidget(parent);
	QVBoxLayout* layout = new QVBoxLayout(widget);

	layout->addWidget(new PropBaseObject(m, widget));
	layout->addWidget(new PropAnnotation(m, widget));
	//layout->addWidget(new PropTransform(&m->getTransform(), widget));
	layout->addWidget( new PropAnnotationSetOfFaces(m, widget) );

	widget->resize(layout->sizeHint());
	widget->setMinimumSize(layout->sizeHint());
	widget->setMaximumSize(layout->sizeHint());

	widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	return widget;
}

QVector<PropWidget*> PropAnnotationSetOfFaces::create_and_get_subwidgets(CBaseObject* obj)
{
	CAnnotationSetOfFaces* m = (CAnnotationSetOfFaces*)obj;
	return QVector<PropWidget*>({ new PropBaseObject(m), new PropAnnotation(m), new PropAnnotationSetOfFaces(m) });
}

void PropAnnotationSetOfFaces::updateProperties()
{
	PropWidget::updateProperties();
	
	//ui.spinPointSize->blockSignals(true);
	//ui.spinPointSize->setValue(obj->m_pointSize);
	//ui.spinPointSize->blockSignals(false);

	QString info = "Number of faces: " + QString::number(obj->size()) + "\n";
	
	obj->getDest();
	if (obj->m_mesh!=nullptr)
		info += "Conected to " + obj->m_mesh->getLabel() + "\n";
	else
		info += "Not conected to points, cannot be rendered\n";

	ui.info->blockSignals(true);
	ui.info->setText(info);
	ui.info->blockSignals(false);
}


void PropAnnotationSetOfFaces::pointSizeChanged(int s)
{
	//obj->m_pointSize = s;

	UI::updateAllViews();
}
