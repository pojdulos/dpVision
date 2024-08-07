#pragma once
#ifndef VOLTKIMAGEDIALOG_H
#define VOLTKIMAGEDIALOG_H

#include "ui_volumetricImageDialog.h"
#include "Volumetric.h"

class ImageLabel;

#include <QMenu>
#include <QBitmap>
#include <QPoint>

#include "FilterDialog.h"

#include "dll_global.h"
class DPVISION_EXPORT VolumetricImageDialog : public QDialog
{
	Q_OBJECT
public:
	explicit VolumetricImageDialog(Volumetric* volTK, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~VolumetricImageDialog() {};

	inline Ui::VolumetricImageDialog& getUI() { return ui; };

	void initContent(std::vector<QString> paths);

	void on_del_in_select_mode(Qt::KeyboardModifiers modifiers);
	void on_del_in_eraser_mode(Qt::KeyboardModifiers modifiers);

	ImageLabel* m_imageLabel;

	Filter conv_filter;

	bool m_is16bit;
	QString m_path;
	std::vector<QString> m_names;

	QVector<QBitmap> m_masks;

	int current_i;
	bool common_mask;

	double back_scale;

	void setPath(QString path) { m_path = path; }
	void addName(QString name);

	void setSelection(int tx, int ty, int bx, int by);
	void setSelection(QRect s);
	QRect getSelection();

	void setTreshWidget(bool check, int min, int max, Volumetric::VoxelType low, Volumetric::VoxelType up);
	bool getTresh(Volumetric::VoxelType& lowT, Volumetric::VoxelType& upT);

	static void test_dialog(Volumetric*);
	QMenu* createLabelMenu();

protected:
	virtual void resizeEvent(QResizeEvent* e) override;
	virtual void keyPressEvent(QKeyEvent* e) override { onImageKeyPressed(e); };
	virtual void mousePressEvent(QMouseEvent* e) override;

public slots:
	void on_hide_all_above_upper_toggled(bool);
	void on_A_btn_toggled(bool);
	void on_B_btn_toggled(bool);
	void on_C_btn_toggled(bool);
	void on_d_btn_toggled(bool b);
	void on_go_first_clicked();
	void on_go_next_clicked();
	void on_go_last_clicked();
	void on_go_prev_clicked();

	void on_cam_yaw_valueChanged(int v);
	void on_cam_pitch_valueChanged(int v);

	void on_norm_x_valueChanged(double x);
	void on_norm_y_valueChanged(double y);
	void on_norm_z_valueChanged(double z);

	void on_orig_x_valueChanged(double x);
	void on_orig_y_valueChanged(double y);
	void on_orig_z_valueChanged(double z);

	void on_rtg_toggled(bool b);
	void on_img_invert_toggled(bool b);
	void on_displ_win_toggled(bool b);
	void on_displ_min_valueChanged(int v);
	void on_displ_max_valueChanged(int v);

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
	void clear_masks();
	void resize_masks();
	void onViewPlaneChanged(QString);
	void onDispBitsChanged(QString);
	QImage aplyConvFilter(QImage srcImg, Filter fltr);
	void onConvFilterBtn();
	//int mediana0(NowaKostka1& kostka, size_t dest0adress, int filterSize);
	//void do3dMedianFilter(NowaKostka1& src, NowaKostka1& dst, int size);
	void onCreateVolTKStructureBtn() {};
	void onImageScaleChanged();
	void onImageMousePressed(QMouseEvent* e);
	void onImageMouseMoved(QMouseEvent* e);
	void onImageKeyPressed(QKeyEvent* e);

	void test_slot() { qInfo() << "test slot"; };

private:
	Ui::VolumetricImageDialog ui;

	Volumetric* m_VolTK, m_rotated_volum;
	Volumetric::LayerPlane currentView;
	int dispBits;

	QPoint lastPos;

	CVector3f free_norm;
	CPoint3f free_origin;
	CVector3f free_up;

	CPoint3f free_view_size;
	CPoint3f calculateProjectionDimensions(const CPoint3f& orig, const CVector3f& norm, const CVector3f& up, float columns, float rows, float layers);

	void calcFreeValues();

	void changeDisplayedPixmap(int i, bool forget_mask = false);
	void changeDisplayedPixmap(QImage image, int i = -1);
};

#endif