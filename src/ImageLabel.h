#pragma once
#include "dll_global.h"

#include <qlabel.h>


class DPVISION_EXPORT ImageLabel : public QLabel
{
	Q_OBJECT

public:
	typedef enum {
		None,
		Draw,
		Move,
		DragLeft,
		DragRight,
		DragTop,
		DragBottom
	} SelMode;

	ImageLabel(const QString& text, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	ImageLabel(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	bool hInside(QPoint p);
	bool vInside(QPoint p);
	bool isLeftMargin(QPoint p);
	bool isRightMargin(QPoint p);
	bool isTopMargin(QPoint p);
	bool isBottomMargin(QPoint p);
	void moveSelection(QPoint);
	~ImageLabel() {};

	double m_scale;
	QRect m_sel;
	SelMode m_mode;

	QPoint m_real_offset;

	QPixmap m_orgPixmap;
	QPixmap m_mask;

	void setPixmap(const QPixmap& p);
	
protected:
	virtual void mouseMoveEvent(QMouseEvent* e) override;
	virtual void mousePressEvent(QMouseEvent* e) override;
	virtual void mouseReleaseEvent(QMouseEvent* e) override;

	virtual void wheelEvent(QWheelEvent* e) override;

	virtual void paintEvent(QPaintEvent* e) override;

signals:
	void mousePressed(QPoint);
	void selectionChanged();// QRect);
	void scaleChanged();
};


