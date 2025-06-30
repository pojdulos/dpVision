#include "propMovement.h"
#include "UI.h"
#include "AP.h"

#include "Model3D.h"

#include "MainWindow.h"

#include <QStandardItemModel>
#include <qclipboard.h>

#include "CSpinBoxDelegate.h"

PropMovement::PropMovement(CMovement* m, QWidget* parent, bool isCameraTransform) : PropWidget(parent)
{
	treeItemLabel = "Animation properties";

	ui.setupUi(this);

	m_animation = m; // &m->getTransform();


#ifdef EXPERIMENTAL_MOVEMENT_UI
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

PropMovement::~PropMovement()
{
}


PropWidget* PropMovement::create(CMovement* m, QWidget* parent)
{
	PropWidget* widget = new PropWidget(parent);
	QVBoxLayout* layout = new QVBoxLayout(widget);

	layout->addWidget(new PropBaseObject(m, widget));
	layout->addWidget(new PropMovement(m, widget));
	//layout->addWidget(new PropTransform(&m->getTransform(), widget));
	
	widget->resize(layout->sizeHint());
	widget->setMinimumSize(layout->sizeHint());
	widget->setMaximumSize(layout->sizeHint());

	widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	return widget;
}

QVector<PropWidget*> PropMovement::create_and_get_subwidgets(CBaseObject* obj)
{
	CMovement* m = (CMovement*)obj;
	return QVector<PropWidget*>({ new PropBaseObject(m), new PropMovement(m) });
}

void PropMovement::updateProperties()
{
	ui.currentFrameSlider->blockSignals(true);
	ui.currentFrameSlider->setValue(m_animation->currentKey());
	ui.currentFrameSlider->blockSignals(false);

	updateGroupFrame();
	updatePropertiesTree();
}

void PropMovement::updatePropertiesTree()
{
#ifdef EXPERIMENTAL_MOVEMENT_UI
	QAbstractItemModel* model = ui.treeView->model();
	QModelIndex index = model->index(m_animation->currentKey(), 0); // 0 oznacza pierwsz� kolumn�

	QItemSelectionModel* selectionModel = ui.treeView->selectionModel();
//	ui.treeView->blockSignals(true);
	selectionModel->clearSelection();
	selectionModel->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
//	ui.treeView->blockSignals(false);
#endif
}


void PropMovement::addSeparator(const QString& title, QStandardItemModel* model )
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

void PropMovement::updateQuaternion(QString itemName, CQuaternion q)
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

void PropMovement::setTreeView()
{
	ui.treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
	QStandardItemModel* model = new QStandardItemModel();
	model->setHorizontalHeaderLabels(QStringList({ "order", "label", "time", }));
	ui.treeView->setModel(model);
	ui.treeView->setUniformRowHeights(true);

	ui.treeView->setItemDelegate(new CSpinBoxDelegate());

	for (int key=0; key< m_animation->size(); key++)
	{
		addFrame(model->invisibleRootItem(), key, m_animation->frame(key));
	}

	ui.currentFrameSlider->setMinimum(0);
	ui.currentFrameSlider->setMaximum(m_animation->size()-1);

	connect(ui.treeView->selectionModel(), &QItemSelectionModel::currentChanged, this, &PropMovement::onCurrentChanged);

	QObject::connect(model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(dataChanged(QStandardItem*)));
}

QStandardItem* PropMovement::addFrame(QStandardItem* parent, int frameKey, CMovement::FrameVal frameVal)
{
	if (parent)
	{
		//DGM: we can't use the 'text' of the item as it will be displayed under the associated editor (label)!
		//So we simply use the 'accessible description' field
		QStandardItem* leftItem = new QStandardItem(QString::number(frameKey));
		leftItem->setAccessibleDescription(QString::number(frameKey));
		leftItem->setData(QVariant::fromValue<int>(frameKey), Qt::ItemDataRole::UserRole);
		leftItem->setEditable(false);

		QStandardItem* leftItemLabel = new QStandardItem();
		leftItemLabel->setData(QVariant(frameVal.getLabel()), Qt::ItemDataRole::DisplayRole);
		leftItemLabel->setData(QVariant::fromValue<QString>(frameVal.getLabel()), Qt::ItemDataRole::EditRole);
		leftItemLabel->setData(QVariant::fromValue<int>(frameKey), Qt::ItemDataRole::UserRole);


		QString vs = QString::number(frameVal.msec);
		//QString vs = frameVal.t.toString("[","]",",");

		QStandardItem* leftItemValue = new QStandardItem();
		leftItemValue->setData(QVariant(vs), Qt::ItemDataRole::DisplayRole);
		leftItemValue->setData(QVariant::fromValue<int>(frameVal.msec), Qt::ItemDataRole::EditRole);
		leftItemValue->setData(QVariant::fromValue<int>(frameKey), Qt::ItemDataRole::UserRole);

		parent->appendRow((QList< QStandardItem* >({ leftItem, leftItemLabel, leftItemValue })));

		return leftItem;
	}
	return nullptr;
}


QStandardItem* PropMovement::addQuaternion(QStandardItem* parent, CQuaternion q, const QString& title, double min, double max, double step, bool loop)
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

void PropMovement::updateVector3d(QString itemName, CVector3d v)
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

QStandardItem* PropMovement::addVector3d(QStandardItem* parent, CVector3d v, const QString& title, double min, double max, double step, bool loop)
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


void PropMovement::updateDouble(QString itemName, double v)
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

QStandardItem* PropMovement::addDouble(QStandardItem* parent, double v, const QString& title, double min, double max, double step, bool loop)
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

QStandardItem* PropMovement::addBool(const QString& title, QStandardItemModel* model, bool b)
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



void PropMovement::dataChanged(QStandardItem* item)
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

void PropMovement::onSliderValueChanged(int val)
{
	m_animation->setKey(val);
	updateProperties();
	UI::updateAllViews();
}

void PropMovement::onPlayButtonClicked()
{
	if (m_animation->isPlaying())
	{
		m_animation->stopPlaying();
	}
	else
	{
		m_animation->startPlaying();
	}
	
} 





void PropMovement::updateGroupFrame()
{
	updateMatrix();
}

void PropMovement::updateMatrix()
{
	if (NULL != m_animation)
	{
		//qInfo() << ">> updateMatrix(), m_currentKey=" << m_currentKey << Qt::endl;

		double matrix[16];
		m_animation->currentFrame().t.toRowMatrixD(matrix);
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



void PropMovement::clearMatrix()
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
	m_animation->currentFrame().t.fromGLMatrixD(matrix);
	updatePropertiesTree();
	UI::updateAllViews();
}

void PropMovement::copyToClipboard()
{
	//double matrix[16];
	QClipboard* cb = QApplication::clipboard();

	//QString text;

	//m_animation->currentFrame().t.toRowMatrixD(matrix);

	//for (int col = 0; col < 4; col++)
	//	for (int row = 0; row < 4; row++)
	//	{
	//		double val = matrix[4 * row + col];
	//		text.append(QString::number(val) + " ");
	//	}

	QString text = m_animation->currentFrame().t.toString();

	cb->setText(text, QClipboard::Clipboard);
}

void PropMovement::pasteFromClipboard()
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

			m_animation->currentFrame().t.fromRowMatrixD(tmpMatrix);

			updatePropertiesTree();
			UI::updateAllViews();
		}
	}
}

void PropMovement::onItemChanged(QStandardItem*)
{
	UI::MESSAGEBOX::information(L"Item changed");
}

void PropMovement::onCurrentChanged(const QModelIndex& current, const QModelIndex& previous)
{
	if (current.isValid())
	{
		QStandardItemModel* model = (QStandardItemModel*)ui.treeView->model();
		QStandardItem* clickedItem = (QStandardItem*)model->itemFromIndex(current);

		int key = clickedItem->data(Qt::UserRole).value<int>();

		ui.currentFrameSlider->blockSignals(true);
		ui.currentFrameSlider->setValue(key);
		ui.currentFrameSlider->blockSignals(false);

		//qInfo() << "Sequence item clicked. Key=" << key << Qt::endl;
		//emit(currentObjectChanged(clickedObject->id()));

		m_animation->setKey(key);

		updateGroupFrame();
		UI::updateAllViews();
	}
	else
	{
		qInfo() << "item index is invalid" << Qt::endl;
		//emit(currentObjectChanged(NO_CURRENT_MODEL));
	}
}


void PropMovement::onTreeViewItemClicked(QModelIndex current)
{
	//if (current.isValid())
	//{
	//	QStandardItemModel* model = (QStandardItemModel*)ui.treeView->model();
	//	QStandardItem* clickedItem = (QStandardItem*)model->itemFromIndex(current);

	//	int key = clickedItem->data(Qt::UserRole).value<int>();

	//	qInfo() << "Sequence item clicked. Key=" << key << Qt::endl;
	//	//emit(currentObjectChanged(clickedObject->id()));

	//	m_trans->m_currentKey = key;

	//	updateGroupFrame();
	//	UI::updateAllViews();
	//}
	//else
	//{
	//	qInfo() << "item index is invalid" << Qt::endl;
	//	//emit(currentObjectChanged(NO_CURRENT_MODEL));
	//}
}
