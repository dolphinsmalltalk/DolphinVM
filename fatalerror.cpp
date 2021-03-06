#include "ist.h"
#include "vmexcept.h"

#ifndef _DEBUG
	#pragma optimize("s", on)
	#pragma auto_inline(off)
#endif

#include "rc_vm.h"

extern int __stdcall DolphinMessage(UINT flags, const wchar_t* msg);
extern void __stdcall DolphinFatalExit(int exitCode, const wchar_t* msg);

#ifndef VM

int __stdcall DolphinMessage(UINT flags, const wchar_t* msg)
{
	wchar_t szCaption[512];
	HMODULE hExe = GetModuleHandle(NULL);
	if (!::LoadStringW(hExe, IDS_APP_TITLE, szCaption, sizeof(szCaption)-1))
		GetModuleFileNameW(hExe, szCaption, sizeof(szCaption));
	return  ::MessageBoxW(NULL, msg, szCaption, flags|MB_TASKMODAL);
}

#endif

int __cdecl DolphinMessageBoxV(const wchar_t* szFormat, UINT flags, va_list args)
{
	wchar_t* buf;
	::FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_STRING,
						szFormat, 0, 0, LPWSTR(&buf), 0, &args);

	int result = DolphinMessage(flags, buf);

	::LocalFree(buf);
	
	return result;
}

int __cdecl DolphinMessageBox(int nPromptId, UINT flags, ...)
{
	va_list args;
	va_start(args, flags);
	wchar_t szPrompt[512];
	::LoadStringW(GetResLibHandle(), nPromptId, szPrompt, sizeof(szPrompt)-1);
	int result = DolphinMessageBoxV(szPrompt, flags, args);
	va_end(args);
	return result;
}

#pragma code_seg()

LPCWSTR __stdcall GetLastErrorText()
{
	return GetErrorText(::GetLastError());
}

// Answer some suitable text for the last system error
LPCWSTR __stdcall GetErrorText(DWORD win32ErrorCode)
{
	LPWSTR buf;
	::FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		0, win32ErrorCode, 0, LPWSTR(&buf), 0, 0);
	return buf;
}

void __stdcall vtrace(const wchar_t* szFormat, va_list args)
{
	wchar_t buf[1024];
	::vswprintf_s(buf, sizeof(buf)/sizeof(buf[0]), szFormat, args);
	::OutputDebugStringW(buf);
}

void __cdecl trace(const wchar_t* szFormat, ...)
{
	va_list args;
	va_start(args, szFormat);
	vtrace(szFormat, args);
	va_end(args);
}

void __cdecl trace(int nPrompt, ...)
{
	wchar_t szFormat[256];
	::LoadStringW(GetResLibHandle(), nPrompt, szFormat, sizeof(szFormat) - 1);

	va_list args;
	va_start(args, szFormat);
	vtrace(szFormat, args);
	va_end(args);
}

HRESULT __stdcall ReportErrorV(int nPrompt, HRESULT hr, va_list args)
{
	wchar_t szFormat[256];
	::LoadStringW(GetResLibHandle(), nPrompt, szFormat, sizeof(szFormat)-1);

	DolphinMessageBoxV(szFormat, MB_SETFOREGROUND|MB_ICONHAND|MB_SYSTEMMODAL, args);

	return hr;
}

HRESULT __cdecl ReportError(int nPrompt, ...)
{
	va_list args;
	va_start(args, nPrompt);

	HRESULT hr = ReportErrorV(nPrompt, MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 2000+nPrompt), args);
	va_end(args);

	return hr;
}

HRESULT __cdecl ReportWin32Error(int nPrompt, DWORD errorCode, LPCWSTR arg)
{
	LPCWSTR errorText = GetErrorText(errorCode);
	HRESULT hr = ReportError(nPrompt, errorCode, errorText, arg);
	::LocalFree((HLOCAL)errorText);
	return hr;
}

void __cdecl RaiseFatalError(int nCode, int nArgs, ...)
{
	va_list args;
	va_start(args, nArgs);
	::RaiseException(MAKE_CUST_SCODE(SEVERITY_ERROR, FACILITY_NULL, nCode), EXCEPTION_NONCONTINUABLE, nArgs, (CONST ULONG_PTR*)args);
}

void __stdcall FatalException(const EXCEPTION_RECORD& exRec)
{
	int nPrompt = exRec.ExceptionCode & 0x2FF;

	wchar_t szFormat[256];
	::LoadStringW(GetResLibHandle(), nPrompt, szFormat, sizeof(szFormat)-1);

	LPWSTR buf;
	::FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_STRING|FORMAT_MESSAGE_ARGUMENT_ARRAY,
						szFormat, 0, 0, LPWSTR(&buf), 0, (va_list*)exRec.ExceptionInformation);


	DolphinFatalExit(exRec.ExceptionCode, buf);

	::LocalFree(buf);
}

#include "vmexcept.h"

void __stdcall DolphinExit(int exitCode)
{
	RaiseFatalError(IDP_EXIT, 1, exitCode);
}

#ifndef VM

void __stdcall DolphinFatalExit(int /*exitCode*/, const wchar_t* msg)
{
	FatalAppExitW(0, msg);
}

#endif