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
\version r836
\author FrankHB <frankhb1989@gmail.com>
\since build 203
\par 创建时间:
	2011-06-02 12:17:38 +0800
\par 修改时间:
	2014-03-27 01:29 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Menu
*/


#ifndef YSL_INC_UI_menu_h_
#define YSL_INC_UI_menu_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_TextList
#include YFM_YSLib_UI_YUIContainer // for UI::ZOrderType;

namespace YSLib
{

namespace UI
{

class MenuHost;

const ZOrderType DefaultMenuZOrder(224); //!< 默认菜单 Z 顺序值。


/*!
\brief 文本菜单。
\since build 252
*/
class YF_API Menu : public TextList
{
	friend class MenuHost;

public:
	using ID = size_t; //!< 菜单标识类型。
	using SubMap = map<IndexType, Menu*>; //!< 子菜单映射表类型。
	using ValueType = SubMap::value_type; //!< 子菜单映射表项目类型。

private:
	ID id; //!< 菜单标识。

protected:
	MenuHost* pHost; //!< 宿主指针。
	Menu* pParent; //!< 父菜单指针。
	SubMap mSubMenus; //!< 子菜单映射表：存储非空子菜单指针。
	mutable vector<bool> vDisabled; //!< 未启用菜单项。

public:
	/*!
	\brief 构造：使用指定边界、文本列表和菜单标识。
	\since build 327
	*/
	explicit
	Menu(const Rect& = {}, const shared_ptr<ListType>& = {}, ID = 0);
	/*!
	\brief 禁止转移构造。
	\since build 379
	\todo 实现转移操作。
	*/
	DefDelMoveCtor(Menu)

	/*!
	\brief 访问索引指定的子菜单。
	\exception std::out_of_range 异常中立：由 at 抛出。
	*/
	PDefHOp(Menu&, [], size_t idx)
		ImplRet(*mSubMenus.at(idx))

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

	DefGetter(const ynothrow, ID, ID, id)
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
	\brief 检查列表中的指定项是否可用。
	\note 当且仅当可用时响应 Confirmed 事件。
	*/
	bool
	CheckConfirmed(ListType::size_type) const override;

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

protected:
	/*!
	\brief 绘制菜单项。
	\since build 346
	*/
	void
	DrawItem(const Graphics&, const Rect& mask, const Rect&,
		ListType::size_type) override;
};


/*!
\brief 定位菜单：以第二个参数作为参考父菜单，按指定参考偏移索引定位菜单。
*/
YF_API void
LocateMenu(Menu&, const Menu&, Menu::IndexType);


/*!
\brief 菜单宿主。
\since build 252
*/
class YF_API MenuHost : private noncopyable, private OwnershipTag<Menu>
{
public:
	using ItemType = Menu*; //!< 菜单组项目类型：记录菜单控件指针。
	using MenuMap = map<Menu::ID, ItemType>; //!< 菜单组类型。
	using ValueType = MenuMap::value_type;

	Window& Frame; //!< 框架窗口。

protected:
	MenuMap mMenus; //!< 菜单组：存储非空菜单指针。

public:
	/*!
	\since build 363
	\brief 根菜单关联映射。

	指定向指定部件转移焦点时不进行隐藏的菜单的映射。
	*/
	map<IWidget*, Menu::ID> Roots;

	MenuHost(Window&);
	/*!
	\brief 析构。
	\note 隐藏菜单组中的所有菜单并清理菜单组。
	*/
	virtual
	~MenuHost();

	/*!
	\brief 向菜单组添加标识和指针指定的菜单。
	\note 覆盖菜单对象的菜单标识成员；若菜单项已存在则覆盖旧菜单项。
	*/
	void
	operator+=(const ValueType&);
	/*!
	\brief 向菜单组添加菜单。
	\note 标识由菜单对象的菜单标识成员指定；若菜单项已存在则覆盖旧菜单项。
	*/
	void
	operator+=(Menu&);

	/*!
	\brief 从菜单组移除菜单标识指定的菜单。
	\note 同时置菜单宿主指针为空。
	*/
	bool
	operator-=(Menu::ID);

	/*!
	\brief 访问菜单标识指定的菜单。
	\exception std::out_of_range 异常中立：由 at 抛出。
	*/
	PDefHOp(Menu&, [], Menu::ID id)
		ImplRet(*mMenus.at(id))

	/*!
	\brief 判断框架窗口中是否正在显示菜单标识指定的菜单。
	*/
	bool
	IsShowing(Menu::ID);

	/*!
	\brief 判断菜单组中是否存在菜单标识指定的菜单。
	*/
	PDefH(bool, Contains, Menu::ID id)
		ImplRet(ystdex::exists(mMenus, id))
	/*!
	\brief 判断菜单组中是否存在指定的菜单。
	*/
	bool
	Contains(Menu&);

	/*!
	\brief 清除菜单组。
	\note 释放所有的菜单对象。
	*/
	void
	Clear();

	/*!
	\brief 按指定 Z 顺序显示菜单组中菜单标识指定的菜单。
	*/
	void
	Show(Menu::ID, ZOrderType = DefaultMenuZOrder);
	/*!
	\brief 按指定 Z 顺序显示指定菜单 mnu 。
	\pre 断言： Contains(mnu) 。
	*/
	void
	Show(Menu& mnu, ZOrderType z = DefaultMenuZOrder)
	{
		YAssert(Contains(mnu), "Menu is not contained.");

		ShowRaw(mnu, z);
	}

	/*!
	\brief 按指定 Z 顺序显示菜单组中的所有菜单。
	*/
	void
	ShowAll(ZOrderType = DefaultMenuZOrder);

private:
	/*!
	\brief 按指定 Z 顺序显示指定菜单 mnu 。
	*/
	void
	ShowRaw(Menu& mnu, ZOrderType = DefaultMenuZOrder);

public:
	/*!
	\brief 隐藏菜单组中菜单标识指定的菜单。
	*/
	void
	Hide(Menu::ID);
	/*!
	\brief 隐藏指定菜单 mnu。
	\pre 断言： Contains(mnu) 。
	*/
	void
	Hide(Menu& mnu)
	{
		YAssert(Contains(mnu), "Menu is not contained.");

		HideRaw(mnu);
	}

	/*!
	\brief 隐藏菜单组中的所有菜单。
	*/
	void
	HideAll();

private:
	/*!
	\brief 隐藏指定菜单 mnu。
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

