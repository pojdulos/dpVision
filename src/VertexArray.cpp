
#include "VertexArray.h"


CVertexArray::CVertexArray(void)
{
	iLimit = 0;
	iSize = 0;
	vBaza.clear();
}

CVertexArray::~CVertexArray(void)
{
	//std::vector<SVertexBlock>::iterator v;
	//for ( v = vBaza.begin(); v != vBaza.end(); v++ )
	//{
	//	delete (*v);
	//}
	vBaza.clear();
	iSize = 0;
	iLimit = 0;
}


CVertex& CVertexArray::operator[]( unsigned long i )
{
	unsigned long ib, iv;

	if ( i > iSize )
	{
		return vBaza[0].d[0];
	}

	ib = i / BLOCK_SIZE;
	iv = i % BLOCK_SIZE;

	return ( vBaza[ ib ].d[ iv ] );
}

void CVertexArray::push_back( CVertex v )
{
	unsigned long iv, ib;

	if ( iSize >= iLimit )
	{
		SVertexBlock blok;// = new SVertexBlock;
		vBaza.push_back( blok );
		iLimit += BLOCK_SIZE;
	}

	iv = iSize % BLOCK_SIZE;
	ib = iSize / BLOCK_SIZE;
	vBaza[ib].d[ iv ] = v;
	iSize++;
}

void CVertexArray::pop_back()
{
	iSize--;
}


void CVertexArray::clear()
{
	//std::vector<SVertexBlock>::iterator v;
	//for ( v = vBaza.begin(); v != vBaza.end(); v++ )
	//{
	//	delete (*v);
	//}
	vBaza.clear();
	iSize = 0;
	iLimit = 0;
}
