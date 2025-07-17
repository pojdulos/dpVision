#include "DockWidgetProperties.h"

#include "UI.h"

#include "DockWidgetWorkspace.h"
#include "Annotation.h"

#include "propBaseObject.h"
#include "propDataChild.h"
#include "propMaterial.h"
#include "propPointCloud.h"
#include "propMesh.h"
#include "propAnnotation.h"
#include "propAnnotationPlane.h"
#include "propAnnotationSphere.h"
#include "propAnnotationPoint.h"
#include "propAnnotationTriple.h"
#include "propAnnotationCamera.h"

#include "propAnnotationSetOfVertices.h"
#include "AnnotationSetOfVertices.h"

#include "propAnnotationSetOfFaces.h"
#include "AnnotationSetOfFaces.h"

#include "propHistogram.h"
#include "propViewer.h"
#include "propModel.h"
#include "propVolTK.h"
#include "propVolumetric.h"
#include "propImage.h"
#include "propMovement.h"
#include "propCustomWidget.h"


#include <QLayout>
#include <QScrollArea>

#include "AP.h"

#include "Model3D.h"

#include "MainWindow.h"
#include "Workspace.h"

#include "AppSettings.h"

DockWidgetProperties::DockWidgetProperties(QWidget *parent)	: QDockWidget(parent)
{
	ui.setupUi(this);

	setObjectName("DockWidgetProperties");

	ui.tree->setHeaderHidden(true);
	ui.tree->setIndentation(0);

	connect(AppSettingsNotifier::instance(), SIGNAL(darkModeChanged(bool)), this, SLOT(onDarkModeChanged(bool)));
}

DockWidgetProperties::~DockWidgetProperties() {}

//void DockWidgetProperties::paintEvent(QPaintEvent * event)
//{
//	QDockWidget::paintEvent(event);
//}

void DockWidgetProperties::selectionChanged( int id )
{
	ui.tree->clear();

	QVector<PropWidget*> submodels = QVector<PropWidget*>(0);

	if (id == NO_CURRENT_MODEL)
	{
		GLViewer* view = AP::mainWin().currentViewer();
		submodels = PropViewer::create_and_get_subwidgets(view);
	}
	else
	{
		std::shared_ptr<CBaseObject> currentObjectPtr = AP::WORKSPACE::findId(id);
		
		CBaseObject* currentObject = currentObjectPtr.get(); //TYMCZASOWO
		
		if (currentObject == nullptr)
		{
			GLViewer* view = AP::mainWin().currentViewer();
			submodels = PropViewer::create_and_get_subwidgets(view);
		}
		else
		{
			switch (currentObject->category())
			{
				case CBaseObject::ANNOTATION:
					switch (currentObject->type())
					{
						case CAnnotation::HISTOGRAM:
							submodels = PropHistogram::create_and_get_subwidgets(currentObject);
							break;
						case CAnnotation::POINT:
							submodels = PropAnnotationPoint::create_and_get_subwidgets(currentObject);
							break;
						case CAnnotation::PLANE:
							submodels = PropAnnotationPlane::create_and_get_subwidgets(currentObject);
							break;
						case CAnnotation::SPHERE:
							submodels = PropAnnotationSphere::create_and_get_subwidgets(currentObject);
							break;
						case CAnnotation::TRIPLE:
							submodels = PropAnnotationTriple::create_and_get_subwidgets(currentObject);
							break;
						case CAnnotation::CAMERA:
							submodels = PropAnnotationCamera::create_and_get_subwidgets(currentObject);
							break;
						case CAnnotation::SETOFVERTICES:
							submodels = PropAnnotationSetOfVertices::create_and_get_subwidgets(currentObject);
							break;
						case CAnnotation::SETOFFACES:
							submodels = PropAnnotationSetOfFaces::create_and_get_subwidgets(currentObject);
							break;
						default:
							submodels = PropAnnotation::create_and_get_subwidgets(currentObject);
							break;
					}
					break;
				case CBaseObject::OBJECT:
					switch (currentObject->type())
					{
						case CObject::MODEL:
							submodels = PropModel::create_and_get_subwidgets(currentObject);
							break;
						case CObject::IMAGE:
							submodels = PropImage::create_and_get_subwidgets(currentObject);
							break;
						case CObject::VOLUMETRIC_NEW:
							submodels = PropVolumetric::create_and_get_subwidgets(currentObject);
							break;
						case CObject::CLOUD:
						case CObject::ORDEREDCLOUD:
							submodels = PropPointCloud::create_and_get_subwidgets(currentObject);
							break;
						case CObject::MESH:
							submodels = PropMesh::create_and_get_subwidgets(currentObject);
							break;
						case CObject::MOVEMENT:
							submodels = PropMovement::create_and_get_subwidgets(currentObject);
							break;

						default:
							if (currentObject->has_prop_widget()) {
								submodels = PropCustomWidget::create_and_get_subwidgets(currentObject);;
							}
							else {
								submodels = PropBaseObject::create_and_get_subwidgets(currentObject);
							}
							break;
					}
					break;
				default:
					submodels = PropWidget::create_and_get_subwidgets(currentObject);
					break;
			}
		}
	}
	addExpandableItems(submodels);
	
	for (auto& kid : submodels)
	{
		kid->updateProperties();
	}
	//updateProperties();
}

void DockWidgetProperties::updateProperties()
{
	QRegularExpression rx("prop");
	QWidgetList children = ui.tree->findChildren<QWidget*>(rx);// , Qt::FindDirectChildrenOnly);

	for (auto& kid : children)
	{
		((PropWidget*)kid)->updateProperties();
	}
}

#include <QTreeWidget>

void DockWidgetProperties::addExpandableItems(QVector<PropWidget*> submodels)
{
	QVectorIterator<PropWidget*> i(submodels);
	while (i.hasNext())
	{
		PropWidget* ww = (PropWidget*)i.next();
		addExpandableItem(ww->treeItemLabel, ww);
	}
}

void DockWidgetProperties::addExpandableItem(QString title, PropWidget* contentWidget)
{
	QTreeWidgetItem* item = new QTreeWidgetItem(ui.tree);
	item->setText(0, title);

	QTreeWidgetItem* childItem = new QTreeWidgetItem();
	item->addChild(childItem);
	ui.tree->setItemWidget(childItem, 0, contentWidget);

	item->setExpanded(true);
}

void DockWidgetProperties::onWorkspaceObjectActivated(int i)
{
	raise();
	selectionChanged(i);
	update();
}

void DockWidgetProperties::onWorkspaceObjectActivated(CBaseObject* obj)
{
	raise();

	if (obj != nullptr)
		selectionChanged(obj->id());
	else
		selectionChanged(NO_CURRENT_MODEL);

	update();
}

void DockWidgetProperties::onWorkspaceObjectRemoved(int) {
	selectionChanged(CWorkspace::instance()->_getCurrentModelId());
	update();
}

void DockWidgetProperties::onDarkModeChanged(bool dark)
{
	QPalette palette = ui.tree->palette();
	QColor dialogBackgroundColor = QApplication::palette().color(QPalette::Window);

	// Ustawienie koloru t³a w palecie
	palette.setColor(QPalette::Base, dialogBackgroundColor);
	ui.tree->setPalette(palette);
}