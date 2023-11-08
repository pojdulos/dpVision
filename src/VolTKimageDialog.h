#pragma once
#ifndef VOLTKIMAGEDIALOG_H
#define VOLTKIMAGEDIALOG_H

#include "ui_VolTKimageDialog.h"
#include "VolTK.h"

class ImageLabel;

#include "FilterDialog.h"

class DPVISION_EXPORT VolTKimageDialog : public QDialog
{
	Q_OBJECT

public:
	explicit VolTKimageDialog(CVolTK* volTK, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~VolTKimageDialog() {};

	inline Ui::VolTKimageDialog& getUI() { return ui; };

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
	void onViewPlaneChanged(QString);
	void onDispBitsChanged(QString);
	QImage aplyConvFilter(QImage srcImg, Filter fltr);
	void onConvFilterBtn();
	int mediana0(NowaKostka1& kostka, size_t dest0adress, int filterSize);
	void do3dMedianFilter(NowaKostka1& src, NowaKostka1& dst, int size);
	void onCreateVolTKStructureBtn();

private:
	Ui::VolTKimageDialog ui;

	CVolTK* m_VolTK;
	CVolTK::Layer currentView;
	int dispBits;

	void changeDisplayedPixmap(int i);
	void changeDisplayedPixmap(QImage image);
};

#endif