/*
	Copyright (C) by Franksoft 2011 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ComboList.cpp
\ingroup UI
\brief 样式相关的图形用户界面组合列表控件。
\version r3901;
\author FrankHB<frankhb1989@gmail.com>
\since build 282 。
\par 创建时间:
	2011-03-07 20:33:05 +0800;
\par 修改时间:
	2012-02-23 21:52 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::ComboList;
*/


#include "YSLib/UI/ComboList.h"
#include "YSLib/UI/ystyle.h"
#include "YSLib/UI/ypanel.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

namespace
{
	const SDst defMinScrollBarWidth(16); //!< 默认最小滚动条宽。
//	const SDst defMinScrollBarHeight(16); //!< 默认最小滚动条高。


	/*!
	\brief 从容器分离指定部件并无效化部件区域。
	\since build 282 。
	*/
	void
	Detach(IWidget* pCon, IWidget& wgt)
	{
		if(const auto p = dynamic_cast<Panel*>(pCon))
		{
			Invalidate(wgt);
			*p -= wgt;
		}
	}
}


ListBox::ListBox(const Rect& r, const shared_ptr<ListType>& h)
	: ScrollableContainer(r),
	lstText(Rect(Point::Zero, r), h)
{
	SetContainerPtrOf(lstText, this),
	VerticalScrollBar.GetTrack().GetScroll() += [this](ScrollEventArgs&& e){
		lstText.LocateViewPosition(SDst(round(e.GetValue())));
	},
	lstText.GetViewChanged() += [this](ViewArgs&& e){
		if(!e.Value && GetWidth() > defMinScrollBarWidth)
		{
			const Size view_arena(GetWidth() - defMinScrollBarWidth,
				lstText.GetFullViewHeight());

			YAssert(view_arena.Height > 1, "Invalid size found"
				" @ ListBox::ListBox;");

			SetSizeOf(lstText, FixLayout(view_arena));
			if(view_arena.Height > lstText.GetHeight())
			{
				VerticalScrollBar.SetSmallDelta(lstText.GetItemHeight());
				VerticalScrollBar.SetMaxValue(view_arena.Height
					- lstText.GetHeight());
				VerticalScrollBar.SetLargeDelta(lstText.GetHeight());
				VerticalScrollBar.SetValue(lstText.GetViewPosition());
			}
		}
	},
	RequestFocus(lstText);
	//刷新文本状态，防止第一次绘制时无法正确决定是否需要滚动条。
	lstText.RefreshTextState();
}

IWidget*
ListBox::GetTopWidgetPtr(const Point& pt, bool(&f)(const IWidget&))
{
	auto pCtl(ScrollableContainer::GetTopWidgetPtr(pt, f));

	pCtl = pCtl ? pCtl : &lstText;
	return f(*pCtl) ? pCtl : nullptr;
}

Rect
ListBox::Refresh(const PaintContext& pc)
{
	auto r(ScrollableContainer::Refresh(pc));

	PaintChild(lstText, pc);
	return r;
}

void
ListBox::ResizeForPreferred(const Size& sup, Size s)
{
	if(s.Width == 0)
		s.Width = lstText.GetMaxTextWidth() + GetHorizontalOf(lstText.Margin);
	if(s.Height == 0)
		s.Height = lstText.GetFullViewHeight();
	if(sup.Width != 0 && s.Width > sup.Width)
		s.Width = sup.Width;
	if(sup.Height != 0 && s.Height > sup.Height)
		yunseq(s.Width = std::min<SDst>(s.Width
			+ defMinScrollBarWidth, sup.Width), s.Height = sup.Height);
	SetSizeOf(*this, s);
	SetSizeOf(lstText, FixLayout(s));
}


FileBox::FileBox(const Rect& r)
	: FileList(), ListBox(r, GetListPtr())
{
	GetConfirmed() += [this](IndexEventArgs&& e){
		if(Contains(e) && bool(*this /= GetList()[e.Value]))
		{
			ListItems();
			ResetView();
		}
	};
	ListItems();
	UpdateView();
}

IO::Path
FileBox::GetPath() const
{
	if(IsSelected() && GetSelectedIndex() >= 0)
		return Directory / (GetList()[GetSelectedIndex()]);
	return Directory;
}


DropDownList::DropDownList(const Rect& r, const shared_ptr<ListType>& h)
	: Button(r), boxList(Rect::Empty, h)
{
	const auto detacher([this](UIEventArgs&&){
		DetachTopWidget();
	});

	yunseq(
		Margin.Left = 4,
		Margin.Right = GetHeight() + 4,
		HorizontalAlignment = TextAlignment::Left,
		boxList.GetView().pDependency = this,
		FetchEvent<TouchDown>(*this) += [this](TouchEventArgs&& e){
			if(!FetchContainerPtr(boxList))
			{
				Point pt;

				if(const auto p = dynamic_cast<Panel*>(
					&FetchTopLevel(*this, pt)))
				{
					// TODO: Compare top space size;
				//	const SDst h1(std::max<SPos>(pt.Y, 0)), h2(std::max<SPos>(
				//		GetSizeOf(*p).Height - pt.Y + GetHeight()));
					pt.Y += GetHeight();
					SetLocationOf(boxList, pt);

					const SPos h(GetSizeOf(*p).Height - pt.Y);

					if(h > 0)
					{
						boxList.ResizeForPreferred(Size(0, h),
							Size(GetWidth(), 0));
						boxList.AdjustViewLength();
						{
							// TODO: move this block as a method of
							// %ListBox or %TextList;
							const auto& lst(boxList.GetList());
							auto i(std::find(lst.begin(), lst.end(), Text));

							if(i != lst.end())
								boxList.SetSelected(i - lst.begin());
							else
								boxList.ClearSelected();
						}
						p->Add(boxList, 224U); // TODO: use non-magic number;
						RequestFocus(boxList);
						e.Handled = true;
					}
				}
			}
		},
		FetchEvent<LostFocus>(*this) += detacher,
		FetchEvent<LostFocus>(boxList) += detacher,
		boxList.GetConfirmed() += [this](IndexEventArgs&& e){
			YAssert(e.Value < boxList.GetList().size(),
				"Index error found @ event Confirmed @ DropDownList::boxList;");

			Text = boxList.GetList()[e.Value];
			Invalidate(*this),
			DetachTopWidget();
		}
	);
}
DropDownList::~DropDownList()
{
	DetachTopWidget();
}

void
DropDownList::DetachTopWidget()
{
	Detach(FetchContainerPtr(boxList), boxList);
}

Rect
DropDownList::Refresh(const PaintContext& pc)
{
	const Rect r(Button::Refresh(pc));
	const SDst h(GetHeight());

	WndDrawArrow(pc.Target, Rect(pc.Location + Vec(GetWidth() - h, 0),
		Size(h, h)), 4, RDeg270, ForeColor);
	return r;
}

YSL_END_NAMESPACE(Components)

YSL_END

