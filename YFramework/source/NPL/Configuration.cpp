/*
	© 2012-2016, 2020-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Configuration.cpp
\ingroup NPL
\brief 配置设置。
\version r1048
\author FrankHB <frankhb1989@gmail.com>
\since build 334
\par 创建时间:
	2012-08-27 15:15:06 +0800
\par 修改时间:
	2022-09-23 02:20 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::Configuration
*/


#include "NPL/YModules.h"
#include YFM_NPL_Configuration // for ystdex::value_or, AccessPtr, string,
//	ystdex::quote, YSLib::AsNode, Deliteralize, PrintNode,
//	LiteralizeEscapeNodeLiteral;
#include YFM_NPL_NPLA1Forms // for A1, Forms, RegisterStrict;
#include YFM_NPL_Dependency // for LoadGroundContext;
#include <ystdex/ios.hpp> // for ystdex::rethrow_badstate,
//	std::ios_base::failbit;

namespace NPL
{

//! \since build 956
namespace
{

YB_ATTR_nodiscard YB_PURE inline string
ParseNPLATermName(const TermNode& term)
{
	if(const auto p = AccessPtr<string>(term))
		return ystdex::quote(*p);
	return ystdex::value_or(AccessPtr<TokenValue>(term));
}

YB_ATTR_nodiscard YB_PURE ValueNode
TransformNode(const TermNode& term)
{
	auto s(term.size());
	const ValueNode::allocator_type a(term.get_allocator());

	if(s == 0)
		return YSLib::AsNode(a, string(),
			string(Deliteralize(ystdex::value_or(AccessPtr<string>(term)))));

	auto i(term.begin());

	if(s == 1)
		return TransformNode(*i);

	const auto& name(ParseNPLATermName(*i));

	if(!name.empty())
		yunseq(++i, --s);
	if(s == 1)
	{
		auto&& nd(TransformNode(*i));

		if(nd.GetName().empty())
			return YSLib::AsNode(a, name, std::move(nd.Value));

		ValueNode::Container node_con(a);

		node_con.insert(std::move(nd));
		return {ValueNode::Container(std::move(node_con), a), name};
	}

	ValueNode::Container node_con(a);

	std::for_each(i, term.end(), [&](const TermNode& tm){
		auto&& node(TransformNode(tm));

		node_con.insert(node.GetName().empty() ? YSLib::AsNode(
			node.get_allocator(), '$' + MakeIndex(node_con.size()),
			std::move(node.Value)) : std::move(node));
	});
	return {std::allocator_arg, a, std::move(node_con), name};
}

} // unnamed namespace;

namespace A1
{

NodeLoader::NodeLoader(const GlobalState& global)
	: cs(global)
{}

ValueNode
NodeLoader::LoadNode(const TermNode& tree) const
{
	TryRet(TransformNode(tree))
	CatchThrow(bad_any_cast& e, LoggedEvent(YSLib::sfmt("Bad NPLA1 tree found:"
		" cast failed from [%s] to [%s] .", e.from(), e.to()), YSLib::Warning))
	catch(std::exception& e)
	{
		YTraceDe(YSLib::Err, "Bad NPLA1 tree found: [%s]%s",
			typeid(e).name(), e.what());
		throw;
	}
}

} // namespace A1;

std::ostream&
operator<<(std::ostream& os, const Configuration& conf)
{
	PrintNode(os, conf.GetRoot(), LiteralizeEscapeNodeLiteral);
	return os;
}

std::istream&
operator>>(std::istream& is, Configuration& conf)
{
	TryExpr(conf.root
		= A1::NodeLoader(A1::GlobalState(conf.get_allocator())).LoadNode(is))
	CatchExpr(..., ystdex::rethrow_badstate(is, std::ios_base::failbit))
	return is;
}

} // namespace NPL;

