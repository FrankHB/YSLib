/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file FileSystem.h
\ingroup YCLib
\brief 平台相关的文件系统接口。
\version r2783
\author FrankHB <frankhb1989@gmail.com>
\since build 312
\par 创建时间:
	2012-05-30 22:38:37 +0800
\par 修改时间:
	2015-11-26 14:19 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::FileSystem
*/


#ifndef YCL_INC_FileSystem_h_
#define YCL_INC_FileSystem_h_ 1

#include "YModules.h"
#include YFM_YCLib_Container // for std::is_integral, std::is_array,
//	ystdex::remove_reference_t, arrlen, ystdex::is_null, string, u16string,
//	ystdex::to_array, std::uint8_t, std::uint32_t, pair, tuple;
#include <system_error> // for std::system_error;
#include <ystdex/base.h> // for ystdex::deref_self;
#if YCL_DS || YCL_MinGW || YCL_Linux || YCL_OS_X
#	include <dirent.h>
#endif
#include <ystdex/iterator.hpp> // for ystdex::indirect_input_iterator;
#include <ctime> // for std::time_t;
#include YFM_YCLib_Debug // for Nonnull, Deref;
#include <ystdex/cstdint.hpp> // for ystdex::read_uint_le;

namespace platform
{

/*!
\brief 判断字符串是否是当前路径。
\since build 409
*/
#define YCL_FS_StringIsCurrent(_s, _p) \
	(ystdex::string_length(_s) == 1 && _s[0] == _p('.'))

/*!
\brief 判断字符串是否是父目录。
\since build 409
*/
#define YCL_FS_StringIsParent(_s, _p) \
	(ystdex::string_length(_s) == 2 && _s[0] == _p('.') && _s[1] == _p('.'))

/*!
\def YCL_FS_CharIsDelimiter
\brief 判断字符是否路径分隔符。
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

#	define YCL_FS_CharIsDelimiter(_c, _p) (_c == _p('/') || _c == _p('\\'))
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

#	define YCL_FS_CharIsDelimiter(_c, _p) (_c == _p(YCL_PATH_DELIMITER))
#else
#	error "Unsupported platform found."
#endif

//! \since build 402
//@{
static_assert(std::is_integral<decltype(YCL_PATH_DELIMITER)>(),
	"Illegal type of delimiter found.");
static_assert(std::is_array<ystdex::remove_reference_t<decltype(
	YCL_PATH_SEPARATOR)>>(), "Non-array type of separator found.");
static_assert(arrlen(YCL_PATH_SEPARATOR) == 2,
	"Wrong length of separator found.");
static_assert(YCL_PATH_SEPARATOR[0] == YCL_PATH_DELIMITER,
	"Mismatched path delimiter and separator found.");
static_assert(ystdex::is_null(YCL_PATH_SEPARATOR[1]),
	"Non-null-terminator as end of separator.");
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
\pre 路径非空。
\exception std::system_error 系统错误。
\note 参数为目标路径和源路径。
\note DS 平台：当前实现不支持替换文件系统，因此始终不支持操作。
*/
//@{
/*!
\brief 创建硬链接。
\note 源路径指定符号链接时，行为未指定。
\since build 633
*/
//@{
YF_API YB_NONNULL(1, 2) void
CreateHardLink(const char*, const char*);
YF_API YB_NONNULL(1, 2) void
CreateHardLink(const char16_t*, const char16_t*);
//@}

/*!
\brief 创建符号链接。
\note 第三参数指定是否创建目录链接。
\note Win32 平台：成功调用需要操作系统和权限（或组策略）支持。
\note 非 Win32 平台：忽略第三参数。
\since build 651
*/
//@{
YF_API YB_NONNULL(1, 2) void
CreateSymbolicLink(const char*, const char*, bool = {});
YF_API YB_NONNULL(1, 2) void
CreateSymbolicLink(const char16_t*, const char16_t*, bool = {});
//@}
//@}


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

protected:
	/*!
	\brief 目录路径。
	\invariant <tt>ystdex::string_length(sDirPath.c_str()) > 0 && \
		sDirPath.back() == YCL_PATH_DELIMITER</tt> 。
	\since build 593
	*/
	string sDirPath;
#endif

private:
	NativeHandle dir;

public:
	/*!
	\brief 构造：打开目录路径。
	\pre 间接断言：参数非空。
	\throw FileOperationFailure 打开失败。
	\note 路径可以一个或多个分隔符结尾；当路径为空字符串时视为 "." 。
	\note Win32 平台： "/" 可能也被作为分隔符支持。
	\note Win32 平台： 前缀 "\\?\" 关闭非结尾的 "/" 分隔符支持，
		且无视 MAX_PATH 限制。
	\since build 654
	*/
	explicit
	DirectorySession(const char* path = ".");
	//! \since build 560
	DirectorySession(DirectorySession&& h) ynothrow
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

public:
	//! \since build 648
	using NativeChar =
#if YCL_Win32
		char16_t
#else
		char
#endif
		;

private:
#if YCL_Win32
	/*!
	\brief 节点信息。
	\invariant <tt>!p_dirent || bool(GetNativeHandle())</tt> 。
	\invariant <tt>!p_dirent || !static_cast<wstring*>(p_dirent).empty()</tt> 。
	\since build 474
	*/
	void* p_dirent;
#else
	/*!
	\brief 节点信息。
	\invariant <tt>!p_dirent || bool(GetNativeHandle())</tt> 。
	\since build 298
	*/
	::dirent* p_dirent;
#endif

public:
	/*!
	\brief 构造：使用目录路径。
	\note 路径要求同 DirectorySession 构造。
	\sa DirectorySession::DirectorySession
	\since build 541
	*/
	explicit YB_NONNULL(2)
	HDirectory(const char* path)
		: DirectorySession(path), p_dirent()
	{}
	//! \since build 543
	HDirectory(HDirectory&& h) ynothrow
		: DirectorySession(std::move(h)), p_dirent(h.p_dirent)
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
	\brief 判断文件系统节点无效或有效性。
	\since build 561
	*/
	DefBoolNeg(explicit, p_dirent)

	//! \since build 648
	DefCvt(const ynothrow, basic_string_view<NativeChar>, GetNativeName())
	//! \since build 593
	operator string() const;
	//! \since build 648
	operator u16string() const;

	/*!
	\brief 取节点状态信息确定的文件系统节点类别。
	\return 未迭代文件时为 NodeCategory::Empty ，否则为对应的其它节点类别。
	\note 不同系统支持的可能不同。
	\since build 474
	*/
	NodeCategory
	GetNodeCategory() const ynothrow;
	/*!
	\brief 间接操作：取节点名称。
	\return 非空结果：子节点不可用时为对应类型的 \c "." ，否则为子节点名称。
	\note 返回的结果在析构和下一次迭代前保持有效。
	\since build 648
	*/
	YB_ATTR(returns_nonnull) const NativeChar*
	GetNativeName() const ynothrow;

	//! \brief 复位。
	using DirectorySession::Rewind;
};

/*!
\relates HDirectory
\sa ystdex::is_undereferenceable
\since build 561
*/
inline PDefH(bool, is_undereferenceable, const HDirectory& i) ynothrow
	ImplRet(!i)


/*!
\brief 文件迭代器。
\since build 411
*/
using FileIterator = ystdex::indirect_input_iterator<HDirectory*>;


//! \since build 654
//@{
template<typename _tChar>
yconstfn bool
IsColon(_tChar c) ynothrow
{
	return c == _tChar(':');
}

template<typename _tChar>
yconstfn bool
IsSlash(_tChar c) ynothrow
{
	return c == _tChar('/');
}

template<typename _tChar>
inline YB_NONNULL(1) const _tChar*
FindColon(const _tChar* p) ynothrowv
{
	return ystdex::ntctschr(Nonnull(p), _tChar(':'));
}

/*!
\pre 间接断言：指针路径参数或路径参数的数据指针非空。
\todo 支持非 POSIX 文件路径特化。
*/
//@{
/*!
\brief 判断指定路径字符串是否表示一个绝对路径。
\note 空路径不是绝对路径。
*/
//@{
template<typename _tChar>
inline YB_NONNULL(2) bool
IsAbsolute_P(IDTag<YF_Platform_DS>, const _tChar* path) ynothrowv
{
	return IsSlash(Deref(path)) || FindColon(path);
}
template<typename _tChar>
inline bool
IsAbsolute_P(IDTag<YF_Platform_DS>, basic_string_view<_tChar> path) ynothrowv
{
	YAssertNonnull(path.data());
	return !path.empty() && (IsSlash(path[0])
		|| path.find(_tChar(':')) != basic_string_view<_tChar>::npos);
}
template<typename _tChar>
inline YB_NONNULL(2) bool
IsAbsolute_P(IDTag<YF_Platform_Win32>, const _tChar* path) ynothrowv
{
	return Deref(path) == YCL_PATH_DELIMITER
		|| (*path != _tChar() && IsColon(*++path));
}
template<typename _tChar>
inline bool
IsAbsolute_P(IDTag<YF_Platform_Win32>, basic_string_view<_tChar> path) ynothrowv
{
	YAssertNonnull(path.data());
	return path.length() > 1
		&& (path[0] == _tChar(YCL_PATH_DELIMITER) || IsColon(path[2]));
}
template<typename _tChar>
inline YB_NONNULL(2) bool
IsAbsolute_P(IDTagBase, const _tChar* path) ynothrowv
{
	return IsSlash(Deref(path));
}
template<typename _tChar>
inline bool
IsAbsolute_P(IDTagBase, basic_string_view<_tChar> path) ynothrowv
{
	YAssertNonnull(path.data());
	return !path.empty() && IsSlash(path[0]);
}

//! \since build 652
YCL_DefPlatformFwdTmpl(IsAbsolute, IsAbsolute_P)
//@}

/*!
\brief 取指定路径的文件系统根节点名称的长度。
\note 计入可能存在的紧随在根名称后的一个或多个文件分隔符。
*/
//@{
template<typename _tChar>
YB_NONNULL(2) size_t
FetchRootNameLength_P(IDTag<YF_Platform_DS>, const _tChar* path) ynothrowv
{
	auto p(FindColon(path));

	if(!p)
		p = path;
	while(*p == '/')
		++p;
	return size_t(p - path);
}
template<typename _tChar>
size_t
FetchRootNameLength_P(IDTag<YF_Platform_DS>, basic_string_view<_tChar> path)
	ynothrowv
{
	YAssertNonnull(path.data());

	auto n(path.find_first_not_of(_tChar(':')));

	if(n != basic_string_view<_tChar>::npos)
		n = path.find_first_not_of(_tChar('/'), n);
	return n != basic_string_view<_tChar>::npos ? n : path.length();
}
template<typename _tChar>
YB_NONNULL(2) size_t
FetchRootNameLength_P(IDTag<YF_Platform_Win32>, const _tChar* path) ynothrowv
{
	if(Deref(path) != _tChar() && IsColon(path[1]))
	{
		size_t n = 2;

		// TODO: Extract as %ystdex::tstr_find_first_not_of?
		while(path[n] != _tChar()
			&& !YCL_FS_CharIsDelimiter(path[n], _tChar))
			++n;
		return n;
	}
	return 0;
}
template<typename _tChar>
size_t
FetchRootNameLength_P(IDTag<YF_Platform_Win32>, basic_string_view<_tChar> path)
	ynothrowv
{
	YAssertNonnull(path.data());

	const auto l(path.length());

	if(l >= 2 && IsColon(path[1]))
	{
		// XXX: Use %YCL_FS_CharIsDelimiter?
		const auto
			n(path.find_first_not_of(&ystdex::to_array<_tChar>("/\\")[0], 2));

		return n != basic_string_view<_tChar>::npos ? n : 2;
	}
	return 0;
}
//! \since build 653
template<typename _tChar>
inline YB_NONNULL(2) size_t
FetchRootNameLength_P(IDTagBase, const _tChar* path) ynothrowv
{
	return Deref(path) == _tChar('/') ? 1 : 0;
}
template<typename _tChar>
inline size_t
FetchRootNameLength_P(IDTagBase, const basic_string_view<_tChar> path)
	ynothrowv
{
	YAssertNonnull(path.data());
	return !path.empty() && path[0] == _tChar('/') ? 1 : 0;
}

//! \since build 653
YCL_DefPlatformFwdTmpl(FetchRootNameLength, FetchRootNameLength_P)
//@}
//@}
//@}


/*!
\brief 已知文件系统类型。
\since build 607
*/
enum class FileSystemType
{
	Unknown,
	FAT12,
	FAT16,
	FAT32
};


/*!
\brief 文件分配表接口。
\since build 607
*/
namespace FAT
{

using EntryDataUnit = std::uint8_t;
using ClusterIndex = std::uint32_t;

//! \since build 608
//@{
//! \see Microsoft FAT Specification Section 3.1 。
//@{
//! \brief BIOS 参数块偏移量。
enum BPB : size_t
{
	BS_jmpBoot = 0,
	BS_OEMName = 3,
	BPB_BytsPerSec = 11,
	BPB_SecPerClus = 13,
	BPB_RsvdSecCnt = 14,
	BPB_NumFATs = 16,
	BPB_RootEntCnt = 17,
	BPB_TotSec16 = 19,
	BPB_Media = 21,
	BPB_FATSz16 = 22,
	BPB_SecPerTrk = 24,
	BPB_NumHeads = 26,
	BPB_HiddSec = 28,
	BPB_TotSec32 = 32
};

/*!
\brief BIOS 参数块偏移量域 BPB_BytsPerSec 的最值。
\since build 609
*/
yconstexpr const size_t MinSectorSize(512), MaxSectorSize(4096);
//@}

//! \brief FAT16 接口（ FAT12 共享实现）。
inline namespace FAT16
{

/*!
\brief FAT12 和 FAT16 扩展 BIOS 参数块偏移量。
\see Microsoft FAT Specification Section 3.2 。
*/
enum BPB : size_t
{
	BS_DrvNum = 36,
	BS_Reserved1 = 37,
	BS_BootSig = 38,
	BS_VolID = 39,
	BS_VolLab = 43,
	BS_FilSysType = 54,
	_reserved_zero_448 = 62,
	Signature_word = 510,
	_reserved_remained = 512
};

} // inline namespace FAT16;

//! \brief FAT32 接口。
inline namespace FAT32
{

/*!
\brief FAT32 扩展 BIOS 参数块偏移量。
\see Microsoft FAT Specification Section 3.3 。
*/
enum BPB : size_t
{
	BPB_FATSz32 = 36,
	BPB_ExtFlags = 40,
	BPB_FSVer = 42,
	BPB_RootClus = 44,
	BPB_FSInfo = 48,
	BPB_BkBootSec = 50,
	BPB_Reserved = 52,
	BS_DrvNum = 64,
	BS_Reserved1 = 65,
	BS_BootSig = 66,
	BS_VolID = 67,
	BS_VolLab = 71,
	BS_FilSysType = 82,
	_reserved_zero_420 = 90
};

} // inline namespace FAT32;

/*!
\brief 卷标数据类型。
\see Microsoft FAT Specification Section 3.2 。
\since build 610
*/
using VolumeLabel = array<byte, 11>;

//! \see Microsoft FAT Specification Section 5 。
//@{
//! \brief 文件系统信息块偏移量。
enum FSI : size_t
{
	FSI_LeadSig = 0,
	FSI_Reserved1 = 4,
	FSI_StrucSig = 484,
	FSI_Free_Count = 488,
	FSI_Nxt_Free = 492,
	FSI_Reserved2 = 496,
	FSI_TrailSig = 508
};

yconstexpr const std::uint32_t FSI_LeadSig_Value(0x41615252),
	FSI_StrucSig_Value(0x61417272), FSI_TrailSig_Value(0xAA550000);
//@}
//@}

/*!
\brief 文件属性。
\see Microsoft FAT specification Section 6 。
*/
enum class Attribute : EntryDataUnit
{
	ReadOnly = 0x01,
	Hidden = 0x02,
	System = 0x04,
	VolumeID = 0x08,
	Directory = 0x10,
	Archive = 0x20,
	LongFileName = ReadOnly | Hidden | System | VolumeID
};

//! \relates Attribute
DefBitmaskEnum(Attribute)


//! \brief 簇接口。
namespace Clusters
{

// !\see Microsoft FAT Specification Section 3.2 。
//@{
yconstexpr const size_t PerFAT12(4085);
yconstexpr const size_t PerFAT16(65525);
//@}

enum : ClusterIndex
{
	FAT16RootDirectory = 0,
	/*!
	\since build 609
	\see Microsoft FAT Specification Section 4 。
	*/
	//@{
	MaxValid12 = 0xFF6,
	MaxValid16 = 0xFFF6,
	MaxValid32 = 0xFFFFFF6,
	Bad12 = 0xFF7,
	Bad16 = 0xFFF7,
	Bad32 = 0xFFFFFF7,
	EndOfFile12 = 0xFFF,
	EndOfFile16 = 0xFFFF,
	EndOfFile32 = 0xFFFFFFFF,
	//@}
	EndOfFile = 0x0FFFFFFF,
	First = 0x00000002,
	Root = 0x00000000,
	Free = 0x00000000,
	Error = 0xFFFFFFFF
};

//! \since build 642
inline PDefH(bool, IsFreeOrEOF, ClusterIndex c) ynothrow
	ImplRet(c == Clusters::Free || c == Clusters::EndOfFile)

} // namespace Clusters;


//! \brief 非法别名字符。
const char IllegalAliasCharacters[]{"\\/:;*?\"<>|&+,=[] "};


//! \brief 文件大小：字节数。
using FileSize = std::uint32_t;

/*!
\brief 最大文件大小。
\note 等于 4GiB - 1B 。
*/
static yconstexpr auto MaxFileSize(FileSize(0xFFFFFFFF));


//! \brief 时间戳：表示日期和时间的整数类型。
using Timestamp = std::uint16_t;

//! \brief 转换日期和时间的时间戳为标准库时间类型。
YF_API std::time_t
ConvertFileTime(Timestamp, Timestamp) ynothrow;

//! \brief 取以实时时钟的文件日期和时间。
YF_API pair<Timestamp, Timestamp>
FetchDateTime() ynothrow;


/*!
\brief 长文件名接口。
\note 仅适用于 ASCII 兼容字符集。
\see Microsoft FAT specification Section 7 。
*/
namespace LFN
{

//! \brief 长文件名目录项偏移量。
enum Offsets
{
	//! \brief 长文件名序数。
	Ordinal = 0x00,
	Char0 = 0x01,
	Char1 = 0x03,
	Char2 = 0x05,
	Char3 = 0x07,
	Char4 = 0x09,
	//! \note 值等于 Attribute::LongFilename 。
	Flag = 0x0B,
	//! \note 保留值 0x00 。
	Reserved1 = 0x0C,
	//! \brief 短文件名（别名）校验和。
	CheckSum = 0x0D,
	Char5 = 0x0E,
	Char6 = 0x10,
	Char7 = 0x12,
	Char8 = 0x14,
	Char9 = 0x16,
	Char10 = 0x18,
	//! \note 保留值 0x0000 。
	Reserved2 = 0x1A,
	Char11 = 0x1C,
	Char12 = 0x1E
};

//! \brief 组成长文件名中的 UCS-2 字符在项中的偏移量表。
yconstexpr const size_t OffsetTable[]{0x01, 0x03, 0x05, 0x07, 0x09, 0x0E,
	0x10, 0x12, 0x14, 0x16, 0x18, 0x1C, 0x1E};

enum : size_t
{
	//! \brief UCS-2 项最大长度。
	MaxLength = 256,
	//! \brief UTF-8 项最大长度。
	MaxMBCSLength = MaxLength * 3,
	EntryLength = arrlen(OffsetTable),
	AliasEntryLength = 11,
	MaxAliasMainPartLength = 8,
	MaxAliasExtensionLength = 3,
	MaxAliasLength = MaxAliasMainPartLength + MaxAliasExtensionLength + 2,
	MaxNumericTail = 999999
};

enum EntryValues : EntryDataUnit
{
	//! \brief WinNT 小写文件名。
	CaseLowerBasename = 0x08,
	//! \brief WinNT 小写扩展名。
	CaseLowerExtension = 0x10,
	//! \brief Ordinal 中标记结束的项。
	LastLongEntry = 0x40
};

//! \brief 非法长文件名字符。
const char IllegalCharacters[]{"\\/:*?\"<>|"};

//! \since build 608
//@{
/*!
\brief 转换长文件名为别名。
\pre 参数指定的字符串不含空字符。
\return 主文件名、扩展名和指定是否为有损转换标识。
\note 返回的文件名长度分别不大于 MaxAliasMainPartLength
	和 MaxAliasExtensionLength 。
*/
YF_API tuple<string, string, bool>
ConvertToAlias(const u16string&);

//! \brief 按指定序数取长文件名偏移。
inline PDefH(size_t, FetchLongNameOffset, EntryDataUnit ord) ynothrow
	ImplRet((size_t(ord & ~LastLongEntry) - 1U) * EntryLength)
//@}

/*!
\brief 转换 UCS-2 路径字符串为多字节字符串。
\since build 610
*/
YF_API string
ConvertToMBCS(const char16_t* path);

/*!
\brief 生成别名校验和。
\pre 断言：参数非空。
\see Microsoft FAT specification Section 7.2 。
*/
YF_API EntryDataUnit
GenerateAliasChecksum(const EntryDataUnit*) ynothrowv;

/*!
\brief 在指定字符串写 '~' 和整数的后缀以作为短文件名。
\pre 字符串长度不小于 MaxAliasMainPartLength 。
\since build 608
*/
YF_API void
WriteNumericTail(string&, size_t) ynothrowv;

} // namespace LFN;

//! \since build 609
//@{
//! \brief 目录项数据大小。
yconstexpr const size_t EntryDataSize(0x20);

/*!
\brief 目录项数据。
\note 默认构造不初始化。
*/
class YF_API EntryData final : private array<EntryDataUnit, EntryDataSize>
{
public:
	using Base = array<EntryDataUnit, EntryDataSize>;
	/*!
	\brief 目录项偏移量。
	\see Microsoft FAT specification Section 6 。
	*/
	enum Offsets : size_t
	{
		Name = 0x00,
		Extension = 0x08,
		Attributes = 0x0B,
		//! \note 项 DIR_NTRes 保留，指定值为 0 但扩展为表示大小写。
		CaseInfo = 0x0C,
		CTimeTenth = 0x0D,
		CTime = 0x0E,
		CDate = 0x10,
		ADate = 0x12,
		ClusterHigh = 0x14,
		MTime = 0x16,
		MDate = 0x18,
		Cluster = 0x1A,
		FileSize = 0x1C
	};
	//! \see Microsoft FAT specification Section 6.1 。
	enum : EntryDataUnit
	{
		Last = 0x00,
		Free = 0xE5,
	};

	using Base::operator[];
	using Base::data;

	DefPred(const ynothrow, Directory,
		bool(Attribute((*this)[Attributes]) & Attribute::Directory))
	DefPred(const ynothrow, LongFileName,
		Attribute((*this)[Attributes]) == Attribute::LongFileName)
	DefPred(const ynothrow, Volume, bool(Attribute((*this)[Attributes])
		& Attribute::VolumeID))
	DefPred(const ynothrow, Writable,
		!bool(Attribute((*this)[Attributes]) & Attribute::ReadOnly))

	PDefH(void, SetDirectoryAttribute, ) ynothrow
		ImplExpr((*this)[Attributes] = EntryDataUnit(Attribute::Directory))
	PDefH(void, SetDot, size_t n) ynothrowv
		ImplExpr(YAssert(n < EntryDataSize, "Invalid argument found."),
			(*this)[n] = '.')

	PDefH(void, Clear, ) ynothrow
		ImplExpr(ystdex::trivially_fill_n(static_cast<Base*>(this)))

	PDefH(void, ClearAlias, ) ynothrow
		ImplExpr(ystdex::trivially_fill_n(data(), LFN::AliasEntryLength, ' '))

	/*!
	\brief 复制长文件名列表项数据到参数指定的缓冲区的对应位置。
	\pre 断言：参数非空。
	*/
	void
	CopyLFN(char16_t*) const ynothrowv;

	PDefH(void, FillLast, ) ynothrow
		ImplExpr(ystdex::trivially_fill_n(static_cast<Base*>(this), 1, Last))

	bool
	FindAlias(const char*, size_t) const;

	string
	GenerateAlias() const;

	PDefH(std::uint32_t, ReadFileSize, ) ynothrow
		ImplRet(ystdex::read_uint_le<32>(data() + FileSize))

	void
	SetupRoot(ClusterIndex) ynothrow;

	void
	WriteAlias(const string&) ynothrow;

	void
	WriteCDateTime() ynothrow;

	void
	WriteCluster(ClusterIndex) ynothrow;

	void
	WriteDateTime() ynothrow;
};
//@}


/*!
\brief 检查参数指定的 MS-DOS 风格路径冒号。
\pre 间接断言：参数非空。
\exception std::system_error 检查失败。
	\li std::errc::invalid_argument 路径有超过一个冒号。
\since build 611
*/
YF_API YB_NONNULL(1) YB_PURE const char*
CheckColons(const char*);

} // namespace FAT;

} // namespace platform;

namespace platform_ex
{

} // namespace platform_ex;

#endif

