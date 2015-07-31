/*
	© 2012-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Configuration.cpp
\ingroup NPL
\brief 配置设置。
\version r935
\author FrankHB <frankhb1989@gmail.com>
\since build 334
\par 创建时间:
	2012-08-27 15:15:06 +0800
\par 修改时间:
	2015-07-30 22:28 +0800
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

std::ostream&
operator<<(std::ostream& os, const Configuration& conf)
{
	PrintNode(os, conf.GetRoot(), LiteralizeEscapeNodeLiteral);
	return os;
}

std::istream&
operator>>(std::istream& is, Configuration& conf)
{
	Session sess;
	char c;

	while((c = is.get()), is)
		Session::DefaultParseByte(sess.Lexer, c);
	conf.root = LoadNPLA1(SContext::Analyze(std::move(sess)));
	return is;
}

} // namespace NPL;

