/*
	© 2014-2023 FrankHB.

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
\version r1584
\author FrankHB <frankhb1989@gmail.com>
\since build 492
\par 创建时间:
	2014-04-09 19:03:55 +0800
\par 修改时间:
	2023-03-07 12:52 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Host
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Host // for make_observer, platform::CallNothrow;
#include YFM_YSLib_Core_YException // for YSLib, to_std_string,
//	FetchCachedCommandResult, FilterExceptions;
#include YFM_YCLib_NativeAPI // for YCL_TraceCallF_CAPI, ::sem_open,
//	::sem_close, ::sem_unlink, ::pipe, MAX_PATH, ToHandle, YCL_CallGlobal,
//	isatty, platform_ex::ToHandle, FILE_TYPE_REMOTE, FILE_TYPE_CHAR,
//	FILE_TYPE_PIPE;
#include YFM_YCLib_FileIO // for MakePathStringW, YCL_Raise_SysE,
//	MakePathString;
#include <ystdex/deref_op.hpp> // for ystdex::call_value_or;
#if YCL_Win32
#	include YFM_Win32_YCLib_NLS // for CloseHandle, MBCSToMBCS, UTF8ToWCS;
#	include YFM_Win32_YCLib_MinGW32 // for YCL_CallF_Win32, HasPTYName;
#	include <limits> // for std::numeric_limits;
#	include YFM_Win32_YCLib_Consoles // for WConsole, COMMON_LVB_UNDERSCORE;
#endif
#include <cstdlib> // for std::getenv;
#include <ystdex/cstdio.h> // for ystdex::setnbuf;
#include <ystdex/cstring.h> // for ystdex::is_nonempty, ystdex::ntctslen;
#if YF_Hosted
#	include YFM_YSLib_Core_YConsole // for complete
//	YSLib::Consoles, IOutputTerminal, GDumbOutputTerminal,
//	AllowsColorByEnvironment, TPutTerminal, ISO6429Terminal;
#	if !(YCL_Win32 || YCL_API_Has_semaphore_h)
#		error "Unsupported platform found."
#	endif
#endif
#include <algorithm> // for std::lower_bound;
#include <ystdex/range.hpp> // for ystdex::begin, ystdex::end;

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
Exception::Exception(std::error_code ec, const std::string& str, RecordLevel lv)
	: system_error(ec, str),
	level(lv)
{}
Exception::Exception(std::error_code ec, string_view sv, RecordLevel lv)
	: Exception(ec, (yunused(Nonnull(sv.data())), to_std_string(sv)), lv)
{}
Exception::Exception(int ev, const std::error_category& ecat, const char* str,
	RecordLevel lv)
	: system_error(ev, ecat, Nonnull(str)),
	level(lv)
{}
Exception::Exception(int ev, const std::error_category& ecat,
	const std::string& str, RecordLevel lv)
	: system_error(ev, ecat, str),
	level(lv)
{}
Exception::Exception(int ev, const std::error_category& ecat, string_view sv,
	RecordLevel lv)
	: Exception(ev, ecat, (yunused(Nonnull(sv.data())), to_std_string(sv)), lv)
{}
ImplDeDtor(Exception)


#	if YCL_Win32
void
HandleDelete::operator()(pointer h) const ynothrowv
{
	YCL_TraceCallF_Win32(CloseHandle, h);
}
#	endif


#	if !YCL_Win32
void
Semaphore::Deleter::operator()(pointer p) const ynothrowv
{
	YCL_TraceCallF_CAPI(::sem_close, static_cast<::sem_t*>(p));
	if(Referent)
		YCL_TraceCallF_CAPI(::sem_unlink, Referent->name.c_str());
}
#	endif


Semaphore::Semaphore(const char* n, CountType init)
#	if YCL_Win32
	: Semaphore(platform::ucast(MakePathStringW(n).c_str()), init)
#	else
	: h_sem([this, n, init]{
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
#	endif
{
#	if !YCL_Win32
	h_sem.get_deleter().Referent = make_observer(this);
#	endif
}
Semaphore::Semaphore(const char16_t* n, CountType init)
#	if YCL_Win32
	// TODO: Allow setting of security attributes.
	: h_sem(YCL_CallF_Win32(CreateSemaphore, {}, init,
		std::numeric_limits<CountType>::max(), platform::wcast(n)))
#	else
	: Semaphore(platform::MakePathString(n).c_str(), init)
#	endif
{}

void
Semaphore::lock()
{
#	if YCL_Win32
	WaitUnique(native_handle());
#	else
	platform::RetryOnInterrupted(std::bind(YCL_WrapCall_CAPI(, ::sem_wait,
		static_cast<::sem_t*>(native_handle())), yfsig));
#	endif
}

bool
Semaphore::try_lock() ynothrow
{
#	if YCL_Win32
	return TryWaitUnique(native_handle());
#	else
	return platform::RetryOnInterrupted([this]{
		return ::sem_trywait(static_cast<::sem_t*>(native_handle()));
	}) == 0;
#	endif
}

void
Semaphore::unlock() ynothrow
{
#	if YCL_Win32
	const auto
		res(YCL_TraceCallF_Win32(ReleaseSemaphore, native_handle(), 1, {}));

	YAssert(res, "Invalid semaphore found.");
#	else
	const auto res(YCL_TraceCallF_CAPI(::sem_post,
		static_cast<::sem_t*>(native_handle())));

	YAssert(res == 0, "Invalid semaphore found.");
#	endif
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
	auto check(
		[] YB_LAMBDA_ANNOTATE((UniqueHandle& h, const char* msg), , nonnull(3)){
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

//! \since build 560
namespace
{

//! \since build 969
using namespace YSLib::Consoles;

#	if YCL_Win32
//! \since build 969
class WConsoleTerminal : private WConsole, implements IOutputTerminal
{
public:
	WConsoleTerminal(int fd)
		: WConsole(ToHandle(fd))
	{}

	//! \since build 755
	YB_ATTR_nodiscard PDefH(bool, Clear, ) ImplI(IOutputTerminal)
		ImplRet(WConsole::Clear(), true)

	//! \since build 560
	//!@{
	YB_ATTR_nodiscard PDefH(bool, RestoreAttributes, ) ImplI(IOutputTerminal)
		ImplRet(WConsole::RestoreAttributes(), true)

	//! \since build 969
	YB_ATTR_nodiscard PDefH(bool, UpdateBackColor, std::uint8_t c)
		ImplI(IOutputTerminal)
		ImplRet(WConsole::UpdateBackColor(c), true)

	YB_ATTR_nodiscard PDefH(bool, UpdateForeColor, std::uint8_t c)
		ImplI(IOutputTerminal)
		ImplRet(WConsole::UpdateForeColor(c), true)

	// XXX: This works with DBCS code pages by default. For other codepages,
	//	%ENABLE_LVB_GRID_WORLDWIDE may be required to specify to the console
	//	output mode manually.
	//! \since build 969
	YB_ATTR_nodiscard PDefH(bool, UpdateUnderline, bool enabled)
		ImplI(IOutputTerminal)
		ImplRet(Attributes = enabled ? Attributes
			| ::WORD(COMMON_LVB_UNDERSCORE) : Attributes
			& ::WORD(~COMMON_LVB_UNDERSCORE), Update(), true) 

	YB_ATTR_nodiscard bool
	WriteString(const char*, size_t) ImplI(IOutputTerminal);
	//!@}
};

bool
WConsoleTerminal::WriteString(const char* s, size_t len)
{
	const auto wstr(UTF8ToWCS(string_view(s, len)));

	return WConsole::WriteString(wstr) == wstr.length();
}
#	endif

//! \since build 969
//!@{
YB_ATTR_nodiscard bool
UseTPut() ynothrow
{
	static const bool use_tput(ystdex::is_nonempty(std::getenv("YF_Use_tput")));

	return use_tput;
}

// XXX: For now, use 'new' to create the %IOutputTerminal client directly.
//! \ingroup traits
//!@{
struct DirectTraits
{
	template<class _tOutTerm, typename... _tParams>
	YB_ATTR_nodiscard YB_ATTR_returns_nonnull static inline IOutputTerminal*
	CreateOutput(_tParams&&... args)
	{
		return new _tOutTerm(yforward(args)...);
	}

	YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_NONNULL(1) static
		IOutputTerminal*
	CreateTTY(std::FILE* fp)
	{
		return CreateOutput<ISO6429Terminal>(fp);
	}
};

struct DumbTraits
{
	template<class _tOutTerm, typename... _tParams>
	YB_ATTR_nodiscard YB_ATTR_returns_nonnull static inline IOutputTerminal*
	CreateOutput(_tParams&&... args)
	{
		return new GDumbOutputTerminal<_tOutTerm>(yforward(args)...);
	}

	YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_NONNULL(1) static
		IOutputTerminal*
	CreateTTY(std::FILE* fp)
	{
		return UseTPut() ? CreateOutput<TPutTerminal>(fp)
			: CreateOutput<ISO6429Terminal>(fp);
	}
};

struct AutoTraits
{
	template<class _tOutTerm, typename... _tParams>
	YB_ATTR_nodiscard YB_ATTR_returns_nonnull static inline IOutputTerminal*
	CreateOutput(_tParams&&... args)
	{
		return AllowsColorByEnvironment()
			? DirectTraits::CreateOutput<_tOutTerm>(yforward(args)...)
			: DumbTraits::CreateOutput<_tOutTerm>(yforward(args)...);
	}

	YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_NONNULL(1) static
		IOutputTerminal*
	CreateTTY(std::FILE* fp)
	{
		return UseTPut() ? CreateOutput<TPutTerminal>(fp)
			: CreateOutput<ISO6429Terminal>(fp);
	}
};
//!@}

template<typename _func>
YB_ATTR_nodiscard YB_NONNULL(2) IOutputTerminal*
WrapInitTerminal(_func f, std::FILE* fp)
{
	// NOTE: See $2023-03 @ %Documentation::Workflow.
	ystdex::setnbuf(fp);
	// NOTE: For Microsoft Windows and compatible implemenations (Wine,
	//	ReactOS), the type of the file is required for the detection on
	//	different cases fit for the terminal I/O. A call to 'isatty' checks
	//	whether the underlying file is associated to a character device. This is
	//	merely an optimization and not sufficient. See ($2015-01, $2021-06,
	//	$2023-03) @ %Documentation::Workflow.
	TryRet(f(fp, YCL_CallGlobal(fileno, Nonnull(fp))))
	// XXX: Errors are ignored.
	CatchIgnore(Exception&)
	return {};
}

template<class _tTraits>
YB_ATTR_nodiscard YB_NONNULL(1) IOutputTerminal*
InitTerminal(std::FILE* fp, int fd)
{
#	if YCL_Win32
	const auto h(platform_ex::ToHandle(fd));

	if(h != INVALID_HANDLE_VALUE)
	{
		switch(::GetFileType(h) & ~static_cast<unsigned long>(FILE_TYPE_REMOTE))
		{
		case FILE_TYPE_CHAR:
			// NOTE: See $2023-03 @ %Documentation::Workflow.
			return _tTraits::template CreateOutput<WConsoleTerminal>(fd);
		case FILE_TYPE_PIPE:
			// NOTE: See $2023-03 @ %Documentation::Workflow.
			if(HasPTYName(h))
				return _tTraits::CreateTTY(fp);
		}
	}
	else
		YCL_Raise_SysE(, "::_get_osfhandle", yfsig);
#	else
	if(YCL_CallGlobal(isatty, fd))
		return _tTraits::CreateTTY(fp);
#	endif
	return {};
}
template<>
YB_ATTR_nodiscard YB_NONNULL(1) IOutputTerminal*
InitTerminal<DirectTraits>(std::FILE* fp, int fd)
{
	// XXX: Ditto.
#	if YCL_Win32
	const auto h(platform_ex::ToHandle(fd));

	if(h != INVALID_HANDLE_VALUE)
	{
		switch(::GetFileType(h) & ~static_cast<unsigned long>(FILE_TYPE_REMOTE))
		{
		case FILE_TYPE_CHAR:
			// NOTE: Ditto.
			return DirectTraits::template CreateOutput<WConsoleTerminal>(fd);
		}
	}
	else
		YCL_Raise_SysE(, "::_get_osfhandle", yfsig);
#	else
	yunused(fd);
#	endif
	return DirectTraits::CreateTTY(fp);
}

template<class _tTraits>
YB_ATTR_nodiscard YB_NONNULL(1) IOutputTerminal*
TryInitTerminal(std::FILE* fp)
{
	return WrapInitTerminal(InitTerminal<_tTraits>, fp);
}
//!@}

} //unnamed namespace;


Terminal::Guard::~Guard()
{
	if(p_terminal && *p_terminal)
		FilterExceptions([this]{
			if(YB_UNLIKELY(!p_terminal->p_data->RestoreAttributes()))
				throw LoggedEvent("Restoring terminal attributes failed.");
		});
}
Terminal::Terminal(std::FILE* fp)
	: p_data(TryInitTerminal<AutoTraits>(fp))
{}
Terminal::Terminal(YSLib::Consoles::ColorOption opt, std::FILE* fp)
	: p_data([=]() -> IOutputTerminal*{
		return WrapInitTerminal([opt](std::FILE* p, int fd){
			switch(opt)
			{
			case ColorOption::Never:
				return InitTerminal<DumbTraits>(p, fd);
			case ColorOption::Auto:
				return InitTerminal<AutoTraits>(p, fd);
			case ColorOption::Always:
				return InitTerminal<DirectTraits>(p, fd);
			}
			YAssert(false, "Invalid option found.");
			YB_ASSUME(false);
		}, fp);
	}())
{}
Terminal::Terminal(YSLib::Consoles::ColorOptionTag<
	YSLib::Consoles::ColorOption::Never>, std::FILE* fp)
	: p_data(TryInitTerminal<DumbTraits>(fp))
{}
Terminal::Terminal(YSLib::Consoles::ColorOptionTag<
	YSLib::Consoles::ColorOption::Always>, std::FILE* fp)
	: p_data(TryInitTerminal<DirectTraits>(fp))
{}
ImplDeDtor(Terminal)

bool
Terminal::Clear()
{
	return CallTe(&IOutputTerminal::Clear, p_data);
}

Terminal::Guard
Terminal::LockBackColor(std::uint8_t c)
{
	return Guard(*this, &Terminal::UpdateBackColor, c);
}

Terminal::Guard
Terminal::LockForeColor(std::uint8_t c)
{
	return Guard(*this, &Terminal::UpdateForeColor, c);
}

Terminal::Guard
Terminal::LockUnderline(bool enabled)
{
	return Guard(*this, &Terminal::UpdateUnderline, enabled);
}

bool
Terminal::RestoreAttributes()
{
	return CallTe(&IOutputTerminal::RestoreAttributes, p_data);
}

bool
Terminal::UpdateBackColor(std::uint8_t c)
{
	return CallTe(&IOutputTerminal::UpdateBackColor, p_data, c);
}

bool
Terminal::UpdateForeColor(std::uint8_t c)
{
	return CallTe(&IOutputTerminal::UpdateForeColor, p_data, c);
}

bool
Terminal::UpdateUnderline(bool enabled)
{
	return CallTe(&IOutputTerminal::UpdateUnderline, p_data, enabled);
}

bool
Terminal::WriteString(std::FILE* p_file, const char* s)
{
	const auto len(ystdex::ntctslen(s));

	if(p_data)
		return p_data->WriteString(s, len);

	const int n(std::fprintf(p_file, "%s", s));

	return n >= 0 && size_t(n) == len;
}

} // namespace platform_ex;

#endif

