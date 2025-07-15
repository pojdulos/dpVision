#include "PMFactory.h"

void PMFactory::vSplit( CMesh &dst, unsigned int iNewSize )
{
	// - dane siatki podstawowej s¹ ju¿ w modelu docelowym
	// - vsplity s¹ w strukturze dst->vsplits
	// - zakladam brak mapowania indeksów !!!

	StatusBarManager::setText( "PMFactory::vSplit(): Begin of method." );

	dst.fnormals().clear();
	dst.fnormals().swap(dst.fnormals());

	size_t matIdx = 0;

	_mapOfFaces tmpFCSset;
	//for ( CArrayOfFaces::iterator itf=dst.faces().begin(); itf!=dst.faces().end(); itf++ )
	for ( size_t j=0; j<static_cast<size_t>(dst.faces().size()); j++ )
	{
		_sof s;
		s.f = dst.faces()[j];
		if ( ! dst.getMaterial(matIdx).texindex.empty() ) s.t = dst.getMaterial(matIdx).texindex[j];

		try {
			tmpFCSset[ s.f ] = s.t; 
		}
		catch ( std::bad_alloc &e ) {
			UI::MESSAGEBOX::error( L"vSplit: tmpFCSset.insert()" );
			throw e;
		}
	}
	dst.faces().clear();
	dst.getMaterial(matIdx).texindex.clear();

	CMesh::Vsplits::iterator its = dst.vsplits.begin();

	if ( iNewSize > (dst.vertices().size()+dst.vsplits.size()) ) iNewSize = dst.vertices().size()+dst.vsplits.size();

	UI::PROGRESSBAR::init( dst.vertices().size(), iNewSize, dst.vertices().size() );

	while ( ( its != dst.vsplits.end() ) && ( dst.vertices().size() < iNewSize ) )
	{
		StatusBarManager::setText( QString("\r    PMFactory::vSplit(): reconstruction to %1 -> current: %2              ").arg(iNewSize).arg(dst.vertices().size()));
		
		UI::PROGRESSBAR::setValue( dst.vertices().size() );

		//modyfikuje wierzcholek v1
		try {
			dst.vertices()[ its->i1 ] = its->v1;
		}
		catch ( std::bad_alloc &e ) {
			UI::MESSAGEBOX::error( L"vSplit (0): dst->vertices()[]=" );
			throw e;
		}

		//dodaje wierzcholek v2
		try {
			dst.vertices().push_back( its->v2 );
		}
		catch ( std::bad_alloc &e ) {
			UI::MESSAGEBOX::error( L"vSplit (1): vertices.push_back()" );
			throw e;
		}

		//dodaje wartosci UV z tablicy delTC
		for ( _vsplit::TextureCoords::iterator itdtc=its->delTC.begin(); itdtc!=its->delTC.end(); itdtc++ )
		{
			dst.getMaterial(matIdx).texcoord.push_back( itdtc->second );
		}

		//zamienia wartosci UV na podstawie tablicy chgTC
		for ( _vsplit::TextureCoords::iterator itctc=its->chgTC.begin(); itctc!=its->chgTC.end(); itctc++ )
		{
			dst.getMaterial(matIdx).texcoord[ itctc->first ] = itctc->second;
		}

		// odczytuje prawdziwy numer wierzcho³ka v1
		// bedzie potrzebny by znalezc sciany 
		unsigned int id1 = its->i1;
		unsigned int id2 = its->i2;

		//szukam scian z tablicy vs.chgFcs i je zmieniam
		for ( _vsplit::Faces::iterator itchg=its->chgFcs.begin(); itchg!=its->chgFcs.end(); itchg++ )
		{
			unsigned int F[3];
			F[0] = itchg->A();
			F[1] = itchg->B();
			F[2] = itchg->C();

			if ( F[0] == id2 )
			{
				CFace face( id1, F[1], F[2] );
				_mapOfFaces::iterator itfcs = tmpFCSset.find( face );
				if (itfcs != tmpFCSset.end())
				{
					_sof s;
					s.t = itfcs->second;
					tmpFCSset.erase( itfcs );
					face.A( id2 );
					s.f = face;
					try {
						tmpFCSset[ s.f ] = s.t;
					}
					catch ( std::bad_alloc &e ) {
						UI::MESSAGEBOX::error( L"vSplit (3): tmpFCSset.insert()" );
						throw e;
					}
				}
			}
			else if ( F[1] == id2 )
			{
				CFace face( F[0], id1, F[2] );
				_mapOfFaces::iterator itfcs = tmpFCSset.find( face );
				if (itfcs != tmpFCSset.end())
				{
					_sof s;
					s.t = itfcs->second;
					tmpFCSset.erase( itfcs );
					face.B( id2 );
					s.f = face;
					try {
						tmpFCSset[ s.f ] = s.t;
					}
					catch ( std::bad_alloc &e ) {
						UI::MESSAGEBOX::error( L"vSplit (4): tmpFCSset.insert()" );
						throw e;
					}
				}
			}
			else if ( F[2] == id2 )
			{
				CFace face( F[0], F[1], id1 );
				_mapOfFaces::iterator itfcs = tmpFCSset.find( face );
				if (itfcs != tmpFCSset.end())
				{
					_sof s;
					s.t = itfcs->second;
					tmpFCSset.erase( itfcs );
					face.C( id2 );
					s.f = face;
					try {
						tmpFCSset[s.f] = s.t;
					}
					catch ( std::bad_alloc &e ) {
						UI::MESSAGEBOX::error( L"vSplit (5): tmpFCSset.insert()" );
						throw e;
					}
				}
			}
			
		}

		//dodaje sciany z tablicy vs.delFcs
		//for ( _vsplit::Faces::iterator itdel=its->delFcs.begin(); itdel!=its->delFcs.end(); itdel++ )
		for ( size_t j=0; j<static_cast<size_t>(its->delFcs.size()); j++ )
		{
			_sof s;
			s.f = its->delFcs[j];
			if ( ! its->delTI.empty() ) s.t = its->delTI[j];

			try {
				tmpFCSset[ s.f ] = s.t;
			}
			catch ( std::bad_alloc &e ) {
				UI::MESSAGEBOX::error( L"vSplit (6): tmpFCSset.insert()" );
				throw e;
			}
		}
		
		// inkrementuje iterator wskazujacy na vsplita
		its++;
	}

	UI::PROGRESSBAR::hide();
	
	// kasuje wykorzystane vsplity:
	if ( its == dst.vsplits.end() )
	{
		dst.vsplits.clear();
		dst.vsplits.swap(dst.vsplits);
	}
	else
	{
		dst.vsplits.erase( dst.vsplits.begin(), its );
		dst.vsplits.swap(dst.vsplits);
	}

	try {
		dst.faces().reserve( tmpFCSset.size() );
		dst.fnormals().reserve( tmpFCSset.size() );
	}
	catch ( std::bad_alloc &e ) {
		UI::MESSAGEBOX::error( L"vSplit (7): reserve()" );
		throw e;
	}

	for ( _mapOfFaces::iterator itfs=tmpFCSset.begin(); itfs!=tmpFCSset.end(); itfs++ )
	{
		CFace f = itfs->first;
		try {

			dst.faces().push_back( f );
			dst.fnormals().push_back( f.getNormal( dst.vertices() ) );
			dst.getMaterial(matIdx).texindex.push_back( itfs->second );
		}
		catch ( std::bad_alloc &e ) {
			UI::MESSAGEBOX::error( L"vSplit (8): faces.push_back()" );
			throw e;
		}
	}
	
	tmpFCSset.clear();
	tmpFCSset.swap(tmpFCSset);

	StatusBarManager::setText( "PMFactory::vSplit(): End of method." );
}
