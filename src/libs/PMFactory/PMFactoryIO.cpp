#include "PMFactory.h"

void PMFactory::CMeshToQuadrics( CMesh &src )
{
	CVertex v;
	CFace f;
	int local_num_vertices = 0;

	size_t matIdx = 0;

	orgsize = src.vertices().size()+src.vsplits.size();

	for (size_t j = 0; j<static_cast<size_t>(src.vsplits.size()); j++)
	{
		mVerticesMap.insert(_mapOfVertices::value_type(src.vsplits[j].i2, j+src.vertices().size()));
	}

	for ( CMesh::Vertices::iterator it=src.vertices().begin(); it!=src.vertices().end(); it++ )
	{
		StatusBarManager::printfTimed( 500, QString("PMFactory: Importing vertices... %1").arg(local_num_vertices));

		v.X( it->X() );
		v.Y( it->Y() );
		v.Z( it->Z() );
		local_num_vertices++;
		vertices.insert(Vertices::value_type(local_num_vertices, v));
		weights.insert(LoF::value_type(local_num_vertices,0)); // zamiast 0 wstawiæ wagê, gdzie 0 = dowolny wierzcho³ek, 255 = ca³kowicie zafiksowany;
	}

	StatusBarManager::setText( "PMFactory: Importing faces..." );
	for ( CMesh::Faces::iterator itf=src.faces().begin(); itf!=src.faces().end(); itf++ )
	{
		f.A( itf->A()+1 );
		f.B( itf->B()+1 );
		f.C( itf->C()+1 );

		faces.insert( _faces::value_type( faces.size(), f ) );
	}

	hasTexture = false;
	if ( ! src.getMaterial(matIdx).texindex.empty() )
	{
		StatusBarManager::setText( "PMFactory: Importing texidxs..." );
		for ( size_t j=0; j<static_cast<size_t>(src.getMaterial(matIdx).texindex.size()); j++ )
		{
			CTIndex ti;
			ti.a = src.getMaterial(matIdx).texindex[j].a;
			ti.b = src.getMaterial(matIdx).texindex[j].b;
			ti.c = src.getMaterial(matIdx).texindex[j].c;

			texidxs.insert( TextureIndexes::value_type( j, ti ) );
		}
		hasTexture = true;
	}

	if ( ! src.getMaterial(matIdx).texcoord.empty() )
	{
		StatusBarManager::setText( "PMFactory: Importing texcoords..." );
		for ( size_t j=0; j<static_cast<size_t>(src.getMaterial(matIdx).texcoord.size()); j++ )
		{
			CTCoord tc;

			tc[0] = src.getMaterial(matIdx).texcoord[j][0];
			tc[1] = src.getMaterial(matIdx).texcoord[j][1];

			texcoords.insert( TextureCoords::value_type( j, tc ) );
		}
	}

	StatusBarManager::setText( "PMFactory: Importing vsplits..." );
	for ( CMesh::Vsplits::reverse_iterator rit=src.vsplits.rbegin(); rit!=src.vsplits.rend(); rit++ )
	{
		SVsplit vs;
		vs.i1 = rit->i1+1;
		vs.v1.X( rit->v1.X() );
		vs.v1.Y( rit->v1.Y() );
		vs.v1.Z( rit->v1.Z() );

		vs.i2 = rit->i2+1;
		vs.v2.X( rit->v2.X() );
		vs.v2.Y( rit->v2.Y() );
		vs.v2.Z( rit->v2.Z() );

		for ( _vsplit::Faces::iterator itfc=rit->chgFcs.begin();itfc!=rit->chgFcs.end();itfc++ )
		{
			CFace f;
			f.A( itfc->A()+1 );
			f.B( itfc->B()+1 );
			f.C( itfc->C()+1 );
			vs.chgFcs.push_back( f );
		}

		for ( _vsplit::Faces::iterator itfd=rit->delFcs.begin();itfd!=rit->delFcs.end();itfd++ )
		{
			CFace f;
			f.A( itfd->A()+1 );
			f.B( itfd->B()+1 );
			f.C( itfd->C()+1 );
			vs.delFcs.push_back( f );
		}

		vs.delTC = rit->delTC;
		vs.delTI = rit->delTI;

		vs.chgTC = rit->chgTC;

		vsplits.push_back( vs );
	}

	StatusBarManager::setText( "Importing data... Done");
}

void PMFactory::QuadricsToCMesh( CMesh &dst )
{
	mVerticesMap.clear();

	//---------------------------------------------------------------------------------

	StatusBarManager::setText( "Exporting data: Vertices array" );

	try {
		dst.vertices().resize( vertices.size() );
	}
	catch ( std::bad_alloc &e ) {
		UI::MESSAGEBOX::error( L"vertices.resize()" );
		throw e;
	}

	size_t idx = 0;
	for (Vertices::iterator iter = vertices.begin(); iter != vertices.end();)
	{
		CVertex v = iter->second;
		size_t key = iter->first;

		dst.vertices()[ idx ] = v;

		dst.expandBoundingBox( v );

		try {
			mVerticesMap.insert(_mapOfVertices::value_type(key, idx));
		}
		catch ( std::bad_alloc &e ) {
			UI::MESSAGEBOX::error( L"mVerticesMap.insert()" );
			throw e;
		}
		idx++;

		iter++;// = vertices.erase( iter );
	}

	vertices.clear();
	weights.clear();

	//---------------------------------------------------------------------------------
	size_t matIdx = 0;

	if ( ! texcoords.empty() )
	{
		try {
			dst.getMaterial(matIdx).texcoord.resize( texcoords.size() );
		}
		catch ( std::bad_alloc &e ) {
			UI::MESSAGEBOX::error( L"texcoord.resize()" );
			throw e;
		}

		StatusBarManager::setText( "PMFactory: Exporting texcoords..." );

		size_t idx = 0;
		//for ( size_t j=0; j<static_cast<size_t>(texcoords.size()); j++ )
		for (TextureCoords::iterator iter = texcoords.begin(); iter != texcoords.end();)
		{
			size_t key = iter->first;

			dst.getMaterial(matIdx).texcoord[ idx ][0] = iter->second[0];
			dst.getMaterial(matIdx).texcoord[ idx ][1] = iter->second[1];

			try {
				mTindexesMap.insert(_mapOfVertices::value_type(key, idx));
			}
			catch ( std::bad_alloc &e ) {
				UI::MESSAGEBOX::error( L"mTindexesMap.insert()" );
				throw e;
			}
			idx++;

			iter++;// = vertices.erase( iter );
		}
	}

	//---------------------------------------------------------------------------------

	try {
		dst.faces().resize( faces.size() );
	}
	catch ( std::bad_alloc &e ) {
		UI::MESSAGEBOX::error( L"faces.resize()" );
		throw e;
	}

	try {
		dst.fnormals().resize( faces.size() );
	}
	catch ( std::bad_alloc &e ) {
		UI::MESSAGEBOX::error( L"fnormals().resize()" );
		throw e;
	}

	StatusBarManager::setText( "Exporting data: Faces array" );
	idx = 0;
	for (CPMFfaces::iterator fit=faces.begin(); fit!=faces.end();)
	{
		CFace f( mVerticesMap[fit->second.A()], mVerticesMap[fit->second.B()], mVerticesMap[fit->second.C()] );

		dst.faces()[ idx ] = f;
		dst.fnormals()[ idx ] = f.getNormal( dst.vertices() );
		idx++;

		fit = faces.erase( fit );
	}
	//faces.clear();

	//----------------------------------------------------------------------------------

	if ( hasTexture && ( ! texidxs.empty() ) )
	{
		dst.getMaterial(matIdx).texindex.resize( texidxs.size() );
		StatusBarManager::setText( "PMFactory: Exporting texidxs..." );

		idx=0;
		for (TextureIndexes::iterator tit=texidxs.begin(); tit!=texidxs.end();)
		{
			CTIndex ti;
			ti.a = mTindexesMap[ tit->second.a ];
			ti.b = mTindexesMap[ tit->second.b ];
			ti.c = mTindexesMap[ tit->second.c ];

			dst.getMaterial(matIdx).texindex[idx] = ti;
			idx++;

			tit++;
		}
	}


	//----------------------------------------------------------------------------------



	dst.vsplits.clear();
	dst.vsplits.reserve( vsplits.size() );

	StatusBarManager::setText( "Exporting data: Vsplits array" );

	for (int i = static_cast<int>(vsplits.size()-1); i >= 0; i--)
	{
		try {
			dst.vsplits.push_back( vsplits[i] );
		}
		catch ( std::bad_alloc &e ) {
			UI::MESSAGEBOX::error( L"vsplits.push_back()" );
			throw std::runtime_error( e.what() );
		} 
	}

	//----------------------------------------------------------------------------------



	dst.orgsize = orgsize;

	Renumerate( dst );

	StatusBarManager::setText( "PMFactory::QuadricsToCMesh(): End of method." );
}

/*
void PMFactory::SavePMT( CMesh &src, const char* nazwa, CVector3f r, CVector3f t )
{
	StatusBarManager::setText( "PMFactory::SaveNewFormSMF() is writing to file (ver.5) now, please wait..." );
	
	FILE *plik = fopen( nazwa, "w" );
	
	fprintf( plik, "# PM wersja pliku 05.14-21.00\n$m 5\n$o %d\n", src.orgsize );
	fprintf( plik, "$v %d\n$f %d\n$s %d\n", src.vertices().size(), src.faces().size(), src.vsplits.size() );
	fprintf( plik, "$i %d\n$c %d\n$n %s\n", src.texindex.size(), src.texcoord.size(), src.TexInfo.absoluteFilePathW().c_str() );
	fprintf( plik, "$r %f,%f,%f\n", r.X(), r.Y(), r.Z() );
	fprintf( plik, "$t %f,%f,%f\n", t.X(), t.Y(), t.Z() );
	fprintf( plik, "$p 1.0\n" );
	fprintf( plik, "#\n" );

	for ( CMesh::Vertices::iterator itv=src.vertices().begin(); itv!=src.vertices().end(); itv++ )
	{
		fprintf( plik, "v %f %f %f\n", itv->X(), itv->Y(), itv->Z() );
	}

	for ( CMesh::Faces::iterator itf=src.faces().begin(); itf!=src.faces().end(); itf++ )
	{
		fprintf( plik, "f %d %d %d\n", itf->A(), itf->B(), itf->C() );
	}

	for ( CArrayOfTIndex::iterator iti=src.texindex.begin(); iti!=src.texindex.end(); iti++ )
	{
		fprintf( plik, "i %d %d %d\n", iti->a, iti->b, iti->c );
	}

	for ( CArrayOfTCoord::iterator itc=src.texcoord.begin(); itc!=src.texcoord.end(); itc++ )
	{
		fprintf( plik, "c %f %f\n", itc->s, itc->t );
	}

	for ( CArrayOfVsplits::iterator its=src.vsplits.begin(); its!=src.vsplits.end(); its++ )
	{
		fprintf( plik, "s %d %f %f %f", its->i1, its->v1.X(), its->v1.Y(), its->v1.Z() );
		fprintf( plik, " %d %f %f %f %d", its->i2, its->v2.X(), its->v2.Y(), its->v2.Z(), its->delFcs.size() );

		for ( _vsplit::Faces::iterator itfd=its->delFcs.begin(); itfd!=its->delFcs.end(); itfd++ )
			fprintf( plik, " %d %d %d", itfd->A(), itfd->B(), itfd->C() );

		fprintf( plik, " %d", its->chgFcs.size() );

		for ( _vsplit::Faces::iterator itfc=its->chgFcs.begin(); itfc!=its->chgFcs.end(); itfc++ )
			fprintf( plik, " %d %d %d", itfc->A(), itfc->B(), itfc->C() );

		fprintf( plik, " %d", its->delTI.size() );
		for ( CArrayOfTIndex::iterator itti=its->delTI.begin(); itti!=its->delTI.end(); itti++ )
			fprintf( plik, " %d %d %d", itti->a, itti->b, itti->c );

		fprintf( plik, " %d", its->delTC.size() );
		for ( _vsplit::TextureCoords::iterator ittc=its->delTC.begin(); ittc!=its->delTC.end(); ittc++ )
			fprintf( plik, " %d %f %f", ittc->first, ittc->second.s, ittc->second.t );

		fprintf( plik, " %d", its->chgTC.size() );
		for ( _vsplit::TextureCoords::iterator ittc=its->chgTC.begin(); ittc!=its->chgTC.end(); ittc++ )
			fprintf( plik, " %d %f %f", ittc->first, ittc->second.s, ittc->second.t );

		fprintf( plik, "\n" );
	}

	fclose( plik );
}
*/
/*
void PMFactory::SaveXML( CMesh &src, const char* nazwa )
{
	StatusBarManager::setText( "PMFactory::SaveXML() is writing to file now, please wait..." );
	
	FILE *plik = fopen( nazwa, "w" );
	
	fprintf( plik, "<mesh orgsize=\"%d\">\n", src.orgsize ); 
 	fprintf( plik, " <rotation x=\"%f\" y=\"%f\" z=\"%f\" />\n", src.rotacja.X(), src.rotacja.Y(), src.rotacja.Z() );
 	fprintf( plik, " <translation x=\"%f\" y=\"%f\" z=\"%f\" />\n", src.translacja.X(), src.translacja.Y(), src.translacja.Z() );
 	fprintf( plik, " <scale>1.0</scale>\n" );

	fprintf( plik, " <basemesh>\n" );

	fprintf( plik, "  <vertices count=\"%d\">\n", src.vertices().size() );
	size_t j=0;
	for ( CMesh::Vertices::iterator itv=src.vertices().begin(); itv!=src.vertices().end(); itv++ )
	{
		fprintf( plik, "   <vertex id=\"%d\" x=\"%f\" y=\"%f\" z=\"%f\" />\n", j, itv->X(), itv->Y(), itv->Z() );
		j++;
	}
	fprintf( plik, "  </vertices>\n" );

	fprintf( plik, "  <faces count=\"%d\">\n", src.faces().size() );
	for ( CMesh::Faces::iterator itf=src.faces().begin(); itf!=src.faces().end(); itf++ )
	{
		fprintf( plik, "   <face a=\"%d\" b=\"%d\" c=\"%d\" />\n", itf->A(), itf->B(), itf->C() );
	}
	fprintf( plik, "  </faces>\n" );

	fprintf( plik, " </basemesh>\n" );

	fprintf( plik, " <texture>\n" );
	fprintf( plik, "  <file>%s</file>\n", src.TexInfo.absoluteFilePathW().c_str() );

	fprintf( plik, "  <texindexes count=\"%d\">\n", src.texindex.size() );
	for ( CArrayOfTIndex::iterator iti=src.texindex.begin(); iti!=src.texindex.end(); iti++ )
	{
		fprintf( plik, "   <ti a=\"%d\" b=\"%d\" c=\"%d\" />\n", iti->a, iti->b, iti->c );
	}
	fprintf( plik, "  </texindexes>\n" );

	fprintf( plik, "  <texcoords count=\"%d\">\n", src.texcoord.size() );
	j=0;
	for ( CArrayOfTCoord::iterator itc=src.texcoord.begin(); itc!=src.texcoord.end(); itc++ )
	{
		fprintf( plik, "    <tc id=\"%d\" s=\"%f\" t=\"%f\" />\n", j, itc->s, itc->t );
		j++;
	}
	fprintf( plik, "  </texcoords>\n" );

	fprintf( plik, " </texture>\n" );

	fprintf( plik, " <vsplits count=\"%d\">\n", src.vsplits.size() );

	j=0;
	for ( CArrayOfVsplits::iterator its=src.vsplits.begin(); its!=src.vsplits.end(); its++ )
	{
		fprintf( plik, "  <vsplit id=\"%d\">\n", j++ );
		fprintf( plik, "   <v1 id=\"%d\" x=\"%f\" y=\"%f\" z=\"%f\" />\n", its->i1, its->v1.X(), its->v1.Y(), its->v1.Z() );
		fprintf( plik, "   <v2 id=\"%d\" x=\"%f\" y=\"%f\" z=\"%f\" />\n", its->i2, its->v2.X(), its->v2.Y(), its->v2.Z() );

		//---------------------------------

		if ( 0 != its->delFcs.size() )
		{
			fprintf( plik, "   <delFcs count=\"%d\">\n", its->delFcs.size() );

			for ( _vsplit::Faces::iterator itfd=its->delFcs.begin(); itfd!=its->delFcs.end(); itfd++ )
				fprintf( plik, "    <face a=\"%d\" b=\"%d\" c=\"%d\" />\n", itfd->A(), itfd->B(), itfd->C() );

			fprintf( plik, "   </delFcs>\n" );
		}
		else
		{
//			fprintf( plik, "   <delFcs count=\"0\" />\n" );
		}

		//---------------------------------

		if ( 0 != its->chgFcs.size() )
		{
			fprintf( plik, "   <chgFcs count=\"%d\">\n", its->chgFcs.size() );

			for ( _vsplit::Faces::iterator itfc=its->chgFcs.begin(); itfc!=its->chgFcs.end(); itfc++ )
				fprintf( plik, "    <face a=\"%d\" b=\"%d\" c=\"%d\" />\n", itfc->A(), itfc->B(), itfc->C() );

			fprintf( plik, "   </chgFcs>\n" );
		}
		else
		{
//			fprintf( plik, "   <chgFcs count=\"0\" />\n" );
		}

		//---------------------------------

		if ( 0 != its->delTI.size() )
		{
			fprintf( plik, "   <delTI count=\"%d\">\n", its->delTI.size() );

			for ( CArrayOfTIndex::iterator itti=its->delTI.begin(); itti!=its->delTI.end(); itti++ )
				fprintf( plik, "    <ti a=\"%d\" b=\"%d\" c=\"%d\" />\n", itti->a, itti->b, itti->c );

			fprintf( plik, "   </delTI>\n" );
		}
		else
		{
//			fprintf( plik, "   <delTI count=\"0\" />\n" );
		}

		//---------------------------------

		if ( 0 != its->delTC.size() )
		{
			fprintf( plik, "   <delTC count=\"%d\">\n", its->delTC.size() );

			for ( _vsplit::TextureCoords::iterator ittc=its->delTC.begin(); ittc!=its->delTC.end(); ittc++ )
				fprintf( plik, "    <tc id=\"%d\" s=\"%f\" t=\"%f\" />\n", ittc->first, ittc->second.s, ittc->second.t );

			fprintf( plik, "   </delTC>\n" );
		}
		else
		{
//			fprintf( plik, "   <delTC count=\"0\" />\n" );
		}

		//---------------------------------

		if ( 0 != its->chgTC.size() )
		{
			fprintf( plik, "   <chgTC count=\"%d\">\n", its->chgTC.size() );

			for ( _vsplit::TextureCoords::iterator ittc=its->chgTC.begin(); ittc!=its->chgTC.end(); ittc++ )
				fprintf( plik, "    <tc id=\"%d\" s=\"%f\" t=\"%f\" />\n", ittc->first, ittc->second.s, ittc->second.t );

			fprintf( plik, "   </chgTC>\n" );
		}
		else
		{
//			fprintf( plik, "   <chgTC count=\"0\" />\n" );
		}

		//---------------------------------

		fprintf( plik, "  </vsplit>\n" );
	}

	fprintf( plik, " </vsplits>\n" );
	fprintf( plik, "</mesh>\n" );
	fclose( plik );
}
*/