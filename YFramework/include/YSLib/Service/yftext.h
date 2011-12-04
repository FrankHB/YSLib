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
\version r1669;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-11-24 23:14:41 +0800;
\par 修改时间:
	2011-12-04 12:48 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YFile_(Text);
*/


#ifndef YSL_INC_CORE_YFTEXT_H_
#define YSL_INC_CORE_YFTEXT_H_

#include "yfile.h"

YSL_BEGIN

// BOM（byte-order mark ，字节顺序标记）常量；零宽无间断空格字符的单字节编码。
yconstexpr char BOM_UTF_16LE[2] = {0xFF, 0xFE};
yconstexpr char BOM_UTF_16BE[2] = {0xFE, 0xFF};
yconstexpr char BOM_UTF_8[3] = {0xEF, 0xBB, 0xBF};
yconstexpr char BOM_UTF_32LE[4] = {0xFF, 0xFE, 0x00, 0x00};
yconstexpr char BOM_UTF_32BE[4] = {0x00, 0x00, 0xFE, 0xFF};


//! \brief 文本文件类。
class TextFile : public File
{
private:
	SizeType bl; //!<  BOM 大小。
	Text::Encoding cp; //!< 编码。

public:
	/*!
	\brief 构造：使用指定文件路径初始化对象。
	*/
	explicit
	TextFile(const_path_t);

	DefGetter(const ynothrow, u8, BOMSize, bl) //!< 取 BOM 大小。
	DefGetter(const ynothrow, Text::Encoding, Encoding, cp) //!< 取编码。
	DefGetter(const ynothrow, SizeType, TextSize, GetSize() - GetBOMSize()) \
		//!< 取文本区段大小。
	DefGetter(const ynothrow, SizeType, TextPosition, GetPosition() - bl) \
		//!< 取文件指针关于文本区段的位置。

	/*!
	\brief 检查文件头是否有 BOM(Byte Order Mark) ，若有则据此判断编码。
		返回 BOM 的长度。
	*/
	u8
	CheckBOM(Text::Encoding&);

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

	using File::Read;
	/*!
	\brief 从文件读 n 字节到 s 中。
	*/
	SizeType
	Read(void* s, SizeType n) const;
};

YSL_END

#endif

