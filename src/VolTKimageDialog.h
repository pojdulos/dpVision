#pragma once
#ifndef VOLTKIMAGEDIALOG_H
#define VOLTKIMAGEDIALOG_H

#include "ui_volumetricImageDialog.h"
#include "Volumetric.h"

class ImageLabel;

#include "FilterDialog.h"

#include "dll_global.h"
class DPVISION_EXPORT VolTKimageDialog : public QDialog
{
	Q_OBJECT

public:
	explicit VolTKimageDialog(Volumetric* volTK, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~VolTKimageDialog() {};

	inline Ui::VolumetricImageDialog& getUI() { return ui; };

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

	void setTreshWidget(bool check, int min, int max, Volumetric::VoxelType low, Volumetric::VoxelType up);
	bool getTresh(Volumetric::VoxelType& lowT, Volumetric::VoxelType& upT);

	static void test_dialog(Volumetric*);

protected:
	virtual void resizeEvent(QResizeEvent* e) override;

public slots:
	void onTreshValueChanged(int i);
	void onTreshCheckBox(int i);
	void onSliderValueChanged(int i);
	void onUpperTreshValueChanged(int i);
	void onFitCheckBox(int);
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
	//int mediana0(NowaKostka1& kostka, size_t dest0adress, int filterSize);
	//void do3dMedianFilter(NowaKostka1& src, NowaKostka1& dst, int size);
	void onCreateVolTKStructureBtn() {};
	void onImageScaleChanged();
	void onImageMousePressed(QPoint);

private:
	Ui::VolumetricImageDialog ui;

	Volumetric* m_VolTK, m_rotated_volum;
	Volumetric::Layer currentView;
	int dispBits;

	void changeDisplayedPixmap(int i);
	void changeDisplayedPixmap(QImage image);
};

#endif