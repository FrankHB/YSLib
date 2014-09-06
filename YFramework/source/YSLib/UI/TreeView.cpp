/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file TreeView.cpp
\ingroup UI
\brief 树形视图控件。
\version r369
\author FrankHB <frankhb1989@gmail.com>
\since build 532
\par 创建时间:
	2014-08-24 16:29:28 +0800
\par 修改时间:
	2014-09-06 13:26 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::TreeView
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_TreeView
#include <ystdex/cast.hpp> // for ystdex::qualify;

namespace YSLib
{

namespace UI
{

namespace
{

/*!
\brief 绑定节点的深度优先遍历。
\since build 532
*/
template<class _tNode, typename _fCallable, typename _fPred, class _tPath>
void
bind_node(const _tNode& node, _fCallable f, _fPred pred, const _tPath& pth,
	size_t depth = 0)
{
	if(pred(pth, depth) && node.GetSize() != 0)
	{
		auto cur_path(pth);

		cur_path.push_back(0),
		++depth;
		for(const auto& nd : node)
		{
			f(nd, cur_path, depth);
			bind_node(nd, f, pred, cur_path, depth);
			++cur_path.back();
		}
	}
}

} // unnamed namespace;

TreeList::TreeList(const Rect& r, const shared_ptr<ListType>& h,
	const pair<Color, Color>& hilight_pair)
	: TextList(r, h, hilight_pair)
{
	auto& unit(GetUnitRef());
	const auto inv_indent_box([this, &unit](const CursorEventArgs& e, bool in){
		ystdex::swap_guard<SPos, void> guard(LabelBrush.Margin.Left,
			LabelBrush.Margin.Left + GetIndentWidth(idxShared));
		const Rect& box(GetIndentBox());
		const auto old(idxCursorOver);

		idxCursorOver = in && box.Contains(e) ? idxShared : size_t(-1);
		// TODO: Optimize.
		if(old != size_t(-1))
			Invalidate(*this);
		else if(!in || old != idxCursorOver)
			Invalidate(e.GetSender(), box);
	});

	yunseq(
	FetchEvent<TouchDown>(unit) += [this](CursorEventArgs&& e){
		const auto st(CheckNodeState(idxShared));

		if(st != NodeState::None)
		{
			Rect box(GetIndentBox());

			box.X += GetIndentWidth(idxShared);
			if(box.Contains(e))
			{
				const auto& branch_pth(GetNodePath(idxShared));

				if(YB_UNLIKELY(branch_pth.empty()))
					throw std::runtime_error("Invalid state found.");
				if(st == NodeState::Branch)
				{
					Expand(idxShared);
					// TODO: Check state.
					expanded.insert(branch_pth);

					const auto& branch(at(TreeRoot, branch_pth));
					auto i(ystdex::qualify(indent_map).lower_bound(idxShared));

					YAssert(i != indent_map.cend(), "Invalid state found.");

					const auto idt(i->second);
					auto j(i);

					if(i->first != idxShared)
						j = indent_map.emplace_hint(i, idxShared, idt);
					else
					{
						++i;
						YAssert(i == indent_map.cend() || i->second <= idt,
							"Invalid state found.");
					}

					IndexType index(idxShared);
					IndentType indent(idt + 1);
					vector<pair<IndexType, IndentType>> vec_ins;
					vector<String> text_list;

					// TODO: Optimize implementation.
					bind_node(branch, [&, this](const ValueNode& node,
						const NodePath&, size_t depth){
						text_list.push_back(ExtractText(node));
						if(indent != depth)
						{
							vec_ins.emplace_back(index, indent);
							indent = depth;
						}
						++index;
					}, [this](const NodePath& pth, size_t){
						return ystdex::exists(expanded, pth);
					}, branch_pth, idt);
					YAssert(index != idxShared, "Invalid state found.");
					vec_ins.emplace_back(index, indent);
					YAssert(vec_ins.back().first > j->first,
						"Invalid insertion sequence found");
					// TODO: Strong exception guarantee.
					// See $2014-09 @ %Documentation::Workflow::Annual2014.
					{
						// XXX: Use %std::make_move_iterator if proper.
						vector<pair<IndentType, IndexType>>
							vec(i, indent_map.cend());
						const auto n(vec_ins.back().first - j->first);

						for(auto& pr : vec)
							pr.first += n;
						i = ystdex::insert_reversed(indent_map,
							indent_map.erase(i, indent_map.end()),
							vec.rbegin(), vec.rend());
					}
					ystdex::insert_reversed(indent_map, i,
						vec_ins.crbegin(), vec_ins.crend());

					auto& lst(GetListRef());

					// XXX: Reuse previous operation.
					// XXX: Use %cbegin after switched to libstdc++ 4.9.
					lst.insert(lst.begin() + idxShared + 1,
						std::make_move_iterator(text_list.begin()),
						std::make_move_iterator(text_list.end()));
				}
				else
				{
					auto i(ystdex::qualify(indent_map).find(idxShared));

					YAssert(i != indent_map.cend(), "Invalid state found.");

					const auto idt(i->second);

					if(++i != indent_map.cend())
					{
						auto& lst(GetListRef());

						YAssert(idxShared < i->first, "Invalid state found.");
						YAssert(i->first < lst.size(), "Invalid list found.");

						auto j(std::find_if_not(i, indent_map.cend(),
							[&, idt](decltype(*i) pr){
							return idt < pr.second;
						}));

						const auto n(std::prev(j)->first - idxShared);

						// XXX: Use %cbegin after switched to libstdc++ 4.9.
						yunseq(ystdex::erase_n(lst, std::next(lst.begin(),
							idxShared + 1), n), indent_map.erase(i, j));
#if 1
						{
							// XXX: Use %std::make_move_iterator if proper.
							vector<pair<IndentType, IndexType>>
								vec(j, indent_map.cend());

							for(auto& pr : vec)
								pr.first -= n;
							ystdex::insert_reversed(indent_map,
								indent_map.erase(j, indent_map.end()),
								vec.rbegin(), vec.rend());
						}
#else
						// See $2014-09 @ %Documentation::Workflow::Annual2014.
						for(; j != indent_map.cend(); ++j)
							j = ystdex::replace_value(indent_map, j->first,
								[n](decltype(*indent_map.cbegin()) pr){
									return make_pair(pr.first - n, pr.second);
								});
#endif
						// TODO: Check state.
						expanded.erase(branch_pth);
						Collapse(idxShared);
					}
				}
			}
		}
	},
	FetchEvent<CursorOver>(unit) += [this, inv_indent_box](CursorEventArgs&& e)
	{
		inv_indent_box(e, true);
	},
	FetchEvent<Leave>(unit) += [this, inv_indent_box](CursorEventArgs&& e){
		if(idxCursorOver != size_t(-1))
			inv_indent_box(e, {});
	},
	// TODO: + AddFront;
	FetchEvent<Paint>(unit).Add([this](PaintEventArgs&& e){
		tmp_margin_left = LabelBrush.Margin.Left;
		LabelBrush.Margin.Left += GetIndentWidth(idxShared);
		switch(CheckNodeState(idxShared))
		{
		case NodeState::Branch:
			DrawArrow(e.Target, e.ClipArea, GetIndentBox() + Vec(e.Location.X,
				e.Location.Y - 1), 5, RDeg0, idxCursorOver == idxShared
				? CursorOverColor : LabelBrush.ForeColor, true);
			break;
		case NodeState::Expanded:
			DrawCornerArrow(e.Target, e.ClipArea,
				e.Location + Vec(LabelBrush.Margin.Left - UnitIndent + 2, 2), 7,
				RDeg0, idxCursorOver == idxShared ? CursorOverColor
				: LabelBrush.ForeColor);
			break;
		default:
			;
		}
	}, BackgroundPriority - 1)
	.Add([this]{
		LabelBrush.Margin.Left = tmp_margin_left;
	}, 0)
	);
}

String
TreeList::DefaultExtractText(const ValueNode& node)
{
	return Access<string>(node);
}

String
TreeList::ExtractNodeName(const ValueNode& node)
{
	return node.GetName();
}

Rect
TreeList::GetIndentBox() const
{
	return
		Rect(LabelBrush.Margin.Left - UnitIndent - 2, 0, 16, GetItemHeight());
}
SDst
TreeList::GetIndentWidth(IndexType idx) const
{
	const auto i(indent_map.lower_bound(idx));

	return (i != indent_map.end() ? i->second : 0) * UnitIndent;
	// TODO: Enable more concrete exception class.
//	throw std::runtime_error("Invalid tree list state found.");
}
TreeList::NodePath
TreeList::GetNodePath(TreeList::IndexType idx) const
{
	const auto i(indent_map.lower_bound(idx));

	if(i != indent_map.cend())
	{
		auto indent(i->second);
		vector<IndexType> seq(indent);
		size_t n(0);
		pair<IndexType, IndentType> last(idx, indent);

		std::for_each(ystdex::make_reverse_iterator(i), indent_map.crend(),
			[&](decltype(*indent_map.crend()) pr){
			if(last.second <= indent)
			{
				YAssert(pr.first < last.first, "Invalid state found.");
				n += last.first - pr.first;
			}
			if(pr.second < indent)
			{
				YAssert(indent > 0, "Invalid indent found");
				seq[indent - 1] += n - 1;
				yunseq(n = 0, indent = pr.second);
			}
			last = pr;
		});
		YAssert(indent > 0, "Invalid indent found");
		seq[indent - 1] += n + last.first;
		return std::move(seq);
	}
	return {};
}
const ValueNode&
TreeList::GetNodeRef(IndexType idx) const
{
	return at(TreeRoot, GetNodePath(idx));
}

void
TreeList::Bind(size_t max_depth)
{
	IndentType indent(1);
	IndexType index(-1);

	// TODO: Optimize implementation.
	bind_node(TreeRoot, [&, this](const ValueNode& node,
		const NodePath& pth, size_t depth){
		GetListRef().push_back(ExtractText(node));
		if(indent != depth)
		{
			indent_map.emplace(index, indent);
			indent = depth;
		}
		if(depth < max_depth && node.GetSize() != 0)
			expanded.insert(pth);
		++index;
	}, [=](const NodePath&, size_t depth){
		return depth < max_depth;
	}, NodePath());
	if(index != IndexType(-1))
		indent_map.emplace(index, indent);
}

TreeList::NodeState
TreeList::CheckNodeState(IndexType idx) const
{
	if(GetNodeRef(idx).GetSize() != 0)
	{
		auto i(indent_map.find(idx));
		const auto indent(i->second);
		const auto e(indent_map.cend());

		// TODO: Optimize.
		return i != e && ++i != e && i->second == indent + 1
			&& ystdex::exists(expanded, GetNodePath(idx))
			? NodeState::Expanded : NodeState::Branch;
	}
	return NodeState::None;
}

} // namespace UI;

} // namespace YSLib;

