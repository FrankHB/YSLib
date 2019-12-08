/*
	© 2009-2016, 2018-2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YCommon.cpp
\ingroup YCLib
\brief 平台相关的公共组件无关函数与宏定义集合。
\version r2908
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-12 22:14:42 +0800
\par 修改时间:
	2019-12-04 04:21 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::YCommon
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Debug
#include <cstdlib> // for std::abort, std::system;
#include YFM_YCLib_NativeAPI // for ::swiWaitForVBlank, ::sysconf,
//	_SC_PAGESIZE, _SC_SEM_NSEMS_MAX, _SC_SEM_VALUE_MAX, _SC_SYMLOOP_MAX,
//	::fifoSendValue32, ::SYSTEM_INFO, ::GetSystemInfo;
#include <limits> // for std::numeric_limits;
#include <limits.h> // for _POSIX_SEM_NSEMS_MAX, _POSIX_SYMLOOP_MAX,
//	_POSIX_SEM_VALUE_MAX;
#if YCL_Win32
#	include YFM_Win32_YCLib_NLS // for platform_ex::UTF8ToWCS, ::STARTUPINFOW,
//	::PROCESS_INFORMATION, ::CreateProcessW, CREATE_UNICODE_ENVIRONMENT,
//	platform_ex::WaitUnique, ::GetExitCodeProcess;
#endif
#include <stdlib.h> // for ::_wsystem, ::_putenv, ::setenv;

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
uspawn(const char* cmd)
{
#if YCL_Win32
	auto wcmd(platform_ex::UTF8ToWCS(cmd));
	// NOTE: See https://blogs.msdn.microsoft.com/oldnewthing/20031210-00/?p=41553/.
	// XXX: With inspection of UCRT source (SDK version 10.0.17134.0), some MSDN
	//	documents seems to be wrong of the limit, e.g. https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/spawnvpe-wspawnvpe?view=vs-2017.
	//	The actual limit should be same to %::_wexecvp and %::CreateProcessW in
	//	current versions of Windows, i.e. 32767.
	::STARTUPINFOW startup_info{};
	::PROCESS_INFORMATION process_info{};

	startup_info.cb = sizeof(::STARTUPINFOW);
	if(::CreateProcessW(nullptr, &wcmd[0], {}, {}, true,
		CREATE_UNICODE_ENVIRONMENT, {}, {}, &startup_info, &process_info))
	{
		platform_ex::WaitUnique(process_info.hProcess);

		unsigned long exit_code(0);

		if(::GetExitCodeProcess(process_info.hProcess, &exit_code))
			// XXX: Cast expexted.
			return int(exit_code);
		YCL_Raise_Win32E("GetExitCodeProcess", yfsig);
	}
	YCL_Raise_Win32E("CreateProcessW", yfsig);
#else
	return std::system(cmd);
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


void
SetEnvironmentVariable(const char* envname, const char* envval)
{
#if YCL_DS
	yunused(envname), yunused(envval);
	ystdex::throw_error(std::errc::function_not_supported, yfsig);
#elif YCL_Win32
	// TODO: Use %::_wputenv_s when available.
	// NOTE: Only narrow enviornment is used.
	// XXX: Though not documented, %::putenv actually copies the argument.
	//	Confirmed in ucrt source. See also https://patchwork.ozlabs.org/patch/127453/.
	YCL_CallF_CAPI(, ::_putenv,
		(string(Nonnull(envname)) + '=' + Nonnull(envval)).c_str());
#else
	YCL_CallF_CAPI(, ::setenv, Nonnull(envname), Nonnull(envval), 1);
#endif
}


size_t
FetchLimit(SystemOption opt) ynothrow
{
	// NOTE: For constant expression value that is always less than max value
	//	of %size_t, there is no check needed.
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
	case SystemOption::MaxSemaphoreNumber:
#if YCL_DS
		return 0;
#elif YCL_Win32
		return size_t(-1);
#elif defined(_SC_SEM_NSEMS_MAX)
		return conf_conv(::sysconf(_SC_SEM_NSEMS_MAX));
#elif defined(_POSIX_SEM_NSEMS_MAX)
		static_assert(size_t(_POSIX_SEM_NSEMS_MAX) == _POSIX_SEM_NSEMS_MAX,
			"Invalid value found.");

		return _POSIX_SEM_NSEMS_MAX;
#else
		return 256;
#endif
	case SystemOption::MaxSemaphoreValue:
#if YCL_Win32
		static_assert(long(size_t(std::numeric_limits<long>::max()))
			== std::numeric_limits<long>::max(), "Invalid value found.");

		return size_t(std::numeric_limits<long>::max());
#elif defined(_SC_SEM_VALUE_MAX)
		return conf_conv(::sysconf(_SC_SEM_VALUE_MAX));
#elif defined(_POSIX_SEM_VALUE_MAX)
		static_assert(size_t(_POSIX_SEM_VALUE_MAX) == _POSIX_SEM_VALUE_MAX,
			"Invalid value found.");

		return _POSIX_SEM_VALUE_MAX;
#else
		return 256;
#endif
	case SystemOption::MaxSymlinkLoop:
#if YCL_DS
		return 0;
#elif defined(_SC_SYMLOOP_MAX)
		return conf_conv(::sysconf(_SC_SYMLOOP_MAX));
#elif defined(_POSIX_SYMLOOP_MAX)
		static_assert(size_t(_POSIX_SYMLOOP_MAX) == _POSIX_SYMLOOP_MAX,
			"Invalid value found.");

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

