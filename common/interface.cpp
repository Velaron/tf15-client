//========= Copyright � 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include <string.h>
#include <stdlib.h>

#include "interface.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dlfcn.h>
#include <unistd.h>
#include <stdio.h>
#endif

InterfaceReg *InterfaceReg::s_pInterfaceRegs = NULL;

InterfaceReg::InterfaceReg( InstantiateInterfaceFn fn, const char *pName ) : m_pName(pName)
{
	m_CreateFn = fn;
	m_pNext = s_pInterfaceRegs;
	s_pInterfaceRegs = this;
}

EXPORT_FUNCTION IBaseInterface *CreateInterface( const char *pName, int *pReturnCode )
{
	InterfaceReg *pCur;
	
	for( pCur=InterfaceReg::s_pInterfaceRegs; pCur; pCur = pCur->m_pNext )
	{
		if( !strcmp( pCur->m_pName, pName ) )
		{
			if ( pReturnCode )
				*pReturnCode = IFACE_OK;
			return pCur->m_CreateFn();
		}
	}
	
	if( pReturnCode )
		*pReturnCode = IFACE_FAILED;

	return NULL;	
}

CreateInterfaceFn Sys_GetFactoryThis( void )
{
	return CreateInterface;
}

#if defined(_WIN32)
HINTERFACEMODULE Sys_LoadModule( const char *pModuleName )
{
	return (HINTERFACEMODULE)LoadLibraryA( pModuleName );
}
#elif defined(__ANDROID__)
HINTERFACEMODULE Sys_LoadModule( const char *pModuleName )
{
	char szPath[1024];

	snprintf( szPath, 1024, "%s/%s", getenv("XASH3D_GAMELIBDIR"), pModuleName );

	return (HINTERFACEMODULE)dlopen( szPath, RTLD_LAZY );
}
#else
HINTERFACEMODULE Sys_LoadModule( const char *pModuleName )
{
	return (HINTERFACEMODULE)dlopen( pModuleName, RTLD_NOW );
}
#endif

#if defined(_WIN32)
void Sys_FreeModule( HINTERFACEMODULE hModule )
{
	if( !hModule )
		return;

	FreeLibrary( (HMODULE)hModule );
}
#else
void Sys_FreeModule( HINTERFACEMODULE hModule )
{
	if( !hModule )
		return;

	dlclose( (void *)hModule );
}
#endif

#if defined(_WIN32)
CreateInterfaceFn Sys_GetFactory( HINTERFACEMODULE hModule )
{
	if( !hModule )
		return NULL;

	return (CreateInterfaceFn)GetProcAddress( (HMODULE)hModule, CREATEINTERFACE_PROCNAME );
}
#else
CreateInterfaceFn Sys_GetFactory( HINTERFACEMODULE hModule )
{
	if( !hModule )
		return NULL;

	return (CreateInterfaceFn)dlsym( (void *)hModule, CREATEINTERFACE_PROCNAME );
}
#endif
