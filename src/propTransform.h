#ifndef PROPTRANSFORM_H
#define PROPTRANSFORM_H

#include "propBaseObject.h"
#include "ui_propTransform.h"

#include "Wektor3D.h"
#include "Quaternion.h"

#include <QItemEditorFactory>
#include <QSpinBox>

class CTransform;
class CModel3D;

class QStandardItemModel;
class QStandardItem;


//#define EXPERIMENTAL_UI

class ItemEditorFactory : public QItemEditorFactory
{
	QWidget* createEditor(int userType, QWidget* parent) const
	{
		if (userType == 4)
			return new QSpinBox(parent);
		return QItemEditorFactory::createEditor(userType, parent);
	}
};

class DPVISION_EXPORT PropTransform : public PropWidget
{
	Q_OBJECT
	CTransform* m_trans;

	static bool group_visible;

public:
	void addSeparator(const QString& title, QStandardItemModel* model);
	void updateDouble(QString itemName, double v);
	void updateVector3d(QString itemName, CVector3d q);
	void updateQuaternion(QString itemName, CQuaternion q);
	QStandardItem* addQuaternion(QStandardItem* parent, CQuaternion q, const QString& title, double min, double max, double step, bool loop = false);
	QStandardItem* addVector3d(QStandardItem* parent, CVector3d v, const QString& title, double min, double max, double step, bool loop = false);
	QStandardItem* addDouble(QStandardItem* parent, double v, const QString& title, double min, double max, double step, bool loop = false);
	QStandardItem* addBool(const QString& title, QStandardItemModel* model, bool b);
	void setTreeView();
	explicit PropTransform(CTransform *m, QWidget *parent = 0, bool isCameraTransform = false);
	~PropTransform();

	//virtual void paintEvent(QPaintEvent * event) override;

	virtual void updateProperties() override;

	void updateTra();

	void updateSca(bool all=true);

	void updateEul();

	void updateQua();

	//void showModelProperties( bool b ) { ui.groupBoxModel->setVisible( b ); }

public slots:
	void changedEul(double);
	void changedQua(double);
	void changedTra(double);
	void changedSca(double);
	void onScaleCheck(int);

	void clearMatrix();
	void copyToClipboard();
	void pasteFromClipboard();

	void onItemChanged(QStandardItem*);

	void onShowScrewCheckBox(bool b);

	void dataChanged(QStandardItem* item);

	void onRotButton();
private:
	Ui::propTransform ui;

	void updateMatrix();
	void updatePropertiesTree();

};

#endif // PROPTRANSFORM_H
