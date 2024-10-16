#pragma once

#include "Workspace.h"

class CWorkspaceFile
{
	CWorkspaceFile() {};
	~CWorkspaceFile() {};

public:
	static void loadDPW(std::wstring p, CWorkspace & wksp);
	static void loadFaro(std::wstring p, CWorkspace & wksp);
	static void saveDPW( CWorkspace &, std::wstring );
};