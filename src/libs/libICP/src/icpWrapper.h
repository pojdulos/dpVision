#pragma once
#include "Vertex.h"
#include "icp.h"

class IcpWrapper : public Icp
{
	// constructor
	// input: M ....... pointer to first model point
	//        M_num ... number of model points
	//        dim   ... dimensionality of model points (2 or 3)
	IcpWrapper(double* M, const int32_t M_num, const int32_t dim) : Icp(M, M_num, dim) {};
	
	IcpWrapper( std::vector<CVertex> &M );


};

