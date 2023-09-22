#include "RAWdicomData.h"

inline RAWdicomData::RAWdicomData() {
	m_cols = m_rows = m_lays = 0;
	m_minC = m_minR = m_minL = 0;
	m_data = nullptr;
	pixelRepresentation = 0;
	rescaleIntercept = 0;
	rescaleSlope = 1;
	//windowCenter = 2048;
	//windowWidth = 2048;
}

inline RAWdicomData::RAWdicomData(const RAWdicomData& src, int flag) : RAWdicomData() {
	copyFrom(src);
}

inline RAWdicomData::RAWdicomData(short cols, short rows, short lays, short minC, short minR, short minL) {
	init(cols, rows, lays, minC, minR, minL);
}

inline RAWdicomData::~RAWdicomData() {
	if (m_data != nullptr)
		delete[] m_data;
}

inline bool RAWdicomData::init(short cols, short rows, short lays, short minC, short minR, short minL) {
	m_cols = cols;
	m_rows = rows;
	m_lays = lays;
	m_minC = minC;
	m_minR = minR;
	m_minL = minL;

	rescaleIntercept = 0;
	rescaleSlope = 1;
	windowCenter = 2048;
	windowWidth = 2048;

	size_t size = (size_t)m_lays * m_rows * m_cols;

	if (size > 0) {
		m_data = new WORD[size];
		return true;
	}

	m_cols = m_rows = m_lays = 0;
	m_data = nullptr;
	return false;
}

RAWdicomData& RAWdicomData::operator=(const RAWdicomData& src)
{
	copyFrom(src);
	return *this;
}

void RAWdicomData::copyFrom(const RAWdicomData& src, int flag)
{
	//m_data = nullptr;

	m_cols = src.m_cols;
	m_rows = src.m_rows;
	m_lays = src.m_lays;

	m_minC = src.m_minC;
	m_minR = src.m_minR;
	m_minL = src.m_minL;

	m_imgPos = src.m_imgPos;

	size_t size = (size_t)m_lays * m_rows * m_cols;

	switch (flag) {
	case 99:

		if (size > 0) {
			//m_data = new WORD[size];

			for (int l = 0; l < m_lays; l++)
				for (int r = 0; r < m_rows; r++)
					for (int c = 0; c < m_cols; c++)
					{
						m_data[idx(c, r, l)] = ((RAWdicomData&)src).getRescaled(c, r, l, 0, 0xffff);
					}
		}

		break;

	case 0: // normalne kopiowanie
	default:
		pixelRepresentation = src.pixelRepresentation;
		rescaleIntercept = src.rescaleIntercept;
		rescaleSlope = src.rescaleSlope;
		windowCenter = src.windowCenter;
		windowWidth = src.windowWidth;

		//=========================================//


		if (size > 0) {
			m_data = new WORD[size];
			memcpy(m_data, src.m_data, size * sizeof(WORD));
		}
		break;
	}
}

WORD RAWdicomData::getRescaled(short x, short y, short z, WORD ymin, WORD ymax) {
	int pix = m_data[idx(x, y, z)];

	if (pixelRepresentation == 1)
	{
		pix = *(int16_t*)&(m_data[idx(x, y, z)]);
	}

	pix *= rescaleSlope;
	pix += rescaleIntercept;

	WORD result;

	if (pix <= (double)windowCenter - 0.5 - (windowWidth - 1) / 2)
		result = ymin;
	else if (pix > (double)windowCenter - 0.5 + (windowWidth - 1) / 2)
		result = ymax;
	else
		result = ((pix - ((double)windowCenter - 0.5)) / ((double)windowWidth - 1) + 0.5) * (ymax - ymin) + ymin;

	return result;
}
