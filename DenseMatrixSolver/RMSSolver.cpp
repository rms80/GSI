// TempDLL.cpp : Defines the entry point for the DLL application.
//

#include <windows.h>
#include "Architecture.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	// detect processor type
	rmssolver::Architecture::Type eArchType = rmssolver::Architecture::Get()->GetType();
	OutputDebugString("[RMSSolver::DllMain] Architecture is ");
	OutputDebugString( rmssolver::Architecture::GetString(eArchType) );
	OutputDebugString("\n");

    return TRUE;
}

