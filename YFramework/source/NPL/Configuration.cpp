/*
	© 2012-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Configuration.cpp
\ingroup NPL
\brief 配置设置。
\version r757
\author FrankHB <frankhb1989@gmail.com>
\since build 334
\par 创建时间:
	2012-08-27 15:15:06 +0800
\par 修改时间:
	2014-10-21 12:50 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::Configuration
*/


#include "NPL/YModules.h"
#include YFM_NPL_Configuration
#include YFM_NPL_SContext

using namespace YSLib;

namespace NPL
{

namespace
{

//! \since build 334
inline File&
WritePrefix(File& f, size_t n = 1, char c = '\t')
{
	while(n--)
		f << c;
	return f;
}

#if 0
//! \since build 334
File&
WriteNode(File& f, const ValueNode& node, size_t depth)
{
	if(node.GetSize() != 0)
		for(const auto& n : node)
		{
			WritePrefix(f, depth);
			f << '(' << '\n';
			TryExpr(WriteNode(f, n, depth + 1))
			CatchIgnore(std::out_of_range&)
			WritePrefix(f, depth);
			f << ')' << '\n';
		}
	return f;
}
#endif

//! \since build 449
bool
PrintNodeString(File& f, const ValueNode& node)
{
	try
	{
		const auto& s(Access<string>(node));

		f << '"' << EscapeLiteral(s) << '"' << '\n';
		return true;
	}
	CatchIgnore(ystdex::bad_any_cast&)
	return {};
}

//! \since build 334
File&
WriteNodeC(File& f, const ValueNode& node, size_t depth)
{
	WritePrefix(f, depth);
	f << EscapeLiteral(node.GetName());
	if(node)
	{
		f << ' ';
		if(PrintNodeString(f, node))
			return f;
		f << '\n';
		for(const auto& n : node)
		{
			WritePrefix(f, depth);
			if(IsPrefixedIndex(n.GetName()))
				PrintNodeString(f, n);
			else
			{
				f << '(' << '\n';
				TryExpr(WriteNodeC(f, n, depth + 1))
				CatchIgnore(std::out_of_range&)
				WritePrefix(f, depth);
				f << ')' << '\n';
			}
		}
	}
	return f;
}

} // unnamed namespace;

File&
operator<<(File& f, const Configuration& conf)
{
	return WriteNodeC(f, conf.GetRoot(), 0);
}

TextFile&
operator>>(TextFile& tf, Configuration& conf)
{
	tf.Rewind();
	conf.root = LoadNPLA1(SContext::Analyze(Session(tf)));
	return tf;
}

} // namespace NPL;

