/*
	Copyright by FrankHB 2009 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yftext.h
\ingroup Service
\brief 平台无关的文本文件抽象。
\version r772
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-24 23:14:41 +0800
\par 修改时间:
	2013-05-06 14:01 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YFile_(Text)
*/


#ifndef YSL_INC_CORE_YFTEXT_H_
#define YSL_INC_CORE_YFTEXT_H_ 1

#include "yfile.h"
#include "../Adaptor/ycont.h" // for string;

YSL_BEGIN

/*!
\brief Unicode 编码模式标记。

Unicode Encoding Scheme Signatures BOM（byte-order mark ，字节顺序标记）常量。
\note 在 Unicode 3.2 前作为零宽无间断空格字符在对应字符集的编码单字节序列。
\note 适用于未明确字节序或字符集的流。
\since build 297
*/
//@{
yconstexpr char BOM_UTF_16LE[]{"\xFF\xFE"};
yconstexpr char BOM_UTF_16BE[]{"\xFE\xFF"};
yconstexpr char BOM_UTF_8[]{"\xEF\xBB\xBF"};
yconstexpr char BOM_UTF_32LE[]{"\xFF\xFE\x00\x00"};
yconstexpr char BOM_UTF_32BE[]{"\x00\x00\xFE\xFF"};
//@}


//! \brief 文本文件类。
class YF_API TextFile : public File
{
private:
	size_t bl; //!<  BOM 大小。

public:
	/*!
	\brief 编码。
	\warning 修改编码可能会造成读取错误。
	\since build 290
	*/
	Text::Encoding Encoding;

	/*!
	\brief 构造：使用指定文件名、编码和模式初始化文本文件对象。
	\note 当打开文件大小为零且以可写方式打开时按编码写入 BOM 。
	\since build 341
	*/
	explicit
	TextFile(IO::const_path_t, std::ios_base::openmode = std::ios_base::in,
		Text::Encoding = Text::CS_Default);
	/*!
	\brief 构造：使用指定文件名初始化只读文本文件对象。
	\since build 305
	*/
	explicit
	TextFile(const String&);

	/*!
	\brief 取 BOM 字符串。
	\post 同 Rewind() 。
	\since build 341
	*/
	string
	GetBOM() const;
	DefGetter(const ynothrow, u8, BOMSize, bl) //!< 取 BOM 大小。
	DefGetter(const ynothrow, size_t, TextSize, GetSize() - GetBOMSize()) \
		//!< 取文本区段大小。
	DefGetter(const ynothrow, size_t, TextPosition, GetPosition() - bl) \
		//!< 取文件指针关于文本区段的位置。

	/*!
	\brief 检查文件头是否有 BOM(Byte Order Mark) ，若有则据此判断编码。
	\return BOM 的长度（字节数）。
	\since build 273
	*/
	size_t
	CheckBOM(Text::Encoding&);

	/*!
	\brief 定位：设置文件读位置。
	\since build 273
	*/
	void
	Locate(u32) const;

	/*!
	\brief 设置文件读位置为文本区段头。
	*/
	void
	Rewind() const;

	/*!
	\brief 按自身编码读取 Unicode 字符。
	\since build 273
	*/
	template<typename _tChar, typename... _tParams>
	inline Text::ConversionResult
	ReadChar(_tChar& c, _tParams&&... args) const
	{
		return MBCToUC(c, GetPtr(), Encoding, args...);
	}

	/*!
	\brief 按自身编码读取但不保存 Unicode 字符。
	\since build 273
	*/
	template<typename... _tParams>
	inline Text::ConversionResult
	SkipChar(_tParams&&... args) const
	{
		return MBCToUC(GetPtr(), Encoding, args...);
	}

	/*!
	\brief 截断文本。
	\see File::Truncate 。
	\since build 341
	*/
	bool
	Truncate(size_t) const override;
};

YSL_END

#endif

