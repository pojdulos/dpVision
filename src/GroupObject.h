#pragma once
#include "Model3D.h"
class __declspec(dllexport) CGroupObject : public CModel3D
{
	int m_selectedChildId;

public:
	CGroupObject();
	~CGroupObject();

	virtual inline int type() { return CObject::Type::GROUP; };


	virtual const bool select(const bool b);

	const bool selectChild( int id, bool b );
	CBaseObject *getSelectedChild();
};

