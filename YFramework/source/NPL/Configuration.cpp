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
\version r838
\author FrankHB <frankhb1989@gmail.com>
\since build 334
\par 创建时间:
	2012-08-27 15:15:06 +0800
\par 修改时间:
	2015-05-16 19:10 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::Configuration
*/


#include "NPL/YModules.h"
#include YFM_NPL_Configuration
#include YFM_NPL_SContext
#include <sstream> // for std::ostringstream;

using namespace YSLib;

namespace NPL
{

File&
operator<<(File& f, const Configuration& conf)
{
	std::ostringstream oss;

	PrintNode(oss, conf.GetRoot(), LiteralizeEscapeNodeLiteral);
	f << oss.str();
	return f;
}

TextFile&
operator>>(TextFile& tf, Configuration& conf)
{
	tf.Rewind();
	conf.root = LoadNPLA1(SContext::Analyze(Session(tf)));
	return tf;
}

} // namespace NPL;

