/*
	© 2013-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Loader.h
\ingroup UI
\brief 动态 GUI 加载。
\version r513
\author FrankHB <frankhb1989@gmail.com>
\since build 433
\par 创建时间:
	2013-08-01 20:37:16 +0800
\par 修改时间:
	2014-04-27 23:38 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Loader
*/


#ifndef YSL_INC_UI_Loader_h_
#define YSL_INC_UI_Loader_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YWidget
#include YFM_YSLib_Core_ValueNode
#include <ystdex/cast.hpp> // for ystdex::polymorphic_downcast;

namespace YSLib
{

namespace UI
{

//! \since build 433
YF_API Rect
ParseRect(const string&);


/*!
\brief 注册部件加载器：创建部件。
\since build 433
\todo 使用 ISO C++1y 多态 lambda 表达式代替。
*/
template<typename _tWidget, typename... _tParams>
static unique_ptr<IWidget>
CreateUniqueWidget(_tParams&&... args)
{
	return make_unique<_tWidget>(yforward(args)...);
}

/*!
\brief 注册部件加载器：添加部件引用。
\since build 495
\todo 使用 ISO C++1y 多态 lambda 表达式代替。
*/
template<typename _tWidget, typename _tParam>
static void
InsertWidget(IWidget& wgt, _tParam&& arg)
{
	ystdex::polymorphic_downcast<_tWidget&>(wgt) += yforward(arg);
}


//! \since build 432
inline bool
CheckChildName(const string& str)
{
	return str.size() != 0 && str[0] != '$';
}


/*!
\ingroup exception_types
\brief 找不到部件。
\since build 495
*/
class YF_API WidgetNotFound : public LoggedEvent
{
public:
	string NodeName;

	WidgetNotFound(const string& name, const std::string& s,
		LevelType l = Warning)
		: LoggedEvent(s, l),
		NodeName(name)
	{}
};


/*!
\brief 按指定名称访问子部件节点。
\throw WidgetNotFound 没有找到指定名称的部件。
\note 不抛出 std::out_of_range （已全部捕获并抛出为 WidgetNotFound ）。
\since build 433
*/
//@{
YF_API IWidget&
AccessWidget(const ValueNode&);
template<typename... _tParams>
IWidget&
AccessWidget(const ValueNode& node, const string& name, _tParams&&... args)
{
	try
	{
		return AccessWidget(node.at("$children").at(name), yforward(args)...);
	}
	catch(std::out_of_range& e)
	{
		throw WidgetNotFound(node.GetName(), "Widget children not found.");
	}
}
//! \throw std::bad_cast 不存在指定类型的部件。
template<class _tWidget, typename... _tParams>
_tWidget&
AccessWidget(const ValueNode& node, _tParams&&... args)
{
	return dynamic_cast<_tWidget&>(AccessWidget(node, yforward(args)...));
}
//@}


//! \since build 494
template<typename... _tParams>
using GWidgetCreator = unique_ptr<IWidget>(*)(_tParams...);

//! \since build 495
template<typename _type>
using GWidgetInserter = void(*)(IWidget&, _type);


/*!
\brief 加载器注册接口：加载一个或多个键和类初始化例程。
\note 加载的键的数量和类的数量需要保持一致。
\sa GRegister
\since build 494
\par 调用示例:
\code
	static GWidgetRegister<> reg;

	reg.Register<Widget, Control, Panel>({"Widget", "Control", "Panel"});
\endcode
\brief 取加载器注册接口处理器：用于加载一个或多个键和类初始化例程。
*/
template<typename... _tParams>
class GWidgetRegister : public GHandlerRegisterBase<
	GWidgetRegister<_tParams...>, string, GWidgetCreator<_tParams...>>
{
public:
	using Base = GHandlerRegisterBase<GWidgetRegister<_tParams...>, string,
		GWidgetCreator<_tParams...>>;

	ImplS(Base) template<typename _tWidget, typename _fHandler>
	static GWidgetCreator<_tParams...>
	GetRegister(const string&)
	{
		return &CreateUniqueWidget<_tWidget, _tParams...>;
	}
};


/*!
\brief 加载器注册接口：加载一个或多个键和添加部件引用例程。
\note 加载的键的数量和类的数量需要保持一致。
\sa GRegister
\since build 495
\par 调用示例:
\code
	static GWidgetInserterRegister<IWidget&&> reg;

	reg.Register<Widget, Control, Panel>({"Widget", "Control", "Panel"});
\endcode
\brief 取加载器注册接口处理器：用于加载一个或多个键和类初始化例程。
*/
template<typename _type>
class GWidgetInserterRegister : public GHandlerRegisterBase<
	GWidgetInserterRegister<_type>, string, GWidgetInserter<_type>>
{
public:
	using Base = GHandlerRegisterBase<GWidgetInserterRegister<_type>,
		string, GWidgetInserter<_type>>;

	ImplS(Base) template<typename _tWidget, typename _fHandler>
	static GWidgetInserter<_type>
	GetRegister(const string&)
	{
		return &InsertWidget<_tWidget, _type>;
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
	//! \since build 495
	GWidgetInserterRegister<IWidget&> Insert;

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

