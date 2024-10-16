#include "PMFactory.h"

void PMFactory::SetFixedPoints(void)
{
	FixedVertices.clear();
	FixedVertices.insert( Vertices::value_type( FixedVertices.size(), CVertex(0.850651f,0.0f,0.525731f) ) );
	FixedVertices.insert( Vertices::value_type( FixedVertices.size(), CVertex(0.525731f,0.850651f,0.0f) ) );


};

bool PMFactory::IsFixed( CVertex v )
{
	for ( Vertices::iterator it=FixedVertices.begin(); it!=FixedVertices.end(); it++ )
	{
		if ( (*it).second == v )
		{
			return true;
		}
	}
	return false;
};

size_t PMFactory::FindClosestToFixed( CVertex v )
{
	size_t id = 0;
	double err = 999999.99;

	// Do przemyœlenia - to jest nieoptymalne !!!
	for ( Vertices::iterator it=vertices.begin(); it!=vertices.end(); it++ )
	{
		CVector3f w( (*it).second.X() - v.X(), (*it).second.Y() - v.Y(), (*it).second.Z() - v.Z() );
		
		double tmperr = (double)w.X()*w.X()+(double)w.Y()*w.Y()+(double)w.Z()*w.Z();

		if ( tmperr<err )
		{
			err = tmperr;
			id = (*it).first;
		}
	}
	return id;
};

bool PMFactory::BarycentricTest( CVertex p0, CVertex p1, CVertex p2, CVertex p )
{
	CMatrix3x3<double>  M( p0.X(), p1.X(), p2.X(), p0.Y(), p1.Y(), p2.Y(), p0.Z(), p1.Z(), p2.Z() );
	CMatrix3x3<double> Mx(  p.X(), p1.X(), p2.X(),  p.Y(), p1.Y(), p2.Y(),  p.Z(), p1.Z(), p2.Z() );
	CMatrix3x3<double> My( p0.X(),  p.X(), p2.X(), p0.Y(),  p.Y(), p2.Y(), p0.Z(),  p.Z(), p2.Z() );
	CMatrix3x3<double> Mz( p0.X(), p1.X(),  p.X(), p0.Y(), p1.Y(),  p.Y(), p0.Z(), p1.Z(),  p.Z() );

	// g³ówny wyznacznik
	double w  = M.det();

	// wspó³rzêdne barycentryczne
	double b0 = Mx.det() / w;
	double b1 = My.det() / w;
	double b2 = Mz.det() / w;

	// true jeœli le¿¹ wewn¹trz trójk¹ta
	return ( ( b0 >= 0 ) && ( b1 >= 0 ) && ( b2 >= 0 ) );
}

bool PMFactory::projectionOfThePointOnTheFace( CFace f, CVertex p, CVertex &ret )
{
	CVertex p0 = vertices[ f.A() ];
	CVertex p1 = vertices[ f.B() ];
	CVertex p2 = vertices[ f.C() ];

	ret = CPlane( p0, p1, p2 ).projectionOfPoint( p );

	return BarycentricTest( p0, p1, p2, ret );
}

void PMFactory::findAllProjections()
{
	std::map<size_t,std::pair<double,std::pair<size_t,CVertex>>> mapa;

	// To prawdopodobnie nie jest optymalne rozwi¹zanie, ale mam tylko jeden przebieg po œcianach
	// wiêc powinno siê wykonaæ w przyzwoitym czasie.
	// Sta³ych punktów (wewnêtrzna pêtla) jest stosunkowo ma³o.

	for ( CPMFfaces::iterator itf=faces.begin(); itf!=faces.end(); itf++ )
	{
		for ( Vertices::iterator itv=FixedVertices.begin(); itv!=FixedVertices.end(); itv++ )
		{
			CVertex rzut;
			if ( projectionOfThePointOnTheFace( (*itf).second, (*itv).second, rzut ) )
			{
				// Ze wspó³rzêdnych barycentrycznych wynika, ¿e obraz le¿y wewn¹trz œciany.
				// Mam dostêpne wspó³rzêdne obrazu oraz indeksy punktu oraz œciany.
				// Trzeba to teraz gdzieœ zapisaæ.

				double wx = (*itv).second.X() - rzut.X();
				double wy = (*itv).second.Y() - rzut.Y();
				double wz = (*itv).second.Z() - rzut.Z();

				double dist = wx*wx + wy*wy + wz*wz;

				std::pair<size_t,CVertex> paraFR( (*itf).first, rzut );

				std::map<size_t,std::pair<double,std::pair<size_t,CVertex>>>::iterator itm;

				if ( ( itm = mapa.find( (*itv).first ) ) != mapa.end() )
				{
					// zmieniam tylko jeœli odleg³oœæ punktu od rzutu
					// jest mniejsza od zapisanej
					if ( dist < (*itm).second.first )
					{
						mapa[ (*itv).first ] = std::pair<double,std::pair<size_t,CVertex>>( dist, paraFR );
					}
				}
				else
				{
					mapa[ (*itv).first ] = std::pair<double,std::pair<size_t,CVertex>>( dist, paraFR );
				}
			}
		}
	}

	// tu powinienem mieæ mapê przypisuj¹c¹ indeksom "sta³ych punktów" ich rzuty
	// oraz œciany na których te rzuty znaleziono.
	// UWAGA: jest mo¿liwe, ¿e nie znaleziono takiego rzutu. Co w takim przypadku???
	// - Ja bym przypisa³ "sta³y punkt" do najbli¿szego wierzcho³ka.
}

