/*
	© 2014-2015 FrankHB.

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
\version r272
\author FrankHB <frankhb1989@gmail.com>
\since build 492
\par 创建时间:
	2014-04-09 19:03:55 +0800
\par 修改时间:
	2015-01-20 01:19 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Host
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Host
#include YFM_YCLib_NativeAPI
#include YFM_YCLib_FileSystem // for platform::FileOperationFailure;
#if YCL_Win32
#	include YFM_MinGW32_YCLib_Consoles
#	include <io.h> // for ::_isatty;
#elif YF_Hosted
#	include <fcntl.h>
#endif

//! \since build 553
using platform::FileOperationFailure;
using namespace YSLib;

#if YF_Hosted

namespace platform_ex
{

Exception::Exception(std::error_code ec, const std::string& msg, LevelType lv)
	: system_error(ec, msg),
	level(lv)
{}
Exception::Exception(int ev, const std::error_category& ecat,
	const std::string& msg, LevelType lv)
	: system_error(ev, ecat, msg),
	level(lv)
{}


#if YCL_Win32
void
HandleDeleter::operator()(pointer h)
{
	::CloseHandle(h);
}
#endif


std::string
FetchCommandOutput(const std::string& cmd, std::size_t buf_size)
{
	if(YB_UNLIKELY(buf_size == 0))
		throw std::invalid_argument("Zero buffer size found.");
	// TODO: Improve Win32 implementation?
	if(const auto fp = ystdex::unique_raw(platform::upopen(cmd.c_str(), "r"),
		platform::upclose))
	{
		ystdex::setnbuf(fp.get());

		// TODO: Improve performance.
		const auto p_buf(make_unique<char[]>(buf_size));
		std::string res;
		size_t n;

		do
		{
			n = std::fread(&p_buf[0], 1, buf_size, fp.get());
			res.append(&p_buf[0], n);
		}while(n != 0);
		return res;
	}
	throw FileOperationFailure(errno, std::generic_category(),
		"Failed opening pipe.");
}


YSLib::locked_ptr<CommandCache>
LockCommandCache()
{
	struct cmd_cache_tag
	{};
	static YSLib::mutex mtx;
	YSLib::unique_lock<YSLib::mutex> lck(mtx);

	return {&ystdex::parameterize_static_object<CommandCache, cmd_cache_tag>(),
		std::move(lck)};
}

const std::string&
FetchCachedCommandResult(const std::string& cmd, std::size_t buf_size)
{
	auto p_locked(LockCommandCache());
	auto& cache(Deref(p_locked));
	try
	{
		const auto i_entry(cache.find(cmd));

		return (i_entry != cache.cend() ? i_entry : (cache.emplace(cmd,
			YB_UNLIKELY(cmd.empty()) ? std::string()
			: FetchCommandOutput(cmd, buf_size))).first)->second;
	}
	CatchExpr(FileOperationFailure& e,
		YTraceDe(Err, "Command execution failed: %s.", e.what()))
	return cache[std::string()];
}


std::pair<UniqueHandle, UniqueHandle>
MakePipe()
{
#	if YCL_Win32
	::HANDLE h_raw_read, h_raw_write;

	if(!::CreatePipe(&h_raw_read, &h_raw_write, {}, 0))
		YF_Raise_Win32Exception("CreatePipe");

	UniqueHandle h_read(h_raw_read), h_write(h_raw_write);

	if(!::SetHandleInformation(h_write.get(), HANDLE_FLAG_INHERIT,
		HANDLE_FLAG_INHERIT))
		YF_Raise_Win32Exception("SetHandleInformation");
	return {std::move(h_read), std::move(h_write)};
#	elif YCL_API_Has_unistd_h
	int fds[2];

	if(::pipe(fds) != 0)
		throw FileOperationFailure(errno, std::generic_category(),
			"Failed getting file size.");
	if(::fcntl(fds[0], F_SETFL, O_NONBLOCK) != 0)
		throw FileOperationFailure(errno, std::generic_category(),
			"Failed making pipe for reading.");
	if(::fcntl(fds[1], F_SETFL, O_NONBLOCK) != 0)
		throw FileOperationFailure(errno, std::generic_category(),
			"Failed making pipe for writing.");
	return {UniqueHandle(fds[0]), UniqueHandle(fds[1])};
#	else
#	error "Unsupported platform found."
#	endif
}


#	if YCL_Win32
std::string
DecodeArg(const char* str)
{
	return MBCSToMBCS(str, CP_ACP, CP_UTF8);
}

std::string
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
	ExecuteCommand(const std::string&) const;

public:
	PDefH(bool, RestoreAttributes, ) ynothrow
		ImplRet(ExecuteCommand("tput sgr0"))

	PDefH(bool, UpdateForeColor, std::uint8_t c) ynothrow
		ImplRet(ExecuteCommand("tput setaf " + to_string(cmap[c & 7]))
			&& (c < ystdex::underlying(platform::Consoles::DarkGray)
			|| ExecuteCommand("tput bold")))
};

bool
TerminalData::ExecuteCommand(const std::string& cmd) const
{
	const auto& str(FetchCachedCommandString(cmd));

	if(str.empty())
		return {};
	std::fprintf(Nonnull(stream), "%s", str.c_str());
	return true;
}
#	endif


Terminal::Terminal(std::FILE* fp)
	: p_term([](std::FILE* fp)->TerminalData*{
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
	}(fp))
{}

Terminal::DefDeDtor(Terminal)

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
#endif

} // namespace platform_ex;

#endif

