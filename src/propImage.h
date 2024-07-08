#ifndef PROPIMAGE_H
#define PROPIMAGE_H


#include "propModel.h"

#include "Wektor3D.h"
#include "Quaternion.h"

#include "ui_propImage.h"

class CImage;

class PropImage : public PropWidget
{
	Q_OBJECT
		CImage* obj;

public:
	explicit PropImage(CImage *m, QWidget *parent = 0);
	~PropImage();

	static PropWidget* create(CImage* m, QWidget* parent = 0);
	static QVector<PropWidget*> create_and_get_subwidgets(CBaseObject* obj);

	//virtual void paintEvent(QPaintEvent * event) override;

	virtual void updateProperties() override;

	//void showModelProperties( bool b ) { ui.groupBoxModel->setVisible( b ); }

	Ui::propImage* getUI() { return &ui; }

public slots:
	void scaleChanged(int);
	void showWindow(bool);
	void show3d(bool);
	void slotChangeShow3d(bool);

signals:
	void signalChangedShow3d(bool);

private:
	Ui::propImage ui;

	bool wasLocked;

	//void updateQuaternion(CQuaternion &q);

};

#endif // PROPMODEL_H
