#include "ImageLabel.h"

#include <QMouseEvent>
#include <QPainter>

///////////////////////////////////////////////////////////////////////////////////////////
// ImageLabel
///////////////////////////////////////////////////////////////////////////////////////////

ImageLabel::ImageLabel(const QString& text, QWidget* parent, Qt::WindowFlags f) : QLabel(text, parent, f)
{
	m_scale = 1;
	m_sel = QRectF(0, 0, 0, 0);
	m_mode = None;
	m_offs = QPointF(0, 0);
}

ImageLabel::ImageLabel(QWidget* parent, Qt::WindowFlags f) : QLabel(parent, f)
{
	m_scale = 1;
	m_sel = QRectF(0, 0, 0, 0);
	m_mode = None;
	m_offs = QPointF(0, 0);
}


bool ImageLabel::hInside(QPoint p)
{
	return (p.x() >= m_sel.left() / m_scale) && (p.x() < m_sel.right() / m_scale);
}

bool ImageLabel::vInside(QPoint p)
{
	return (p.y() >= m_sel.top() / m_scale) && (p.y() < m_sel.bottom() / m_scale);
}


bool ImageLabel::isLeftMargin(QPoint p)
{
	double left = m_sel.left() / m_scale;
	bool testX = (p.x() >= left) && (p.x() < left + 2);

	return testX && vInside(p);
}

bool ImageLabel::isRightMargin(QPoint p)
{
	double right = m_sel.right() / m_scale;
	bool testX = (p.x() <= right) && (p.x() > right - 2);

	return testX && vInside(p);
}

bool ImageLabel::isTopMargin(QPoint p)
{
	double top = m_sel.top() / m_scale;
	bool testY = (p.y() >= top) && (p.y() < top + 2);

	return testY && hInside(p);
}

bool ImageLabel::isBottomMargin(QPoint p)
{
	double bottom = m_sel.bottom() / m_scale;
	bool testY = (p.y() <= bottom) && (p.y() > bottom - 2);

	return testY && hInside(p);
}

#include "UI.h"

void ImageLabel::mouseMoveEvent(QMouseEvent* e)
{
	UI::STATUSBAR::setText( QString("pozycja kursora: %1 %2").arg(e->pos().x()).arg(e->pos().y()).toStdString() );

	QPointF realPos = m_scale * QPointF(e->pos());

	if (e->buttons() == Qt::LeftButton)
	{
		//dragging

		switch (m_mode)
		{
		case DragLeft:
			if (realPos.x() < m_sel.right()) m_sel.setLeft(realPos.x());
			break;
		case DragRight:
			if (realPos.x() > m_sel.left()) m_sel.setRight(realPos.x());
			break;
		case DragTop:
			if (realPos.y() < m_sel.bottom()) m_sel.setTop(realPos.y());
			break;
		case DragBottom:
			if (realPos.y() > m_sel.top()) m_sel.setBottom(realPos.y());
			break;
		case None:
			if ((realPos.x() > m_sel.left()) && (realPos.y() > m_sel.top())) m_sel.setBottomRight(realPos);
			break;
		case Move:
			QSizeF wh = m_sel.size();
			m_sel.setTopLeft(m_offs + realPos);
			m_sel.setSize(wh);
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
		}
	}

}

void ImageLabel::mousePressEvent(QMouseEvent* e)
{
	if (e->buttons() == Qt::LeftButton)
	{
		if (m_mode == Move)
		{
			m_offs = m_sel.topLeft() - m_scale * QPointF(e->pos());
		}
		else if (m_mode == None)
		{
			m_sel.setTopLeft(m_scale * QPointF(e->pos()));
			m_sel.setSize(QSizeF(0, 0));
		}
	}
	else
	{
		m_offs = QPointF(0, 0);
		m_sel.setTopLeft(QPointF(0, 0));
		m_sel.setSize(QSizeF(0, 0));
	}
	emit(selectionChanged());
}

void ImageLabel::paintEvent(QPaintEvent* e)
{
	QLabel::paintEvent(e);

	QPixmap p = m_orgPixmap.copy();

	if (m_sel.isValid())
	{
		QPainter painter(&p);

		QRectF rect(m_sel.x() / m_scale, m_sel.y() / m_scale, m_sel.width() / m_scale, m_sel.height() / m_scale);

		QPen pen(QColor("#ff0000"), 1, Qt::PenStyle::SolidLine, Qt::FlatCap, Qt::RoundJoin);

		painter.setPen(pen);
		painter.drawRect(rect);
	}

	QLabel::setPixmap(p);
}

void ImageLabel::setPixmap(const QPixmap& p)
{
	m_orgPixmap = p;
	QLabel::setPixmap(p);
}
