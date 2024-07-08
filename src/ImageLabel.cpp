#include "ImageLabel.h"

#include <QtCore>
#include <QMouseEvent>
#include <QApplication>
#include <QPainter>
#include <QtWidgets>

///////////////////////////////////////////////////////////////////////////////////////////
// ImageLabel
///////////////////////////////////////////////////////////////////////////////////////////

ImageLabel::ImageLabel(const QString& text, QWidget* parent, Qt::WindowFlags f) : QLabel(text, parent, f)
{
	m_scale = 1.0;
	m_sel = QRect(0, 0, 0, 0);
	m_mode = None;
	m_real_offset = QPoint(0, 0);
	//m_last_mouse_pos = QPoint(0, 0);
}

ImageLabel::ImageLabel(QWidget* parent, Qt::WindowFlags f) : QLabel(parent, f)
{
	m_scale = 1.0;
	m_sel = QRect(0, 0, 0, 0);
	m_mode = None;
	m_real_offset = QPoint(0, 0);
	//m_last_mouse_pos = QPoint(0, 0);
}


bool ImageLabel::hInside(QPoint p)
{
	double x = double(p.x()) / m_scale;
	return (x >= m_sel.left()) && (x < m_sel.right());
}

bool ImageLabel::vInside(QPoint p)
{
	double y = double(p.y()) / m_scale;
	return (y >= m_sel.top()) && (y < m_sel.bottom());
}


bool ImageLabel::isLeftMargin(QPoint p)
{
	double left = m_sel.left() * m_scale;
	bool testX = (p.x() >= left) && (p.x() < left + 2);

	return testX && vInside(p);
}

bool ImageLabel::isRightMargin(QPoint p)
{
	double right = m_sel.right() * m_scale;
	bool testX = (p.x() <= right) && (p.x() > right - 2);

	return testX && vInside(p);
}

bool ImageLabel::isTopMargin(QPoint p)
{
	double top = m_sel.top() * m_scale;
	bool testY = (p.y() >= top) && (p.y() < top + 2);

	return testY && hInside(p);
}

bool ImageLabel::isBottomMargin(QPoint p)
{
	double bottom = m_sel.bottom() * m_scale;
	bool testY = (p.y() <= bottom) && (p.y() > bottom - 2);

	return testY && hInside(p);
}

void ImageLabel::moveSelection(QPoint real_mouse)
{
	QSize size = m_sel.size();

	QPoint new_top_left = real_mouse - m_real_offset;

	m_sel.setTopLeft(new_top_left);
	m_sel.setSize(size);
}

#include "UI.h"

void ImageLabel::mouseMoveEvent(QMouseEvent* e)
{
	UI::STATUSBAR::setText( QString("pozycja kursora: %1 %2").arg(e->pos().x()).arg(e->pos().y()).toStdString() );

	QPoint current_mouse_pos = e->pos();
	
	// przeskalowana pozycja kursora
	QPoint real_mouse_pos = ( QPointF(current_mouse_pos) / m_scale ).toPoint();

	if (e->buttons() == Qt::LeftButton)
	{
		switch (m_mode)
		{
		case Draw:
			if ((real_mouse_pos.x() > m_sel.left()) && (real_mouse_pos.y() > m_sel.top()))
				m_sel.setBottomRight(real_mouse_pos);
			break;
		case Move:
			moveSelection(real_mouse_pos);
			break;
		case DragLeft:
			if (real_mouse_pos.x() < m_sel.right()) m_sel.setLeft(real_mouse_pos.x());
			break;
		case DragRight:
			if (real_mouse_pos.x() > m_sel.left()) m_sel.setRight(real_mouse_pos.x());
			break;
		case DragTop:
			if (real_mouse_pos.y() < m_sel.bottom()) m_sel.setTop(real_mouse_pos.y());
			break;
		case DragBottom:
			if (real_mouse_pos.y() > m_sel.top()) m_sel.setBottom(real_mouse_pos.y());
			break;
		case None:
			break;
		}

		emit(selectionChanged());// m_sel.toRect()));
	}
	else
	{
		if (isLeftMargin(e->pos()))
		{
			this->setCursor(Qt::CursorShape::SizeHorCursor);
			m_mode = DragLeft;
		}
		else if (isRightMargin(e->pos()))
		{
			this->setCursor(Qt::CursorShape::SizeHorCursor);
			m_mode = DragRight;
		}
		else if (isTopMargin(e->pos()))
		{
			this->setCursor(Qt::CursorShape::SizeVerCursor);
			m_mode = DragTop;
		}
		else if (isBottomMargin(e->pos()))
		{
			this->setCursor(Qt::CursorShape::SizeVerCursor);
			m_mode = DragBottom;
		}
		else if (hInside(e->pos()) && vInside(e->pos()))
		{
			this->setCursor(Qt::CursorShape::DragMoveCursor);
			m_mode = Move;
		}
		else
		{
			this->setCursor(Qt::CursorShape::CrossCursor);
			m_mode = None;
			m_real_offset = QPoint(0, 0);
		}
	}
	this->update();
}

void ImageLabel::mousePressEvent(QMouseEvent* e)
{
	QPoint current_mouse_pos = e->pos();

	// przeskalowana pozycja kursora
	QPoint real_mouse_pos = ( QPointF(current_mouse_pos) / m_scale ).toPoint();

	if (e->buttons() == Qt::LeftButton)
	{
		if (m_mode == Move)
		{
			m_real_offset = real_mouse_pos - m_sel.topLeft();
			qInfo() << m_real_offset;

			emit(selectionChanged());
		}
		else if (m_mode == None)
		{
			m_mode = Draw;
			m_real_offset = QPoint(0, 0);
			m_sel.setTopLeft(real_mouse_pos);
			m_sel.setSize(QSize(0, 0));

			emit(selectionChanged());
		}
	}
	this->update();
}

void ImageLabel::mouseReleaseEvent(QMouseEvent* e)
{
	m_mode = None;
	m_real_offset = QPoint(0, 0);
}

void ImageLabel::wheelEvent(QWheelEvent* e)
{
	if (Qt::ControlModifier == QApplication::keyboardModifiers())
	{
		this->m_scale += (e->angleDelta().y() > 0) ? 0.2 : -0.2;
		if (this->m_scale <= 0.0) this->m_scale = 0.2;
		if (this->m_scale > 20.0) this->m_scale = 20.0;
		emit(scaleChanged());
	}
	else
	{
		QLabel::wheelEvent(e);
	}
	this->update();
}


void ImageLabel::paintEvent(QPaintEvent* e)
{
	QLabel::paintEvent(e);

	QPainter painter(this);
	painter.drawPixmap(0, 0, m_orgPixmap);
	painter.drawPixmap(0, 0, m_mask);
	
	if (m_sel.isValid())
	{
		QRectF rect(m_sel.x() * m_scale, m_sel.y() * m_scale, m_sel.width() * m_scale, m_sel.height() * m_scale);

		QPen pen(QColor("#ff0000"), 1, Qt::PenStyle::SolidLine, Qt::FlatCap, Qt::RoundJoin);

		painter.setPen(pen);
		painter.drawRect(rect);
	}
}


void ImageLabel::setPixmap(const QPixmap& p)
{
	m_orgPixmap = p;
	
	m_mask = m_mask.scaled(p.size(), Qt::KeepAspectRatio);
	//m_mask.fill(Qt::transparent);
}
