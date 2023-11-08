#pragma once
#ifndef _VSPLIT_H_BY_DP_INCLUDED_
#define _VSPLIT_H_BY_DP_INCLUDED_

#include "Face.h"
#include "TCoord.h"
#include "TIndex.h"


typedef struct _vsplit
{
	typedef std::vector<CFace> Faces;
	typedef std::vector<CTIndex> TextureIndexes;
	typedef std::pair<size_t,CTCoord> TCoord;
	typedef std::vector<TCoord> TextureCoords;

	//_vsplit::~_vsplit()
	//{
	//	delFcs.clear();
	//	chgFcs.clear();
	//	delTC.clear();
	//	delTI.clear();
	//};

	size_t i1, i2;
	CVertex v1, v2;
	Faces delFcs;
	Faces chgFcs;

	TextureCoords delTC;
	TextureCoords chgTC;
	TextureIndexes delTI;
} SVsplit;

//class DPVISION_EXPORT CArrayOfVsplits : public std::vector<SVsplit> {};

#endif /* _VSPLIT_H_BY_DP_INCLUDED_ */