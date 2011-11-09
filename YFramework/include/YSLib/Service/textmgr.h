/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file textmgr.h
\ingroup Service
\brief 文本管理服务。
\version r4550;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-01-05 17:48:09 +0800;
\par 修改时间:
	2011-11-05 10:54 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Service::TextManager;
*/


#ifndef YSL_INC_SERVICE_TEXTMGR_H_
#define YSL_INC_SERVICE_TEXTMGR_H_

#include "../Core/ystring.h"
#include "yftext.h"
#include "../Adaptor/ycont.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Text)

//文本文件块缓冲区。
class TextFileBuffer : public TextFile, protected std::vector<ucs2_t>
{
public:
	typedef vector<ucs2_t>::const_iterator const_iterator;
	typedef vector<ucs2_t>::iterator iterator;
	typedef const_iterator Iterator;

	/*!
	\brief 构造：使用文本文件。
	*/
	explicit
	TextFileBuffer(TextFile&);
	/*!
	\brief 析构：空实现。
	*/
	virtual DefEmptyDtor(TextFileBuffer)

	using vector<ucs2_t>::operator[];

	using vector<ucs2_t>::size;
	using vector<ucs2_t>::capacity;

	using vector<ucs2_t>::at;

	using vector<ucs2_t>::clear;

	using vector<ucs2_t>::resize;

	using vector<ucs2_t>::begin;
	using vector<ucs2_t>::end;
	using vector<ucs2_t>::cbegin;
	using vector<ucs2_t>::cend;

protected:
	/*!
	\brief 从文本文件中读取连续 n 个字节到文本缓冲区块中，并返回成功读取的字符数。
	\note 超过最大长度则放弃读取。
	*/
	SizeType
	LoadText(SizeType n);
};

YSL_END_NAMESPACE(Text)

YSL_END

#endif

