/*
	© 2009-2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file File.h
\ingroup Service
\brief 平台中立的文件抽象。
\version r1694
\author FrankHB <frankhb1989@gmail.com>
\since build 473
\par 创建时间:
	2009-11-24 23:14:41 +0800
\par 修改时间:
	2021-08-03 19:35 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::File
*/


#ifndef YSL_INC_Service_File_h_
#define YSL_INC_Service_File_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YCoreUtilities // for MappedFile, std::istream,
//	ofstream;
#include <ystdex/streambuf.hpp> // for ystdex::membuf;

namespace YSLib
{

namespace IO
{

/*!
\brief 打开文件。
\pre 路径参数非空。
\throw std::system_error 打开失败。
\return 非空的文件指针。
\sa uopen
\since build 923
*/
template<typename... _tParams>
YB_NONNULL(1) UniqueFile
OpenFile(const char* filename, _tParams&&... args)
{
	if(UniqueFile p_ifile{uopen(filename, yforward(args)...)})
		return p_ifile;
	else
		ystdex::throw_error(errno, "Failed opening file '"
			+ std::string(filename) + '\'');
}


/*!
\brief 移除文件链接。
\pre 间接断言：参数非空。
\throw std::system_error 文件存在且操作失败。
\since build 713
*/
//@{
template<typename _tChar>
YB_NONNULL(1) void
Remove(const _tChar* path)
{
	if(YB_UNLIKELY(!uremove(path)))
	{
		const int err(errno);

		if(err != ENOENT)
			ystdex::throw_error(err, ("Failed removing destination file '"
				+ IO::MakePathString(path) + '\'').c_str());
	}
}

template<typename _tChar>
YB_NONNULL(1) void
Unlink(const _tChar* path)
{
	if(YB_UNLIKELY(!uunlink(path)))
	{
		const int err(errno);

		if(err != ENOENT)
			ystdex::throw_error(err, ("Failed unlinking destination file '"
				+ IO::MakePathString(path) + '\'').c_str());
	}
}
//@}


//! \since build 724
//@{
//! \brief 共享锁定的文件映射输入流。
class YF_API SharedInputMappedFileStream : private MappedFile,
	private SharedIndirectLockGuard<const UniqueFile>, private ystdex::membuf,
	public std::istream
{
public:
	// NOTE: See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=91127.
#if YB_IMPL_GNUCPP
	YB_ATTR(nonnull)
#else
	YB_NONNULL(2)
#endif
	SharedInputMappedFileStream(const char*);

	//! \since build 805
	using basic_ios::operator!;

	//! \since build 805
	using basic_ios::operator bool;

	//! \brief 虚析构：类定义外默认实现。
	~SharedInputMappedFileStream() override;
};


//! \brief 独占锁定的文件输出流。
class YF_API UniqueLockedOutputFileStream : public ofstream
{
private:
	FileDescriptor desc;
	unique_lock<FileDescriptor> lock;

	//! \since build 838
	UniqueLockedOutputFileStream(int, std::ios_base::openmode);

public:
	//! \since build 838
	UniqueLockedOutputFileStream(UniqueFile, std::ios_base::openmode);
	/*!
	\pre 间接断言：指针参数非空。
	\since build 838
	*/
	//@{
	/*!
	\pre 指定打开模式的参数兼容。
	\note 为避免误用，打开模式不提供默认参数。
	*/
	template<typename _tChar>
	// XXX: Not sure why this is not effected by the bug of GCC reported at PR
	//	91127.
	YB_NONNULL(3)
	UniqueLockedOutputFileStream(int omode, const _tChar* filename,
		std::ios_base::openmode mode, mode_t pmode = DefaultPMode())
		: UniqueLockedOutputFileStream(uopen(filename, omode, pmode), mode)
	{}
	//! \note std::ios_base::openmode 可能是 int 。
	template<typename _tChar>
	// NOTE: See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=91127.
#if YB_IMPL_GNUCPP
	YB_ATTR(nonnull)
#else
	YB_NONNULL(2)
#endif
	UniqueLockedOutputFileStream(const _tChar* filename,
		std::ios_base::openmode mode, mode_t pmode = DefaultPMode())
		: UniqueLockedOutputFileStream(uopen(filename, {}, mode, pmode), mode)
	{}
	//@}

	//! \brief 虚析构：类定义外默认实现。
	~UniqueLockedOutputFileStream() override;
};
//@}

} // namespace IO;

} // namespace YSLib;

#endif

