#pragma once
#include <qstyleditemdelegate.h>
#include "CItemEditorFactory.h"

typedef enum {
	Visible,
	TraXYZ,
	TraX,
	TraY,
	TraZ,
	RotXYZW,
	RotX,
	RotY,
	RotZ,
	RotW,
	Sca
} ItemId;


#define QtRoleDataPointer Qt::ItemDataRole::UserRole+1
#define QtRoleDataMin Qt::ItemDataRole::UserRole+2
#define QtRoleDataMax Qt::ItemDataRole::UserRole+3
#define QtRoleDataStep Qt::ItemDataRole::UserRole+4
#define QtRoleDataLoop Qt::ItemDataRole::UserRole+5

class __declspec(dllexport) CSpinBoxDelegate :
    public QStyledItemDelegate
{
    Q_OBJECT

public:
    CSpinBoxDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent)
	{
		//setItemEditorFactory(new CItemEditorFactory);
	};

	QWidget* createSpinBox(QWidget* parent, int min, int max, int step) const;

	QWidget* createDoubleSpinBox(QWidget* parent, double min, double max, double step) const;

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;

    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model,
        const QModelIndex& index) const override;

    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;

public slots:
    void valueChanged(double d);
};

