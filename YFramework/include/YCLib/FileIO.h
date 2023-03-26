/*
	© 2011-2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file FileIO.h
\ingroup YCLib
\brief 平台相关的文件访问和输入/输出接口。
\version r3572
\author FrankHB <frankhb1989@gmail.com>
\since build 616
\par 创建时间:
	2015-07-14 18:50:35 +0800
\par 修改时间:
	2023-03-26 02:36 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::FileIO
*/


#ifndef YCL_INC_FileIO_h_
#define YCL_INC_FileIO_h_ 1

#include "YModules.h"
#include YFM_YCLib_Debug // for YB_NONNULL, YB_PURE, std::string, Nonnull,
//	string, ynothrow, ystdex::totally_ordered, std::uint64_t, std::FILE,
//	ystdex::enable_for_string_class_t, errno, u16string,
//	std::system_error, YTraceDe, array, std::wstring, string_view, wstring;
#include <ystdex/cstdio.h> // for ystdex::fixed_openmode;
#include YFM_YCLib_Reference // for unique_ptr_from;
#include <ios> // for std::ios_base::sync_with_stdio, std::ios_base::openmode;
#include <fstream> // for std::basic_filebuf, std::filebuf, std::wfilebuf,
//	std::basic_ifstream, std::basic_ofstream, std::basic_fstream,
//	std::ifstream, std::ofstream, std::fstream, std::wifstream, std::wofstream,
//	std::wfstream;
#if _LIBCPP_VERSION || __GLIBCXX__ || YB_IMPL_MSCPP
#include <istream> // for std::basic_istream;
#include <ostream> // for std::basic_ostream;
#	if __GLIBCXX__
#		include <ext/stdio_filebuf.h> // for __gnu_cxx::stdio_filebuf;
//	ystdex::exchange;
#	elif YB_IMPL_MSCPP
#		include <ystdex/cstdio.h> // for ystdex::openmode_conv;
#		include <locale> // for std::use_facet, std::codecvt;
#		include <stdio.h> // for ::_wfdopen;
#	endif
#endif
#include <iosfwd> // for std::istream, std::ostream;

namespace platform
{

/*!
\pre 间接断言：第一参数非空。
\since build 970
*/
//!@{
/*!
\brief 构造适合表示 \c std::fopen 模式的 \c char 字符串。
\note 字符类型非 \c char 时转换。
\note 若字符串不指定模式，转换结果未指定。
*/
//!@{
YB_ATTR_nodiscard YF_API YB_NONNULL(1) YB_PURE ystdex::fixed_openmode<>
MakeFixedMode(const char*) ynothrowv;
YB_ATTR_nodiscard YF_API YB_NONNULL(1) YB_PURE ystdex::fixed_openmode<>
MakeFixedMode(const char16_t*) ynothrowv;
//!@}

/*!
\brief 构造适合表示路径的 \c char 字符串。
\note 字符类型非 \c char 时转换。
*/
//!@{
template<class _tString = std::string>
YB_ATTR_nodiscard YB_NONNULL(1) YB_PURE inline _tString
MakePathString(const typename _tString::value_type* s)
{
	return _tString(Nonnull(s));
}
template<class _tString = string>
YB_ATTR_nodiscard YB_NONNULL(1) YB_PURE inline _tString
MakePathString(const typename _tString::value_type* s,
	typename _tString::allocator_type a)
{
	return _tString(Nonnull(s), a);
}
// NOTE: Use templates instead of functions allow specialized optimizations and
//	explicit template arguments, and they are more efficient at least with
//	x86_64-pc-linux G++ 12.1.
template<class _tString = std::string>
YB_ATTR_nodiscard YB_NONNULL(1) YB_PURE _tString
MakePathString(const char16_t*);
template<class _tString = string>
YB_ATTR_nodiscard YB_NONNULL(1) YB_PURE _tString
MakePathString(const char16_t*, typename _tString::allocator_type);
//! \note Win32 平台：因实现不直接访问左值，字符的动态类型可为布局兼容的整数类型。
//!@{
template<>
YB_ATTR_nodiscard YF_API YB_NONNULL(1) YB_PURE std::string
MakePathString<std::string>(const char16_t*);
template<>
YB_ATTR_nodiscard YF_API YB_NONNULL(1) YB_PURE string
MakePathString<string>(const char16_t*, string::allocator_type);
//!@}
//!@}
//!@}


/*!
\brief 文件模式类型。
\since build 626
*/
//!@{
#if YCL_Win32
using mode_t = unsigned short;
#else
using mode_t = ::mode_t;
#endif
//!@}


/*!
\brief 文件节点类别。
\since build 412
*/
enum class NodeCategory : std::uint_least32_t;

/*
\brief 取指定模式对应的文件节点类型。
\relates NodeCategory
\since build 658
*/
YB_ATTR_nodiscard YF_API YB_STATELESS NodeCategory
CategorizeNode(mode_t) ynothrow;


/*!
\brief 文件描述符包装类。
\note 除非另行指定，具有无异常抛出保证的操作失败时可能设置 errno 。
\note 不支持的平台操作失败设置 errno 为 ENOSYS 。
\note 除非另行指定，无异常抛出的操作使用值初始化的返回类型表示失败结果。
\note 以 \c int 为返回值的操作返回 \c -1 表示失败。
\note 满足 NullablePointer 要求。
\note 满足共享锁要求。
\see WG21 N4606 17.6.3.3[nullablepointer.requirements] 。
\see WG21 N4606 30.4.1.4[thread.sharedmutex.requirements] 。
\since build 565
*/
class YF_API FileDescriptor : private ystdex::totally_ordered<FileDescriptor>
{
public:
	/*!
	\ingroup deleters
	\brief 删除器。
	\since build 624
	*/
	struct YF_API Deleter
	{
		using pointer = FileDescriptor;

		//! \since build 671
		void
		operator()(pointer) const ynothrow;
	};

private:
	//! \since build 554
	int desc = -1;

public:
	yconstfn DefDeCtor(FileDescriptor)
	yconstfn
	FileDescriptor(int fd) ynothrow
		: desc(fd)
	{}
	//! \since build 644
	yconstfn
	FileDescriptor(nullptr_t) ynothrow
	{}
	/*!
	\brief 构造：使用标准流。
	\note 对空参数不设置 errno 。

	当参数为空时得到表示无效文件的空描述符，否则调用 POSIX \c fileno 函数。
	*/
	FileDescriptor(std::FILE*) ynothrow;

	/*!
	\note 和 operator-> 不一致，不返回引用，以避免生存期问题。
	\since build 628
	*/
	YB_ATTR_nodiscard PDefHOp(int, *, ) const ynothrow
		ImplRet(desc)

	//! \since build 625
	YB_ATTR_nodiscard PDefHOp(FileDescriptor*, ->, ) ynothrow
		ImplRet(this)
	//! \since build 625
	YB_ATTR_nodiscard PDefHOp(const FileDescriptor*, ->, ) const ynothrow
		ImplRet(this)

	YB_ATTR_nodiscard explicit DefCvt(const ynothrow, bool, desc != -1)

	//! \since build 639
	YB_ATTR_nodiscard YB_PURE friend yconstfn PDefHOp(bool, ==,
		const FileDescriptor& x, const FileDescriptor& y) ynothrow
		ImplRet(x.desc == y.desc)

	//! \since build 639
	YB_ATTR_nodiscard YB_PURE friend yconstfn PDefHOp(bool, <,
		const FileDescriptor& x, const FileDescriptor& y) ynothrow
		ImplRet(x.desc < y.desc)

	//! \exception std::system_error 参数无效或调用失败。
	//!@{
	/*!
	\brief 取节点类别。
	\return 失败时为 NodeCategory::Invalid ，否则为对应类别。
	\since build 638
	*/
	YB_ATTR_nodiscard NodeCategory
	GetCategory() const ynothrow;
	//! \since build 719
	//!@{
	/*!
	\brief 取旗标。
	\note 非 POSIX 平台：不支持操作。
	*/
	YB_ATTR_nodiscard
	#if !YCL_API_POSIXFileSystem
	YB_NORETURN
	#endif
	int
	GetFlags() const;
	//! \brief 取模式。
	YB_ATTR_nodiscard mode_t
	GetMode() const;
	//!@}
	//!@}
	/*!
	\brief 取大小。
	\return 以字节计算的文件大小。
	\throw std::system_error 描述符无效或文件大小查询失败。
	\throw std::invalid_argument 文件大小查询结果小于 0 。
	\note 非常规文件或文件系统实现可能出错。
	\since build 628
	*/
	YB_ATTR_nodiscard std::uint64_t
	GetSize() const;

	/*!
	\throw std::system_error 调用失败或不支持操作。
	\note 非 POSIX 平台：不支持操作。
	\since build 719
	*/
	//!@{
	/*!
	\brief 设置阻塞模式。
	\return 是否需要并改变设置。
	\see http://pubs.opengroup.org/onlinepubs/9699919799/functions/fcntl.html 。
	*/
#if !YCL_API_POSIXFileSystem
	YB_NORETURN
#endif
	bool
	SetBlocking() const;
	//! \brief 设置旗标。
#if !YCL_API_POSIXFileSystem
	YB_NORETURN
#endif
	void
	SetFlags(int) const;
	//! \brief 设置访问模式。
#if !YCL_API_POSIXFileSystem
	YB_NORETURN
#endif
	void
	SetMode(mode_t) const;
	//!@}
	/*!
	\brief 设置非阻塞模式。
	\return 是否需要并改变设置。
	\throw std::system_error 调用失败或不支持操作。
	\note 非 POSIX 平台：不支持操作。
	\note 对不支持非阻塞的文件描述符， POSIX 未指定是否忽略 \c O_NONBLOCK 。
	\see http://pubs.opengroup.org/onlinepubs/9699919799/functions/fcntl.html 。
	\since build 719
	*/
#if !YCL_API_POSIXFileSystem
	YB_NORETURN
#endif
	bool
	SetNonblocking() const;
	//! \since build 625
	//!@{
	/*!
	\brief 调整文件至指定长度。
	\pre 指定文件需已经打开并可写。
	\return 是否成功。
	\note 不改变文件读写位置。

	若文件不足指定长度，扩展并使用空字节填充；否则保留起始指定长度的字节。
	*/
	YB_ATTR_nodiscard bool
	SetSize(size_t) ynothrow;
	/*!
	\brief 设置翻译模式。
	\note 兼容 Win32 文本模式行为的平台：参数和返回值意义和语义同 \c setmode 函数。
	\note 其它平台：无作用。
	\since build 637
	*/
#if !YCL_Win32
	YB_STATELESS
#endif
	int
	SetTranslationMode(int) const ynothrow;

	/*!
	\brief 刷新。
	\throw std::system_error 调用失败。
	\since build 711
	*/
	void
	Flush();

	/*!
	\pre 间接断言：文件有效。
	\pre 间接断言：指针参数非空。
	\note 每次读写首先清除 errno ；读写时遇 EINTR 时继续。
	*/
	//!@{
	/*!
	\brief 循环读写文件。
	*/
	//!@{
	/*!
	\note 每次读 0 字节时设置 errno 为 0 。
	\sa Read
	*/
	YB_NONNULL(2) size_t
	FullRead(void*, size_t) ynothrowv;

	/*!
	\note 每次写 0 字节时设置 errno 为 ENOSPC 。
	\sa Write
	*/
	YB_NONNULL(2) size_t
	FullWrite(const void*, size_t) ynothrowv;
	//!@}

	/*!
	\brief 读写文件。
	\note 首先清除 errno 。
	\note Win64 平台：大小截断为 32 位。
	\return 若发生错误为 size_t(-1) ，否则为读取的字节数。
	*/
	//!@{
	YB_NONNULL(2) size_t
	Read(void*, size_t) ynothrowv;

	YB_NONNULL(2) size_t
	Write(const void*, size_t) ynothrowv;
	//!@}
	//!@}
	//!@}

	/*!
	\brief 第二参数内容写入第一参数指定的文件。
	\pre 断言：文件有效。
	\pre 最后参数指定的缓冲区大小不等于 0 。
	\throw std::system_error 文件读写失败。
	\since build 634
	*/
	//!@{
	//! \pre 参数指定的缓冲区非空且大小不等于 0 。
	static YB_NONNULL(3) void
	WriteContent(FileDescriptor, FileDescriptor, byte*, size_t);
	/*!
	\note 最后一个参数指定缓冲区大小的上限，若分配失败自动重新分配。
	\throw std::bad_alloc 缓冲区分配失败。
	*/
	static void
	WriteContent(FileDescriptor, FileDescriptor,
		size_t = yimpl(size_t(BUFSIZ << 4)));
	//!@}

	/*!
	\pre 间接断言：文件有效。
	\note DS 平台：无作用。
	\note POSIX 平台：不使用 POSIX 文件锁而使用 BSD 代替，
		以避免无法控制的释放导致安全漏洞。
	\since build 721
	*/
	//!@{
	/*!
	\note Win32 平台：对内存映射文件为协同锁，其它文件为强制锁。
	\note 其它平台：协同锁。
	\warning 网络或分布式文件系统可能不能正确支持独占锁（如 Andrew File System ）。
	*/
	//!@{
	//! \throw std::system_error 文件锁定失败。
	//!@{
	void
	lock();

	void
	lock_shared();
	//!@}

	//! \return 是否锁定成功。
	//!@{
	bool
	try_lock() ynothrowv;

	bool
	try_lock_shared() ynothrowv;
	//!@}
	//!@}

	//! \pre 进程对文件访问具有所有权。
	//!@{
	void
	unlock() ynothrowv;

	void
	unlock_shared() ynothrowv;
	//!@}
	//!@}
};


//! \since build 626
using UniqueFile = unique_ptr_from<FileDescriptor::Deleter>;


/*!
\brief 取默认权限。
\since build 638
*/
YB_ATTR_nodiscard YF_API YB_STATELESS mode_t
DefaultPMode() ynothrow;

/*!
\brief 设置标准库流二进制输入/输出模式。
\pre 间接断言：参数非空。
\since build 669
*/
//!@{
YF_API void
SetBinaryIO(std::FILE*) ynothrowv;

/*!
\warning 改变默认日志默认发送器前，不应使用 std::cerr 和 std::clog
	等依赖 \c stderr 的流，以避免导致同步问题。
\sa FetchCommonLogger
\sa Logger::DefaultSendLog
*/
inline PDefH(void, SetupBinaryStdIO, std::FILE* in = stdin,
	std::FILE* out = stdout, bool sync = {}) ynothrowv
	ImplExpr(SetBinaryIO(in), SetBinaryIO(out),
		std::ios_base::sync_with_stdio(sync))
//!@}


/*!
\brief 在 POSIX 文件打开旗标扩展第一参数指定的二进制模式。
\return 扩展后的模式。
\note POSIX 平台：忽略第一参数。
\since build 923
*/
YB_ATTR_nodiscard YF_API YB_STATELESS int
oflag_extend_binary(std::ios_base::openmode, int) ynothrow;

/*!
\brief ISO C++ 标准输入输出接口打开模式转换为 POSIX 文件打开模式。
\return 若失败为 0 ，否则为对应的值。
\note 失败的返回值的具体含义依赖实现，可对应未指定或未定义的访问模式。
\note 忽略二进制模式。
\since build 648
*/
YB_ATTR_nodiscard YF_API YB_STATELESS int
omode_conv(std::ios_base::openmode) ynothrow;


/*!
\ingroup tags
\brief 使用 std::ios_base::openmode 的重载标签，保证非整数类型且可默认初始化。
\since build 923
*/
using use_openmode_t = yimpl(nullptr_t);


/*!
\pre 断言：第一参数非空。
\note 若在发生其它错误前存储分配失败，设置 errno 为 \c ENOMEM 。
*/
//!@{
/*!
\param filename 文件名，意义同 POSIX \c ::open 。
\param pmode 打开模式，基本语义同 POSIX.1 2004 ，具体行为取决于实现。
\since build 720
*/
//!@{
//! \param oflag 打开旗标，基本语义同 POSIX.1 2004 ，具体行为取决于实现。
//!@{
//! \brief 以 UTF-8 文件名无缓冲打开文件。
YB_ATTR_nodiscard YF_API YB_NONNULL(1) int
uopen(const char* filename, int oflag, mode_t pmode = DefaultPMode())
	ynothrowv;
//! \brief 以 UCS-2 文件名无缓冲打开文件。
YB_ATTR_nodiscard YF_API YB_NONNULL(1) int
uopen(const char16_t* filename, int oflag, mode_t pmode = DefaultPMode())
	ynothrowv;
//!@}
/*!
\param mode 打开模式，基本语义与 ISO C++11 对应，具体行为取决于实现。
\note 第二参数避免重载歧义。
\warning 不检查代开模式的转换是否失败。
\sa omode_conv
\since build 923

第三参数首先以 omode_conv 转换得到打开旗标，并经过以第三参数模式扩展二进制模式，
若转换失败，且 omode_conv 失败时的返回值的不对应实现已定义的值，则行为未定义。
继续同没有 use_openmode_t 参数的 uopen 重载的方式打开文件。
*/
//!@{
//! \brief 以 UTF-8 文件名无缓冲打开文件。
YB_ATTR_nodiscard YF_API YB_NONNULL(1) int
uopen(const char* filename, use_openmode_t, std::ios_base::openmode mode,
	mode_t pmode = DefaultPMode()) ynothrowv;
//! \brief 以 UCS-2 文件名无缓冲打开文件。
YB_ATTR_nodiscard YF_API YB_NONNULL(1) int
uopen(const char16_t* filename, use_openmode_t, std::ios_base::openmode mode,
	mode_t pmode = DefaultPMode()) ynothrowv;
//!@}
//!@}

//! \param filename 文件名，意义同 std::fopen 。
//!@{
/*!
\param mode 打开模式，基本语义同 ISO C11 ，具体行为取决于实现。
\pre 断言：<tt>mode && *mode != 0</tt> 。
\since build 669
*/
//!@{
//! \brief 以 UTF-8 文件名打开文件。
YB_ATTR_nodiscard YF_API YB_NONNULL(1, 2) std::FILE*
ufopen(const char* filename, const char* mode) ynothrowv;
//! \brief 以 UCS-2 文件名打开文件。
YB_ATTR_nodiscard YF_API YB_NONNULL(1, 2) std::FILE*
ufopen(const char16_t* filename, const char16_t* mode) ynothrowv;
//!@}
//! \param mode 打开模式，基本语义与 ISO C++11 对应，具体行为取决于实现。
//!@{
//! \brief 以 UTF-8 文件名打开文件。
YB_ATTR_nodiscard YF_API YB_NONNULL(1) std::FILE*
ufopen(const char* filename, std::ios_base::openmode mode) ynothrowv;
//! \brief 以 UCS-2 文件名打开文件。
YB_ATTR_nodiscard YF_API YB_NONNULL(1) std::FILE*
ufopen(const char16_t* filename, std::ios_base::openmode mode) ynothrowv;
//!@}

/*!
\note 使用同 ufopen 兼容的方式打开测试实现。
\note 第二参数含义同 ystdex::fexists 。
\sa ystdex::fexists
\since build 970
*/
//!@{
//! \brief 判断指定 UTF-8 文件名的文件是否存在。
//!@{
YB_ATTR_nodiscard YF_API YB_NONNULL(1, 2) bool
ufexists(const char*, const char* = "rb") ynothrowv;
YB_ATTR_nodiscard YF_API YB_NONNULL(1) bool
ufexists(const char*, std::ios_base::openmode) ynothrowv;
//!@}
//! \brief 判断指定 UCS-2 文件名的文件是否存在。
//!@{
YB_ATTR_nodiscard YF_API YB_NONNULL(1, 2) bool
ufexists(const char16_t*, const char16_t* = u"rb") ynothrowv;
YB_ATTR_nodiscard YF_API YB_NONNULL(1) bool
ufexists(const char16_t*, std::ios_base::openmode) ynothrowv;
//!@}
//!@}

/*!
\param filename 文件名，意义同 POSIX \c ::popen 。
\param mode 打开模式，基本语义同 POSIX.1 2004 ，具体行为取决于实现。
\pre 断言：\c filename 。
\pre 间接断言：\c mode 。
\note 不支持的平台操作失败设置 errno 为 ENOSYS 。
\warning 应使用 upclose 而不是 ::close 关闭管道流，否则可能引起未定义行为。
\sa upclose
\since build 566
*/
//!@{
//! \brief 以 UTF-8 文件名无缓冲打开管道流。
YB_ATTR_nodiscard YF_API YB_NONNULL(1, 2) std::FILE*
upopen(const char* filename, const char* mode) ynothrowv;
//! \brief 以 UCS-2 文件名无缓冲打开管道流。
YB_ATTR_nodiscard YF_API YB_NONNULL(1, 2) std::FILE*
upopen(const char16_t* filename, const char16_t* mode) ynothrowv;
//!@}
//!@}

/*!
\brief 关闭管道流。
\pre 参数非空，表示通过和 upopen 或使用相同实现打开的管道流。
\note 基本语义同 POSIX.1 2004 的 \c ::pclose ，具体行为取决于实现。
\since build 566
*/
YB_ATTR_nodiscard YF_API YB_NONNULL(1) int
upclose(std::FILE*) ynothrowv;
//!@}


/*!
\ingroup workarounds
\since build 616
*/
//!@{
#if _LIBCPP_VERSION || __GLIBCXX__ || YB_IMPL_MSCPP
/*!
\note 扩展打开模式。
\since build 721
*/
//!@{
#	if _LIBCPP_VERSION
yconstexpr const auto ios_nocreate(yimpl(std::ios::yimpl(trunc << 1)));
yconstexpr const auto ios_noreplace(yimpl(std::ios::yimpl(trunc << 2)));
#	elif __GLIBCXX__
//! \brief 表示仅打开已存在文件而不创建文件的模式。
yconstexpr const auto ios_nocreate(
	std::ios_base::openmode(std::_Ios_Openmode::yimpl(_S_trunc << 1)));
/*!
\brief 表示仅创建不存在文件的模式。
\note 可被 ios_nocreate 覆盖而不生效。
*/
yconstexpr const auto ios_noreplace(
	std::ios_base::openmode(std::_Ios_Openmode::yimpl(_S_trunc << 2)));
#	else
yconstexpr const auto ios_nocreate(std::ios::_Nocreate);
yconstexpr const auto ios_noreplace(std::ios::_Noreplace);
#	endif
//!@}


template<typename _tChar, class _tTraits = std::char_traits<_tChar>>
class basic_filebuf
#	if __GLIBCXX__
	: public yimpl(__gnu_cxx::stdio_filebuf<_tChar, _tTraits>)
#	else
	: public yimpl(std::basic_filebuf<_tChar, _tTraits>)
#	endif
{
public:
	using char_type = _tChar;
	using int_type = typename _tTraits::int_type;
	using pos_type = typename _tTraits::pos_type;
	using off_type = typename _tTraits::off_type;
	//! \since build 846
	using traits_type = _tTraits;

	//! \since build 620
	DefDeCtor(basic_filebuf)
#	if __GLIBCXX__
	using yimpl(__gnu_cxx::stdio_filebuf<_tChar, _tTraits>::stdio_filebuf);
#	else
	//! \since build 709
	using yimpl(std::basic_filebuf<_tChar, _tTraits>::basic_filebuf);
#	endif
	DefDeCopyMoveCtorAssignment(basic_filebuf)

public:
	/*!
	\since build 627
	\note 忽略扩展模式。
	*/
	basic_filebuf<_tChar, _tTraits>*
	open(UniqueFile p, std::ios_base::openmode mode)
	{
		if(p)
		{
			mode &= ~(ios_nocreate | ios_noreplace);
#	if _LIBCPP_VERSION
#		ifdef _LIBCPP_HAS_NO_GLOBAL_FILESYSTEM_NAMESPACE
		// XXX: See https://reviews.llvm.org/rL232049. This configuration shall
		//	not be used in the YCLib platforms.
#			error "Nonconforming implementation is not supported."
#		endif
			this->__open(*p.get(), mode);
			return this;
#	elif __GLIBCXX__
			this->_M_file.sys_open(*p.get(), mode);
			if(open_check(mode))
			{
				p.release();
				return this;
			}
#	else
			if(!this->is_open())
			{
				const auto fp(open_fd(*p.get(), mode));

				if(fp)
					p.release();
				return fp;
			}
#	endif
		}
		return {};
	}
	template<typename _tPathChar>
	std::basic_filebuf<_tChar, _tTraits>*
	open(const _tPathChar* s, std::ios_base::openmode mode)
	{
		if(!this->is_open())
		{
			// XXX: Now %std::_Fiopen is always not used as in Microsoft VC++'s
			//	<fstream> implementation, for some potential bugs. See
			//	https://github.com/microsoft/STL/issues/2093.
			// NOTE: To handle %ios_nocreate, %::ufopen is not used directly. To
			//	avoid redendant overhead of the mode conversion to the C file
			//	mode in the underlying calls, the result of %uopen is checked at
			//	first.
			const int fd(uopen(s, use_openmode_t(), mode));

			if(fd != -1)
			{
#	if _LIBCPP_VERSION || __GLIBCXX__
				const auto smode(mode & ~(ios_nocreate | ios_noreplace));

#		if _LIBCPP_VERSION
				// TODO: Is it worthy to support %_LIBCPP_HAS_OPEN_WITH_WCHAR?
				return this->__open(fd, smode);
#		elif __GLIBCXX__
				// XXX: If %smode is invalid, it would fail and to be checked.
				this->_M_file.sys_open(fd, smode);
				if(open_check(smode))
					return this;
#		endif
#	else
				// XXX: See the overload above.
				return open_fd(fd, mode);
#	endif
			}
		}
		return {};
	}
	//! \since build 618
	template<class _tString,
		yimpl(typename = ystdex::enable_for_string_class_t<_tString>)>
	std::basic_filebuf<_tChar, _tTraits>*
	open(const _tString& str, std::ios_base::openmode mode)
	{
		return open(str.c_str(), mode);
	}

private:
#	if __GLIBCXX__
	//! \since build 627
	YB_ATTR_nodiscard bool
	open_check(std::ios_base::openmode mode)
	{
		if(this->is_open())
		{
			this->_M_allocate_internal_buffer();
			this->_M_mode = mode;
			yunseq(this->_M_reading = {}, this->_M_writing = {});
			this->_M_set_buffer(-1);
			yunseq(this->_M_state_cur = this->_M_state_beg,
				this->_M_state_last = this->_M_state_beg);
			if((mode & std::ios_base::ate) && this->seekoff(0,
				std::ios_base::end) == pos_type(off_type(-1)))
				this->close();
			else
				return true;
			return true;
		}
		return {};
	}
#	elif YB_IMPL_MSCPP
	//! \since build 923
	YB_ATTR_nodiscard basic_filebuf<_tChar, _tTraits>*
	open_fd(int fd, std::ios_base::openmode mode)
	{
		// XXX: Assume Win32 using MSVCRT (with %::_wfdopen) here. Note
		//	traditional Windows (non-NT) versions without %::_wfdopen are not
		//	supported.
		if(const auto c_mode = ystdex::openmode_conv<wchar_t>(mode))
			// XXX: With MSVCRT, %::_fdopen may convert the encoding of the mode
			//	string internally before the call to %::_wfdopen. This is
			//	useless since the underlying API does not support mode valid
			//	mode strings to be encoded differently. Use wide-string
			//	%ystdex::openmode_conv to directly eliminate the unnecessary
			//	conversion.
			return open_file_ptr(::_wfdopen(fd, c_mode));
		return {};
	}

	//! \since build 837
	YB_ATTR_nodiscard YB_NONNULL(2) basic_filebuf<_tChar, _tTraits>*
	open_file_ptr(std::FILE* p_file)
	{
		// NOTE: %::_Filet and %std::_Filet were aliases of %::FILE in
		//	<cstdio>. This is not available since Microsoft VC++ 2017 15.8.
		if(p_file)
		{
			this->_Init(p_file, std::basic_filebuf<_tChar, _tTraits>::_Openfl);
			this->_Initcvt(
#		if YB_IMPL_MSCPP < 1914
				// NOTE: At least this works in Microsoft VC++ 2017 15.6
				//	(toolchain version 14.13.26128).
				&
#		endif
				std::use_facet<std::codecvt<_tChar, char,
				typename _tTraits::state_type>>(
				std::basic_streambuf<_tChar, _tTraits>::getloc()));
			return this;
		}
		return {};
	}
#	endif
};


//extern template class YF_API basic_filebuf<char>;
//extern template class YF_API basic_filebuf<wchar_t>;

using filebuf = basic_filebuf<char>;
using wfilebuf = basic_filebuf<wchar_t>;


//! \since build 619
//!@{
template<typename _tChar, class _tTraits = std::char_traits<_tChar>>
class basic_ifstream : public std::basic_istream<_tChar, _tTraits>
{
public:
	using char_type = _tChar;
	using int_type = typename _tTraits::int_type;
	using pos_type = typename _tTraits::pos_type;
	using off_type = typename _tTraits::off_type;
	using traits_type = _tTraits;

private:
	using base_type = std::basic_istream<char_type, traits_type>;

	mutable basic_filebuf<_tChar, _tTraits> fbuf{};

public:
	basic_ifstream()
		: base_type(nullptr)
	{
		this->init(&fbuf);
	}
	//! \since build 627
	template<typename _tParam,
		yimpl(typename = ystdex::exclude_self_t<basic_ifstream, _tParam>)>
	explicit
	basic_ifstream(_tParam&& s,
		std::ios_base::openmode mode = std::ios_base::in)
		: base_type(nullptr)
	{
		this->init(&fbuf);
		this->open(yforward(s), mode);
	}
	DefDelCopyCtor(basic_ifstream)
	basic_ifstream(basic_ifstream&& rhs)
		: base_type(std::move(rhs)),
		fbuf(std::move(rhs.fbuf))
	{
		base_type::set_rdbuf(&fbuf);
	}
	DefDeDtor(basic_ifstream)

	DefDelCopyAssignment(basic_ifstream)
	basic_ifstream&
	operator=(basic_ifstream&& rhs)
	{
		base_type::operator=(std::move(rhs));
		fbuf = std::move(rhs.fbuf);
		return *this;
	}

	void
	swap(basic_ifstream& rhs)
	{
		base_type::swap(rhs),
		fbuf.swap(rhs.fbuf);
	}

	void
	close()
	{
		if(!fbuf.close())
			this->setstate(std::ios_base::failbit);
	}

	YB_ATTR_nodiscard bool
	is_open() const
	{
		return fbuf.is_open();
	}

	//! \since build 627
	template<typename _tParam>
	void
	open(_tParam&& s,
		std::ios_base::openmode mode = std::ios_base::in)
	{
		if(fbuf.open(yforward(s), mode))
			this->clear();
		else
			this->setstate(std::ios_base::failbit);
	}

	YB_ATTR_returns_nonnull std::basic_filebuf<_tChar, _tTraits>*
	rdbuf() const
	{
		return &fbuf;
	}
};

template<typename _tChar, class _tTraits>
inline DefSwapMem(, basic_ifstream<_tChar YPP_Comma _tTraits>)


template<typename _tChar, class _tTraits = std::char_traits<_tChar>>
class basic_ofstream : public std::basic_ostream<_tChar, _tTraits>
{
public:
	using char_type = _tChar;
	using int_type = typename _tTraits::int_type;
	using pos_type = typename _tTraits::pos_type;
	using off_type = typename _tTraits::off_type;
	using traits_type = _tTraits;

private:
	using base_type = std::basic_ostream<char_type, traits_type>;

	//! \since build 617
	mutable basic_filebuf<_tChar, _tTraits> fbuf{};

public:
	basic_ofstream()
		: base_type(nullptr)
	{
		this->init(&fbuf);
	}
	//! \since build 627
	template<typename _tParam,
		yimpl(typename = ystdex::exclude_self_t<basic_ofstream, _tParam>)>
	explicit
	basic_ofstream(_tParam&& s,
		std::ios_base::openmode mode = std::ios_base::out)
		: base_type(nullptr)
	{
		this->init(&fbuf);
		this->open(yforward(s), mode);
	}
	DefDelCopyCtor(basic_ofstream)
	basic_ofstream(basic_ofstream&& rhs)
		: base_type(std::move(rhs)),
		fbuf(std::move(rhs.fbuf))
	{
		base_type::set_rdbuf(&fbuf);
	}
	DefDeDtor(basic_ofstream)

	DefDelCopyAssignment(basic_ofstream)
	basic_ofstream&
	operator=(basic_ofstream&& rhs)
	{
		base_type::operator=(std::move(rhs));
		fbuf = std::move(rhs.fbuf);
		return *this;
	}

	void
	swap(basic_ofstream& rhs)
	{
		base_type::swap(rhs),
		fbuf.swap(rhs.fbuf);
	}

	void
	close()
	{
		if(!fbuf.close())
			this->setstate(std::ios_base::failbit);
	}

	YB_ATTR_nodiscard bool
	is_open() const
	{
		return fbuf.is_open();
	}

	//! \since build 627
	template<typename _tParam>
	void
	open(_tParam&& s, std::ios_base::openmode mode = std::ios_base::out)
	{
		if(fbuf.open(yforward(s), mode))
			this->clear();
		else
			this->setstate(std::ios_base::failbit);
	}

	YB_ATTR_returns_nonnull std::basic_filebuf<_tChar, _tTraits>*
	rdbuf() const
	{
		return &fbuf;
	}
};

template<typename _tChar, class _tTraits>
inline DefSwapMem(, basic_ofstream<_tChar YPP_Comma _tTraits>)
//!@}


template<typename _tChar, class _tTraits = std::char_traits<_tChar>>
class basic_fstream : public std::basic_iostream<_tChar, _tTraits>
{
public:
	using char_type = _tChar;
	using int_type = typename _tTraits::int_type;
	using pos_type = typename _tTraits::pos_type;
	using off_type = typename _tTraits::off_type;
	using traits_type = _tTraits;

private:
	using base_type = std::basic_iostream<char_type, traits_type>;

	//! \since build 617
	mutable basic_filebuf<_tChar, _tTraits> fbuf{};

public:
	basic_fstream()
		: base_type(nullptr)
	{
		this->init(&fbuf);
	}
	template<typename _tParam,
		yimpl(typename = ystdex::exclude_self_t<basic_fstream, _tParam>)>
	explicit
	basic_fstream(_tParam&& s,
		std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out)
		: base_type(nullptr)
	{
		this->init(&fbuf);
		this->open(yforward(s), mode);
	}
	DefDelCopyCtor(basic_fstream)
	basic_fstream(basic_fstream&& rhs)
		: base_type(std::move(rhs)),
		fbuf(std::move(rhs.fbuf))
	{
		base_type::set_rdbuf(&fbuf);
	}
	DefDeDtor(basic_fstream)

	DefDelCopyAssignment(basic_fstream)
	basic_fstream&
	operator=(basic_fstream&& rhs)
	{
		base_type::operator=(std::move(rhs));
		fbuf = std::move(rhs.fbuf);
		return *this;
	}

	void
	swap(basic_fstream& rhs)
	{
		base_type::swap(rhs),
		fbuf.swap(rhs.fbuf);
	}

	void
	close()
	{
		if(!fbuf.close())
			this->setstate(std::ios_base::failbit);
	}

	YB_ATTR_nodiscard bool
	is_open() const
	{
		return fbuf.is_open();
	}

	//! \since build 617
	template<typename _tParam>
	void
	open(_tParam&& s,
		std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out)
	{
		if(fbuf.open(yforward(s), mode))
			this->clear();
		else
			this->setstate(std::ios_base::failbit);
	}

	YB_ATTR_returns_nonnull std::basic_filebuf<_tChar, _tTraits>*
	rdbuf() const
	{
		return &fbuf;
	}
};

template<typename _tChar, class _tTraits>
inline DefSwapMem(, basic_fstream<_tChar YPP_Comma _tTraits>)


//extern template class YF_API basic_fstream<char>;
//extern template class YF_API basic_fstream<wchar_t>;

//! \since build 619
//!@{
using ifstream = basic_ifstream<char>;
using ofstream = basic_ofstream<char>;
//! \since build 616
using fstream = basic_fstream<char>;
using wifstream = basic_ifstream<wchar_t>;
using wofstream = basic_ofstream<wchar_t>;
//!@}
using wfstream = basic_fstream<wchar_t>;
#else
// TODO: Use VC++ extensions to support %char16_t path initialization.
using std::basic_filebuf;
using std::filebuf;
using std::wfilebuf;
//! \since build 619
//!@{
using std::basic_ifstream;
using std::basic_ofstream;
//! \since build 616
using std::basic_fstream;
using std::ifstream;
using std::ofstream;
//! \since build 616
using std::fstream;
using std::wifstream;
using std::wofstream;
//!@}
using std::wfstream;
#endif
//!@}


/*!
\brief 从流中输入字符串。
\pre 断言：第二参数非空。
\note Win32 平台：检查流是否使用控制台。若使用控制台，刷新同步的流并使用控制台输出。
\sa std::getline
\since build 902

无格式输入字符串行。
默认使用 std::getline ，但可对特定的流实现检查并使用不同的方式。
*/
YF_API void
StreamGet(std::istream&, string&);

/*!
\brief 向流中输出字符串。
\note Win32 平台：检查流是否使用控制台。若使用控制台，刷新流并使用控制台输出。
*/
//!@{
/*!
\pre 断言：参数非空。
\return 是否输出成功。
\since build 901

无格式输出字符串。
默认使用 std::fputs ，但可对特定的流实现检查并使用不同的方式。
*/
YF_API YB_NONNULL(1, 2) bool
StreamPut(std::FILE*, const char*);
/*!
\pre 断言：第二参数非空。
\sa ystdex::write_ntcts
\since build 901

无格式输出字符串。
默认使用 ystdex::write_ntcts ，但可对特定的流实现检查并使用不同的方式。
*/
YF_API YB_NONNULL(2) void
StreamPut(std::ostream&, const char*);
//!@}


/*!
\brief 尝试删除指定路径的文件后再以指定路径和模式创建常规文件。
\pre 间接断言：第一参数非空。
\note 参数依次表示目标路径、打开模式、覆盖目标允许的链接数和是否允许共享覆盖。
\note 第二参数被限制 Mode::User 内。
\note 若目标已存在，创建或覆盖文件保证目标文件链接数不超过第三参数指定的值。
\note 若目标已存在、链接数大于 1 且不允许写入共享则先删除目标。
\note 忽略目标不存在导致的删除失败。
\throw std::system_error 创建目标失败。
\since build 639
*/
YB_ATTR_nodiscard YF_API YB_NONNULL(1) UniqueFile
EnsureUniqueFile(const char*, mode_t = DefaultPMode(), size_t = 1, bool = {});

/*!
\brief 比较文件内容相等。
\throw std::system_error 文件按流打开失败。
\warning 读取失败时即截断返回，因此需要另行比较文件大小。
\sa IsNodeShared
\since build 658

首先比较文件节点，若为相同文件直接相等，否则清除 errno ，打开文件读取内容并比较。
*/
//!@{
//! \note 间接断言：参数非空。
//!@{
YB_ATTR_nodiscard YF_API YB_NONNULL(1, 2) bool
HaveSameContents(const char*, const char*, mode_t = DefaultPMode());
//! \since build 701
YB_ATTR_nodiscard YF_API YB_NONNULL(1, 2) bool
HaveSameContents(const char16_t*, const char16_t*, mode_t = DefaultPMode());
//!@}
/*!
\note 使用表示文件名称的字符串，仅用于在异常消息中显示（若为空则省略）。
\note 参数表示的文件已以可读形式打开，否则按流打开失败。
\note 视文件起始于当前读位置。
\since build 658
*/
YB_ATTR_nodiscard YF_API bool
HaveSameContents(UniqueFile, UniqueFile, const char*, const char*);
//!@}

} // namespace platform;

namespace platform_ex
{

/*!
\pre 间接断言：第一参数非空。
\since build 970
*/
//!@{
#if YCL_Win32
/*!
\brief 构造适合表示 \c std::fopen 模式的 \c char 字符串。
\note 字符类型非 \c char 时转换。
\note 若字符串不指定模式，转换结果未指定。
*/
//!@{
YB_ATTR_nodiscard YF_API YB_NONNULL(1) YB_PURE ystdex::fixed_openmode<wchar_t>
MakeFixedModeW(const wchar_t*) ynothrowv;
YB_ATTR_nodiscard YF_API YB_NONNULL(1) YB_PURE ystdex::fixed_openmode<wchar_t>
MakeFixedModeW(const char*) ynothrowv;
//!@}
#endif

//! \brief 构造适合表示路径的 UCS-2 字符串。
//!@{
#if YCL_Win32
//! \note 字符类型非 \c wchar_t 时转换。
//!@{
template<class _tString = std::wstring>
YB_ATTR_nodiscard YB_NONNULL(1) YB_PURE inline _tString
MakePathStringW(const typename _tString::value_type* s)
{
	return _tString(Nonnull(s));
}
template<class _tString = wstring>
YB_ATTR_nodiscard YB_NONNULL(1) YB_PURE inline _tString
MakePathStringW(const typename _tString::value_type* s,
	typename _tString::allocator_type a)
{
	return _tString(Nonnull(s), a);
}
// NOTE: As %MakePathString.
template<class _tString = std::wstring>
YB_ATTR_nodiscard YB_NONNULL(1) YB_PURE _tString
MakePathStringW(const char*);
template<class _tString = wstring>
YB_ATTR_nodiscard YB_NONNULL(1) YB_PURE _tString
MakePathStringW(const char*, typename _tString::allocator_type);
template<>
YB_ATTR_nodiscard YF_API YB_NONNULL(1) YB_PURE std::wstring
MakePathStringW<std::wstring>(const char*);
template<>
YB_ATTR_nodiscard YF_API YB_NONNULL(1) YB_PURE wstring
MakePathStringW<wstring>(const char*, wstring::allocator_type);
//!@}
#else
//! \note 字符类型非 \c char16_t 时转换。
//!@{
template<class _tString = std::u16string>
YB_ATTR_nodiscard YB_NONNULL(1) YB_PURE inline _tString
MakePathStringU(const typename _tString::value_type* s)
{
	return _tString(Nonnull(s));
}
template<class _tString = u16string>
YB_ATTR_nodiscard YB_NONNULL(1) YB_PURE inline _tString
MakePathStringU(const typename _tString::value_type* s,
	typename _tString::allocator_type a)
{
	return _tString(Nonnull(s), a);
}
// NOTE: As %MakePathString.
template<class _tString = std::u16string>
YB_ATTR_nodiscard YB_NONNULL(1) YB_PURE _tString
MakePathStringU(const char*);
template<class _tString = u16string>
YB_ATTR_nodiscard YB_NONNULL(1) YB_PURE _tString
MakePathStringU(const char*, typename _tString::allocator_type);
template<>
YB_ATTR_nodiscard YF_API YB_NONNULL(1) YB_PURE std::u16string
MakePathStringU<std::u16string>(const char*);
template<>
YB_ATTR_nodiscard YF_API YB_NONNULL(1) YB_PURE u16string
MakePathStringU<u16string>(const char*, u16string::allocator_type);
//!@}
#endif
//!@}
//!@}

} // namespace platform_ex;

#endif

