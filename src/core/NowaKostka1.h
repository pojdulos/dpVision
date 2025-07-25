#pragma once
#ifndef _NOWA_KOSTKA_1_
#define _NOWA_KOSTKA_1_

#include <limits>
#include "Utilities.h"

class DPVISION_EXPORT NowaKostka1
{
public:
	typedef uint16_t Type;

private:
	Type* m_data;

	inline size_t idx(size_t c, size_t r, size_t l) { return (l * m_cols * m_rows) + (r * m_cols) + c; }

public:
	size_t m_cols, m_rows, m_lays;
	std::vector<CPoint3d> m_imgPos;
	std::vector<double> m_gantra;
	CPoint3d m_voxelSize;

	inline explicit NowaKostka1() : m_cols(0), m_rows(0), m_lays(0), m_data(nullptr) {}
	inline explicit NowaKostka1(const NowaKostka1& src) : NowaKostka1() { copyFrom(src); }
	inline explicit NowaKostka1(size_t cols, size_t rows, size_t lays) { init(cols, rows, lays); }
	~NowaKostka1() {
		if (m_data != nullptr) delete[] m_data;
		m_imgPos.clear();
	}
	
	inline Type minAcceptedValue() { return std::numeric_limits<Type>::min(); }
	inline Type maxAcceptedValue() { return std::numeric_limits<Type>::max(); }

	inline bool isValid() { return m_data != nullptr; }
	inline size_t size() { return m_lays * m_rows * m_cols; }

	inline void clear() {
		if (m_data != nullptr) delete[] m_data;
		m_imgPos.clear();
		m_cols = m_rows = m_lays = 0;
	}

	bool init(size_t cols, size_t rows, size_t lays) {
		if (m_data != nullptr) delete[] m_data;
		m_voxelSize.Set(1.0, 1.0, 1.0);
		size_t _size = cols * rows * lays;
		if (_size > 0) {
			m_cols = cols;
			m_rows = rows;
			m_lays = lays;

			for (int l=0; l<lays; l++)
			{
				m_imgPos.push_back(CPoint3d(0,0,l));
				m_gantra.push_back(0);
			}

			m_data = new Type[_size];
			return true;
		}

		m_cols = m_rows = m_lays = 0;
		m_data = nullptr;
		return false;
	}

	NowaKostka1& operator=(const NowaKostka1& src) {
		copyFrom(src);
		return *this;
	}

	void setAllData(Type val) {
		size_t _size = size();
		memset(m_data, val, _size * sizeof(Type));
	}

	void copyFrom(const NowaKostka1& src) {
		if (m_data != nullptr) delete[] m_data;

		size_t _size = ((NowaKostka1&)src).size();

		if (_size > 0) {
			m_cols = src.m_cols;
			m_rows = src.m_rows;
			m_lays = src.m_lays;

			m_imgPos = src.m_imgPos;

			m_data = new Type[_size];
			memcpy(m_data, src.m_data, _size * sizeof(Type));
		}
	}

	inline Type* layer(size_t z) { return &m_data[idx(0, 0, z)]; }

	//inline Type& operator()(size_t x, size_t y, size_t z) { return m_pairs[idx(x, y, z)]; }
	
	inline Type get(size_t x, size_t y, size_t z) { return m_data[idx(x, y, z)]; }
	inline Type get(size_t adr) { return m_data[adr]; }

	CPoint3d getRealXYZ(size_t column, size_t row, size_t layer, double gantra)
	{
		double realX = m_imgPos[layer].x + (m_voxelSize.x * column);
		double realY = m_imgPos[layer].y + (m_voxelSize.y * row - m_voxelSize.z * layer * tan(deg2rad(gantra)));
		double realZ = m_imgPos[layer].z;// +(m_voxelSize.z * layer);

		return CPoint3d(realX, realY, realZ);
	}

	inline CPoint3d getRealXYZ(CPoint3s point, double gantra) { return getRealXYZ(point.x, point.y, point.z, gantra); }

	inline CPoint3d getRealXYZ(size_t column, size_t row, size_t layer) { return getRealXYZ(column, row, layer, m_gantra[layer]); }
	inline CPoint3d getRealXYZ(CPoint3s point) { return getRealXYZ(point.x, point.y, point.z, m_gantra[point.z]); }

	void set(size_t c, size_t r, size_t l, Type d) { m_data[idx(c, r, l)] = d; }
	void set(size_t adr, Type d) { m_data[adr] = d; }
};


#endif /* _VOLTK_H_BY_DP_INCLUDED_ */