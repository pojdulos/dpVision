#include "SamplePlugin.h"

#include "Workspace.h"
//#include "MainApplication.h"

#include "AnnotationPoint.h"
#include "AnnotationPlane.h"
#include "FileConnector.h"

#include "AP.h"
#include "UI.h"

#include "AppSettings.h"

#include <QPushButton>

/**
 * @brief Called when button on PluginPanel is pressed
 * @param name Button name
 */
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
	else if (name == "cutMesh")
	{
		cutMesh();
	}
}

/**
 * @brief Called when plugin is loaded
 */
void SamplePlugin::onLoad()
{
	UI::PLUGINPANEL::create( m_ID, "Working example" );

	UI::PLUGINPANEL::clear(m_ID);

	int nextPos = 0;
	
	UI::PLUGINPANEL::addButton(m_ID, "createBox", "Create box", nextPos++, 0);
	UI::PLUGINPANEL::addButton(m_ID, "loadObject", "Load object", nextPos++, 0);
	UI::PLUGINPANEL::addButton(m_ID, "cutMesh", "Cut mesh", nextPos++, 0);

	//QObject::connect(UI::PLUGINPANEL::addButton(m_ID, "create box", "createBox", nextPos++, 0), SIGNAL(clicked()), SLOT(createBox()));

	UI::PLUGINPANEL::setEnabled(m_ID, true);
}

/**
 * @brief loads model in path, if file in path do not exists, displays OpenFileName dialog and next loads choosen model
 * @param path path to model file
 */
void SamplePlugin::loadObject(const QString &path)
{
	QString fileName = path;
	
	if (!QFileInfo(fileName).exists()) {
		fileName = UI::FILECHOOSER::getOpenFileName(tr("Open File"), AppSettings::mainSettings()->value("recentFile").toString(), CFileConnector::getLoadExts());
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

/**
 * @brief creates simple mesh and shows it in workspace
 */
void SamplePlugin::createBox()
{
	std::shared_ptr<CMesh> mesh = std::make_shared<CMesh>();
	
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
	std::shared_ptr<CModel3D> obj = std::make_shared<CModel3D>();
	obj->addChild(obj, mesh);
	obj->importChildrenGeometry();

	// this command causes the object to be displayed
	// and visible in the scene tree
	AP::WORKSPACE::addModel(obj, true);
}


/**
 * @brief Calculates the signed distance of a point from a plane defined by a centroid and a normal vector.
 * @param point The point to evaluate.
 * @param centroid The centroid point of the plane.
 * @param normal The normal vector of the plane.
 * @return double The signed distance from the point to the plane.
 */
static double PlaneDistance(const CPoint3d& point, const CPoint3d& centroid, const CVector3d& normal) {
	// The plane equation is given by: (point - centroid) . normal = 0
	// The signed distance is calculated as:
	return ((point.x - centroid.x) * normal.x +
		(point.y - centroid.y) * normal.y +
		(point.z - centroid.z) * normal.z);
}

/**
 * @brief Divides a triangular mesh into two parts based on a plane.
 * @param mesh The input triangular mesh.
 * @param centroid The centroid point of the plane.
 * @param normal The normal vector of the plane.
 * @param aboveTriangles Output vector for faces (as vertex indices) above the plane.
 * @param belowTriangles Output vector for faces (as vertex indices) below the plane.
 */
static void DivideMesh(std::shared_ptr<CMesh> mesh, const CPoint3d& centroid, const CVector3d& normal,
	CMesh::Faces& aboveTriangles, CMesh::Faces& belowTriangles) {
	
	for (auto& face : mesh->faces()) {
		// Calculate the position of each vertex relative to the plane
		double d1 = PlaneDistance(mesh->vertices()[face.A()], centroid, normal);
		double d2 = PlaneDistance(mesh->vertices()[face.B()], centroid, normal);
		double d3 = PlaneDistance(mesh->vertices()[face.C()], centroid, normal);

		// Classify the triangle based on its vertices' positions relative to the plane
		if (d1 > 0 && d2 > 0 && d3 > 0) {
			aboveTriangles.push_back(face); // All vertices are above the plane
		}
		else if (d1 < 0 && d2 < 0 && d3 < 0) {
			belowTriangles.push_back(face); // All vertices are below the plane
		}
		else {
			// Triangle intersects the plane, further processing can be done if needed
			// For now, we will just ignore these triangles
		}
	}
}


void SamplePlugin::cutMesh() {
	QString fileName = UI::FILECHOOSER::getOpenFileName(
		tr("Open File"),
		AppSettings::mainSettings()->value("recentFile").toString(),
		CFileConnector::getLoadExts());

	std::shared_ptr<CModel3D> obj = nullptr;
	if (!fileName.isEmpty()) // if empty: you pressed Cancel 
	{
		if (QFileInfo(fileName).exists())
		{
			// load the model and if exists add it to workspace
			// using AP namespace is good choise because it automaticaly refreshes viewer
			obj = AP::WORKSPACE::loadModel(fileName);

			if (obj == nullptr)
			{
				UI::MESSAGEBOX::error("Something went wrong", "Can't read file");
				return;
			}
		}
		else // this should not have happened
		{
			UI::MESSAGEBOX::error("Something went wrong", "File doesn't exists");
			return;
		}

	
		// object can heave more than one child, but if you don't specify id of child
		// you always get first child in list or nullptr if none
		std::shared_ptr<CBaseObject> child = obj->getChild();

		CPoint3d centroid(0, 0, 0);
		CVector3d normal(0, 1, 0);


		std::shared_ptr<CAnnotationPlane> plane = std::make_shared<CAnnotationPlane>(centroid, normal);
		plane->setSize(50);

		// add plane to workspace
		// using AP namespace is good choise because it automaticaly refreshes viewer
		AP::OBJECT::addChild(obj, plane);

		if (child->hasType(CObject::MESH))
		{
			std::shared_ptr<CMesh> mesh = std::dynamic_pointer_cast<CMesh>(child);

			CMesh::Faces aboveTriangles, belowTriangles;

			DivideMesh(mesh, centroid, normal, aboveTriangles, belowTriangles);

			// create deep copy of original mesh
			std::shared_ptr<CMesh> mesh1 = std::dynamic_pointer_cast<CMesh>(mesh->getCopy());
			
			// replace vector of faces with result of divide
			mesh1->faces() = aboveTriangles;
			
			// Removes vertices not assigned to any face and corects indices
			mesh1->removeUnusedVertices();
			
			// currently you must set CModel3D as a parent of any other objects
			// it is good because CModel3D is the "transformation object",
			// so it let you to rotate, translate and scale
			std::shared_ptr<CModel3D> obj1 = std::make_shared<CModel3D>();

			// set obj1 as parent of your mesh
			obj1->addChild(obj1, mesh1);

			// and optionaly correct parents boundingbox
			obj1->importChildrenGeometry();

			// you can rename every object as you want
			obj1->setLabel("upper part");
			
			// add to workspace for display it in viewer and in workspace tree dialog
			AP::WORKSPACE::addObject(obj1);

			// create deep copy of original mesh
			std::shared_ptr<CMesh> mesh2 = std::dynamic_pointer_cast<CMesh>(mesh->getCopy());

			// replace vector of faces with result of divide
			mesh2->faces() = belowTriangles;

			// Removes vertices not assigned to any face and corects indices
			mesh2->removeUnusedVertices();

			std::shared_ptr<CModel3D> obj2 = std::make_shared<CModel3D>();

			obj2->addChild(obj2, mesh2);
			obj2->importChildrenGeometry();
			obj2->setLabel("bottom part");

			AP::WORKSPACE::addObject(obj2);
		}
		else
		{
			UI::MESSAGEBOX::error("Something went wrong", "Object is not mesh");
			return;
		}

		// refresh viewers, probably it is not needed
		UI::updateAllViews();
	}

}