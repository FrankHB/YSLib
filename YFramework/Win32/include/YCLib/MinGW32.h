/*
	© 2013-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file MinGW32.h
\ingroup YCLib
\ingroup Win32
\brief YCLib MinGW32 平台公共扩展。
\version r1046
\author FrankHB <frankhb1989@gmail.com>
\since build 412
\par 创建时间:
	2012-06-08 17:57:49 +0800
\par 修改时间:
	2015-09-24 23:42 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(Win32)::MinGW32
*/


#ifndef YCL_MinGW32_INC_MinGW32_h_
#define YCL_MinGW32_INC_MinGW32_h_ 1

#include "YCLib/YModules.h"
#include YFM_YCLib_Host
#include YFM_YCLib_NativeAPI
#include YFM_YCLib_Debug // for string, wstring;
#include <ystdex/enum.hpp> // for ystdex::enum_union, ystdex::wrapped_enum_traits_t;
#if !YCL_Win32
#	error "This file is only for Win32."
#endif
#include <chrono> // for std::chrono::nanoseconds;

namespace platform_ex
{

/*!
\brief Windows 平台扩展接口。
\since build 427
*/
inline namespace Windows
{

//! \since build 622
//@{
/*!
\brief 转换 Win32 错误为 errno 。
\return 当对应不存在时 EINVAL ，否则参数对应的 errno 。
\since build 633
*/
YF_API YB_STATELESS int
ConvertToErrno(unsigned long) ynothrow;

//! \breif 取转换为 errno 的 Win32 错误。
inline PDefH(int, GetErrnoFromWin32, ) ynothrow
	ImplRet(ConvertToErrno(::GetLastError()))
//@}


/*!
\ingroup exception_types
\brief Win32 错误引起的宿主异常。
\since build 426
*/
class YF_API Win32Exception : public Exception
{
public:
	//! \since build 435
	//@{
	using ErrorCode = unsigned long;

public:
	/*!
	\pre 错误码不等于 0 。
	\warning 初始化参数时可能会改变 ::GetLastError() 的结果。
	\since build 624
	*/
	Win32Exception(ErrorCode, const std::string& = "Win32 exception",
		YSLib::RecordLevel = YSLib::Emergent);
	//! \since build 586
	DefDeCopyCtor(Win32Exception)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~Win32Exception() override;

	DefGetter(const ynothrow, ErrorCode, ErrorCode, ErrorCode(code().value()))
	//! \since build 437
	DefGetter(const ynothrow, std::string, Message,
		FormatMessage(GetErrorCode()))

	explicit DefCvt(const ynothrow, ErrorCode, GetErrorCode())

	/*!
	\brief 取错误类别。
	\return std::error_category 派生类的 const 引用。
	\since build 545
	*/
	static const std::error_category&
	GetErrorCategory();

	/*!
	\brief 格式化错误消息字符串。
	\return 若发生异常则结果为空，否则为区域固定为 en-US 的系统消息字符串。
	*/
	static std::string
	FormatMessage(ErrorCode) ynothrow;
	//@}
};


//! \since build 592
//@{
//! \brief 按 ::GetLastError 的结果和指定参数抛出 Windows::Win32Exception 对象。
#	define YCL_Raise_Win32Exception(...) \
	{ \
		const auto err(::GetLastError()); \
	\
		throw platform_ex::Windows::Win32Exception(err, __VA_ARGS__); \
	}

//! \brief 按表达式求值和指定参数抛出 Windows::Win32Exception 对象。
#	define YCL_Raise_Win32Exception_On_Failure(_expr, ...) \
	{ \
		const auto err(Win32Exception::ErrorCode(_expr)); \
	\
		if(err != ERROR_SUCCESS) \
			throw platform_ex::Windows::Win32Exception(err, __VA_ARGS__); \
	}

//! \brief 调用 WinAPI ，若失败抛出 Windows::Win32Exception 对象。
//@{
#	define YCL_WrapCallWin32(_fn, _msg, ...) \
	[&]{ \
		const auto res(::_fn(__VA_ARGS__)); \
	\
		if(YB_UNLIKELY(!res)) \
			YCL_Raise_Win32Exception(#_fn " @ " _msg); \
		return res; \
	}

//! \since build 628
#	define YCL_CallWin32(_fn, ...) YCL_WrapCallWin32(_fn, __VA_ARGS__)()
//@}

/*!
\brief 调用 WinAPI ，若失败跟踪 ::GetLastError 的结果。
\note 格式转换说明符置于最前以避免宏参数影响结果。
*/
//@{
#	define YCL_WrapCallWin32_Trace(_fn, _msg, ...) \
	[&]{ \
		const auto res(::_fn(__VA_ARGS__)); \
	\
		if(YB_UNLIKELY(!res)) \
			YTraceDe(Warning, "Error %lu: failed calling " #_fn " @ " _msg \
				".", ::GetLastError()); \
		return res; \
	}

//! \since build 628
#	define YCL_CallWin32_Trace(_fn, ...) \
	YCL_WrapCallWin32_Trace(_fn, __VA_ARGS__)()
//@}
//@}


//! \since build 593
//@{
//! \brief 全局存储删除器。
struct YF_API GlobalDelete
{
	using pointer = ::HGLOBAL;

	void
	operator()(pointer) const ynothrow;
};


//! \brief 全局锁定存储。
class YF_API GlobalLocked
{
private:
	//! \invariant <tt>bool(p_locked)</tt> 。
	void* p_locked;

public:
	/*!
	\brief 构造：锁定存储。
	\throw Win32Exception ::GlobalLock 调用失败。
	*/
	//@{
	GlobalLocked(::HGLOBAL);
	template<typename _tPointer>
	GlobalLocked(const _tPointer& p)
		: GlobalLocked(p.get())
	{}
	//@}
	~GlobalLocked();

	template<typename _type = void>
	_type*
	GetPtr() const ynothrow
	{
		return static_cast<_type*>(p_locked);
	}
};
//@}


//! \since build 629
//@{
/*!
\brief 访问权限。
\see https://msdn.microsoft.com/en-us/library/windows/desktop/aa374892(v=vs.85).aspx 。
\see https://msdn.microsoft.com/en-us/library/windows/desktop/aa374896(v=vs.85).aspx 。
*/
enum class AccessRights : ::ACCESS_MASK
{
	None = 0,
	GenericRead = GENERIC_READ,
	GenericWrite = GENERIC_WRITE,
	GenericReadWrite = GenericRead | GenericWrite,
	GenericExecute = GENERIC_EXECUTE,
	GenericAll = GENERIC_ALL,
	MaximumAllowed = MAXIMUM_ALLOWED,
	AccessSystemACL = ACCESS_SYSTEM_SECURITY,
	Delete = DELETE,
	ReadControl = READ_CONTROL,
	Synchronize = SYNCHRONIZE,
	WriteDAC = WRITE_DAC,
	WriteOwner = WRITE_OWNER,
	All = STANDARD_RIGHTS_ALL,
	Execute = STANDARD_RIGHTS_EXECUTE,
	StandardRead = STANDARD_RIGHTS_READ,
	Required = STANDARD_RIGHTS_REQUIRED,
	StandardWrite = STANDARD_RIGHTS_WRITE
};

//! \relates AccessRights
DefBitmaskEnum(AccessRights)


//! \brief 文件特定的访问权限。
enum class FileSpecificAccessRights : ::ACCESS_MASK
{
	AddFile = FILE_ADD_FILE,
	AddSubdirectory = FILE_ADD_SUBDIRECTORY,
	AllAccess = FILE_ALL_ACCESS,
	AppendData = FILE_APPEND_DATA,
	CreatePipeInstance = FILE_CREATE_PIPE_INSTANCE,
	DeleteChild = FILE_DELETE_CHILD,
	Execute = FILE_EXECUTE,
	ListDirectory = FILE_LIST_DIRECTORY,
	ReadAttributes = FILE_READ_ATTRIBUTES,
	ReadData = FILE_READ_DATA,
	ReadEA = FILE_READ_EA,
	Traverse = FILE_TRAVERSE,
	WriteAttributes = FILE_WRITE_ATTRIBUTES,
	WriteData = FILE_WRITE_DATA,
	WriteEA = FILE_WRITE_EA,
	Read = STANDARD_RIGHTS_READ,
	Write = STANDARD_RIGHTS_WRITE
};

//! \relates FileSpecificAccessRights
DefBitmaskEnum(FileSpecificAccessRights)


//! \brief 文件访问权限。
using FileAccessRights
	= ystdex::enum_union<AccessRights, FileSpecificAccessRights>;

//! \relates FileAccessRights
DefBitmaskOperations(FileAccessRights,
	ystdex::wrapped_enum_traits_t<FileAccessRights>)


//! \brief 文件共享模式。
enum class FileShareMode : ::ACCESS_MASK
{
	None = 0,
	Delete = FILE_SHARE_DELETE,
	Read = FILE_SHARE_READ,
	Write = FILE_SHARE_WRITE,
	ReadWrite = Read | Write,
	All = Delete | Read | Write
};

//! \relates FileShareMode
DefBitmaskEnum(FileShareMode)


//! \brief 文件创建选项。
enum class CreationDisposition : unsigned long
{
	CreateAlways = CREATE_ALWAYS,
	CreateNew = CREATE_NEW,
	OpenAlways = OPEN_ALWAYS,
	OpenExisting = OPEN_EXISTING,
	TruncateExisting = TRUNCATE_EXISTING
};
//@}


//! \since build 632
//@{
enum class FileAttributesAndFlags : unsigned long
{
	Normal = FILE_ATTRIBUTE_NORMAL,
	NormalWithDirectory = Normal | FILE_FLAG_BACKUP_SEMANTICS,
	NormalAll = NormalWithDirectory | FILE_FLAG_OPEN_REPARSE_POINT
};

//! \relates FileAttributesAndFlags
DefBitmaskEnum(FileAttributesAndFlags)


/*!
\brief 创建或打开独占的文件或设备。
\pre 间接断言：路径参数非空。
\note 调用 \c ::CreateFileW 实现。
*/
//@{
YF_API YB_NONNULL(1) UniqueHandle
MakeFile(const wchar_t*, FileAccessRights = AccessRights::None,
	FileShareMode = FileShareMode::All, CreationDisposition
	= CreationDisposition::OpenExisting,
	FileAttributesAndFlags = FileAttributesAndFlags::NormalAll) ynothrowv;
inline YB_NONNULL(1) PDefH(UniqueHandle, MakeFile, const wchar_t* path,
	FileAccessRights desired_access, FileShareMode shared_mode,
	FileAttributesAndFlags attributes_and_flags
	= FileAttributesAndFlags::NormalAll) ynothrowv
	ImplRet(MakeFile(path, desired_access, shared_mode,
		CreationDisposition::OpenExisting, attributes_and_flags))
inline YB_NONNULL(1) PDefH(UniqueHandle, MakeFile, const wchar_t* path,
	FileAccessRights desired_access, CreationDisposition creation_disposition,
	FileAttributesAndFlags attributes_and_flags
	= FileAttributesAndFlags::NormalAll) ynothrowv
	ImplRet(MakeFile(path, desired_access, FileShareMode::All,
		creation_disposition, attributes_and_flags))
//! \since build 637
inline YB_NONNULL(1) PDefH(UniqueHandle, MakeFile, const wchar_t* path,
	FileAccessRights desired_access,
	FileAttributesAndFlags attributes_and_flags) ynothrowv
	ImplRet(MakeFile(path, desired_access, FileShareMode::All,
		CreationDisposition::OpenExisting, attributes_and_flags))
//@}
//@}


/*!
\brief 安装控制台处理器。
\throw Win32Exception 设置失败。
\note 默认行为使用 <tt>::ExitProcess</tt> ，可能造成 C/C++ 运行时无法正常清理。
\warning 默认不应在 std::at_quick_exit 注册依赖静态或线程生存期对象状态的回调。
\see http://msdn.microsoft.com/en-us/library/windows/desktop/ms682658(v=vs.85).aspx
\see http://msdn.microsoft.com/en-us/library/windows/desktop/ms686016(v=vs.85).aspx
\see $2015-01 @ %Documentation::Workflow::Annual2014.
\since build 565

若第一参数为空，则使用具有以下行为的处理器：
对 \c CTRL_C_EVENT \c CTRL_CLOSE_EVENT 、 \c CTRL_BREAK_EVENT 、
\c CTRL_LOGOFF_EVENT 、和 \c CTRL_SHUTDOWN_EVENT ，调用
<tt>std::_Exit(STATUS_CONTROL_C_EXIT)</tt> 。
第二参数指定添加或移除。
*/
YF_API void
FixConsoleHandler(int(WINAPI*)(unsigned long) = {}, bool = true);

/*!
\brief 判断是否在 Wine 环境下运行。
\note 检查 HKEY_CURRENT_USER 和 HKEY_LOCAL_MACHINE 下的 Software\Wine 键实现。
\since build 435
*/
YF_API bool
CheckWine();


// TODO: Add more Windows specific APIs.

/*!	\defgroup native_encoding_conv Native Encoding Conversion
\brief 本机文本编码转换。
\pre 长度参数非零且不上溢 \c int 时间接断言：字符串指针参数非空。
\exception 长度参数上溢 \int 或转换中溢出。
\note 长度为零时直接返回空字符串，无其它效果。
\since build 593

转换第一个 \c unsigned 参数指定编码的字符串为第二个 \c unsigned 参数指定的编码。
*/
//@{
YF_API string
MBCSToMBCS(size_t, const char*, unsigned = CP_UTF8, unsigned = CP_ACP);
inline YB_NONNULL(1) PDefH(string, MBCSToMBCS, const char* str,
	unsigned cp_src = CP_UTF8, unsigned cp_dst = CP_ACP)
	ImplRet(Windows::MBCSToMBCS(ystdex::ntctslen(str), str, cp_src, cp_dst))
inline PDefH(string, MBCSToMBCS, const string& str,
	unsigned cp_src = CP_UTF8, unsigned cp_dst = CP_ACP)
	ImplRet(Windows::MBCSToMBCS(str.length(), str.c_str(), cp_src, cp_dst))

YF_API string
WCSToMBCS(size_t, const wchar_t*, unsigned = CP_ACP);
inline YB_NONNULL(1) PDefH(string, WCSToMBCS, const wchar_t* str,
	unsigned cp = CP_ACP)
	ImplRet(Windows::WCSToMBCS(ystdex::ntctslen(str), str, cp))
inline PDefH(string, WCSToMBCS, const wstring& str,
	unsigned cp = CP_ACP)
	ImplRet(Windows::WCSToMBCS(str.length(), str.c_str(), cp))

YF_API wstring
MBCSToWCS(size_t, const char*, unsigned = CP_ACP);
inline YB_NONNULL(1) PDefH(wstring, MBCSToWCS, const char* str,
	unsigned cp = CP_ACP)
	ImplRet(Windows::MBCSToWCS(ystdex::ntctslen(str), str, cp))
inline PDefH(wstring, MBCSToWCS, const string& str,
	unsigned cp = CP_ACP)
	ImplRet(Windows::MBCSToWCS(str.length(), str.c_str(), cp))

inline PDefH(string, WCSToUTF8, const wchar_t* str, size_t len)
	ImplRet(WCSToMBCS(len, str, CP_UTF8))
inline YB_NONNULL(1) PDefH(string, WCSToUTF8, const wchar_t* str)
	ImplRet(Windows::WCSToUTF8(str, ystdex::ntctslen(str)))
inline PDefH(string, WCSToUTF8, const wstring& str)
	ImplRet(Windows::WCSToUTF8(str.c_str(), str.length()))

inline PDefH(wstring, UTF8ToWCS, const char* str, size_t len)
	ImplRet(MBCSToWCS(len, str, CP_UTF8))
inline YB_NONNULL(1) PDefH(wstring, UTF8ToWCS, const char* str)
	ImplRet(Windows::UTF8ToWCS(str, ystdex::ntctslen(str)))
inline PDefH(wstring, UTF8ToWCS, const string& str)
	ImplRet(Windows::UTF8ToWCS(str.c_str(), str.length()))
//@}


/*!
\brief 文件系统目录查找状态。
\since build 549
*/
class YF_API DirectoryFindData : private ystdex::noncopyable
{
private:
	/*!
	\brief 查找起始的目录名称。
	\since build 593
	*/
	wstring dir_name;
	//! \brief Win32 查找数据。
	::WIN32_FIND_DATAW find_data;
	//! \brief 查找节点句柄。
	::HANDLE h_node = {};
	/*!
	\brief 当前查找的项目名称。
	\since build 593
	*/
	wstring d_name{};

public:
	/*!
	\brief 构造：使用指定的目录路径。
	\throw platform::FileOperationFailure 打开路径失败，或指定的路径不是目录。
	\note 目录路径无视结尾的斜杠和反斜杠。
	\note 去除结尾斜杠和反斜杠后若为空则视为当前路径。
	\since build 593
	*/
	//@{
	//! \note 使用 UTF-8 目录路径。
	DirectoryFindData(string);
	//! \note 使用 UTF-16 目录路径。
	DirectoryFindData(wstring);
	//@}
	//! \brief 析构：若查找节点句柄非空则关闭查找状态。
	~DirectoryFindData();

	//! \since build 556
	DefBoolNeg(explicit, h_node)

	//! \since build 564
	DefGetter(const ynothrow, unsigned long, Attributes,
		find_data.dwFileAttributes)
	DefGetter(const ynothrow, const ::WIN32_FIND_DATAW&, FindData, find_data)
	//! \since build 593
	DefGetter(const ynothrow, const wstring&, DirName, dir_name)

private:
	/*!
	\brief 关闭查找状态。
	\post 断言：关闭成功。
	*/
	void
	Close() ynothrow;

public:
	/*!
	\brief 读取：迭代当前查找状态。
	\return 若迭代结束后节点非空，表示当前查找项目名的指针；否则为空指针。
	\since build 593

	若查找节点句柄非空则迭代当前查找状态查找下一个文件系统项。
	否则查找节点句柄为空或迭代失败则关闭查找状态并置查找节点句柄空。
	最终查找节点非空时保存记录当前查找的项目状态。
	*/
	wstring*
	Read();

	//! \brief 复位查找状态：若查找节点句柄非空则关闭查找状态并置查找节点句柄空。
	void
	Rewind() ynothrow;
};


/*!
\brief 查询文件链接数。
\since build 637
*/
//@{
YF_API size_t
QueryFileLinks(UniqueHandle::pointer);
/*!
\pre 间接断言：路径参数非空。
\throw Win32Exception 访问文件失败。
*/
YF_API YB_NONNULL(1) size_t
QueryFileLinks(const wchar_t*);
//@}

/*!
\brief 查询文件的创建、访问和修改时间。
\note 后三个参数可选，指针为空时忽略。
\since build 629
*/
//@{
//! \pre 文件句柄不为 \c INVALID_HANDLE_VALUE 且具有 AccessRights::GenericRead 权限。
YF_API void
QueryFileTime(UniqueHandle::pointer, ::FILETIME* = {}, ::FILETIME* = {},
	::FILETIME* = {});
/*!
\pre 间接断言：路径参数非空。
\throw Win32Exception 访问文件失败。
\note 即使可选参数都为空指针时仍访问文件。最后参数表示跟踪重解析点。
\since build 632
*/
//@{
//! \note 使用 UTF-8 路径。
YF_API YB_NONNULL(1) void
QueryFileTime(const char*, ::FILETIME* = {}, ::FILETIME* = {},
	::FILETIME* = {}, bool = {});
YF_API YB_NONNULL(1) void
QueryFileTime(const wchar_t*, ::FILETIME* = {}, ::FILETIME* = {},
	::FILETIME* = {}, bool = {});
//@}
//@}

/*!
\brief 转换文件时间为以 POSIX 历元起始度量的时间间隔。
\throw std::system_error 输入的时间表示不被实现支持。
\since build 632
*/
YF_API std::chrono::nanoseconds
ConvertTime(const ::FILETIME&);

/*!
\brief 展开字符串中的环境变量。
\since build 593
*/
//@{
//! \note 第一参数为空指针或第二参数等于 0 时返回空串。
YF_API wstring
ExpandEnvironmentStrings(const wchar_t*, size_t);
inline PDefH(wstring, ExpandEnvironmentStrings, const wstring& str)
	ImplRet(ExpandEnvironmentStrings(str.c_str(), str.length()))
//@}

/*!
\brief 取系统目录路径。
\note 保证以一个分隔符结尾。
\since build 593
*/
YF_API wstring
FetchSystemPath(size_t s = MAX_PATH);

} // inline namespace Windows;

} // namespace platform_ex;

#endif

