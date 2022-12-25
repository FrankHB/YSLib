/*
	© 2014-2022 FrankHB.

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
\version r1144
\author FrankHB <frankhb1989@gmail.com>
\since build 492
\par 创建时间:
	2014-04-09 19:03:55 +0800
\par 修改时间:
	2022-11-28 19:52 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Host
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Host // for make_observer, platform::CallNothrow;
#include YFM_YSLib_Core_YCoreUtilities // for YSLib, to_std_string,
//	FetchCachedCommandResult, ystdex::underlying, FilterExceptions;
#include YFM_YCLib_NativeAPI // for YCL_TraceCallF_CAPI, ::sem_open,
//	::sem_close, ::sem_unlink, ::pipe, MAX_PATH, ToHandle, YCL_CallGlobal,
//	isatty, platform_ex::ToHandle, FILE_TYPE_CHAR, FILE_TYPE_PIPE;
#include YFM_YCLib_FileIO // for MakePathStringW, YCL_Raise_SysE,
//	MakePathString;
#include <ystdex/deref_op.hpp> // for ystdex::call_value_or;
#include <cstdlib> // for std::getenv;
#if YCL_Win32
// XXX: This is not used because currently no '-lntdll' is expected in the
//	linker command line, and the dynamic loading should be better in
//	compatibility.
#	if false && __has_include(<winternl.h>)
#		include <winternl.h> // for optional ::NtQueryObject;
#	endif
#	include YFM_Win32_YCLib_MinGW32 // for YCL_DeclW32Call, YCL_CallF_Win32,
#	include YFM_Win32_YCLib_NLS // for CloseHandle, MBCSToMBCS, UTF8ToWCS;
#	include <limits> // for std::numeric_limits;
#	include <ystdex/type_pun.hpp> // for ystdex::replace_storage_t;
#	include YFM_Win32_YCLib_Consoles // for WConsole;

//! \since build 921
//@{
namespace
{

#	if !(false && __has_include(<winternl.h>))
enum OBJECT_INFORMATION_CLASS
{
	ObjectBasicInformation,
	ObjectNameInformation,
	ObjectTypeInformation,
	ObjectAllInformation,
	ObjectDataInformation
};
#	endif

namespace YCL_Impl_details
{

// NOTE: To avoid hiding of the global name, the declarations shall not be in
//	namespace %platform_ex.
YCL_DeclW32Call(NtQueryObject, ntdll, long, ::HANDLE, OBJECT_INFORMATION_CLASS,
	void*, unsigned long, unsigned long*)

} // namespace YCL_Impl_details;

} // unnamed namespace;
//@}
#endif
#include <ystdex/cstring.h> // for ystdex::ntctslen;
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

DeclI(, ITerminalData)
	YB_ATTR_nodiscard DeclIEntry(bool Clear())
	YB_ATTR_nodiscard DeclIEntry(bool RestoreAttributes())
	YB_ATTR_nodiscard DeclIEntry(bool UpdateForeColor(std::uint8_t))
	YB_ATTR_nodiscard
		DeclIEntry(bool WriteString(const char*, size_t))
EndDecl

ImplDeDtor(ITerminalData)


//! \since build 560
namespace
{

// NOTE: This maps the code in the initial values of enumerators in
//	%YSLib::Consoles::Color to the code conforming to SGR codes defined in
//	ISO/IEC 6429:1992 8.3.118 minus the initial index value 30.
yconstexpr const int cmap[] = {0, 4, 2, 6, 1, 5, 3, 7};

//! \since build 921
//@{
#	if YCL_Win32
struct UNICODE_STRING
{
	unsigned short Length;
	unsigned short MaximumLength;
	wchar_t* Buffer;
};


struct OBJECT_NAME_INFORMATION
{
	UNICODE_STRING Name;
	wchar_t NameBuffer[1];
};


YB_ATTR_nodiscard bool
HasPTY(::HANDLE h) ynothrow
{
	// NOTE: This try to detect the PTY simulation used by Cygwin/MSYS programs
	//	like MinTTY. See $2021-06 @ %Documentation::Workflow.
	using buf_t = byte[sizeof(OBJECT_NAME_INFORMATION)
		+ MAX_PATH * sizeof(wchar_t)];
	ystdex::replace_storage_t<buf_t, yalignof(OBJECT_NAME_INFORMATION)> storage;
	unsigned long res;

	if(YCL_Impl_details::NtQueryObject(h, ObjectNameInformation,
		storage.access(), sizeof(buf_t) - 2, &res) >= 0)
	{
		const auto& n(storage.access<OBJECT_NAME_INFORMATION>().Name);
		wchar_t* s(n.Buffer);

		s[n.Length / sizeof(*s)] = 0;
		// XXX: The prefix L"\\Device\\NamedPipe\\", the number of digits and
		//	the suffix are not checked for simplicity and compatibility in
		//	future (although the name scheme should not be likely to change
		//	without sufficient reasons).
		return (std::wcsstr(s, L"msys-") || std::wcsstr(s, L"cygwin-"))
			&& std::wcsstr(s, L"-pty");
	}
	return {};
}
#	endif


class ATerminalData : private noncopyable, private nonmovable,
	implements ITerminalData
{
private:
	//! \invariant \c stream 。
	std::FILE* stream;

public:
	YB_NONNULL(2)
	ATerminalData(std::FILE* fp)
		: stream(Nonnull(fp))
	{}

	YB_ATTR_nodiscard YB_ATTR_returns_nonnull
		DefGetter(const ynothrow, std::FILE*, Stream, Nonnull(stream))

	YB_ATTR_nodiscard YB_NONNULL(2)
		PDefH(bool, PrintEscape, const char* s) const
		ImplRet(PrintFormat("\033[%s", s))

	template<typename... _tParams>
	YB_ATTR_nodiscard YB_NONNULL(2) bool
	PrintFormat(const char* fmt, _tParams&&... args) const
	{
		// XXX: Error from 'std::fprintf' is ignored.
		std::fprintf(GetStream(), Nonnull(fmt), yforward(args)...);
		return true;
	}

	YB_ATTR_nodiscard YB_NONNULL(2)
		PDefH(bool, PrintString, const char* s) const
		ImplRet(PrintFormat("%s", s))

	YB_ATTR_nodiscard YB_NONNULL(2) bool
	WriteString(const char*, size_t) ImplI(ITerminalData);
};

bool
ATerminalData::WriteString(const char* s, size_t len)
{
	const int n(std::fprintf(GetStream(), "%s", Nonnull(s)));

	return n >= 0 && size_t(n) == len;
}


class TPutTerminalData final : public ATerminalData
{
public:
	TPutTerminalData(std::FILE* fp)
		: ATerminalData(fp)
	{}

	/*!
	\brief 输出缓存的命令结果。
	\pre 断言：参数的数据指针非空。
	\since build 839
	*/
	YB_ATTR_nodiscard bool
	ExecuteCachedCommand(string_view) const;

	//! \since build 755
	YB_ATTR_nodiscard PDefH(bool, Clear, ) ynothrow ImplI(ATerminalData)
		ImplRet(ExecuteCachedCommand("tput clear"))

	//! \since build 560
	YB_ATTR_nodiscard PDefH(bool, RestoreAttributes, ) ynothrow
		ImplI(ATerminalData)
		ImplRet(ExecuteCachedCommand("tput sgr0"))

	//! \since build 560
	YB_ATTR_nodiscard PDefH(bool, UpdateForeColor, std::uint8_t c) ynothrow
		ImplI(ATerminalData)
		ImplRet(ExecuteCachedCommand((yimpl(std::)string("tput setaf ")
			+ char('0' + cmap[c & 7])).data()) && (c < ystdex::underlying(
			YSLib::Consoles::DarkGray) || ExecuteCachedCommand("tput bold")))
};

bool
TPutTerminalData::ExecuteCachedCommand(string_view cmd) const
{
	YAssertNonnull(cmd.data());
	if(!cmd.empty())
	{
		const auto& str(FetchCachedCommandResult(cmd));

		if(!str.empty())
		{
			// XXX: Error from 'std::fprintf' is ignored.
			std::fprintf(GetStream(), "%s", str.c_str());
			return true;
		}
	}
	return {};
}


class ISO6429TerminalData final : public ATerminalData
{
public:
	ISO6429TerminalData(std::FILE* fp)
		: ATerminalData(fp)
	{}

	YB_ATTR_nodiscard PDefH(bool, Clear, ) ynothrow ImplI(ATerminalData)
		ImplRet(PrintEscape("H\033[J"))

	YB_ATTR_nodiscard PDefH(bool, RestoreAttributes, ) ynothrow
		ImplI(ATerminalData)
		ImplRet(PrintEscape("0;10m"))

	YB_ATTR_nodiscard PDefH(bool, UpdateForeColor, std::uint8_t c) ynothrow
		ImplI(ATerminalData)
		ImplRet(PrintFormat("\033[%c%cm", c < ystdex::underlying(
			YSLib::Consoles::DarkGray) ? '3' : '9', '0' + cmap[c & 7]))
};


YB_ATTR_returns_nonnull YB_NONNULL(1) ITerminalData*
CreateTTYTerminalData(std::FILE* fp)
{
	static const bool use_tput(std::getenv("YF_Use_tput"));

	return use_tput ? static_cast<ITerminalData*>(new TPutTerminalData(fp))
		: new ISO6429TerminalData(fp);
}
//@}

} //unnamed namespace


#	if YCL_Win32
//! \since build 921
class WConsoleTerminalData final : private WConsole, implements ITerminalData
{
public:
	//! \since build 567
	WConsoleTerminalData(int fd)
		: WConsole(ToHandle(fd))
	{}

	//! \since build 755
	YB_ATTR_nodiscard PDefH(bool, Clear, ) ImplI(ITerminalData)
		ImplRet(WConsole::Clear(), true)

	//! \since build 560
	//@{
	YB_ATTR_nodiscard PDefH(bool, RestoreAttributes, ) ImplI(ITerminalData)
		ImplRet(WConsole::RestoreAttributes(), true)

	YB_ATTR_nodiscard PDefH(bool, UpdateForeColor, std::uint8_t c)
		ImplI(ITerminalData)
		ImplRet(WConsole::UpdateForeColor(c), true)

	YB_ATTR_nodiscard bool
	WriteString(const char*, size_t) ImplI(ITerminalData);
	//@}
};

bool
WConsoleTerminalData::WriteString(const char* s, size_t len)
{
	const auto wstr(UTF8ToWCS(string_view(s, len)));

	return WConsole::WriteString(wstr) == wstr.length();
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
	: p_data([fp]()->ITerminalData*{
#	if !YCL_Win32
		// XXX: Performance?
		ystdex::setnbuf(fp);
#	endif
		const int fd(YCL_CallGlobal(fileno, Nonnull(fp)));

		// NOTE: For Microsoft Windows, the type of the file is required for the
		//	detection on different cases fit for the terminal I/O. A call to
		//	'isatty' checks whether the underlying file is associated to a
		//	character device. This is merely an optimization and not sufficient.
		//	See ($2015-01, $2021-06) @ %Documentation::Workflow.
		try
		{
			// XXX: For now, use 'new' to create %ITerminalData client directly.
#	if YCL_Win32

			const auto h(platform_ex::ToHandle(fd));

			if(h != INVALID_HANDLE_VALUE)
			{
				// NOTE: All other files are treated as a non-console.
				switch(::GetFileType(h))
				{
				// NOTE: Non-console character devices (e.g. LPT devices) will
				//	fail in initialization of %WConsole which throw an exception
				//	caught later.
				case FILE_TYPE_CHAR:
					return new WConsoleTerminalData(fd);
				case FILE_TYPE_PIPE:
					if(HasPTY(h))
						return CreateTTYTerminalData(fp);
				}
			}
			else
				YCL_Raise_SysE(, "::_get_osfhandle", yfsig);
#	else
			if(YCL_CallGlobal(isatty, fd))
				return CreateTTYTerminalData(fp);
#	endif
		}
		// XXX: Errors are ignored.
		CatchIgnore(Exception&)
		return {};
	}())
{}
ImplDeDtor(Terminal)

bool
Terminal::Clear()
{
	return CallTe(&ITerminalData::Clear, p_data);
}

Terminal::Guard
Terminal::LockForeColor(std::uint8_t c)
{
	return Guard(*this, &Terminal::UpdateForeColor, c);
}

bool
Terminal::RestoreAttributes()
{
	return CallTe(&ITerminalData::RestoreAttributes, p_data);
}

bool
Terminal::UpdateForeColor(std::uint8_t c)
{
	return CallTe(&ITerminalData::UpdateForeColor, p_data, c);
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

} // namespace platform_ex;

#endif

