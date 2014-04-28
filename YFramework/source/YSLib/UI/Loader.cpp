/*
	© 2013-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Loader.cpp
\ingroup UI
\brief 动态 GUI 加载。
\version r250
\author FrankHB <frankhb1989@gmail.com>
\since build 433
\par 创建时间:
	2013-08-01 20:39:49 +0800
\par 修改时间:
	2014-04-27 23:45 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Loader
*/


#include "YSLib/UI/YModules.h"
#include "NPL/YModules.h"
#include YFM_YSLib_UI_Loader
#include <sstream>
#include YFM_YSLib_UI_YPanel
#include YFM_NPL_SContext
#include YFM_NPL_Configuration

namespace YSLib
{

namespace UI
{

Rect
ParseRect(const string& str)
{
	std::istringstream iss(str);
	int buf[4];

	for(size_t i(0); i < 4; ++i)
		if(iss)
			iss >> buf[i];
		else
			throw std::invalid_argument("Parse 'Rect' failed: bad state.");
	// FIXME: Complete max value checking.
	if(buf[2] < 0 || buf[3] < 0)
		throw std::invalid_argument("Parse 'Rect' failed: underflow.");

	Rect res(buf[0], buf[1], buf[2], buf[3]);

	YTraceDe(Informative, "ParseRect: %s.\n", to_string(res).c_str());

	return res;
}


IWidget&
AccessWidget(const ValueNode& node)
{
	try
	{
		const auto& p(AccessChild<shared_ptr<IWidget>>(node, "$pointer"));

		YAssertNonnull(p);

		return *p;
	}
	catch(std::out_of_range&)
	{
		throw WidgetNotFound(node.GetName(), "Widget pointer not found.");
	}
}


unique_ptr<IWidget>
WidgetLoader::DetectWidgetNode(const ValueNode& node)
{
	try
	{
		const auto& type_str(AccessChild<string>(node, "$type"));

		if(const auto* p_bounds_str = AccessChildPtr<string>(node, "$bounds"))
			try
			{
				const Rect& bounds(ParseRect(*p_bounds_str));

				return Bounds.Call(type_str, bounds);
			}
			catch(std::invalid_argument&)
			{}
		return Default.Call(type_str);
	}
	catch(ystdex::bad_any_cast&)
	{}
	return {};
}

ValueNode
WidgetLoader::LoadUILayout(const string& str)
{
	return TransformUILayout(
		NPL::LoadNPLA1(NPL::SContext::Analyze(NPL::Session(str))));
}

ValueNode
WidgetLoader::TransformUILayout(const ValueNode& node)
{
	if(unique_ptr<IWidget> p_new_widget{DetectWidgetNode(node)})
	{
		ValueNode res(0, node.GetName());
		const auto& key(AccessChild<string>(node, "$type"));

		if(Insert.Contains(key))
		{
			auto p_cont(make_unique<ValueNode::Container>());

			for(const auto& vn : node)
				if(CheckChildName(vn.GetName()))
					try
					{
						if(ValueNode child{TransformUILayout(vn)})
						{
							auto& wgt(*AccessChild<shared_ptr<IWidget>>(child,
								"$pointer"));

							if(p_cont->insert(std::move(child)).second)
								Insert.Call(key, *p_new_widget, wgt);
						}
					}
					catch(ystdex::bad_any_cast&)
					{}
			res += {0, "$children", std::move(p_cont), PointerTag()};
		}
		res += {0, "$pointer", shared_ptr<IWidget>(std::move(p_new_widget))};
		return res;
	}
	return {};
}

} // namespace UI;

} // namespace YSLib;

