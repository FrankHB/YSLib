/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ComboList.cpp
\ingroup UI
\brief 样式相关的图形用户界面组合列表控件。
\version r3249
\author FrankHB <frankhb1989@gmail.com>
\since build 282
\par 创建时间:
	2011-03-07 20:33:05 +0800
\par 修改时间:
	2015-09-18 14:05 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::ComboList
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_ComboList
#include YFM_YSLib_UI_YStyle
#include YFM_YSLib_UI_YPanel
#include YFM_YSLib_Service_TextLayout // for FetchMaxTextWidth;

namespace YSLib
{

namespace UI
{

namespace
{

const SDst defMinScrollBarWidth(16); //!< 默认最小滚动条宽。


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

} // unnamed namespace;


ListBox::ListBox(const Rect& r, unique_ptr<TextList> p_textlist)
	: ScrollableContainer(r),
	pTextList(std::move(p_textlist))
{
	Background = nullptr,
	SetContainerPtrOf(GetTextListRef(), this),
	vsbVertical.GetTrackRef().Scroll += [this](ScrollEventArgs&& e){
		GetTextListRef().LocateViewPosition(SDst(round(e.GetValue())));
	},
	GetTextListRef().ViewChanged += [this](ViewArgs&& e){
		if(!e.Value && GetWidth() > defMinScrollBarWidth)
		{
			const Size view_arena(GetWidth() - defMinScrollBarWidth,
				GetTextListRef().GetFullViewHeight());

			SetSizeOf(GetTextListRef(), FixLayout(view_arena));
			if(view_arena.Height > GetTextListRef().GetHeight())
			{
				vsbVertical.SetSmallDelta(GetTextListRef().GetItemHeight());
				vsbVertical.SetMaxValue(view_arena.Height
					- GetTextListRef().GetHeight());
				vsbVertical.SetLargeDelta(GetTextListRef().GetHeight());
				vsbVertical.SetValue(GetTextListRef().GetViewPosition());
			}
		}
	},
	RequestFocus(GetTextListRef());
	// NOTE: Text state refreshed for correctly showing scroll bars.
	ResetView();
}
ListBox::ListBox(const Rect& r, const shared_ptr<ListType>& h)
	: ListBox(r, make_unique<TextList>(Rect(r.GetSize()), h))
{}
ImplDeDtor(ListBox)

void
ListBox::ResizeForPreferred(const Size& sup, Size s)
{
	if(s.Width == 0)
		s.Width = FetchMaxTextWidth(GetTextListRef().LabelBrush.Font,
			GetTextListRef().GetList().cbegin(),
			GetTextListRef().GetList().cend())
			+ GetHorizontalOf(GetTextListRef().LabelBrush.Margin);
	if(s.Height == 0)
		s.Height = GetTextListRef().GetFullViewHeight();
	if(sup.Width != 0 && s.Width > sup.Width)
		s.Width = sup.Width;
	if(sup.Height != 0 && s.Height > sup.Height)
	{
		yunseq(s.Width += defMinScrollBarWidth, s.Height = sup.Height);
		if(sup.Width != 0 && sup.Width < s.Width)
			s.Width = sup.Width;
	}
	SetSizeOf(*this, s);
	SetSizeOf(GetTextListRef(), FixLayout(s));
	UpdateView();
}


FileBox::FileBox(const Rect& r)
	: ListBox(r), pthDirectory()
{
	GetConfirmed() += [this](IndexEventArgs&& e){
		if(Contains(e) && bool(*this /= IO::Path(GetList()[e.Value])))
		{
			GetListRef() = ListItems();
			ResetView();
		}
	};
	ListItems();
	UpdateView();
}
ImplDeDtor(FileBox)

IO::Path
FileBox::GetPath() const
{
	return IsSelected() ? pthDirectory / IO::Path(GetList()[GetSelectedIndex()])
		: pthDirectory;
}

bool
FileBox::operator=(const IO::Path& d)
{
	if(VerifyDirectory(d))
	{
		pthDirectory = d;
		return true;
	}
	return {};
}
bool
FileBox::operator/=(const String& d)
{
	return *this = pthDirectory / d;
}
bool
FileBox::operator/=(const IO::Path& d)
{
	return *this = pthDirectory / d;
}

bool
FileBox::SetPath(const IO::Path& pth)
{
	if(operator=(pth))
	{
		GetListRef() = ListItems();
		UpdateView();
		return true;
	}
	return {};
}

FileBox::ListType
FileBox::ListItems() const
{
	ListType lst;

	ListFiles(pthDirectory, lst);
	// TODO: Platform-dependent name converting.
	return lst;
}


DropDownList::DropDownList(const Rect& r, const shared_ptr<ListType>& h)
	: Button(r),
	lbContent({}, h)
{
	const auto detacher([this](UIEventArgs&& e){
		if(!dynamic_cast<RoutedEventArgs*>(&e))
			DetachTopWidget();
	});

	yunseq(
	Margin.Left = 4,
	Margin.Right = 18,
	HorizontalAlignment = TextAlignment::Left,
	lbContent.GetView().DependencyPtr = this,
	FetchEvent<TouchDown>(*this) += [this](CursorEventArgs&& e){
		if(!FetchContainerPtr(lbContent))
		{
			Point pt;

			if(const auto p
				= dynamic_cast<Panel*>(&FetchTopLevel(e.GetSender(), pt)))
			{
				// NOTE: Get height of top widget, top and bottom spaces.
				const SDst h0(GetSizeOf(*p).Height);
				// XXX: Conversion to 'SPos' might be implementation-defined.
				const SDst h1(SDst(max<SPos>(0, pt.Y))), h2(SDst(max<SPos>(0,
					SPos(h0) - pt.Y - SPos(GetHeight()))));

				if(IsInOpenInterval(h1, h0) || IsInOpenInterval(h2, h0))
				{
					lbContent.ResizeForPreferred(Size(0, max(h1, h2)),
						Size(GetWidth(), 0));

					const SDst h3(lbContent.GetHeight());

					// NOTE: Bottom space is preferred.
					// XXX: Conversion to 'SPos' might be
					//	implementation-defined.
					pt.Y += SPos(h2 < h3 ? -h3 : GetHeight());
					SetLocationOf(lbContent, pt);
					lbContent.AdjustViewLength();
					{
						const auto& lst(lbContent.GetList());
						const auto i(std::find(lst.cbegin(), lst.cend(), Text));

						if(i != lst.cend())
							lbContent.SetSelected(size_t(i - lst.cbegin()));
						else
							lbContent.ClearSelected();
					}
					p->Add(lbContent, 224U); // TODO: Use non-magic number.
					RequestFocusCascade(lbContent);
					e.Handled = true;
				}
			}
		}
	},
	FetchEvent<LostFocus>(*this) += detacher,
	FetchEvent<LostFocus>(lbContent) += detacher,
	lbContent.GetConfirmed() += [this](IndexEventArgs&& e){
		YAssert(e.Value < lbContent.GetList().size(), "Invalid index found.");

		Text = lbContent.GetList()[e.Value];
		// XXX: This seems to be redundant if the detached top widget would be
		//	always invalidated, however there is no such guarantee.
		Invalidate(e.GetSender()),
		Invalidate(*this);
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
	Detach(FetchContainerPtr(lbContent), lbContent);
}

void
DropDownList::Refresh(PaintEventArgs&& e)
{
	const auto cs(GetCursorState());

	if(FetchContainerPtr(lbContent))
		csCurrent = CursorState::Pressed;
	Button::Refresh(std::move(e));
	csCurrent = cs;
	// XXX: Conversion to 'SPos' might be implementation-defined.
	DrawArrow(e.Target, e.ClipArea, Rect(e.Location.X + SPos(GetWidth()) - 16,
		e.Location.Y, Size(16, GetHeight())), 4, RDeg270, ForeColor);
}

} // namespace UI;

} // namespace YSLib;

