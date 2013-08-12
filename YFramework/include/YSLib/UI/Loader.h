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
\brief 动态 GUI 加载。
\version r357
\author FrankHB <frankhb1989@gmail.com>
\since build 433
\par 创建时间:
	2013-08-01 20:37:16 +0800
\par 修改时间:
	2013-08-10 19:00 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Loader
*/


#ifndef YSL_INC_UI_Loader_h_
#define YSL_INC_UI_Loader_h_ 1

#include "ywidget.h"
#include "../Core/ValueNode.h"

namespace YSLib
{

namespace UI
{

//! \since build 433
YF_API Rect
ParseRect(const string&);


/*!
\brief 注册部件加载器。
\since build 433
\todo 使用 ISO C++1y 多态 lambda 表达式代替。
*/
template<typename _tWidget, typename... _tParams>
static unique_ptr<IWidget>
CreateUniqueWidget(_tParams&&... args)
{
	return make_unique<_tWidget>(yforward(args)...);
}


//! \since build 432
inline bool
CheckChildName(const string& str)
{
	return str.size() != 0 && str[0] != '$';
}

//! \since build 433
//@{
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


//! \since build 434
template<typename... _tParams>
using GWidgetCreatorMap
	= unordered_map<string, unique_ptr<IWidget>(*)(_tParams...)>;


/*!
\brief 加载器注册接口：加载一个或多个键和类初始化例程。
\note 加载的键的数量和类的数量需要保持一致。
\since build 434
\par 调用示例:
\code
	static GWidgetRegister<> reg;

	reg.Register<Widget, Control, Panel>({"Widget", "Control", "Panel"});
\endcode
*/
template<typename... _tParams>
struct GWidgetRegister
{
private:
	GWidgetCreatorMap<_tParams...> wgt_map;

public:
	unique_ptr<IWidget>
	CreateWidget(const string& type_str, _tParams&&... args)
	{
		if(const auto f = wgt_map[type_str])
		{
			YTraceDe(Notice, "Found widget creator: %s.\n", type_str.c_str());

			return f(yforward(args)...);
		}
		return {};
	}

	template<class _tWidget>
	void
	Register(const string& key)
	{
		wgt_map.emplace(key, &CreateUniqueWidget<_tWidget, _tParams...>);
	}
	template<typename _tIn, class _tWidget, class _tTuple>
	void
	Register(_tIn first, _tIn last)
	{
		YAssert(first != last && std::distance(first, last)
			== std::tuple_size<_tTuple>::value + 1, "Wrong range found.");

		Register<_tWidget>(*first);
		++first;

		YAssert((first == last) == (std::tuple_size<_tTuple>::value == 0),
			"Wrong number of parameters found.");

	//	static_if(std::tuple_size<_tTuple>::value != 0)
	//		RegisterTail<_tIn, std::tuple_element<0, _tTuple>,
	//			typename tuple_split<_tTuple>::tail>(first, last);
		RegisterTail<_tIn>(static_cast<_tTuple*>(nullptr), first, last);
	}
	template<class _tWidget, class... _tWidgets>
	void
	Register(std::initializer_list<string> il)
	{
		YAssert(il.size() == sizeof...(_tWidgets) + 1,
			"Wrong size of initializer list found.");

		Register<std::initializer_list<string>::const_iterator, _tWidget,
			tuple<_tWidgets...>>(il.begin(), il.end());
	}

private:
	template<typename _tIn>
	void
	RegisterTail(tuple<>*, _tIn first, _tIn last)
	{
		YAssert(first == last, "Wrong size of initializer list found.");

		yunused(first),
		yunused(last);
	}
	template<typename _tIn, class _tWidget, class... _tWidgets>
	void
	RegisterTail(tuple<_tWidget, _tWidgets...>*, _tIn first, _tIn last)
	{
		Register<_tIn, _tWidget, tuple<_tWidgets...>>(first, last);
	}
};


/*!
\brief 部件加载器。
\since build 434
*/
class YF_API WidgetLoader
{
public:
	GWidgetRegister<> Default;
	GWidgetRegister<const Rect&> Bounds;

	unique_ptr<IWidget>
	DetectWidgetNode(const ValueNode&);

	ValueNode
	LoadUILayout(const string&);

	ValueNode
	TransformUILayout(const ValueNode&);
};


/*!
\brief 动态部件。
\since build 434
*/
class YF_API DynamicWidget
{
public:
	std::reference_wrapper<WidgetLoader> Loader;
	ValueNode WidgetNode;

	DynamicWidget(WidgetLoader& ldr, const string& str)
		: Loader(ldr), WidgetNode(ldr.LoadUILayout(str))
	{}
};


/*!
\brief 声明动态部件。
\since build 436
*/
#define DeclDynWidget(_t, _n, ...) \
	auto& _n(YSLib::UI::AccessWidget<_t>(__VA_ARGS__));
/*!
\brief 声明指定节点下的按相同名称访问的动态部件。
\note ... 展开为参数列表，其中第一个为节点名称，之后的可选为名称字符串参数。
\since build 436
*/
#define DeclDynWidgetN(_t, _n, ...) \
	DeclDynWidget(_t, _n, __VA_ARGS__, #_n)
/*!
\brief 声明名称为 node 的节点下的按相同名称访问的动态部件。
\since build 436
*/
#define DeclDynWidgetNode(_t, _n) DeclDynWidgetN(_t, _n, node)

} // namespace UI;

} // namespace YSLib;

#endif

