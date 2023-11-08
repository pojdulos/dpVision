#pragma once
#include "dll_global.h"

#include <qlabel.h>

class DPVISION_EXPORT ImageLabel : public QLabel
{
	Q_OBJECT

public:
	typedef enum {
		None,
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
	~ImageLabel() {};

	double m_scale;
	QRectF m_sel;
	SelMode m_mode;

	QPointF m_offs;
	QPixmap m_orgPixmap;

	void setPixmap(const QPixmap& p);


protected:
	virtual void mouseMoveEvent(QMouseEvent* e) override;
	virtual void mousePressEvent(QMouseEvent* e) override;

	virtual void paintEvent(QPaintEvent* e) override;

signals:
	void selectionChanged();// QRect);

};


