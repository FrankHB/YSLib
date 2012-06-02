/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file FileSystem.h
\ingroup YCLib
\brief 平台相关的文件系统接口。
\version r1362;
\author FrankHB<frankhb1989@gmail.com>
\since build 312 。
\par 创建时间:
	2012-05-30 22:38:37 +0800;
\par 修改时间:
	2012-06-01 22:53 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YCLib::FileSystem;
*/


#ifndef YCL_INC_FILESYSTEM_H_
#define YCL_INC_FILESYSTEM_H_

#include "ycommon.h"
#ifdef YCL_MINGW32
// NOTE: no use of MinGW32 ::dirent, ::DIR, ::opendir ...
struct DIR;
struct dirent;
#else
#include <dirent.h>
#endif

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

#ifdef YCL_API_FILESYSTEM_POSIX
	/*!
	\brief 文件路径分隔符。
	\since build 298 。
	*/
#	define YCL_PATH_DELIMITER '/'
	/*!
	\brief 文件路径分隔字符串。
	*/
#	define YCL_PATH_SEPERATOR "/"
	/*!
	\brief 根目录路径。
	*/
#	define YCL_PATH_ROOT YCL_PATH_SEPERATOR

/*!
\brief 本机路径字符类型。
\since build 286 。
*/
typedef char NativePathCharType;
#elif defined(YCL_MINGW32)
	/*!
	\brief 文件路径分隔符。
	\since build 296 。
	*/
#	define YCL_PATH_DELIMITER '\\'
//	#define YCL_PATH_DELIMITER L'\\'
	/*!
	\brief 文件路径分隔字符串。
	\since build 296 。
	*/
#	define YCL_PATH_SEPERATOR "\\"
//	#define YCL_PATH_SEPERATOR L"\\"
	/*!
	\brief 虚拟根目录路径。
	\since build 297 。
	*/
#	define YCL_PATH_ROOT YCL_PATH_SEPERATOR

/*!
\brief 本机路径字符类型。
\since build 296 。
\todo 解决 const_path_t 冲突。
*/
//	typedef wchar_t NativePathCharType;
typedef char NativePathCharType;
#else
#	error Unsupported platform found!
#endif

//类型定义。
/*!
\brief 本机路径字符串类型。
\since build 286 。
*/
typedef NativePathCharType PATHSTR[YCL_MAX_PATH_LENGTH];
/*!
\brief 本机文件名类型。
\since build 286 。
*/
typedef NativePathCharType FILENAMESTR[YCL_MAX_FILENAME_LENGTH];

// using ystdex;
using ystdex::const_path_t;
using ystdex::path_t;


/*!
\brief 以 UTF-8 文件名打开文件。
\param filename 文件名，意义同 std::fopen 。
\param mode 打开模式，基本语义同 ISO C99 ，具体行为取决于实现。
\pre 断言检查：<tt>filename && mode && *mode != 0</tt> 。
\since build 299 。
*/
std::FILE*
ufopen(const char* filename, const char* mode);
/*!
\brief 以 UCS-2LE 文件名打开文件。
\param filename 文件名，意义同 std::fopen 。
\param mode 打开模式，基本语义同 ISO C99，具体行为取决于实现。
\pre 断言检查：<tt>filename && mode && *mode != 0</tt> 。
\since build 305 。
*/
std::FILE*
ufopen(const char16_t* filename, const char16_t* mode);

/*!
\brief 判断指定 UTF-8 文件名的文件是否存在。
\note 使用 ufopen 实现。
\pre 断言检查：参数非空。
\since build 299 。
*/
bool
ufexists(const char*);
/*!
\brief 判断指定 UCS-2 文件名的文件是否存在。
\note 使用 ufopen 实现。
\pre 断言检查：参数非空。
\since build 305 。
*/
bool
ufexists(const char16_t*);
/*!
\brief 判断指定字符串为文件名的文件是否存在。
\note 使用 NTCTS 参数 ufexists 实现。
\since build 305 。
*/
template<class _tString>
inline bool
ufexists(const _tString& s)
{
	return ufexists(s.c_str());
}

/*!
\brief 判断指定路径的目录是否存在。
*/
bool
direxists(const_path_t);

/*!
\brief 判断指定 UTF-8 路径的目录是否存在。
\since build 312 。
*/
bool
udirexists(const_path_t);

/*!
\brief 当第一参数非空时取当前工作目录复制至指定缓冲区中。
\param buf 缓冲区起始指针。
\param size 缓冲区长。
\return 若成功为 buf ，否则为空指针。
\deprecated 特定平台上的编码不保证是 UTF-8 。
*/
char*
getcwd_n(char* buf, std::size_t size);

/*!
\brief 当第一参数非空时取当前工作目录（ UCS2-LE 编码）复制至指定缓冲区中。
\param buf 缓冲区起始指针。
\param size 缓冲区长。
\return 若成功为 buf ，否则为空指针。
\since build 304 。
*/
char16_t*
u16getcwd_n(char16_t* buf, std::size_t size);

/*!
\brief 切换当前工作路径至指定的 UTF-8 字符串。
\since build 313 。
*/
int
uchdir(const_path_t);

/*!
\brief 按路径新建一个或多个目录。
*/
bool
mkdirs(const_path_t);


/*!
\brief 文件系统节点迭代器。
\since build 298 。
*/
class HFileNode
{
public:
	typedef ::DIR* IteratorType; //!< 本机迭代器类型。

	static int LastError; //!< 上一次操作结果，0 为无错误。

private:
	IteratorType dir;
	/*!
	\brief 节点信息。
	\since build 298 。
	*/
	::dirent* p_dirent;

public:
	/*!
	\brief 构造：使用路径字符串。
	*/
	explicit
	HFileNode(const_path_t path = nullptr)
		: dir(), p_dirent()
	{
		Open(path);
	}
	/*!
	\brief 复制构造：默认实现。
	\note 浅复制。
	*/
	HFileNode(const HFileNode&) = default;
	/*!
	\brief 析构。
	*/
	~HFileNode()
	{
		Close();
	}

	/*!
	\brief 复制赋值：默认实现。
	\note 浅复制。
	\since build 311 。
	*/
	HFileNode&
	operator=(const HFileNode&) = default;

	/*!
	\brief 迭代：向后遍历。
	*/
	HFileNode&
	operator++();
	/*!
	\brief 迭代：向前遍历。
	*/
	HFileNode
	operator++(int)
	{
		return ++HFileNode(*this);
	}

	/*!
	\brief 判断文件系统节点有效性。
	*/
	bool
	IsValid() const
	{
		return dir;
	}
	/*!

	\brief 从节点状态信息判断是否为目录。
	*/
	bool
	IsDirectory() const;

	/*!
	\brief 取节点名称。
	\post 返回值非空。
	\since build 298 。
	*/
	const char*
	GetName() const;

	/*!
	\brief 打开。
	*/
	void
	Open(const_path_t);

	/*!
	\brief 关闭。
	*/
	void
	Close();

	/*!
	\brief 复位。
	*/
	void
	Reset();
};


/*!
\brief 判断指定路径字符串是否表示一个绝对路径。
*/
bool
IsAbsolute(const_path_t);

/*!
\brief 取指定路径的文件系统根节点名称的长度。
*/
std::size_t
GetRootNameLength(const_path_t);

} // namespace platform;

namespace platform_ex
{

} // namespace platform_ex;

#endif

