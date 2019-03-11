/*
	© 2011-2016, 2018-2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Menu.h
\ingroup UI
\brief 样式相关的菜单。
\version r1071
\author FrankHB <frankhb1989@gmail.com>
\since build 573
\par 创建时间:
	2011-06-02 12:17:38 +0800
\par 修改时间:
	2019-03-05 22:20 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Menu
*/


#ifndef YSL_INC_UI_Menu_h_
#define YSL_INC_UI_Menu_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_ListControl
#include YFM_YSLib_UI_YUIContainer // for UI::ZOrder;
#include <ystdex/cast.hpp> // for ystdex::polymorphic_downcast;

namespace YSLib
{

namespace UI
{

class MenuHost;

/*!
\brief 默认菜单 Z 顺序值。
\since build 555
*/
yconstexpr const ZOrder DefaultMenuZOrder(224);


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
	//! \brief 子菜单映射表类型。
	using SubMap = map<IndexType, observer_ptr<Menu>>;
	//! \brief 子菜单映射表项目类型。
	using ValueType = SubMap::value_type;

protected:
	//! \since build 672
	//@{
	//! \brief 宿主指针。
	observer_ptr<MenuHost> pHost{};
	//! \brief 父菜单指针。
	observer_ptr<Menu> pParent{};
	//@}
	//! \brief 子菜单映射表：存储非空子菜单指针。
	SubMap mSubMenus{};
	//! \brief 未启用菜单项。
	mutable vector<bool> vDisabled;

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
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~Menu() override;

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

	//! \since build 672
	DefGetter(const ynothrow, observer_ptr<Menu>, ParentPtr, pParent)

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
	\brief 显示菜单。
	\note 菜单宿主指针为空时忽略。
	\return 菜单宿主指针非空。
	\since build 574
	*/
	bool
	Show();

	/*!
	\brief 显示索引指定的子菜单。
	\note 菜单宿主指针为空时忽略。
	\return 菜单宿主指针非空且索引指定的子菜单存在时为子菜单指针，否则为空指针。
	\since build 672
	*/
	observer_ptr<Menu>
	ShowSub(IndexType);

	/*!
	\brief 尝试显示索引指定的子菜单并选择子菜单的首个菜单项。
	\return 是否进行操作。
	\note 若不存在子菜单，不进行操作。
	\sa ShowSub
	\since build 854
	*/
	bool
	TryShowingSub(IndexType);

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
\brief 定位菜单：以第二参数作为参考父菜单，按指定参考偏移索引定位菜单。
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
private:
	//! \since build 830
	set<lref<Menu>, ystdex::get_less<>> menus{};

public:
	/*!
	\brief 根菜单关联映射。
	\since build 830

	指定向指定部件转移焦点时不进行隐藏的菜单的映射。
	*/
	map<lref<IWidget>, observer_ptr<Menu>, ystdex::get_less<>> Roots{};

	//! \since build 574
	DefDeCtor(MenuHost)

	/*!
	\brief 向菜单组添加菜单并隐藏。
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
	\brief 显示菜单组中指定的菜单。
	\note 若不在菜单组中则忽略。
	\since build 574
	*/
	void
	Show(Menu&);

	/*!
	\brief 显示菜单组中的所有菜单。
	\since build 574
	*/
	void
	ShowAll();

private:
	/*!
	\brief 显示指定菜单 mnu 。
	\pre 断言： Contains(mnu) 。
	\since build 574
	*/
	void
	ShowRaw(Menu& mnu);

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


/*!
\brief 绑定顶层部件的按键事件弹出菜单。
\note 只接受一个按键。
\since build 575
*/
YF_API void
BindTopLevelPopupMenu(MenuHost&, Menu&, IWidget&,
	KeyIndex k = KeyCodes::Secondary);

} // namespace UI;

} // namespace YSLib;

#endif

