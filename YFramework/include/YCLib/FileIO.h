/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file FileIO.h
\ingroup YCLib
\brief 平台相关的文件访问和输入/输出接口。
\version r1092
\author FrankHB <frankhb1989@gmail.com>
\since build 616
\par 创建时间:
	2015-07-14 18:50:35 +0800
\par 修改时间:
	2015-08-25 22:24 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::FileIO
*/


#ifndef YCL_INC_FileIO_h_
#define YCL_INC_FileIO_h_ 1

#include "YModules.h"
#include YFM_YCLib_Platform
#include YFM_YBaseMacro
#include <ystdex/string.hpp> // for ynothrow, ystdex::enable_for_string_class_t,
//	std::uint64_t;
#include <cstdio> // for std::FILE;
#include YFM_YCLib_Reference // for unique_ptr;
#include <ios> // for std::ios_base::sync_with_stdio;
#include <fstream> // for std::filebuf;
#if __GLIBCXX__
#	include <ext/stdio_filebuf.h> // for __gnu_cxx::stdio_filebuf;
#	include <ystdex/utility.hpp> // for ystdex::swap_underlying,
//	ystdex::exchange;
#endif
#include <system_error> // for std::system_error;
#include <chrono> // for std::chrono::nanoseconds;

namespace platform
{

/*!
\brief 文件模式类型。
\since build 626
*/
//@{
#if YCL_Win32
using mode_t = unsigned short;
#else
using mode_t = ::mode_t;
#endif
//@}

/*!
\brief 文件描述符包装类。
\note 满足 NullablePointer 要求。
\see ISO WG21/N4140 17.6.3.3[nullablepointer.requirements] 。
\since build 565
*/
class YF_API FileDescriptor
{
public:
	/*!
	\brief 删除器。
	\since build 624
	*/
	struct YF_API Deleter
	{
		using pointer = FileDescriptor;

		void
		operator()(pointer) ynothrow;
	};

private:
	//! \since build 554
	int desc;

public:
	FileDescriptor() ynothrow
		: desc(-1)
	{}
	FileDescriptor(int fd) ynothrow
		: desc(fd)
	{}
	/*!
	\brief 构造：使用标准流。
	\note 对非空参数可能设置 \c errno 。

	当参数为空时得到无效文件空描述符，否则调用 POSIX \c fileno 函数。
	*/
	FileDescriptor(std::FILE*) ynothrow;
	FileDescriptor(std::nullptr_t) ynothrow
		: desc(-1)
	{}

	//! \since build 625
	//@{
	PDefHOp(int, *, ) ynothrow
		ImplRet(desc)

	PDefHOp(FileDescriptor*, ->, ) ynothrow
		ImplRet(this)
	PDefHOp(const FileDescriptor*, ->, ) const ynothrow
		ImplRet(this)
	//@}

	explicit DefCvt(const ynothrow, bool, desc != -1)

	friend PDefHOp(bool, ==, const FileDescriptor& x, const FileDescriptor& y)
		ImplRet(x.desc == y.desc)
	friend PDefHOp(bool, !=, const FileDescriptor& x, const FileDescriptor& y)
		ImplRet(x.desc != y.desc)

	//! \since build 624
	//@{
	/*!
	\brief 取文件的修改时间。
	\return 以 POSIX 时间相同历元的时间间隔。
	\throw FileOperationFailure 参数无效或文件修改时间查询失败。
	\note 当前 Windows 使用 \c ::GetFileTime 实现，其它只保证最高精确到秒。
	*/
	YF_API std::chrono::nanoseconds
	GetModificationTime();
	/*
	\brief 取文件的大小。
	\return 以字节计算的文件大小。
	\throw FileOperationFailure 参数无效或文件大小查询失败。
	*/
	std::uint64_t
	GetSize();

	/*!
	\brief 设置阻塞模式。
	\note 仅在非 Windows 平台有效。
	\note 可能设置 \c errno 。
	\return 是否进行了设置。
	\since build 625
	*/
	bool
	SetBlocking() const ynothrow;
	/*!
	\brief 设置模式。
	\note 参数和返回值意义和语义同 \c setmode 函数，在 Windows 以外的平台无作用。
	\since build 565
	*/
	int
	SetMode(int) const ynothrow;
	/*!
	\brief 设置非阻塞模式。
	\note 仅在 POSIX 平台有效。
	\note 对不支持非阻塞的平台， POSIX 未指定文件描述符是否忽略 \c O_NONBLOCK 。
	\note 可能设置 \c errno 。
	\return 是否进行了设置。
	\see http://pubs.opengroup.org/onlinepubs/9699919799/ 。
	*/
	bool
	SetNonblocking() const ynothrow;
	//@}
	//! \since build 625
	//@{
	/*!
	\brief 调整文件至指定长度。
	\pre 指定文件需已经打开并可写。
	\note 不改变文件读写位置。

	若文件不足指定长度，扩展并使用空字节填充；否则保留起始指定长度的字节。
	*/
	bool
	SetSize(size_t) ynothrow;

	//! \note 每次读写首先清除 \c errno ；读写时遇 \c EINTR 时继续。
	//@{
	/*!
	\brief 循环读写文件。
	*/
	//@{
	/*!
	\note 每次读 0 字节时设置 \c errno 为 0 。
	\sa Read
	*/
	YB_NONNULL(2) size_t
	FullRead(void*, size_t) ynothrowv;

	/*
	\note 每次写 0 字节时设置 \c errno 为 ENOSPC 。
	\sa Write
	*/
	YB_NONNULL(2) size_t
	FullWrite(const void*, size_t) ynothrowv;
	//@}

	/*!
	\brief 读写文件。
	\note 首先清除 \c errno 。
	\return 若发生错误为 size_t(-1) ，否则为读取的字节数。
	*/
	//@{
	YB_NONNULL(2) size_t
	Read(void*, size_t) ynothrowv;

	YB_NONNULL(2) size_t
	Write(const void*, size_t) ynothrowv;
	//@}
	//@}
	//@}
};


//! \since build 626
using UniqueFile = unique_ptr<FileDescriptor, FileDescriptor::Deleter>;


/*!
\brief 取默认权限。
\since build 626
*/
YF_API YB_STATELESS mode_t
GetDefaultPermissionMode() ynothrow;

/*!
\brief 设置标准库流二进制输入/输出模式。
\pre 间接断言：参数非空。
\since build 599
*/
//@{
YF_API void
SetBinaryIO(std::FILE*) ynothrow;

/*!
\warning 改变默认日志默认发送器前，不应使用 \c std::cerr 和 \c std::clog
	等依赖 \c stderr 的流，以避免导致同步问题。
\sa FetchCommonLogger
\sa Logger::DefaultSendLog
*/
inline PDefH(void, SetupBinaryStdIO, std::FILE* in = stdin,
	std::FILE* out = stdout, bool sync = {}) ynothrow
	ImplExpr(SetBinaryIO(in), SetBinaryIO(out),
		std::ios_base::sync_with_stdio(sync))
//@}

/*!
\brief 尝试关闭流：设置 \c error 后关闭参数指定的流，必要时重试。
\return 非 \c EINTR 的错误。
\note 首先清除 \c errno ；遇 \c EINTR 时重试。
\note 使用 \c std::fclose 关闭流。
\since build 616
*/
YB_NONNULL(1) int
TryClose(std::FILE*) ynothrow;


/*
\brief ISO C++ 标准输入输出接口打开模式转换为 POSIX 文件打开模式。
\return 若失败为 0 ，否则为对应的值。
\since build 617
*/
//@{
//! \note 忽略二进制模式。
YF_API int
omode_conv(std::ios_base::openmode);

/*!
\note 扩展：不忽略二进制模式。
\note POSIX 平台下同 omode_conv 。
*/
YF_API int
omode_convb(std::ios_base::openmode);
//@}

//! \pre 断言：第一参数非空。
//@{
/*!
\brief 测试路径可访问性。
\param path 路径，意义同 POSIX <tt>::open</tt> 。
\param amode 模式，基本语义同 POSIX.1 2004 ，具体行为取决于实现。 。
\note \c errno 在出错时会被设置，具体值由实现定义。
\since build 549
*/
//@{
YF_API int
uaccess(const char* path, int amode) ynothrow;
YF_API int
uaccess(const char16_t* path, int amode) ynothrow;
//@}

/*!
\param filename 文件名，意义同 POSIX \c ::open 。
\param oflag 打开标识，基本语义同 POSIX.1 2004 ，具体行为取决于实现。
\param pmode 打开模式，基本语义同 POSIX.1 2004 ，具体行为取决于实现。
\since build 626
*/
//@{
//! \brief 以 UTF-8 文件名无缓冲打开文件。
YF_API int
uopen(const char* filename, int oflag, mode_t pmode = 0) ynothrow;
//! \brief 以 UCS-2 文件名无缓冲打开文件。
YF_API int
uopen(const char16_t* filename, int oflag, mode_t pmode = 0) ynothrow;
//@}

//! \param filename 文件名，意义同 \c std::fopen 。
//@{
/*!
\param mode 打开模式，基本语义同 ISO C11 ，具体行为取决于实现。
\pre 断言：<tt>mode && *mode != 0</tt> 。
*/
//@{
/*!
\brief 以 UTF-8 文件名打开文件。
\since build 299
*/
YF_API std::FILE*
ufopen(const char* filename, const char* mode) ynothrow;
/*!
\brief 以 UCS-2 文件名打开文件。
\since build 324
*/
YF_API std::FILE*
ufopen(const char16_t* filename, const char16_t* mode) ynothrow;
//@}
/*!
\param mode 打开模式，基本语义与 ISO C++11 对应，具体行为取决于实现。
\since build 616
*/
//@{
//! \brief 以 UTF-8 文件名打开文件。
YF_API std::FILE*
ufopen(const char* filename, std::ios_base::openmode mode) ynothrow;
//! \brief 以 UCS-2 文件名打开文件。
YF_API std::FILE*
ufopen(const char16_t* filename, std::ios_base::openmode mode) ynothrow;
//@}
//@}
//@}

/*!
\note 使用 ufopen 二进制只读模式打开测试实现。
\pre 间接断言：参数非空。
*/
//@{
//! \brief 判断指定 UTF-8 文件名的文件是否存在。
YF_API YB_NONNULL(1) bool
ufexists(const char*) ynothrow;
//! \brief 判断指定 UCS-2 文件名的文件是否存在。
YF_API YB_NONNULL(1) bool
ufexists(const char16_t*) ynothrow;
//@}
/*!
\brief 判断指定字符串为文件名的文件是否存在。
\note 使用 NTCTS 参数 ufexists 实现。
*/
template<class _tString,
	yimpl(typename = ystdex::enable_for_string_class_t<_tString>)>
inline bool
ufexists(const _tString& str) ynothrow
{
	return platform::ufexists(str.c_str());
}

/*!
\brief 关闭管道流。
\param 基本语义同 POSIX.1 2004 的 <tt>::pclose</tt> ，具体行为取决于实现。
\pre 参数非空，表示通过和 upopen 或使用相同实现打开的管道流。
\since build 566
*/
YF_API YB_NONNULL(1) int
upclose(std::FILE*) ynothrow;

/*!
\param filename 文件名，意义同 POSIX <tt>::popen</tt> 。
\param mode 打开模式，基本语义同 POSIX.1 2004 ，具体行为取决于实现。
\pre 断言：\c filename 。
\pre 间接断言： \c mode 。
\warning 应使用 upclose 而不是 \c std::close 关闭管道流，否则行为可能未定义。
\since build 566
*/
//@{
//! \brief 以 UTF-8 文件名无缓冲打开管道流。
YF_API YB_NONNULL(1, 2) std::FILE*
upopen(const char* filename, const char* mode) ynothrow;
//! \brief 以 UCS-2 文件名无缓冲打开管道流。
YF_API YB_NONNULL(1, 2) std::FILE*
upopen(const char16_t* filename, const char16_t* mode) ynothrow;
//@}

/*!
\brief 取当前工作目录（ UCS-2 编码）复制至指定缓冲区中。
\param buf 缓冲区起始指针。
\param size 缓冲区长。
\return 若成功为 buf ，否则为空指针。
\note 当 <tt>!buf || size == 0</tt> 时失败，设置 \c errno 为 \c EINVAL 。
\note 指定的 size 不能容纳结果时失败，设置 \c errno 为 \c ERANGE 。
\note 若分配存储失败，设置 \c errno 为 \c ENOMEM 。
\since build 324
*/
YF_API YB_NONNULL(1) char16_t*
u16getcwd_n(char16_t* buf, size_t size) ynothrow;

/*
\pre 断言：参数非空。
\return 操作是否成功。
\note \c errno 在出错时会被设置，具体值由实现定义。
\note DS 使用 newlib 实现。 MinGW32 使用 MSVCRT 实现。 Android 使用 bionic 实现。
	其它 Linux 使用 GLibC 实现。
*/
//@{
/*!
\brief 切换当前工作路径至指定的 UTF-8 字符串。
\since build 476
*/
YF_API YB_NONNULL(1) bool
uchdir(const char*) ynothrow;

/*!
\brief 按 UTF-8 路径以默认权限新建一个目录。
\note 权限由实现定义： DS 使用最大权限； MinGW32 使用 _wmkdir 指定的默认权限。
\since build 475
*/
YF_API YB_NONNULL(1) bool
umkdir(const char*) ynothrow;

/*!
\brief 按 UTF-8 路径删除一个空目录。
\since build 475
*/
YF_API YB_NONNULL(1) bool
urmdir(const char*) ynothrow;

/*!
\brief 按 UTF-8 路径删除一个非目录文件。
\since build 476
*/
YF_API YB_NONNULL(1) bool
uunlink(const char*) ynothrow;

/*!
\brief 按 UTF-8 路径删除一个文件。
\since build 476
*/
YF_API YB_NONNULL(1) bool
uremove(const char*) ynothrow;
//@}


//! \since build 616
//@{
#if __GLIBCXX__
template<typename _tChar, class _tTraits = std::char_traits<_tChar>>
class basic_filebuf : public yimpl(__gnu_cxx::stdio_filebuf<_tChar, _tTraits>)
{
public:
	using char_type = _tChar;
	using int_type = typename _tTraits::int_type;
	using pos_type = typename _tTraits::pos_type;
	using off_type = typename _tTraits::off_type;
	using traits = _tTraits;

	//! \since build 620
	DefDeCtor(basic_filebuf)
	using yimpl(__gnu_cxx::stdio_filebuf<_tChar, _tTraits>::stdio_filebuf);
#	if YB_IMPL_GNUCPP && !(YB_IMPL_GNUCPP >= 50000 && __GLIBCXX__ > 20140922)
	//! \since build 620
	//@{
	DefDelCopyCtor(basic_filebuf)
	basic_filebuf(basic_filebuf&& rhs)
		: __gnu_cxx::stdio_filebuf<_tChar, _tTraits>()
	{
		assign(std::move(rhs)),
		yunseq(this->_M_pback = rhs._M_pback,
			this->_M_codecvt = rhs._M_codecvt);
	}

	DefDelCopyAssignment(basic_filebuf)
	basic_filebuf&
	operator=(basic_filebuf&& rhs)
	{
		this->close();
		ystdex::swap_underlying(this->_M_file, rhs._M_file),
		assign(std::move(rhs));
		return *this;
	}

private:
	void
	assign(basic_filebuf&& rhs)
	{
	//	static_cast<std::basic_streambuf<_tChar, _tTraits>&>(*this) = rhs;
		yunseq(
		this->_M_mode
			= ystdex::exchange(rhs._M_mode, std::ios_base::openmode(0)),
		this->_M_state_beg = std::move(rhs._M_state_beg),
		this->_M_state_cur = std::move(rhs._M_state_cur),
		this->_M_state_last = std::move(rhs._M_state_last),
		this->_M_buf = ystdex::exchange(rhs._M_buf, {}),
		this->_M_buf_size = ystdex::exchange(rhs._M_buf_size, 1U),
		this->_M_buf_allocated = ystdex::exchange(rhs._M_buf_allocated, {}),
		this->_M_ext_buf = ystdex::exchange(rhs._M_ext_buf, {}),
		this->_M_ext_buf_size = ystdex::exchange(rhs._M_ext_buf_size, 0),
		this->_M_ext_next = ystdex::exchange(rhs._M_ext_next, {}),
		this->_M_ext_end = ystdex::exchange(rhs._M_ext_end, {}),
		this->_M_reading = ystdex::exchange(rhs._M_reading, {}),
		this->_M_writing = ystdex::exchange(rhs._M_writing, {}),
		this->_M_pback_cur_save = ystdex::exchange(rhs._M_pback_cur_save, {}),
		this->_M_pback_end_save = ystdex::exchange(rhs._M_pback_end_save, {}),
		this->_M_pback_init = ystdex::exchange(rhs._M_pback_init, {})
		);
		rhs._M_set_buffer(-1);
		yunseq(rhs._M_state_last = rhs._M_state_beg,
			rhs._M_state_cur = rhs._M_state_beg);
	}
	//@}
#endif

public:
	template<typename _tPathChar>
	std::basic_filebuf<_tChar, _tTraits>*
	open(const _tPathChar* s, std::ios_base::openmode mode)
	{
		if(!this->is_open())
		{
			this->_M_file.sys_open(uopen(s, omode_convb(mode),
				GetDefaultPermissionMode()), mode);
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
					return this;
			}
		}
		return {};
	}
	//! \since build 618
	template<class _tString,
		yimpl(typename = ystdex::enable_for_string_class_t<_tString>)>
	std::basic_filebuf<_tChar, _tTraits>*
	open(const _tString& s, std::ios_base::openmode mode)
	{
		return open(s.c_str(), mode);
	}
};


//extern template class YF_API basic_filebuf<char>;
//extern template class YF_API basic_filebuf<wchar_t>;

using filebuf = basic_filebuf<char>;
using wfilebuf = basic_filebuf<wchar_t>;


//! \since build 619
//@{
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
		: base_type({})
	{
		this->init(&fbuf);
	}
	template<typename _tParam,
		yimpl(typename = ystdex::exclude_self_ctor_t<basic_ifstream, _tParam>)>
	explicit
	basic_ifstream(_tParam&& s,
		std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out)
		: base_type({})
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

#	if YB_IMPL_GNUCPP && YB_IMPL_GNUCPP >= 50000 && __GLIBCXX__ > 20140922
	void
	swap(basic_ifstream& rhs)
	{
		base_type::swap(rhs),
		fbuf.swap(rhs.fbuf);
	}
#	endif

	void
	close()
	{
		if(!fbuf.close())
			this->setstate(std::ios_base::failbit);
	}

	bool
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

	std::basic_filebuf<_tChar, _tTraits>*
	rdbuf() const
	{
		return &fbuf;
	}
};

#	if __GLIBCXX__ > 20140922
template<typename _tChar, class _tTraits>
inline DefSwap(, basic_ifstream<_tChar YPP_Comma _tTraits>)
#	endif


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
		: base_type({})
	{
		this->init(&fbuf);
	}
	template<typename _tParam,
		yimpl(typename = ystdex::exclude_self_ctor_t<basic_ofstream, _tParam>)>
	explicit
	basic_ofstream(_tParam&& s,
		std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out)
		: base_type({})
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

#	if YB_IMPL_GNUCPP && YB_IMPL_GNUCPP >= 50000 && __GLIBCXX__ > 20140922
	void
	swap(basic_ofstream& rhs)
	{
		base_type::swap(rhs),
		fbuf.swap(rhs.fbuf);
	}
#	endif

	void
	close()
	{
		if(!fbuf.close())
			this->setstate(std::ios_base::failbit);
	}

	bool
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

	std::basic_filebuf<_tChar, _tTraits>*
	rdbuf() const
	{
		return &fbuf;
	}
};

#	if __GLIBCXX__ > 20140922
template<typename _tChar, class _tTraits>
inline DefSwap(, basic_ofstream<_tChar YPP_Comma _tTraits>)
#	endif
//@}


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
		: base_type({})
	{
		this->init(&fbuf);
	}
	template<typename _tParam,
		yimpl(typename = ystdex::exclude_self_ctor_t<basic_fstream, _tParam>)>
	explicit
	basic_fstream(_tParam&& s,
		std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out)
		: base_type({})
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

#	if YB_IMPL_GNUCPP && YB_IMPL_GNUCPP >= 50000 && __GLIBCXX__ > 20140922
	void
	swap(basic_fstream& rhs)
	{
		base_type::swap(rhs),
		fbuf.swap(rhs.fbuf);
	}
#	endif

	void
	close()
	{
		if(!fbuf.close())
			this->setstate(std::ios_base::failbit);
	}

	bool
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

	std::basic_filebuf<_tChar, _tTraits>*
	rdbuf() const
	{
		return &fbuf;
	}
};

#	if __GLIBCXX__ > 20140922
template<typename _tChar, class _tTraits>
inline DefSwap(, basic_fstream<_tChar YPP_Comma _tTraits>)
#	endif


//extern template class YF_API basic_fstream<char>;
//extern template class YF_API basic_fstream<wchar_t>;

//! \since build 619
//@{
using ifstream = basic_ifstream<char>;
using ofstream = basic_ofstream<char>;
//! \since build 616
using fstream = basic_fstream<char>;
using wifstream = basic_ifstream<wchar_t>;
using wofstream = basic_ofstream<wchar_t>;
//@}
using wfstream = basic_fstream<wchar_t>;
#else
// TODO: Use VC++ extensions to support %char16_t path initialization.
using std::basic_filebuf;
using std::filebuf;
using std::wfilebuf;
//! \since build 619
//@{
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
//@}
using std::wfstream;
#endif
//@}


/*!
\brief 表示文件操作失败的异常。
\since build 411
*/
class YF_API FileOperationFailure : public std::system_error
{
public:
	//! \since build 538
	using system_error::system_error;

	//! \since build 586
	DefDeCopyCtor(FileOperationFailure)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~FileOperationFailure() override;
};


/*!
\sa FileDescriptor::GetModificationTime
\exception FileOperationFailure 参数无效或文件修改时间查询失败。
\since build 544
*/
//@{
YF_API YB_NONNULL(1) std::chrono::nanoseconds
GetFileModificationTimeOf(std::FILE*);
//! \pre 断言：参数非空。
//@{
YF_API YB_NONNULL(1) std::chrono::nanoseconds
GetFileModificationTimeOf(const char*);
YF_API YB_NONNULL(1) std::chrono::nanoseconds
GetFileModificationTimeOf(const char16_t*);
//@}
//! \note 使用 NTCTS 参数 GetFileModificationTimeOf 实现。
template<class _tString,
	yimpl(typename = ystdex::enable_for_string_class_t<_tString>)>
inline std::chrono::nanoseconds
GetFileModificationTimeOf(const _tString& str)
{
	return platform::GetFileModificationTimeOf(str.c_str());
}
//@}

/*!
\excption FileOperationFailure 参数无效或文件大小查询失败。
\sa FileDescriptor::GetSize
\since build 475
*/
YF_API YB_NONNULL(1) std::uint64_t
GetFileSizeOf(std::FILE*);

} // namespace platform;

#endif

