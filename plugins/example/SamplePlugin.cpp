#include "SamplePlugin.h"

#include "Workspace.h"
#include "MainApplication.h"

#include "AnnotationPoint.h"
#include "FileConnector.h"

#include "AP.h"

#include <QPushButton>

void SamplePlugin::onButton(const QString &name)
{
	qInfo() << "onButton  " << name << Qt::endl;
	if (name == "createBox")
	{
		createBox();
	}
	else if (name == "loadObject")
	{
		loadObject();
	}
}

void SamplePlugin::onLoad()
{
	UI::PLUGINPANEL::create( m_ID, "Working example" );

	UI::PLUGINPANEL::clear(m_ID);

	int nextPos = 0;
	
	UI::PLUGINPANEL::addButton(m_ID, "createBox", "Create box", nextPos++, 0);
	UI::PLUGINPANEL::addButton(m_ID, "loadObject", "Load object", nextPos++, 0);

	//QObject::connect(UI::PLUGINPANEL::addButton(m_ID, "create box", "createBox", nextPos++, 0), SIGNAL(clicked()), SLOT(createBox()));

	UI::PLUGINPANEL::setEnabled(m_ID, true);
}


void SamplePlugin::loadObject(const QString &path)
{
	QString fileName = path;
	
	if (!QFileInfo(fileName).exists()) {
		fileName = UI::FILECHOOSER::getOpenFileName(tr("Open File"), AP::mainApp().settings->value("recentFile").toString(), CFileConnector::getLoadExts());
	}
	
	if (!fileName.isEmpty()) // if empty: you pressed Cancel 
	{
		if (QFileInfo(fileName).exists())
		{
			AP::WORKSPACE::loadModel(fileName);
		}
		else // this should not have happened
		{
			UI::MESSAGEBOX::error("Something went wrong", "File doesn't exists");
		}
	}
}

void SamplePlugin::createBox()
{
	CMesh* mesh = new CMesh;
	
	// addvertex and addFace methods have several
	// alternative sets of parameters.
	// Take a look at their declarations
	// to find out all the possibilities for their use.
	
	// this is simple, specify x, y, z for each vertex.
	mesh->addVertex(-5.0, -5.0, -5.0);
	mesh->addVertex( 5.0, -5.0, -5.0);
	mesh->addVertex( 5.0, -5.0,  5.0);
	mesh->addVertex(-5.0, -5.0,  5.0);
	mesh->addVertex(-5.0,  5.0, -5.0);
	mesh->addVertex( 5.0,  5.0, -5.0);
	mesh->addVertex( 5.0,  5.0,  5.0);
	mesh->addVertex(-5.0,  5.0,  5.0);
	
	// You can define coloured faces or omit the colour
	// and then the wall will be the default colour.
	// However, you must be consistent and should not
	// mix coloured and non-coloured faces.
	// Here: addFace( v0, v1, v2, R, G, B A)
	mesh->addFace(0, 1, 2, 1.0f, 0.0f, 0.0f, 1.0f);
	mesh->addFace(0, 2, 3, 1.0f, 1.0f, 0.0f, 0.5f);
	mesh->addFace(7, 6, 5, 0.0f, 1.0f, 0.0f, 1.0f);
	mesh->addFace(7, 5, 4, 0.0f, 0.0f, 1.0f, 1.0f);
	
	// for practice, add the missing walls if you wish

	// creating CModel3D will add a transformation
	// matrix to your object,
	// so you can rotate and move it
	CModel3D* obj = new CModel3D();
	obj->addChild(mesh);
	obj->importChildrenGeometry();

	// this command causes the object to be displayed
	// and visible in the scene tree
	AP::WORKSPACE::addModel(obj, true);
}


