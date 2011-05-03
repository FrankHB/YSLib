/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yfile.h
\ingroup Core
\brief 平台无关的文件抽象。
\version 0.1794;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-24 23:14:41 +0800;
\par 修改时间:
	2011-05-03 19:32 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YFile;
*/


#ifndef YSL_INC_CORE_YFILE_H_
#define YSL_INC_CORE_YFILE_H_

#include "../Core/ycutil.h"

YSL_BEGIN

//! \brief 文件基类。
class File : public noncopyable
{
public:
	typedef size_t SizeType; //!< 大小类型。
	typedef std::ptrdiff_t OffsetType; //!< 偏移量类型。

protected:
	FILE* fp; //!< 默认文件指针。
	SizeType fsize; //!< 文件大小。

public:
	/*!
	\brief 构造：使用指定文件路径初始化对象。
	*/
	explicit
	File(CPATH);
	/*!
	\brief 析构。
	\note 自动关闭文件。
	*/
	virtual
	~File();

	DefPredicate(Valid, fp) //!< 判断文件指针是否有效。

	DefGetter(FILE*, Ptr, fp) //!< 取文件指针。
	DefGetter(SizeType, Size, fsize) //!< 取文件大小。
	/*!
	\brief 取文件指针的位置，返回值语义同 std::ftell 。
	*/
	PDefH0(OffsetType, GetPosition) const
		ImplRet(std::ftell(fp))

	/*!
	\brief 设置文件指针位置。
	\note 参数和返回值语义同 std::fseek 。
	*/
	PDefH2(int, SetPosition, OffsetType offset, int whence) const
		ImplRet(std::fseek(fp, offset, whence))

	/*!
	\brief 检测文件结束符。
	\note 参数和返回值语义同 std::feof() 。
	*/
	PDefH0(int, CheckEOF) const
		ImplRet(std::feof(fp))

	/*!
	\brief 关闭文件。
	\note 清除文件指针。
	*/
	void
	Close();

	/*!
	\brief 打开指定路径的文件。
	*/
	bool
	Open(CPATH);

	/*!
	\brief 连续读 nmemb 个大小为 size 文件块到 ptr 中。
	\return 返回成功读取的文件块数。
	*/
	PDefH3(SizeType, Read, void* ptr, SizeType size, SizeType nmemb) const
		ImplRet(std::fread(ptr, size, nmemb, fp))

	/*!
	\brief 文件指针返回到文件头，语义同 std::rewind 。
	*/
	PDefH0(void, Rewind) const
		ImplRet(std::rewind(fp))
};

YSL_END

#endif

