#include "SHA.h"

#pragma comment( lib, "BCrypt.lib" )

// The static algorithm handle allocated once on first use and never freed
// because this is an expensive operation, and the class only supports SHA1.
BCRYPT_ALG_HANDLE SHA::algHandle = NULL;

SHA::SHA():
	hashHandle( NULL )
{
	if( algHandle == NULL )
	{
		// Open a handle to the SHA1 argorithm.
		NTSTATUS status = BCryptOpenAlgorithmProvider( &algHandle, BCRYPT_SHA1_ALGORITHM, NULL,  0 );
		if( ! BCRYPT_SUCCESS( status ) )
			::RaiseException( status, 0, 0, NULL );
	}

	Create();
}

SHA::~SHA()
{
	Destroy();
}

// Initialize new hash.

void SHA::Create()
{
	Destroy(); // Free if already initialized.

	NTSTATUS status = BCryptCreateHash( algHandle, &hashHandle, NULL, 0, NULL, 0, 0 );
	if( ! BCRYPT_SUCCESS( status ) )
		::RaiseException( status, 0, 0, NULL );
}

// Free current hash.

void SHA::Destroy()
{
	if( hashHandle != NULL )
	{
        BCryptDestroyHash( hashHandle );
		hashHandle = NULL;
	}
}

// Update hash with new data.

void SHA::HashBuffer( BYTE* buffer, int bufferSize )
{
	NTSTATUS status = BCryptHashData( hashHandle, buffer, bufferSize, 0 );
	if( ! BCRYPT_SUCCESS( status ) )
		::RaiseException( status, 0, 0, NULL );
}

// Get hash for previously hased data.

void SHA::GetHash( BYTE* hash )
{
    NTSTATUS status = BCryptFinishHash( hashHandle, hash, hashSize, 0 );
	if( ! BCRYPT_SUCCESS( status ) )
		::RaiseException( status, 0, 0, NULL );
}
