#include "propWidget.h"

//virtual void paintEvent(QPaintEvent * event) override ;

#include <QLayout>
#include <QRegularExpression>
#include "QFormLayout"

PropWidget::PropWidget(QWidget* parent) : QWidget(parent)
{
	treeItemLabel = this->objectName() + " properties";
}

void PropWidget::updateProperties()
{
	printf("PROPWIDGET !!!!\n");
	QRegularExpression rx("prop");

	//const QList<PropWidget*>& pList = this->layout()->findChildren<PropWidget*>("",Qt::FindDirectChildrenOnly);
	const QWidgetList& pList = this->findChildren<QWidget*>(rx);// , Qt::FindDirectChildrenOnly);

	int size = pList.size();

	QListIterator<QWidget*> i(pList);
	while (i.hasNext())
		((PropWidget * )i.next())->updateProperties();
}


PropWidget* PropWidget::build(QVector<PropWidget*> content, QWidget* parent)
{
	PropWidget* widget = new PropWidget(parent);
	
	widget->setObjectName("MainPropWidget");


	QFormLayout* layout = qobject_cast<QFormLayout*>(widget->layout());

	if (layout == nullptr)
	{
		layout = new QFormLayout(widget);
		widget->setLayout(layout);
	}

	//QFormLayout* layout = new QFormLayout(widget);
	//QVBoxLayout* layout = new QVBoxLayout(widget);

	QVectorIterator<PropWidget*> i(content);
	while (i.hasNext())
		layout->addRow( (PropWidget*)i.next() );

	//layout->setAlignment(Qt::AlignmentFlag::AlignTop);

	widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	
	widget->resize(layout->sizeHint());
	widget->setMinimumSize(layout->sizeHint());
	widget->setMaximumSize(layout->sizeHint());


	//widget->setLayout(layout);

	return widget;
}

QVector<PropWidget*> PropWidget::create_and_get_subwidgets(CBaseObject* obj)
{
	return QVector<PropWidget*>();
}
