#pragma once
#ifndef TKIMAGEDIALOG_H
#define TKIMAGEDIALOG_H

#include "ui_TKimageDialog.h"

class ImageLabel;

class __declspec(dllexport) TKimageDialog : public QDialog
{
	Q_OBJECT

public:
	explicit TKimageDialog(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~TKimageDialog() {};

	inline Ui::TKimageDialog& getUI() { return ui; };

	void initContent(std::vector<QString> paths);


	ImageLabel* m_imageLabel;

	bool m_is16bit;
	QString m_path;
	std::vector<QString> m_names;

	void setPath(QString path) { m_path = path; }
	void addName(QString name);

	void setSelection(int tx, int ty, int bx, int by);
	void setSelection(QRect s);
	QRect getSelection();

	void setTreshWidget(bool check, int min, int max, uint16_t low, uint16_t up);
	bool getTresh(uint16_t& lowT, uint16_t& upT);

protected:
	virtual void resizeEvent(QResizeEvent* e) override;

public slots:
	void onTreshValueChanged(int i);
	void onTreshCheckBox(int i);
	void onSliderValueChanged(int i);
	void onUpperTreshValueChanged(int i);
	void onFitCheckBox(int i);
	void onSelectionChanged();// QRect);

	void onSelLeftEdited(int);
	void onSelRightEdited(int);
	void onSelTopEdited(int);
	void onSelBottomEdited(int);

	void onLowerTreshEdited(int);
	void onUpperTreshEdited(int);

private:
	Ui::TKimageDialog ui;

	void changeDisplayedPixmap(int i);
};

#endif