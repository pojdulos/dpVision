#ifndef PROPVIEWER_H
#define PROPVIEWER_H

//#include <QtWidgets/QWidget>
#include "propWidget.h"
#include "propTransform.h"

#include "ui_propViewer.h"

#include "Wektor3D.h"


//class MySpinBox : public QSpinBox
//{
//	Q_OBJECT
//
//public:
//	MySpinBox(QWidget* parent = 0) : QSpinBox(parent) {};
//	//~MySpinBox() {};
//
//signals:
//	//needNumpad(bool hasFocus);
//
//	//virtual void stepBy(int steps) override;
//
//protected:
//	
//	virtual void focusInEvent(QFocusEvent* e) {
//		QLineEdit::focusInEvent(e);
//		emit(needNumpad(true));
//	}
//	virtual void focusOutEvent(QFocusEvent* e) {
//		QLineEdit::focusInEvent(e);
//		emit(needNumpad(false));
//		this->>stepUp();
//		
//	}
//}

class GLViewer;

class PropViewer : public PropWidget
{
	Q_OBJECT

	CVector3<double> rot;
	CVector3<double> tra;
public:
	explicit PropViewer(GLViewer* m, QWidget *parent = 0);
	~PropViewer();

	static PropWidget* create(GLViewer* m, QWidget* parent = 0);

	static QVector<PropWidget*> create_and_get_subwidgets(GLViewer* m);

	virtual void paintEvent(QPaintEvent * event) override;
	virtual void updateProperties() override;


public slots:
	void rotXeditingFinished();
	void changedRotX( double );
	void changedRotY( double );
	void changedRotZ( double );

	void changedTraXYZ( double );

	void changedScale( double );
	void changedBGcolor( double );
	void changedAngle( int );
	void changedOrthoViewSize(double);

	void radioPropToggled(bool);

private:
	Ui::propViewer ui;
};

#endif // PROPVIEWER_H
