#pragma once
#include <qspinbox.h>
class CDoubleSpinBox : public QDoubleSpinBox
{
	Q_OBJECT
public:
	CDoubleSpinBox(QWidget* parent = nullptr) : QDoubleSpinBox(parent) {};
public slots:
	//void setValue(int val) override;
};

