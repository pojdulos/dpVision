#include "propBaseObject.h"
#include "BaseObject.h"
#include "UI.h"

bool PropBaseObject::group_visible = true;


//PropBaseObject::PropBaseObject(CBaseObject *o, QWidget *parent, bool vis3) : PropWidget(parent)
PropBaseObject::PropBaseObject(CBaseObject* o, QWidget* parent) : PropWidget(parent)
{
	obj = o;

	treeItemLabel = QString("Base Object - Id: %1").arg(obj->id());

	// Usuń istniejący layout
	//QLayout* oldLayout = layout();
	//if (oldLayout) {
	//	QLayoutItem* item;
	//	while ((item = oldLayout->takeAt(0)) != nullptr) {
	//		delete item->widget();
	//		delete item;
	//	}
	//	delete oldLayout;
	//}

	ui.setupUi(this);

	keywords_layout = new FlowLayout(4, 0, 0);
	ui.keywordsAreaContents->setLayout(keywords_layout);

	//this->setStyleSheet("background-color:#00ffff");

	//ui.baseObject->setStyleSheet("QGroupBox { border: 2px solid #666666; padding: 10px; border-radius: 5px; }"
	//	"QGroupBox::title { subcontrol-position: top center; padding: 0 10px; }"
	//	"QCheckBox { color: #ffffff; }"
	//	"QGroupBox:checked { background-color: #444444; }");
}

PropBaseObject::~PropBaseObject()
{
}


void PropBaseObject::updateProperties()
{
	if (NULL != obj)
	{
		// BaseObject
		//ui.baseObject->setTitle("BaseObject - Id: " + QString::number(obj->id()));

		ui.labelEdit->blockSignals(true);
		ui.labelEdit->setText( obj->getLabel() );
		ui.labelEdit->blockSignals(false);

		ui.descrEdit->blockSignals(true);
		ui.descrEdit->setText(obj->getDescr());
		ui.descrEdit->blockSignals(false);

		ui.selected->blockSignals(true);
		ui.selected->setChecked(obj->isChecked());
		ui.selected->blockSignals(false);

		ui.visible->blockSignals(true);
		ui.visible->setChecked(obj->getSelfVisibility());
		ui.visible->blockSignals(false);

		ui.visibleKids->blockSignals(true);
		ui.visibleKids->setChecked(obj->getKidsVisibility());
		ui.visibleKids->blockSignals(false);

		ui.keywordsAreaContents->update();
		updateKeywords();

		//ui.baseObject->setChecked(PropBaseObject::group_visible);

		//adjustGroupBoxHeight(ui.baseObject, PropBaseObject::group_visible);
	}
}


void PropBaseObject::updateKeywords()
{
	ui.keywordsAreaContents->blockSignals(true);

	QSet<QString> tmpset;

	QList<QPushButton*> pList = this->ui.keywordsAreaContents->findChildren<QPushButton*>();// , Qt::FindDirectChildrenOnly);
	QListIterator<QPushButton*> i(pList);
	while (i.hasNext()) {
		QPushButton* bt = i.next();
		if (obj->keywords().find(bt->text()) == obj->keywords().end())
		{
			delete bt;
		}
		else
		{
			tmpset.insert(bt->text());
		}
	}

	for (const QString& k : obj->keywords()) {
		if (tmpset.find(k) == tmpset.end())	addKeywordToWidget(k);
	}

	ui.keywordsAreaContents->blockSignals(false);

	ui.keywordsAreaContents->update();
}

#include "AP.h"
#include "Workspace.h"

void PropBaseObject::changedLabel(QString s)
{
	((CBaseObject*)obj)->setLabel(s.toStdWString());
	UI::updateAllViews();
	UI::DOCK::WORKSPACE::setItemLabelById(obj->id(), s.toStdWString());
}

QVector<PropWidget*> PropBaseObject::create_and_get_subwidgets(CBaseObject* obj)
{
	return QVector<PropWidget*>({ new PropBaseObject(obj) });
}

void PropBaseObject::onDescrChanged()
{
	obj->setDescr(ui.descrEdit->toPlainText());
}

void PropBaseObject::addKeywordToWidget(QString keyword)
{
	QPushButton* pushButton = new QPushButton(ui.keywordsAreaContents);
	pushButton->setObjectName(QString::fromUtf8("pushButton_") + keyword);
	pushButton->setText(keyword);

	QFont font;
	font.setPointSize(7);
	pushButton->setFont(font);

	pushButton->setIcon(QIcon((QString::fromUtf8(":/icons/Erase.ico"))));

	pushButton->setFixedWidth(QFontMetrics(pushButton->font()).width(keyword) + pushButton->iconSize().width() + 8);
	pushButton->setFixedHeight(pushButton->iconSize().height() + 4);

	QObject::connect(pushButton, SIGNAL(clicked()), this, SLOT(deleteKeyword()));

	this->keywords_layout->addWidget(pushButton);
}



void PropBaseObject::onAddKeywordButtonClick()
{
	/*
	* 1. odczytaj okienko
	* 2. sprawdz czy keyword istnieje w obiekcie
	* 2.a. jesli nie dodaj keyword do obiektu
	* 2.b. oraz utw�rz przycisk na widgecie
	*/
	QString keyword = ui.addKeywordCombo->currentText();
	if (!obj->hasKeyword(keyword)) {
		obj->addKeyword(keyword);
		//addKeywordToWidget(keyword);
	}
	updateKeywords();
	ui.addKeywordCombo->setCurrentText("");
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
		obj->setChecked( b );
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

void PropBaseObject::deleteKeyword()
{
	QPushButton *bt = (QPushButton*) QObject::sender();
	qDebug() << "this should delete keyword: " << bt->text() << Qt::endl;

	obj->delKeyword(bt->text());
	updateKeywords();
}
