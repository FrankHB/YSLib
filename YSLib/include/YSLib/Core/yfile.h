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
\version 0.1698;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-24 23:14:41 + 08:00;
\par 修改时间:
	2011-02-20 13:49 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YFile;
*/


#ifndef INCLUDED_YFILE_H_
#define INCLUDED_YFILE_H_

#include "ysdef.h"
#include "yobject.h"
#include "ystring.h"

YSL_BEGIN

//! \brief 文件基类。
class YFile : public YObject
{
public:
	typedef YObject ParentType;
	typedef std::size_t SizeType; //!< 大小类型。
	typedef std::ptrdiff_t OffType; //!< 偏移量类型。

protected:
	FILE* fp; //!< 默认文件指针。
	SizeType fsize; //!< 文件大小。

public:
	/*!
	\brief 构造：使用指定文件路径初始化对象。
	*/
	explicit
	YFile(CPATH);
	/*!
	\brief 析构。
	\note 自动关闭文件。
	*/
	virtual
	~YFile();

	DefPredicate(Valid, fp) //!< 判断文件指针是否有效。
	DefGetter(FILE*, Ptr, fp) //!< 取文件指针。
	DefGetter(SizeType, Size, fsize) //!< 取文件大小。

	/*!
	\brief 清除文件指针。
	\note 关闭文件。
	*/
	void
	Release();

	/*!
	\brief 打开指定路径的文件。
	*/
	bool
	Open(CPATH);

	/*!
	\brief 取文件指针的位置，返回值语义同 std::ftell 。
	*/
	OffType
	ftell() const;

	/*!
	\brief 文件指针返回到文件头，同 std:: rewind 。
	*/
	void
	rewind() const;

	/*!
	\brief 设置文件指针位置。
	\note 参数和返回值语义同 std::fseek 。
	*/
	int
	fseek(OffType, int) const;

	/*!
	\brief 连续读 nmemb 个大小为 size 文件块到 ptr 中。
	\return 返回成功读取的文件块数。
	*/
	SizeType
	fread(void* ptr, SizeType size, SizeType nmemb) const;

	/*!
	\brief 检测文件结束符。
	\note 参数和返回值语义同 std::feof() 。
	*/
	int
	feof() const;
};

inline YFile::OffType
YFile::ftell() const
{
	return std::ftell(fp);
}

inline void
YFile::rewind() const
{
	std::rewind(fp);
}

inline int
YFile::fseek(YFile::OffType offset, int whence) const
{
	return std::fseek(fp, offset, whence);
}

inline std::size_t
YFile::fread(void* ptr, std::size_t size, std::size_t nmemb) const
{
	return std::fread(ptr, size, nmemb, fp);
}

inline int
YFile::feof() const
{
	return std::feof(fp);
}

YSL_END

#endif

