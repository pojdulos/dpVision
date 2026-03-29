#include "propImage.h"
#include "../core/Workspace.h"

#include "Image.h"
#include "ImageViewerHost.h"
#include "ImageViewerState.h"

#include "QScrollArea"


PropImage::PropImage(CImage *m, QWidget *parent) : PropWidget( parent )
{
	obj = m;
	treeItemLabel = "Image properties";

	ui.setupUi((QWidget*)this);

	wasLocked = ((CModel3D*)obj)->isLocked();
}

PropImage::~PropImage()
{
}

void PropImage::updateProperties()
{
	ui.showImageWindow->setChecked(obj->getShowViewer());

	ui.show3d->setChecked(((CModel3D*)obj)->getSelfVisibility());

}

PropWidget* PropImage::create(CImage* m, QWidget* parent)
{
	PropWidget* widget = new PropWidget(parent);
	QVBoxLayout* layout = new QVBoxLayout(widget);

	PropBaseObject* bo = new PropBaseObject((CModel3D*)m, widget);
	layout->addWidget(bo);

	PropImage* pI = new PropImage(m, widget);
	pI->getUI()->show3d->layout()->addWidget(new PropTransform(&m->getTransform(), pI, false, m));

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
		ImageViewerHost::setFitToWindow(obj->id(), false);
		break;
	case 1:
	case 2:
	case 3:
		ImageViewerHost::setFitToWindow(obj->id(), true);
		break;
	}
}

void PropImage::showWindow(bool b)
{
	obj->setShowViewer(b);
	CWorkspace::instance()->notifyObjectStateChanged(obj->id());
}

void PropImage::show3d(bool b)
{
	if (b)
	{
		((CModel3D*)obj)->setLocked(wasLocked);
	}
	else
	{
		wasLocked = ((CModel3D*)obj)->isLocked();
		((CModel3D*)obj)->setLocked(true);
	}
	
	emit signalChangedShow3d(b);

	obj->setSelfVisibility(b);
	CWorkspace::instance()->notifyObjectStateChanged(obj->id());
}

void PropImage::slotChangeShow3d(bool v)
{
	ui.show3d->setChecked(v);
}
