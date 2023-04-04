/*
	© 2013-2016, 2018-2023 FrankHB.

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
\version r2642
\author FrankHB <frankhb1989@gmail.com>
\since build 412
\par 创建时间:
	2012-06-08 17:57:49 +0800
\par 修改时间:
	2023-03-31 18:05 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(Win32)::MinGW32
*/


#ifndef YCL_Win32_INC_MinGW32_h_
#define YCL_Win32_INC_MinGW32_h_ 1

#include "YCLib/YModules.h"
#include YFM_YCLib_Host // for YSLib::RecordLevel, YSLib::Emergent, std::string,
//	string_view, DefBitmaskEnum, ::HANDLE, wstring, unique_ptr_from,
//	ystdex::ends_with, ystdex::aligned_storage_t, ystdex::pun_ref, vector,
//	string, pair, ystdex::remove_pointer_t, std::is_function, YSLib::Err;
#include YFM_YCLib_NativeAPI // for ERROR_SUCCESS, ::HMODULE, GetModuleHandleW,
//	MAXIMUM_REPARSE_DATA_BUFFER_SIZE, INFINITE, MAX_PATH, ::HGLOBAL, ::HLOCAL;
#if !YCL_Win32
#	error "This file is only for Win32."
#endif
#include <ystdex/enum.hpp> // for ystdex::enum_union,
//	ystdex::wrapped_enum_traits_t;
#include <ystdex/base.h> // for ystdex::noncopyable;
#include YFM_YCLib_Debug // for platform::Nonnull, platform::Deref;
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
YB_ATTR_nodiscard YF_API YB_STATELESS int
ConvertToErrno(ErrorCode) ynothrow;

/*!
\brief 取转换为 errno 的 Win32 错误。
\since build 622
*/
YB_ATTR_nodiscard inline PDefH(int, GetErrnoFromWin32, ) ynothrow
	ImplRet(ConvertToErrno(::GetLastError()))


/*!
\ingroup exceptions
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
	//!@{
	//! \since build 861
	//!@{
	YB_NONNULL(3)
	Win32Exception(ErrorCode, const char* = "Win32 exception",
		YSLib::RecordLevel = YSLib::Emergent);
	Win32Exception(ErrorCode, const std::string&,
		YSLib::RecordLevel = YSLib::Emergent);
	Win32Exception(ErrorCode, string_view,
		YSLib::RecordLevel = YSLib::Emergent);
	//!@}
	/*!
	\pre 第三参数非空。
	\note 第三参数是表示调用位置函数签名，可以使用 \c __func__ 。
	*/
	YB_NONNULL(4)
	Win32Exception(ErrorCode, string_view, const char*,
		YSLib::RecordLevel = YSLib::Emergent);
	//!@}
	//! \since build 586
	DefDeCopyCtor(Win32Exception)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~Win32Exception() override;

	YB_ATTR_nodiscard DefGetter(const ynothrow, ErrorCode, ErrorCode,
		ErrorCode(code().value()))
	//! \since build 437
	YB_ATTR_nodiscard DefGetter(const ynothrow, std::string, Message,
		FormatMessage(GetErrorCode()))

	YB_ATTR_nodiscard explicit DefCvt(const ynothrow, ErrorCode, GetErrorCode())

	/*!
	\brief 取错误类别。
	\return std::error_category 派生类的 const 引用。
	\since build 545
	*/
	YB_ATTR_nodiscard YB_PURE static const std::error_category&
	GetErrorCategory();

	/*!
	\brief 格式化错误消息字符串。
	\return 若发生异常则结果为空，否则为区域固定为 en-US 的系统消息字符串。
	*/
	YB_ATTR_nodiscard YB_PURE static std::string
	FormatMessage(ErrorCode) ynothrow;
	//!@}
};


//! \since build 714
//!@{
/*!
\brief 按 \c ::GetLastError 的结果和指定参数抛出 Windows::Win32Exception 对象。
\note 先调用 \c ::GetLastError 以避免参数中的副作用影响结果。
*/
#	define YCL_Raise_Win32E(...) \
	do \
	{ \
		const auto err_(::GetLastError()); \
	\
		throw platform_ex::Windows::Win32Exception(err_, __VA_ARGS__); \
	}while(false)

//! \brief 按表达式求值和指定参数抛出 Windows::Win32Exception 对象。
#	define YCL_RaiseZ_Win32E(_expr, ...) \
	do \
	{ \
		const auto err_(Windows::ErrorCode(_expr)); \
	\
		if(err_ != ERROR_SUCCESS) \
			throw platform_ex::Windows::Win32Exception(err_, __VA_ARGS__); \
	}while(false)

//! \brief 跟踪 ::GetLastError 取得的调用状态结果。
#	define YCL_Trace_Win32E(_lv, _fn, _sig) \
	do \
	{ \
		const auto err_(::GetLastError()); \
	\
		yunused(err_); \
		YTraceDe(_lv, "Failed calling " #_fn " @ %s with error %lu: %s", \
			_sig, err_, \
			platform_ex::Win32Exception::FormatMessage(err_).c_str()); \
	}while(false)

/*!
\brief 调用 Win32 API 或其它可用 ::GetLastError 取得调用状态的例程。
\note 调用时直接使用实际参数，可指定非标识符的表达式，不保证是全局名称。
*/
//!@{
/*!
\note 若失败抛出 Windows::Win32Exception 对象。
\sa YCL_Raise_Win32E
*/
//!@{
#	define YCL_WrapCall_Win32(_fn, ...) \
	[&] YB_LAMBDA_ANNOTATE((const char* sig_), , nonnull(2)){ \
		const auto res_(_fn(__VA_ARGS__)); \
	\
		if(YB_UNLIKELY(!res_)) \
			YCL_Raise_Win32E(#_fn, sig_); \
		return res_; \
	}

#	define YCL_Call_Win32(_fn, _sig, ...) \
	YCL_WrapCall_Win32(_fn, __VA_ARGS__)(_sig)

//! \since build 638
#	define YCL_CallF_Win32(_fn, ...) YCL_Call_Win32(_fn, yfsig, __VA_ARGS__)
//!@}

/*!
\note 若失败跟踪 ::GetLastError 的结果。
\note 格式转换说明符置于最前以避免宏参数影响结果。
\sa YCL_Trace_Win32E
*/
//!@{
#	define YCL_TraceWrapCall_Win32(_fn, ...) \
	[&] YB_LAMBDA_ANNOTATE((const char* sig_), , nonnull(2)){ \
		const auto res_(_fn(__VA_ARGS__)); \
	\
		yunused(sig_); \
		if(YB_UNLIKELY(!res_)) \
			YCL_Trace_Win32E(platform::Descriptions::Warning, _fn, sig_); \
		return res_; \
	}

#	define YCL_TraceCall_Win32(_fn, _sig, ...) \
	YCL_TraceWrapCall_Win32(_fn, __VA_ARGS__)(_sig)

#	define YCL_TraceCallF_Win32(_fn, ...) \
	YCL_TraceCall_Win32(_fn, yfsig, __VA_ARGS__)
//!@}
//!@}
//!@}


//! \since build 629
//!@{
/*!
\brief 访问权限。
\see https://msdn.microsoft.com/library/windows/desktop/aa374892.aspx 。
\see https://msdn.microsoft.com/library/windows/desktop/aa374896.aspx 。
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
//!@}


//! \since build 971
//!@{
//! \see https://msdn.microsoft.com/library/gg258117.aspx 。
enum class FileAttributes : unsigned long
{
	//! \since build 639
	//!@{
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
	//!@}
};

//! \relates FileAttributes
DefBitmaskEnum(FileAttributes)
//!@}


//! \since build 639
//!@{
//! \see https://msdn.microsoft.com/library/aa363858.aspx 。
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
	// \see https://msdn.microsoft.com/library/windows/desktop/aa365150.aspx 。
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
//!@}


//! \since build 971
//!@{
enum class FileAttributesAndFlags : unsigned long
{
	//! \since build 639
	NormalWithDirectory = unsigned(FileAttributes::Normal) | BackupSemantics,
	//! \since build 639
	NormalAll = NormalWithDirectory | OpenReparsePoint
};

//! \relates FileAttributesAndFlags
DefBitmaskEnum(FileAttributesAndFlags)
//!@}


/*!
\brief 判断 \c FileAttributes 是否指定目录。
\since build 701
*/
YB_ATTR_nodiscard YB_STATELESS yconstfn
	PDefH(bool, IsDirectory, FileAttributes attr) ynothrow
	ImplRet(bool(attr & FileAttributes::Directory))
/*!
\brief 判断 \c ::WIN32_FIND_DATAA 指定的节点是否为目录。
\since build 298
*/
YB_ATTR_nodiscard YB_STATELESS inline
	PDefH(bool, IsDirectory, const ::WIN32_FIND_DATAA& d) ynothrow
	ImplRet(IsDirectory(FileAttributes(d.dwFileAttributes)))
/*!
\brief 判断 \c ::WIN32_FIND_DATAW 指定的节点是否为目录。
\since build 299
*/
YB_ATTR_nodiscard YB_STATELESS inline
	PDefH(bool, IsDirectory, const ::WIN32_FIND_DATAW& d) ynothrow
	ImplRet(IsDirectory(FileAttributes(d.dwFileAttributes)))

/*!
\throw Win32Exception 调用失败。
\since build 701
*/
//!@{
/*!
\brief 按打开的文件句柄归类节点从属的属性类别。
\return 指定句柄为字符、管道或未知类别。
*/
YB_ATTR_nodiscard YF_API platform::NodeCategory
TryCategorizeNodeAttributes(UniqueHandle::pointer);

/*!
\brief 按打开的文件句柄归类节点从属的设备类别。
\return 指定句柄为字符、管道或未知类别。
*/
YB_ATTR_nodiscard YF_API platform::NodeCategory
TryCategorizeNodeDevice(UniqueHandle::pointer);
//!@}

/*!
\return 指定非目录或不被支持的重解析标签时为 NodeCategory::Empty ，
	否则为对应的目录和重解析标签的组合节点类别。
\since build 701
\todo 对目录链接和符号链接的重解析标签提供适当实现。
*/
//!@{
//! \brief 按 FileAttributes 和重解析标签归类节点类别。
YB_ATTR_nodiscard YF_API YB_STATELESS platform::NodeCategory
CategorizeNode(FileAttributes, unsigned long = 0) ynothrow;
//! \brief 按 \c ::WIN32_FIND_DATAA 归类节点类别。
YB_ATTR_nodiscard YB_STATELESS inline PDefH(platform::NodeCategory,
	CategorizeNode, const ::WIN32_FIND_DATAA& d) ynothrow
	ImplRet(CategorizeNode(FileAttributes(d.dwFileAttributes), d.dwReserved0))
/*!
\brief 按 \c ::WIN32_FIND_DATAW 归类节点类别。
\since build 658
*/
YB_ATTR_nodiscard YB_STATELESS inline PDefH(platform::NodeCategory,
	CategorizeNode, const ::WIN32_FIND_DATAW& d) ynothrow
	ImplRet(CategorizeNode(FileAttributes(d.dwFileAttributes), d.dwReserved0))
//!@}
/*!
\brief 按打开的文件句柄归类节点类别。
\return 指定句柄空时为 NodeCategory::Invalid ，
	否则为文件属性和设备类别查询的位或结果。
\sa TryCategorizeNodeAttributes
\sa TryCategorizeNodeDevice
\since build 701
*/
YB_ATTR_nodiscard YF_API YB_PURE platform::NodeCategory
CategorizeNode(UniqueHandle::pointer) ynothrow;


/*!
\brief 创建或打开独占的文件或设备。
\pre 间接断言：路径参数非空。
\note 调用 \c ::CreateFileW 实现。
\since build 632
*/
//!@{
YB_ATTR_nodiscard YF_API YB_NONNULL(1) UniqueHandle
MakeFile(const wchar_t*, FileAccessRights = AccessRights::None,
	FileShareMode = FileShareMode::All, CreationDisposition
	= CreationDisposition::OpenExisting,
	FileAttributesAndFlags = FileAttributesAndFlags::NormalAll) ynothrowv;
//! \since build 660
YB_ATTR_nodiscard YB_NONNULL(1) inline PDefH(UniqueHandle, MakeFile,
	const wchar_t* path, FileAccessRights desired_access, FileShareMode
	shared_mode, FileAttributesAndFlags attributes_and_flags) ynothrowv
	ImplRet(MakeFile(path, desired_access, shared_mode,
		CreationDisposition::OpenExisting, attributes_and_flags))
YB_ATTR_nodiscard YB_NONNULL(1) inline PDefH(UniqueHandle, MakeFile, const wchar_t* path,
	FileAccessRights desired_access, CreationDisposition creation_disposition,
	FileAttributesAndFlags attributes_and_flags
	= FileAttributesAndFlags::NormalAll) ynothrowv
	ImplRet(MakeFile(path, desired_access, FileShareMode::All,
		creation_disposition, attributes_and_flags))
//! \since build 637
YB_ATTR_nodiscard YB_NONNULL(1) inline PDefH(UniqueHandle, MakeFile,
	const wchar_t* path, FileAccessRights desired_access,
	FileAttributesAndFlags attributes_and_flags) ynothrowv
	ImplRet(MakeFile(path, desired_access, FileShareMode::All,
		CreationDisposition::OpenExisting, attributes_and_flags))
//! \since build 660
//!@{
YB_ATTR_nodiscard YB_NONNULL(1) inline PDefH(UniqueHandle, MakeFile,
	const wchar_t* path, FileShareMode shared_mode, CreationDisposition
	creation_disposition = CreationDisposition::OpenExisting,
	FileAttributesAndFlags attributes_and_flags
	= FileAttributesAndFlags::NormalAll) ynothrowv
	ImplRet(MakeFile(path, AccessRights::None, shared_mode,
		creation_disposition, attributes_and_flags))
//! \since build 701
YB_ATTR_nodiscard YB_NONNULL(1) inline PDefH(UniqueHandle, MakeFile,
	const wchar_t* path, CreationDisposition creation_disposition,
	FileAttributesAndFlags attributes_and_flags
	= FileAttributesAndFlags::NormalAll) ynothrowv
	ImplRet(MakeFile(path, AccessRights::None, FileShareMode::All,
		creation_disposition, attributes_and_flags))
YB_ATTR_nodiscard YB_NONNULL(1) inline PDefH(UniqueHandle, MakeFile,
	const wchar_t* path, FileShareMode shared_mode, FileAttributesAndFlags
	attributes_and_flags = FileAttributesAndFlags::NormalAll) ynothrowv
	ImplRet(MakeFile(path, AccessRights::None, shared_mode,
		CreationDisposition::OpenExisting, attributes_and_flags))
YB_ATTR_nodiscard YB_NONNULL(1) inline PDefH(UniqueHandle, MakeFile,
	const wchar_t* path, FileAttributesAndFlags attributes_and_flags) ynothrowv
	ImplRet(MakeFile(path, AccessRights::None, FileShareMode::All,
		CreationDisposition::OpenExisting, attributes_and_flags))
//!@}
//!@}


/*!
\brief 判断是否在 Wine 环境下运行。
\note 检查 \c HKEY_CURRENT_USER 和 \c HKEY_LOCAL_MACHINE
	下的 \c Software\Wine 键实现。
\since build 435
*/
YB_ATTR_nodiscard YF_API bool
CheckWine();

/*!
\brief 判断参数是否关联 Cygwin/MSYS 程序使用的 PTY 模拟的对象名称。
\return 参数关联的对象名称符合 Cygwin/MSYS 的对象名称模式。
\note MinTTY 等非 Win32 控制台实现的终端模拟器可能使用 PTY 模拟。
\see $2021-06 @ %Documentation::Workflow 。
\since build 969

查询参数指定的句柄关联的对象名称信息，检查是否符合特定的名称模式。
Cygwin/MSYS 程序可使用管道模拟 PTY ，且其对象名称具有这些名称模式。
若查询对象名称失败，则视为不关联。
模拟对象的句柄指定管道，即使用 ::GetFileType 得到类型为 FILE_TYPE_PIPE 的文件。
判断句柄指定管道后，判断关联的对象 NT 名称一般即可确定管道是 PTY 模拟对象。
*/
YB_ATTR_nodiscard YF_API bool
HasPTYName(::HANDLE h) ynothrow;


/*!
\brief 文件系统目录查找状态。
\warning 非虚析构。
\since build 549
*/
class YF_API DirectoryFindData : private ystdex::noncopyable
{
private:
	/*!
	\ingroup deleters
	\since build 702
	*/
	class YF_API Deleter
	{
	public:
		using pointer = ::HANDLE;

		void
		operator()(pointer) const ynothrowv;
	};

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
	/*!
	\brief 查找节点。
	\since build 702
	*/
	unique_ptr_from<Deleter> p_node{};

public:
	/*!
	\brief 构造：使用指定的目录路径。
	\pre 间接断言：路径参数的数据指针非空。
	\throw std::system_error 指定的路径不是目录。
	\throw Win32Exception 路径属性查询失败。
	\since build 971

	打开 UTF-16 路径指定的目录。
	目录路径无视结尾的斜杠和反斜杠。 去除结尾斜杠和反斜杠后若为空则视为当前路径。
	*/
	//!@{
	DirectoryFindData(wstring_view sv, wstring::allocator_type a = {})
		: DirectoryFindData(wstring(sv, a))
	{}
	DirectoryFindData(u16string_view sv, u16string::allocator_type a = {})
		: DirectoryFindData(wstring(sv.cbegin(), sv.cend(), a))
	{}
	DirectoryFindData(const wstring& str)
		: DirectoryFindData(wstring(str, str.get_allocator()))
	{}
	DirectoryFindData(wstring&&);
	// XXX: Constructor from 'const u16string&' is not provided to prevent
	//	inefficient path by creation of unexpected 'u16string' objects, since
	//	construction from 'u16string&&' is not available due to strict aliasing.
	//!@}
	//! \brief 析构：若查找节点句柄非空则关闭查找状态。
	DefDeDtor(DirectoryFindData)

	//! \since build 556
	YB_ATTR_nodiscard DefBoolNeg(YB_ATTR_nodiscard explicit, p_node.get())

	//! \since build 564
	YB_ATTR_nodiscard DefGetter(const ynothrow, unsigned long, Attributes,
		find_data.dwFileAttributes)
	/*!
	\brief 返回当前查找项目名。
	\return 当前查找项目名。
	\since build 705
	*/
	YB_ATTR_nodiscard DefGetter(const ynothrow,
		const wchar_t*, EntryName, find_data.cFileName)
	YB_ATTR_nodiscard DefGetter(const ynothrow, const ::WIN32_FIND_DATAW&,
		FindData, find_data)
	//! \since build 593
	YB_ATTR_nodiscard DefGetter(const ynothrow, const wstring&, DirName,
		(YAssert(dir_name.length() > 1 && ystdex::ends_with(dir_name, L"\\*"),
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
	YB_ATTR_nodiscard platform::NodeCategory
	GetNodeCategory() const ynothrow;

	/*!
	\brief 读取：迭代当前查找状态。
	\throw Win32Exception 读取失败。
	\return 若迭代结束后节点且文件名非空。
	\since build 705

	若查找节点句柄非空则迭代当前查找状态查找下一个文件系统项。
	否则查找节点句柄为空或迭代失败则关闭查找状态并置查找节点句柄空。
	最终查找节点非空时保存记录当前查找的项目状态。
	*/
	bool
	Read();

	//! \brief 复位查找状态：若查找节点句柄非空则关闭查找状态并置查找节点句柄空。
	void
	Rewind() ynothrow;
};


/*!
\brief 重解析点数据。
\note 避免直接使用指针转换成显式不同的类型时引起未定义行为。
\warning 非虚析构。
\since build 705
*/
class YF_API ReparsePointData
{
public:
	struct Data;

private:
	//! \since build 712
	ystdex::aligned_storage_t<MAXIMUM_REPARSE_DATA_BUFFER_SIZE,
		yalignof(void*)> target_buffer;
	//! \invariant <tt>&pun.get() == &target_buffer</tt>
	ystdex::pun_ref<Data> pun;

public:
	ReparsePointData();
	/*!
	\brief 析构：类定义外默认实现。
	\note 允许 Data 作为不完整类型使用。
	*/
	~ReparsePointData();

	YB_ATTR_nodiscard DefGetter(const ynothrow, Data&, , pun.get())
};


/*!
\brief 读取重解析点内容。
\pre 断言：参数非空。
\exception Win32Exception 打开文件失败。
\throw std::invalid_argument 打开的文件不是重解析点。
\throw std::system_error 重解析点检查失败。
	\li std::errc::not_supported 重解析点标签不被支持。
*/
//!@{
//! \since build 660
YB_ATTR_nodiscard YF_API YB_NONNULL(1) wstring
ResolveReparsePoint(const wchar_t*);
//! \since build 705
YB_ATTR_nodiscard YF_API YB_NONNULL(1) wstring_view
ResolveReparsePoint(const wchar_t*, ReparsePointData::Data&);
//!@}


/*!
\brief 展开字符串中的环境变量。
\pre 间接断言：参数非空。
\throw Win32Exception 调用失败。
\since build 658
*/
YB_ATTR_nodiscard YF_API YB_NONNULL(1) wstring
ExpandEnvironmentStrings(const wchar_t*);

/*!
\brief 解析应用程序命令行参数。
\return UTF-8 编码的参数向量。
\since build 928
\see $2020-10 @ %Documentation::Workflow 。

解析 Unicode 编码的应用程序命令行参数字符串。
参数向量同标准 C++ 的 \c ::main 命令行参数的格式，但不依赖实现使用的代码页。
不依赖入口函数是否为 \c ::main 及其具体声明形式。
特别地，使用声明为 \c ::WinMain 和 \c ::wWinMain 入口函数的程序也使用一致的参数。
解析参数字符串不展开通配符。具体行为兼容 UCRT 的实现，不保证和 CRT 历史实现完全一致。
*/
//!@{
//! \note 以 \c ::GetCommandLineW 调用取参数。
YB_ATTR_nodiscard YF_API YB_PURE vector<string>
ParseCommandArguments(vector<string>::allocator_type = {});
//! \pre 断言：参数非空。
YB_ATTR_nodiscard YF_API YB_NONNULL(1) YB_PURE vector<string>
ParseCommandArguments(const wchar_t*, vector<string>::allocator_type = {});
//!@}


/*!
\see https://msdn.microsoft.com/library/windows/desktop/aa363788.aspx 。
\since build 638
*/
//!@{
//! \brief 文件标识。
using FileID = std::uint64_t;
//! \brief 卷序列号。
using VolumeID = std::uint32_t;
//!@}

//! \throw Win32Exception 访问文件或查询文件元数据失败。
//!@{
//! \since build 660
//!@{
//! \brief 查询文件链接数。
//!@{
//! \since build 637
YB_ATTR_nodiscard YF_API size_t
QueryFileLinks(UniqueHandle::pointer);
/*!
\pre 间接断言：路径参数非空。
\note 最后参数表示跟踪重解析点。
*/
YB_ATTR_nodiscard YF_API YB_NONNULL(1) size_t
QueryFileLinks(const wchar_t*, bool = {});
//!@}

/*!
\brief 查询文件标识。
\return 卷标识和卷上文件的标识的二元组。
\bug ReFS 上不保证唯一。
\see https://msdn.microsoft.com/library/windows/desktop/aa363788.aspx 。
\see https://bugs.python.org/issue40095 。
*/
//!@{
//! \since build 638
YB_ATTR_nodiscard YF_API pair<VolumeID, FileID>
QueryFileNodeID(UniqueHandle::pointer);
/*!
\pre 间接断言：路径参数非空。
\note 最后参数表示跟踪重解析点。
*/
YB_ATTR_nodiscard YF_API YB_NONNULL(1) pair<VolumeID, FileID>
QueryFileNodeID(const wchar_t*, bool = {});
//!@}
//!@}

/*!
\brief 查询文件大小。
\throw std::invalid_argument 查询文件得到的大小小于 0 。
\since build 718
*/
//!@{
YB_ATTR_nodiscard YF_API std::uint64_t
QueryFileSize(UniqueHandle::pointer);
//! \pre 间接断言：路径参数非空。
YB_ATTR_nodiscard YF_API YB_NONNULL(1) std::uint64_t
QueryFileSize(const wchar_t*);
//!@}

/*
\note 后三个参数可选，指针为空时忽略。
\note 最高精度取决于文件系统。
*/
//!@{
//! \brief 查询文件的创建、访问和/或修改时间。
//!@{
/*!
\pre 文件句柄不为 \c INVALID_HANDLE_VALUE ，
	且具有 AccessRights::GenericRead 权限。
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
//!@}

/*!
\brief 设置文件的创建、访问和/或修改时间。
\since build 651
*/
//!@{
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
//!@}
//!@}
//!@}

/*!
\throw std::system_error 调用失败。
	\li std::errc::not_supported 输入的时间表示不被实现支持。
*/
//!@{
/*!
\brief 转换文件时间为以 POSIX 历元起始度量的时间间隔。
\since build 632
*/
YB_ATTR_nodiscard YF_API std::chrono::nanoseconds
ConvertTime(const ::FILETIME&);
/*!
\brief 转换以 POSIX 历元起始度量的时间间隔为文件时间。
\since build 651
*/
YB_ATTR_nodiscard YF_API ::FILETIME
ConvertTime(std::chrono::nanoseconds);
//!@}


/*!
\pre 文件句柄不为 \c INVALID_HANDLE_VALUE ，
	且具有 AccessRights::GenericRead 或 AccessRights::GenericWrite 权限。
\since build 901
*/
//!@{
/*!
\brief 锁定文件。
\note 对内存映射文件为协同锁，其它文件为强制锁。
\note 第二和第三参数指定文件锁定范围的起始偏移量和大小。
\note 最后两个参数分别表示是否为独占锁和是否立刻返回。
*/
//!@{
//! \throw Win32Exception 锁定失败。
YF_API void
LockFile(UniqueHandle::pointer, std::uint64_t = 0,
	std::uint64_t = std::uint64_t(-1), bool = true, bool = {});

YB_ATTR_nodiscard YF_API bool
TryLockFile(UniqueHandle::pointer, std::uint64_t = 0,
	std::uint64_t = std::uint64_t(-1), bool = true, bool = true) ynothrow;
//!@}

/*!
\brief 解锁文件。
\pre 文件已被锁定。
*/
YF_API bool
UnlockFile(UniqueHandle::pointer, std::uint64_t = 0,
	std::uint64_t = std::uint64_t(-1)) ynothrow;
//!@}


/*!
\brief 以指定的时间间隔上限等待句柄指定的同步对象。
\note 时间间隔单位为毫秒。
\since build 720
*/
//!@{
//! \throw 等待失败。
YF_API void
WaitUnique(UniqueHandle::pointer, unsigned long = INFINITE);

//! \return 是否等待成功。
YB_ATTR_nodiscard YF_API bool
TryWaitUnique(UniqueHandle::pointer, unsigned long = 0) ynothrow;
//!@}


/*!
\brief 互斥量：可用于进程间同步。
\note 满足 Lockable 要求。
\since build 713
*/
class YF_API Mutex : private ystdex::noncopyable
{
public:
	using native_handle_type = UniqueHandle::pointer;

private:
	UniqueHandle h_mutex;

public:
	//! \brief 使用内核对象名。
	Mutex(const wchar_t*);

	//! \note 允许递归锁。
	PDefH(void, lock, )
		ImplExpr(WaitUnique(native_handle()))

	YB_ATTR_nodiscard PDefH(bool, try_lock, ) ynothrow
		ImplRet(TryWaitUnique(native_handle()))

	/*!
	\pre 线程对互斥量具有所有权。
	\since build 714
	*/
	void
	unlock() ynothrowv;

	//! \see WG21 N4606 30.2.3[thread.req.native] 。
	PDefH(native_handle_type, native_handle, )
		ImplRet(h_mutex.get())
};


/*!
\throw Win32Exception 调用失败。
\note 保证以一个分隔符结束。
\since build 971
*/
//!@{
//! \brief 取系统目录路径。
YB_ATTR_nodiscard YF_API wstring
FetchSystemPath(wstring::allocator_type a = {}, size_t = MAX_PATH);

//! \brief 取系统目录路径。
YB_ATTR_nodiscard YF_API wstring
FetchWindowsPath(wstring::allocator_type a = {}, size_t = MAX_PATH);
//!@}


/*!
\note 当前只支持 x64 系统下 32 位进程启用的重定向，不支持包括 ARM64 的其它情形。
\since build 937
*/
//!@{
/*!
\brief 判断当前进程是否为 WOW64 进程。
\note 调用的 API 结果会被缓存，在进程生存期中不会改变。
*/
YB_ATTR_nodiscard YF_API YB_STATELESS bool
IsWOW64Process() ynothrow;

/*!
\brief WOW64 文件系统重定向守卫：允许临时禁用重定向。
\warning 非虚析构。
*/
class YF_API WOW64FileSystemRedirectionGuard : private ystdex::noncopyable
{
private:
	bool activated;
	void* old_value;

public:
	//! \brief 构造：通过参数指定是否激活临时禁用重定向。
	WOW64FileSystemRedirectionGuard(bool = {});
	WOW64FileSystemRedirectionGuard(WOW64FileSystemRedirectionGuard&&);
	~WOW64FileSystemRedirectionGuard();

	WOW64FileSystemRedirectionGuard&
	operator=(WOW64FileSystemRedirectionGuard&&) ynothrow;

	YB_ATTR_nodiscard DefPred(ynothrow, Activated, activated)
};
//!@}


/*!
\brief 取模块映像路径。
\since build 960
*/
YB_ATTR_nodiscard YF_API wstring
FetchModuleFileName(::HMODULE = {}, wstring::allocator_type = {},
	YSLib::RecordLevel = YSLib::Err);

//! \since build 937
//!@{
//! \brief 取进程已加载的指定名称的模块句柄。
YB_ATTR_nodiscard YB_NONNULL(1) inline
	PDefH(::HMODULE, FetchModuleHandle, const wchar_t* module)
	ImplRet(YCL_CallF_Win32(GetModuleHandleW, platform::Nonnull(module)))

//! \brief 取内部缓存的进程已加载的指定名称的模块句柄。
YB_ATTR_nodiscard YF_API YB_NONNULL(1) ::HMODULE
FetchModuleHandleCached(const wchar_t*);
//!@}

//! \since build 651
//!@{
//! \brief 加载过程地址得到的过程类型。
using ModuleProc
	= ystdex::remove_pointer_t<decltype(::GetProcAddress(::HMODULE(), {}))>;
// XXX: Microsoft VC++'s compiler (but not IntelliSense) would not play well on
//	'decltype' with an lvalue of function type, as there is an unexpected
//	function-to-pointer conversion, so
//	'decltype(*::GetProcAddress(::HMODULE(), {}))' is actually a function
//	pointer type, instead of a function reference type in an conforming
//	implementation. An alternative workaround is
//	'decltype(::GetProcAddress(::HMODULE(), {})())(__stdcall)()', requiring the
//	explicit calling conversion Anyway, handling on the pointer here is simpler.

//! \since build 923
static_assert(std::is_function<ModuleProc>(), "Invalid type found.");

/*!
\brief 从模块加载指定过程的指针。
\pre 参数非空。
\exception Win32Exception 动态加载失败。
*/
//!@{
YB_ATTR_nodiscard YF_API YB_ATTR_returns_nonnull YB_NONNULL(2) ModuleProc*
LoadProc(::HMODULE, const char*);
template<typename _func>
YB_ATTR_nodiscard YB_NONNULL(2) inline _func&
LoadProc(::HMODULE h_module, const char* proc)
{
	static_assert(std::is_function<_func>(), "Invalid function type found.");

	// NOTE: See https://gcc.gnu.org/gcc-8/changes.html.
#if YB_IMPL_GNUCPP >= 80000
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wcast-function-type"
#endif
	// NOTE: This should be safe with the additional ABI guarantees, similar to
	//	https://debarshiray.wordpress.com/2019/04/01/about-wextra-and-wcast-function-type/.
	return
	// XXX: This should be OK for other implementations, but just keep it for
	//	the specific buggy implementations.
#if YB_IMPL_MSCPP
		// XXX: This is required sometimes with %platform::Deref, as the result
		//	may be a function pointer instead of the function reference. It
		//	seems a compiler bug in VC++ for the inconsistent behaviors. The
		//	diagnostic message shows the type of the expression is the type
		//	the template parameter '_type' in of %platform::Deref, which
		//	indicates some improper deduction directly performed in the call and
		//	there is an unexpected function-to-pointer conversion. Such
		//	conversion seems to be in the unevaluated lvalue operand of
		//	'decltype', behaving same to the common implementation of
		//	%ModuleProc.
		*
#endif
		platform::Deref(reinterpret_cast<_func*>(LoadProc(h_module, proc)));
#if YB_IMPL_GNUCPP >= 80000
#	pragma GCC diagnostic pop
#endif
}
template<typename _func>
YB_ATTR_nodiscard YB_NONNULL(1, 2) inline _func&
LoadProc(const wchar_t* module, const char* proc)
{
	return LoadProc<_func>(FetchModuleHandle(module), proc);
}

/*!
\pre 加载的模块不被改变（包括卸载）。
\note 使用全局内部缓存以减少重复的 Win32 API 调用。
*/
//!@{
YB_ATTR_nodiscard YF_API YB_ATTR_returns_nonnull YB_NONNULL(2) ModuleProc*
LoadProcCached(::HMODULE, const char*);
template<typename _func>
YB_ATTR_nodiscard YB_NONNULL(2) inline _func&
LoadProcCached(::HMODULE h_module, const char* proc)
{
	static_assert(std::is_function<_func>(), "Invalid function type found.");

	// NOTE: See https://gcc.gnu.org/gcc-8/changes.html.
#if YB_IMPL_GNUCPP >= 80000
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wcast-function-type"
#endif
	// NOTE: Ditto.
	return
	// XXX: Ditto.
#if YB_IMPL_MSCPP
	// XXX: Ditto.
		*
#endif
		platform::Deref(
			reinterpret_cast<_func*>(LoadProcCached(h_module, proc)));
#if YB_IMPL_GNUCPP >= 80000
#	pragma GCC diagnostic pop
#endif
}
template<typename _func>
YB_ATTR_nodiscard YB_NONNULL(1, 2) inline _func&
LoadProcCached(const wchar_t* module, const char* proc)
{
	return LoadProcCached<_func>(FetchModuleHandleCached(module), proc);
}
//!@}
//!@}


#define YCL_Impl_W32Call_Fn(_fn) W32_##_fn##_t
#define YCL_Impl_W32Call_FnCall(_fn) W32_##_fn##_call

/*!
\brief 声明调用 WinAPI 的例程。
\note 函数名称使用 ADL 查找调用，应为非限定名称。当没有合式调用时从指定模块加载。
\note 为避免歧义，应在非全局命名空间中使用；注意类作用域名称查找顺序会引起歧义。
\note 为避免无限递归实例化，需声明的参数类型应在当前命名空间之外提前声明。
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
		return platform_ex::Windows::LoadProcCached<YCL_Impl_W32Call_Fn(_fn)>( \
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
//!@}


//! \since build 593
//!@{
/*!
\ingroup functors
\brief 全局存储删除器。
*/
struct YF_API GlobalDelete
{
	using pointer = ::HGLOBAL;

	void
	operator()(pointer) const ynothrow;
};


/*!
\brief 全局锁定存储。
\warning 非虚析构。
*/
class YF_API GlobalLocked
{
private:
	//! \invariant \c bool(p_locked) 。
	void* p_locked;

public:
	/*!
	\brief 构造：锁定存储。
	\throw Win32Exception ::GlobalLock 调用失败。
	*/
	//!@{
	GlobalLocked(::HGLOBAL);
	template<typename _tPointer>
	GlobalLocked(const _tPointer& p)
		: GlobalLocked(p.get())
	{}
	//!@}
	~GlobalLocked();

	template<typename _type = void>
	YB_ATTR_nodiscard observer_ptr<_type>
	GetPtr() const ynothrow
	{
		return make_observer(static_cast<_type*>(p_locked));
	}
};
//!@}


/*!
\ingroup functors
\brief 局部存储删除器。
\since build 658
*/
struct YF_API LocalDelete
{
	using pointer = ::HLOCAL;

	void
	operator()(pointer) const ynothrow;
};

} // inline namespace Windows;

} // namespace platform_ex;

#endif

