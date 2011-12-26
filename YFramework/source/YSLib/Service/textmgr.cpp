/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file textmgr.cpp
\ingroup Service
\brief 文本管理服务。
\version r4282;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2010-01-05 17:48:09 +0800;
\par 修改时间:
	2011-12-25 11:48 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Service::TextManager;
*/


#include "YSLib/Service/textmgr.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Text)

TextFileBuffer::TextFileBuffer(TextFile& file)
	: File(file), Buffer()
{
	using namespace CharSet;

	switch(File.GetEncoding())
	{
	case UTF_16:
	case UTF_16LE:
	case UTF_16BE:
		Buffer.reserve(File.GetTextSize());
		break;
	default:
		Buffer.reserve(File.GetTextSize() << 1);
	}
	LoadText(File.GetTextSize());
}

TextFileBuffer::Iterator
TextFileBuffer::GetBegin() const
{
	return Buffer.begin();
}
TextFileBuffer::Iterator
TextFileBuffer::GetEnd() const
{
	return Buffer.end();
}

size_t
TextFileBuffer::LoadText(size_t n)
{
	if(n > Buffer.capacity())
		return 0;
	if(File.IsValid())
	{
		auto l(Buffer.size());

		Buffer.resize(l + n);

		const auto l_old(l);
		const auto fp(File.GetPtr());
		const auto cp(File.GetEncoding());
		size_t idx(0);
		ConversionState st;

		while(idx < n)
		{
			auto t(MBCToUC(Buffer[l], fp, cp, st));

			if(GetCountOf(st) < 0)
				break;
			yunseq(idx += t, ++l);
		}
		Buffer.resize(l);
		return l - l_old;
	}
	return 0;
}

YSL_END_NAMESPACE(Text)

YSL_END

