/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yftext.h
\ingroup Core
\brief 平台无关的文本文件抽象。
\version 0.1588;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-24 23:14:41 +0800;
\par 修改时间:
	2011-04-03 19:18 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YFile_(Text);
*/


#ifndef INCLUDED_YFTEXT_H_
#define INCLUDED_YFTEXT_H_

#include "yfile.h"

YSL_BEGIN

//! \brief 文本文件类。
class YTextFile : public YFile
{
public:
	typedef YFile ParentType;

private:
	SizeType bl; //!<  BOM 大小。
	Text::CSID cp; //!< 编码。

public:
	/*!
	\brief 构造：使用指定文件路径初始化对象。
	*/
	explicit
	YTextFile(CPATH);

	DefGetter(u8, BOMSize, bl) //!< 取 BOM 大小。
	DefGetter(Text::CSID, CP, cp) //!< 取编码。
	DefGetter(SizeType, TextSize, GetSize() - GetBOMSize()) //!< 取文本区段大小。
	DefGetter(SizeType, Pos, ftell() - bl) //!< 取文件指针关于文本区段的位置。

	//
	/*!
	\brief 检查文件头是否有 BOM(Byte Order Mark) ，若有则据此判断编码。
	//			返回 BOM 的长度。
	*/
	u8
	CheckBOM(Text::CSID&);

	/*!
	\brief 设置文件读位置为文本区段头。
	*/
	void
	Rewind() const;

	/*!
	\brief 设置文件读位置。
	*/
	void
	SetPos(u32) const;

	/*!
	\brief 设置文件读位置。
	\note whence 语义同 fseek 函数（除 SEEK_SET 表示起始为文本区段头）。
	*/
	void
	Seek(long, int whence) const;

	/*!
	\brief 从文件读 n 字节到 s 中。
	*/
	SizeType
	Read(void* s, SizeType n) const;

	/*!
	\brief 从文件读 n 字节到 s 中。
	\note 按默认编码转化为 UTF-16LE 。
	*/
	SizeType
	ReadS(uchar_t* s, SizeType n) const;
};

YSL_END

#endif

