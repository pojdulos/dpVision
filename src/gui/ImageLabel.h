#pragma once
#include "dll_global.h"

#include <qlabel.h>

#include <QBitmap>

class DPVISION_EXPORT ImageLabel : public QLabel
{
	Q_OBJECT

public:
	typedef enum {
		WorkModeUnknown,
		WorkModeSelect,
		WorkModeErase,
		WorkModeZoom
	} WorkMode;


	typedef enum {
		NothingToDo,
		MoveSelection,
		DragBottomRight,
		DragLeft,
		DragRight,
		DragTop,
		DragBottom
	} SelMode;

	ImageLabel(const QString& text, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	ImageLabel(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	ImageLabel(QSize size, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	bool hInside(QPoint p);
	bool vInside(QPoint p);
	bool isLeftMargin(QPoint p);
	bool isRightMargin(QPoint p);
	bool isTopMargin(QPoint p);
	bool isBottomMargin(QPoint p);
	void moveSelection(QPoint);
	~ImageLabel() {};

	double m_scale;
	QRect m_selection;

	QPoint m_eraser_pos;
	int m_eraser_rad;

	WorkMode m_work_mode;
	SelMode m_sel_mode;

	QPoint m_real_offset;

	QImage m_current_image;
	QPixmap m_scaled_pixmap;
	QImage m_mask;

	void setImageDisplayed(const QImage& image, int width = 0, int height = 0);

	inline QImage& mask_ref() { return m_mask; }
	void mask_reset();
	void mask_set(const QImage& _mask);
	void mask_set(const QBitmap& _mask);
	QImage mask_to_image();
	QBitmap mask_to_bitmap();
	static QImage mask_bitmap_to_image(const QBitmap& bitmap);
	static QBitmap mask_image_to_bitmap(const QImage& image);

	//void setPixmap(const QPixmap& p);
	
protected:
	//virtual void keyPressEvent(QKeyEvent* e) override {	emit(keyPressed(e)); };
	virtual void mouseMoveEvent(QMouseEvent* e) override;
	virtual void mousePressEvent(QMouseEvent* e) override;
	virtual void mouseReleaseEvent(QMouseEvent* e) override;

	virtual void wheelEvent(QWheelEvent* e) override;

	virtual void paintEvent(QPaintEvent* e) override;


signals:
	void mousePressed(QMouseEvent* e);
	void mouseMoved(QMouseEvent* e);
	void selectionChanged();// QRect);
	void scaleChanged();
	void keyPressed(QKeyEvent*);
};


