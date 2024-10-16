#pragma once
#include <vector>
#include "Vertex.h"

#define BLOCK_SIZE 50

struct SVertexBlock 
{
	CVertex d[BLOCK_SIZE];
};

class CVertexArray
{
	std::vector<SVertexBlock> vBaza;
	
	unsigned long iSize;
	unsigned long iLimit;

public:
	CVertexArray(void);
	~CVertexArray(void);

	CVertex &operator[](unsigned long i);

	void push_back( CVertex v );
	void pop_back();

	unsigned long size() {return iSize;};
	//unsigned long begin() { return 0; };
	//unsigned long end() { return iSize-1; };
	void clear();
	bool empty() { return (iSize==0); };
};
