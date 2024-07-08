#include "propImage.h"
#include "UI.h"
#include "AP.h"

#include "Image.h"

#include "MainWindow.h"
#include "QScrollArea"


PropImage::PropImage(CImage *m, QWidget *parent) : PropWidget( parent )
{
	obj = m;
	ui.setupUi((QWidget*)this);

	wasLocked = ((CModel3D*)obj)->isLocked();
}

PropImage::~PropImage()
{
}

void PropImage::updateProperties()
{
	QMdiSubWindow* window = AP::mainWinPtr()->getPicViewerInstance(obj->id());
	
	ui.showImageWindow->setChecked(window != nullptr);

	ui.show3d->setChecked(((CModel3D*)obj)->getSelfVisibility());

}

PropWidget* PropImage::create(CImage* m, QWidget* parent)
{
	PropWidget* widget = new PropWidget(parent);
	QVBoxLayout* layout = new QVBoxLayout(widget);

	PropBaseObject* bo = new PropBaseObject((CModel3D*)m, widget);
	layout->addWidget(bo);

	PropImage* pI = new PropImage(m, widget);
	pI->getUI()->show3d->layout()->addWidget(new PropTransform(&m->getTransform(), pI));

	layout->addWidget(pI);

	pI->getUI()->show3d->resize(pI->getUI()->show3d->layout()->sizeHint());
	pI->getUI()->show3d->setMinimumSize(pI->getUI()->show3d->layout()->sizeHint());
	pI->getUI()->show3d->setMaximumSize(pI->getUI()->show3d->layout()->sizeHint());
	pI->getUI()->show3d->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	widget->resize(layout->sizeHint());
	widget->setMinimumSize(layout->sizeHint());
	widget->setMaximumSize(layout->sizeHint());
	widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	connect(bo, SIGNAL(signalChangedVisibility(bool)), pI, SLOT(slotChangeShow3d(bool)));
	connect(pI, SIGNAL(signalChangedShow3d(bool)), bo, SLOT(slotChangeVisibility(bool)));

	return widget;
}

QVector<PropWidget*> PropImage::create_and_get_subwidgets(CBaseObject* obj)
{
	return QVector<PropWidget*>({PropImage::create((CImage*)obj)});
}

#include "PicViewer.h"
#include "MdiChild.h"

void PropImage::scaleChanged(int i)
{
	switch (i)
	{
	case 0:
	default:
		obj->fitToWindow = false;
		break;
	case 1:
	case 2:
	case 3:
		obj->fitToWindow = true;
		break;
	}

	QMdiSubWindow* window = AP::mainWinPtr()->getPicViewerInstance(obj->id());
	if (window != nullptr)
	{
		((PicViewer*)((MdiChild*)window->widget())->m_widget)->reloadImage();
	}
}

void PropImage::showWindow(bool b)
{
	if (b)
	{
		AP::mainWinPtr()->activatePicViewerInstance(obj->id());
	}
	else
	{
		AP::mainWinPtr()->closePicViewers(obj->id());
	}
}

void PropImage::show3d(bool b)
{
	if (b)
	{
		((CModel3D*)obj)->setLocked(wasLocked);
		UI::DOCK::WORKSPACE::setItemLockedById(obj->id(), wasLocked);
	}
	else
	{
		wasLocked = ((CModel3D*)obj)->isLocked();
		((CModel3D*)obj)->setLocked(true);
		UI::DOCK::WORKSPACE::setItemLockedById(obj->id(), wasLocked);
	}
	
	emit signalChangedShow3d(b);

	obj->setSelfVisibility(b);
	UI::DOCK::WORKSPACE::setItemVisibleById(obj->id(), b);
	UI::updateAllViews();
}

void PropImage::slotChangeShow3d(bool v)
{
	ui.show3d->setChecked(v);
}
