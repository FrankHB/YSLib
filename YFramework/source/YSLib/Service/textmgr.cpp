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
\version r4266;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-01-05 17:48:09 +0800;
\par 修改时间:
	2011-10-10 02:10 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Service::TextManager;
*/


#include "textmgr.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Text)

TextFileBuffer::TextFileBuffer(TextFile& file)
	: TextFile(file), vector<ucs2_t>()
{
	using namespace CharSet;

	switch(GetEncoding())
	{
	case UTF_16:
	case UTF_16LE:
	case UTF_16BE:
		reserve(GetTextSize());
		break;
	default:
		reserve(GetTextSize() << 1);
	}
	LoadText(GetTextSize());
}

SizeType
TextFileBuffer::LoadText(SizeType n)
{
	TextFile& f(*this);
	vector<ucs2_t>& b(*this);

	if(n > b.capacity())
		return 0;
	if(f.IsValid())
	{
		auto l(b.size());

		b.resize(l + n);

		const auto l_old(l);
		const auto fp(f.GetPtr());
		const auto cp(f.GetEncoding());
		SizeType idx(0);
		ConversionState st;

		while(idx < n)
		{
			auto t(MBCToUC(b[l], fp, cp, st));

			if(GetCountOf(st) < 0)
				break;
			yunsequenced(idx += t, ++l);
		}
		b.resize(l);
		return l - l_old;
	}
	return 0;
}

YSL_END_NAMESPACE(Text)

YSL_END

