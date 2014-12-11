/*
	© 2012-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file FileSystem.h
\ingroup YCLib
\brief 平台相关的文件系统接口。
\version r1592
\author FrankHB <frankhb1989@gmail.com>
\since build 312
\par 创建时间:
	2012-05-30 22:38:37 +0800
\par 修改时间:
	2014-12-09 23:32 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::FileSystem
*/


#ifndef YCL_INC_FileSystem_h_
#define YCL_INC_FileSystem_h_ 1

#include "YModules.h"
#include YFM_YCLib_YCommon // for std::uint64_t, std::FILE*;
#include <ystdex/utility.hpp> // for std::is_array, std::is_integral,
//	ystdex::remove_reference_t, ystdex::arrlen;
#include <ystdex/cstring.h> // for ystdex::is_null;
#include <ystdex/path.hpp> // for ystdex::string_length, std::string,
//	ystdex::path_category;
#include "CHRLib/YModules.h"
#include YFM_CHRLib_Encoding
#include <system_error>
#if YCL_DS || YCL_MinGW || YCL_Linux
#	include <dirent.h>
#endif
#include <chrono> // for std::chrono::nanoseconds;
#include <ystdex/iterator.hpp> // for ystdex::indirect_input_iterator;

namespace platform
{

//! \since build 538
//@{
static_assert(std::is_same<CHRLib::ucs2_t, char16_t>::value,
	"Wrong character type found.");
static_assert(std::is_same<CHRLib::ucs4_t, char32_t>::value,
	"Wrong character type found.");
#if YCL_Win32
static_assert(sizeof(wchar_t) == sizeof(CHRLib::ucs2_t),
	"Wrong character type found.");
static_assert(yalignof(wchar_t) == yalignof(CHRLib::ucs2_t),
	"Inconsistent alignment between character types found.");
#endif
//@}

/*
\brief 判断字符串是否是当前路径。
\since build 409
*/
#define YCL_FS_StringIsCurrent(_s, _p) \
	(ystdex::string_length(_s) == 1 && _s[0] == YPP_Concat(_p, '.'))

/*
\brief 判断字符串是否是父目录。
\since build 409
*/
#define YCL_FS_StringIsParent(_s, _p) \
	(ystdex::string_length(_s) == 2 \
	&& _s[0] == YPP_Concat(_p, '.') && _s[1] == YPP_Concat(_p, '.'))

/*
\def YCL_FS_CharIsDelimiter
\brief 判断字符是否路径分隔符。
\since build 409
*/

/*
\def YCL_FS_StringIsRoot
\brief 判断字符是否表示根目录路径。
\since build 409
*/

#if YCL_Win32
	/*!
	\brief 文件路径分隔符。
	\since build 296
	*/
#	define YCL_PATH_DELIMITER '\\'
//	#define YCL_PATH_DELIMITER L'\\'
	/*!
	\brief 文件路径分界符。
	\since build 402
	*/
#	define YCL_PATH_SEPARATOR "\\"
//	#define YCL_PATH_SEPARATOR L"\\"
	/*!
	\brief 虚拟根目录路径。
	\since build 297
	*/
#	define YCL_PATH_ROOT YCL_PATH_SEPARATOR

/*!
\brief 本机路径字符类型。
\since build 296
*/
//	using NativePathCharType = wchar_t;
using NativePathCharType = char;

#	define YCL_FS_CharIsDelimiter(_c, _p) \
	(_c == YPP_Concat(_p, '/') || _c == YPP_Concat(_p, '\\'))
#	define YCL_FS_StringIsRoot(_s, _p) \
		(ystdex::string_length(_s) == 3 \
		&& _s[1] == ':' && YCL_FS_CharIsDelimiter(_s[2], _p))

/*!
\brief 路径字符串编码。
\since build 402
*/
yconstexpr CHRLib::CharSet::Encoding CS_Path(CHRLib::CharSet::UTF_8);
#elif defined(YCL_API_POSIXFileSystem)
	/*!
	\brief 文件路径分隔符。
	\since build 298
	*/
#	define YCL_PATH_DELIMITER '/'
	/*!
	\brief 文件路径分界符。
	\since build 402
	*/
#	define YCL_PATH_SEPARATOR "/"
	/*!
	\brief 根目录路径。
	*/
#	define YCL_PATH_ROOT YCL_PATH_SEPARATOR

/*!
\brief 本机路径字符类型。
\since build 286
*/
using NativePathCharType = char;

#	define YCL_FS_CharIsDelimiter(_c, _p) \
	(_c == YPP_Join(_p, YCL_PATH_DELIMITER))
#	define YCL_FS_StringIsRoot(_s, _p) (platform_ex::FS_IsRoot(&_s[0]))

/*!
\brief 路径字符串编码。
\since build 402
*/
yconstexpr CHRLib::CharSet::Encoding CS_Path(CHRLib::CharSet::UTF_8);
#else
#	error "Unsupported platform found."
#endif

//! \since build 402
//@{
static_assert(std::is_integral<decltype(YCL_PATH_DELIMITER)>::value,
	"Illegal type of delimiter found.");
static_assert(std::is_array<ystdex::remove_reference_t<decltype(
	YCL_PATH_SEPARATOR)>>::value, "Non-array type of separator found.");
//! \since build 458 as workaround for Visual C++ 2013
#if YB_HAS_CONSTEXPR
static_assert(ystdex::arrlen(YCL_PATH_SEPARATOR) == 2,
	"Wrong length of separator found.");
static_assert(YCL_PATH_SEPARATOR[0] == YCL_PATH_DELIMITER,
	"Mismatched path delimiter and separator found.");
static_assert(ystdex::is_null(YCL_PATH_SEPARATOR[1]),
	"Non-null-terminator as end of separator.");
#endif
//@}


/*!
\brief 文件描述符包装类。
\note 满足 NullablePointer 要求。
\see ISO WG21/N4140 17.6.3.3[nullablepointer.requirements] 。
\since build 553
*/
class YF_API file_desc
{
private:
	//! \since build 554
	int desc;

public:
	file_desc() ynothrow
		: desc(-1)
	{}
	file_desc(int fd) ynothrow
		: desc(fd)
	{}
	file_desc(std::nullptr_t) ynothrow
		: desc(-1)
	{}

	PDefHOp(int, *, )
		ImplRet(desc)

	explicit DefCvt(const ynothrow, bool, desc != -1)

	friend PDefHOp(bool, ==, const file_desc& x, const file_desc& y)
		ImplRet(x.desc == y.desc)
	friend PDefHOp(bool, !=, const file_desc& x, const file_desc& y)
		ImplRet(x.desc != y.desc)
};


/*!
\brief 文件描述符删除器。
\since build 553
*/
struct YF_API file_desc_deleter
{
	using pointer = file_desc;

	void
	operator()(pointer) ynothrow;
};


/*!
\brief 测试路径可访问性。
\param path 路径，意义同 POSIX <tt>::open</tt> 。
\param amode 模式，基本语义同 POSIX.1 2004 ，具体行为取决于实现。 。
\pre 断言：<tt>filename</tt> 。
\note \c errno 在出错时会被设置，具体值由实现定义。
\since build 549
*/
//@{
YF_API int
uaccess(const char* path, int amode) ynothrow;
YF_API int
uaccess(const char16_t* path, int amode) ynothrow;
//@}

//! \since build 324
//@{
/*!
\param filename 文件名，意义同 POSIX <tt>::open</tt> 。
\param oflag 打开标识，基本语义同 POSIX.1 2004 ，具体行为取决于实现。
\pre 断言：<tt>filename</tt> 。
*/
//@{
//! \brief 以 UTF-8 文件名无缓冲打开文件。
//@{
YF_API int
uopen(const char* filename, int oflag) ynothrow;
YF_API int
uopen(const char* filename, int oflag, int pmode) ynothrow;
//@}
//! \brief 以 UCS-2 文件名无缓冲打开文件。
//@{
YF_API int
uopen(const char16_t* filename, int oflag) ynothrow;
//! \param pmode 打开模式，基本语义同 POSIX.1 2004 ，具体行为取决于实现。
YF_API int
uopen(const char16_t* filename, int oflag, int pmode) ynothrow;
//@}

/*!
\param mode 打开模式，基本语义同 ISO C99 ，具体行为取决于实现。
\pre 断言：<tt>filename && mode && *mode != 0</tt> 。
*/
//@{
/*!
\brief 以 UTF-8 文件名打开文件。
\since build 299
*/
YF_API std::FILE*
ufopen(const char* filename, const char* mode) ynothrow;
//! \brief 以 UCS-2 文件名打开文件。
YF_API std::FILE*
ufopen(const char16_t* filename, const char16_t* mode) ynothrow;
//@}
//@}

/*!
\note 使用 ufopen 二进制只读模式打开测试实现。
\pre 间接断言：参数非空。
*/
//@{
//! \brief 判断指定 UTF-8 文件名的文件是否存在。
YF_API bool
ufexists(const char*) ynothrow;
//! \brief 判断指定 UCS-2 文件名的文件是否存在。
YF_API bool
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
\brief 取当前工作目录（ UCS-2 编码）复制至指定缓冲区中。
\param buf 缓冲区起始指针。
\param size 缓冲区长。
\return 若成功为 buf ，否则为空指针。
\note 当 <tt>!buf || size == 0</tt> 时失败，设置 \c errno 为 \c EINVAL 。
\note 指定的 size 不能容纳结果时失败，设置 \c errno 为 \c ERANGE 。
\note 若分配存储失败，设置 \c errno 为 \c ENOMEM 。
*/
YF_API char16_t*
u16getcwd_n(char16_t* buf, std::size_t size) ynothrow;
//@}

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
YF_API bool
uchdir(const char*) ynothrow;

/*!
\brief 按 UTF-8 路径以默认权限新建一个目录。
\note 权限由实现定义： DS 使用最大权限； MinGW32 使用 _wmkdir 指定的默认权限。
\since build 475
*/
YF_API bool
umkdir(const char*) ynothrow;

/*!
\brief 按 UTF-8 路径删除一个空目录。
\since build 475
*/
YF_API bool
urmdir(const char*) ynothrow;

/*!
\brief 按 UTF-8 路径删除一个非目录文件。
\since build 476
*/
YF_API bool
uunlink(const char*) ynothrow;

/*!
\brief 按 UTF-8 路径删除一个文件。
\since build 476
*/
YF_API bool
uremove(const char*) ynothrow;

/*!
\brief 截断文件至指定长度。
\pre 指定文件需已经打开并可写。
\note 不改变文件读写位置。
\since build 341

若文件不足指定长度，扩展并使用空字节填充；否则保留起始指定长度的字节。
*/
YF_API bool
truncate(std::FILE*, std::size_t) ynothrow;
//@}


/*!
\brief 取文件的修改时间。
\return 以 POSIX 时间相同历元的时间间隔。
\throw FileOperationFailure 参数无效或文件修改时间查询失败。
\note 当前 Windows 使用 \c ::GetFileTime 实现，其它只保证最高精确到秒。
\since build 544
*/
//@{
YF_API std::chrono::nanoseconds
GetFileModificationTimeOf(int);
//! \pre 断言：输入非空指针。
YF_API std::chrono::nanoseconds
GetFileModificationTimeOf(std::FILE*);
//! \pre 断言：输入非空指针。
YF_API std::chrono::nanoseconds
GetFileModificationTimeOf(const char*);
//! \pre 断言：输入非空指针。
YF_API std::chrono::nanoseconds
GetFileModificationTimeOf(const char16_t*);
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
\brief 取文件的大小。
\return 以字节计算的文件大小。
\throw FileOperationFailure 参数无效或文件大小查询失败。
\since build 475
*/
//@{
YF_API std::uint64_t
GetFileSizeOf(int);
//! \pre 断言：输入非空指针。
YF_API std::uint64_t
GetFileSizeOf(std::FILE*);
//@}


/*!
\brief 文件系统节点类别。
\since build 412
*/
enum class NodeCategory : std::uint_least32_t
{
	Empty = 0,
	//! \since build 474
	//@{
	Invalid = 1 << 0,
	Regular = 1 << 1,
	//@}
	Unknown = Invalid | Regular,
	//! \since build 474
	//@{
	Device = 1 << 9,
	Block = Device,
	Character = Device | 1 << 7,
	Communicator = 2 << 9,
	FIFO = Communicator | 1 << 6,
	Socket = Communicator | 2 << 6,
	//@}
	SymbolicLink = 1 << 12,
	MountPoint = 2 << 12,
	Junction = MountPoint,
	//! \since build 474
	//@{
	Link = SymbolicLink | Junction,
	//@}
	Directory = 1 << 15,
	//! \since build 474
	//@{
	Missing = 1 << 16,
	Special = Link | Missing
	//@}
};

/*!
\relates NodeCategory
\since build 543
*/
DefBitmaskEnum(NodeCategory)


/*!
\brief 表示文件操作失败的异常。
\since build 411
*/
class YF_API FileOperationFailure : public std::system_error
{
public:
	//! \since build 538
	using system_error::system_error;
};


/*!
\brief 目录会话：表示打开的目录。
\warning 非虚析构。
\since build 411
*/
class YF_API DirectorySession
{
public:
#if YCL_Win32
	using NativeHandle = void*;
#else
	using NativeHandle = ::DIR*;
#endif

private:
	NativeHandle dir;

public:
	/*!
	\brief 构造：打开目录路径。
	\throw FileOperationFailure 打开失败。
	\note 路径可以一个或多个分隔符结尾；当路径为空指针或空字符串时视为 "." 。
	\note 对 MinGW32 实现， "/" 也被作为分隔符支持。
	\note 对 MinGW32 实现， 前缀 "\\?\" 关闭非结尾的 "/" 分隔符支持，
		且无视 MAX_PATH 限制。
	*/
	explicit
	DirectorySession(const char* path = {});
	DirectorySession(DirectorySession&& h)
		: dir(h.dir)
	{
		h.dir = {};
	}
	/*!
	\brief 析构：关闭目录路径。
	\since build 461
	*/
	~DirectorySession();

	//! \since build 549
	DefDeMoveAssignment(DirectorySession)

	//! \since build 413
	DefGetter(const ynothrow, NativeHandle, NativeHandle, dir)

	//! \brief 复位目录状态。
	void
	Rewind() ynothrow;
};


/*!
\brief 目录句柄：表示打开的目录和内容迭代状态。
\since build 411
*/
class YF_API HDirectory final
	: private DirectorySession, private ystdex::deref_self<HDirectory>
{
	//! \since build 556
	friend deref_self<HDirectory>;

private:
#if YCL_Win32
	/*!
	\brief 节点信息。
	\since build 474
	*/
	void* p_dirent;

	/*!
	\brief 节点 UTF-8 名称。
	\since build 402
	*/
	mutable std::string utf8_name{};
#else
	/*!
	\brief 节点信息。
	\invariant <tt>!p_dirent || bool(GetNativeHandle())</tt>
	\since build 298
	*/
	::dirent* p_dirent;
#endif

public:
	/*!
	\brief 构造：使用目录路径。
	\since build 541
	*/
	explicit
	HDirectory(const char* path)
		: DirectorySession(path), p_dirent()
	{}
	//! \since build 543
	HDirectory(HDirectory&& h) ynothrow
		: DirectorySession(std::move(h)), p_dirent(h.p_dirent)
#if YCL_Win32
		, utf8_name(std::move(h.utf8_name))
#endif
	{
		h.p_dirent = {};
	}

	//! \since build 549
	DefDeMoveAssignment(HDirectory)

	/*!
	\brief 间接操作：取自身引用。
	\note 使用 ystdex::indirect_input_iterator 和转换函数访问。
	\since build 556
	*/
	using ystdex::deref_self<HDirectory>::operator*;

	/*!
	\brief 迭代：向后遍历。
	\throw FileOperationFailure 读取目录失败。
	\throw FileOperationFailure 目录没有打开。
	*/
	HDirectory&
	operator++();
	//@}

	/*!
	\brief 判断文件系统节点有效性。
	\since build 319
	*/
	explicit DefCvt(const ynothrow, bool, p_dirent)

	//! \since build 412
	DefCvt(const, std::string, GetName())

	/*!
	\brief 间接操作：取节点名称。
	\return 非空结果：子节点不可用时为 \c "." ，否则为子节点名称。
	\note 返回的结果在析构和下一次迭代前保持有效。
	\since build 412
	*/
	const char*
	GetName() const ynothrow;

	/*!
	\brief 取节点状态信息确定的文件系统节点类别。
	\return 未迭代文件时为 NodeCategory::Empty ，否则为对应的其它节点类别。
	\note 不同系统支持的可能不同。
	\since build 474
	*/
	NodeCategory
	GetNodeCategory() const ynothrow;

	//! \brief 复位。
	using DirectorySession::Rewind;
};


/*!
\brief 文件迭代器。
\since build 411
*/
using FileIterator = ystdex::indirect_input_iterator<HDirectory*>;


/*!
\brief 判断指定路径字符串是否表示一个绝对路径。
\pre 间接断言：参数非空。
*/
//@{
//! \since build 412
YF_API bool
IsAbsolute(const char*);
//! \since build 559
YF_API bool
IsAbsolute(const char16_t*);
//@}

/*!
\brief 取指定路径的文件系统根节点名称的长度。
\pre 间接断言：参数非空。
\since build 412
*/
YF_API std::size_t
GetRootNameLength(const char*);

} // namespace platform;

namespace platform_ex
{

#if !YCL_Win32
//! \since build 409
char16_t
FS_IsRoot(const char16_t*);
#endif

} // namespace platform_ex;

#endif

