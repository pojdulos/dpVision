#include "ImageLabel.h"

#include <QtCore>
#include <QMouseEvent>
#include <QApplication>
#include <QPainter>
#include <QtWidgets>

#include <omp.h>


///////////////////////////////////////////////////////////////////////////////////////////
// ImageLabel
///////////////////////////////////////////////////////////////////////////////////////////

ImageLabel::ImageLabel(const QString& text, QWidget* parent, Qt::WindowFlags f) : QLabel(text, parent, f)
{
	m_scale = 1.0;
	m_selection = QRect(0, 0, 0, 0);
	m_work_mode = WorkMode::WorkModeSelect;
	m_sel_mode = SelMode::NothingToDo;
	m_real_offset = QPoint(0, 0);
	//m_last_mouse_pos = QPoint(0, 0);
	m_eraser_pos = QPoint(0, 0);
	m_eraser_rad = 5;

	m_mask = QImage(200, 200, QImage::Format::Format_Mono);
	m_mask.fill(Qt::GlobalColor::transparent);
}

ImageLabel::ImageLabel(QWidget* parent, Qt::WindowFlags f) : QLabel(parent, f)
{
	m_scale = 1.0;
	m_selection = QRect(0, 0, 0, 0);
	m_work_mode = WorkMode::WorkModeSelect;
	m_sel_mode = SelMode::NothingToDo;
	m_real_offset = QPoint(0, 0);
	//m_last_mouse_pos = QPoint(0, 0);
	m_eraser_pos = QPoint(0, 0);
	m_eraser_rad = 25;

	m_mask = QImage(200, 200, QImage::Format::Format_Mono);
	m_mask.fill(Qt::GlobalColor::transparent);
}

ImageLabel::ImageLabel(QSize size, QWidget* parent, Qt::WindowFlags f) : QLabel(parent, f)
{
	m_scale = 1.0;
	m_selection = QRect(0, 0, 0, 0);
	m_work_mode = WorkMode::WorkModeSelect;
	m_sel_mode = SelMode::NothingToDo;
	m_real_offset = QPoint(0, 0);
	//m_last_mouse_pos = QPoint(0, 0);
	m_eraser_pos = QPoint(0, 0);
	m_eraser_rad = 25;

	m_mask = QImage(size, QImage::Format::Format_ARGB32);
	m_mask.fill(Qt::GlobalColor::transparent);
}


bool ImageLabel::hInside(QPoint p)
{
	double x = double(p.x()) / m_scale;
	return (x >= m_selection.left()) && (x < m_selection.right());
}

bool ImageLabel::vInside(QPoint p)
{
	double y = double(p.y()) / m_scale;
	return (y >= m_selection.top()) && (y < m_selection.bottom());
}


bool ImageLabel::isLeftMargin(QPoint p)
{
	double left = m_selection.left() * m_scale;
	bool testX = (p.x() >= left) && (p.x() < left + 2);

	return testX && vInside(p);
}

bool ImageLabel::isRightMargin(QPoint p)
{
	double right = m_selection.right() * m_scale;
	bool testX = (p.x() <= right) && (p.x() > right - 2);

	return testX && vInside(p);
}

bool ImageLabel::isTopMargin(QPoint p)
{
	double top = m_selection.top() * m_scale;
	bool testY = (p.y() >= top) && (p.y() < top + 2);

	return testY && hInside(p);
}

bool ImageLabel::isBottomMargin(QPoint p)
{
	double bottom = m_selection.bottom() * m_scale;
	bool testY = (p.y() <= bottom) && (p.y() > bottom - 2);

	return testY && hInside(p);
}

void ImageLabel::moveSelection(QPoint real_mouse)
{
	QSize size = m_selection.size();

	QPoint new_top_left = real_mouse - m_real_offset;

	m_selection.setTopLeft(new_top_left);
	m_selection.setSize(size);
}

#include "UI.h"


void ImageLabel::mouseMoveEvent(QMouseEvent* e)
{
	QPoint mouse_position = e->pos();
	
	// przeskalowana pozycja kursora
	QPoint position_in_image = (QPointF(mouse_position) / m_scale).toPoint();

	position_in_image.setX(std::max(0, std::min(m_current_image.width() - 1, position_in_image.x())));
	position_in_image.setY(std::max(0, std::min(m_current_image.height() - 1, position_in_image.y())));


	if (m_work_mode == WorkMode::WorkModeSelect) {
		if (e->buttons() == Qt::LeftButton)
		{
			switch (m_sel_mode)
			{
			case DragBottomRight:
				//qDebug() << "Dragging to (" << pos.x() << "," << pos.y() << ")";

				if ((position_in_image.x() > m_selection.left()) && (position_in_image.y() > m_selection.top()))
					m_selection.setBottomRight(position_in_image);
				break;
			case MoveSelection:
				moveSelection(position_in_image);
				break;
			case DragLeft:
				if (position_in_image.x() < m_selection.right()) m_selection.setLeft(position_in_image.x());
				break;
			case DragRight:
				if (position_in_image.x() > m_selection.left()) m_selection.setRight(position_in_image.x());
				break;
			case DragTop:
				if (position_in_image.y() < m_selection.bottom()) m_selection.setTop(position_in_image.y());
				break;
			case DragBottom:
				if (position_in_image.y() > m_selection.top()) m_selection.setBottom(position_in_image.y());
				break;
			case NothingToDo:
				break;
			}

			emit(selectionChanged());// m_sel.toRect()));
		}
		else
		{
			if (isLeftMargin(e->pos()))
			{
				this->setCursor(Qt::CursorShape::SizeHorCursor);
				m_sel_mode = SelMode::DragLeft;
			}
			else if (isRightMargin(e->pos()))
			{
				this->setCursor(Qt::CursorShape::SizeHorCursor);
				m_sel_mode = SelMode::DragRight;
			}
			else if (isTopMargin(e->pos()))
			{
				this->setCursor(Qt::CursorShape::SizeVerCursor);
				m_sel_mode = SelMode::DragTop;
			}
			else if (isBottomMargin(e->pos()))
			{
				this->setCursor(Qt::CursorShape::SizeVerCursor);
				m_sel_mode = SelMode::DragBottom;
			}
			else if (hInside(e->pos()) && vInside(e->pos()))
			{
				this->setCursor(Qt::CursorShape::DragMoveCursor);
				m_sel_mode = SelMode::MoveSelection;
			}
			else
			{
				this->setCursor(Qt::CursorShape::CrossCursor);
				m_sel_mode = SelMode::NothingToDo;
				m_real_offset = QPoint(0, 0);
			}
		}
	}
	else if (m_work_mode == WorkMode::WorkModeErase) {
		m_eraser_pos = position_in_image;

		if (e->buttons() == Qt::LeftButton)
		{
			QPainter painter(&m_mask);
			//painter.setPen(Qt::NoPen);
			if (e->modifiers() & Qt::ShiftModifier)
			{
				painter.setCompositionMode(QPainter::CompositionMode_Clear);
				painter.setBrush(QColor(0, 0, 0, 0));
				painter.setPen(QColor(0, 0, 0, 0));
			}
			else
			{
				painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
				painter.setBrush(QColor(0, 255, 255));
				painter.setPen(QColor(0, 255, 255));
			}
			//if (m_eraser_rad > 1)
				painter.drawEllipse(m_eraser_pos, m_eraser_rad, m_eraser_rad);
			//else if (m_eraser_rad > 0)
			//	painter.drawRect(m_eraser_pos.x()-1,m_eraser_pos.y()-1,3,3);
			//else
			//	painter.drawPoint(m_eraser_pos);
			painter.end();
		}
	}
	else
	{
		emit(mouseMoved(e));
	}

	this->update();

	// Usuwa zalegle komunikaty z kolejki. POTENCJALNIE NIEBEZPIECZNE
	QCoreApplication::removePostedEvents(this, QEvent::MouseMove);
}

void ImageLabel::mousePressEvent(QMouseEvent* e)
{
	QPoint mouse_position = e->pos();

	// przeskalowana pozycja kursora
	QPoint position_in_image = (QPointF(mouse_position) / m_scale).toPoint();

	position_in_image.setX(std::max(0, std::min(m_current_image.width() - 1, position_in_image.x())));
	position_in_image.setY(std::max(0, std::min(m_current_image.height() - 1, position_in_image.y())));


	if (m_work_mode == WorkMode::WorkModeSelect) {
		if (e->buttons() == Qt::LeftButton)
		{
			if (m_sel_mode == SelMode::MoveSelection)
			{
				m_real_offset = position_in_image - m_selection.topLeft();
				qInfo() << m_real_offset;

				emit(selectionChanged());
			}
			else if (m_sel_mode == SelMode::NothingToDo)
			{
				m_sel_mode = SelMode::DragBottomRight;
				m_real_offset = QPoint(0, 0);
				m_selection.setTopLeft(position_in_image);
				m_selection.setSize(QSize(0, 0));

				emit(selectionChanged());
			}
		}
	}
	else if (m_work_mode == WorkMode::WorkModeErase) {
		m_eraser_pos = position_in_image;

		if (e->buttons() == Qt::LeftButton)
		{
			QPainter painter(&m_mask);
			//painter.setPen(Qt::NoPen);
			if (e->modifiers() & Qt::ShiftModifier)
			{
				painter.setCompositionMode(QPainter::CompositionMode_Clear);
				painter.setBrush(QColor(0, 0, 0, 0));
				painter.setPen(QColor(0, 0, 0, 0));
			}
			else
			{
				painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
				painter.setBrush(QColor(0, 255, 255));
				painter.setPen(QColor(0, 255, 255));
			}
			//if (m_eraser_rad > 1)
				painter.drawEllipse(m_eraser_pos, m_eraser_rad, m_eraser_rad);
			//else if (m_eraser_rad > 0)
			//	painter.drawRect(m_eraser_pos.x() - 1, m_eraser_pos.y() - 1, 3, 3);
			//else
			//	painter.drawPoint(m_eraser_pos);
			painter.end();
		}
	}

	emit(mousePressed(e));

	this->update();
}

void ImageLabel::mouseReleaseEvent(QMouseEvent* e)
{
	m_sel_mode = SelMode::NothingToDo;
	m_real_offset = QPoint(0, 0);
}

void ImageLabel::wheelEvent(QWheelEvent* e)
{

	if (Qt::ControlModifier & e->modifiers())
	{
		if (Qt::ShiftModifier & e->modifiers())
		{
			if (m_work_mode == WorkMode::WorkModeErase)
			{
				m_eraser_rad += (e->angleDelta().y() > 0) ? 1 : -1;

				if (m_eraser_rad < 2) m_eraser_rad = 2;
				if (m_eraser_rad > 100) m_eraser_rad = 100;
				e->accept();
			}
		}
		else
		{
			this->m_scale += (e->angleDelta().y() > 0) ? 0.2 : -0.2;
			if (this->m_scale <= 0.0) this->m_scale = 0.2;
			if (this->m_scale > 20.0) this->m_scale = 20.0;
			emit(scaleChanged());
		}
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
	painter.drawPixmap(0, 0, m_scaled_pixmap);

	if (m_work_mode == WorkMode::WorkModeSelect) {
		if (m_selection.isValid())
		{
			QRectF rect(m_selection.x() * m_scale, m_selection.y() * m_scale, m_selection.width() * m_scale, m_selection.height() * m_scale);

			QPen pen(QColor("#ff0000"), 1, Qt::PenStyle::SolidLine, Qt::FlatCap, Qt::RoundJoin);

			painter.setPen(pen);
			painter.drawRect(rect);
		}
	}
	else if (m_work_mode == WorkMode::WorkModeErase)
	{
		QImage scaled_mask = m_mask.scaled(m_scaled_pixmap.size(), Qt::KeepAspectRatio);
		
		QImage newImg(m_scaled_pixmap.size(), QImage::Format_ARGB32);
		newImg.fill(Qt::transparent);

		QPainter ptr(&newImg);
		ptr.setOpacity(0.3);// 51);
		ptr.drawImage(scaled_mask.rect(), scaled_mask);

		ptr.setPen(Qt::cyan);
		//ptr.drawText(5, 15, "Select areas for deletion, and press <DEL>");

		ptr.drawEllipse(QPointF(m_eraser_pos) * m_scale, m_scale * m_eraser_rad, m_scale * m_eraser_rad);

		ptr.end();

		painter.drawImage(0, 0, newImg);

	}

}

void ImageLabel::mask_reset()
{
	m_mask = QImage(m_current_image.size(), QImage::Format::Format_ARGB32);
	m_mask.fill(Qt::GlobalColor::transparent);

	this->update();
}

void ImageLabel::mask_set(const QImage& _mask)
{
	m_mask = _mask.copy();

	this->update();
}

void ImageLabel::mask_set(const QBitmap& _mask)
{
	QImage monoImage = _mask.toImage();
	//QImage result(monoImage.size(), QImage::Format_ARGB32);

#pragma omp parallel for
	for (int y = 0; y < monoImage.height(); ++y) {
		for (int x = 0; x < monoImage.width(); ++x) {
			m_mask.setPixel(x, y, qBlue(monoImage.pixel(x, y)) ? qRgba(0, 255, 255, 255) : qRgba(0, 0, 0, 0));
		}
	}

	this->update();
}

QImage ImageLabel::mask_to_image()
{
	return m_mask.copy();
}

QBitmap ImageLabel::mask_to_bitmap()
{
	QImage monoImage(m_mask.size(), QImage::Format_Mono);

#pragma omp parallel for
	for (int y = 0; y < m_mask.height(); ++y) {
		for (int x = 0; x < m_mask.width(); ++x) {
			monoImage.setPixel(x, y, qAlpha(m_mask.pixel(x, y)) > 0 ? 1 : 0);
		}
	}

	return QBitmap::fromImage(monoImage);
}

inline QImage ImageLabel::mask_bitmap_to_image(const QBitmap& bitmap)
{
	QImage monoImage = bitmap.toImage();
	QImage result(monoImage.size(), QImage::Format_ARGB32);

#pragma omp parallel for
	for (int y = 0; y < monoImage.height(); ++y) {
		for (int x = 0; x < monoImage.width(); ++x) {
			result.setPixel(x, y, qBlue(monoImage.pixel(x, y)) ? qRgba(0, 255, 255, 255) : qRgba(0, 0, 0, 0));
		}
	}

	return result;
}

inline QBitmap ImageLabel::mask_image_to_bitmap(const QImage& image)
{
	QImage monoImage(image.size(), QImage::Format_Mono);

#pragma omp parallel for
	for (int y = 0; y < image.height(); ++y) {
		for (int x = 0; x < image.width(); ++x) {
			monoImage.setPixel(x, y, qAlpha(image.pixel(x, y)) > 0 ? 1 : 0);
		}
	}

	return QBitmap::fromImage(monoImage);
}

void ImageLabel::setImageDisplayed(const QImage& image, int width, int height)
{
	m_current_image = image.copy();

	if (width > 0 && height > 0) // use it for fit to window
	{
		m_scaled_pixmap = QPixmap::fromImage(m_current_image).scaled(width, height, Qt::KeepAspectRatio);

		m_scale = double(m_scaled_pixmap.width()) / m_current_image.width();
	}
	else if (m_scale == 1.0) // original scale
	{
		m_scaled_pixmap = QPixmap::fromImage(m_current_image);
	}
	else // true scale
	{
		int w = round(double(image.width()) * m_scale);
		int h = round(double(image.height()) * m_scale);

		m_scaled_pixmap = QPixmap::fromImage(m_current_image).scaled(w, h, Qt::KeepAspectRatio);
	}

	QPixmap pusta = m_scaled_pixmap.copy();
	pusta.fill(Qt::GlobalColor::transparent);

	QLabel::setPixmap(pusta);

	// Dostosowanie rozmiaru etykiety po ustawieniu nowej pixmapy
	this->adjustSize();
}

//void ImageLabel::setPixmap(const QPixmap& p)
//{
//	m_scaled_pixmap = p;
//	
//
//	QPixmap pusta = p;
//	pusta.fill(Qt::GlobalColor::transparent);
//
//	QLabel::setPixmap(pusta);
//
//	// Dostosowanie rozmiaru etykiety po ustawieniu nowej pixmapy
//	this->adjustSize();
//
//	//this->resize(pusta.size());
//}
