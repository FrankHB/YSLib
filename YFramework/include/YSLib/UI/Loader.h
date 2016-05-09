/*
	© 2013-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Loader.h
\ingroup UI
\brief 动态 GUI 加载。
\version r663
\author FrankHB <frankhb1989@gmail.com>
\since build 433
\par 创建时间:
	2013-08-01 20:37:16 +0800
\par 修改时间:
	2016-05-09 13:28 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Loader
*/


#ifndef YSL_INC_UI_Loader_h_
#define YSL_INC_UI_Loader_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YUIContainer
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
\todo 使用 ISO C++14 多态 lambda 表达式代替。
*/
template<typename _tWidget, typename... _tParams>
static unique_ptr<IWidget>
CreateUniqueWidget(_tParams&&... args)
{
	return make_unique<_tWidget>(yforward(args)...);
}

/*!
\brief 注册部件加载器：添加部件引用。
\since build 496
\todo 使用 ISO C++14 多态 lambda 表达式代替。
*/
template<typename _tWidget, typename... _tParams>
static void
InsertWidget(IWidget& wgt, _tParams&&... args)
{
	UI::AddWidget(ystdex::polymorphic_downcast<_tWidget&>(wgt),
		yforward(args)...);
}


/*!
\brief 检查是否为元数据名称。
\pre 断言：参数的数据指针非空。
\since build 659
*/
inline PDefH(bool, CheckChildName, string_view sv)
	ImplRet((YAssertNonnull(sv.data()), !sv.empty() && sv.front() != '$'))


/*!
\ingroup exceptions
\brief 找不到部件。
\since build 495
*/
class YF_API WidgetNotFound : public LoggedEvent
{
public:
	string NodeName;

	/*!
	\pre 间接断言：字符串参数对应的数据指针非空。
	\since build 659
	*/
	YB_NONNULL(3)
	WidgetNotFound(string_view name, const char* s,
		RecordLevel lv = Warning)
		: LoggedEvent(s, lv),
		NodeName((Nonnull(name.data()), name))
	{}
	WidgetNotFound(string_view name, const string_view sv,
		RecordLevel lv = Warning)
		: LoggedEvent(sv, lv),
		NodeName((Nonnull(name.data()), name))
	{}

	//! \since build 586
	DefDeCopyCtor(WidgetNotFound)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~WidgetNotFound() override;
};


/*
\throw WidgetNotFound 没有找到指定名称的部件。
\note 不抛出 std::out_of_range （已全部捕获并抛出为 WidgetNotFound ）。
\since build 667
*/
//@{
//! \brief 按指定名称访问子部件节点。
//@{
yconstfn PDefH(ValueNode&, AccessWidgetNode, ValueNode& nd)
	ImplRet(nd)
template<typename... _tParams>
ValueNode&
AccessWidgetNode(ValueNode& node, const string& name, _tParams&&... args)
{
	TryRet(AccessWidgetNode(AccessNode(AccessNode(node, "$children"), name),
		yforward(args)...))
	CatchThrow(std::out_of_range&,
		WidgetNotFound(node.GetName(), "Widget children not found."))
}
//@}

/*!
\brief 按指定名称访问子部件。
\exception ystdex::bad_any_cast 异常中立：由 Access 抛出。
*/
//@{
YF_API IWidget&
AccessWidget(const ValueNode&);
template<typename... _tParams>
IWidget&
AccessWidget(ValueNode& node, const string& name, _tParams&&... args)
{
	return AccessWidget(AccessWidgetNode(node, name, yforward(args)...));
}
//! \throw std::bad_cast 不存在指定类型的部件。
template<class _tWidget, typename... _tParams>
_tWidget&
AccessWidget(ValueNode& node, _tParams&&... args)
{
	return dynamic_cast<_tWidget&>(AccessWidget(node, yforward(args)...));
}
//@}
//@}


//! \since build 494
template<typename... _tParams>
using GWidgetCreator = unique_ptr<IWidget>(*)(_tParams...);

//! \since build 496
template<typename... _tParams>
using GWidgetInserter = void(*)(IWidget&, _tParams...);


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
\since build 496
\par 调用示例:
\code
	static GWidgetInserterRegister<IWidget&&> reg;

	reg.Register<Widget, Control, Panel>({"Widget", "Control", "Panel"});
\endcode
\brief 取加载器注册接口处理器：用于加载一个或多个键和类初始化例程。
*/
template<typename... _tParams>
class GWidgetInserterRegister : public GHandlerRegisterBase<
	GWidgetInserterRegister<_tParams...>, string, GWidgetInserter<_tParams...>>
{
public:
	using Base = GHandlerRegisterBase<GWidgetInserterRegister<_tParams...>,
		string, GWidgetInserter<_tParams...>>;

	ImplS(Base) template<typename _tWidget, typename _fHandler>
	static GWidgetInserter<_tParams...>
	GetRegister(const string&)
	{
		return &InsertWidget<_tWidget, _tParams...>;
	}
};


/*!
\brief 部件加载器。
\since build 434
*/
class YF_API WidgetLoader
{
public:
	GWidgetRegister<> Default{};
	GWidgetRegister<const Rect&> Bounds{};
	//! \since build 495
	GWidgetInserterRegister<IWidget&> Insert{};
	//! \since build 555
	GWidgetInserterRegister<IWidget&, const ZOrder&> InsertZOrdered{};

	//! \exception std::out_of_range 异常中立：找不到内部类型节点。
	unique_ptr<IWidget>
	DetectWidgetNode(const ValueNode&);

	/*!
	\brief 加载 NPLA1 翻译单元，翻译后变换为 UI 布局树。
	\pre 间接断言：参数的数据指针非空。
	\sa TransformUILayout
	\since build 659
	*/
	ValueNode
	LoadUILayout(string_view);

	/*!
	\brief 变换 UI 布局树：根据 NPLA1 中间表示动态创建部件树。
	\exception std::out_of_range 异常中立：找不到 $type 节点。
	\exception ystdex::bad_any_cast 异常中立：$type 节点的值不是 string 类型。
	\sa CheckChildName

	参数指定的树中，使用以下元数据名称的节点表示数据：
	$type 指定部件的类型；
	$pointer 使用 shared_ptr<IWidget> 类型的值指定部件；
	可选 $z 指定 Z 顺序值。
	*/
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
	//! \since build 554
	lref<WidgetLoader> Loader;
	ValueNode WidgetNode;

	/*!
	\pre 间接断言：参数的数据指针非空。
	\since build 659
	*/
	DynamicWidget(WidgetLoader& ldr, string_view sv)
		: Loader(ldr), WidgetNode(ldr.LoadUILayout(sv))
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

