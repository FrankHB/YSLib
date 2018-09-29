﻿/*
	© 2014-2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Host.cpp
\ingroup YCLib
\ingroup YCLibLimitedPlatforms
\ingroup Host
\brief YCLib 宿主平台公共扩展。
\version r799
\author FrankHB <frankhb1989@gmail.com>
\since build 492
\par 创建时间:
	2014-04-09 19:03:55 +0800
\par 修改时间:
	2018-09-20 11:15 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Host
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Host // for make_observer, platform::CallNothrow;
#include YFM_YSLib_Core_YCoreUtilities // for YSLib, FetchCachedCommandResult,
//	ystdex::underlying, FilterExceptions;
#include YFM_YCLib_NativeAPI // for YCL_TraceCallF_CAPI, ::sem_open,
//	::sem_close, ::sem_unlink, ::pipe, ToHandle, YCL_CallGlobal, isatty;
#include YFM_YCLib_FileIO // for MakePathStringW, YCL_Raise_SysE,
//	MakePathString;
#include <stdlib.h> // for ::putenv, ::setenv;
#if YCL_Win32
#	include <limits> // for std::numeric_limits;
#	include YFM_Win32_YCLib_NLS // for CloseHandle, MBCSToMBCS;
#	include YFM_Win32_YCLib_Consoles // for WConsole;
#endif
#if YF_Hosted
#	include YFM_YSLib_Core_YConsole
#	if !(YCL_Win32 || YCL_API_Has_semaphore_h)
#		error "Unsupported platform found."
#	endif
#endif

using namespace YSLib;
//! \since build 659
using YSLib::string;
//! \since build 659
using platform::string_view;

namespace platform
{

using namespace platform_ex;



} // namespace platform;

#if YF_Hosted
namespace platform_ex
{

Exception::Exception(std::error_code ec, const char* str, RecordLevel lv)
	: system_error(ec, Nonnull(str)),
	level(lv)
{}
Exception::Exception(std::error_code ec, string_view sv, RecordLevel lv)
	: system_error(ec, (Nonnull(sv.data()), string(sv))),
	level(lv)
{}
Exception::Exception(int ev, const std::error_category& ecat, const char* str,
	RecordLevel lv)
	: system_error(ev, ecat, Nonnull(str)),
	level(lv)
{}
Exception::Exception(int ev, const std::error_category& ecat, string_view sv,
	RecordLevel lv)
	: system_error(ev, ecat, (Nonnull(sv.data()), string(sv))),
	level(lv)
{}
ImplDeDtor(Exception)


#if YCL_Win32
void
HandleDelete::operator()(pointer h) const ynothrowv
{
	YCL_TraceCallF_Win32(CloseHandle, h);
}
#endif


#if !YCL_Win32
void
Semaphore::Deleter::operator()(pointer p) const ynothrowv
{
	YCL_TraceCallF_CAPI(::sem_close, static_cast<::sem_t*>(p));
	if(Referent)
		YCL_TraceCallF_CAPI(::sem_unlink, Referent->name.c_str());
}
#endif


Semaphore::Semaphore(const char* n, CountType init)
#if YCL_Win32
	: Semaphore(platform::ucast(MakePathStringW(n).c_str()), init)
#else
	: h_sem([=]{
		using platform::OpenMode;
		const auto create([=](OpenMode omode) ynothrow{
			// TODO: Allow setting of permission mode.
			return ystdex::retry_on_cond([](::sem_t* res) ynothrow{
				return res == SEM_FAILED && errno == EINTR;
			}, [=]() ynothrow{
				// NOTE: Some implementations may force shared lock when
				//	%O_CREAT is specified, e.g. https://www.gnu.org/software/libc/manual/html_node/Open_002dtime-Flags.html.
				// FIXME: Blocked. Possible race condition otherwise.
				return ::sem_open(Nonnull(n),
					int(omode), platform::DefaultPMode(), init);
			});
		});

		auto p(create(OpenMode::CreateExclusive));

		if(p != SEM_FAILED)
			name = n;
		else
		{
			// FIXME: Blocked. TOCTTOU access. The semaphore might be recreated
			//	before the call.
			if(errno == EEXIST)
				p = create(OpenMode::Create);
			// FIXME: Blocked. TOCTTOU access. The semaphore might be recreated
			//	and the creation state is not recorded properly, missing
			//	%::sem_unlink would cause leak.
			if(YB_UNLIKELY(p == SEM_FAILED))
				YCL_Raise_SysE(, "::sem_open", yfsig);
		}
		return p;
	}())
#endif
{
#if !YCL_Win32
	h_sem.get_deleter().Referent = make_observer(this);
#endif
}
Semaphore::Semaphore(const char16_t* n, CountType init)
#if YCL_Win32
	// TODO: Allow setting of security attributes.
	: h_sem(YCL_CallF_Win32(CreateSemaphore, {}, init,
		std::numeric_limits<CountType>::max(), platform::wcast(n)))
#else
	: Semaphore(platform::MakePathString(n).c_str(), init)
#endif
{}

void
Semaphore::lock()
{
#if YCL_Win32
	WaitUnique(native_handle());
#else
	platform::RetryOnInterrupted(std::bind(YCL_WrapCall_CAPI(, ::sem_wait,
		static_cast<::sem_t*>(native_handle())), yfsig));
#endif
}

bool
Semaphore::try_lock() ynothrow
{
#if YCL_Win32
	return TryWaitUnique(native_handle());
#else
	return platform::RetryOnInterrupted([this]{
		return ::sem_trywait(static_cast<::sem_t*>(native_handle()));
	}) == 0;
#endif
}

void
Semaphore::unlock() ynothrow
{
#if YCL_Win32
	const auto
		res(YCL_TraceCallF_Win32(ReleaseSemaphore, native_handle(), 1, {}));

	YAssert(res, "Invalid semaphore found.");
#else
	const auto res(YCL_TraceCallF_CAPI(::sem_post,
		static_cast<::sem_t*>(native_handle())));

	YAssert(res == 0, "Invalid semaphore found.");
#endif
	yunused(res);
}


pair<UniqueHandle, UniqueHandle>
MakePipe()
{
#	if YCL_Win32
	::HANDLE h_raw_read, h_raw_write;

	YCL_CallF_Win32(CreatePipe, &h_raw_read, &h_raw_write, {}, 0);

	UniqueHandle h_read(h_raw_read), h_write(h_raw_write);

	YCL_CallF_Win32(SetHandleInformation, h_write.get(), HANDLE_FLAG_INHERIT,
		HANDLE_FLAG_INHERIT);
	return {std::move(h_read), std::move(h_write)};
#	elif YCL_API_Has_unistd_h
	int fds[2];

	// TODO: Check whether '::socketpair' is available.
	YCL_CallF_CAPI(, ::pipe, fds);

	auto pr(make_pair(UniqueHandle(fds[0]), UniqueHandle(fds[1])));
	auto check([](UniqueHandle& h, const char* msg) YB_NONNULL(3){
		// NOTE: %O_NONBLOCK is initially cleared on ::pipe results.
		//	See http://pubs.opengroup.org/onlinepubs/9699919799/.
		if(h)
			h->SetNonblocking();
		else
			ystdex::throw_error(std::errc::bad_file_descriptor, msg);
	});

	check(pr.first, "Failed making pipe for reading."),
	check(pr.second, "Failed making pipe for writing.");
	return pr;
#	else
#	error "Unsupported platform found."
#	endif
}


#	if YCL_Win32
string
DecodeArg(const char* str)
{
	return MBCSToMBCS(str, CP_ACP, CP_UTF8);
}

string
EncodeArg(const char* str)
{
	return MBCSToMBCS(str);
}
#	endif


#if !YCL_Android
//! \since build 755
namespace
{

template<typename _func, typename _tPointer, typename... _tParams>
bool
CallTe(_func pmf, _tPointer& p, _tParams&... args)
{
	return ystdex::call_value_or(ystdex::bind1(pmf, std::ref(args)...), p);
}

} // unnamed namespace;

#	if YCL_Win32
class TerminalData : private WConsole
{
public:
	//! \since build 567
	TerminalData(int fd)
		: WConsole(ToHandle(fd))
	{}

	//! \since build 755
	PDefH(bool, Clear, )
		ImplRet(WConsole::Clear(), true)

	PDefH(bool, RestoreAttributes, )
		ImplRet(WConsole::RestoreAttributes(), true)

	PDefH(bool, UpdateForeColor, std::uint8_t c)
		ImplRet(WConsole::UpdateForeColor(c), true)
};
#	else
//! \since build 560
namespace
{

yconstexpr const int cmap[] = {0, 4, 2, 6, 1, 5, 3, 7};

} //unnamed namespace

//! \since build 560
class TerminalData : private noncopyable, private nonmovable
{
private:
	//! \since build 567
	std::FILE* stream;

public:
	TerminalData(std::FILE* fp)
		: stream(fp)
	{}

	/*!
	\brief 输出缓存的命令结果。
	\pre 断言：参数的数据指针非空。
	\since build 839
	*/
	bool
	ExecuteCachedCommand(string_view) const;

	//! \since build 755
	PDefH(bool, Clear, ) ynothrow
		ImplRet(ExecuteCachedCommand("tput clear"))

	PDefH(bool, RestoreAttributes, ) ynothrow
		ImplRet(ExecuteCachedCommand("tput sgr0"))

	PDefH(bool, UpdateForeColor, std::uint8_t c) ynothrow
		ImplRet(ExecuteCachedCommand("tput setaf " + to_string(cmap[c & 7]))
			&& (c < ystdex::underlying(YSLib::Consoles::DarkGray)
			|| ExecuteCachedCommand("tput bold")))
};

bool
TerminalData::ExecuteCachedCommand(string_view cmd) const
{
	YAssertNonnull(cmd.data());

	if(!cmd.empty())
	{
		const auto& str(FetchCachedCommandResult(cmd));

		if(!str.empty())
		{
			std::fprintf(Nonnull(stream), "%s", str.c_str());
			return true;
		}
	}
	return {};
}
#	endif


Terminal::Guard::~Guard()
{
	if(p_terminal && *p_terminal)
		FilterExceptions([this]{
			if(YB_UNLIKELY(!p_terminal->p_data->RestoreAttributes()))
				throw LoggedEvent("Restoring terminal attributes failed.");
		});
}


Terminal::Terminal(std::FILE* fp)
	: p_data([fp]()->TerminalData*{
#	if !YCL_Win32
		// XXX: Performance?
		ystdex::setnbuf(fp);
#	endif
		const int fd(YCL_CallGlobal(fileno, Nonnull(fp)));

		// NOTE: This is not necessary for Windows since it only determine
		//	whether the file descriptor is associated with a character device.
		//	However as a optimization, it is somewhat more efficient for some
		//	cases. See $2015-01 @ %Documentation::Workflow::Annual2015.
		if(YCL_CallGlobal(isatty, fd))
#	if YCL_Win32
			TryRet(new TerminalData(fd))
#	else
			TryRet(new TerminalData(fp))
#	endif
			CatchExpr(Exception& e,
				YTraceDe(Informative, "Creating console failed: %s.", e.what()))
		return {};
	}())
{}

ImplDeDtor(Terminal)

bool
Terminal::Clear()
{
	return CallTe(&TerminalData::Clear, p_data);
}

Terminal::Guard
Terminal::LockForeColor(std::uint8_t c)
{
	return Guard(*this, &Terminal::UpdateForeColor, c);
}

bool
Terminal::RestoreAttributes()
{
	return CallTe(&TerminalData::RestoreAttributes, p_data);
}

bool
Terminal::UpdateForeColor(std::uint8_t c)
{
	return CallTe(&TerminalData::UpdateForeColor, p_data, c);
}

bool
UpdateForeColorByLevel(Terminal& term, RecordLevel lv)
{
	if(term)
	{
		using namespace Consoles;
		static yconstexpr const RecordLevel
			lvs[]{Err, Warning, Notice, Informative, Debug};
		static yconstexpr const Color
			colors[]{Red, Yellow, Cyan, Magenta, DarkGreen};
		const auto
			i(std::lower_bound(ystdex::begin(lvs), ystdex::end(lvs), lv));

		if(i == ystdex::end(lvs))
			term.RestoreAttributes();
		else
			term.UpdateForeColor(colors[i - lvs]);
		return true;
	}
	return {};
}
#endif

} // namespace platform_ex;

#endif

