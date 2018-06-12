// DLL interface to SHA class.
// These can be removed if the SHA class is moved to the VM. 

#include "SHA.h"

// ================ Interface

extern "C" 
{  
	__declspec( dllexport ) SHA* __cdecl SHACreate();
	__declspec( dllexport ) void __cdecl SHAHashBuffer( SHA* sha, BYTE* buffer, DWORD bufferSize );
	__declspec( dllexport ) void __cdecl SHAGetHash( SHA* sha, BYTE* hash );
	__declspec( dllexport ) void __cdecl SHADestroy( SHA* sha );
}

// ================ Implementation, route to SHA class.

__declspec( dllexport ) SHA* __cdecl SHACreate()
{
	return new SHA;
}

__declspec( dllexport ) void __cdecl SHAHashBuffer( SHA* sha, BYTE* buffer, DWORD bufferSize )
{
	sha->HashBuffer( buffer, bufferSize );
}

__declspec( dllexport ) void __cdecl SHAGetHash( SHA* sha, BYTE* hash )
{
	sha->GetHash( hash );
}

__declspec( dllexport ) void __cdecl SHADestroy( SHA* sha )
{
	delete sha;
}
