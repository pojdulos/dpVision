#include "CItemEditorFactory.h"
#include "qspinbox.h"
#include "qlineedit.h"

QWidget* CItemEditorFactory::createDoubleSpinBox(QWidget* parent, double min, double max, double step) const
{
    QDoubleSpinBox* editor = new QDoubleSpinBox(parent);
    editor->setFrame(false);
    editor->setMinimum(0);
    editor->setMaximum(100);
    editor->setSingleStep(0.1);

    return editor;
}

QWidget* CItemEditorFactory::createEditor(QVariant::Type type, QWidget* parent) const
{
    QWidget* editor;
    switch (type)
    {
    case QVariant::Type::Double:
        editor = createDoubleSpinBox(parent, 0, 100, 0.1);
        //QObject::connect(editor, SIGNAL(valueChanged(double)), this, SLOT(valueChanged(double)));
        break;
    default:
        editor = new QLineEdit(parent);
        break;
    }


    return editor;
}
