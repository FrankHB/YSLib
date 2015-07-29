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
\version r889
\author FrankHB <frankhb1989@gmail.com>
\since build 334
\par 创建时间:
	2012-08-27 15:15:06 +0800
\par 修改时间:
	2015-07-29 10:23 +0800
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

TextFile&
operator>>(TextFile& tf, Configuration& conf)
{
	if(tf)
	{
		tf.Rewind();

		using namespace ystdex;
		auto sentry(tf.GetSentry());
		auto& i(sentry.GetIteratorRef());
		Session sess;

		while(!tf.CheckEOF())
		{
			if(YB_UNLIKELY(is_undereferenceable(i)))
				throw LoggedEvent("Bad Source!", Critical);
			Session::DefaultParseByte(sess.Lexer, char(*i));
			++i;
		}
		conf.root = LoadNPLA1(SContext::Analyze(std::move(sess)));
		return tf;
	}
	throw GeneralEvent("Invalid file found when opening NPL session.");
}


ValueNode
ReadConfiguration(TextFile& tf)
{
	if(YB_LIKELY(tf))
	{
		YTraceDe(Debug, "Found accessible configuration file.");
		if(YB_UNLIKELY(tf.Encoding != Text::CharSet::UTF_8))
			throw GeneralEvent("Wrong encoding of configuration file.");

		NPL::Configuration conf;

		tf >> conf;
		YTraceDe(Debug, "Plain configuration loaded.");
		if(!conf.GetNodeRRef().empty())
			return conf.GetNodeRRef();
		YTraceDe(Warning, "Empty configuration found.");
	}
	throw GeneralEvent("Invalid file found when reading configuration.");
}

void
WriteConfiguration(TextFile& tf, const ValueNode& node)
{
	if(YB_UNLIKELY(!tf))
		throw GeneralEvent("Invalid file found when writing configuration.");
	YTraceDe(Debug, "Writing configuration...");
	tf << NPL::Configuration(ValueNode(node.GetContainerRef()));
	YTraceDe(Debug, "Writing configuration done.");
}

} // namespace NPL;

