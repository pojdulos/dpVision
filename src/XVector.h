#pragma once
#ifndef XVECTOR_H
#define XVECTOR_H

template<typename _T> static inline _T sqr(_T x) { return x * x; }

#include "Triple.h"

template<class _Typ, int _Dim>
	class XVector
{
public:
	XVector() : m_data() {}

	XVector(const XVector& e)
	{
		for (int i = 0; i < _Dim; i++)
		{
			m_data[i] = e.m_data[i];
		}
	}

	XVector(const CTriple<_Typ>& e)
	{
		for (int i = 0; i < 3; i++)
		{
			m_data[i] = e.v[i];
		}
	}

	XVector(_Typ* data)
	{
		for (int i = 0; i < _Dim; i++)
		{
			m_data[i] = data[i];
		}
	}

	_Typ& operator[](int index)
	{
		assert(0 <= index && index < _Dim);
		return m_data[index];
	}

	XVector operator=(XVector e)
	{
		for (int i = 0; i < _Dim; i++)
		{
			m_data[i] = e.m_data[i];
		}

		return *this;
	}

	XVector operator+=(XVector e)
	{
		for (int i = 0; i < _Dim; i++)
		{
			m_data[i] += e.m_data[i];
		}

		return *this;
	}

	XVector operator-=(XVector e)
	{
		for (int i = 0; i < _Dim; i++)
		{
			m_data[i] -= e.m_data[i];
		}

		return *this;
	}

	XVector operator/=(_Typ d)
	{
		assert(d != 0);
		for (int i = 0; i < _Dim; i++)
		{
			m_data[i] /= d;
		}

		return *this;
	}

	XVector operator*=(_Typ d)
	{
		for (int i = 0; i < _Dim; i++)
		{
			m_data[i] *= d;
		}

		return *this;
	}

	XVector operator+(XVector e)
	{
		XVector result;
		for (int i = 0; i < _Dim; i++)
		{
			result[i] = m_data[i] + e.m_data[i];
		}

		return result;
	}

	XVector operator-(XVector e)
	{
		XVector result;
		for (int i = 0; i < _Dim; i++)
		{
			result[i] = m_data[i] - e.m_data[i];
		}

		return result;
	}

	XVector operator*(_Typ d)
	{
		XVector result;
		for (int i = 0; i < _Dim; i++)
		{
			result[i] = m_data[i] * d;
		}

		return result;
	}

	XVector operator/(_Typ d)
	{
		assert(d != 0);
		XVector result;
		for (int i = 0; i < _Dim; i++)
		{
			result[i] = m_data[i] / d;
		}

		return result;
	}

	bool insideBox(XVector min, XVector max)
	{
		for (int i = 0; i < _Dim; i++)
		{
			assert(min.m_data[i] <= max.m_data[i]);

			if ((m_data[i] < min.m_data[i]) || (m_data[i] > max.m_data[i]))
			{
				return false;
			};
		}
		return true;
	}

	_Typ dist2to(const XVector& e)
	{
		double dist2 = 0.0;
		for (int i = 0; i < _Dim; i++)
		{
			dist2 += sqr((double)m_data[i] - e.m_data[i]);
		}
		return (_Typ) dist2;
	}

	inline _Typ distTo(const XVector& e)
	{
		return sqrt(dist2to(e));
	}

	/*
	* org - ray origin
	* dir - ray direction
	*/
	_Typ dist2toRay(const XVector& org, const XVector& dir)
	{
		double sum1 = 0.0;
		double sum2 = 0.0;
		for (int i = 0; i < _Dim; i++)
		{
			double d = (double)m_data[i] - org.m_data[i];
			sum1 += d * d;
			sum2 += d * dir.m_data[i];
		}
		return (_Typ)(sum1 - (sum2 * sum2));
	}

	inline _Typ distToRay(const XVector& p, const XVector& d)
	{
		return sqrt(dist2toRay(p, d));
	}

private:
	_Typ m_data[_Dim];
};

#endif