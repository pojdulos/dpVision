#pragma once
#include "Annotation.h"
#include "Punkt3D.h"
#include "Wektor3D.h"
#include "RGBA.h"
#include "Model3D.h"
#include <list>

typedef std::list<INDEX_TYPE> ListOfVIndices;

class DPVISION_EXPORT CAnnotationVPath : public CAnnotation, public ListOfVIndices
{
public:
	//ListOfVIndices m_list;
	double m_length;

	CAnnotationVPath(std::shared_ptr<CBaseObject> m = nullptr) :CAnnotation(m), ListOfVIndices()
	{
		setLabel("v_path");
	};

	CAnnotationVPath(const CAnnotationVPath& p) :CAnnotation(p), ListOfVIndices(p)
	{
		setLabel("v_path");
		//m_list = p.m_list;
		m_length = p.m_length;
	};

	CAnnotationVPath(ListOfVIndices& list, double d, std::shared_ptr<CBaseObject> m = nullptr) :CAnnotation(m), ListOfVIndices(list)
	{
		setLabel("v_path");
		//m_list = list;
		m_length = d;
	};

	CAnnotationVPath(ListOfVIndices& list, std::shared_ptr<CBaseObject> m);
	
	~CAnnotationVPath() { this->clear(); };
	
	virtual inline void clear() override
	{
		CAnnotation::clear();
		ListOfVIndices::clear();
		//m_list.clear();
	};


	virtual int type() { return CAnnotation::VPATH; }

	double getLength() { return m_length; };
	virtual std::wstring getTypeWSTR() { return L"v_path"; };
	virtual std::wstring getInfoRow();
	virtual void renderSelf() override;
	ListOfVIndices::iterator cyclicNext(ListOfVIndices::iterator current);
	ListOfVIndices::iterator cyclicPrev(ListOfVIndices::iterator current);
};

//class DPVISION_EXPORT CPathsMap : public std::map<int, CAnnotationPath*> {};
