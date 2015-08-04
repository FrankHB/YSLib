/*
	© 2009-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file File.h
\ingroup Service
\brief 平台无关的文件抽象。
\version r1405
\author FrankHB <frankhb1989@gmail.com>
\since build 473
\par 创建时间:
	2009-11-24 23:14:41 +0800
\par 修改时间:
	2015-08-04 22:14 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::File
*/


#ifndef YSL_INC_Service_File_h_
#define YSL_INC_Service_File_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YCoreUtilities
#include <iterator> // for std::istream_iterator;
#include <cwctype>
#include YFM_YSLib_Adaptor_YTextBase
#include <sstream> // for std::ostringstream;

namespace YSLib
{

/*!
\brief 文件基类。
\since build 206
*/
class YF_API File : private noncopyable
{
private:
	//! \since build 621
	mutable fstream stream{};
	//! \since build 341
	size_t fsize = 0; //!< 文件大小。

public:
	DefDeCtor(File)
	/*!
	\brief 构造：使用指定文件路径初始化对象。
	\note 自动打开文件。
	\since build 412
	*/
	File(const char*, std::ios_base::openmode);
	/*!
	\brief 析构。
	\note 自动关闭文件。
	*/
	virtual
	~File();

	/*!
	\brief 判断是否已经打开或未打开。
	\since build 319
	*/
	DefBoolNeg(explicit, stream.is_open())

	/*!
	\brief 取流引用。
	\since build 621
	*/
	DefGetter(const ynothrow, fstream&, Stream, stream)
	DefGetter(const ynothrow, size_t, Size, fsize) //!< 取文件大小。
	/*!
	\brief 取文件指针的位置。
	\return 成功时同 std::ftell 。
	\throw LoggedEvent 失败。
	\since build 587
	*/
	size_t
	GetPosition();

	/*!
	\brief 检测文件结束符。
	\note 参数语义同 \c std::fstream::eof() 。
	\since build 621
	*/
	PDefH(bool, CheckEOF, ) const
		ImplRet(stream.eof())

private:
	/*!
	\brief 检查文件有效长度后读位置返回文件起始。
	\since build 305
	*/
	void
	CheckSize();

public:
	/*!
	\brief 以指定模式打开指定路径的文件并清楚空白符分隔读写。
	\note 语义同 \c std::fstream::open 。
	\since build 412
	*/
	bool
	Open(const char*, std::ios_base::openmode);
};

} // namespace YSLib;

#endif

