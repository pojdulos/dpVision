#include "Image.h"

#include "PicViewer.h"
#include "GLViewer.h"
#include "MdiChild.h"

#include "MainWindow.h"

#include "StatusBarManager.h"

MdiChild::MdiChild(Type t) : QWidget(), m_type( t )
{
	QVBoxLayout* mdiChildLayout = new QVBoxLayout(this);
	mdiChildLayout->setMargin(0);
	mdiChildLayout->setSpacing(0);
	mdiChildLayout->setObjectName(QString::fromUtf8("mdiChildLayout"));
	mdiChildLayout->setContentsMargins(0, 0, 0, 0);
	
	if (m_type == Type::GL)
	{
		m_widget = new GLViewer();
	}
	else if (m_type == Type::Pic)
	{
		m_widget = new PicViewer(this);
	}

	
	mdiChildLayout->addWidget( m_widget );
}

MdiChild::MdiChild(CImage* im) : QWidget(), m_type(Type::Pic)
{
	QVBoxLayout* mdiChildLayout = new QVBoxLayout(this);
	mdiChildLayout->setSpacing(0);
	mdiChildLayout->setObjectName(QString::fromUtf8("mdiChildLayout"));
	mdiChildLayout->setContentsMargins(0, 0, 0, 0);

	m_widget = new PicViewer(im, this);

	mdiChildLayout->addWidget( m_widget );
}

void MdiChild::mouseMoveEvent(QMouseEvent* event)
{
	StatusBarManager::setText(QString("(MdiChild) pozycja kursora: " + QString::number(event->pos().x()) + " " + QString::number(event->pos().y())));
}

//void MdiChild::closeAllPic(int id)
//{
//	for ( QMdiSubWindow* window : AP::mainWin().ui.mdiArea->subWindowList() )
//	{
//		MdiChild* child = (MdiChild*)window;
//
//		if (child != nullptr)
//		{
//			if (child->hasType(MdiChild::Type::Pic))
//			{
//				PicViewer* pic = qobject_cast<PicViewer*>(child->widget());
//				if (pic->id() == id)
//				{
//					child->close();
//				}
//			}
//		}
//
//	}
//}


void MdiChild::fullScreen()
{
	if (isFullScreen())
	{
		setWindowFlags(Qt::Widget);
		if (m_itWasMaximized)
		{
			showMaximized();
		}
		else
		{
			showNormal();
		}
	}
	else
	{
		//m_itWasMaximized = isMaximized();
		m_itWasMaximized = parentWidget()->isMaximized();

		setWindowFlags(Qt::Window);
		showFullScreen();
	}
}

void MdiChild::keyPressEvent(QKeyEvent* e)
{
	// To dzia³a tylko w trybie pe³nego ekranu

	StatusBarManager::setText(QString::number(e->key()));
	if (e->key() == Qt::Key_F11)
	{
		fullScreen();
	}
}

void MdiChild::resizeEvent(QResizeEvent* e)
{
//	qDebug() << "QWidget::resizeEvent()\n\r";
	QWidget::resizeEvent(e);
//	qDebug() << "MdiChild::resizeEvent()\n\r";
}


QMdiSubWindow* MdiChild::create(Type t, QMdiArea* mdiArea, Show show )
{
//	qDebug() << "MdiChild::create()\n\r";
	MdiChild* child = new MdiChild(t);

//	qDebug() << "MdiChild - setMinimumSize\n\r";
	child->setMinimumSize(400, 300);
	//qDebug() << "MdiChild - resize\n\r";
	//child->resize(800, 600);

	QMdiSubWindow* subWindow = mdiArea->addSubWindow( child );

	if (t==Type::Pic) subWindow->setWindowTitle("Image viewer");
	else if (t == Type::GL) subWindow->setWindowTitle("Workspace viewer");

	switch (show) {
		case Minimized: 
			subWindow->showMinimized();
			break;
		case Maximized: 
			subWindow->showMaximized();
			break;
		case Normal:
		default: 
			subWindow->showNormal();
			break;
	}

	subWindow->update();

	return subWindow;
}


QMdiSubWindow* MdiChild::create(CImage* im, QMdiArea* mdiArea)
{
	MdiChild* child = new MdiChild(im);
	
	QMdiSubWindow* subWindow = mdiArea->addSubWindow(child);
	
	subWindow->setWindowTitle( "Image viewer [" + im->getLabel() + "]" );

	subWindow->resize(im->width(), im->height());

	subWindow->setMinimumSize( std::min(400, im->width()), std::min(300, im->height()));
	subWindow->show();

	return subWindow;
}

void MdiChild::updateView(bool buffered)
{
	if ( hasType(MdiChild::Type::GL)) 
	{
		if (buffered)
		{
			qobject_cast<GLViewer*>(m_widget)->update();
		}
		else
		{
			qobject_cast<GLViewer*>(m_widget)->repaint();
		}
	}
	else if (hasType(MdiChild::Type::Pic))
	{
		qobject_cast<PicViewer*>(m_widget)->repaint();
	}
}


