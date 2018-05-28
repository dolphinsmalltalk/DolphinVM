#include "ist.h"

#ifndef _DEBUG
	#pragma optimize("s", on)
	#pragma auto_inline(off)
#endif
#include "segdefs.h"

#pragma code_seg(DEBUG_SEG)
#include "TraceStream.h"

std::wostream& operator<<(std::wostream& stream, const SYSTEMTIME& st)
{
	wchar_t buf[128];
	::GetTimeFormatW(LOCALE_SYSTEM_DEFAULT, TIME_FORCE24HOURFORMAT, &st, NULL, buf, 128);
	stream <<  buf << L", ";
	::GetDateFormatW(LOCALE_SYSTEM_DEFAULT, DATE_SHORTDATE, &st, NULL, buf, 128);
	return stream << buf;
}
