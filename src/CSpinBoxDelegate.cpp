#include "CSpinBoxDelegate.h"
#include "QtWidgets/QSpinBox"
#include "QtWidgets/QLineEdit"

#include "UI.h"

QWidget* CSpinBoxDelegate::createSpinBox(QWidget* parent, int min, int max, int step) const
{
    QDoubleSpinBox* editor = new QDoubleSpinBox(parent);
    editor->setFrame(false);
    editor->setMinimum(0);
    editor->setMaximum(100);

    return editor;
}

QWidget* CSpinBoxDelegate::createDoubleSpinBox(QWidget* parent, double min, double max, double step) const
{
    QDoubleSpinBox* editor = new QDoubleSpinBox(parent);
    editor->setFrame(false);
    editor->setMinimum(min);
    editor->setMaximum(max);
    editor->setSingleStep(step);

    //editor->setStepType(QAbstractSpinBox::StepType::AdaptiveDecimalStepType);

    return editor;
}

QWidget* CSpinBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option , const QModelIndex& index ) const
{
    QWidget* editor;

    QVariant data_value = index.model()->data(index, Qt::ItemDataRole::EditRole);
    QVariant data_role = index.model()->data(index, Qt::ItemDataRole::UserRole);

    QVariant data_min = index.model()->data(index, QtRoleDataMin );
    QVariant data_max = index.model()->data(index, QtRoleDataMax);
    QVariant data_step = index.model()->data(index, QtRoleDataStep);
    QVariant data_loop = index.model()->data(index, QtRoleDataLoop);

    switch ( data_value.type() )
    {
    case QVariant::Double:
        editor = createDoubleSpinBox(parent, data_min.toDouble(), data_max.toDouble(), data_step.toDouble() );
        QObject::connect(editor, SIGNAL(valueChanged(double)), this, SLOT(valueChanged(double)));
        break;
    default:
        editor = new QLineEdit(parent);
        break;
    }

    return editor;
}

#include "qvector3d.h"
#include "qquaternion.h"

void CSpinBoxDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QVariant::Type data_type = index.model()->data(index, Qt::EditRole).type();

    switch (data_type)
    {
    case QVariant::Double:
        {
            double value = index.model()->data(index, Qt::EditRole).toDouble();

            QDoubleSpinBox* spinBox = static_cast<QDoubleSpinBox*>(editor);
            spinBox->setValue(value);
        }
        break;
    case QVariant::Vector3D:
        {
            QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
            QVector3D vec = index.model()->data(index, Qt::EditRole).value<QVector3D>();
            QString vs = "[" + QString::number(vec.x()) + ", " + QString::number(vec.y()) + ", " + QString::number(vec.z()) + "]";
            lineEdit->setText(vs);
        }
        break;
    case QVariant::Quaternion:
        {
            QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
            QQuaternion q = index.model()->data(index, Qt::EditRole).value<QQuaternion>();
            QString vs = "[" + QString::number(q.x()) + ", " + QString::number(q.y()) + ", " + QString::number(q.z()) + ", " + QString::number(q.scalar()) + "]";
            lineEdit->setText(vs);
        }
        break;
    default:
        {
            QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
            lineEdit->setText(index.model()->data(index, Qt::EditRole).toString());
        }
        break;
    }
}


void CSpinBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    QVariant::Type data_type = model->data(index, Qt::EditRole).type();

    switch (data_type)
    {
    case QVariant::Double:
        {
            QDoubleSpinBox* spinBox = static_cast<QDoubleSpinBox*>(editor);
            spinBox->interpretText();
            double value = spinBox->value();

            model->setData(index, value, Qt::EditRole);
        }
        break;
    default:
        {
            QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
            model->setData(index, lineEdit->text(), Qt::EditRole);
        }
        break;
    }
}

void CSpinBoxDelegate::updateEditorGeometry(QWidget* editor,
    const QStyleOptionViewItem& option,
    const QModelIndex&/* index */) const
{
    editor->setGeometry(option.rect);
}

void CSpinBoxDelegate::valueChanged(double value)
{
    QDoubleSpinBox* spinBox = static_cast<QDoubleSpinBox*>(sender());
    
    if (value == spinBox->minimum())
    {
        spinBox->blockSignals(true);
        spinBox->setValue( spinBox->maximum() );
        spinBox->blockSignals(false);
    }
    else if (value == spinBox->maximum())
    {
        spinBox->blockSignals(true);
        spinBox->setValue( spinBox->minimum() );
        spinBox->blockSignals(false);
    }
    emit commitData(static_cast<QWidget*>(sender()));
}