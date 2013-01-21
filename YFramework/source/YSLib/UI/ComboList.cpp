/*
	Copyright by FrankHB 2011 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ComboList.cpp
\ingroup UI
\brief 样式相关的图形用户界面组合列表控件。
\version r3010
\author FrankHB<frankhb1989@gmail.com>
\since build 282
\par 创建时间:
	2011-03-07 20:33:05 +0800
\par 修改时间:
	2013-01-18 22:50 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::ComboList
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
	\since build 282
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
	lstText(Rect(r.GetSize()), h)
{
	Background = nullptr,
	SetContainerPtrOf(lstText, this),
	vsbVertical.GetTrack().GetScroll() += [this](ScrollEventArgs&& e){
		lstText.LocateViewPosition(SDst(round(e.GetValue())));
	},
	lstText.GetViewChanged() += [this](ViewArgs&& e){
		if(!e.Value && GetWidth() > defMinScrollBarWidth)
		{
			const Size view_arena(GetWidth() - defMinScrollBarWidth,
				lstText.GetFullViewHeight());

			SetSizeOf(lstText, FixLayout(view_arena));
			if(view_arena.Height > lstText.GetHeight())
			{
				vsbVertical.SetSmallDelta(lstText.GetItemHeight());
				vsbVertical.SetMaxValue(view_arena.Height
					- lstText.GetHeight());
				vsbVertical.SetLargeDelta(lstText.GetHeight());
				vsbVertical.SetValue(lstText.GetViewPosition());
			}
		}
	},
	RequestFocus(lstText);
	//刷新文本状态，防止第一次绘制时无法正确决定是否需要滚动条。
	lstText.RefreshTextState();
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
	{
		yunseq(s.Width = s.Width + defMinScrollBarWidth, s.Height = sup.Height);
		if(sup.Width != 0 && sup.Width < s.Width)
			s.Width = sup.Width;
	}
	SetSizeOf(*this, s);
	SetSizeOf(lstText, FixLayout(s));
	lstText.UpdateView();
}


FileBox::FileBox(const Rect& r)
	: FileList(), ListBox(r, GetListPtr())
{
	GetConfirmed() += [this](IndexEventArgs&& e){
		if(Contains(e) && bool(*this /= GetList()[e.Value]))
			ResetView();
	};
	ListItems();
	UpdateView();
}

IO::Path
FileBox::GetPath() const
{
	return IsSelected() ? Directory / (GetList()[GetSelectedIndex()])
		: Directory;
}

bool
FileBox::SetPath(const IO::Path& pth)
{
	if(FileList::operator=(pth))
	{
		UpdateView();
		return true;
	}
	return false;
}


DropDownList::DropDownList(const Rect& r, const shared_ptr<ListType>& h)
	: Button(r), boxList(Rect(), h)
{
	const auto detacher([this](UIEventArgs&&){
		DetachTopWidget();
	});

	yunseq(
		Margin.Left = 4,
		Margin.Right = 18,
		HorizontalAlignment = TextAlignment::Left,
		boxList.GetView().DependencyPtr = this,
		FetchEvent<TouchDown>(*this) += [this](TouchEventArgs&& e){
			if(!FetchContainerPtr(boxList))
			{
				Point pt;

				if(const auto p = dynamic_cast<Panel*>(
					&FetchTopLevel(*this, pt)))
				{
					// NOTE: Get height of top widget, top and bottom spaces.
					const SDst h0(GetSizeOf(*p).Height);
					const SDst h1(max<SPos>(0, pt.Y)), h2(max<SPos>(0, h0 - pt.Y
						- GetHeight()));

					if(IsInOpenInterval(h1, h0) || IsInOpenInterval(h2, h0))
					{
						boxList.ResizeForPreferred(Size(0, max(h1, h2)),
							Size(GetWidth(), 0));

						const SDst h(boxList.GetHeight());

						// NOTE: Bottom space is preferred.
						pt.Y += h2 < h ? -h : GetHeight();
						SetLocationOf(boxList, pt);
						boxList.AdjustViewLength();
						{
							const auto idx(boxList.Find(Text));

							if(idx + 1 != 0)
								boxList.SetSelected(idx);
							else
								boxList.ClearSelected();
						}
						p->Add(boxList, 224U); // TODO: Use non-magic number.
						RequestFocus(boxList);
						e.Handled = true;
					}
				}
			}
		},
		FetchEvent<LostFocus>(*this) += detacher,
		FetchEvent<LostFocus>(boxList) += detacher,
		boxList.GetConfirmed() += [this](IndexEventArgs&& e){
			YAssert(e.Value < boxList.GetList().size(), "Invalid index found.");

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

void
DropDownList::Refresh(PaintEventArgs&& e)
{
	bool b(bPressed);

	bPressed = bPressed || FetchContainerPtr(boxList);
	Button::Refresh(std::move(e));
	bPressed = b;
	DrawArrow(e.Target, Rect(e.Location + Vec(GetWidth() - 16, 0),
		Size(16, GetHeight())), 4, RDeg270, ForeColor);
}

YSL_END_NAMESPACE(Components)

YSL_END

