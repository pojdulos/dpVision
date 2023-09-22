#include "propWidget.h"

//virtual void paintEvent(QPaintEvent * event) override ;

#include <QLayout>
#include <QRegularExpression>

void PropWidget::updateProperties()
{
	QRegularExpression rx("prop");

	//const QList<PropWidget*>& pList = this->layout()->findChildren<PropWidget*>("",Qt::FindDirectChildrenOnly);
	const QWidgetList& pList = this->findChildren<QWidget*>(rx);// , Qt::FindDirectChildrenOnly);

	int size = pList.size();

	QListIterator<QWidget*> i(pList);
	while (i.hasNext())
		((PropWidget * )i.next())->updateProperties();
}

#include "QFormLayout"

PropWidget* PropWidget::build(QVector<PropWidget*> content, QWidget* parent)
{
	PropWidget* widget = new PropWidget(parent);
	QVBoxLayout* layout = new QVBoxLayout(widget);
	//QFormLayout* layout = new QFormLayout(widget);

	QVectorIterator<PropWidget*> i(content);
	while (i.hasNext())
		layout->addWidget( (PropWidget*)i.next() );

	widget->resize(layout->sizeHint());
	widget->setMinimumSize(layout->sizeHint());
	widget->setMaximumSize(layout->sizeHint());

	widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	return widget;
}
