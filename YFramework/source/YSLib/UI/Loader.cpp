/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Loader.cpp
\ingroup UI
\brief 动态 UI 加载。
\version r159
\author FrankHB <frankhb1989@gmail.com>
\since build 433
\par 创建时间:
	2013-08-01 20:39:49 +0800
\par 修改时间:
	2013-08-02 01:11 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Loader
*/


#include "YSLib/UI/Loader.h"
#include <sstream>
#include "YSLib/UI/ypanel.h"
#include "NPL/SContext.h"
#include "NPL/Configuration.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(UI)

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


template GWidgetCreatorMap<unique_ptr<IWidget>(*)()>&
FetchWidgetMapping();
template GWidgetCreatorMap<unique_ptr<IWidget>(*)(const Rect&)>&
FetchWidgetMapping();


unique_ptr<IWidget>
DetectWidgetNode(const ValueNode& node)
{
	try
	{
		const auto& type_str(AccessChild<string>(node, "$type"));

		if(const auto* p_bounds_str = AccessChildPtr<string>(node, "$bounds"))
			try
			{
				const Rect& bounds(ParseRect(*p_bounds_str));

				return CreateWidget(type_str, bounds);
			}
			catch(std::invalid_argument&)
			{}
		return CreateWidget(type_str);
	}
	catch(ystdex::bad_any_cast&)
	{}
	return {};
}


ValueNode
TransformUILayout(const ValueNode& node)
{
	if(unique_ptr<IWidget> p_new_widget{DetectWidgetNode(node)})
	{
		ValueNode res(0, node.GetName());

		if(auto p_pnl = dynamic_cast<Panel*>(p_new_widget.get()))
		{
			auto p_cont(make_unique<ValueNode::Container>());

			for(const auto& vn : node)
				if(CheckChildName(vn.GetName()))
					try
					{
						if(ValueNode child{TransformUILayout(vn)})
						{
							auto& p_wgt(*AccessChild<shared_ptr<IWidget>>(child,
								"$pointer"));

							if(p_cont->insert(std::move(child)).second)
								*p_pnl += p_wgt;
						}
					}
					catch(ystdex::bad_any_cast&)
					{}
			res += ValueNode{0, "$children", p_cont.release(), PointerTag()};
		}
		res += ValueNode{0, "$pointer",
			shared_ptr<IWidget>(std::move(p_new_widget))};
		return std::move(res);
	}
	return {};
}

ValueNode
ConvertUILayout(const string& str)
{
	using namespace NPL;
	ValueNode root;

	try
	{
		root = TransformConfiguration(SContext::Analyze(Session(str)));
	}
	catch(ystdex::bad_any_cast& e)
	{
		// TODO: Avoid memory allocation.
		throw LoggedEvent(ystdex::sfmt(
			"Bad configuration found: cast failed from [%s] to [%s] .",
			e.from(), e.to()), Warning);
	}
//	return std::move(root);
	return TransformUILayout(std::move(root));
}

IWidget&
AccessWidget(const ValueNode& node)
{
	const auto& p(AccessChild<shared_ptr<IWidget>>(node, "$pointer"));

	YAssert(bool(p), "Null pointer found.");

	return *p;
}

YSL_END_NAMESPACE(UI)

YSL_END

