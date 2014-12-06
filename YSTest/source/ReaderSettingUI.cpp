/*
	© 2013-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ReaderSettingUI.cpp
\ingroup YReader
\brief 阅读器设置界面。
\version r479
\author FrankHB <frankhb1989@gmail.com>
\since build 390
\par 创建时间:
	2013-03-20 20:28:23 +0800
\par 修改时间:
	2014-12-02 18:44 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::ReaderSettingUI
*/


#include "ReaderSettingUI.h"

namespace YReader
{

//! \since build 436
namespace
{

const char TU_ReaderSettingUI[]{u8R"NPL(root
($type "TabControl")($bounds "8 24 240 160")
(pnlPage1
	($type "Panel")
	(btnFontSizeDecrease
		($type "Button")($bounds "12 12 80 22"))
	(btnFontSizeIncrease
		($type "Button")($bounds "148 12 80 22"))
	(btnSetUpBack
		($type "Button")($bounds "12 44 80 22"))
	(btnSetDownBack
		($type "Button")($bounds "148 44 80 22"))
	(btnTextColor
		($type "Button")($bounds "12 76 80 22"))
	(ddlFont
		($type "DropDownList")($bounds "148 76 80 22"))
)
(pnlPage2
	($type "Panel")
	(ddlEncoding
		($type "DropDownList")($bounds "12 12 192 22"))
)
(pnlPage3
	($type "Panel")
	(cbSmoothScroll
		($type "CheckButton")($bounds "12 12 72 18"))
	(ddlScrollTiming
		($type "DropDownList")($bounds "84 12 128 22"))
)

)NPL"};

} // unnamed namespace;

using namespace Text;
using std::chrono::milliseconds;

String
FetchEncodingString(MTextList::IndexType i)
{
	if(YB_LIKELY(i < arrlen(Encodings)))
	{
		const auto& pr(Encodings[i]);

		return String(to_string(pr.first) + ": ") + pr.second;
	}
	return u"---";
}


SettingPanel::SettingPanel()
	: DialogPanel(Size(MainScreenWidth, MainScreenHeight)),
	dynWgts(FetchWidgetLoader(), TU_ReaderSettingUI),
	lblAreaUp({20, 12, 216, 72}), lblAreaDown({20, 108, 216, 72}),
	boxColor({4, 80}), pColor(), current_encoding(),
	scroll_duration(), smooth_scroll_duration()
{
	// NOTE: Values of durations keep uninitialized. Use carefully.
	auto& node(dynWgts.WidgetNode);
	DeclDynWidget(TabControl, root, node)
	auto& node_pnlPage1(AccessWidgetNode(node, "pnlPage1"));
	auto& node_pnlPage2(AccessWidgetNode(node, "pnlPage2"));
	auto& node_pnlPage3(AccessWidgetNode(node, "pnlPage3"));
	DeclDynWidgetN(Button, btnFontSizeDecrease, node_pnlPage1)
	DeclDynWidgetN(Button, btnFontSizeIncrease, node_pnlPage1)
	DeclDynWidgetN(Button, btnSetUpBack, node_pnlPage1)
	DeclDynWidgetN(Button, btnSetDownBack, node_pnlPage1)
	DeclDynWidgetN(Button, btnTextColor, node_pnlPage1)
	DeclDynWidgetN(DropDownList, ddlFont, node_pnlPage1)
	DeclDynWidgetN(DropDownList, ddlEncoding, node_pnlPage2)
	DeclDynWidgetN(CheckButton, cbSmoothScroll, node_pnlPage3)
	DeclDynWidgetN(DropDownList, ddlScrollTiming, node_pnlPage3)
	const auto set_font_size([this](FontSize size){
		lblAreaUp.Font.SetSize(size),
		lblAreaDown.Font.SetSize(size);
		UpdateInfo();
		Invalidate(lblAreaUp),
		Invalidate(lblAreaDown);
	});

	AddButtonToTabBar(root, node_pnlPage1, "btnTabDisplay", u"显示设置");
	AddButtonToTabBar(root, node_pnlPage2, "btnTabText", u"文本设置");
	AddButtonToTabBar(root, node_pnlPage3, "btnTabOperation", u"操作设置");
	root.UpdateTabPages();
	for(const auto& p_pnl : root.GetPages())
		if(p_pnl)
			FetchEvent<Paint>(*p_pnl).Add(BorderBrush(), BoundaryPriority);
	AddWidgets(*this, root),
	Add(boxColor, 112U),
	SetVisibleOf(boxColor, false),
	ddlFont.SetList(FetchFontFamilyNames()),
	ddlEncoding.SetList(
		share_raw(new TextList::ListType(Encodings | ystdex::get_value,
		(Encodings + arrlen(Encodings)) | ystdex::get_value))),
	yunseq(
	root.Background = nullptr,
	btnFontSizeDecrease.Text = u"减小字体",
	btnFontSizeIncrease.Text = u"增大字体",
	btnSetUpBack.Text = u"上屏颜色...",
	btnSetDownBack.Text = u"下屏颜色...",
	btnTextColor.Text = u"文字颜色...",
	cbSmoothScroll.Text = u"平滑滚屏",
//	FetchEvent<Paint>(lblColorAreaUp).Add(BorderBrush(BorderStyle),
//		BoundaryPriority),
//	FetchEvent<Paint>(lblColorAreaDown).Add(BorderBrush(BorderStyle),
//		BoundaryPriority),
	FetchEvent<KeyDown>(*this) += OnEvent_StopRouting<KeyEventArgs>,
	FetchEvent<KeyHeld>(*this) += OnEvent_StopRouting<KeyEventArgs>,
	FetchEvent<Click>(btnFontSizeDecrease) += [this, set_font_size]{
		auto size(lblAreaUp.Font.GetSize());

		if(YB_LIKELY(size > Font::MinimalSize))
			set_font_size(--size);
	},
	FetchEvent<Click>(btnFontSizeIncrease) += [this, set_font_size]{
		auto size(lblAreaUp.Font.GetSize());

		if(YB_LIKELY(size < Font::MaximalSize))
			set_font_size(++size);
	},
	FetchEvent<Click>(btnTextColor) += [this]{
		boxColor.SetColor(*(pColor = &lblAreaUp.ForeColor));
		Show(boxColor);
	},
	FetchEvent<Click>(btnSetUpBack) += [this]{
		boxColor.SetColor(
			*(pColor = &lblAreaUp.Background.target<SolidBrush>()->Color));
		Show(boxColor);
	},
	FetchEvent<Click>(btnSetDownBack) += [this]{
		boxColor.SetColor(
			*(pColor = &lblAreaDown.Background.target<SolidBrush>()->Color));
		Show(boxColor);
	},
	ddlFont.GetConfirmed() += [&, this]{
		if(const auto p = FetchDefaultFontCache()
			.GetFontFamilyPtr(ddlFont.Text.GetMBCS().c_str()))
		{
			lblAreaUp.Font = Font(*p, lblAreaUp.Font.GetSize());
			lblAreaDown.Font = lblAreaUp.Font;
			Invalidate(lblAreaUp),
			Invalidate(lblAreaDown);
		}
	},
	ddlEncoding.GetConfirmed() += [this](IndexEventArgs&& e){
		yunseq(current_encoding = Encodings[e.Value].first,
			lblAreaDown.Text = FetchEncodingString(e.Value)),
		Invalidate(lblAreaDown);
	},
	cbSmoothScroll.Ticked += [&, this](CheckBox::TickedArgs&& e){
		using ystdex::get_init;
		const auto fetch_scroll_durations([](bool is_smooth){
			const auto postfix(is_smooth ? u"毫秒/像素行" : u"毫秒/文本行");
			auto& lst(*new TextList::ListType(20U));
			const std::uint16_t delta(is_smooth ? 10 : 100);
			std::uint16_t t(0);

			std::generate(lst.begin(), lst.end(), [&, is_smooth, delta]{
				return String(to_string(t += delta)) + postfix;
			});
			return share_raw(&lst);
		});

		ddlScrollTiming.SetList(e.Value ? get_init<true>(fetch_scroll_durations,
			true) : get_init<false>(fetch_scroll_durations, false));
		ddlScrollTiming.Text = ddlScrollTiming.GetList()[(e.Value
			? smooth_scroll_duration.count() / 10U : scroll_duration.count()
			/ 100U) - 1U],
		Invalidate(ddlScrollTiming);
	},
	ddlScrollTiming.GetConfirmed() += [&, this](IndexEventArgs&& e){
		if(cbSmoothScroll.IsTicked())
			smooth_scroll_duration = milliseconds((e.Value + 1U) * 10);
		else
			scroll_duration = milliseconds((e.Value + 1U) * 100);
	},
	FetchEvent<TouchHeld>(boxColor) += OnTouchHeld_Dragging,
	FetchEvent<Click>(boxColor.btnOK) += [this]{
		if(pColor)
		{
			// TODO: Determine less area to be invalidated.
			*pColor = boxColor.GetColor();
			lblAreaDown.ForeColor = lblAreaUp.ForeColor;
			Invalidate(lblAreaUp), Invalidate(lblAreaDown);
			pColor = {};
		}
	}
	);
}

SettingPanel&
SettingPanel::operator<<(const ReaderSetting& s)
{
	auto& node(dynWgts.WidgetNode);
	DeclDynWidgetN(DropDownList, ddlFont, node, "pnlPage1")
	DeclDynWidgetN(CheckButton, cbSmoothScroll, node, "pnlPage3")

	yunseq(
	lblAreaUp.ForeColor = s.FontColor,
	lblAreaUp.Background = SolidBrush(s.UpColor),
	lblAreaUp.Font = s.Font,
	lblAreaDown.ForeColor = s.FontColor,
	lblAreaDown.Background = SolidBrush(s.DownColor),
	lblAreaDown.Font = s.Font,
	ddlFont.Text = s.Font.GetFamilyName(),
	scroll_duration = s.ScrollDuration,
	smooth_scroll_duration = s.SmoothScrollDuration
	),
	cbSmoothScroll.Tick(s.SmoothScroll ? CheckBox::Checked
		: CheckBox::Unchecked);
	UpdateInfo();
	return *this;
}

SettingPanel&
SettingPanel::operator>>(ReaderSetting& s)
{
	auto& node(dynWgts.WidgetNode);
	DeclDynWidgetN(CheckButton, cbSmoothScroll, node, "pnlPage3")

	yunseq(s.UpColor = lblAreaUp.Background.target<SolidBrush>()->Color,
		s.DownColor = lblAreaDown.Background.target<SolidBrush>()->Color,
		s.FontColor = lblAreaUp.ForeColor,
		s.Font = lblAreaUp.Font,
		s.SmoothScroll = cbSmoothScroll.IsTicked(),
		s.ScrollDuration = scroll_duration,
		s.SmoothScrollDuration = smooth_scroll_duration);
	return *this;
}

void
SettingPanel::UpdateInfo()
{
	lblAreaUp.Text = u"上屏文字大小: "
		+ String(to_string(lblAreaUp.Font.GetSize()) + " 。");
}

} // namespace YReader;

