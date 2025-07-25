#pragma once

#include "dll_global.h"

#include <vector>
#include <map>
#include <typeinfo>
#include <set>

class DPVISION_EXPORT CVariantMap
{
	std::map< std::string, void* > m_map;

public:
	template<typename _T> void add(const std::string key)
	{
		m_map[key] = (void*) new _T;
	};

	template<typename _T> void del(const std::string key)
	{
		delete (_T*) m_map[key];
		m_map.erase(key);
	};

	template<typename _T> _T& at(const std::string s)
	{
		return  *(_T*)m_map[s];
	};


	template<typename _T> void addVector(const std::string key)
	{
		m_map[key] = (void*) new std::vector<_T>;
	};

	template<typename _T> void delVector(const std::string key)
	{
		delete (std::vector<_T>*) m_map[key];
		m_map.erase(key);
	};

	template<typename _T> std::vector<_T>& vector(const std::string s) 
	{
		return  *(std::vector<_T>*)m_map[s];
	};

	template<typename _T> void addSet(const std::string key)
	{
		m_map[key] = (void*) new std::set<_T>;
	};

	template<typename _T> void delSet(const std::string key)
	{
		delete (std::set<_T>*) m_map[key];
		m_map.erase(key);
	};

	template<typename _T> std::set<_T>& set(const std::string s)
	{
		return  *(std::set<_T>*)m_map[s];
	};
};

