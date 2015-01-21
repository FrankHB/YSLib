/*
	© 2014-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Host.h
\ingroup YCLib
\ingroup YCLibLimitedPlatforms
\ingroup Host
\brief YCLib 宿主平台公共扩展。
\version r231
\author FrankHB <frankhb1989@gmail.com>
\since build 492
\par 创建时间:
	2014-04-09 19:03:55 +0800
\par 修改时间:
	2015-01-20 00:59 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Host
*/


#ifndef YCL_INC_Host_h_
#define YCL_INC_Host_h_ 1

#include "YCLib/YModules.h"
#include "YSLib/Core/YModules.h"
#include YFM_YCLib_YCommon
#include YFM_YSLib_Core_YException // for YSLib::LoggedEvent;
#include <memory> // for std::unique_ptr;
#include <unordered_map> // for std::unordered_map;
#include <system_error> // for std::system_error;
#if !YCL_Win32
#	include YFM_YCLib_FileSystem // for platform::file_desc;
#else
//! \since build 564
using HANDLE = void*;
#endif

#if YF_Hosted

namespace platform_ex
{

/*!
\ingroup exception_types
\brief 宿主异常。
\since build 426
*/
class YF_API Exception : public std::system_error
{
public:
	//! \since build 545
	//@{
	using LevelType = YSLib::LoggedEvent::LevelType;

private:
	LevelType level = YSLib::Emergent;

public:
	Exception(std::error_code, const std::string& = "unknown host exception",
		LevelType = YSLib::Emergent);
	Exception(int, const std::error_category&, const std::string&
		= "unknown host exception", LevelType = YSLib::Emergent);

	DefGetter(const ynothrow, LevelType, Level, level)
	//@}
};


#	if !YCL_Win32 && YCL_API_Has_unistd_h
//! \since build 553
using HandleDeleter = platform::FileDescriptorDeleter;
#	else
/*!
\brief 句柄删除器。
\since build 520
*/
struct YF_API HandleDeleter
{
#		if YCL_Win32
	using pointer = ::HANDLE;

	void
	operator()(pointer);
#		else
	using pointer = int*;

	PDefHOp(void, (), pointer h)
		ImplExpr(delete h)
#		endif
};
#endif

//! \since build 520
using UniqueHandle = std::unique_ptr<HandleDeleter::pointer, HandleDeleter>;


//! \since build 567
//@{
//! \brief 默认命令缓冲区大小。
yconstexpr size_t DefaultCommandBufferSize(yimpl(4096));

/*!
\brief 取命令在标准输出上的执行结果。
\return 读取的二进制存储。
\throw std::invalid_argument 第二参数的值等于 \c 0 。
\throw std::system_error 表示读取失败的派生类异常对象。
\note 第二参数指定每次读取的缓冲区大小，先于执行命令进行检查。
*/
YF_API std::string
FetchCommandOutput(const std::string&, std::size_t = DefaultCommandBufferSize);


//! \brief 命令和命令执行结果的缓冲区类型。
using CommandCache = std::unordered_map<std::string, std::string>;

/*!
\brief 锁定命令执行缓冲区。
\return 静态对象的非空锁定指针。
*/
YF_API YSLib::locked_ptr<CommandCache>
LockCommandCache();

//! \brief 取缓冲的命令执行结果。
YF_API const std::string&
FetchCachedCommandResult(const std::string&,
	std::size_t = DefaultCommandBufferSize);

//! \brief 取缓冲的命令执行结果字符串。
inline PDefH(std::string, FetchCachedCommandString, const std::string& cmd,
	std::size_t buf_size = DefaultCommandBufferSize)
	ImplRet(ystdex::trail(std::string(FetchCachedCommandResult(cmd, buf_size))))
//@}

/*!
\brief 创建管道。
\throw std::system_error 表示创建失败的派生类异常对象。
\since build 520
*/
YF_API std::pair<UniqueHandle, UniqueHandle>
MakePipe();


/*!
\brief 从外部环境编码字符串参数或解码为外部环境字符串参数。
\pre Win32 平台可能间接断言参数非空。
\since build 560

对 Win32 平台调用当前代码页的 platform::MBCSToMBCS 编解码字符串，其它直接传递参数。
此时和 platform::MBCSToMBCS 不同，参数为 \c std::string 时长度通过 NTCTS 计算。
若需要使用 <tt>const char*</tt> 指针，可直接使用 <tt>&arg[0]</tt> 的形式。
*/
//@{
#	if YCL_Win32
YF_API std::string
DecodeArg(const char*);
inline PDefH(std::string, DecodeArg, const std::string& arg)
	ImplRet(DecodeArg(&arg[0]))
#	endif
template<typename _type
#if YCL_Win32
	, yimpl(typename = ystdex::enable_if_t<!std::is_constructible<std::string,
		_type&&>::value>)
#endif
	>
yconstfn auto
DecodeArg(_type&& arg) -> decltype(yforward(arg))
{
	return yforward(arg);
}

#	if YCL_Win32
YF_API std::string
EncodeArg(const char*);
inline PDefH(std::string, EncodeArg, const std::string& arg)
	ImplRet(EncodeArg(&arg[0]))
#	endif
template<typename _type
#if YCL_Win32
	, yimpl(typename = ystdex::enable_if_t<!std::is_constructible<std::string,
		_type&&>::value>)
#endif
	>
yconstfn auto
EncodeArg(_type&& arg) -> decltype(yforward(arg))
{
	return yforward(arg);
}
//@}


#	if !YCL_Android
//! \since build 560
//@{
/*!
\brief 终端数据。
\note 非公开实现。
*/
class TerminalData;

/*!
\brief 终端。
\note 非 Win32 平台使用 \c tput 实现，多终端改变当前屏幕时可能导致未预期的行为。
\warning 非虚析构。
*/
class YF_API Terminal
{
private:
	std::unique_ptr<TerminalData> p_term;

public:
	/*!
	\brief 构造：使用标准流对应的文件指针。
	\pre 间接断言：参数非空。
	\note 非 Win32 平台下关闭参数指定的流的缓冲以避免 \tput 不同步。
	*/
	Terminal(std::FILE* = stdout);
	~Terminal();

	DefBoolNeg(explicit, bool(p_term))

	bool
	RestoreAttributes();

	bool
	UpdateForeColor(std::uint8_t);
};
//@}
#	endif

} // namespace platform_ex;

#endif

#endif

