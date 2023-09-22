#pragma once
#include <qitemeditorfactory.h>
class CItemEditorFactory :
    public QItemEditorFactory
{
    QWidget* createDoubleSpinBox(QWidget* parent, double min, double max, double step) const;
    QWidget* createEditor(QVariant::Type type, QWidget* parent) const;
};

