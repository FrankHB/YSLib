/*
	© 2011-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file menu.h
\ingroup UI
\brief 样式相关的菜单。
\version r955
\author FrankHB <frankhb1989@gmail.com>
\since build 203
\par 创建时间:
	2011-06-02 12:17:38 +0800
\par 修改时间:
	2014-11-15 00:59 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Menu
*/


#ifndef YSL_INC_UI_menu_h_
#define YSL_INC_UI_menu_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_ListControl
#include YFM_YSLib_UI_YUIContainer // for UI::ZOrderType;
#include <ystdex/cast.hpp> // for ystdex::polymorphic_downcast;

namespace YSLib
{

namespace UI
{

class MenuHost;

yconstexpr const ZOrderType DefaultMenuZOrder(224); //!< 默认菜单 Z 顺序值。


/*!
\brief 菜单。
\note LabelBrush 非强异常安全：不抛出异常时保持每次绘制最后项目的状态。
\since build 252
\todo 允许表示非文本内容。
*/
class YF_API Menu : public TextList
{
	friend class MenuHost;

public:
	using SubMap = map<IndexType, Menu*>; //!< 子菜单映射表类型。
	using ValueType = SubMap::value_type; //!< 子菜单映射表项目类型。

protected:
	MenuHost* pHost; //!< 宿主指针。
	Menu* pParent; //!< 父菜单指针。
	SubMap mSubMenus; //!< 子菜单映射表：存储非空子菜单指针。
	mutable vector<bool> vDisabled; //!< 未启用菜单项。

public:
	/*!
	\brief 构造：使用指定边界、文本列表和菜单标识。
	\since build 537
	*/
	explicit
	Menu(const Rect& = {}, const shared_ptr<ListType>& = {});
	/*!
	\brief 禁止转移构造。
	\since build 379
	\todo 实现转移操作。
	*/
	DefDelMoveCtor(Menu)

	/*!
	\brief 向子菜单组添加关联索引和指针指定的菜单。
	\note 若索引越界或菜单指针为空则忽略。
	*/
	void
	operator+=(const ValueType&);

	/*!
	\brief 向菜单组移除指定子菜单索引项。
	\note 若索引越界或不存在则忽略。
	*/
	bool
	operator-=(IndexType);

	/*!
	\brief 判断菜单项是否有效。
	\pre 断言： <tt>IsInInterval(idx, GetList().size())</tt> 。
	\post <tt>vDisabled.size() == GetList().size()</tt> 。
	*/
	bool
	IsItemEnabled(ListType::size_type) const;

	DefGetter(const ynothrow, Menu*, ParentPtr, pParent)

	/*!
	\brief 设置 idx 指定的菜单项的可用性。
	\pre 断言： <tt>IsInInterval(idx, GetList().size())</tt> 。
	\post <tt>vDisabled.size() == GetList().size()</tt> ；
		IsItemEnabled(idx) 。
	*/
	void
	SetItemEnabled(ListType::size_type idx, bool = true);

protected:
	/*!
	\brief 调整 vDisabled 大小。
	\post <tt>vDisabled.size() == GetList().size()</tt> 。
	*/
	void
	AdjustSize() const;

public:
	/*!
	\brief 按指定 Z 顺序显示菜单。
	\note 菜单宿主指针为空时忽略。
	\return 菜单宿主指针非空。
	*/
	bool
	Show(ZOrderType = DefaultMenuZOrder);

	/*!
	\brief 按指定 Z 顺序显示索引指定的子菜单。
	\note 菜单宿主指针为空时忽略。
	\return 菜单宿主指针非空且索引指定的子菜单存在时为子菜单指针，否则为空指针。
	*/
	Menu*
	ShowSub(IndexType, ZOrderType = DefaultMenuZOrder);

	/*!
	\brief 隐藏菜单。
	\note 菜单宿主指针为空时忽略。
	*/
	bool
	Hide();

	/*!
	\brief 访问索引指定的子菜单。
	\exception std::out_of_range 异常中立：由 at 抛出。
	\since build 537
	*/
	PDefH(Menu&, at, size_t idx)
		ImplRet(*mSubMenus.at(idx))
};

/*!
\brief 定位菜单：以第二个参数作为参考父菜单，按指定参考偏移索引定位菜单。
\relates Menu
*/
YF_API void
LocateMenu(Menu&, const Menu&, Menu::IndexType);


/*!
\brief 菜单宿主。
\warning 非虚析构。
\since build 252
*/
class YF_API MenuHost : private noncopyable
{
public:
	Window& Frame; //!< 框架窗口。

private:
	//! \since build 554
	set<lref<Menu>, ystdex::composed<std::less<Menu*>, ystdex::composed<
		ystdex::addressof_op<Menu>, ystdex::mem_get<lref<Menu>>>>> menus{};

public:
	/*!
	\brief 根菜单关联映射。
	\since build 537

	指定向指定部件转移焦点时不进行隐藏的菜单的映射。
	*/
	map<IWidget*, Menu*> Roots{};

	MenuHost(Window&);
	/*!
	\brief 析构。
	\note 隐藏菜单组中的所有菜单并清理菜单组。
	*/
	~MenuHost();

	/*!
	\brief 向菜单组添加菜单。
	\note 标识由菜单对象的菜单标识成员指定；若菜单项已存在则覆盖旧菜单项。
	*/
	void
	operator+=(Menu&);

	/*!
	\brief 从菜单组移除指定的菜单。
	\note 同时置菜单宿主指针为空。
	\since build 537
	*/
	bool
	operator-=(Menu&);

	/*!
	\brief 判断窗口中是否正在显示指定的菜单。
	\since build 537
	*/
	bool
	IsShowing(Menu&) const;

	/*!
	\brief 判断菜单组中是否存在指定的菜单。
	\since build 531
	*/
	PDefH(bool, Contains, Menu& mnu) const
		ImplRet(ystdex::exists(menus, ystdex::ref(mnu)))

	/*!
	\brief 清除菜单组。
	\note 释放所有的菜单对象。
	*/
	void
	Clear();

	/*!
	\brief 按指定 Z 顺序显示菜单组中指定的菜单。
	\note 若不在菜单组中则忽略。
	\since build 537
	*/
	void
	Show(Menu&, ZOrderType = DefaultMenuZOrder);

	/*!
	\brief 按指定 Z 顺序显示菜单组中的所有菜单。
	*/
	void
	ShowAll(ZOrderType = DefaultMenuZOrder);

private:
	/*!
	\brief 按指定 Z 顺序显示指定菜单 mnu 。
	\pre 断言： Contains(mnu) 。
	*/
	void
	ShowRaw(Menu& mnu, ZOrderType = DefaultMenuZOrder);

public:
	/*!
	\brief 隐藏菜单组中指定的菜单。
	\since build 537
	*/
	void
	Hide(Menu&);

	/*!
	\brief 隐藏菜单组中的所有菜单。
	*/
	void
	HideAll();

private:
	/*!
	\brief 隐藏指定菜单 mnu。
	\pre 断言： Contains(mnu) 。
	*/
	void
	HideRaw(Menu& mnu);

public:
	/*!
	\brief 隐藏从 mnu 起向上层遍历菜单树的过程中不相关的菜单。
	\note 相关菜单指 mnu 的父菜单中的 mnuParent 及其直接或间接父菜单。
	\note 若 <tt>!Contains(mnuParent)</tt>
		或 mnuParent 不是 mnu 的直接或间接父菜单，则隐藏所有菜单。
	*/
	void
	HideUnrelated(Menu& mnu, Menu& mnuParent);
};

} // namespace UI;

} // namespace YSLib;

#endif

