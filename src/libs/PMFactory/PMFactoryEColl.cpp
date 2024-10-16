#include "PMFactory.h"

void PMFactory::eColl( CMesh &dst, unsigned int iNewSize )
{
	UI::STATUSBAR::printf( "PMFactory::eColl(): Begin of method." );

	CMeshToQuadrics( dst );
	UI::STATUSBAR::printf( "PMFactory::eColl(): After call CMeshToQuadrics()." );


	initial_quadrics();
	UI::STATUSBAR::printf( "PMFactory::eColl(): After call initial_quadrics()." );

	try {
		Reduction( iNewSize );
	}
	catch ( std::bad_alloc &e ) {
		quadrics.clear();
		errors.clear();
		faces.clear();
		vertices.clear();
		weights.clear();
		vsplits.clear();
		texidxs.clear();
		texcoords.clear();
		UI::STATUSBAR::printf( "Allocation error in Reduction(): %s", e.what() );
		UI::MESSAGEBOX::error( L"Allocation error in Reduction()" );
		return;
	}
	UI::STATUSBAR::printf( "PMFactory::eColl(): After call Reduction()." );

	dst.orgsize = this->orgsize;
	
	QuadricsToCMesh( dst );

	UI::STATUSBAR::printf( "PMFactory::eColl(): End of method." );
}


void PMFactory::Renumerate( CMesh &dst )
{
	UI::STATUSBAR::printf( "PMFactory::Renumerate(): Begin of method." );

	size_t tcidx = mTindexesMap.size();
	size_t value = mVerticesMap.size();
	for (size_t i=0;i<static_cast<size_t>(dst.vsplits.size());i++)
	{
		try {
			mVerticesMap[ dst.vsplits[i].i2 ] = value++;
		}
		catch ( std::bad_alloc &e ) {
			UI::MESSAGEBOX::error( L"PMFactory::Renumerate(): mVerticesMap[]=" );
			throw e;
		}

		for ( _vsplit::TextureCoords::iterator it=dst.vsplits[i].delTC.begin(); it!=dst.vsplits[i].delTC.end(); it++ )
		{
			mTindexesMap[ it->first ] = tcidx++;
		}

	}

	UI::STATUSBAR::printf( "PMFactory::Renumerate(): After mVerticesMap generation." );

	for ( CMesh::Vsplits::iterator itvs=dst.vsplits.begin(); itvs!=dst.vsplits.end(); itvs++ )
	{
		itvs->i1 = mVerticesMap[ itvs->i1 ];
		itvs->i2 = mVerticesMap[ itvs->i2 ];

		for ( _vsplit::Faces::iterator itfc=itvs->chgFcs.begin(); itfc!=itvs->chgFcs.end(); itfc++ )
		{
			itfc->A( mVerticesMap[ itfc->A() ] );
			itfc->B( mVerticesMap[ itfc->B() ] );
			itfc->C( mVerticesMap[ itfc->C() ] );
		}

		for ( _vsplit::Faces::iterator itfd=itvs->delFcs.begin(); itfd!=itvs->delFcs.end(); itfd++ )
		{
			itfd->A( mVerticesMap[ itfd->A() ] );
			itfd->B( mVerticesMap[ itfd->B() ] );
			itfd->C( mVerticesMap[ itfd->C() ] );
		}

		for ( _vsplit::TextureCoords::iterator it=itvs->delTC.begin(); it!=itvs->delTC.end(); it++ )
		{
			it->first = mTindexesMap[ it->first ];
		}

		for ( _vsplit::TextureIndexes::iterator it=itvs->delTI.begin(); it!=itvs->delTI.end(); it++ )
		{
			it->a = mTindexesMap[ it->a ];
			it->b = mTindexesMap[ it->b ];
			it->c = mTindexesMap[ it->c ];
		}
	}

	dst.version = 2;

	UI::STATUSBAR::printf( "PMFactory::Renumerate(): End of method." );
	return;
}

void PMFactory::Reduction(size_t target_num_vertices)
{
	UI::STATUSBAR::printf( "PMFactory::Reduction(): calculating initial errors..." );
	// calculate initial error for each valid pair

	select_pair();
	
	size_t id_v1, id_v2;
	double vx, vy, vz;
	SVsplit v;

	TCmap tcmap;
	for ( TextureCoords::iterator itc=texcoords.begin(); itc!=texcoords.end(); itc++ )
	{
		tcmap[ itc->first ] = 0;
	}
	
	for ( TextureIndexes::iterator itx=texidxs.begin(); itx!=texidxs.end(); itx++ )
	{
		tcmap[ itx->second.a ]++;
		tcmap[ itx->second.b ]++;
		tcmap[ itx->second.c ]++;
	}

	for ( TCmap::iterator itm=tcmap.begin(); itm!=tcmap.end(); )
	{
		if ( itm->second == 0 ) 
			itm = tcmap.erase( itm );
		else
			itm++;
	}

	UI::PROGRESSBAR::init( 0, vertices.size() - target_num_vertices, 0 );

	while (vertices.size() > target_num_vertices)
	{
		UI::STATUSBAR::printfTimed( 500, "\r    PMFactory::Reduction(): reduction from %d to %d -> current: %d              ", orgsize, target_num_vertices, vertices.size() );
		UI::PROGRESSBAR::setValue( orgsize - vertices.size() );

		CPMFerrors::iterator iter_min_error = errors.GetMinError();
		
		if ( iter_min_error == errors.end() )
		{
			UI::STATUSBAR::printf( "PMFactory::Reduction(): ERROR" );
			break;
		}

		id_v1 = iter_min_error->first.first;
		id_v2 = iter_min_error->first.second;

		// wyznaczam nowe wspolrzedne wierzcholka id_v1
		calculate_error(id_v1, id_v2, &vx, &vy, &vz);	// get coordinate of vf

		v.v1 = vertices[id_v1];
		v.i1 = id_v1;
		
		v.v2 = vertices[id_v2];
		v.i2 = id_v2;

		// modyfikuje wierzcholek id_v1
		vertices[id_v1] = CVertex( vx, vy, vz );

		/***********************************************************/
		// CZY DLA USTALONYCH WIERZCHO£KÓW TO JEST POPRAWNE? :

		// update quadric of v1
		quadrics[id_v1] = quadrics[id_v1] + quadrics[id_v2];
		/***********************************************************/
		

		v.delFcs.clear();
		v.chgFcs.clear();
		v.delTC.clear();
		v.delTI.clear();
		v.chgTC.clear();

		// szukam scian zawierajacych wierzcholek id_v2 i zamieniam go na nowy id_v1
		// sciany zawierajace jednoczesnie id_v2 i id_v1 usuwam
		CPMFfaces::iterator fiter;
		while ( faces.end() != ( fiter = faces.FindIfV( id_v2 ) ) )
		{
			size_t fkey = fiter->first;

			if ( -1 != fiter->second.hasVertex( id_v1 ) )
			{
				// jesli sciana zawiera tez id_v1 to KASUJE SCIANE
				
				// najpierw kasuje wpisy zwiazane z tekstura
				if ( ! texidxs.empty() )
				{
					size_t a = texidxs[ fkey ].a;
					tcmap[ a ]--;
					if ( tcmap[ a ] == 0 )
					{
						v.delTC.push_back( _vsplit::TCoord(a,texcoords[ a ]) );
						texcoords.erase( a );
						tcmap.erase( a );
					}
					
					size_t b = texidxs[ fkey ].b;
					tcmap[ b ]--;
					if ( tcmap[ b ] == 0 )
					{
						v.delTC.push_back( _vsplit::TCoord(b,texcoords[ b ]) );
						texcoords.erase( b );
						tcmap.erase( b );
					}

					size_t c = texidxs[ fkey ].c;
					tcmap[ c ]--;
					if ( tcmap[ c ] == 0 )
					{
						v.delTC.push_back( _vsplit::TCoord(c,texcoords[ c ]) );
						texcoords.erase( c );
						tcmap.erase( c );
					}

					v.delTI.push_back( texidxs[ fkey ] );
					texidxs.erase( fkey );
				}

				// dopisuje sciane do delFcs
				v.delFcs.push_back( fiter->second );
				// usuwam sciane
				faces.erase( fiter );
			}
			else
			{
				// jesli sciana nie zawiera id_v1 to ZMIENIAM SCIANE

				// teksture na razie pozostawiam bez zmian
				//if ( ! texidxs.empty() )
				//{
				//	if ( j == 0 ) texidxs[ fkey ].a = id_v1-1;
				//	else if ( j == 1 ) texidxs[ fkey ].b = id_v1-1;
				//	else if ( j == 2 ) texidxs[ fkey ].c = id_v1-1;
				//}

				// dopisuje oryginalna sciane do chgFcs
				v.chgFcs.push_back( fiter->second );
				// zamieniam id_v2 na id_v1
				faces.ChangeValue( fiter->first, id_v2, id_v1 );
			}
		}

		vsplits.push_back(v);

		vertices.erase(id_v2);

		CPMFerrors::iterator iter;
		while ( errors.end() != ( iter = errors.FindIfV( id_v2 ) ) )
		{
			Pair p = iter->first;

			if ( (p.first == id_v2 && p.second == id_v1) || (p.second == id_v2 && p.first == id_v1) )
			{
				errors.erase(iter);
			}
			else if (p.first == id_v2 && p.second != id_v1)
			{
				errors.erase(iter);
				errors.insert(CPMFerrors::value_type( Pair(std::min(id_v1, p.second), std::max(id_v1, p.second)), 0.0 ));
			}
			else if (p.second == id_v2 && p.first != id_v1)
			{
				errors.erase(iter);
				errors.insert(CPMFerrors::value_type( Pair(std::min(id_v1, p.first), std::max(id_v1, p.first)), 0.0 ));
			}
		}


		// dla wszystkich krawedzi zawieraj¹cych id_v1 przeliczamy b³êdy
		std::vector<CPMFerrors::iterator> founds;
		errors.FindAllIfV( id_v1, founds );
		for (std::vector<CPMFerrors::iterator>::iterator iter=founds.begin(); iter!=founds.end(); ++iter )
		{
			errors.ChangeValue( (*iter), calculate_error( (*iter)->first.first, (*iter)->first.second ) );
		}

	}

	UI::PROGRESSBAR::hide();

	quadrics.clear();
	quadrics.swap(quadrics);

	errors.clear();
	errors.swap(errors);
}
