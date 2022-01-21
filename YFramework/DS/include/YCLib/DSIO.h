/*
	© 2011-2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file DSIO.h
\ingroup YCLib
\ingroup DS
\brief DS 底层输入输出接口。
\version r1611
\author FrankHB <frankhb1989@gmail.com>
\since build 604
\par 创建时间:
	2015-06-06 03:01:27 +0800
\par 修改时间:
	2021-12-29 01:18 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(DS)::DSIO
*/


#ifndef YCL_DS_INC_DSIO_h_
#define YCL_DS_INC_DSIO_h_ 1

#include "YCLib/YModules.h"
#include YFM_YCLib_YCommon // for size_t;
#if YCL_DS
#	include YFM_YCLib_Reference // for lref;
#	include <nds/disc_io.h> // for ::DISC_INTERFACE, ::sec_t;
#	include <ystdex/base.h> // for ystdex::noncopyable, ystdex::nonmomable;
#	include <ystdex/cache.hpp> // for ystdex::used_list_cache;
#	include <ystdex/cstdio.h> //for ystdex::block_buffer;
#	include YFM_YCLib_FileSystem // for platform::FAT, platform::Deref,
//	platform::Concurrency, platform::FileSystemType, std::system_error,
//	string_view, platform::FetchSeparator, array, string;
#	include <sys/syslimits.h> // for NAME_MAX.
#	include <ystdex/function.hpp> // for ystdex::function;
#	include <ystdex/optional.h> // for ystdex::ref_opt;
#	include <ystdex/path.hpp> // for ystdex::is_parent_or_self;
#	include <bitset> // for std::bitset;
#endif

//! \since build 680
struct stat;
//! \since build 680
struct statvfs;

namespace platform_ex
{

#if YCL_DS
//! \since build 604
//@{
/*!
\brief DS 底层外部存储接口封装。
\see LibNDS 头文件 <nds/disc_io.h> 。
\see LibFAT 源码文件 "source/disc.h" 。
*/
class YF_API Disc : private lref<const ::DISC_INTERFACE>
{
public:
	Disc(const ::DISC_INTERFACE& disc) ynothrow
		: lref(disc)
	{}

	DefPred(const ynothrow, Inserted, get().isInserted)
	DefGetter(const ynothrow, std::uint32_t, Type, get().ioType)
	DefGetter(const ynothrow, std::uint32_t, Features, get().features)

	DefFwdTmpl(const ynothrow, bool, ReadSectors,
		get().readSectors(yforward(args)...))

	PDefH(bool, Shutdown, ) const ynothrow
		ImplRet(get().shutdown())

	PDefH(bool, Starup, ) const ynothrow
		ImplRet(get().startup())

	DefFwdTmpl(const ynothrow, bool, WriteSectors,
		get().writeSectors(yforward(args)...))

	using lref::get;
};


//! \brief 扇区缓冲。
class YF_API SectorCache final : private ystdex::noncopyable,
	private ystdex::nonmovable
{
private:
	using UsedListCache
		= ystdex::used_list_cache<::sec_t, ystdex::block_buffer>;
	Disc disc;
	size_t number_of_pages;
	size_t sectors_per_page_shift;
	::sec_t end_of_partition;
	size_t bytes_per_sector;
	UsedListCache entries;

public:
	SectorCache(size_t, size_t, Disc, ::sec_t, size_t);
	~SectorCache();

private:
	//! \since build 656
	size_t
	GetBlockCount(::sec_t) const ynothrowv;

public:
	//! \since build 609
	DefGetter(const ynothrow, Disc, Disc, disc)

private:
	PDefH(size_t, GetKey, ::sec_t sec) const ynothrowv
		ImplRet(sec & ~((1 << sectors_per_page_shift) - 1))
	//! \since build 656
	ystdex::block_buffer*
	GetPage(::sec_t) ynothrowv;

public:
	//! \since build 608
	YB_NONNULL(4) bool
	EraseWritePartialSector(::sec_t, size_t, const void*, size_t = 1) ynothrowv;

	//! \since build 608
	bool
	FillPartialSector(::sec_t, size_t, size_t = 1, byte = {}) ynothrowv;

	//! \since build 606
	bool
	FillSectors(::sec_t, size_t = 1, byte = {}) ynothrow;

	bool
	Flush() ynothrow;

private:
	bool
	FlushEntry(UsedListCache::value_type&) ynothrow;

	//! \since build 656
	//@{
	template<typename _func>
	bool
	PerformPartialSectorIO(_func f, ::sec_t sec, size_t offset, size_t n)
		ynoexcept_spec(f(std::declval<ystdex::block_buffer&>(),
		(sec - size_t(0)) * bytes_per_sector))
	{
		if(!(bytes_per_sector < offset + n))
		{
			const auto key(GetKey(sec));

			if(const auto p_entry = GetPage(key))
			{
				f(*p_entry, (sec - key) * bytes_per_sector);
				return true;
			}
		}
		return {};
	}

	template<typename _func>
	bool
	PerformSectorsIO(_func f, ::sec_t& sec, size_t& n) ynoexcept_spec(
		f(std::declval<ystdex::block_buffer&>(), sec - size_t(0), size_t(0)))
	{
		while(0 < n)
		{
			const auto key(GetKey(sec));

			if(const auto p_entry = GetPage(key))
			{
				const auto sec_off(sec - key);
				// TODO: Compare and assert 'count' and 'sec_off'?
				const auto secs_to_process(
					std::min<size_t>(GetBlockCount(key) - sec_off, n));

				f(*p_entry, sec_off, secs_to_process);
				yunseq(sec += secs_to_process,  n -= secs_to_process);
			}
			else
				return {};
		}
		return true;
	}

public:
	YB_NONNULL(2) bool
	ReadPartialSector(void*, ::sec_t, size_t, size_t = 1) ynothrowv;

	YB_NONNULL(2) bool
	ReadSectors(void*, ::sec_t, size_t = 1) ynothrowv;

	YB_NONNULL(4) bool
	WritePartialSector(::sec_t, size_t, const void*, size_t = 1) ynothrowv;

	YB_NONNULL(3) bool
	WriteSectors(::sec_t, const void*, size_t = 1) ynothrow;
	//@}
};
//@}

/*!
\brief 基于文件分配表的文件系统扩展接口。
\since build 610
*/
namespace FAT
{

//! \see Microsoft FAT Specification Section 2 。
static_assert(std::is_same<byte, ystdex::octet>::value,
	"Only 8-bit byte is supported.");

static_assert(!(sizeof(size_t) < sizeof(std::uint32_t)),
	"Invalid type found.");

using namespace platform::FAT;
using platform::Deref;
using namespace platform::Concurrency;
using platform::FileSystemType;
//! \since build 611
using namespace platform::Threading;

// XXX: Mutex lock probable may throw on platforms other than DS. However,
//	ISO C++ [thread.req.exception] guaranteed only specified error conditions
//	are allowed, and all of them in [thread.mutex.requirements.mutex] are
//	carefully avoided. Thus the exception specifications are not affected even
//	if each creation of lock is not in a try-block.


/*!
\brief 分配表。
\warning 不具有事务语义。无异常抛出的操作总是假定读写成功。介质损坏可能引起数据丢失。
*/
class YF_API AllocationTable final
{
private:
	mutable mutex part_mutex{};
	size_t sectors_per_cluster;
	size_t bytes_per_sector;
	size_t bytes_per_cluster;
	::sec_t table_start;
	size_t table_size;
	size_t total_sectors_num;
	size_t limit;
	size_t root_dir_start;
	size_t root_dir_sectors_num;
	size_t data_sectors_num;
	//! \invariant \c IsValid(first_free) 。
	ClusterIndex first_free = Clusters::First;
	//! \since build 900
	std::uint32_t free_count = 0;
	//! \invariant \c IsValid(last_alloc_cluster) 。
	ClusterIndex last_alloc_cluster = 0;
	//! \invariant <tt>Clusters::First <= last_cluster</tt> 。
	ClusterIndex last_cluster;
	FileSystemType fs_type;
	ClusterIndex root_dir_cluster;

public:
	mutable SectorCache Cache;

	YB_NONNULL(3)
	AllocationTable(::sec_t, const byte*, size_t, size_t, Disc) ynothrowv;

	DefDeCopyAssignment(AllocationTable)

	//! \since build 657
	YB_ATTR_nodiscard YB_PURE PDefH(bool, IsFreeOrValid, ClusterIndex c) const
		ynothrow
		ImplRet(c == Clusters::Free || IsValid(c))
	//! \since build 657
	PDefH(bool, IsValid, ClusterIndex c) const ynothrow
		// TODO: This will catch Clusters::Error?
		ImplRet(Clusters::First <= c && c <= last_cluster)

	DefGetter(const ynothrow, size_t, BytesPerCluster, bytes_per_cluster)
	DefGetter(const ynothrow, size_t, BytesPerSector, bytes_per_sector)
	DefGetter(const ynothrow, FileSystemType, FileSystemType, fs_type)
	//! \since build 900
	DefGetter(const ynothrow, ClusterCount, FreeClusters,
		fs_type == FileSystemType::FAT32 ? free_count : CountFreeCluster())
	DefGetter(const ynothrow, mutex&, MutexRef, part_mutex)
	DefGetter(const ynothrow, ClusterIndex, RootDirCluster, root_dir_cluster)
	DefGetter(const ynothrow, ClusterIndex, RootDirSectorsNum,
		root_dir_sectors_num)
	DefGetter(const ynothrow, size_t, SectorsPerCluster, sectors_per_cluster)
	DefGetter(const ynothrow, std::uint64_t, TotalSize, std::uint64_t(
		data_sectors_num) * std::uint64_t(bytes_per_sector))
	DefGetter(const ynothrow, size_t, UsedClusters,
		last_cluster - Clusters::First + 1)

	PDefH(::sec_t, ClusterToSector, ClusterIndex c) const ynothrow
		ImplRet((c >= Clusters::First ? (c - Clusters::First) * ::sec_t(
			sectors_per_cluster) + root_dir_sectors_num : 0) + root_dir_start)

	/*!
	\brief 若指定簇有效，清除之后链接的所有簇。
	\return 参数指定的簇是否有效。
	\sa IsValid
	*/
	bool
	ClearLinks(ClusterIndex) ynothrow;

	//! \since build 900
	YB_ATTR_nodiscard ClusterCount
	CountFreeCluster() const ynothrow;

	/*!
	\return 成功分配的簇。
	\since build 713
	*/
	//@{
	/*!
	\brief 分配空闲的簇。
	\exception std::system_error 调用失败。
		\li std::errc::no_space_on_device 空间不足。
	*/
	ClusterIndex
	LinkFree(ClusterIndex) ythrow(std::system_error);

	/*!
	\brief 分配已填充零的空间。
	\exception std::system_error 调用失败。
		\li std::errc::no_space_on_device 空间不足。
		\li std::errc::io_error 写错误。
	\sa LinkFree
	*/
	YB_ATTR_nodiscard ClusterIndex
	LinkFreeCleared(ClusterIndex) ythrow(std::system_error);
	//@}

	/*!
	\brief 查询参数指定的簇的下一簇。
	\pre 断言：<tt>IsFreeOrValid(c)</tt> 。
	\return 若指定空闲簇则为空闲簇；若文件系统类型非法则为错误簇；
		否则为读取的下一簇。
	\since build 657
	*/
	YB_ATTR_nodiscard ClusterIndex
	QueryNext(ClusterIndex c) const ynothrowv;

	YB_ATTR_nodiscard ClusterIndex
	QueryLast(ClusterIndex) const ynothrow;

	/*!
	\brief 同步：读取并按需更新簇信息。
	\return 参数指定的缓冲区被更新。
	\since build 900
	*/
	YB_ATTR_nodiscard YB_NONNULL(2) bool
	SynchronizeClusters(byte*) ynothrowv;

	/*!
	\brief 修剪存储的簇链：跳过指定保留的簇，移除之后链接的所有簇。
	\exception std::system_error 调用失败。
		\li std::errc::io_error 下一簇有效性校验失败。
	\note 第二参数指定保留的簇的数量上限减 1 。
	\sa ClearLinks
	\sa QueryNext

	以 ClearLinks 调用清除连续的簇。
	若保留簇，标记参数指定的为文件结束。
	*/
	YB_ATTR_nodiscard ClusterIndex
	TrimChain(ClusterIndex, size_t) ythrow(std::system_error);

	//! \since build 900
	YB_NONNULL(2) void
	UpdateFSInfo(byte*) ynothrowv;

	YB_NONNULL(2) void
	WriteClusters(byte*) const ynothrowv;

	bool
	WriteEntry(ClusterIndex, std::uint32_t) const ynothrow;
};


//! \since build 611
//@{
static_assert(LFN::MaxMBCSLength <= NAME_MAX, "Invalid constant found.");


//! \brief 文件读写位置。
class YF_API FilePosition final
{
private:
	ClusterIndex cluster;
	::sec_t sector;
	size_t byte;

public:
	DefDeCtor(FilePosition)
	FilePosition(ClusterIndex c)
		: cluster(c), sector(0), byte(0)
	{}
	FilePosition(ClusterIndex c, ::sec_t sec, size_t b)
		: cluster(c), sector(sec), byte(b)
	{}

	DefDeCopyAssignment(FilePosition)

	DefGetter(const ynothrow, size_t, Byte, byte)
	//! \since build 900
	DefGetter(const ynothrow, ClusterIndex, Cluster, cluster)
	DefGetter(const ynothrow, ::sec_t, Sector, sector)

	DefSetter(ynothrow, size_t, Byte, byte)
	//! \since build 900
	DefSetter(ynothrow, ClusterIndex, Cluster, cluster)
	DefSetter(ynothrow, ::sec_t, Sector, sector)

	PDefH(void, AddByte, size_t x) ynothrow
		ImplExpr(byte += x)

	PDefH(void, AddSector, size_t x) ynothrow
		ImplExpr(sector += x)

	void
	Decompose(std::uint32_t, size_t, size_t) ynothrowv;

	PDefH(void, IncSector, ) ynothrow
		ImplUnseq(++sector)

	PDefH(void, IncSectorAndResetOffset, ) ynothrow
		ImplUnseq(byte = 0, ++sector)
};


//! \brief 目录项位置。
class YF_API DEntryPosition final
{
private:
	ClusterIndex cluster;
	::sec_t sector;
	size_t offset;

public:
	DefDeCtor(DEntryPosition)
	DEntryPosition(ClusterIndex c)
		: cluster(c), sector(0), offset(0)
	{}
	DEntryPosition(ClusterIndex c, ::sec_t sec, size_t o)
		: cluster(c), sector(sec), offset(o)
	{}

	DefDeCopyAssignment(DEntryPosition)

	YB_ATTR_nodiscard YB_PURE PDefH(bool, IsFAT16RootCluster) const ynothrow
		ImplRet(cluster == Clusters::FAT16RootDirectory)

	DefGetter(const ynothrow, size_t, Byte, offset * EntryDataSize)
	//! \since build 900
	DefGetter(const ynothrow, ClusterIndex, Cluster, cluster)
	DefGetter(const ynothrow, ::sec_t, Sector, sector)

	//! \since build 900
	DefSetter(ynothrow, ClusterIndex, Cluster, cluster)
	DefSetter(ynothrow, ::sec_t, Sector, sector)

	PDefH(size_t, IncOffset, ) ynothrow
		ImplRet(++offset)

	PDefH(void, IncSectorAndResetOffset, ) ynothrow
		ImplUnseq(offset = 0, ++sector)

	YB_ATTR_nodiscard YB_PURE friend PDefHOp(bool, ==, const DEntryPosition& x,
		const DEntryPosition& y) ynothrow
		ImplRet(x.cluster == y.cluster && x.sector == y.sector
			&& x.offset == y.offset)
};


/*!
\brief 路径解析找到最终项后的动作。
\since build 655
*/
enum class LeafAction
{
	//! \brief 找到最终项后直接返回。
	Return = 0,
	/*!
	\brief 若找到最终项，
		抛出 std::errc::file_exists 的 std::system_error 。
	*/
	ThrowExisted,
	/*!
	\brief 进一步查询簇，若发现最终项非目录项，
		则抛出 std::errc::not_a_directory 的 std::system_error 。
	*/
	EnsureDirectory
};

/*!
\brief 扩展目录项位置的结果。
\sa Partition::IncrementPosition
\since build 656
*/
enum class ExtensionResult
{
	Success,
	EndOfFile,
	NoSpace
};


/*!
\brief 判断简化的绝对路径。
\pre 参数的数据指针非空。
\note 只检查路径非空和首个字符是分隔符。
\since 900
*/
YB_ATTR_nodiscard YB_PURE inline
	PDefH(bool, IsSimpleAbsolute, string_view sv) ynothrow
	ImplRet(YAssertNonnull(sv.data()),
		!sv.empty() && sv.front() == platform::FetchSeparator<char>())


class Partition;

//! \brief 目录项。
class DEntry final
{
public:
	/*!
	\brief 名称项位置类型：指定文件名所在目录项的位置。

	第一元素：长文件名存在时指向长文件名项起始位置，否则指向别名项。
	第二元素：总是指向短文件名（别名）项。
	*/
	using NamePosition = array<DEntryPosition, 2>;

	//! \brief 目录项数据。
	EntryData Data;
	/*!
	\brief 名称项位置。
	\invariant 对确定的文件分配表，分量的簇满足 AllocationTable::IsFreeOrValid 。
	\warning 不检查不变量。
	*/
	NamePosition NamePos;

private:
	//! \brief 名称。
	string name;

public:
	/*!
	\brief 构造：使用先于第一个项的初始名称位置。
	\note 调用 QueryNextFrom 成功后有效。
	*/
	DEntry(ClusterIndex c)
		: NamePos(GenerateBeforeFirstNamePos(c))
	{}
	/*!
	\brief 构造：分区根目录项。
	\exception std::system_error 调用失败。
		\li std::errc::no_space_on_device 空间不足。
		\li std::errc::io_error 写错误。
	\sa Partition::ExtendPosition
	*/
	DEntry(const Partition&);
	/*!
	\brief 构造：使用分区上的指定名称位置。
	\exception std::system_error 调用失败。
		\li std::errc::io_error 读错误。
	*/
	DEntry(Partition&, const NamePosition&);
	//! \since build 900
	//@{
	/*!
	\pre 断言：路径参数的数据指针非空。
	\exception std::system_error 调用失败。
		\li std::errc::file_exists 指定最终项或添加项时项已存在。
		\li std::errc::filename_too_long 路径太长。
		\li std::errc::io_error 读取的项指定错误的簇。
		\li std::errc::no_such_file_or_directory
			路径前缀的项或添加时指定的最终项不存在。
	\note 路径相对分区，无根前缀，空串路径视为当前工作目录。
	\note 最后一个参数保存成功遍历的父目录簇（不包括根目录簇）。
	*/
	//@{
	/*!
	\brief 构造：使用分区上的指定路径和叶节点动作。
	\exception std::system_error 调用失败。
		\li std::errc::not_a_directory 非叶节点非目录项；
			或动作指定为 LeafAction::EnsureDirectory 时，叶节点非目录项。
	\note 动作指定为 LeafAction::EnsureDirectory 时最后一个参数保存根目录簇。
	*/
	DEntry(Partition&, string_view, LeafAction = LeafAction::Return,
		ClusterIndex& = ystdex::ref_opt<ClusterIndex>());
	/*!
	\brief 构造：使用分区上的指定路径和叶节点动作，必要时通过回调函数添加项。
	\pre <tt>act != LeafAction::EnsureDirectory</tt> 。
	\exception std::system_error 调用失败。
		\li std::errc::file_exists 指定最终项或添加项时项已存在。
		\li std::errc::invalid_argument
			添加项时，去除右端空格的名称为空，
			或含有 LFN::IllegalCharacters 中的字符，
			或非法导致生成后缀失败；
			或项已存在且指定分区根目录。
		\li std::errc::filename_too_long 路径太长。
		\li std::errc::io_error 添加项时读写错误或读取的项指定错误的簇。
		\li std::errc::no_space_on_device 添加项时空间不足。
		\li std::errc::no_such_file_or_directory
			路径前缀的项或添加时指定的最终项不存在。
		\li std::errc::not_a_directory 非叶节点非目录项。
	\note 若添加项，长短文件名由 FindEntryGap 调用设置。
	*/
	DEntry(Partition&, string_view, LeafAction, ystdex::function<void(DEntry&)>,
		ClusterIndex& = ystdex::ref_opt<ClusterIndex>());
	//@}
	DefDeMoveCtor(DEntry)

	DefDeMoveAssignment(DEntry)
	//@}

	DefPred(const ynothrow, Dot, ystdex::is_parent_or_self(name))
	/*!
	\brief 判断分区类型是 FAT32 且 Data 表示根目录。
	\exception std::system_error 调用失败。
		\li std::errc::io_error 读取的项指定错误的簇。
	\since build 900
	*/
	YB_ATTR_nodiscard bool
	IsFAT32Root(const Partition&) const;

	DefGetter(const ynothrow, const string&, Name, name)
	DefGetter(ynothrow, string&, NameRef, name)

private:
	//! \since build 900
	//@{
	YB_ATTR_nodiscard bool
	ConsBranch(Partition&, string_view&, size_t, ClusterIndex&);

	void
	ConsCheckDirectory(const Partition&, ClusterIndex&);

	void
	ConsLeafNoCallback(Partition&, string_view, LeafAction, ClusterIndex&);

	//! \pre <tt>act != LeafAction::EnsureDirectory</tt> 。
	void
	ConsLeafWithCallback(Partition&, string_view, LeafAction, ClusterIndex&);

	PDefH(void, ConsReset, const Partition& part)
		ImplExpr(this->~DEntry(), ::new(this) DEntry(part))
	//@}

public:
	/*!
	\brief 查找指定簇后的空闲空间并分配位置。
	\pre 参数指定的分区和之前所有成员函数调用一致。
	\exception std::system_error 调用失败。
		\li std::errc::no_space_on_device 空间不足。
		\li std::errc::io_error 读写错误。
	\sa ExtendPosition
	*/
	void
	FindEntryGap(Partition&, ClusterIndex, size_t) ythrow(std::system_error);

private:
	static YB_STATELESS PDefH(DEntryPosition, GenerateBeforeFirstDEntryPos,
		ClusterIndex c) ynothrow
		ImplRet({c, 0, size_t(-1)})

public:
	//! \brief 生成先于第一个项的初始名称位置。
	static YB_STATELESS PDefH(NamePosition, GenerateBeforeFirstNamePos,
		ClusterIndex c) ynothrow
		ImplRet({GenerateBeforeFirstDEntryPos(c),
			GenerateBeforeFirstDEntryPos(c)})

	/*!
	\brief 查询分区内下一个项，根据名称项位置更新名称。
	\pre 参数指定的分区和之前所有成员函数调用一致。
	\exception std::system_error 查询失败。
		\li std::errc::io_error 读错误。
	*/
	bool
	QueryNextFrom(Partition&) ythrow(system_error);
};


class FileInfo;

/*!
\brief FAT 分区。
\note 成员函数参数路径默认相对此分区，无根前缀，空串路径视为当前工作目录。
\warning 除非另行指定，不保证并发读写安全。
\todo 添加修改卷标的接口。
*/
class Partition final : private ystdex::noncopyable, private ystdex::nonmovable
{
public:
	using OpenFilesSet = set<lref<FileInfo>, ystdex::get_less<>>;

private:
	Disc disc;
	//! \brief 卷标。
	VolumeLabel label;

public:
	AllocationTable Table;
	/*!
	\brief 从项数据中读取簇索引并校验有效性。
	\exception std::system_error 调用失败。
		\li std::errc::io_error 校验失败：读取的项指定错误的簇。
	\since build 900
	*/
	lref<ClusterIndex(const AllocationTable&, const EntryData&)
		ythrow(std::system_error)> ReadClusterFromEntry;

private:
	::sec_t fs_info_sector;
	bool read_only;
	ClusterIndex cwd_cluster;
	OpenFilesSet open_files{};

	/*!
	\note 起始扇区指定分区位置，为 0 且校验失败时尝试搜索 FAT 位置。
	\note 和 LibFAT 不同，校验扇区的读取也是全缓冲的。
	\excetpion std::runtime_error 嵌套异常：初始化或校验失败。
	*/
	//@{
	/*!
	\brief 构造：使用临时缓冲区、底层存储接口、分页数、每页扇区数的二进制位数
		和起始扇区。
	\pre 临时缓冲区指针非空且指向至少能存储 MaxSectorSize 的空间。
	\note 锁定读写。
	\sa MaxSectorSize
	*/
	YB_NONNULL(2)
	Partition(byte*, Disc, size_t, size_t, ::sec_t);

public:
	/*!
	\brief 构造：使用底层存储接口、分页数、每页扇区数的二进制位数和起始扇区。
	\note 分配 MaxSectorSize 字节的临时缓冲区用于构造时读写分区。
	\note 锁定读写。
	*/
	Partition(Disc, size_t, size_t, ::sec_t);
	//@}
	//! \brief 析构：刷新并关闭打开的文件。
	~Partition();

	DefPred(const ynothrow, ReadOnly, read_only)

	DefGetterMem(const ynothrow, size_t, BytesPerCluster, Table)
	DefGetterMem(const ynothrow, size_t, BytesPerSector, Table)
	DefGetter(const ynothrow, ClusterIndex, CWDCluster, cwd_cluster)
	DefGetter(const ynothrow, SectorCache&, CacheRef, Table.Cache)
	DefGetterMem(const ynothrow, FileSystemType, FileSystemType, Table)
	DefGetter(const ynothrow, const VolumeLabel&, Label, label)
	//! \since build 642
	DefGetter(const ynothrow, basic_string_view<byte>, LabelString,
		{label.data(), label.size()})
	DefGetterMem(const ynothrow, mutex&, MutexRef, Table)
	DefGetterMem(const ynothrow, ClusterIndex, RootDirCluster, Table)
	DefGetterMem(const ynothrow, size_t, SectorsPerCluster, Table)
	DefGetterMem(const ynothrow, size_t, TotalSize, Table)

	/*!
	\brief 切换当前工作目录至参数指定路径的目录。
	\pre 间接断言：路径参数的数据指针非空。
	\exception std::system_error 调用失败。
		\li std::errc::not_a_directory 指定的路径不是目录。
	\since build 643
	*/
	void
	ChangeDir(string_view) ythrow(std::system_error);

	/*!
	\brief 创建文件系统信息。
	\pre 断言：文件系统类型为 FAT32 。
	\see Microsoft FAT Specification Section 5 。
	*/
	void
	CreateFSInfo();

private:
	/*!
	\brief 查找符合校验条件的第一个分区。
	\note 和 LibFAT 不同，读错误抛出异常而不是返回 0 。
	\exception std::system_error 调用失败。
		\li std::errc::io_error 查询项时读错误。
	\return 找到的分区扇区，或 0 。
	\since build 801
	*/
	YB_ATTR_nodiscard YB_NONNULL(1) ::sec_t
	FindFirstValidPartition(byte*) const ythrow(std::system_error);

public:
	/*!
	\brief 检查并设置文件位置为下一簇。
	\exception std::system_error 调用失败。
		\li std::errc::invalid_argument 文件位置的扇区数非法。
		\li std::errc::no_space_on_device 空间不足。
	*/
	void
	CheckPositionForNextCluster(FilePosition&) ythrow(std::system_error);

	/*!
	\brief 检查指定名称和簇的项是否存在。
	\exception std::system_error 调用失败。
		\li std::errc::io_error 查询项时读错误。
	\since build 642
	*/
	YB_ATTR_nodiscard bool
	EntryExists(string_view, ClusterIndex) ythrow(system_error);

	/*!
	\brief 移动目录项位置至下一个项，当遇到文件结束时扩展。
	\pre 间接断言：<tt>Table.IsFreeOrValid(pos.GetCluster())</tt> 。
	\exception std::system_error 调用失败。
		\li std::errc::no_space_on_device 空间不足。
		\li std::errc::io_error 写错误。
	\sa AllocationTable::LinkFreeCleared
	\sa IncremntPosition
	*/
	void
	ExtendPosition(DEntryPosition&) ythrow(std::system_error);

	/*!
	\brief 刷新。
	\exception std::system_error 调用失败。
		\li std::errc::io_error 写错误。
	*/
	PDefH(void, Flush, ) ythrow(std::system_error)
		ImplExpr(GetCacheRef().Flush() ? void()
			: ystdex::throw_error(std::errc::io_error))

	/*!
	\brief 移动目录项位置至下一个项。
	\pre 断言：<tt>Table.IsFreeOrValid(pos.GetCluster())</tt> 。
	\invariant <tt>Table.IsFreeOrValid(pos.GetCluster())<tt> 。
	\note 读到的无效簇视为文件结束标记。
	\since build 657
	*/
	YB_ATTR_nodiscard ExtensionResult
	IncrementPosition(DEntryPosition& pos) ynothrowv;

	YB_ATTR_nodiscard PDefH(locked_ptr<OpenFilesSet>, LockOpenFiles, )
		ImplRet({&open_files, GetMutexRef()})

	/*!
	\brief 创建路径指定的目录。
	\pre 间接断言：路径参数的数据指针非空。
	\exception std::system_error 调用失败。
		\li std::errc::file_exists 项已存在。
		\li std::errc::read_only_file_system 文件系统只读。
		\li std::errc::not_a_directory 路径前缀不是目录。
		\li std::errc::io_error 读写错误。
		\li std::errc::no_space_on_device 空间不足。
	\note 最后调用 Flush 。
	\since build 643
	*/
	void
	MakeDir(string_view) ythrow(std::system_error);

private:
	/*!
	\pre 断言：文件系统类型为 FAT32 。
	\see Microsoft FAT Specification Section 5 。
	*/
	void
	ReadFSInfo();

public:
	YB_ATTR_nodiscard YB_NONNULL(1) PDefH(bool, ReadFSInfoSector, byte* sec_buf)
		const ynothrowv
		ImplRet(ReadSector(fs_info_sector, sec_buf))

	YB_ATTR_nodiscard YB_NONNULL(1)
		PDefH(bool, ReadSector, ::sec_t sector, byte* sec_buf) const ynothrowv
		ImplRet(disc.ReadSectors(sector, 1, sec_buf))

	/*!
	\brief 移除名称位置指定的项。
	\exception std::system_error 调用失败。
		\li std::errc::io_error 读写错误。
	\note 先写入新文件覆盖，因此底层可能空间不足，此时作为写错误处理。
	\note 最后调用 Flush 。
	\sa IncrementPosition
	*/
	void
	RemoveEntry(const DEntry::NamePosition&) ythrow(std::system_error);

	/*!
	\brief 重命名路径。
	\pre 间接断言：第一路径参数的数据指针非空。
	\pre 断言：第二路径参数的数据指针非空。
	\exception std::system_error 调用失败。
		\li std::errc::cross_device_link 路径指定的不是同一分区。
		\li std::errc::read_only_file_system 文件系统只读。
		\li std::errc::no_such_file_or_directory
			指定的旧文件不存在或新路径为空串。
		\li std::errc::file_exists 指定的新项已存在；
		std::errc::io_error 读写错误。
	\note 第一参数指定已有文件的路径，第二参数指定新文件的路径。
	\note 第二个路径是完整路径，可能有根前缀。
	\since build 643
	*/
	void
	Rename(string_view, string_view) ythrow(std::system_error);

	//! \brief 查询文件系统信息并填充到参数。
	void
	StatFS(struct ::statvfs&);

	/*!
	\brief 查询指定路径的文件信息并填充到第一参数。
	\pre 间接断言：路径参数的数据指针非空。
	\exception std::system_error 调用失败。
		\li std::errc::io_error 读写错误或读取的项指定错误的簇。
	\since build 643
	*/
	PDefH(void, Stat, struct ::stat& st, string_view path)
		ythrow(std::system_error)
		ImplExpr(StatFromEntry(DEntry(*this, path).Data, st))

	/*!
	\brief 查询项数据的文件信息并填充到第一参数。
	\exception std::system_error 调用失败。
		\li std::errc::io_error 读取的项指定错误的簇。
	\sa ReadClusterFromEntry
	\since build 657
	*/
	void
	StatFromEntry(const EntryData&, struct ::stat&) const
		ythrow(std::system_error);

	/*!
	\brief 移除路径参数指定的链接。
	\pre 间接断言：路径参数的数据指针非空。
	\exception std::system_error 调用失败。
		\li std::errc::read_only_file_system 文件系统只读。
		\li std::errc::no_such_file_or_directory 指定的项不存在。
		\li std::errc::operation_not_permitted 无法移除 . 或 .. 项。
		\li std::errc::io_error 读写错误或读取的项指定错误的簇。
	\sa ReadClusterFromEntry
	\since build 643
	*/
	void
	Unlink(string_view) ythrow(std::system_error);

	/*!
	\pre 断言：文件系统类型为 FAT32 。
	\see Microsoft FAT Specification Section 5 。
	*/
	void
	WriteFSInfo() const;

	YB_ATTR_nodiscard YB_NONNULL(3) PDefH(bool, WriteSector, ::sec_t sector,
		const byte* sec_buf) const ynothrowv
		ImplRet(disc.WriteSectors(sector, 1, sec_buf))
};


/*!
\brief 取参数指定的路径对应的分区。
\pre 间接断言：参数非空。
*/
YB_ATTR_nodiscard YF_API YB_NONNULL(1) Partition*
FetchPartitionFromPath(const char*) ynothrowv;


/*!
\brief 目录状态。
\warning 除非另行指定，不保证并发读写安全。
*/
class DirState final
{
private:
	lref<Partition> part_ref;
	// XXX: Members %(start_cluster, valid_entry) should be protected by the
	//	partition mutex in construction.
	// TODO: More precise concurrency safety?
	ClusterIndex start_cluster;
	DEntry current_entry;
	bool valid_entry;

public:
	/*!
	\brief 构造：使用分区和路径。
	\pre 间接断言：路径参数的数据指针非空。
	\pre 断言：路径参数非空串。
	\exception std::system_error 构造失败。
		\li std::errc::no_such_device 无法访问路径指定的分区。
		\li std::errc::invalid_argument 路径非法。
		\li std::errc::not_a_directory 路径指定的不是目录。
		\li std::errc::no_such_file_or_directory 路径指定的目录项不存在。
		\li std::errc::io_error 查询项时读错误。
	\note 路径相对分区，无根前缀，空串路径视为当前工作目录。
	\since build 656
	*/
	DirState(Partition&, string_view) ythrow(std::system_error);

	DefGetter(const ynothrow, Partition&, PartitionRef, part_ref)

	/*!
	\brief 迭代：取下一个项并更新内部状态。
	\pre 间接断言：路径参数非空。
	\exception std::system_error 迭代失败。
		\li std::errc::no_such_device 无法访问路径指定的分区。
		\li std::errc::io_error 查询项时读错误或读取的项指定错误的簇。
	\return 0 表示迭代成功，-1 表示没有更多项可迭代。
	\note 没有更多项可迭代时，不需要区分原因，不视为错误，可总是视为到目录结尾。
	\note ::stat 指针参数指定需要更新的项信息，为空时忽略。
	\sa Partition::StatFromEntry
	\since build 900
	*/
	YB_ATTR_nodiscard YB_NONNULL(2) int
	Iterate(char*, struct ::stat*) ythrow(std::system_error);

	/*!
	\brief 复位状态。
	\exception std::system_error 复位失败。
		\li std::errc::io_error 查询项时读错误。
	*/
	void
	Reset() ythrow(std::system_error);
};


/*!
\brief 文件信息。
\warning 除非另行指定，不保证并发读写安全。
*/
class FileInfo final
{
private:
	enum Bits
	{
		ReadBit = 0,
		WriteBit,
		AppendBit,
		ModifiedBit,
		BitSize
	};
	std::uint32_t file_size;
	ClusterIndex start_cluster;
	std::uint32_t current_position;
	/*!
	\invariant <tt>GetPartitionRef().Table.IsFreeOrValid</tt>
		<tt>(rw_position.GetCluster())</tt> 。
	*/
	FilePosition rw_position;
	FilePosition append_position;
	//! \since build 642
	DEntry::NamePosition name_position;
	Partition* part_ptr;
	std::bitset<BitSize> attr{};

public:
	/*!
	\brief 构造：使用分区、路径和访问标识（指定读写权限）。
	\pre 间接断言：路径参数的数据指针非空。
	\exception std::system_error 构造失败。
		\lic std::errc::permission_denied 访问标识没有指定读写权限。
		\lic std::errc::is_a_directory 使用写权限打开目录。
		\lic std::errc::file_exists 要求创建不存在的文件但文件已存在。
		\lic std::errc::read_only_file_system 创建文件但文件系统只读。
		\lic std::errc::no_such_file_or_directory 文件不存在。
	\note 路径相对分区，无根前缀，空串路径视为当前工作目录。
	\note 访问标识包含 O_RDONLY 、O_WRONLY 或 O_RDWR 指定读写权限。
	\note 锁定分区访问。
	\since build 643
	*/
	FileInfo(Partition&, string_view, int);

	YB_ATTR_nodiscard YB_PURE PDefH(bool, CanAppend) const ynothrow
		ImplRet(attr[AppendBit])
	YB_ATTR_nodiscard YB_PURE PDefH(bool, CanRead) const ynothrow
		ImplRet(attr[ReadBit])
	YB_ATTR_nodiscard YB_PURE PDefH(bool, CanWrite) const ynothrow
		ImplRet(attr[WriteBit])
	DefPred(const ynothrow, Modified, attr[ModifiedBit])

	DefGetter(const ynothrow, std::uint32_t, CurrentPosition, current_position)
	//! \since build 642
	DefGetter(const ynothrow, const DEntry::NamePosition&, NamePosition,
		name_position)
	// XXX: Extended partition mutex to lock directory states.
	DefGetter(const ynothrow, mutex&, MutexRef, GetPartitionRef().GetMutexRef())
	DefGetter(const ynothrow, Partition&, PartitionRef, Deref(part_ptr))
	DefGetter(const ynothrow, const FilePosition&, RWPosition, rw_position)
	DefGetter(const ynothrow, FileSize, Size, file_size)
	DefGetter(const ynothrow, ClusterIndex, StartCluster, start_cluster)

	DefSetter(ynothrow, const FilePosition&, AppendPosition, append_position)
	DefSetter(ynothrow, FileSize, CurrentPosition, current_position)
	//! \pre 参数是先前由此对象的 GetRWPosition 取得的结果。
	DefSetter(ynothrow, const FilePosition&, RWPosition, rw_position)

	void
	CheckAppend() ynothrow;

	/*!
	\brief 扩展：分配更大空间。
	\exception std::system_error 扩展失败。
		\li std::errc::invalid_argument 参数非法。
		\li std::errc::no_space_on_device 空间不足。
		\li std::errc::io_error 写错误。
	*/
	void
	Extend() ythrow(std::system_error);

	/*!
	\brief 读文件内容到第一参数指定的缓冲区。
	\pre 间接断言：第一参数非空。
	\exception std::system_error 调用失败。
		\li std::errc::io_error 读错误。
	\since build 713
	*/
	YB_ATTR_nodiscard YB_NONNULL(2) ::ssize_t
	Read(char*, size_t) ythrow(std::system_error);

	/*!
	\brief 设置文件读写位置。
	\exception std::system_error 调用失败。
		\li std::errc::invalid_argument 参数非法。
		\li std::errc::value_too_large 位置溢出。
	\note 第二参数取值为 SEEK_SET 、 SEEK_CUR 和 SEEK_END 之一。
	\since build 713
	*/
	YB_ATTR_nodiscard ::off_t
	Seek(::off_t, int) ythrow(std::system_error);

	/*!
	\brief 读取文件信息并保存到参数。
	\exception std::system_error 调用失败。
		\li std::errc::io_error 读错误。
	*/
	void
	Stat(struct ::stat&) const;

	/*!
	\brief 同步：写入底层存储并清空修改状态。
	\exception std::system_error 同步失败。
		\li std::errc::io_error 读写错误。
	\since build 656
	*/
	void
	SyncToDisc() ythrow(std::system_error);

	/*!
	\brief 截断文件。
	\exception std::system_error 调用失败。
		\li std::errc::invalid_argument 参数非法。
		\li std::errc::io_error 缩减簇链时检验失败。
		\li std::errc::no_space_on_device 空间不足。
	\sa AllocationTable::TrimChain
	*/
	void
	Truncate(FileSize) ythrow(std::system_error);

	/*!
	\brief 设置文件读写位置。
	\exception std::system_error 调用失败。
		\li std::errc::invalid_argument 参数非法。
		\li std::errc::no_space_on_device 空间不足。
		\li std::errc::io_error 写错误。
	\since build 713
	*/
	YB_ATTR_nodiscard YB_NONNULL(2) ::ssize_t
	Write(const char*, size_t) ythrow(std::system_error);

	PDefH(void, UpdateSize, std::uint32_t size) ynothrow
		ImplUnseq(file_size = size, attr.set(ModifiedBit))
};


/*!
\brief 挂载 FAT 分区。
\pre 断言：第一参数的数据指针非空。
\note 参数为分区名、接口、起始扇区、分页数和每页扇区数的二进制位数。
\note 和 LibFAT 不同，不检查分区名长度超过 8 。
\since build 643

读取并装载 FAT 分区，设置 ::devoptab 操作。
文件和目录操作提供以下非 POSIX 指定的扩展错误：
错误 EINVAL 表示路径参数中有超过一个冒号；
错误 EIO 可能在迭代目录时发生。
*/
YB_ATTR_nodiscard YF_API bool
Mount(string_view, const ::DISC_INTERFACE&, ::sec_t, size_t, size_t);

/*!
\brief 卸载 FAT 分区。
\pre 间接断言：参数非空。
\return 是否找到已挂载的 FAT 分区并执行卸载。
*/
YB_ATTR_nodiscard YF_API YB_NONNULL(1) bool
Unmount(const char* name = "fat") ynothrow;
//@}

} // namespace FAT;
#endif

} // namespace platform_ex;

#endif

