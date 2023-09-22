#include "propBaseObject.h"
#include "BaseObject.h"
#include "UI.h"

//PropBaseObject::PropBaseObject(CBaseObject *o, QWidget *parent, bool vis3) : PropWidget(parent)
PropBaseObject::PropBaseObject(CBaseObject* o, QWidget* parent) : PropWidget(parent)
{
	obj = o;
	ui.setupUi(this);

	//ui.visible->setTristate(vis3);
}

PropBaseObject::~PropBaseObject()
{
}

void PropBaseObject::updateProperties()
{
	if (NULL != obj)
	{
		// BaseObject
		ui.baseObject->setTitle("BaseObject - Id: " + QString::number(obj->id()));

		ui.labelEdit->blockSignals(true);
		ui.labelEdit->setText( obj->getLabel() );
		ui.labelEdit->blockSignals(false);

		ui.descrEdit->blockSignals(true);
		ui.descrEdit->setText(obj->getDescr());
		ui.descrEdit->blockSignals(false);

		ui.selected->blockSignals(true);
		ui.selected->setChecked(obj->isSelected());
		ui.selected->blockSignals(false);

		ui.visible->blockSignals(true);
		ui.visible->setChecked(obj->getSelfVisibility());
		ui.visible->blockSignals(false);

		ui.visibleKids->blockSignals(true);
		ui.visibleKids->setChecked(obj->getKidsVisibility());
		ui.visibleKids->blockSignals(false);
	}
}

#include "AP.h"
#include "Workspace.h"

void PropBaseObject::changedLabel(QString s)
{
	((CBaseObject*)obj)->setLabel(s.toStdWString());
	UI::updateAllViews();
	UI::DOCK::WORKSPACE::setItemLabelById(obj->id(), s.toStdWString());
}

void PropBaseObject::onDescrChanged()
{
	obj->setDescr(ui.descrEdit->toPlainText());
}

void PropBaseObject::changedSelection(int checkState)
{
	bool b = (checkState == Qt::Checked);
	if (obj->hasType(CBaseObject::MODEL))
	{
		AP::getWorkspace()->changeSelection(obj->id(), b );
	}
	else
	{
		obj->setSelected( b );
	}
	UI::DOCK::WORKSPACE::setItemCheckedById(obj->id(), b );
	UI::updateAllViews();
}

void PropBaseObject::changedVisibility(int b)
{
	bool vis = (b == Qt::CheckState::Checked);

	obj->setSelfVisibility(vis);

	UI::updateAllViews();
	UI::DOCK::WORKSPACE::setItemVisibleById(obj->id(), vis);

	//emit signalChangedVisibility(vis);
}

void PropBaseObject::changedKidsVisibility(bool b)
{
	obj->setKidsVisibility(b);

	UI::updateAllViews();
	UI::DOCK::WORKSPACE::setItemKidsVisibleById(obj->id(), b);

	//emit signalChangedVisibility(vis);
}


void PropBaseObject::slotChangeVisibility(bool v)
{
	ui.visible->blockSignals(true);
	ui.visible->setChecked(v);
	ui.visible->blockSignals(false);
}
