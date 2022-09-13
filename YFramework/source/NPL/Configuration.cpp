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
\version r981
\author FrankHB <frankhb1989@gmail.com>
\since build 334
\par 创建时间:
	2012-08-27 15:15:06 +0800
\par 修改时间:
	2022-09-10 02:11 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::Configuration
*/


#include "NPL/YModules.h"
#include YFM_NPL_Configuration // for PrintNode, LiteralizeEscapeNodeLiteral;
#include YFM_NPL_SContext // for Session, SContext::Analyze;
#include <iterator> // for std::istreambuf_iterator;
#include <ystdex/ios.hpp> // for ystdex::rethrow_badstate,
//	std::ios_base::failbit;

namespace NPL
{

namespace A1
{

ValueNode
LoadNode(const TermNode& tree)
{
	TryRet(A1::TransformNode(tree))
	CatchThrow(bad_any_cast& e, LoggedEvent(YSLib::sfmt(
		"Bad NPLA1 tree found: cast failed from [%s] to [%s] .", e.from(),
		e.to()), YSLib::Warning))
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
	using sb_it_t = std::istreambuf_iterator<char>;
	// TODO: Validate for S-expression?
	const auto a(conf.get_allocator());
	Session sess(a);

	TryExpr(conf.root = A1::LoadNode(SContext::Analyze(std::allocator_arg, a,
		sess, sess.Process(sb_it_t(is), sb_it_t{}))))
	CatchExpr(..., ystdex::rethrow_badstate(is, std::ios_base::failbit))
	return is;
}

} // namespace NPL;

