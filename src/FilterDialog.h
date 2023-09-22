#pragma once
#include <QDialog>

#ifndef FILTERDIALOG19131037_H
#define FILTERDIALOG19131037_H

class __declspec(dllexport) Filter
{
	int m_size;
	std::vector<double> matrix;

public:
	Filter() {
		m_size = 3;
		matrix.resize(9, 1.0);
	
		//for (int i = 0; i < 9; i++) matrix[i] = 1.0;
	}

	int setSize(int s) {
		m_size = (s > 8) ? 9 : (s > 6) ? 7 : (s > 4) ? 5 : 3;
		matrix.resize(m_size * m_size, 1.0);
		return m_size;
	}
	int getSize() {	return m_size; }

	double& operator()(int r, int c) { 
		if (c > (m_size - 1)) c = m_size - 1;
		if (r > (m_size - 1)) r = m_size - 1;
		if (c < 0) c = 0;
		if (r < 0) r = 0;

		return matrix[c + m_size * r];
	}
};


#include "ui_FilterDialog.h"

class __declspec(dllexport) FilterDialog : public QDialog
{
	Q_OBJECT

public:
	explicit FilterDialog(Filter* fltr, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~FilterDialog() {};

private:
	Ui::FilterDialog ui;

	Filter* m_Filter;

private slots:
	void spinValueChanged(double);
};

#endif