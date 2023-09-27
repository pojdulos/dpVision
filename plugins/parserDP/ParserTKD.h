#pragma once
#include "dll_global.h"
#include "Parser.h"

#define TESTVOLTK
//#define TESTPOLYVOX

class CVolTK;

class DPVISION_DLL_API CParserTKD : public CParser
{

public:
	CParserTKD(void);
	~CParserTKD(void);

	bool is16bit;

	int m_winTX, m_winTY;
	int m_winBX, m_winBY;

#if defined(TESTVOLTK)

	//size_t readVoxels(CVolTK* pVolTK, WORD readTreshold, WORD cleanTreshold = 0);
	void cleanLayerW(int w, int h, int depth, int layerId, std::vector<void*>& maskMap, WORD treshold);
	void cleanLayerB(int w, int h, int depth, int layerId, std::vector<void*>& maskMap, BYTE treshold);

	size_t readVoxelsBACK(CVolTK* pVolTK, int depth, std::string prefix, std::string ext, int min, int max, WORD treshold, bool cleanIt);
	
	size_t readVoxels(CVolTK* pVolTK, int depth, std::string prefix, std::string ext, int min, int max, WORD lowTreshold, WORD upTreshold, bool cleanIt);

	size_t readVoxels2(CVolTK* pVolTK, int depth, std::string prefix, std::string ext, int min, int max, WORD lowTreshold, WORD upTreshold, bool cleanIt);
	
	//size_t removeInternalVoxels(int w, int h, std::vector<std::vector<WORD>*>& maskMap, WORD threshold);
#else
	std::vector<std::vector<WORD>*> maskMap;
	std::set<WORD> treshold;

	CTriple<double> readVoxelsOLD(int& w, int& h, std::vector<std::vector<WORD>*>& maskMap, WORD treshold, WORD& minData, WORD& maxData);
	size_t removeInternalVoxelsOLD(int w, int h, std::vector<std::vector<WORD>*>& maskMap, WORD treshold);

#endif

	virtual size_t Run() override;
};
