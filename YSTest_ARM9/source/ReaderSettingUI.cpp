/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ReaderSettingUI.cpp
\ingroup YReader
\brief 阅读器设置界面。
\version r229
\author FrankHB <frankhb1989@gmail.com>
\since build 390
\par 创建时间:
	2013-03-20 20:28:23 +0800
\par 修改时间:
	2013-06-28 05:19 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::ReaderSettingUI
*/


#include "ReaderSettingUI.h"

YSL_BEGIN_NAMESPACE(YReader)

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
	lblAreaUp({20, 12, 216, 72}), lblAreaDown({20, 108, 216, 72}),
	btnFontSizeDecrease({20, 32, 80, 22}),
	btnFontSizeIncrease({148, 32, 80, 22}),
	btnSetUpBack({20, 64, 80, 22}), btnSetDownBack({148, 64, 80, 22}),
	btnTextColor({20, 96, 80, 22}),
	ddlFont({148, 96, 80, 22}, FetchFontFamilyNames()),
	ddlEncoding({20, 128, 192, 22}, share_raw(new
		TextList::ListType(Encodings | ystdex::get_value, (Encodings
		+ arrlen(Encodings)) | ystdex::get_value))),
	cbSmoothScroll({20, 160, 72, 18}),
	ddlScrollTiming({96, 160, 128, 22}),
	boxColor(Point(4, 80)), pColor(), current_encoding(),
	scroll_duration(), smooth_scroll_duration()
{
	const auto set_font_size([this](FontSize size){
		lblAreaUp.Font.SetSize(size),
		lblAreaDown.Font.SetSize(size);
		UpdateInfo();
		Invalidate(lblAreaUp),
		Invalidate(lblAreaDown);
	});

	AddWidgets(*this, btnFontSizeDecrease, btnFontSizeIncrease, btnSetUpBack,
		btnSetDownBack, btnTextColor, ddlFont, ddlEncoding, cbSmoothScroll,
		ddlScrollTiming),
	Add(boxColor, 112U),
	SetVisibleOf(boxColor, false);
	yunseq(
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
		FetchEvent<Click>(btnFontSizeDecrease)
			+= [this, set_font_size](TouchEventArgs&&){
			auto size(lblAreaUp.Font.GetSize());

			if(YB_LIKELY(size > Font::MinimalSize))
				set_font_size(--size);
		},
		FetchEvent<Click>(btnFontSizeIncrease)
			+= [this, set_font_size](TouchEventArgs&&){
			auto size(lblAreaUp.Font.GetSize());

			if(YB_LIKELY(size < Font::MaximalSize))
				set_font_size(++size);
		},
		FetchEvent<Click>(btnTextColor) += [this](TouchEventArgs&&){
			boxColor.SetColor(*(pColor = &lblAreaUp.ForeColor));
			Show(boxColor);
		},
		FetchEvent<Click>(btnSetUpBack) += [this](TouchEventArgs&&){
			boxColor.SetColor(*(pColor
				= &lblAreaUp.Background.target<SolidBrush>()->Color));
			Show(boxColor);
		},
		FetchEvent<Click>(btnSetDownBack) += [this](TouchEventArgs&&){
			boxColor.SetColor(*(pColor
				= &lblAreaDown.Background.target<SolidBrush>()->Color));
			Show(boxColor);
		},
		ddlFont.GetConfirmed() += [this](IndexEventArgs&&){
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
		cbSmoothScroll.GetTicked() += [this](CheckBox::TickedArgs&& e){
			using ystdex::get_init;

			static yconstexpr auto fetch_scroll_durations([](bool is_smooth)
			{
				const auto postfix(is_smooth ? u"毫秒/像素行" : u"毫秒/文本行");
				auto& lst(*new TextList::ListType(20U));
				const u16 delta(is_smooth ? 10 : 100);
				u16 t(0);

				std::generate(lst.begin(), lst.end(), [&, is_smooth, delta]{
					return String(to_string(t += delta)) + postfix;
				});
				return share_raw(&lst);
			});

			ddlScrollTiming.SetList(e.Value ? get_init<true>(
				fetch_scroll_durations, true) : get_init<false>(
				fetch_scroll_durations, false));
			ddlScrollTiming.Text = ddlScrollTiming.GetList()[(e.Value
				? smooth_scroll_duration.count() / 10U : scroll_duration.count()
				/ 100U) - 1U],
			Invalidate(ddlScrollTiming);
		},
		ddlScrollTiming.GetConfirmed() += [this](IndexEventArgs&& e){
			if(cbSmoothScroll.IsTicked())
				smooth_scroll_duration = milliseconds((e.Value + 1U) * 10);
			else
				scroll_duration = milliseconds((e.Value + 1U) * 100);
		},
		FetchEvent<TouchMove>(boxColor) += OnTouchMove_Dragging,
		FetchEvent<Click>(boxColor.btnOK) += [this](TouchEventArgs&&){
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
	yunseq(lblAreaUp.ForeColor = s.FontColor,
		lblAreaUp.Background = SolidBrush(s.UpColor),
		lblAreaUp.Font = s.Font,
		lblAreaDown.ForeColor = s.FontColor,
		lblAreaDown.Background = SolidBrush(s.DownColor),
		lblAreaDown.Font = s.Font,
		ddlFont.Text = s.Font.GetFamilyName(),
		scroll_duration = s.ScrollDuration,
		smooth_scroll_duration = s.SmoothScrollDuration),
	cbSmoothScroll.Tick(s.SmoothScroll);
	UpdateInfo();
	return *this;
}

SettingPanel&
SettingPanel::operator>>(ReaderSetting& s)
{
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

YSL_END_NAMESPACE(YReader)

