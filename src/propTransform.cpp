#include "propTransform.h"
#include "UI.h"
#include "AP.h"

#include "Model3D.h"

#include "MainWindow.h"

#include <QStandardItemModel>
#include <qclipboard.h>

#include "CSpinBoxDelegate.h"

bool PropTransform::group_visible = true;

QString PropTransform::treeItemLabel()
{
	return QString("Transformation");
}


void PropTransform::addSeparator(const QString& title, QStandardItemModel* model )
{
	if (model)
	{
		//DGM: we can't use the 'text' of the item as it will be displayed under the associated editor (label)!
		//So we simply use the 'accessible description' field
		QStandardItem* leftItem = new QStandardItem(title);
		leftItem->setData(0);
		leftItem->setAccessibleDescription(title);
		model->appendRow(leftItem);

		//if (ui.treeView != nullptr)
		//{
		//	ui.treeView->openPersistentEditor(model->index(model->rowCount() - 1, 0));
		//}
	}
}

#include "qquaternion.h"

void PropTransform::updateQuaternion(QString itemName, CQuaternion q)
{
	QStandardItemModel* model = (QStandardItemModel * )ui.treeView->model();


	QList<QStandardItem*> list1 = model->findItems(itemName);
	
	if (!list1.isEmpty())
	{
		QStandardItem* leftItem = list1[0];
		QStandardItem* rightItem = model->item(model->indexFromItem(leftItem).row(), 1);

		QString vs = "[" + QString::number(q.x()) + ", " + QString::number(q.y()) + ", " + QString::number(q.z()) + ", " + QString::number(q.w()) + "]";

		rightItem->setData(QVariant::fromValue<QQuaternion>(q.toQt()), Qt::ItemDataRole::EditRole);
		rightItem->setData(QVariant(vs), Qt::ItemDataRole::DisplayRole);

		//=========================

		if (leftItem->hasChildren())
		{
			leftItem->child(0, 1)->setData(QVariant(q.x()), Qt::ItemDataRole::EditRole);
			leftItem->child(1, 1)->setData(QVariant(q.y()), Qt::ItemDataRole::EditRole);
			leftItem->child(2, 1)->setData(QVariant(q.z()), Qt::ItemDataRole::EditRole);
			leftItem->child(3, 1)->setData(QVariant(q.w()), Qt::ItemDataRole::EditRole);
		}
	}
}

QStandardItem* PropTransform::addQuaternion(QStandardItem* parent, CQuaternion q, const QString& title, double min, double max, double step, bool loop)
{
	if (parent)
	{
		//DGM: we can't use the 'text' of the item as it will be displayed under the associated editor (label)!
		//So we simply use the 'accessible description' field
		QStandardItem* leftItem = new QStandardItem(title);
		leftItem->setData(0);
		leftItem->setAccessibleDescription(title);
		leftItem->setEditable(false);

		QString vs = "[" + QString::number(q.x()) + ", " + QString::number(q.y()) + ", " + QString::number(q.z()) + ", " + QString::number(q.w()) + "]";
		
		QStandardItem* leftItemValue = new QStandardItem();
		leftItemValue->setData(QVariant::fromValue<QQuaternion>(q.toQt()), Qt::ItemDataRole::EditRole);
		leftItemValue->setData(QVariant(vs), Qt::ItemDataRole::DisplayRole);
		leftItemValue->setData(ItemId::RotXYZW, Qt::ItemDataRole::UserRole);

		parent->appendRow((QList< QStandardItem* >({ leftItem, leftItemValue })));

		addDouble(leftItem, q.x(), "x", min, max, step, loop);
		addDouble(leftItem, q.y(), "y", min, max, step, loop);
		addDouble(leftItem, q.z(), "z", min, max, step, loop);
		addDouble(leftItem, q.w(), "w", min, max, step, loop);

		return leftItem;
	}
	return nullptr;
}

#include "qvector3d.h"

void PropTransform::updateVector3d(QString itemName, CVector3d v)
{
	QStandardItemModel* model = (QStandardItemModel*)ui.treeView->model();

	QList<QStandardItem*> list1 = model->findItems(itemName);

	if (!list1.isEmpty())
	{
		QStandardItem* leftItem = list1[0];
		QStandardItem* rightItem = model->item(model->indexFromItem(leftItem).row(), 1);

		QString vs = "[" + QString::number(v.X()) + ", " + QString::number(v.Y()) + ", " + QString::number(v.Z()) + "]";
		QVector3D vec(v.X(), v.Y(), v.Z());
		
		rightItem->setData(QVariant::fromValue<QVector3D>(vec), Qt::ItemDataRole::EditRole);
		rightItem->setData(QVariant(vs), Qt::ItemDataRole::DisplayRole);

		//=========================

		if (leftItem->hasChildren())
		{
			leftItem->child(0, 1)->setData(QVariant(v.x), Qt::ItemDataRole::EditRole);
			leftItem->child(1, 1)->setData(QVariant(v.y), Qt::ItemDataRole::EditRole);
			leftItem->child(2, 1)->setData(QVariant(v.z), Qt::ItemDataRole::EditRole);
		}
	}
}

QStandardItem* PropTransform::addVector3d(QStandardItem* parent, CVector3d v, const QString& title, double min, double max, double step, bool loop)
{
	if (parent)
	{
		//DGM: we can't use the 'text' of the item as it will be displayed under the associated editor (label)!
		//So we simply use the 'accessible description' field
		QStandardItem* leftItem = new QStandardItem(title);
		leftItem->setData(0);
		leftItem->setAccessibleDescription(title);
		leftItem->setEditable(false);

		QString vs = "[" + QString::number(v.X()) + ", " + QString::number(v.Y()) + ", " + QString::number(v.Z()) + "]";
		
		QVector3D vec(v.X(), v.Y(), v.Z());
		QStandardItem* leftItemValue = new QStandardItem();
		leftItemValue->setData(QVariant::fromValue<QVector3D>(vec), Qt::ItemDataRole::EditRole);
		leftItemValue->setData(QVariant(vs), Qt::ItemDataRole::DisplayRole);
		leftItemValue->setData(ItemId::TraXYZ, Qt::ItemDataRole::UserRole);

		parent->appendRow((QList< QStandardItem* >({ leftItem, leftItemValue })));

		addDouble(leftItem, v.x, "x", min, max, step, loop);
		addDouble(leftItem, v.y, "y", min, max, step, loop);
		addDouble(leftItem, v.z, "z", min, max, step, loop);

		return leftItem;
	}
	return nullptr;
}


void PropTransform::updateDouble(QString itemName, double v)
{
	QStandardItemModel* model = (QStandardItemModel*)ui.treeView->model();

	QList<QStandardItem*> list1 = model->findItems(itemName);

	if (!list1.isEmpty())
	{
		QStandardItem* leftItem = list1[0];
		QStandardItem* rightItem = model->item(model->indexFromItem(leftItem).row(), 1);

		rightItem->setData(QVariant(v), Qt::ItemDataRole::EditRole);
	}
}

QStandardItem* PropTransform::addDouble(QStandardItem* parent, double v, const QString& title, double min, double max, double step, bool loop)
{
	if (parent)
	{
		//DGM: we can't use the 'text' of the item as it will be displayed under the associated editor (label)!
		//So we simply use the 'accessible description' field
		QStandardItem* leftItem = new QStandardItem(title);
		leftItem->setData(0);
		leftItem->setAccessibleDescription(title);
		leftItem->setEditable(false);

		QStandardItem* leftItemValue = new QStandardItem();
		
		leftItemValue->setData(ItemId::Sca, Qt::ItemDataRole::UserRole);

		leftItemValue->setData(QVariant(min), QtRoleDataMin);
		leftItemValue->setData(QVariant(max), QtRoleDataMax);
		leftItemValue->setData(QVariant(step), QtRoleDataStep);
		leftItemValue->setData(QVariant(loop), QtRoleDataLoop);

		leftItemValue->setData(QVariant::fromValue(static_cast<void*>(&v)), QtRoleDataPointer);

		leftItemValue->setData(QVariant(v), Qt::ItemDataRole::EditRole);

		parent->appendRow((QList< QStandardItem* >({ leftItem, leftItemValue })));

		return leftItem;
	}
	return nullptr;
}

QStandardItem* PropTransform::addBool(const QString& title, QStandardItemModel* model, bool b)
{
	if (model)
	{
		//DGM: we can't use the 'text' of the item as it will be displayed under the associated editor (label)!
		//So we simply use the 'accessible description' field
		QStandardItem* leftItem = new QStandardItem(title);
		leftItem->setData(0);
		leftItem->setAccessibleDescription(title);
		leftItem->setEditable(false);

		QStandardItem* leftItemValue = new QStandardItem();
		leftItemValue->setData(QVariant(b), Qt::ItemDataRole::EditRole);
		leftItemValue->setData(ItemId::Visible, Qt::ItemDataRole::UserRole);

//		QStandardItem* leftItemValue = new QStandardItem(b?"true":"false");/
//		leftItemValue->setData( ItemType::Bool + ItemId::Visible, Qt::ItemDataRole::UserRole);
//		leftItemValue->setCheckable(true);
//		leftItemValue->setCheckState(b ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);

		model->appendRow((QList< QStandardItem* >({ leftItem, leftItemValue })));

		return leftItem;
	}
	return nullptr;
}

void PropTransform::setTreeView()
{
	ui.treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
	QStandardItemModel* model = new QStandardItemModel();
	model->setHorizontalHeaderLabels(QStringList({ "property", "value" }));
	ui.treeView->setModel(model);
	ui.treeView->setUniformRowHeights(true);

	ui.treeView->setItemDelegate(new CSpinBoxDelegate());

	//addBool("visible", model, m_trans->true);
	addVector3d(model->invisibleRootItem(), m_trans->translation(), "translation", -1000, 1000, 0.1);
	addQuaternion(model->invisibleRootItem(), m_trans->rotation(), "rotation", -1.0, 1.0, 0.01 );
	addDouble(model->invisibleRootItem(), m_trans->scale().x, "scale", 0, 1000, 0.1 );

	QObject::connect(model, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(dataChanged(QStandardItem *)));
}


void PropTransform::dataChanged(QStandardItem* item)
{
	ItemId key = (ItemId) item->data(Qt::ItemDataRole::UserRole).toInt();

	QVariant data_value = item->data(Qt::ItemDataRole::EditRole);

	switch (key)
	{
	case ItemId::Visible:
		item->setText((item->checkState() == Qt::CheckState::Checked) ? "true" : "false");
		break;
	case ItemId::Sca:
	case ItemId::TraX:
	case ItemId::TraY:
	case ItemId::TraZ:
	case ItemId::RotX:
	case ItemId::RotY:
	case ItemId::RotZ:
	case ItemId::RotW:
		//m_trans->setScale(data_value.toDouble());

		*((double*)item->data(QtRoleDataPointer).value<void*>()) = data_value.toDouble();
		break;
	//case ItemId::TraX:
	//	m_trans->translation().X(data_value.toDouble());
	//	break;
	//case ItemId::TraY:
	//	m_trans->translation().Y(data_value.toDouble());
	//	break;
	//case ItemId::TraZ:
	//	m_trans->translation().Z(data_value.toDouble());
	//	break;
	default:
		break;
	}

	UI::updateAllViews();
	//UI::DOCK::PROPERTIES::updateProperties();
}

void PropTransform::onRotButton()
{
	double angle = ui.angleSpinBox->value();
	QString ax = ui.axisCombo->currentText();
	bool internal = ui.internalAxisRadio->isChecked();
	CVector3d axis;
	if (ax.startsWith("x"))
	{
		axis = CVector3d::XAxis();
	}
	else if (ax.startsWith("y"))
	{
		axis = CVector3d::YAxis();
	}
	else
	{
		axis = CVector3d::ZAxis();
	}

	m_trans->rotateAroundAxisDeg(axis, angle, internal);
	UI::updateAllViews();
	UI::DOCK::PROPERTIES::updateProperties();
}


PropTransform::PropTransform(CTransform *m, QWidget *parent, bool isCameraTransform) : PropWidget( parent )
{
	m_trans = m; // &m->getTransform();

	ui.setupUi(this);

	if (isCameraTransform)
	{
		ui.externalAxisRadio->setDisabled(true);
		ui.scaleX->setDisabled(true);
		ui.scaleCheck->setDisabled(true);
	}

#ifdef EXPERIMENTAL_UI
	setTreeView();
#else
	ui.treeView->setVisible(false);
	this->resize(this->layout()->sizeHint());
#endif

	//QObject::connect( ui.spinRotX, SIGNAL(valueChanged(double)), this, SLOT(updateModelRotX(double)) );
	//QObject::connect( ui.spinRotY, SIGNAL(valueChanged(double)), this, SLOT(updateModelRotY(double)) );
	//QObject::connect( ui.spinRotZ, SIGNAL(valueChanged(double)), this, SLOT(updateModelRotZ(double)) );

	//QObject::connect( ui.spinTransX, SIGNAL(valueChanged(double)), this, SLOT(updateModelTrans(double)) );
	//QObject::connect( ui.spinTransY, SIGNAL(valueChanged(double)), this, SLOT(updateModelTrans(double)) );
	//QObject::connect( ui.spinTransZ, SIGNAL(valueChanged(double)), this, SLOT(updateModelTrans(double)) );

	//QObject::connect( ui.spinScale, SIGNAL(valueChanged(double)), this, SLOT(updateModelScale(double)) );
	//QObject::connect( ui.checkCentered, SIGNAL(stateChanged(int)), this, SLOT(updateModelCentered(int)) );

	//ChildWindow *child = (ChildWindow*)AP::mainWin().ui.mdiArea->activeSubWindow();
	//if (NULL != child)
	//{
	//	GLViewer *v = child->getViewer();
	//	QObject::connect( v, SLOT(translationChanged(double, double, double)), this, SLOT(updateTranslations(double, double, double)), Qt::DirectConnection );
	//}

}

PropTransform::~PropTransform()
{
}


//void PropTransform::paintEvent(QPaintEvent * event)
//{
//	QWidget::paintEvent(event);
//}


void PropTransform::updateProperties()
{
	updateEul();
	updateQua();
	updateTra();
	updateSca();

	updateMatrix();
	//assert("TEST");

	ui.showScrewCheckBox->blockSignals(true);
	ui.showScrewCheckBox->setChecked(m_trans->isScrewVisible());
	ui.showScrewCheckBox->blockSignals(false);

	updatePropertiesTree();

	
}

void PropTransform::updateTra()
{
	CVector3d t = m_trans->translation();

	t.x = QString::number(t.x, 'f', 8).toDouble();
	t.y = QString::number(t.y, 'f', 8).toDouble();
	t.z = QString::number(t.z, 'f', 8).toDouble();

	ui.transX->blockSignals(true);
	ui.transY->blockSignals(true);
	ui.transZ->blockSignals(true);
	ui.transX->setValue(t.x);
	ui.transY->setValue(t.y);
	ui.transZ->setValue(t.z);
	ui.transX->blockSignals(false);
	ui.transY->blockSignals(false);
	ui.transZ->blockSignals(false);
}

void PropTransform::updateSca(bool all)
{
	CPoint3d s = m_trans->scale();

	if (all)
	{
		ui.scaleX->blockSignals(true);
		ui.scaleX->setValue(s.x);
		ui.scaleX->blockSignals(false);
	}

	ui.scaleY->blockSignals(true);
	ui.scaleY->setValue(s.y);
	ui.scaleY->blockSignals(false);

	ui.scaleZ->blockSignals(true);
	ui.scaleZ->setValue(s.z);
	ui.scaleZ->blockSignals(false);
}

void PropTransform::updateEul()
{
	CVector3d e = m_trans->rotation().eulerAngles();

	e.x = QString::number(rad2deg(e.x), 'f', 8).toDouble();
	e.y = QString::number(rad2deg(e.y), 'f', 8).toDouble();
	e.z = QString::number(rad2deg(e.z), 'f', 8).toDouble();

	ui.eulerX->blockSignals(true);
	ui.eulerY->blockSignals(true);
	ui.eulerZ->blockSignals(true);
	ui.eulerX->setValue(e.x);
	ui.eulerY->setValue(e.y);
	ui.eulerZ->setValue(e.z);
	ui.eulerX->blockSignals(false);
	ui.eulerY->blockSignals(false);
	ui.eulerZ->blockSignals(false);
}

void PropTransform::updateQua()
{
	CQuaternion q = m_trans->rotation();

	ui.quatX->setValue(q.x());
	ui.quatY->setValue(q.y());
	ui.quatZ->setValue(q.z());
	ui.quatW->setValue(q.w());
}

void PropTransform::changedEul(double val)
{
	//CVector3d oldRot = m_trans->rotation().eulerAnglesDeg();
	//CVector3d newRot(ui.eulerX->value(), ui.eulerY->value(), ui.eulerZ->value());
	//CVector3d r = newRot - oldRot;
	//m_trans->rotate({ {CVector3d::XAxis(),r.x},{CVector3d::YAxis(),r.y},{CVector3d::ZAxis(),r.z} });

	m_trans->rotation().fromEulerAnglesDeg(ui.eulerX->value(), ui.eulerY->value(), ui.eulerZ->value());

	updateQua();
	updateMatrix();
	
	UI::updateAllViews();
}

void PropTransform::changedQua(double val)
{
//	m_trans->rotation().fromEulerAnglesDeg(ui.eulerX->value(), ui.eulerY->value(), ui.eulerZ->value());

	UI::updateAllViews();
}


void PropTransform::changedTra(double val)
{
	m_trans->translation() = CVector3d(ui.transX->value(), ui.transY->value(), ui.transZ->value());

	updateMatrix();

	UI::updateAllViews();
}

void PropTransform::changedSca(double val)
{
	double sx = ui.scaleX->value();
	double sy = ui.scaleY->value();
	double sz = ui.scaleZ->value();

	if (ui.scaleCheck->isChecked())
	{
		m_trans->scale() = CPoint3d(sx, sy, sz);
	}
	else
	{
		m_trans->setScale(sx);

		updateSca(false);
	}

	updateMatrix();

	UI::updateAllViews();
}

void PropTransform::onScaleCheck(int i)
{
	if (ui.scaleCheck->isChecked())
	{
		ui.scaleY->setEnabled(true);
		ui.scaleZ->setEnabled(true);
	}
	else
	{
		ui.scaleY->setEnabled(false);
		ui.scaleZ->setEnabled(false);

		m_trans->setScale(ui.scaleX->value());

		updateSca(false);
	}

	updateMatrix();

	UI::updateAllViews();
}





void PropTransform::updateMatrix()
{
	if (NULL != m_trans)
	{
		double matrix[16];
		m_trans->toGLMatrixD(matrix);
		ui.matrixTable->blockSignals(true);
		for (int i = 0; i < 16; i++)
		{
			double val = matrix[i];
			//ui.matrixTable->setItem(i % 4, i / 4, new QTableWidgetItem(std::to_string(val).c_str()));
			QModelIndex index = ui.matrixTable->model()->index(i % 4, i / 4);
			ui.matrixTable->model()->setData(index, val);
		}
		ui.matrixTable->blockSignals(false);
	}
}

void PropTransform::updatePropertiesTree()
{
#ifdef EXPERIMENTAL_UI
	ui.treeView->blockSignals(true);
	updateQuaternion("rotation", m_trans->rotation() );
	updateVector3d("translation", m_trans->translation());
	updateDouble("scale", m_trans->scale().x);
	ui.treeView->blockSignals(false);
#endif
}


void PropTransform::clearMatrix()
{
	double matrix[16];
	for (int col = 0; col < 4; col++)
		for (int row = 0; row < 4; row++)
		{
			if (row == col)
				matrix[4 * row + col] = 1.0;
			else
				matrix[4 * row + col] = 0.0;
		}
	m_trans->fromGLMatrixD(matrix);
	updatePropertiesTree();
	UI::updateAllViews();
}

void PropTransform::copyToClipboard()
{
	double matrix[16];
	QClipboard* cb = QApplication::clipboard();

	QString text;

	m_trans->toGLMatrixD(matrix);

	for (int col = 0; col < 4; col++)
		for (int row = 0; row < 4; row++)
		{
			double val = matrix[4 * row + col];
			text.append(QString::number(val) + " ");
		}

	cb->setText(text, QClipboard::Clipboard);
}

void PropTransform::pasteFromClipboard()
{
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

			m_trans->fromRowMatrixD(tmpMatrix);

			updatePropertiesTree();
			UI::updateAllViews();
		}
	}
}

void PropTransform::onItemChanged(QStandardItem*)
{
	UI::MESSAGEBOX::information(L"Item changed");
}

void PropTransform::onShowScrewCheckBox(bool b)
{
	//UI::MESSAGEBOX::information(L"onShowScrewCheckBox(bool)");
	m_trans->showScrew(b);
	UI::updateAllViews();
}