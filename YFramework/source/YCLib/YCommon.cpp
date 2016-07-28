/*
	© 2009-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YCommon.cpp
\ingroup YCLib
\brief 平台相关的公共组件无关函数与宏定义集合。
\version r2818
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-12 22:14:42 +0800
\par 修改时间:
	2016-07-25 20:13 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::YCommon
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Debug
#include <cstdlib> // for std::abort, std::system;
#include YFM_YCLib_NativeAPI // for ::swiWaitForVBlank, _SC_SYMLOOP_MAX,
//	::fifoSendValue32, ::SYSTEM_INFO, ::GetSystemInfo;
#include <limits> // for std::numeric_limits;
#include <limits.h> // for _SC_PAGESIZE, _POSIX_SYMLOOP_MAX;
#if YCL_Win32
#	include YFM_Win32_YCLib_MinGW32
#	include <stdlib.h> // for ::_wsystem;
#	include YFM_Win32_YCLib_NLS // for platform_ex::UTF8ToWCS;
#endif

namespace platform
{

void
terminate() ynothrow
{
#if YCL_DS
	while(true)
		::swiWaitForVBlank();
#else
	std::abort();
#endif
}

int
usystem(const char* cmd)
{
#if YCL_Win32
	return ::_wsystem(platform_ex::UTF8ToWCS(cmd).c_str());
#else
	return std::system(cmd);
#endif
}


size_t
FetchLimit(SystemOption opt) ynothrow
{
	// XXX: Values greater than maximum value of %size_t would be truncated.
	const auto conf_conv([](long n){
		using res_t = ystdex::common_type_t<long, size_t>;
		return n >= 0 && res_t(n) < res_t(std::numeric_limits<size_t>::max())
			? size_t(n) : size_t(-1);
	});

	// NOTE: For POSIX-defined minimum values, if the macro is undefined,
	//	use the value specified in POSIX.1-2013 instead. See http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/limits.h.html.
	switch(opt)
	{
	case SystemOption::PageSize:
		// NOTE: Value of 0 indicates the target system does not support paging.
		//	Though POSIX requires 1 as minimum acceptable value, it is intended
		//	here. For platforms without an appropriate page size, it should be
		//	specified here explicitly, rather than relying on the system library
		//	with POSIX compliance. For example, RTEMS: https://devel.rtems.org/ticket/1216.
#if YCL_DS
		return 0;
#elif YCL_Win32
		::SYSTEM_INFO info;
		::GetSystemInfo(&info);

		return size_t(info.dwPageSize);
#elif defined(_SC_PAGESIZE)
		return conf_conv(::sysconf(_SC_PAGESIZE));
#else
		// XXX: %::getpagesize is not used. Fallback to POSIX allowed minimum
		//	value.
		return 1;
#endif
	case SystemOption::MaxSymlinkLoop:
#if YCL_DS
		return 0;
#elif defined(_SC_SYMLOOP_MAX)
		return conf_conv(::sysconf(_SC_SYMLOOP_MAX));
#elif defined(_POSIX_SYMLOOP_MAX)
		return _POSIX_SYMLOOP_MAX;
#else
		// NOTE: Windows has nothing about this concept. Win32 error
		//	%ERROR_CANT_RESOLVE_FILENAME maps to Windows NT error
		//	%STATUS_REPARSE_POINT_NOT_RESOLVED, which seems to be not
		//	necessarily related to a fixed loop limit. To keep POSIX compliant
		//	and better performance of loop detection, minimum allowed value is
		//	used. See also https://cygwin.com/ml/cygwin/2009-03/msg00335.html.
		// TODO: Make it configurable for Windows?
		return 8;
#endif
	default:
		break;
	}
#if !YCL_DS
	YTraceDe(Descriptions::Warning,
		"Configuration option '%zu' is not handled.", size_t(opt));
	errno = EINVAL;
#endif
	return size_t(-1);
}

} // namespace platform;

namespace platform_ex
{

#if YCL_DS
bool
AllowSleep(bool b)
{
	// NOTE: Synchronized with ARM7 power management.
	static bool b_sleep_enabled(true);
	const bool b_old(b_sleep_enabled);

	if(b != b_sleep_enabled)
	{
		b_sleep_enabled = b;
		::fifoSendValue32(FIFO_PM,
			b ? PM_REQ_SLEEP_ENABLE : PM_REQ_SLEEP_DISABLE);
	}
	return b_old;
}
#elif YCL_Win32

#endif

}

