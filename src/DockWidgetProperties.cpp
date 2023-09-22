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
#include "propImage.h"


#include <QLayout>
#include <QScrollArea>

#include "AP.h"

#include "Model3D.h"

#include "MainWindow.h"
#include "Workspace.h"

DockWidgetProperties::DockWidgetProperties(QWidget *parent)	: QDockWidget(parent)
{
	ui.setupUi(this);

	m_scroll = new QScrollArea();

	QSizePolicy policy = m_scroll->sizePolicy();

	policy.setVerticalStretch(1);
	policy.setHorizontalStretch(1);

	m_layout = new QVBoxLayout(m_scroll);

	this->setWidget(m_scroll);

	m_widget = new PropWidget;
	//GLViewer* view = AP::mainWin().currentViewer();
	//m_widget = PropViewer::create(view, this);
	
	addWidgetToScrollArea(m_widget);
}

DockWidgetProperties::~DockWidgetProperties()
{
	delete m_layout;
}

void DockWidgetProperties::paintEvent(QPaintEvent * event)
{
	QDockWidget::paintEvent(event);
}

void DockWidgetProperties::selectionChanged( int id )
{
	delete m_widget;

	if (id == NO_CURRENT_MODEL)
	{
		//m_widget = new PropViewer(this);
		GLViewer* view = AP::mainWin().currentViewer();

		m_widget = PropViewer::create(view, this);
	}
	else
	{
		CBaseObject* currentObject = AP::WORKSPACE::findId(id);
		
		if (currentObject == nullptr)
		{
			m_widget = new PropWidget(this);
		}
		else
		{
			switch (currentObject->category())
			{
				case CBaseObject::ANNOTATION:
					switch (currentObject->type())
					{
						case CAnnotation::HISTOGRAM:
							m_widget = PropHistogram::create((CHistogram*)currentObject, this);
							break;
						case CAnnotation::POINT:
							m_widget = PropAnnotationPoint::create((CAnnotationPoint*)currentObject, this);
							break;
						case CAnnotation::PLANE:
							m_widget = PropAnnotationPlane::create((CAnnotationPlane*)currentObject, this);
							break;
						case CAnnotation::SPHERE:
							m_widget = PropAnnotationSphere::create((CAnnotationSphere*)currentObject, this);
							break;
						case CAnnotation::TRIPLE:
							m_widget = PropAnnotationTriple::create((CAnnotationTriple*)currentObject, this);
							break;
						case CAnnotation::CAMERA:
							m_widget = PropAnnotationCamera::create((CAnnotationCamera*)currentObject, this);
							break;
						case CAnnotation::SETOFVERTICES:
							m_widget = PropAnnotationSetOfVertices::create((CAnnotationSetOfVertices*)currentObject, this);
							break;
						case CAnnotation::SETOFFACES:
							m_widget = PropAnnotationSetOfFaces::create((CAnnotationSetOfFaces*)currentObject, this);
							break;
						default:
							m_widget = PropAnnotation::create((CAnnotation*)currentObject, this);
							break;
					}
					break;
				case CBaseObject::OBJECT:
					switch (currentObject->type())
					{
						case CObject::MODEL:
							m_widget = PropModel::create((CModel3D*)currentObject, this);
							break;
						case CObject::IMAGE:
							m_widget = PropImage::create((CImage*)currentObject, this);
							break;
						case CObject::VOLTK:
							m_widget = PropVolTK::create((CVolTK*)currentObject, this);
						case CObject::CLOUD:
						case CObject::ORDEREDCLOUD:
							m_widget = PropPointCloud::create((CPointCloud*)currentObject, this);
							break;
						case CObject::MESH:
							m_widget = PropMesh::create((CMesh*)currentObject, this);
							break;
							
						default:
							//m_widget = new PropBaseObject(an,this);
							m_widget = PropDataChild::create(currentObject, this);
							break;
					}
					break;
				default:
					m_widget = new PropWidget(this);
					break;
			}
		}
	}
	addWidgetToScrollArea(m_widget);
	m_widget->updateProperties();
}

void DockWidgetProperties::updateProperties()
{
	m_widget->updateProperties();
}


void DockWidgetProperties::onCurrentObjectChanged(int i)
{
	raise();
	selectionChanged(i);
	update();
}

void DockWidgetProperties::onCurrentObjectChanged(CBaseObject* obj)
{
	raise();

	if (obj != nullptr)
		selectionChanged(obj->id());
	else
		selectionChanged(NO_CURRENT_MODEL);

	update();
}

void DockWidgetProperties::addWidgetToScrollArea(QWidget * widget)
{
	widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QSize AdjustSize = widget->size();

	widget->setMinimumSize(AdjustSize);  // Advantage: everytime you change the size of pJob,
													// because of adding new functionyllity -eg widgets to it..
													// scrollArea will adapt automatically (-:  

	m_scroll->setWidgetResizable(true);
	m_scroll->setWidget(widget);
}


