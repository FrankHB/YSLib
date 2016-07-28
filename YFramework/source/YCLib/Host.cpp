/*
	© 2014-2016 FrankHB.

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
\version r410
\author FrankHB <frankhb1989@gmail.com>
\since build 492
\par 创建时间:
	2014-04-09 19:03:55 +0800
\par 修改时间:
	2016-07-25 11:20 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Host
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Host
#include YFM_YCLib_NativeAPI
#include YFM_YCLib_FileIO // for platform::FileOperationFailure,
//	platform::ThrowFileOperationFailure;
#include YFM_YSLib_Core_YException // for YSLib::FilterExceptions;
#if YCL_Win32
#	include YFM_Win32_YCLib_NLS // for MBCSToMBCS;
#	include YFM_Win32_YCLib_Consoles // for WConsole;
#	include <io.h> // for ::_isatty;
#endif
#if YF_Hosted
#	include YFM_YSLib_Core_YConsole
#endif

using namespace YSLib;
//! \since build 553
using platform::FileOperationFailure;
//! \since build 654
using platform::ThrowFileOperationFailure;
//! \since build 659
using YSLib::string;
//! \since build 659
using platform::string_view;

#if YF_Hosted

namespace platform_ex
{

Exception::Exception(std::error_code ec, const char* str, RecordLevel lv)
	: system_error(ec, Nonnull(str)),
	level(lv)
{}
Exception::Exception(std::error_code ec, string_view sv, RecordLevel lv)
	: system_error(ec, (Nonnull(sv.data()), std::string(sv))),
	level(lv)
{}
Exception::Exception(int ev, const std::error_category& ecat, const char* str,
	RecordLevel lv)
	: system_error(ev, ecat, Nonnull(str)),
	level(lv)
{}
Exception::Exception(int ev, const std::error_category& ecat, string_view sv,
	RecordLevel lv)
	: system_error(ev, ecat, (Nonnull(sv.data()), std::string(sv))),
	level(lv)
{}
ImplDeDtor(Exception)


#if YCL_Win32
void
HandleDelete::operator()(pointer h) const ynothrowv
{
	YCL_CallWin32F_Trace(CloseHandle, h);
}
#endif


string
FetchCommandOutput(const char* cmd, size_t buf_size)
{
	if(YB_UNLIKELY(buf_size == 0))
		throw std::invalid_argument("Zero buffer size found.");
	// TODO: Improve Win32 implementation?
	if(const auto fp = ystdex::unique_raw(platform::upopen(cmd, "r"),
		platform::upclose))
	{
		ystdex::setnbuf(fp.get());

		// TODO: Improve performance?
		const auto p_buf(make_unique_default_init<char[]>(buf_size));
		string res;

		for(size_t n; (n = std::fread(&p_buf[0], 1, buf_size, fp.get())) != 0; )
			res.append(&p_buf[0], n);
		return res;
	}
	ThrowFileOperationFailure("Failed opening pipe.");
}


YSLib::locked_ptr<CommandCache>
LockCommandCache()
{
	struct cmd_cache_tag
	{};
	static YSLib::mutex mtx;

	return {&ystdex::parameterize_static_object<CommandCache, cmd_cache_tag>(),
		mtx};
}

const string&
FetchCachedCommandResult(const string& cmd, size_t buf_size)
{
	auto p_locked(LockCommandCache());
	auto& cache(Deref(p_locked));

	try
	{
		// TODO: Blocked. Use %string_view as argument using C++14 heterogeneous
		//	%find template.
		const auto i_entry(cache.find(cmd));

		return (i_entry != cache.cend() ? i_entry : (cache.emplace(cmd,
			YB_UNLIKELY(cmd.empty()) ? string()
			: FetchCommandOutput(cmd.c_str(), buf_size))).first)->second;
	}
	CatchExpr(FileOperationFailure& e,
		YTraceDe(Err, "Command execution failed: %s", e.what()))
	return cache[string()];
}


pair<UniqueHandle, UniqueHandle>
MakePipe()
{
#	if YCL_Win32
	::HANDLE h_raw_read, h_raw_write;

	YCL_CallWin32F(CreatePipe, &h_raw_read, &h_raw_write, {}, 0);

	UniqueHandle h_read(h_raw_read), h_write(h_raw_write);

	YCL_CallWin32F(SetHandleInformation, h_write.get(), HANDLE_FLAG_INHERIT,
		HANDLE_FLAG_INHERIT);
	return {std::move(h_read), std::move(h_write)};
#	elif YCL_API_Has_unistd_h
	int fds[2];

	// TODO: Check whether '::socketpair' is available.
	if(::pipe(fds) != 0)
		ThrowFileOperationFailure("Failed getting file size.");

	auto pr(make_pair(UniqueHandle(fds[0]), UniqueHandle(fds[1])));
	auto check([](UniqueHandle& h, const char* msg){
		// NOTE: %O_NONBLOCK is initially cleared on ::pipe results.
		//	See http://pubs.opengroup.org/onlinepubs/9699919799/.
		if(!(h && h->SetNonblocking()))
			ThrowFileOperationFailure(msg);
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
#	if YCL_Win32
class TerminalData : private WConsole
{
public:
	//! \since build 567
	TerminalData(int fd)
		: WConsole(::HANDLE(::_get_osfhandle(fd)))
	{}

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
private:
	//! \since build 567
	bool
	ExecuteCommand(const string&) const;

public:
	PDefH(bool, RestoreAttributes, ) ynothrow
		ImplRet(ExecuteCommand("tput sgr0"))

	PDefH(bool, UpdateForeColor, std::uint8_t c) ynothrow
		ImplRet(ExecuteCommand("tput setaf " + to_string(cmap[c & 7]))
			&& (c < ystdex::underlying(YSLib::Consoles::DarkGray)
			|| ExecuteCommand("tput bold")))
};

bool
TerminalData::ExecuteCommand(const string& cmd) const
{
	const auto& str(FetchCachedCommandString(cmd));

	if(str.empty())
		return {};
	std::fprintf(Nonnull(stream), "%s", str.c_str());
	return true;
}
#	endif


Terminal::Guard::~Guard()
{
	if(terminal)
		FilterExceptions([this]{
			if(!YB_LIKELY(terminal.p_term->RestoreAttributes()))
				throw LoggedEvent("Restoring terminal attributes failed.");
		});
}


Terminal::Terminal(std::FILE* fp)
	: p_term([fp]()->TerminalData*{
#	if YCL_Win32
		const int fd(::_fileno(Nonnull(fp)));

		// NOTE: This is not necessary for Windows since it only determine
		//	whether the file descriptor is associated with a character device.
		//	However as a optimization, it is somewhat more efficient for some
		//	cases. See $2015-01 @ %Documentation::Workflow::Annual2015.
		if(::_isatty(fd))
			TryRet(new TerminalData(fd))
#	else
		// XXX: Performance?
		ystdex::setnbuf(fp);

		const int fd(fileno(Nonnull(fp)));

		if(::isatty(fd))
			TryRet(new TerminalData(fp))
#	endif
			CatchExpr(Exception& e,
				YTraceDe(Informative, "Creating console failed: %s.", e.what()))
		return {};
	}())
{}

ImplDeDtor(Terminal)

bool
Terminal::RestoreAttributes()
{
	return p_term ? p_term->RestoreAttributes() : false;
}

bool
Terminal::UpdateForeColor(std::uint8_t c)
{
	return p_term ? p_term->UpdateForeColor(c) : false;
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

