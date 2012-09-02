/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Configuration.cpp
\ingroup NPL
\brief 配置设置。
\version r367;
\author FrankHB<frankhb1989@gmail.com>
\since build 334 。
\par 创建时间:
	2012-08-27 15:15:06 +0800;
\par 修改时间:
	2012-09-02 20:08 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	NPL::Configuration;
*/


#include "NPL/Configuration.h"
#include "NPL/SContext.h"

using namespace YSLib;

YSL_BEGIN_NAMESPACE(NPL)

namespace
{

//! \since build 334 。
inline File&
WritePrefix(File& f, size_t n = 1, char c = '\t')
{
	while(n--)
		f << c;
	return f;
}

#if 0
//! \since build 334 。
File&
WriteNode(File& f, const ValueNode& node, size_t depth)
{
	WritePrefix(f, depth);
	if(node.GetSize() != 0)
	{
		++depth;
		for(const auto& n : node)
		{
			f << '(' << '\n';
			try
			{
				WriteNode(f, n.second, depth);
			}
			catch(std::out_of_range&)
			{}
			f << ')' << '\n';
		}
		--depth;
	}
	f << '\n';
	return f;
}
#endif

//! \since build 335 。
string
EscapeNodeString(const string& str)
{
	const char c(CheckLiteral(str));
	auto content(MakeEscape(c == char() ? str : ystdex::get_mid(str)));

	return c == char() ? std::move(content) : c + content + c;
}

//! \since build 334 。
File&
WriteNodeC(File& f, const ValueNode& node, size_t depth)
{
	WritePrefix(f, depth);
	if(node.GetSize() != 0)
	{
		++depth;
		for(const auto& pr : node)
		{
			f << '(' << '\n';
			try
			{
				WriteNodeC(f, pr.second, depth);
			}
			catch(std::out_of_range&)
			{}
			f << ')' << '\n';
		}
		--depth;
	}
	else
		f << node.GetName() << ' ' << EscapeNodeString(node.Access<string>());
	f << '\n';
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
	try
	{
		tf.Rewind();
		conf.root = TransformConfiguration(SContext::Analyze(Session(tf)));
	}
	catch(ystdex::bad_any_cast&)
	{
		throw LoggedEvent("Bad configuration found.", 0x80);
	}
	return tf;
}


ValueNode
TransformConfiguration(const ValueNode& node)
{
	const auto s(node.GetSize());

	if(s == 0)
		return node ? ValueNode(node.Access<string>()) : ValueNode();

	auto i(node.GetBegin());

	if(s == 1)
		return TransformConfiguration(i->second);
	if(i->second.GetSize() == 0)
	{
		const auto& new_name(i->second.Access<string>());

		++i;
		try
		{
			return ValueNode(new_name, i->second.Access<string>());
		}
		catch(ystdex::bad_any_cast&)
		{}
		return new_name;
	}

	ValueNode new_node;

	for(const auto& pr : node)
		new_node.Add(TransformConfiguration(pr.second));
	return new_node;
}

YSL_END_NAMESPACE(NPL)

