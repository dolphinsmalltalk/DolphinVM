#pragma once

// SHA1 Implementation using Windows BCrypt (CNG) API.

#include <Windows.h>
#include <BCrypt.h>

class SHA
{
public:
	SHA();
	~SHA();

	void Create();
	void Destroy();

	void HashBuffer( BYTE* buffer, int count );
	void GetHash( BYTE* digest );

private:
	const DWORD hashSize = 20;
    static BCRYPT_ALG_HANDLE algHandle;
	BCRYPT_HASH_HANDLE hashHandle;
};
