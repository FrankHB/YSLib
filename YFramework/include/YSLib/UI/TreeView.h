/*
	© 2014-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file TreeView.h
\ingroup UI
\brief 树形视图控件。
\version r262
\author FrankHB <frankhb1989@gmail.com>
\since build 532
\par 创建时间:
	2014-09-04 19:48:13 +0800
\par 修改时间:
	2015-03-21 15:32 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::TreeView
*/


#ifndef YSL_INC_UI_TreeView_h_
#define YSL_INC_UI_TreeView_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_ComboList
#include YFM_YSLib_Core_ValueNode // for YSLib::ValueNode;
#include <ystdex/cast.hpp> // for ystdex::polymorphic_downcast;

namespace YSLib
{

namespace UI
{

/*!
\brief 树形列表。
\note 展开或收缩节点不保证强异常安全性：若抛出异常则未指定状态（可能不一致）。
\warning 使用 SetList 或 GetListRef 替换列表或减少列表元素后不保证状态一致，
	可能引起未定义行为。
\warning 若修改 idxShared 则不保证状态一致。
\since build 531
*/
class YF_API TreeList : public TextList
{
public:
	//! \brief 缩进类型：用于表示视图中项目的层次。
	using IndentType = size_t;
	//! \brief 节点状态。
	enum class NodeState
	{
		None = 0,
		Branch,
		Expanded
	};
	//! \since build 532
	using IndentMap = map<IndexType, IndentType>;
	//! \since build 532
	using NodePath = vector<IndexType>;

	//! \brief 收缩节点后的回调。
	GEvent<void(IndexType)> Collapse;
	//! \brief 展开节点前的回调。
	GEvent<void(IndexType)> Expand;
	//! \brief 文本提取。
	std::function<String(const ValueNode&)> ExtractText{DefaultExtractText};
	/*!
	\brief 响应 CursorOver 事件时节点分支图标的颜色。
	\since build 532
	*/
	Color CursorOverColor{39, 199, 247};
	/*!
	\brief 数据源。
	\warning 修改当前已展开的节点不保证状态一致；减少节点可能引起未定义行为。
	*/
	ValueNode TreeRoot{};
	//! \brief 单元缩进：指定在界面上显示层次的水平距离和缩进值的比。
	SDst UnitIndent = 16;

protected:
	/*!
	\brief 最近响应 CursorOver 的项的索引。
	\note 值等于 size_t(-1) 表示指针未在任何项上。
	\since build 532
	*/
	size_t idxCursorOver = size_t(-1);

private:
	/*!
	\brief 列表项位置索引。
	\since build 532

	以列表项的最大位置为键，映射到缩进值和相同缩进的连续列表项数量。
	*/
	IndentMap indent_map{};
	SPos tmp_margin_left = 0;
	/*!
	\brief 判断是否已经展开的索引。
	\since build 532
	\note 使用 trie 树优化。
	*/
	set<NodePath> expanded{};

public:
	//! \brief 构造：使用指定边界、文本列表和高亮背景色/文本色对。
	explicit
	TreeList(const Rect& = {}, const shared_ptr<ListType>& = {},
		const pair<Color, Color>& = FetchGUIConfiguration().Colors.GetPair(
		Styles::Highlight, Styles::HighlightText));
	DefDeMoveCtor(TreeList)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~TreeList() override;

	//! \brief 取相对单元部件的基准缩进框边界。
	Rect
	GetIndentBox() const;
	/*!
	\brief 取相对部件的指定索引的缩进框边界。
	\since build 533
	*/
	Rect
	GetIndentBoxBounds(IndexType) const;
	//! \since build 533
	DefGetter(const ynothrow, Size, IndentBoxSize, Size(16, GetItemHeight()))
	//! \since build 532
	DefGetter(const ynothrow, const IndentMap&, IndentMap, indent_map)
	SDst
	GetIndentWidth(IndexType) const;
	//! \since build 532
	NodePath
	GetNodePath(IndexType) const;
	const ValueNode&
	GetNodeRef(IndexType) const;

	/*!
	\brief 绑定数据源，展开不超过指定深度的节点。
	\note 清除未指定的不一致状态；不调用事件。
	\since build 534
	*/
	void
	Bind(size_t = size_t(-1));

	/*!
	\brief 绑定数据源并更新视图。
	\sa Bind
	\sa TextList::ResetView
	\since build 534
	*/
	PDefH(void, BindView, size_t max_depth = size_t(-1))
		ImplExpr(Bind(max_depth), ResetView())

	NodeState
	CheckNodeState(IndexType) const;

	/*!
	\brief 满足节点折叠条件（状态为 NodeState::Expanded ）时折叠节点。
	\return 折叠前节点的状态。
	\note 若满足状态不设置 idxShared 并调用 Collpase 。
	\since build 541
	*/
	PDefH(NodeState, CollapseNode, size_t idx)
		ImplRet(ExpandOrCollapseNode(NodeState::Expanded, idx))

	static String
	DefaultExtractText(const ValueNode&);

	static String
	ExtractNodeName(const ValueNode&);

	//! \since build 541
	//@{
	/*!
	\brief 满足节点展开条件（状态为 NodeState::Branch ）时展开节点。
	\return 折叠前节点的状态。
	\note 设置 idxShared 并调用 Expand 。
	*/
	PDefH(NodeState, ExpandNode, size_t idx)
		ImplRet(ExpandOrCollapseNode(NodeState::Branch, idx))

private:
	NodeState
	ExpandOrCollapseNode(NodeState, size_t);

	//! \since build 540
	void
	ExpandOrCollapseNodeImpl(NodeState, size_t);
	//@}
};


/*!
\brief 带滚动条的树形视图。
\invariant dynamic_cast<TreeList*>(pTextList.get())
\since build 534
\todo 实现 Resize 事件调整内容布局。
*/
class YF_API TreeView : public ListBox
{
public:
	using NodePath = TreeList::NodePath;
	using IndentMap = TreeList::IndentMap;

	explicit
	TreeView(const Rect& = {}, const shared_ptr<ListType>& = {});
	DefDeMoveCtor(TreeView)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~TreeView() override;

	DefGetter(, GEvent<void(IndexType)>&, Collapse, GetTreeListRef().Collapse)
	DefGetter(, GEvent<void(IndexType)>&, Expand, GetTreeListRef().Expand)
	DefGetter(, std::function<String(const ValueNode&)>&, ExtractText,
		GetTreeListRef().ExtractText)
	DefGetterMem(const, Rect, IndentBox, GetTreeListRef())
	PDefH(Rect, GetIndentBoxBounds, IndexType idx) const
		ImplRet(GetTreeListRef().GetIndentBoxBounds(idx))
	DefGetterMem(const ynothrow, Size, IndentBoxSize, GetTreeListRef())
	DefGetterMem(const ynothrow, const IndentMap&, IndentMap, GetTreeListRef())
	PDefH(SDst, GetIndentWidth, IndexType idx) const
		ImplRet(GetTreeListRef().GetIndentWidth(idx))
	PDefH(NodePath, GetNodePath, IndexType idx) const
		ImplRet(GetTreeListRef().GetNodePath(idx))
	PDefH(const ValueNode&, GetNodeRef, IndexType idx) const
		ImplRet(GetTreeListRef().GetNodeRef(idx))
	DefGetter(const, TreeList&, TreeListRef,
		*ystdex::polymorphic_downcast<TreeList*>(pTextList.get()))
	DefGetter(const, ValueNode&, TreeRootRef, GetTreeListRef().TreeRoot)

	PDefH(void, Bind, size_t max_depth = size_t(-1))
		ImplExpr(GetTreeListRef().Bind(max_depth))

	PDefH(void, BindView, size_t max_depth = size_t(-1))
		ImplExpr(GetTreeListRef().BindView(max_depth))

	PDefH(void, CheckNodeState, IndexType idx)
		ImplExpr(GetTreeListRef().CheckNodeState(idx))
};

} // namespace UI;

} // namespace YSLib;

#endif

