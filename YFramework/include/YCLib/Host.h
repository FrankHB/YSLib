/*
	© 2014-2016 FrankHB.

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
\version r365
\author FrankHB <frankhb1989@gmail.com>
\since build 492
\par 创建时间:
	2014-04-09 19:03:55 +0800
\par 修改时间:
	2016-05-09 13:29 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Host
*/


#ifndef YCL_INC_Host_h_
#define YCL_INC_Host_h_ 1

#include "YCLib/YModules.h"
#include "YSLib/Core/YModules.h"
#include YFM_YCLib_Container // for unordered_map, pair, string_view, string;
#include YFM_YSLib_Core_YException // for YSLib::LoggedEvent;
#include YFM_YCLib_Reference // for unique_ptr;
#include <system_error> // for std::system_error;
#if !YCL_Win32
#	include YFM_YCLib_FileIO // for platform::FileDescriptor::Deleter;
#else
//! \since build 564
using HANDLE = void*;
#endif

#if YF_Hosted

namespace platform_ex
{

/*!
\ingroup exceptions
\brief 宿主异常。
\since build 426
*/
class YF_API Exception : public std::system_error
{
private:
	//! \since build 624
	YSLib::RecordLevel level = YSLib::Emergent;

public:
	/*!
	\pre 间接断言：字符串参数对应的数据指针非空。
	\since build 643
	*/
	//@{
	YB_NONNULL(3)
	Exception(std::error_code, const char* = "unknown host exception",
		YSLib::RecordLevel = YSLib::Emergent);
	Exception(std::error_code, string_view,
		YSLib::RecordLevel = YSLib::Emergent);
	YB_NONNULL(4)
	Exception(int, const std::error_category&, const char*
		= "unknown host exception", YSLib::RecordLevel = YSLib::Emergent);
	Exception(int, const std::error_category&, string_view,
		YSLib::RecordLevel = YSLib::Emergent);
	//@}
	//! \since build 586
	DefDeCopyCtor(Exception)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~Exception() override;

	//! \since build 624
	DefGetter(const ynothrow, YSLib::RecordLevel, Level, level)
};


#	if YCL_Win32
/*!
\brief 句柄删除器。
\since build 592
*/
struct YF_API HandleDelete
{
	/*!
	\warning 只检查空句柄作为空值。对不同的 Win32 API 可能需要额外检查。
	\see http://blogs.msdn.com/b/oldnewthing/archive/2004/03/02/82639.aspx 。
	*/
	using pointer = ::HANDLE;

	void
	operator()(pointer) const ynothrow;
};
#	elif YCL_API_Has_unistd_h
//! \since build 592
using HandleDelete = platform::FileDescriptor::Deleter;
#	else
#		error "Unsupported platform found."
#	endif

//! \since build 520
using UniqueHandle = unique_ptr_from<HandleDelete>;


//! \since build 567
//@{
//! \brief 默认命令缓冲区大小。
yconstexpr const size_t DefaultCommandBufferSize(yimpl(4096));

/*!
\brief 取命令在标准输出上的执行结果。
\pre 间接断言：第一参数非空。
\return 读取的二进制存储。
\throw std::invalid_argument 第二参数的值等于 \c 0 。
\throw std::system_error 表示读取失败的派生类异常对象。
\note 第一参数指定命令；第二参数指定每次读取的缓冲区大小，先于执行命令进行检查。
\since build 658
*/
YF_API YB_NONNULL(1) string
FetchCommandOutput(const char*, size_t = DefaultCommandBufferSize);


//! \brief 命令和命令执行结果的缓冲区类型。
using CommandCache = unordered_map<string, string>;

/*!
\brief 锁定命令执行缓冲区。
\return 静态对象的非空锁定指针。
*/
YF_API YSLib::locked_ptr<CommandCache>
LockCommandCache();

//! \since build 593
//@{
//! \brief 取缓冲的命令执行结果。
YF_API const string&
FetchCachedCommandResult(const string&, size_t = DefaultCommandBufferSize);

//! \brief 取缓冲的命令执行结果字符串。
inline PDefH(string, FetchCachedCommandString, const string& cmd,
	size_t buf_size = DefaultCommandBufferSize)
	ImplRet(ystdex::trail(string(FetchCachedCommandResult(cmd, buf_size))))
//@}
//@}

/*!
\brief 创建管道。
\throw std::system_error 表示创建失败的派生类异常对象。
\since build 593
*/
YF_API pair<UniqueHandle, UniqueHandle>
MakePipe();


/*!
\brief 从外部环境编码字符串参数或解码为外部环境字符串参数。
\pre Win32 平台可能间接断言参数非空。
\since build 593

对 Win32 平台调用当前代码页的 platform::MBCSToMBCS 编解码字符串，其它直接传递参数。
此时和 platform::MBCSToMBCS 不同，参数可转换为 \c string_view 时长度通过 NTCTS 计算。
若需要使用 <tt>const char*</tt> 指针，可直接使用 <tt>&arg[0]</tt> 的形式。
*/
//@{
#	if YCL_Win32
YF_API YB_NONNULL(1) string
DecodeArg(const char*);
inline PDefH(string, DecodeArg, string_view arg)
	ImplRet(DecodeArg(&arg[0]))
#	endif
template<typename _type
#if YCL_Win32
	, yimpl(typename = ystdex::enable_if_t<
		!std::is_constructible<string_view, _type&&>::value>)
#endif
	>
yconstfn auto
DecodeArg(_type&& arg) -> decltype(yforward(arg))
{
	return yforward(arg);
}

#	if YCL_Win32
YF_API YB_NONNULL(1) string
EncodeArg(const char*);
inline PDefH(string, EncodeArg, const string& arg)
	ImplRet(EncodeArg(&arg[0]))
#	endif
template<typename _type
#if YCL_Win32
	, yimpl(typename = ystdex::enable_if_t<!std::is_constructible<string,
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
\note 非 Win32 平台使用 \c tput 实现，多终端改变当前屏幕时可能引起未预期的行为。
\warning 非虚析构。
*/
class YF_API Terminal
{
public:
	/*!
	\brief 终端界面状态守护。
	\since build 624
	*/
	class YF_API Guard final
	{
	private:
		Terminal& terminal;

	public:
		template<typename _func>
		Guard(Terminal& term, _func f)
			: terminal(term)
		{
			if(term)
				f(term);
		}
		//! \brief 析构：重置终端属性，截获并记录错误。
		~Guard();
	};

private:
	//! \since build 593
	unique_ptr<TerminalData> p_term;

public:
	/*!
	\brief 构造：使用标准流对应的文件指针。
	\pre 间接断言：参数非空。
	\note 非 Win32 平台下关闭参数指定的流的缓冲以避免 \tput 不同步。
	*/
	Terminal(std::FILE* = stdout);
	~Terminal();

	//! \brief 判断终端有效或无效。
	DefBoolNeg(explicit, bool(p_term))

	bool
	RestoreAttributes();

	bool
	UpdateForeColor(std::uint8_t);
};
//@}

/*!
\brief 根据等级设置终端的前景色。
\return 终端是否有效。
\note 当终端无效时忽略。
\relates Terminal
\since build 624
*/
YF_API bool
UpdateForeColorByLevel(Terminal&, YSLib::RecordLevel);
#	endif

} // namespace platform_ex;

#endif

#endif

