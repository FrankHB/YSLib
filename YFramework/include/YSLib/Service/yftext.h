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
\version r1701;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-11-24 23:14:41 +0800;
\par 修改时间:
	2011-12-30 22:21 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YFile_(Text);
*/


#ifndef YSL_INC_CORE_YFTEXT_H_
#define YSL_INC_CORE_YFTEXT_H_

#include "yfile.h"

YSL_BEGIN

/*!
\brief Unicode 编码模式标记。

Unicode Encoding Scheme Signatures BOM（byte-order mark ，字节顺序标记）常量。
\note 在 Unicode 3.2 前作为零宽无间断空格字符在对应字符集的编码单字节序列。
\note 适用于未明确字节序或字符集的流。
\since build 244 。
*/
//@{
yconstexpr char BOM_UTF_16LE[2] = {0xFF, 0xFE};
yconstexpr char BOM_UTF_16BE[2] = {0xFE, 0xFF};
yconstexpr char BOM_UTF_8[3] = {0xEF, 0xBB, 0xBF};
yconstexpr char BOM_UTF_32LE[4] = {0xFF, 0xFE, 0x00, 0x00};
yconstexpr char BOM_UTF_32BE[4] = {0x00, 0x00, 0xFE, 0xFF};
//@}


//! \brief 文本文件类。
class TextFile : public File
{
private:
	size_t bl; //!<  BOM 大小。
	Text::Encoding cp; //!< 编码。

public:
	/*!
	\brief 构造：使用指定文件路径初始化对象。
	*/
	explicit
	TextFile(const_path_t);

	DefGetter(const ynothrow, u8, BOMSize, bl) //!< 取 BOM 大小。
	DefGetter(const ynothrow, Text::Encoding, Encoding, cp) //!< 取编码。
	DefGetter(const ynothrow, size_t, TextSize, GetSize() - GetBOMSize()) \
		//!< 取文本区段大小。
	DefGetter(const ynothrow, size_t, TextPosition, GetPosition() - bl) \
		//!< 取文件指针关于文本区段的位置。

	/*!
	\brief 检查文件头是否有 BOM(Byte Order Mark) ，若有则据此判断编码。
	\return BOM 的长度（字节数）。
	\since build 273 。
	*/
	size_t
	CheckBOM(Text::Encoding&);

	/*!
	\brief 定位：设置文件读位置。
	\since build 273 。
	*/
	void
	Locate(u32) const;

	/*!
	\brief 设置文件读位置为文本区段头。
	*/
	void
	Rewind() const;

	using File::Read;
	/*!
	\brief 从文件读 n 字节到 s 中。
	*/
	size_t
	Read(void* s, size_t n) const;

	/*!
	\brief 按自身编码读取 Unicode 字符。
	\since build 273 。
	*/
	template<typename _tChar, typename... _tParams>
	inline Text::ConversionResult
	ReadChar(_tChar& c, _tParams&&... args) const
	{
		return MBCToUC(c, fp, cp, args...);
	}

	/*!
	\brief 按自身编码读取但不保存 Unicode 字符。
	\since build 273 。
	*/
	template<typename... _tParams>
	inline Text::ConversionResult
	SkipChar(_tParams&&... args) const
	{
		return MBCToUC(fp, cp, args...);
	}
};

YSL_END

#endif

