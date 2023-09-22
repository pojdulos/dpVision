
#include "MainApplication.h"
#include "AnnotationPoint.h"

//#include "AP.h"
#include "GroupObject.h"
//#include "Plugin.h"


#include "GLViewer.h"
#include <QtOpenGL>
#include "GL/Glu.h"

#include "AnnotationPyramid.h"
#include "AnnotationPath.h"
#include "AnnotationPoints.h"

bool GLViewer::screen2obj(double xx, double yy, CModel3D *obj, CPoint3d &in, CPoint3d &out, CVector3d &dir)
{
	if (convertCoords(xx, yy, /*ref*/in, /*ref*/out))
	{
		in = m_transform.w2l(in + cam.m_pos);
		out = m_transform.w2l(out + cam.m_pos);

		in = obj->getTransform().w2l(in);
		out = obj->getTransform().w2l(out);
		
		dir = CVector3d(in, out);
		dir.normalize();

		return true;
	}
	return false;
}


bool GLViewer::convertWorldToWin(CPoint3d workspaceCoords, CPoint3d& winCoords)
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	//convert workspace to real window-word coords
	CPoint3d wC = m_transform.l2w(workspaceCoords) - cam.m_pos;

	double winX, winY, winZ;
	//int res1 = gluProject(worldCoords.X(), worldCoords.Y(), worldCoords.Z(), modelview, projection, viewport, &winX, &winY, &winZ);
	int res1 = gluProject(wC.X(), wC.Y(), wC.Z(), modelview, projection, viewport, &winX, &winY, &winZ);

	winCoords = CPoint3d(winX, (double)viewport[3] - winY, winZ);

	return (GL_TRUE == res1);
}


CPoint3d GLViewer::world2win(CPoint3d world)
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	double winX, winY, winZ;
	int res1 = gluProject(world.X(), world.Y(), world.Z(), modelview, projection, viewport, &winX, &winY, &winZ);

	return CPoint3d(winX, (double)viewport[3] - winY, winZ);
}



bool GLViewer::convertWinToWorld(CPoint3d winCoords, CPoint3d& workspaceCoords)
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	double worldX, worldY, worldZ;
	int res1 = gluUnProject(winCoords.X(), (double)viewport[3] - winCoords.Y(), winCoords.Z(), modelview, projection, viewport, &worldX, &worldY, &worldZ);


	workspaceCoords = m_transform.w2l(CPoint3d(worldX, worldY, worldZ) + cam.m_pos);


	return (GL_TRUE == res1);
}


bool GLViewer::convertCoords(GLdouble winX, GLdouble winY, CPoint3d& pkt0, CPoint3d& pkt1)
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	double worldX, worldY, worldZ;
	int res1 = gluUnProject(winX, (double)viewport[3] - winY, 0.0, modelview, projection, viewport, &worldX, &worldY, &worldZ);

	pkt0.Set(worldX, worldY, worldZ);

	int res2 = gluUnProject(winX, (double)viewport[3] - winY, 1.0, modelview, projection, viewport, &worldX, &worldY, &worldZ);

	pkt1.Set(worldX, worldY, worldZ);

	return ((GL_TRUE == res1) && (GL_TRUE == res2));
}


bool GLViewer::inTriangle(CPoint3d pt, CPoint3d pA, CPoint3d pB, CPoint3d pC)
{
	CVector3d u = CVector3d(pA, pB);
	CVector3d v = CVector3d(pA, pC);

	// --------------------------------------------------------------
	// sprawdzam czy pIntersectionPoint lezy w trójk¹cie tFace
	// --------------------------------------------------------------
	// to jest wyznaczane ze wspó³rzêdnych barycentrycznych

	double uu = u.dotProduct(u);
	double uv = u.dotProduct(v);
	double vv = v.dotProduct(v);

	CVector3d w = CVector3d(pA, pt);

	double wu = w.dotProduct(u);
	double wv = w.dotProduct(v);

	double D = uv * uv - uu * vv;

	// get and test parametric coords
	double s = (uv * wv - vv * wu) / D;

	if (s < 0.0 || s > 1.0)         // pIntersectionPoint le¿y poza trójk¹tem
		return false;

	double t = (uv * wu - uu * wv) / D;

	if (t < 0.0 || (s + t) > 1.0)  // pIntersectionPoint le¿y poza trójk¹tem
		return false;

	return true; // pIntersectionPoint le¿y na trójk¹cie tFace
}


bool GLViewer::inRectangle(CPoint3d pt, CPoint3d A, CPoint3d B, CPoint3d C, CPoint3d D)
{
	return inTriangle(pt, A, B, C) || inTriangle(pt, C, D, A);
}

#include "Plane.h"

void GLViewer::findIntersection(CPoint3d origin, CVector3d direction, CPoint3d A, CPoint3d B, CPoint3d C, CPoint3d D, std::vector<std::pair<double, CPoint3d>>& intersections)
{
	CPoint3d pIntersectionPoint;
	double pDistance;

	CPlane plane(A, B, C);
	//plane.rayIntersection(origin, direction, pDistance, pIntersectionPoint);
	if (plane.rayIntersect3D(direction, origin, pIntersectionPoint, pDistance))
	{
		if (inRectangle(pIntersectionPoint, A, B, C, D))
			intersections.push_back(std::pair<double, CPoint3d>(pDistance, pIntersectionPoint));
	}
#ifdef _DEBUG
	else
		assert(0);
#endif
}

bool GLViewer::getEntryAndExit(CPoint3d pMin, CPoint3d pMax, CPoint3d origin, CVector3d direction, CPoint3d& entry, CPoint3d& exit)
{
	CPoint3d p000(pMin.X(), pMin.Y(), pMin.Z());
	CPoint3d p010(pMin.X(), pMax.Y(), pMin.Z());
	CPoint3d p110(pMax.X(), pMax.Y(), pMin.Z());
	CPoint3d p100(pMax.X(), pMin.Y(), pMin.Z());
	CPoint3d p101(pMax.X(), pMin.Y(), pMax.Z());
	CPoint3d p111(pMax.X(), pMax.Y(), pMax.Z());
	CPoint3d p011(pMin.X(), pMax.Y(), pMax.Z());
	CPoint3d p001(pMin.X(), pMin.Y(), pMax.Z());

	std::vector<std::pair<double, CPoint3d>> intersections;

	findIntersection(origin, direction, p000, p001, p011, p010, intersections);	 //X=0
	findIntersection(origin, direction, p100, p101, p001, p000, intersections);	 //Y=0
	findIntersection(origin, direction, p000, p010, p110, p100, intersections);	 //Z=0
	findIntersection(origin, direction, p110, p111, p101, p100, intersections);	 //X=1
	findIntersection(origin, direction, p010, p011, p111, p110, intersections);	 //Y=1
	findIntersection(origin, direction, p101, p111, p011, p001, intersections);	 //Z=1

	int s = intersections.size();
	if (s > 1)
	{
		std::make_heap(intersections.begin(), intersections.end());
		std::sort_heap(intersections.begin(), intersections.end());

		entry = intersections.front().second;
		exit = intersections.back().second;

		return true;
	}
#ifdef _DEBUG
	else
		assert(0);
#endif
	return false;
}


CAnnotationPyramid* GLViewer::getPyramid(double xx, double yy, CPoint3d entry, CPoint3d exit, CModel3D* obj, double& r)
{
	double xMin = xx - 1.0;
	double yMin = yy - 1.0;
	double xMax = xx + 1.0;
	double yMax = yy + 1.0;

	CPoint3d win00, win01, win11, win10;
	CPoint3d in00, in01, in11, in10;
	CPoint3d out00, out01, out11, out10;
	CVector3d dir00, dir01, dir10, dir11;

	if (screen2obj(xMin, yMin, obj, win00, out00, dir00))
	{
		getEntryAndExit(obj->getMin(), obj->getMax(), win00, dir00, /*ref*/ in00, /*ref*/ out00);
	}

	if (screen2obj(xMin, yMax, obj, win01, out01, dir01))
	{
		getEntryAndExit(obj->getMin(), obj->getMax(), win01, dir01, /*ref*/ in01, /*ref*/ out01);
	}

	if (screen2obj(xMax, yMin, obj, win10, out10, dir10))
	{
		getEntryAndExit(obj->getMin(), obj->getMax(), win10, dir10, /*ref*/ in10, /*ref*/ out10);
	}

	if (screen2obj(xMax, yMax, obj, win11, out11, dir11))
	{
		getEntryAndExit(obj->getMin(), obj->getMax(), win11, dir11, /*ref*/ in11, /*ref*/ out11);
	}

	CAnnotationPyramid* apy = new CAnnotationPyramid();
	apy->setTop(in00, in01, in11, in10);
	apy->setBottom(out00, out01, out11, out10);
	apy->setColor(CRGBA((unsigned char)0, 255, 255, 64));

	r = std::max(CVector3d(out00, out11).length(), CVector3d(out01, out10).length()) / 2;

	return apy;
}

//#define isInPixelShadow(pxlX,pxlY,ptX,ptY) ((ceil(pxlX)==ceil(ptX))&&(ceil(pxlY)==ceil(ptY)))

bool GLViewer::isInPixelShadow(double pxlX, double pxlY, double ptX, double ptY)
{
	double r = 0.6;

	return (((pxlX - r) <= ptX) && ((pxlX + r) > ptX)
		&& ((pxlY - r) <= ptY) && ((pxlY + r) > ptY));
}


void GLViewer::PickMeshPoint(double xx, double yy, CModel3D* obj)
{
	CPoint3d pkt0, pkt1;
	CVector3d vRay;
	if (screen2obj(xx, yy, obj, /*ref*/pkt0, /*ref*/pkt1, /*ref*/vRay))
	{
		CMesh* mesh = (CMesh*)obj->getChild();

		CPoint3f IntersectionPoint;
		INDEX_TYPE faceIdx;

		ULONGLONG t0 = GetTickCount64();
		if (mesh->getClosestFace(pkt0, vRay, /*ref*/IntersectionPoint, /*ref*/faceIdx))
		{
			bool usedInPlugin = false;

			double dA = CVector3d(IntersectionPoint, mesh->vertices()[mesh->faces()[faceIdx].A()]).length();
			double dB = CVector3d(IntersectionPoint, mesh->vertices()[mesh->faces()[faceIdx].B()]).length();
			double dC = CVector3d(IntersectionPoint, mesh->vertices()[mesh->faces()[faceIdx].B()]).length();

			size_t vertexId = 0;

			if (dA <= dB)
				if (dC < dA)
					vertexId = mesh->faces()[faceIdx].C();
				else
					vertexId = mesh->faces()[faceIdx].A();
			else
				if (dC < dB)
					vertexId = mesh->faces()[faceIdx].C();
				else
					vertexId = mesh->faces()[faceIdx].B();

			if (NULL != AP::mainApp().activePlugin)
			{
				Plugin::PickEvent pickEvent;

				pickEvent.viewer = this;
				pickEvent.objId = obj->id();

				pickEvent.mouse.x = xx;
				pickEvent.mouse.y = yy;

				pickEvent.origin.x = pkt0.X();
				pickEvent.origin.y = pkt0.Y();
				pickEvent.origin.z = pkt0.Z();

				pickEvent.direction.x = vRay.X();
				pickEvent.direction.y = vRay.Y();
				pickEvent.direction.z = vRay.Z();


				pickEvent.x = IntersectionPoint.X();
				pickEvent.y = IntersectionPoint.Y();
				pickEvent.z = IntersectionPoint.Z();

				pickEvent.faceId = faceIdx;

				pickEvent.vertexId = vertexId;

				pickEvent.facePicked = true;

				usedInPlugin = AP::mainApp().activePlugin->onMousePick(pickEvent);
			}


			if (!usedInPlugin && AP::mainApp().bGlobalPicking)
			{
				CAnnotationPoint* pt = new CAnnotationPoint(IntersectionPoint);

				pt->setParent(obj);

				if (AP::mainApp().bPickSnap)
				{
					pt->setPoint(mesh->vertices()[vertexId]);
				}

				pt->setRay(vRay);

				pt->setFace(
					faceIdx,
					mesh->vertices()[mesh->faces()[faceIdx].A()],
					mesh->vertices()[mesh->faces()[faceIdx].B()],
					mesh->vertices()[mesh->faces()[faceIdx].C()]);

				unsigned int ptId = AP::MODEL::addAnnotation(obj, pt);
			}

			//tymczasowo !!! -----------------------------------------------------
			//if ( !usedInPlugin && (NULL != AP::mainApp().activePlugin) )
			//{
			//	AP::mainApp().activePlugin->pickSlot(obj->id(), *pt);
			//}
			//------------------------------------------------------------------


			ULONGLONG t1 = GetTickCount64();
			UI::STATUSBAR::printf("Selected point [%lf,%lf,%lf] on object %d (time:%ld)", IntersectionPoint.X(), IntersectionPoint.Y(), IntersectionPoint.Z(), obj->id(), t1 - t0);
		}
	}
}

void GLViewer::PickCloudPoint(double xx, double yy, CModel3D * obj)
{
	CPoint3d pkt0, pkt1;
	CVector3d vRay;
	if (screen2obj(xx, yy, obj, /*ref*/pkt0, /*ref*/pkt1, /*ref*/vRay))
	{
		CPointCloud* cloud = (CPointCloud*)obj->getChild();

		ULONGLONG t0 = GetTickCount64();

		double r;
		CAnnotationPyramid* apy = getPyramid(xx, yy, pkt0, pkt1, obj, r);

		std::vector<INDEX_TYPE> idxs = cloud->getKDtree(CMesh::KDtree::PRESERVE).find_all_in_distance_to_ray(r, pkt0, vRay);

		CAnnotationPoints* hits = new CAnnotationPoints();
		CAnnotationPoints* pointsInPixelShadow = new CAnnotationPoints();

		std::set<INDEX_TYPE> points;

		if (!idxs.empty())
		{

			//double minDist = DBL_MAX;
			//INDEX_TYPE minIdx;

			//CModel3D* obj1 = new CModel3D();
			//AP::WORKSPACE::addModel(obj1);

			for (auto idx : idxs)
			{
				CPoint3d pt(cloud->vertices()[idx]);

				hits->addPoint(pt);

				CPoint3d workspace = obj->getTransform().l2w(pt);
				CPoint3d world = m_transform.l2w(workspace) - cam.m_pos;
				CPoint3d win = world2win(world);

				if (isInPixelShadow(xx, yy, win.x, win.y))
				{
					//double myd = CVector3d(entry, pt).length();

					//if (myd < minDist)
					//{
					//	minDist = myd;
					//	minIdx = idx;
					//}
					points.insert(idx);
					pointsInPixelShadow->addPoint(pt);
				}
			}

			//CAnnotationVertex* hit = new CAnnotationVertex(obj, minIdx);
			//hit->setLabel(L"Trafienie");
			//AP::MODEL::addAnnotation(obj, hit);
		}


		bool usedInPlugin = false;

		if (NULL != AP::mainApp().activePlugin)
		{
			Plugin::PickEvent pickEvent;

			pickEvent.viewer = this;

			pickEvent.objId = obj->id();

			pickEvent.mouse.x = xx;
			pickEvent.mouse.y = yy;

			pickEvent.origin.x = pkt0.X();
			pickEvent.origin.y = pkt0.Y();
			pickEvent.origin.z = pkt0.Z();

			pickEvent.direction.x = vRay.X();
			pickEvent.direction.y = vRay.Y();
			pickEvent.direction.z = vRay.Z();

			pickEvent.hits = points;

			pickEvent.facePicked = false;

			usedInPlugin = AP::mainApp().activePlugin->onMousePick(pickEvent);
		}

		if (!usedInPlugin && AP::mainApp().bGlobalPicking)
		{
			AP::MODEL::addAnnotation(obj, apy);

			AP::MODEL::addAnnotation(obj, hits);

			pointsInPixelShadow->setColor(CRGBA(1.0f, 1.0f, 0.0f));
			AP::MODEL::addAnnotation(obj, pointsInPixelShadow);
		}
		else
		{
			delete apy;
			delete hits;
			delete pointsInPixelShadow;
		}

		ULONGLONG t1 = GetTickCount64();
		UI::STATUSBAR::printf("Object clicked (%d) (time:%ld)", obj->id(), t1 - t0);
	}
}


void GLViewer::PickPoint(int x, int y)
{
	double wx=0.0, wy=0.0, wz=0.0;
	double wx0=0.0, wy0=0.0, wz0=0.0;

	double xx = 0.5 + x;
	double yy = 0.5 + y;

	CModel3D* obj = AP::WORKSPACE::getCurrentModel();

	if ( obj != nullptr )
	{

		if ( obj->type() == CObject::Type::GROUP )
		{
			obj = (CModel3D*) ((CGroupObject *)obj)->getSelectedChild();
		}

		CMesh* mesh = (CMesh*)obj->getChild();
		int typ = mesh->type();

		if (obj->getChild()->hasType(CObject::MESH))
		{
			PickMeshPoint(xx, yy, obj);
		}
		else if ((mesh->hasType(CObject::CLOUD)) || (mesh->hasType(CObject::ORDEREDCLOUD)))
		{
			PickCloudPoint(xx, yy, obj);
		}
		else
		{
			UI::MESSAGEBOX::information(L"Operation not supported", L"I'm sorry. In the current version of the software, this feature is only available for point clouds and meshes.");
		}
	}
	else
	{
		UI::STATUSBAR::printf( "No objects selected" );
	}

//	UI::getMainWindow().updateListViewSelection( sel );
}

void GLViewer::PickObject(int x, int y)
{
	int id = readSelectBuffer( x, y );

	AP::WORKSPACE::setCurrentModel(id);
}


int GLViewer::readSelectBuffer(int x, int y)
{
	const int BUFFER_LENGTH = 64;
	GLuint select_buffer[BUFFER_LENGTH];
	memset(select_buffer, 0, 64);

	//this->glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth))
	glSelectBuffer( BUFFER_LENGTH, select_buffer );

		int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	int width = viewport[2];
	int height = viewport[3];

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glLoadIdentity();
	gluPickMatrix(x, height - y, 1, 1, viewport);

	glRenderMode(GL_SELECT);

	// generowanie sceny 3D
	Render();

	glFlush();

	GLint hits = glRenderMode(GL_RENDER);

	int sel = -1;
	unsigned int selmin = UINT_MAX;
	int indeks = 0;

	for (int i = 0; i < hits; i++)
	{
		if (select_buffer[indeks] > 0)
		{
			for (int n = 0; n < select_buffer[indeks]; n++)
			{
				if (select_buffer[indeks + 3 + n] >= MODEL_ID_OFFSET)
				{
					if (select_buffer[indeks + 1] < selmin)
					{
						selmin = select_buffer[indeks + 1];
						sel = select_buffer[indeks + 3 + n];
					}
				}
			}
		}
		indeks += select_buffer[indeks] + 3;
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	return sel;
}



//int GLViewer::readSelectBuffer(int x, int y)
//{
//    const int BUFFER_LENGTH = 64;
//    GLuint select_buffer[ BUFFER_LENGTH ];
//	memset( select_buffer, 0, 64 );
//
//	this->glReadPixels( x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth))
//	//glSelectBuffer( BUFFER_LENGTH, select_buffer );
//
//    int viewport[4];
//    glGetIntegerv( GL_VIEWPORT, viewport );
//
//    int width = viewport [2];
//    int height = viewport [3];
//
//    glMatrixMode (GL_PROJECTION);
//    glPushMatrix ();
//
//	glLoadIdentity ();
//	gluPickMatrix( x, height-y, 1, 1, viewport );
//
//	glRenderMode( GL_SELECT );
//
//	// generowanie sceny 3D
//	Render();
//
//	glFlush();
//
//	GLint hits = glRenderMode( GL_RENDER );
//
//	glMatrixMode( GL_PROJECTION );
//    glPopMatrix ();
//
//	int sel = -1;
//	unsigned int selmin = UINT_MAX;
//	int indeks = 0;
//
//	for ( int i = 0; i < hits; i++ )
//	{
//		if ( select_buffer[ indeks ] > 0 )
//		{
//			for ( int n = 0; n < select_buffer[ indeks ]; n++ )
//			{
//				if ( select_buffer[ indeks + 3 + n ] >= MODEL_ID_OFFSET )
//				{
//					if ( select_buffer[ indeks + 1 ] < selmin )
//					{
//						selmin = select_buffer[ indeks + 1 ];
//						sel = select_buffer[ indeks + 3 + n ];
//					}
//				}
//			}
//		}
//		indeks += select_buffer[ indeks ]+3;
//	}
//
//	return sel;
//}

