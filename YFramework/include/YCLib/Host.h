/*
	© 2014-2022 FrankHB.

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
\version r677
\author FrankHB <frankhb1989@gmail.com>
\since build 492
\par 创建时间:
	2014-04-09 19:03:55 +0800
\par 修改时间:
	2022-01-31 20:01 +0800
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
#include YFM_YCLib_Container // for unordered_map, pair, string_view, string,
//	ystdex::invoke;
#include YFM_YSLib_Core_YException // for YSLib::LoggedEvent;
#include YFM_YCLib_Reference // for unique_ptr_from, unique_ptr, observer_ptr,
//	tidy_ptr, make_observer;
#include <system_error> // for std::system_error;
#include <cstdio> // for std::FILE;
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
	//! \since build 861
	Exception(std::error_code, const std::string&,
		YSLib::RecordLevel = YSLib::Emergent);
	Exception(std::error_code, string_view,
		YSLib::RecordLevel = YSLib::Emergent);
	YB_NONNULL(4)
	Exception(int, const std::error_category&, const char*
		= "unknown host exception", YSLib::RecordLevel = YSLib::Emergent);
	//! \since build 861
	Exception(int, const std::error_category&, const std::string&,
		YSLib::RecordLevel = YSLib::Emergent);
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
\warning 非虚析构。
\since build 592
*/
struct YF_API HandleDelete
{
	/*!
	\warning 默认空句柄作为空值；对不同的 Win32 API 可能需要额外检查。
	\see http://blogs.msdn.com/b/oldnewthing/archive/2004/03/02/82639.aspx 。
	*/
	using pointer = ::HANDLE;

	//! \since build 712
	void
	operator()(pointer) const ynothrowv;
};
#	elif YCL_API_Has_unistd_h
//! \since build 592
using HandleDelete = platform::FileDescriptor::Deleter;
#	else
#		error "Unsupported platform found."
#	endif

//! \since build 520
using UniqueHandle = unique_ptr_from<HandleDelete>;


/*!
\brief 信号量。
\note 满足 Lockable 要求。
\note 只支持命名信号量。
\see http://stackoverflow.com/questions/27736618/why-are-sem-init-sem-getvalue-sem-destroy-deprecated-on-mac-os-x-and-w 。
\since build 720
*/
class YF_API Semaphore : private ystdex::noncopyable
{
public:
#if YCL_Win32
	using CountType = long;
#else
	using CountType = unsigned;
#endif

private:
#if YCL_Win32
	using Deleter = HandleDelete;
#else
	class YF_API Deleter
	{
	public:
		using pointer = void*;

		observer_ptr<Semaphore> Referent;

		/*!
		\since build 793
		\see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=58328.
		*/
		DefDeCtor(Deleter)
		void
		operator()(pointer) const ynothrowv;
	};
#endif

public:
	using native_handle_type = typename Deleter::pointer;

private:
#if !YCL_Win32
	string name{};
#endif
	unique_ptr_from<Deleter> h_sem;

public:
	/*!
	\brief 以指定名称和初始值创建命名信号量。
	\pre 间接断言：参数非空。
	\see https://msdn.microsoft.com/library/windows/desktop/ms682438(v=vs.85).aspx 。
	\see http://pubs.opengroup.org/onlinepubs/9699919799/functions/sem_open.html 。

	若指定名称的信号量已存在，则打开此信号量，忽略名称以外的参数。
	名称长度和受到平台限制。
	Win32 平台：使用 \c ::CreateSemaphore 创建。
	非 Win32 平台：使用 POSIX \c ::sem_open 创建。
	若需可移植性，名称不应包含文件分隔符。
	*/
	//@{
	YB_NONNULL(2)
	Semaphore(const char*, CountType = 1);
	YB_NONNULL(2)
	Semaphore(const char16_t*, CountType = 1);
	//@}

	//! \note 允许递归锁。
	void
	lock();

	//! \since build 721
	bool
	try_lock() ynothrow;

	/*!
	\note 和互斥量不同，在满足 Lockable 要求之外，
		可从不具有所有权的线程或进程安全调用。
	*/
	void
	unlock() ynothrow;

	/*!
	\note Win32 平台：实际为 \c ::HANDLE 类型。
	\note POSIX 平台：实际为 \c ::sem_t* 类型。
	\see WG21 N4606 30.2.3[thread.req.native] 。
	*/
	PDefH(native_handle_type, native_handle, )
		ImplRet(h_sem.get())
};


/*!
\brief 创建管道。
\return 用于管道两端读写的文件句柄对。
\throw std::system_error 创建失败或设置管道读写属性失败。
\since build 593
\todo 允许设置继承性。

创建匿名管道并关联其中文件的读写权限。
*/
YF_API pair<UniqueHandle, UniqueHandle>
MakePipe();


/*!
\brief 从外部环境编码字符串参数或解码为外部环境字符串参数。
\pre Win32 平台可能间接断言：参数的数据指针非空。
\since build 593

Win32 平台：调用当前代码页的 platform::MBCSToMBCS 编解码字符串。
其它平台：直接传递参数。
和 platform::MBCSToMBCS 不同，参数可转换为 \c string_view 时，
作为 NTCTS 计算字符串长度。
用于传递可能不统一编码的文本参数时统一调用形式，避免过多的根据平台的条件编译判断。
若参数可能是 string 或 <tt>const char*</tt> 类型时，可直接使用 \c &arg[0] 的形式。
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
		!std::is_constructible<string_view, _type>::value>)
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
//! \since build 742
inline PDefH(string, EncodeArg, string_view arg)
	ImplRet(EncodeArg(arg.data()))
#	endif
template<typename _type
#if YCL_Win32
	, yimpl(typename = ystdex::enable_if_t<
		!std::is_constructible<string_view, _type>::value>)
#endif
	>
yconstfn auto
EncodeArg(_type&& arg) -> decltype(yforward(arg))
{
	return yforward(arg);
}
//@}


/*!
\brief 终端数据接口。
\note 非公开实现。
\since build 921
*/
FwdDeclI(ITerminalData)

/*!
\brief 终端。
\note 多终端改变当前屏幕时可能引起未预期的行为。
\warning 非虚析构。
\since build 560
\todo 外部命令的 termcap 支持。

对底层控制台接口封装的设备接口，提供终端控制操作和兼容终端的输出操作。
当非控制台文件关联的流初始化，或不存在可用的终端接口时，终端控制操作无作用。
Win32 平台：
	若以控制台文件关联的流初始化，使用 Win32 控制台 API 实现；
	否则，当检查当前终端为模拟终端时，使用同非 Win32 平台的方式实现。
非 Win32 平台：
	若以控制台文件关联的流初始化：
		若环境变量 YF_Use_tput 的值非空，使用外部命令 \c tput 实现；
		否则，使用内建的控制序列（符合 ISO/IEC 6429:1992 的 ANSI 转义序列）；
	否则，操作无作用。
当前终端检查为模拟终端，当且仅当 Win32 平台下：
	非控制台文件关联的流初始化；
	 MSYS 或 Cygwin 通过管道模拟 PTY 。
当前使用 \c tput 的实现假定使用 terminfo ，暂不支持 termcap ；
	因此，使用外部命令的实现不支持 FreeBSD 等只使用 termcap 的终端环境。
*/
class YF_API Terminal
{
public:
	/*!
	\brief 终端界面状态守卫。
	\since build 624
	*/
	class YF_API Guard final
	{
	private:
		tidy_ptr<Terminal> p_terminal;

	public:
		//! \since build 766
		template<typename _fCallable, typename... _tParams>
		Guard(Terminal& te, _fCallable f, _tParams&&... args)
			: p_terminal(make_observer(&te))
		{
			if(te)
				ystdex::invoke(f, te, yforward(args)...);
		}
		//! \since build 755
		DefDeMoveCtor(Guard)
		//! \brief 析构：重置终端属性，截获并记录错误。
		~Guard();

		//! \since build 755
		DefDeMoveAssignment(Guard)

		/*!
		\brief 转换为流操纵子。
		\since build 756
		*/
		DefCvt(const ynothrow, ystdex::add_ptr_t<
			std::ios_base&(std::ios_base&)>, ystdex::id<std::ios_base&>())
	};

private:
	//! \since build 921
	unique_ptr<ITerminalData> p_data;

public:
	/*!
	\brief 构造：使用标准流对应的文件指针。
	\pre 间接断言：参数非空。
	\note 非 Win32 平台下关闭参数指定的流的缓冲以避免 \tput 不同步。
	*/
	Terminal(std::FILE* = stdout);
	~Terminal();

	//! \brief 判断终端有效或无效。
	DefBoolNeg(explicit, bool(p_data))

	//! \since build 755
	//@{
	//! \brief 清除显示的内容。
	bool
	Clear();

	/*!
	\brief 临时固定前景色。
	\sa UpdateForeColor
	*/
	Guard
	LockForeColor(std::uint8_t);
	//@}

	bool
	RestoreAttributes();

	bool
	UpdateForeColor(std::uint8_t);

	/*!
	\brief 输出 UTF-8 NTCTS 。
	\pre 断言：参数非空。
	\note 仅当不可使用终端输出时使用第一参数指定的流。
	\return 成功输出了所有字符。
	\since build 905
	*/
	YB_NONNULL(2, 3) bool
	WriteString(std::FILE*, const char*);
};

/*!
\brief 根据等级设置终端的前景色。
\return 终端是否有效。
\note 当终端无效时忽略。
\relates Terminal
\since build 624
*/
YF_API bool
UpdateForeColorByLevel(Terminal&, YSLib::RecordLevel);

} // namespace platform_ex;
#endif

#endif

