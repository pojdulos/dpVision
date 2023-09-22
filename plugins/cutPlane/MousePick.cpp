#include "stdafx.h"
#include "CommonDLL.h"
#include "ConcretePlugin.h"

#include "Workspace.h"
#include "MainApplication.h"

#include "AP.h"

#include "OrderedPointCloud.h"

#include "AnnotationPoint.h"
#include "AnnotationPoints.h"
#include "AnnotationVertex.h"
#include "AnnotationPath.h"
#include "AnnotationPlane.h"
#include "AnnotationBoundVectors.h"
#include "AnnotationPyramid.h"
#include "AnnotationSetOfVertices.h"


bool rayPlaneIntersect3D(CPoint3d pA, CVector3d vNorm, CVector3d vRay, CPoint3d pP0, CPoint3d& pIntersectionPoint, double& pDistance)
{
	if (0 == vNorm.length()) return false;

	// wektor od wierzcho³ka trójk¹ta do punktu na promieniu
	CVector3d w0 = CVector3d(pA, pP0);

	// iloczyn skalarny -> zero jeœli wektory prostopad³e
	double a = -vNorm.dotProduct(w0); // 0 -> w0 prostopad³y do vNorm -> punkt pP0 le¿y na p³aszczyŸnie trójk¹ta

	if (a == 0)
	{
		pIntersectionPoint = pP0;
		pDistance = 0.0;

		return true;
	}

	double b = vNorm.dotProduct(vRay);	// b == 0 -> vRay prostopad³y do vNorm -> vRay jest równoleg³y do trójk¹ta
								// b < 0  -> vRay wpada od przodu -> OK
								// b > 0  -> vRay wpada od ty³u -> NIE OK

	//if ( fabs(b) < prawieZero ) // to jest chyba niepotrzebne o ile sie nie pojawi¹ b³êdy
	if (b == 0)
	{ // vRay jest równoleg³y do p³aszczyzny trójk¹ta
		return false;
	}

	double r = a / b;

	// Wyznaczam punkt przeciêcia promienia z p³aszczyzna œciany
	CVector3d vec = (vRay * a) / b;

	pDistance = abs(r);
	pIntersectionPoint = pP0 + vec;

	return true;
}

bool inTriangle(CPoint3d pt, CPoint3d pA, CPoint3d pB, CPoint3d pC)
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

//bool inRectangle(CPoint3d pt, CPlane pl, CPoint3d A, CPoint3d B, CPoint3d C, CPoint3d D)
//{
//	CPoint3d ppt = pl.projectionOfPoint(pt);
//	return inTriangle(ppt, A, B, C) || inTriangle(ppt, C, D, A);
//}
//
bool inRectangle(CPoint3d pt, CPoint3d A, CPoint3d B, CPoint3d C, CPoint3d D)
{
	return inTriangle(pt, A, B, C) || inTriangle(pt, C, D, A);
}

void findIntersection(CPoint3d origin, CVector3d direction, CPoint3d A, CPoint3d B, CPoint3d C, CPoint3d D, std::vector<std::pair<double, CPoint3d>>& intersections)
{
	CPoint3d pIntersectionPoint;
	double pDistance;

	CPlane plane(A, B, C);

	if (rayPlaneIntersect3D(plane.getCenter(), plane.getNormal(), direction, origin, pIntersectionPoint, pDistance))
	{
		if (inRectangle(pIntersectionPoint, A, B, C, D))
			intersections.push_back(std::pair<double, CPoint3d>(pDistance, pIntersectionPoint));
	}
#ifdef _DEBUG
	else
		assert(0);
#endif
}

bool getEntryAndExit(CPoint3d pMin, CPoint3d pMax, CPoint3d origin, CVector3d direction, CPoint3d& entry, CPoint3d& exit)
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

CPoint3d convertLocalToWin(CPoint3d pt, CTransform& tr)
{
	CPoint3d winCoords;
	UI::CAMERA::convertWorldToWin(tr.local2world(pt), winCoords);
	return winCoords;
}

CPoint3d convertWinToLocal(CPoint3d pt, CTransform& tr)
{
	CPoint3d worldCoords;
	UI::CAMERA::convertWinToWorld(pt, worldCoords);
	return tr.world2local(worldCoords);
}

CAnnotationPyramid* getPyramid(double x, double y, CPoint3d entry, CPoint3d exit, CTransform tr, double& r)
{
	CPoint3d win, winA;

	winA = convertLocalToWin(entry, tr);
	win = convertLocalToWin(exit, tr);

	CPoint3d win00(x - 0.5, y - 0.5, win.Z());
	CPoint3d win01(x - 0.5, y + 0.5, win.Z());
	CPoint3d win11(x + 0.5, y + 0.5, win.Z());
	CPoint3d win10(x + 0.5, y - 0.5, win.Z());
	CPoint3d p00, p01, p11, p10;

	p00 = convertWinToLocal(win00, tr);
	p01 = convertWinToLocal(win01, tr);
	p11 = convertWinToLocal(win11, tr);
	p10 = convertWinToLocal(win10, tr);

	CPoint3d win00a(x - 0.5, y - 0.5, winA.Z());
	CPoint3d win01a(x - 0.5, y + 0.5, winA.Z());
	CPoint3d win11a(x + 0.5, y + 0.5, winA.Z());
	CPoint3d win10a(x + 0.5, y - 0.5, winA.Z());
	CPoint3d p00a, p01a, p11a, p10a;

	p00a = convertWinToLocal(win00a, tr);
	p01a = convertWinToLocal(win01a, tr);
	p11a = convertWinToLocal(win11a, tr);
	p10a = convertWinToLocal(win10a, tr);

	CAnnotationPyramid* apy = new CAnnotationPyramid();
	apy->setTop(p00a, p01a, p11a, p10a);
	apy->setBottom(p00, p01, p11, p10);
	apy->setColor(CRGBA((unsigned char)0, 255, 255, 64));

	r = std::max(CVector3d(exit, p00).length(), CVector3d(exit, p11).length());

	return apy;
}

inline bool isInPixelShadow(double pxlX, double pxlY, double ptX, double ptY)
{
	double r = 0.6;
	return (((pxlX - r) <= ptX) && ((pxlX + r) > ptX)
		&& ((pxlY - r) <= ptY) && ((pxlY + r) > ptY));
}

bool ConcretePlugin::onMousePick(Plugin::PickEvent pickEvent)
{
	CModel3D* obj = AP::WORKSPACE::getCurrentModel();
	CMesh* mesh = (CMesh*)(obj->getChild());

	double x = pickEvent.mouse.x + 0.5;
	double y = pickEvent.mouse.y + 0.5;

	CPoint3d mouseNear(x, y, 0.0);
	CPoint3d mouseFar(x, y, 1.0);

	CPoint3d worldNear, worldFar;

	CPoint3d origin = convertWinToLocal(mouseNear, obj->getTransform());
	CPoint3d farorigin = convertWinToLocal(mouseFar, obj->getTransform());

	CVector3d direction = CVector3d(origin, farorigin).getNormalized();

	CPoint3d entry;
	CPoint3d exit;

	UI::STATUSBAR::setText("PT0");

	bool test = getEntryAndExit(obj->getMin(), obj->getMax(), origin, direction, entry, exit);

	if (test)
	{
		UI::STATUSBAR::setText("PT1");

		//CAnnotationPoint *in = new CAnnotationPoint(entry);
		//in->setLabel(L"Entry point");
		//AP::MODEL::addAnnotation(obj, in);

		//CAnnotationPoint *out = new CAnnotationPoint(exit);
		//out->setLabel(L"Exit point");
		//AP::MODEL::addAnnotation(obj, out);

		CAnnotationPath* path = new CAnnotationPath();
		path->addPoint(entry);
		path->addPoint(exit);

		double r;
		CAnnotationPyramid* apy = getPyramid(x, y, entry, exit, obj->getTransform(), r);

		std::vector<INDEX_TYPE> idxs = mesh->getKDtree(CMesh::KDtree::PRESERVE).find_all_in_distance_to_ray(r, entry, direction);

		UI::STATUSBAR::printf("Znaleziono %d punktów", idxs.size());

		std::vector<std::pair<double, size_t>> wybrane;

		size_t limit = 100;
		double ostatni = DBL_MAX;

		wybrane.clear();

		CAnnotationPoints* pts = new CAnnotationPoints();

		for (std::vector<INDEX_TYPE>::iterator its = idxs.begin(); its != idxs.end(); its++)
		{
			pts->addPoint(mesh->vertices()[*its]);

			CPoint3d winCoords = convertLocalToWin(mesh->vertices()[*its], obj->getTransform());

			//if ((pickEvent.mouse.x == floor(winCoords.X())) && (pickEvent.mouse.y == floor(winCoords.Y())))
			if (isInPixelShadow(x, y, winCoords.X(), winCoords.Y()))
			{
				double myd = CVector3d(entry, mesh->vertices()[*its]).length(); //odleg³oœæ punktu od operatora

				if ((myd < ostatni) || (wybrane.size() < limit))
				{
					wybrane.push_back(std::make_pair(myd, *its)); //dok³ada element do wektora
					std::push_heap(wybrane.begin(), wybrane.end());	// przesuwa najwiêkszy element na pocz¹tek wektora

					if (wybrane.size() > limit)
					{
						std::pop_heap(wybrane.begin(), wybrane.end()); // przesuwa najwiêkszy element na koniec wektora,
																// a jednoczeœnie na pocz¹tku pojawia sie drugi w kolejnoœci element
						wybrane.pop_back();						// usuwa ostatni element wektora
					}

					ostatni = wybrane[0].first;				// teraz pierwszy element wektora zawiera najbardziej odleg³y punkt z wybranych
				}
			}
		}

		if (wybrane.size() > 0)
		{
			std::sort_heap(wybrane.begin(), wybrane.end());

			CAnnotationVertex* pt = new CAnnotationVertex(obj->id(), wybrane[0].second);
			pt->setLabel(L"Trafienie");
			AP::MODEL::addAnnotation(obj, pt);
		}
		else
		{
#ifdef _DEBUG
			AP::MODEL::addAnnotation(obj, apy);
			//AP::MODEL::addAnnotation(obj, path);
			AP::MODEL::addAnnotation(obj, pts);
#else
			UI::STATUSBAR::setText("Sorry, you did nit hit the point !");
#endif
		}
	}
#ifdef _DEBUG
	else
		assert(0);
#endif

	UI::updateAllViews();
	return true;
}
