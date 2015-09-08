/*
	© 2011-2015 FrankHB.

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
\version r940
\author FrankHB <frankhb1989@gmail.com>
\since build 604
\par 创建时间:
	2015-06-06 03:01:27 +0800
\par 修改时间:
	2015-09-07 22:35 +0800
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
//	platform::Concurrency, platform::FileSystemType, std::system_error, array,
//	string, ystdex::replace_cast;
#	include <bitset> // for std::bitset;
#	include <sys/syslimits.h> // for NAME_MAX.
#endif

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
	size_t
	GetBlockCount(::sec_t) const ynothrow;

public:
	//! \since build 609
	DefGetter(const ynothrow, Disc, Disc, disc)

private:
	PDefH(size_t, GetKey, ::sec_t sec) const ynothrowv
		ImplRet(sec & ~((1 << sectors_per_page_shift) - 1))
	ystdex::block_buffer*
	GetPage(::sec_t) ynothrow;

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

	//! \since build 608
	//@{
	template<typename _func>
	bool
	PerformPartialSectorIO(_func f, ::sec_t sec, size_t offset, size_t n)
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
	PerformSectorsIO(_func f, ::sec_t& sec, size_t& n)
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
//	carefully avoided. Thus the exception specifications are not effected even
//	if each creation of lock is not in a try-block.


//! \brief 分配表。
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
	ClusterIndex first_free = Clusters::First;
	ClusterIndex free_cluster = 0;
	ClusterIndex last_alloc_cluster = 0;
	ClusterIndex last_cluster;
	FileSystemType fs_type;
	ClusterIndex root_dir_cluster;

public:
	mutable SectorCache Cache;

	YB_NONNULL(3)
	AllocationTable(::sec_t, const byte*, size_t, size_t, Disc) ynothrowv;

	DefDeCopyAssignment(AllocationTable)

	PDefH(bool, IsValidCluster, ClusterIndex c) const ynothrow
		// NOTE: This will catch Clusters::Error.
		ImplRet(Clusters::First <= c && c <= last_cluster)

	DefGetter(const ynothrow, size_t, BytesPerCluster, bytes_per_cluster)
	DefGetter(const ynothrow, size_t, BytesPerSector, bytes_per_sector)
	DefGetter(const ynothrow, FileSystemType, FileSystemType, fs_type)
	DefGetter(const ynothrow, size_t, FreeClusters,
		fs_type == FileSystemType::FAT32 ? free_cluster : CountFreeCluster())
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

	bool
	ClearLinks(ClusterIndex) ynothrow;

	ClusterIndex
	CountFreeCluster() const ynothrow;

	ClusterIndex
	LinkFree(ClusterIndex) ynothrow;

	ClusterIndex
	QueryNext(ClusterIndex) const ynothrow;

	ClusterIndex
	QueryLast(ClusterIndex) const ynothrow;

	YB_NONNULL(2) void
	ReadClusters(const byte*) ynothrowv;

	ClusterIndex
	TrimChain(ClusterIndex, size_t) ynothrow;

	/*!
	\brief 链接空闲空间。
	\exception std::system_error 调用失败。
		\li std::errc::no_space_on_device 空间不足。
	*/
	ClusterIndex
	TryLinkFree(ClusterIndex) ythrow(std::system_error);

	YB_NONNULL(2) void
	WriteClusters(byte*) const ynothrowv;

	bool
	WriteEntry(ClusterIndex, std::uint32_t) const ynothrow;

	YB_NONNULL(2) void
	WriteFSInfo(byte*) ynothrowv;
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
	DefGetter(const ynothrow, size_t, Cluster, cluster)
	DefGetter(const ynothrow, ::sec_t, Sector, sector)

	DefSetter(ynothrow, size_t, Byte, byte)
	DefSetter(ynothrow, size_t, Cluster, cluster)
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

	PDefH(bool, IsFAT16RootCluster) const ynothrow
		ImplRet(cluster == Clusters::FAT16RootDirectory)

	DefGetter(const ynothrow, size_t, Byte, offset * EntryDataSize)
	DefGetter(const ynothrow, size_t, Cluster, cluster)
	DefGetter(const ynothrow, ::sec_t, Sector, sector)

	DefSetter(ynothrow, size_t, Cluster, cluster)
	DefSetter(ynothrow, ::sec_t, Sector, sector)

	PDefH(size_t, IncOffset, ) ynothrow
		ImplRet(++offset)

	PDefH(void, IncSectorAndResetOffset, ) ynothrow
		ImplUnseq(offset = 0, ++sector)

	friend PDefHOp(bool, ==, const DEntryPosition& x,
		const DEntryPosition& y) ynothrow
		ImplRet(x.cluster == y.cluster && x.sector == y.sector
			&& x.offset == y.offset)
};


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
	//! \brief 名称项位置。
	NamePosition NamePos;

private:
	string name;

public:
	/*!
	\brief 构造：使用先于第一个项的初始名称位置。
	\note 调用 QueryNextFrom 成功后有效。
	*/
	DEntry(ClusterIndex c)
		: NamePos(GenerateBeforeFirstNamePos(c))
	{}
	//! \brief 构造：分区根目录项。
	DEntry(Partition&);
	/*!
	\brief 构造：使用分区上的指定名称位置。
	\exception std::system_error 调用失败。
		\li std::errc::io_error 读错误。
	*/
	DEntry(Partition&, const NamePosition&);
	//! \pre 断言：指针参数非空。
	//@{
	/*!
	\brief 构造：使用分区上的指定路径。
	\exception std::system_error 调用失败。
		\li std::errc::filename_too_long 路径太长。
		\li std::errc::no_such_file_or_directory 项不存在。
	*/
	//@{
	YB_NONNULL(3)
	DEntry(Partition&, const char*);
	//! \note 最后一个参数表示路径结束位置。
	YB_NONNULL(3, 4)
	DEntry(Partition&, const char*, const char*);
	//@}
	/*!
	\brief 构造：初始化用于被添加的新条目。
	\note 第一参数未使用。
	\exception std::system_error 调用失败。
		\li std::errc::filename_too_long 路径太长。
		\li std::errc::no_such_file_or_directory 项不存在。
		\li std::errc::not_a_directory 非目录项。
		\li std::errc::invalid_argument 名称为空，
			或含有 LFN::IllegalCharacters 中的字符。
	*/
	YB_NONNULL(4)
	DEntry(int, Partition&, const char*, ClusterIndex&);
	//@}

	DefPred(const ynothrow, Dot, name == "." || name == "..")

	DefGetter(const ynothrow, const string&, Name, name)
	DefGetter(ynothrow, string&, NameRef, name)

	/*!
	\brief 添加项。
	\pre 使用 int 起始参数类型构造，且之前没有被调用。
	\exception std::system_error 调用失败。
		\li std::errc::file_exists ：项已存在。
		\li std::errc::no_space_on_device 空间不足。
		\li std::errc::invalid_argument 文件名非法导致生成后缀失败。
		\li std::errc::io_error 读写错误。
	*/
	void
	AddTo(Partition&, ClusterIndex);

	/*!
	\brief 查找指定簇后的空闲空间并分配位置。
	\pre 参数指定的分区和之前所有成员函数调用一致。
	\exception std::system_error 调用失败。
		\li std::errc::no_space_on_device 空间不足。
		\li std::errc::io_error 读写错误。
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
	\brief 查询分区内下一个状态。
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
\todo 添加修改卷标的接口。
*/
class Partition final : private ystdex::noncopyable, private ystdex::nonmovable
{
public:
	using OpenFilesSet = set<lref<FileInfo>, ystdex::lref_less<FileInfo>>;

private:
	Disc disc;
	//! \brief 卷标。
	VolumeLabel label;

public:
	AllocationTable Table;

private:
	::sec_t fs_info_sector;
	bool read_only;
	ClusterIndex cwd_cluster;
	OpenFilesSet open_files{};

	/*!
	\brief 构造：使用临时缓冲区、底层存储接口、分页数、每页扇区数的二进制位数
		和起始扇区。
	*/
	Partition(byte*, Disc, size_t, size_t, ::sec_t);

public:
	/*!
	\brief 构造：使用底层存储接口、分页数、每页扇区数的二进制位数和起始扇区。
	\note 分配 MaxSectorSize 字节的临时缓冲区用于构造时读写分区。
	*/
	Partition(Disc, size_t, size_t, ::sec_t);
	//! \brief 析构：刷新并关闭打开的文件。
	~Partition();

	DefPred(const ynothrow, ReadOnly, read_only)

	DefGetterMem(const ynothrow, size_t, BytesPerCluster, Table)
	DefGetterMem(const ynothrow, size_t, BytesPerSector, Table)
	DefGetter(const ynothrow, ClusterIndex, CWDCluster, cwd_cluster)
	DefGetter(const ynothrow, SectorCache&, CacheRef, Table.Cache)
	DefGetterMem(const ynothrow, FileSystemType, FileSystemType, Table)
	DefGetter(const ynothrow, const VolumeLabel&, Label, label)
	//! \since build 628
	DefGetter(const, string, LabelString,
		string(ystdex::replace_cast<const char*>(label.data()), label.size()))
	DefGetterMem(const ynothrow, mutex&, MutexRef, Table)
	DefGetterMem(const ynothrow, ClusterIndex, RootDirCluster, Table)
	DefGetterMem(const ynothrow, size_t, SectorsPerCluster, Table)
	DefGetterMem(const ynothrow, size_t, TotalSize, Table)

	/*!
	\brief 切换当前工作目录至参数指定路径的目录。
	\pre 间接断言：参数非空。
	\exception std::system_error 调用失败。\li std::errc::not_a_directory 指定的路径不是目录。
	*/
	YB_NONNULL(2) void
	ChangeDir(const char*) ythrow(std::system_error);

	/*!
	\brief 创建文件系统信息。
	\pre 断言：文件系统类型为 FAT32 。
	\see Microsoft FAT Specification Section 5 。
	*/
	void
	CreateFSInfo();

private:
	YB_NONNULL(1) ::sec_t
	FindFirstValidPartition(byte*) const ynothrowv;

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
	*/
	bool
	EntryExists(const string&, ClusterIndex) ythrow(system_error);

	//! \brief 从项数据中读取簇索引。
	ClusterIndex
	EntryGetCluster(const EntryData&) const ynothrow;

	/*!
	\brief 移动目录项位置至下一个项，当遇到文件结束时扩展。
	\exception std::system_error 调用失败。
		\li std::errc::no_space_on_device 空间不足。
	\todo 合并 IncrementPosition 实现。
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
			: ystdex::throw_system_error(std::errc::io_error))

	//! \brief 移动目录项位置至下一个项。
	bool
	IncrementPosition(DEntryPosition&) ynothrow;

	/*!
	\brief 分配已填充零的空间。
	\exception std::system_error 调用失败。
		\li std::errc::no_space_on_device 空间不足。
		\li std::errc::io_error 写错误。
	\sa AllocationTable::LinkFree
	*/
	ClusterIndex
	LinkFreeClusterCleared(ClusterIndex) ythrow(std::system_error);

	PDefH(locked_ptr<OpenFilesSet>, LockOpenFiles, )
		ImplRet({&open_files, GetMutexRef()})

	/*!
	\brief 创建路径指定的目录。
	\exception std::system_error 调用失败。
		\li std::errc::file_exists 项已存在。
		\li std::errc::read_only_file_system 文件系统只读。
		\li std::errc::not_a_directory 路径前缀不是目录。
		\li std::errc::io_error 读写错误。
		\li std::errc::no_space_on_device 空间不足。
	*/
	void
	MakeDir(const char*) ythrow(std::system_error);

private:
	//! \pre 断言：文件系统类型为 FAT32 。
	void
	ReadFSInfo();

public:
	YB_NONNULL(1) PDefH(bool, ReadFSInfoSector, byte* sec_buf) const ynothrowv
		ImplRet(ReadSector(fs_info_sector, sec_buf))

	YB_NONNULL(1) PDefH(bool, ReadSector, ::sec_t sector, byte* sec_buf) const
		ynothrowv
		ImplRet(disc.ReadSectors(sector, 1, sec_buf))

	/*!
	\brief 移除名称位置指定的项。
	\exception std::system_error 调用失败。\li std::errc::io_error 读写错误。
	*/
	void
	RemoveEntry(const DEntry::NamePosition&) ythrow(std::system_error);

	/*!
	\brief 重命名路径。
	\pre 间接断言：参数非空。
	\exception std::system_error 调用失败。
		\li std::errc::cross_device_link 路径指定的不是同一分区。
		\li std::errc::read_only_file_system 文件系统只读。
		\li std::errc::no_such_file_or_directory 指定的旧项不存在。
		\li std::errc::file_exists 指定的新项已存在；
		std::errc::io_error 读写错误。
	*/
	YB_NONNULL(2, 3) void
	Rename(const char*, const char*) ythrow(std::system_error);

	//! \brief 查询文件系统信息并填充到参数。
	void
	StatFS(struct ::statvfs&);

	/*!
	\brief 查询指定路径的项信息并填充到第一参数。
	\pre 间接断言：路径参数非空。
	\exception std::system_error 调用失败。
		\li std::errc::io_error 读写错误。
	*/
	YB_NONNULL(3) void
	Stat(struct ::stat&, const char*) ythrow(std::system_error);

	//! \brief 查询项数据的项信息并填充到第二参数。
	void
	StatFromEntry(const EntryData&, struct ::stat&) const ynothrow;

	/*!
	\brief 同步：更新文件信息到底层存储。
	\return 是否成功（刷新时没有发生读写错误）。
	\exception std::system_error 调用失败。
		\li std::errc::io_error 刷新前发生读写错误。
	*/
	bool
	Sync(const FileInfo&) const ythrow(system_error);

	/*!
	\brief 移除路径参数指定的链接。
	\exception std::system_error 调用失败。
		\li std::errc::read_only_file_system 文件系统只读。
		\li std::errc::no_such_file_or_directory 指定的项不存在。
		\li std::errc::operation_not_permitted 无法移除 . 或 .. 项。
		\li std::errc::io_error 读写错误。
	*/
	void
	Unlink(const char*) ythrow(std::system_error);

	//! \pre 断言：文件系统类型为 FAT32 。
	void
	WriteFSInfo() const;

	YB_NONNULL(3) PDefH(bool, WriteSector, ::sec_t sector, const byte* sec_buf)
		const ynothrowv
		ImplRet(disc.WriteSectors(sector, 1, sec_buf))
};


/*!
\brief 取参数指定的路径对应的分区。
\pre 间接断言：参数非空。
*/
YF_API YB_NONNULL(1) Partition*
FetchPartitionFromPath(const char*) ynothrow;


//! \brief 目录状态。
class DirState final
{
private:
	lref<Partition> part_ref;
	// XXX: Members %(start_cluster, valid_entry) should be protected by the
	//	partition mutex in construction.
	// FIXME: More precise concurrency safety?
	ClusterIndex start_cluster;
	DEntry current_entry;
	bool valid_entry;

public:
	/*!
	\brief 构造：使用路径。
	\pre 间接断言：参数非空。
	\exception std::system_error 构造失败。
		\li std::errc::no_such_device 无法访问路径指定的分区。
		\li std::errc::invalid_argument 路径非法。
		\li std::errc::not_a_directory 路径指定的不是目录。
		\li std::errc::no_such_file_or_directory 路径指定的目录项不存在。
		\li std::errc::io_error 查询项时读错误。
	\sa CheckColons
	\sa FetchPartitionFromPath
	*/
	YB_NONNULL(2)
	DirState(const char*) ythrow(std::system_error);

private:
	/*!
	\exception std::system_error 构造失败。
		\li std::errc::invalid_argument 路径非法。
		\li std::errc::not_a_directory 路径指定的不是目录。
		\li std::errc::no_such_file_or_directory 路径指定的目录项不存在。
		\li std::errc::io_error 查询项时读错误。
	\sa CheckColons
	*/
	DirState(Partition&, const char*, unique_lock<mutex>)
		ythrow(std::system_error);

public:
	DefGetter(const ynothrow, Partition&, PartitionRef, part_ref)

	/*!
	\brief 迭代：取下一个项并更新内部状态。
	\pre 间接断言：路径参数非空。
	\exception std::system_error 迭代失败。
		\li std::errc::no_such_device 无法访问路径指定的分区。
		\li std::errc::io_error 查询项时读错误。
	\note ::stat 指针参数指定需要更新的项信息，为空时忽略。
	*/
	YB_NONNULL(2) void
	Iterate(char*, struct ::stat*) ythrow(std::system_error);

	/*!
	\brief 复位状态。
	\exception std::system_error 复位失败。
		\li std::errc::io_error 查询项时读错误。
	*/
	void
	Reset() ythrow(std::system_error);
};


//! \brief 文件信息。
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
	FilePosition rw_position;
	FilePosition append_position;
	DEntry::NamePosition name_pos;
	Partition* part_ptr;
	std::bitset<BitSize> attr{};

public:
	/*!
	\brief 构造：使用分区、路径和访问标识（指定读写权限）。
	\pre 第二参数非空。
	\exception std::system_error 构造失败。
		\lic std::errc::permission_denied 访问标识没有指定读写权限。
		\lic std::errc::is_a_directory 使用写权限打开目录。
		\lic std::errc::file_exists 要求创建不存在的文件但文件已存在。
		\lic std::errc::read_only_file_system 创建文件但文件系统只读。
		\lic std::errc::no_such_file_or_directory 文件不存在。
	\note 访问标识包含 O_RDONLY 、 O_WRONLY 或 O_RDWR 指定读写权限。
	*/
	YB_NONNULL(3)
	FileInfo(Partition&, const char*, int);

	PDefH(bool, CanAppend) const ynothrow
		ImplRet(attr[AppendBit])
	PDefH(bool, CanRead) const ynothrow
		ImplRet(attr[ReadBit])
	PDefH(bool, CanWrite) const ynothrow
		ImplRet(attr[WriteBit])
	DefPred(const ynothrow, Modified, attr[ModifiedBit])

	DefGetter(const ynothrow, std::uint32_t, CurrentPosition, current_position)
	DefGetter(const ynothrow, const DEntry::NamePosition&, NamePos, name_pos)
	// XXX: Extended partition mutex to lock directory states.
	DefGetter(const ynothrow, mutex&, MutexRef, GetPartitionRef().GetMutexRef())
	DefGetter(const ynothrow, Partition&, PartitionRef, Deref(part_ptr))
	DefGetter(const ynothrow, const FilePosition&, RWPosition, rw_position)
	DefGetter(const ynothrow, FileSize, Size, file_size)
	DefGetter(const ynothrow, ClusterIndex, StartCluster, start_cluster)

	DefSetter(ynothrow, const FilePosition&, AppendPosition, append_position)
	DefSetter(ynothrow, FileSize, CurrentPosition, current_position)
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

	void
	Shrink() ynothrow;

	int
	SyncToDisc() ynothrow;

	/*!
	\brief 读取文件信息并保存到参数。
	\exception std::system_error 调用失败。
		\li std::errc::io_error 读错误。
	*/
	void
	Stat(struct ::stat&) const;

	/*!
	\brief 截断文件。
	\exception std::system_error 调用失败。
		\li std::errc::invalid_argument 参数非法。
		\li std::errc::no_space_on_device 空间不足。
	*/
	void
	Truncate(FileSize) ythrow(std::system_error);

	/*!
	\brief 读文件内容到第一参数指定的缓冲区。
	\pre 间接断言：第一参数非空。
	\exception std::system_error 调用失败。
		\li std::errc::io_error 读错误。
	*/
	YB_NONNULL(2) ::ssize_t
	TryRead(char*, size_t) ythrow(std::system_error);

	/*!
	\brief 设置文件读写位置。
	\exception std::system_error 调用失败。
		\li std::errc::invalid_argument 参数非法。
		\li std::errc::value_too_large 位置溢出。
	\note 第二参数取值为 SEEK_SET 、 SEEK_CUR 和 SEEK_END 之一。
	*/
	::off_t
	TrySeek(::off_t, int) ythrow(std::system_error);

	/*!
	\brief 设置文件读写位置。
	\exception std::system_error 调用失败。
		\li std::errc::invalid_argument 参数非法。
		\li std::errc::no_space_on_device 空间不足。
		\li std::errc::io_error 写错误。
	*/
	YB_NONNULL(2) ::ssize_t
	TryWrite(const char*, size_t) ythrow(std::system_error);

	PDefH(void, UpdateSize, std::uint32_t size) ynothrow
		ImplUnseq(file_size = size, attr.set(ModifiedBit))
};


/*!
\brief 挂载 FAT 分区。
\note 参数为分区名、接口、起始扇区、分页数和每页扇区数的二进制位数。

读取并装载 FAT 分区，设置 ::devoptab 操作。
文件和目录操作提供以下非 POSIX 指定的扩展错误：
错误 EINVAL 表示路径参数中有超过一个冒号；
错误 EIO 可能在迭代目录时发生。
*/
YF_API bool
Mount(const string&, const ::DISC_INTERFACE&, ::sec_t, size_t, size_t);

/*!
\brief 卸载 FAT 分区。
\pre 间接断言：参数非空。
\return 是否找到已挂载的 FAT 分区并执行卸载。
*/
YF_API YB_NONNULL(1) bool
Unmount(const char* name = "fat") ynothrow;
//@}

} // namespace FAT;

#endif

} // namespace platform_ex;

#endif

