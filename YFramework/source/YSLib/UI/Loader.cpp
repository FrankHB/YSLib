/*
	© 2013-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Loader.cpp
\ingroup UI
\brief 动态 GUI 加载。
\version r330
\author FrankHB <frankhb1989@gmail.com>
\since build 433
\par 创建时间:
	2013-08-01 20:39:49 +0800
\par 修改时间:
	2014-12-12 22:49 +0800
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
	Rect res;

	try
	{
		std::istringstream iss(str);

		iss.exceptions(std::ios::failbit | std::ios::badbit);
		iss >> res.X >> res.Y >> res.Width >> res.Height;
	}
	catch(std::exception& e)
	{
		YTraceDe(Warning, "Error: %s", e.what());
		throw std::invalid_argument("Parse 'Rect' failed: bad state.");
	}
	YTraceDe(Debug, "ParseRect: %s.", to_string(res).c_str());
	return res;
}


ImplDeDtor(WidgetNotFound)


IWidget&
AccessWidget(const ValueNode& node)
{
	TryRet(Deref(AccessChild<shared_ptr<IWidget>>(node, "$pointer")))
	CatchThrow(std::out_of_range&,
		WidgetNotFound(node.GetName(), "Widget pointer not found."))
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
			CatchIgnore(std::invalid_argument&)
		return Default.Call(type_str);
	}
	CatchIgnore(ystdex::bad_any_cast&)
	return {};
}

ValueNode
WidgetLoader::LoadUILayout(string_view sv)
{
	return TransformUILayout(NPL::LoadNPLA1(NPL::SContext::Analyze(sv)));
}

ValueNode
WidgetLoader::TransformUILayout(const ValueNode& node)
{
	if(unique_ptr<IWidget> p_new_widget{DetectWidgetNode(node)})
	{
		ValueNode res(0, node.GetName());
		const auto& key(AccessChild<string>(node, "$type"));
		const bool ins(Insert.Contains(key));
		const bool insz(InsertZOrdered.Contains(key));

		if(ins || insz)
		{
			ValueNode::Container node_con;

			for(const auto& vn : node)
				if(CheckChildName(vn.GetName()))
					try
					{
						if(ValueNode child{TransformUILayout(vn)})
						{
							auto& wgt(*AccessChild<shared_ptr<IWidget>>(child,
								"$pointer"));
							const auto p_z(AccessChildPtr<string>(vn, "$z"));
							auto z(DefaultZOrder);

							if(insz && p_z)
								try
								{
									const auto r(std::stoul(*p_z));

									// XXX: Do not use magic number.
									if(r < 0x100)
										z = r;
								}
								CatchIgnore(std::invalid_argument&)
							if(node_con.insert(std::move(child)).second)
							{
								if(insz && (p_z || !ins))
									InsertZOrdered.Call(key, *p_new_widget, wgt,
										z);
								else
									Insert.Call(key, *p_new_widget, wgt);
							}
						}
					}
					CatchIgnore(ystdex::bad_any_cast&)
			res += {std::move(node_con), "$children"};
		}
		res += {0, "$pointer", shared_ptr<IWidget>(std::move(p_new_widget))};
		return res;
	}
	return {};
}

} // namespace UI;

} // namespace YSLib;

