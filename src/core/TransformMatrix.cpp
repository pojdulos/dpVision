#include "TransformMatrix.h"
#include <regex>

CTransformMatrix::CTransformMatrix()
{
	setIdentity();
}

CTransformMatrix::CTransformMatrix(CTransformMatrix& m)
{
	std::memcpy(m_data, m.m_data, 16 * sizeof(double));
}

CTransformMatrix::CTransformMatrix(double d[16])
{
	std::memcpy(m_data, d, 16 * sizeof(double));
}

CTransformMatrix::~CTransformMatrix()
{
}

double& CTransformMatrix::at(int col, int row)
{
	if ((col < 0) || (col > 3) || (row < 0) || (row > 3))
		return m_data[0];
	
	return m_data[col * 4 + row];
}

std::string CTransformMatrix::toString(bool transpose)
{
	std::string str;

	if (transpose)
	{
		for (int r = 0; r < 4; r++)
			for (int c = 0; c < 4; c++)
				str.append(std::to_string(this->at(c, r)) + " ");
	}
	else
	{
		for (int c = 0; c < 4; c++)
			for (int r = 0; r < 4; r++)
				str.append(std::to_string(this->at(c, r)) + " ");
	}

	return str;
}

void CTransformMatrix::fromString(std::string matrixSTR, bool transpose)
{
	std::regex rgx("\\s+");

	std::sregex_token_iterator iter(matrixSTR.begin(), matrixSTR.end(), rgx, -1);
	std::sregex_token_iterator end;

	int row = 0;
	int col = 0;
	while (iter != end) {
		if (transpose)
			this->at(col, row) = std::stod(*iter);
		else
			this->at(row, col) = std::stod(*iter);

		++iter;

		col++;
		if (col > 3)
		{
			row++;
			col = 0;
		}
	}
}

void CTransformMatrix::setIdentity()
{
	m_data[0] = 1.0;
	m_data[1] = 0.0;
	m_data[2] = 0.0;
	m_data[3] = 0.0;

	m_data[4] = 0.0;
	m_data[5] = 1.0;
	m_data[6] = 0.0;
	m_data[7] = 0.0;

	m_data[8] = 0.0;
	m_data[9] = 0.0;
	m_data[10] = 1.0;
	m_data[11] = 0.0;

	m_data[12] = 0.0;
	m_data[13] = 0.0;
	m_data[14] = 0.0;
	m_data[15] = 1.0;
}
