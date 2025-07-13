#ifndef PROPMOVEMENT_H
#define PROPMOVEMENT_H

#include "propBaseObject.h"
#include "ui_propMovement.h"

#include "Wektor3D.h"
#include "Quaternion.h"

#include <QItemEditorFactory>
#include <QSpinBox>

#include "Movement.h"
class CModel3D;

class QStandardItemModel;
class QStandardItem;


#define EXPERIMENTAL_MOVEMENT_UI

//class ItemEditorFactory : public QItemEditorFactory
//{
//	QWidget* createEditor(int userType, QWidget* parent) const
//	{
//		if (userType == 4)
//			return new QSpinBox(parent);
//		return QItemEditorFactory::createEditor(userType, parent);
//	}
//};

class DPVISION_EXPORT PropMovement : public PropWidget
{
	Q_OBJECT
	CMovement* m_animation;

public:
	static PropWidget* create(CMovement* m, QWidget* parent = 0);
	static QVector<PropWidget*> create_and_get_subwidgets(CBaseObject* obj);

	void addSeparator(const QString& title, QStandardItemModel* model);
	void updateDouble(QString itemName, double v);
	void updateVector3d(QString itemName, CVector3d q);
	void updateQuaternion(QString itemName, CQuaternion q);
	
	QStandardItem* addFrame(QStandardItem* parent, int frameKey, CMovement::FrameVal frameVal);

	QStandardItem* addQuaternion(QStandardItem* parent, CQuaternion q, const QString& title, double min, double max, double step, bool loop = false);
	QStandardItem* addVector3d(QStandardItem* parent, CVector3d v, const QString& title, double min, double max, double step, bool loop = false);
	QStandardItem* addDouble(QStandardItem* parent, double v, const QString& title, double min, double max, double step, bool loop = false);
	QStandardItem* addBool(const QString& title, QStandardItemModel* model, bool b);
	void setTreeView();
	explicit PropMovement(CMovement *m, QWidget *parent = 0, bool isCameraTransform = false);
	~PropMovement();

	//virtual void paintEvent(QPaintEvent * event) override;

	virtual void updateProperties() override;


	//void showModelProperties( bool b ) { ui.groupBoxModel->setVisible( b ); }

public slots:
	void clearMatrix();
	void copyToClipboard();
	void pasteFromClipboard();

	void onItemChanged(QStandardItem*);
	
	void onCurrentChanged(const QModelIndex& current, const QModelIndex& previous);

	void dataChanged(QStandardItem* item);

	void onSliderValueChanged(int val);

	void onPlayButtonClicked();

	void onTreeViewItemClicked(QModelIndex);
private:
	Ui::propMovement ui;

	void updateGroupFrame();
	void updateMatrix();
	void updatePropertiesTree();

};

#endif // PROPMOVEMENT_H
