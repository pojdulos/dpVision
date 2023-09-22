#include "propAnnotationSetOfVertices.h"
#include "AnnotationSetOfVertices.h"
#include "propTransform.h"

#include "UI.h"
#include <QVBoxLayout>

PropAnnotationSetOfVertices::PropAnnotationSetOfVertices(CAnnotationSetOfVertices *an, QWidget *parent) : PropWidget(parent)//: PropAnnotation( an, parent )
{
	obj = an;
	ui.setupUi(this);
}

PropAnnotationSetOfVertices::~PropAnnotationSetOfVertices()
{
}

PropWidget* PropAnnotationSetOfVertices::create(CAnnotationSetOfVertices* m, QWidget* parent)
{
	PropWidget* widget = new PropWidget(parent);
	QVBoxLayout* layout = new QVBoxLayout(widget);

	layout->addWidget(new PropBaseObject(m, widget));
	layout->addWidget(new PropAnnotation(m, widget));
	//layout->addWidget(new PropTransform(&m->getTransform(), widget));
	layout->addWidget( new PropAnnotationSetOfVertices(m, widget) );

	widget->resize(layout->sizeHint());
	widget->setMinimumSize(layout->sizeHint());
	widget->setMaximumSize(layout->sizeHint());

	widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	return widget;
}

void PropAnnotationSetOfVertices::updateProperties()
{
	PropWidget::updateProperties();
	
	ui.spinPointSize->blockSignals(true);
	ui.spinPointSize->setValue(obj->m_pointSize);
	ui.spinPointSize->blockSignals(false);

	QString info = "Number of vertices: " + QString::number(obj->size()) + "\n";
	
	obj->getDest();
	if (obj->m_cloud!=nullptr)
		info += "Conected to " + obj->m_cloud->getLabel() + "\n";
	else
		info += "Not conected to points, cannot be rendered\n";

	ui.info->blockSignals(true);
	ui.info->setText(info);
	ui.info->blockSignals(false);
}


void PropAnnotationSetOfVertices::pointSizeChanged(int s)
{
	obj->m_pointSize = s;

	UI::updateAllViews();
}
