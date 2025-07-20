#include "propAnnotationCamera.h"
#include "AnnotationCamera.h"

#include <qclipboard.h>

#include "../api/UI.h"
#include <QVBoxLayout>

PropAnnotationCamera::PropAnnotationCamera(CAnnotationCamera*an, QWidget *parent) : PropWidget( parent )
{
	obj = an;
	ui.setupUi(this);

	updateSensorProperties();
	//UI::IMAGEVIEWER::setImage( "E:/images/" + an->getLabelA(), false );
}

PropAnnotationCamera::~PropAnnotationCamera()
{
}

PropWidget* PropAnnotationCamera::create(CAnnotationCamera* m, QWidget* parent)
{
	PropWidget* widget = new PropWidget(parent);
	QVBoxLayout* layout = new QVBoxLayout(widget);

	layout->addWidget(new PropBaseObject(m, widget));
	layout->addWidget(new PropAnnotation(m, widget));
	//layout->addWidget(new PropTransform(&m->getTransform(), widget));
	layout->addWidget(new PropAnnotationCamera(m, widget));

	widget->resize(layout->sizeHint());
	widget->setMinimumSize(layout->sizeHint());
	widget->setMaximumSize(layout->sizeHint());

	widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	return widget;
}

QVector<PropWidget*> PropAnnotationCamera::create_and_get_subwidgets(CBaseObject* obj)
{
	CAnnotationCamera* m = (CAnnotationCamera*)obj;
	return QVector<PropWidget*>({ new PropBaseObject(m), new PropAnnotation(m), new PropAnnotationCamera(m) });
}

void PropAnnotationCamera::clearMatrix()
{
	CAnnotationCamera* a = (CAnnotationCamera*)obj;
	for (int col = 0; col < 4; col++)
		for (int row = 0; row < 4; row++)
		{
			if (row == col)
				a->matrix()[4 * row + col] = 1.0;
			else
				a->matrix()[4 * row + col] = 0.0;
		}
	updateSensorProperties();
	UI::updateAllViews();
}

void PropAnnotationCamera::copyToClipboard()
{
	CAnnotationCamera* a = (CAnnotationCamera*)obj;
	QClipboard* cb = QApplication::clipboard();

	QString text;

	for (int col = 0; col < 4; col++)
		for (int row = 0; row < 4; row++)
		{
			double val = a->matrix()[4 * row + col];
			text.append(QString::number(val) + " ");
		}

	cb->setText(text, QClipboard::Clipboard);
}

void PropAnnotationCamera::pasteFromClipboard()
{
	CAnnotationCamera* a = (CAnnotationCamera*)obj;
	QClipboard* cb = QApplication::clipboard();

	QString text = cb->text(QClipboard::Clipboard);

	if (!text.isNull())
	{
		QRegExp sep("(\\s+)");
		QStringList pieces = text.split(sep);

		if (pieces.length() >= 16)
		{
			double tmpMatrix[16];

			bool ok = false;
			for (int i = 0; i < 16; i++)
			{
				tmpMatrix[i] = pieces[i].toDouble(&ok);

				if (!ok) return;
			}

			a->setMatrix(tmpMatrix);

			updateSensorProperties();
			UI::updateAllViews();
		}
	}
}

void PropAnnotationCamera::updateSensorProperties()
{
	CAnnotationCamera* a = (CAnnotationCamera*)obj;

	if (NULL != a)
	{
		ui.matrixTable->blockSignals(true);
		for (int i = 0; i < 16; i++)
		{
			double val = a->matrix()[i];
			ui.matrixTable->setItem(i % 4, i / 4, new QTableWidgetItem(std::to_string(val).c_str()));
		}
		ui.matrixTable->blockSignals(false);

		CAnnotationCamera::Sensor& sensor = a->sensor();

		ui.sensorProps->blockSignals(true);
		ui.sensorProps->setItem(0, 0, new QTableWidgetItem(QString::number(sensor.xResolution)));
		ui.sensorProps->setItem(0, 1, new QTableWidgetItem(QString::number(sensor.realWidth)));
		ui.sensorProps->setItem(1, 0, new QTableWidgetItem(QString::number(sensor.yResolution)));
		ui.sensorProps->setItem(1, 1, new QTableWidgetItem(QString::number(sensor.realHeight)));
		ui.sensorProps->setItem(2, 0, new QTableWidgetItem(QString::number(sensor.pxFocalLenght)));
		ui.sensorProps->setItem(2, 1, new QTableWidgetItem(QString::number(sensor.mmFocalLength)));
		ui.sensorProps->blockSignals(false);
	}
}

void PropAnnotationCamera::updateProperties()
{
	//PropAnnotation::updateProperties();
}

