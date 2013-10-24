/*
	© 2012-2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file FileSystem.h
\ingroup YCLib
\brief 平台相关的文件系统接口。
\version r1034
\author FrankHB <frankhb1989@gmail.com>
\since build 312
\par 创建时间:
	2012-05-30 22:38:37 +0800
\par 修改时间:
	2013-10-23 19:59 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::FileSystem
*/


#ifndef YCL_INC_FileSystem_h_
#define YCL_INC_FileSystem_h_ 1

#include "ycommon.h"
#include <ystdex/utility.hpp> // for std::is_array, std::is_integral,
//	ystdex::remove_reference_t, ystdex::arrlen;
#include <ystdex/cstring.h> // for ystdex::is_null;
#include <ystdex/string.hpp> // for ystdex::string_length, std::string;
#include <CHRLib/encoding.h>
#include <dirent.h>
#include <ystdex/iterator.hpp> // for ystdex::indirect_input_iterator;

namespace platform
{

//平台相关的全局常量。

//最大路径长度。
#ifdef PATH_MAX
#	define YCL_MAX_PATH_LENGTH PATH_MAX
#elif defined(MAXPATHLEN)
#	define YCL_MAX_PATH_LENGTH MAXPATHLEN
#else
#	define YCL_MAX_PATH_LENGTH 256
#endif

//最大文件名长度。
#ifdef NAME_MAX
#	define YCL_MAX_FILENAME_LENGTH NAME_MAX
#else
#	define YCL_MAX_FILENAME_LENGTH YCL_MAX_PATH_LENGTH
#endif

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

#ifdef YCL_API_FILESYSTEM_POSIX
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

#	define YCL_FS_CharIsDelimiter(_c, _p) (_c == YPP_Join(_p, YCL_PATH_DELIMITER))
#	define YCL_FS_StringIsRoot(_s, _p) (platform_ex::FS_IsRoot(&_s[0]))

/*!
\brief 路径字符串编码。
\since build 402
*/
yconstexpr CHRLib::CharSet::Encoding CS_Path(CHRLib::CharSet::UTF_8);
#elif YCL_MinGW32
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
#else
#	error "Unsupported platform found."
#endif

//! \since build 402
//@{
static_assert(std::is_integral<decltype(YCL_PATH_DELIMITER)>::value,
	"Illegal type of delimiter found.");
static_assert(std::is_array<ystdex::remove_reference_t<decltype(
	YCL_PATH_SEPARATOR)>>::value, "Non-array type of separator found.");
static_assert(ystdex::arrlen(YCL_PATH_SEPARATOR) == 2,
	"Wrong length of separator found.");
static_assert(YCL_PATH_SEPARATOR[0] == YCL_PATH_DELIMITER,
	"Mismatched path delimiter and separator found.");
static_assert(ystdex::is_null(YCL_PATH_SEPARATOR[1]),
	"Non-null-terminator as end of separator.");
//@}

//类型定义。
/*!
\brief 本机路径字符串类型。
\since build 286
*/
using PATHSTR = NativePathCharType[YCL_MAX_PATH_LENGTH];
/*!
\brief 本机文件名类型。
\since build 286
*/
using FILENAMESTR = NativePathCharType[YCL_MAX_FILENAME_LENGTH];


/*!
\brief 以 UTF-8 文件名无缓冲打开文件。
\param filename 文件名，意义同 POSIX <tt>::open</tt> 。
\param oflag 打开标识，基本语义同 POSIX 2003 ，具体行为取决于实现。
\pre 断言：<tt>filename</tt> 。
\bug MinGW32 环境下非线程安全。
\since build 324
*/
YF_API int
uopen(const char* filename, int oflag) ynothrow;
/*!
\brief 以 UTF-8 文件名无缓冲打开文件。
\param filename 文件名，意义同 POSIX <tt>::open</tt> 。
\param oflag 打开标识，基本语义同 POSIX 2003 ，具体行为取决于实现。
\param pmode 打开模式，基本语义同 POSIX 2003 ，具体行为取决于实现。
\pre 断言：<tt>filename</tt> 。
\bug MinGW32 环境下非线程安全。
\since build 324
*/
YF_API int
uopen(const char* filename, int oflag, int pmode) ynothrow;
/*!
\brief 以 UCS-2LE 文件名无缓冲打开文件。
\param filename 文件名，意义同 POSIX <tt>::open</tt> 。
\param oflag 打开标识，基本语义同 POSIX 2003 ，具体行为取决于实现。
\pre 断言：<tt>filename</tt> 。
\since build 324
*/
YF_API int
uopen(const char16_t* filename, int oflag) ynothrow;
/*!
\brief 以 UCS-2LE 文件名无缓冲打开文件。
\param filename 文件名，意义同 POSIX <tt>::open</tt> 。
\param oflag 打开标识，基本语义同 POSIX 2003 ，具体行为取决于实现。
\param pmode 打开模式，基本语义同 POSIX 2003 ，具体行为取决于实现。
\pre 断言：<tt>filename</tt> 。
\since build 324
*/
YF_API int
uopen(const char16_t* filename, int oflag, int pmode) ynothrow;

/*!
\brief 以 UTF-8 文件名打开文件。
\param filename 文件名，意义同 std::fopen 。
\param mode 打开模式，基本语义同 ISO C99 ，具体行为取决于实现。
\pre 断言：<tt>filename && mode && *mode != 0</tt> 。
\bug MinGW32 环境下非线程安全。
\since build 299
*/
YF_API std::FILE*
ufopen(const char* filename, const char* mode) ynothrow;
/*!
\brief 以 UCS-2LE 文件名打开文件。
\param filename 文件名，意义同 std::fopen 。
\param mode 打开模式，基本语义同 ISO C99 ，具体行为取决于实现。
\pre 断言：<tt>filename && mode && *mode != 0</tt> 。
\since build 324
*/
YF_API std::FILE*
ufopen(const char16_t* filename, const char16_t* mode) ynothrow;

/*!
\brief 判断指定 UTF-8 文件名的文件是否存在。
\note 使用 ufopen 实现。
\pre 断言：参数非空。
\since build 324
*/
YF_API bool
ufexists(const char*) ynothrow;
/*!
\brief 判断指定 UCS-2 文件名的文件是否存在。
\note 使用 ufopen 实现。
\pre 断言：参数非空。
\since build 324
*/
YF_API bool
ufexists(const char16_t*) ynothrow;
/*!
\brief 判断指定字符串为文件名的文件是否存在。
\note 使用 NTCTS 参数 ufexists 实现。
\since build 324
*/
template<class _tString>
inline PDefH(bool, ufexists, const _tString& str) ynothrow
	ImplRet(ufexists(str.c_str()))

/*!
\brief 当第一参数非空时取当前工作目录复制至指定缓冲区中。
\param buf 缓冲区起始指针。
\param size 缓冲区长。
\return 若成功为 buf ，否则为空指针。
\deprecated 特定平台上的编码不保证是 UTF-8 。
\since build 324
*/
YF_API char*
getcwd_n(char* buf, std::size_t size) ynothrow;

/*!
\brief 当第一参数非空时取当前工作目录（ UCS2-LE 编码）复制至指定缓冲区中。
\param buf 缓冲区起始指针。
\param size 缓冲区长。
\return 若成功为 buf ，否则为空指针。
\since build 324
*/
YF_API char16_t*
u16getcwd_n(char16_t* buf, std::size_t size) ynothrow;

/*!
\brief 切换当前工作路径至指定的 UTF-8 字符串。
\bug MinGW32 环境下非线程安全。
\since build 412
*/
YF_API int
uchdir(const char*) ynothrow;

/*!
\brief 按路径新建一个或多个目录。
\since build 412
*/
YF_API bool
mkdirs(const char*) ynothrow;

/*!
\brief 截断文件至指定长度。
\pre 指定文件需已经打开并可写。
\return 操作是否成功。
\note 不改变文件读写位置。
\since build 341

若文件不足指定长度，扩展并使用空字节填充；否则保留起始指定长度的字节。
*/
YF_API bool
truncate(std::FILE*, std::size_t) ynothrow;


/*!
\brief 表示文件操作失败的异常。
\since build 411
*/
class YF_API FileOperationFailure : public std::runtime_error
{
public:
	//! \since build 413
	FileOperationFailure(const std::string& msg = "") ynothrow
		: runtime_error(msg)
	{}
};


/*!
\brief 目录会话：表示打开的目录。
\warning 非虚析构。
\since build 411
*/
class YF_API DirectorySession
{
public:
#if YCL_DS
	using NativeHandle = ::DIR*;
#else
	using NativeHandle = ::_WDIR*;
#endif

private:
	NativeHandle dir;

public:
	/*!
	\brief 构造：打开目录路径。
	\throw FileOperationFail 打开失败。
	\note 当路径为空指针或空字符串时视为 "." 。
	*/
	explicit
	DirectorySession(const char* path = {});
	DirectorySession(DirectorySession&& h)
		: dir(h.dir)
	{
		h.dir = {};
	}
	//! \brief 析构：关闭目录路径。
	~DirectorySession() ynothrow;

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
class YF_API HDirectory final : private DirectorySession
{
private:
#if YCL_DS
	/*!
	\brief 节点信息。
	\since build 298
	*/
	::dirent* p_dirent;
#else
	/*!
	\brief 节点信息。
	\since build 402
	*/
	::_wdirent* p_dirent;

	/*!
	\brief 节点 UTF-8 名称。
	\since build 402
	*/
	mutable std::string utf8_name;
#endif

public:
	//! \brief 构造：使用目录路径。
	explicit
	HDirectory(const char* path) ynothrow
		: DirectorySession(path)
	{}

	/*!
	\brief 间接操作：取自身引用。
	\note 使用 ystdex::indirect_input_iterator 和转换函数访问。
	\since build 412
	*/
	//@{
	PDefHOp(HDirectory&, *, ) ynothrow
		ImplRet(*this)
	PDefHOp(const HDirectory&, *, ) const ynothrow
		ImplRet(*this)
	//@}

	/*!
	\brief 迭代：向后遍历。
	\throw FileOperationFail 读取目录失败。
	\throw FileOperationFail 目录没有打开。
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
	\return 非空结果：子节点不可用时为 "." ，否则为子节点名称。
	\note 返回的结果在析构和下一次迭代前保持有效。
	\since build 412
	*/
	const char*
	GetName() const ynothrow;

	/*!
	\brief 从节点状态信息判断是否为目录。
	\since build 319
	*/
	bool
	IsDirectory() const ynothrow;

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
\since build 412
*/
YF_API bool
IsAbsolute(const char*);

/*!
\brief 取指定路径的文件系统根节点名称的长度。
\since build 412
*/
YF_API std::size_t
GetRootNameLength(const char*);

} // namespace platform;

namespace platform_ex
{

#if YCL_DS
//! \since build 409
char16_t
FS_IsRoot(const char16_t*);
#endif

} // namespace platform_ex;

#endif

