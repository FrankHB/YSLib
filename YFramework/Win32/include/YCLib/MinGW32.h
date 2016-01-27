/*
	© 2013-2016 FrankHB.

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
\version r1577
\author FrankHB <frankhb1989@gmail.com>
\since build 412
\par 创建时间:
	2012-06-08 17:57:49 +0800
\par 修改时间:
	2016-01-25 00:43 +0800
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
#if !YCL_Win32
#	error "This file is only for Win32."
#endif
#include YFM_YCLib_Debug // for string, platform::Deref, wstring, ystdex::ends_with, pair;
#include <ystdex/enum.hpp> // for ystdex::enum_union, ystdex::wrapped_enum_traits_t;
#include YFM_YCLib_FileIO // for platform::NodeCategory;
#include <chrono> // for std::chrono::nanoseconds;

namespace platform_ex
{

/*!
\brief Windows 平台扩展接口。
\since build 427
*/
inline namespace Windows
{

//! \since build 435
using ErrorCode = unsigned long;

/*!
\brief 转换 Win32 错误为 errno 。
\return 当对应不存在时 EINVAL ，否则参数对应的 errno 。
\since build 639
*/
YF_API YB_STATELESS int
ConvertToErrno(ErrorCode) ynothrow;

/*!
\brief 取转换为 errno 的 Win32 错误。
\since build 622
*/
inline PDefH(int, GetErrnoFromWin32, ) ynothrow
	ImplRet(ConvertToErrno(::GetLastError()))


/*!
\ingroup exception_types
\brief Win32 错误引起的宿主异常。
\since build 426
*/
class YF_API Win32Exception : public Exception
{
public:
	/*!
	\pre 错误码不等于 0 。
	\warning 初始化参数时可能会改变 ::GetLastError() 的结果。
	\since build 643
	*/
	//@{
	Win32Exception(ErrorCode, string_view = "Win32 exception",
		YSLib::RecordLevel = YSLib::Emergent);
	/*!
	\pre 第三参数非空。
	\note 第三参数表示函数名，可以使用 \c __func__ 。
	*/
	YB_NONNULL(4)
	Win32Exception(ErrorCode, string_view, const char*,
		YSLib::RecordLevel = YSLib::Emergent);
	//@}
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
		const auto err(Windows::ErrorCode(_expr)); \
	\
		if(err != ERROR_SUCCESS) \
			throw platform_ex::Windows::Win32Exception(err, __VA_ARGS__); \
	}
//@}

/*!
\brief 调用 Win32 API 或其它可用 ::GetLastError 取得调用状态的例程。
\note 调用时直接使用实际参数，可指定非标识符的表达式，不保证是全局名称。
*/
//@{
/*!
\note 若失败抛出 Windows::Win32Exception 对象。
\since build 651
*/
//@{
#	define YCL_WrapCallWin32(_fn, ...) \
	[&](const char* msg) YB_NONNULL(1){ \
		const auto res(_fn(__VA_ARGS__)); \
	\
		if(YB_UNLIKELY(!res)) \
			YCL_Raise_Win32Exception(#_fn, msg); \
		return res; \
	}

#	define YCL_CallWin32(_fn, _msg, ...) YCL_WrapCallWin32(_fn, __VA_ARGS__)(_msg)

//! \since build 638
#	define YCL_CallWin32F(_fn, ...) YCL_CallWin32(_fn, yfsig, __VA_ARGS__)
//@}

/*!
\note 若失败跟踪 ::GetLastError 的结果。
\note 格式转换说明符置于最前以避免宏参数影响结果。
\since build 651
*/
//@{
#	define YCL_WrapCallWin32_Trace(_fn, ...) \
	[&](const char* msg) YB_NONNULL(1){ \
		const auto res(_fn(__VA_ARGS__)); \
	\
		if(YB_UNLIKELY(!res)) \
			YTraceDe(Warning, "Error %lu: failed calling " #_fn " @ %s.", \
				::GetLastError(), msg); \
		return res; \
	}

#	define YCL_CallWin32_Trace(_fn, _msg, ...) \
	YCL_WrapCallWin32_Trace(_fn, __VA_ARGS__)(_msg)

//! \since build 638
#	define YCL_CallWin32F_Trace(_fn, ...) \
	YCL_CallWin32_Trace(_fn, yfsig, __VA_ARGS__)
//@}
//@}


//! \since build 651
//@{
//! \brief 加载过程地址得到的过程类型。
using ModuleProc
	= ystdex::remove_reference_t<decltype(*::GetProcAddress(::HMODULE(), {}))>;

/*!
\brief 从模块加载指定过程的指针。
\pre 参数非空。
*/
//@{
YF_API YB_ATTR(returns_nonnull) YB_NONNULL(2) ModuleProc*
LoadProc(::HMODULE, const char*);
template<typename _func>
inline YB_NONNULL(2) _func&
LoadProc(::HMODULE h_module, const char* proc)
{
	return platform::Deref(reinterpret_cast<_func*>(LoadProc(h_module, proc)));
}
template<typename _func>
YB_NONNULL(1, 2) _func&
LoadProc(const wchar_t* module, const char* proc)
{
	return LoadProc<_func>(YCL_CallWin32F(GetModuleHandleW, module), proc);
}
//@}


#define YCL_Impl_W32Call_Fn(_fn) W32_##_fn##_t
#define YCL_Impl_W32Call_FnCall(_fn) W32_##_fn##_call

/*!
\brief 声明调用 WinAPI 的例程。
\note 为避免歧义，应在非全局命名空间中使用；注意类作用域名称查找顺序会引起歧义。
\note 当没有合式调用时从指定模块加载。
*/
#define YCL_DeclW32Call(_fn, _module, _tRet, ...) \
	YCL_DeclCheck_t(_fn, _fn) \
	using YCL_Impl_W32Call_Fn(_fn) = _tRet __stdcall(__VA_ARGS__); \
	\
	template<typename... _tParams> \
	auto \
	YCL_Impl_W32Call_FnCall(_fn)(_tParams&&... args) \
		-> YCL_CheckDecl_t(_fn)<_tParams...> \
	{ \
		return _fn(yforward(args)...); \
	} \
	template<typename... _tParams> \
	ystdex::enable_fallback_t<YCL_CheckDecl_t(_fn), \
		YCL_Impl_W32Call_Fn(_fn), _tParams...> \
	YCL_Impl_W32Call_FnCall(_fn)(_tParams&&... args) \
	{ \
		return platform_ex::Windows::LoadProc<YCL_Impl_W32Call_Fn(_fn)>( \
			L###_module, #_fn)(yforward(args)...); \
	} \
	\
	template<typename... _tParams> \
	auto \
	_fn(_tParams&&... args) \
		-> decltype(YCL_Impl_W32Call_FnCall(_fn)(yforward(args)...)) \
	{ \
		return YCL_Impl_W32Call_FnCall(_fn)(yforward(args)...); \
	}
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


/*!
\brief 局部存储删除器。
\since build 658
*/
struct YF_API LocalDelete
{
	using pointer = ::HLOCAL;

	void
	operator()(pointer) const ynothrow;
};


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


//! \since build 639
//@{
//! \see https://msdn.microsoft.com/en-us/library/gg258117(v=vs.85).aspx 。
enum FileAttributes : unsigned long
{
	ReadOnly = FILE_ATTRIBUTE_READONLY,
	Hidden = FILE_ATTRIBUTE_HIDDEN,
	System = FILE_ATTRIBUTE_SYSTEM,
	Directory = FILE_ATTRIBUTE_DIRECTORY,
	Archive = FILE_ATTRIBUTE_ARCHIVE,
	Device = FILE_ATTRIBUTE_DEVICE,
	Normal = FILE_ATTRIBUTE_NORMAL,
	Temporary = FILE_ATTRIBUTE_TEMPORARY,
	SparseFile = FILE_ATTRIBUTE_SPARSE_FILE,
	ReparsePoint = FILE_ATTRIBUTE_REPARSE_POINT,
	Compressed = FILE_ATTRIBUTE_COMPRESSED,
	Offline = FILE_ATTRIBUTE_OFFLINE,
	NotContentIndexed = FILE_ATTRIBUTE_NOT_CONTENT_INDEXED,
	Encrypted = FILE_ATTRIBUTE_ENCRYPTED,
//	IntegrityStream = FILE_ATTRIBUTE_INTEGRITY_STREAM,
	IntegrityStream = 0x8000,
	Virtual = FILE_ATTRIBUTE_VIRTUAL,
//	NoScrubData = FILE_ATTRIBUTE_NO_SCRUB_DATA,
	NoScrubData = 0x20000,
//	EA = FILE_ATTRIBUTE_EA,
	//! \warning 非 MSDN 文档公开。
	EA = 0x40000,
	Invalid = INVALID_FILE_ATTRIBUTES
};

//! \see https://msdn.microsoft.com/en-us/library/aa363858(v=vs.85).aspx 。
enum FileFlags : unsigned long
{
	WriteThrough = FILE_FLAG_WRITE_THROUGH,
	Overlapped = FILE_FLAG_OVERLAPPED,
	NoBuffering = FILE_FLAG_NO_BUFFERING,
	RandomAccess = FILE_FLAG_RANDOM_ACCESS,
	SequentialScan = FILE_FLAG_SEQUENTIAL_SCAN,
	DeleteOnClose = FILE_FLAG_DELETE_ON_CLOSE,
	BackupSemantics = FILE_FLAG_BACKUP_SEMANTICS,
	POSIXSemantics = FILE_FLAG_POSIX_SEMANTICS,
	SessionAware = FILE_FLAG_SESSION_AWARE,
	OpenReparsePoint = FILE_FLAG_OPEN_REPARSE_POINT,
	OpenNoRecall = FILE_FLAG_OPEN_NO_RECALL,
	// \see https://msdn.microsoft.com/zh-cn/library/windows/desktop/aa365150(v=vs.85).aspx 。
	FirstPipeInstance = FILE_FLAG_FIRST_PIPE_INSTANCE
};

enum SecurityQoS : unsigned long
{
	Anonymous = SECURITY_ANONYMOUS,
	ContextTracking = SECURITY_CONTEXT_TRACKING,
	Delegation = SECURITY_DELEGATION,
	EffectiveOnly = SECURITY_EFFECTIVE_ONLY,
	Identification = SECURITY_IDENTIFICATION,
	Impersonation = SECURITY_IMPERSONATION,
	Present = SECURITY_SQOS_PRESENT,
	ValidFlags = SECURITY_VALID_SQOS_FLAGS
};

enum FileAttributesAndFlags : unsigned long
{
	NormalWithDirectory = Normal | BackupSemantics,
	NormalAll = NormalWithDirectory | OpenReparsePoint
};
//@}


/*!
\brief 判断 \c ::WIN32_FIND_DATAA 指定的节点是否为目录。
\since build 298
*/
inline PDefH(bool, IsDirectory, const ::WIN32_FIND_DATAA& d) ynothrow
	ImplRet(d.dwFileAttributes & Directory)
/*!
\brief 判断 \c ::WIN32_FIND_DATAW 指定的节点是否为目录。
\since build 299
*/
inline PDefH(bool, IsDirectory, const ::WIN32_FIND_DATAW& d) ynothrow
	ImplRet(d.dwFileAttributes & Directory)

/*!
\brief 按 \c ::WIN32_FIND_DATAW 归类节点类别。
\return 指定非目录或不被支持的重分析标签时为 NodeCategory::Empty ，
	否则为对应的其它节点类别。
\since build 658
\todo 对目录链接和符号链接的重分析标签提供适当实现。
*/
YF_API platform::NodeCategory
CategorizeNode(const ::WIN32_FIND_DATAW&) ynothrow;
/*!
\brief 按打开的文件句柄归类节点类别。
\pre 断言：参数非空。
\return 指定句柄非打开的文件或调用失败时 NodeCategory::Invalid ，
	否则为对应的其它节点类别。
\since build 658
*/
YF_API platform::NodeCategory
CategorizeNode(UniqueHandle::pointer) ynothrowv;


/*!
\brief 创建或打开独占的文件或设备。
\pre 间接断言：路径参数非空。
\note 调用 \c ::CreateFileW 实现。
\since build 632
*/
//@{
YF_API YB_NONNULL(1) UniqueHandle
MakeFile(const wchar_t*, FileAccessRights = AccessRights::None,
	FileShareMode = FileShareMode::All, CreationDisposition
	= CreationDisposition::OpenExisting,
	FileAttributesAndFlags = FileAttributesAndFlags::NormalAll) ynothrowv;
//! \since build 660
inline YB_NONNULL(1) PDefH(UniqueHandle, MakeFile, const wchar_t* path,
	FileAccessRights desired_access, FileShareMode shared_mode,
	FileAttributesAndFlags attributes_and_flags) ynothrowv
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
//! \since build 660
//@{
inline YB_NONNULL(1) PDefH(UniqueHandle, MakeFile, const wchar_t* path,
	FileShareMode shared_mode, CreationDisposition creation_disposition
	= CreationDisposition::OpenExisting, FileAttributesAndFlags
	attributes_and_flags = FileAttributesAndFlags::NormalAll) ynothrowv
	ImplRet(MakeFile(path, AccessRights::None, shared_mode,
		creation_disposition, attributes_and_flags))
inline YB_NONNULL(1) PDefH(UniqueHandle, MakeFile, const wchar_t* path,
	CreationDisposition creation_disposition
	= CreationDisposition::OpenExisting, FileAttributesAndFlags
	attributes_and_flags = FileAttributesAndFlags::NormalAll) ynothrowv
	ImplRet(MakeFile(path, AccessRights::None, FileShareMode::All,
		creation_disposition, attributes_and_flags))
inline YB_NONNULL(1) PDefH(UniqueHandle, MakeFile, const wchar_t* path,
	FileShareMode shared_mode, FileAttributesAndFlags
	attributes_and_flags = FileAttributesAndFlags::NormalAll) ynothrowv
	ImplRet(MakeFile(path, AccessRights::None, shared_mode,
		CreationDisposition::OpenExisting, attributes_and_flags))
inline YB_NONNULL(1) PDefH(UniqueHandle, MakeFile, const wchar_t* path,
	FileAttributesAndFlags attributes_and_flags) ynothrowv
	ImplRet(MakeFile(path, AccessRights::None, FileShareMode::All,
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
\see $2015-01 @ %Documentation::Workflow::Annual2015.
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


/*!	\defgroup native_encoding_conv Native Encoding Conversion
\brief 本机文本编码转换。
\exception YSLib::LoggedEvent 长度为负数或溢出 int 。
\since build 644

转换第一个 \c unsigned 参数指定编码的字符串为第二个 \c unsigned 参数指定的编码。
*/
//@{
//! \pre 间接断言：字符串指针参数非空。
YF_API YB_NONNULL(1) string
MBCSToMBCS(const char*, unsigned = CP_UTF8, unsigned = CP_ACP);
//! \pre 长度参数非零且不上溢 \c int 时间接断言：字符串指针参数非空。
YF_API string
MBCSToMBCS(string_view, unsigned = CP_UTF8, unsigned = CP_ACP);

//! \pre 间接断言：字符串指针参数非空。
YF_API YB_NONNULL(1) wstring
MBCSToWCS(const char*, unsigned = CP_ACP);
//! \pre 长度参数非零且不上溢 \c int 时间接断言：字符串指针参数非空。
YF_API wstring
MBCSToWCS(string_view, unsigned = CP_ACP);

//! \pre 间接断言：字符串指针参数非空。
YF_API YB_NONNULL(1) string
WCSToMBCS(const wchar_t*, unsigned = CP_ACP);
//! \pre 长度参数非零且不上溢 \c int 时间接断言：字符串指针参数非空。
YF_API string
WCSToMBCS(wstring_view, unsigned = CP_ACP);

//! \pre 间接断言：字符串指针参数非空。
inline YB_NONNULL(1) PDefH(wstring, UTF8ToWCS, const char* str)
	ImplRet(MBCSToWCS(str, CP_UTF8))
//! \pre 长度参数非零且不上溢 \c int 时间接断言：字符串指针参数非空。
inline PDefH(wstring, UTF8ToWCS, string_view sv)
	ImplRet(MBCSToWCS(sv, CP_UTF8))

//! \pre 间接断言：字符串指针参数非空。
inline YB_NONNULL(1) PDefH(string, WCSToUTF8, const wchar_t* str)
	ImplRet(WCSToMBCS(str, CP_UTF8))
//! \pre 长度参数非零且不上溢 \c int 时间接断言：字符串指针参数非空。
inline PDefH(string, WCSToUTF8, wstring_view sv)
	ImplRet(WCSToMBCS(sv, CP_UTF8))
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
	\invariant <tt>dir_name.length() > 1
		&& ystdex::ends_with(dir_name, L"\\*")</tt> 。
	\sa GetDirName
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
	\pre 间接断言：路径参数的数据指针非空。
	\throw platform::FileOperationFailure 打开路径失败，或指定的路径不是目录。
	\since build 658

	打开 UTF-16 路径指定的目录。
	目录路径无视结尾的斜杠和反斜杠。 去除结尾斜杠和反斜杠后若为空则视为当前路径。
	*/
	DirectoryFindData(wstring_view);
	//! \brief 析构：若查找节点句柄非空则关闭查找状态。
	~DirectoryFindData();

	//! \since build 556
	DefBoolNeg(explicit, h_node)

	//! \since build 564
	DefGetter(const ynothrow, unsigned long, Attributes,
		find_data.dwFileAttributes)
	DefGetter(const ynothrow, const ::WIN32_FIND_DATAW&, FindData, find_data)
	//! \since build 593
	DefGetter(const ynothrow, const wstring&, DirName, (YAssert(
		dir_name.length() > 1 && ystdex::ends_with(dir_name, L"\\*"),
		"Invalid directory name found."), dir_name))

	/*!
	\brief 取子节点的类型。
	\return 当前节点无效或查找的项目名称为空时为 platform::NodeCategory::Empty ，
		否则为处理文件信息得到的值。
	\sa CategorizeNode
	\since build 658

	处理文件信息时，首先调用 CategorizeNode 对查找数据归类；
	然后打开名称指定的文件进一步调用 CategorizeNode 对打开的文件判断归类。
	*/
	platform::NodeCategory
	GetNodeCategory() const ynothrow;

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
	\return 若迭代结束后节点且文件名非空，表示当前查找项目名的指针；否则为空指针。
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
\brief 读取重分析点内容。
\pre 断言：参数非空。
\exception Win32Exception 打开文件失败。
\throw std::invalid_argument 打开的文件不是重分析点。
\throw std::system_error 重分析点检查失败。
	\li std::errc::not_supported 重分析点标签不被支持。
\since build 660
*/
YF_API YB_NONNULL(1) wstring
ResolveReparsePoint(const wchar_t*);


/*!
\see https://msdn.microsoft.com/zh-cn/library/windows/desktop/aa363788(v=vs.85).aspx 。
\since build 638
*/
//@{
//! \brief 文件标识。
using FileID = std::uint64_t;
//! \brief 卷序列号。
using VolumeID = std::uint32_t;
//@}

//! \throw Win32Exception 访问文件或查询文件元数据失败。
//@{
//! \since build 660
//@{
//! \brief 查询文件链接数。
//@{
//! \since build 637
YF_API size_t
QueryFileLinks(UniqueHandle::pointer);
/*!
\pre 间接断言：路径参数非空。
\note 最后参数表示跟踪重解析点。
*/
YF_API YB_NONNULL(1) size_t
QueryFileLinks(const wchar_t*, bool = {});
//@}

/*!
\brief 查询文件标识。
\return 卷标识和卷上文件的标识的二元组。
\bug ReFS 上不保证唯一。
\see https://msdn.microsoft.com/zh-cn/library/windows/desktop/aa363788(v=vs.85).aspx 。
*/
//@{
//! \since build 638
YF_API pair<VolumeID, FileID>
QueryFileNodeID(UniqueHandle::pointer);
/*!
\pre 间接断言：路径参数非空。
\note 最后参数表示跟踪重解析点。
*/
YF_API YB_NONNULL(1) pair<VolumeID, FileID>
QueryFileNodeID(const wchar_t*, bool = {});
//@}
//@}

/*
\note 后三个参数可选，指针为空时忽略。
\note 最高精度取决于文件系统。
*/
//@{
//! \brief 查询文件的创建、访问和/或修改时间。
//@{
/*!
\pre 文件句柄不为 \c INVALID_HANDLE_VALUE ，
	且具有 AccessRights::GenericRead权限。
\since build 629
*/
YF_API void
QueryFileTime(UniqueHandle::pointer, ::FILETIME* = {}, ::FILETIME* = {},
	::FILETIME* = {});
/*!
\pre 间接断言：路径参数非空。
\note 即使可选参数都为空指针时仍访问文件。最后参数表示跟踪重解析点。
\since build 632
*/
YF_API YB_NONNULL(1) void
QueryFileTime(const wchar_t*, ::FILETIME* = {}, ::FILETIME* = {},
	::FILETIME* = {}, bool = {});
//@}

/*!
\brief 设置文件的创建、访问和/或修改时间。
\since build 651
*/
//@{
/*!
\pre 文件句柄不为 \c INVALID_HANDLE_VALUE ，
	且具有 FileSpecificAccessRights::WriteAttributes 权限。
*/
YF_API void
SetFileTime(UniqueHandle::pointer, ::FILETIME* = {}, ::FILETIME* = {},
	::FILETIME* = {});
/*!
\pre 间接断言：路径参数非空。
\note 即使可选参数都为空指针时仍访问文件。最后参数表示跟踪重解析点。
*/
YF_API YB_NONNULL(1) void
SetFileTime(const wchar_t*, ::FILETIME* = {}, ::FILETIME* = {},
	::FILETIME* = {}, bool = {});
//@}
//@}
//@}

/*!
\throw std::system_error 调用失败。
	\li std::errc::not_supported 输入的时间表示不被实现支持。
*/
//@{
/*!
\brief 转换文件时间为以 POSIX 历元起始度量的时间间隔。
\since build 632
*/
YF_API std::chrono::nanoseconds
ConvertTime(const ::FILETIME&);
/*!
\brief 转换以 POSIX 历元起始度量的时间间隔为文件时间。
\since build 651
*/
YF_API ::FILETIME
ConvertTime(std::chrono::nanoseconds);
//@}

/*!
\brief 展开字符串中的环境变量。
\pre 间接断言：参数非空。
\since build 658
*/
YF_API YB_NONNULL(1) wstring
ExpandEnvironmentStrings(const wchar_t*);

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

