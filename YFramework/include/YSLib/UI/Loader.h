/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Loader.h
\ingroup UI
\brief 动态 UI 加载。
\version r159
\author FrankHB <frankhb1989@gmail.com>
\since build 433
\par 创建时间:
	2013-08-01 20:37:16 +0800
\par 修改时间:
	2013-08-02 01:11 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Loader
*/


#ifndef YSL_INC_UI_Loader_h_
#define YSL_INC_UI_Loader_h_ 1

#include "ywidget.h"
#include "../Core/ValueNode.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(UI)

//! \since build 433
YF_API Rect
ParseRect(const string&);

//! \since build 432
//@{
template<typename _fCreator>
using GWidgetCreatorMap = unordered_map<string, _fCreator>;


template<typename _fCreator>
GWidgetCreatorMap<_fCreator>&
FetchWidgetMapping()
{
	static GWidgetCreatorMap<_fCreator> widget_map;

	return widget_map;
}


/*!
\brief 显式实例化默认使用的映射。
\note 对于 DLL 是必要的，因为模板不能直接指定静态局部对象的同一性。
\since build 433
*/
//@{
extern template YF_API GWidgetCreatorMap<unique_ptr<IWidget>(*)()>&
FetchWidgetMapping();
extern template YF_API GWidgetCreatorMap<unique_ptr<IWidget>(*)(const Rect&)>&
FetchWidgetMapping();
//@}


template<class _tWidget>
class GWidgetFactory
{
private:
	template<typename... _tParams>
	static unique_ptr<IWidget>
	CreateWidget(_tParams&&... args)
	{
		return make_unique<_tWidget>(yforward(args)...);
	}

	template<typename... _tParams>
	static int
	Register(const string& key)
	{
		FetchWidgetMapping<unique_ptr<IWidget>(*)(_tParams...)>().emplace(
			key, &CreateWidget<_tParams...>);
		return 0;
	}

public:
	GWidgetFactory(const string& key)
	{
		static int create_def(Register<>(key));
		static int create_bounds(Register<const Rect&>(key));

		static_cast<void>(create_def);
		static_cast<void>(create_bounds);
	}
};


template<typename... _tParams>
unique_ptr<IWidget>
CreateWidget(const string& type_str, _tParams&&... args)
{
	if(const auto f = FetchWidgetMapping<unique_ptr<IWidget>(*)(_tParams...)>()[
		type_str])
	{
		YTraceDe(Notice, "Found widget creator: %s.\n", type_str.c_str());

		return f(yforward(args)...);
	}
	return {};
}
//@}


//! \since build 433
YF_API unique_ptr<IWidget>
DetectWidgetNode(const ValueNode&);

//! \since build 432
inline bool
CheckChildName(const string& str)
{
	return str.size() != 0 && str[0] != '$';
}

//! \since build 433
//@{
YF_API ValueNode
TransformUILayout(const ValueNode&);

YF_API ValueNode
ConvertUILayout(const string&);


YF_API IWidget&
AccessWidget(const ValueNode&);
template<typename... _tParams>
IWidget&
AccessWidget(const ValueNode& node, const string& name, _tParams&&... args)
{
	return AccessWidget(node.at("$children").at(name), yforward(args)...);
}
template<class _tWidget, typename... _tParams>
_tWidget&
AccessWidget(const ValueNode& node, _tParams&&... args)
{
	return dynamic_cast<_tWidget&>(AccessWidget(node, yforward(args)...));
}
//@}

YSL_END_NAMESPACE(UI)

YSL_END

#endif

