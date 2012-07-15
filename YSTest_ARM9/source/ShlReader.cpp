/*
	Copyright (C) by Franksoft 2010 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ShlReader.cpp
\ingroup YReader
\brief Shell 阅读器框架。
\version r4603;
\author FrankHB<frankhb1989@gmail.com>
\since build 263 。
\par 创建时间:
	2011-11-24 17:13:41 +0800;
\par 修改时间:
	2012-07-14 14:08 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YReader::ShlReader;
*/


#include <ShlReader.h>
#include <ystdex/iterator.hpp>

////////
//测试用声明：全局资源定义。
//extern char gstr[128];

using namespace ystdex;
using std::chrono::milliseconds;

YSL_BEGIN_NAMESPACE(YReader)

namespace
{
//	ResourceMap GlobalResourceMap;

using namespace Text;

/*!
\brief 取编码字符串。
\since build 290 。
*/
String
FetchEncodingString(MTextList::IndexType i)
{
	if(YB_LIKELY(i < arrlen(Encodings)))
	{
		const auto& pr(Encodings[i]);

		return String(to_string(pr.first) + ": "
			+ String(pr.second).GetMBCS().c_str());
	}
	return u"---";
}


/*!
\since build 293 。
*/
milliseconds
FetchTimerSetting(const ReaderSetting& s)
{
	return s.SmoothScroll ? s.SmoothScrollDuration : s.ScrollDuration;
}


/*
\brief 文本阅读器菜单项。
\since build 303 。
*/
enum MNU_READER : Menu::IndexType
{
	MR_Return = 0,
	MR_Setting,
	MR_FileInfo,
	MR_LineUp,
	MR_LineDown,
	MR_ScreenUp,
	MR_ScreenDown
};

} // unnamed namespace;


ReaderBox::ReaderBox(const Rect& r)
	: Control(r),
	btnMenu(Rect(4, 12, 16, 16)),
	btnInfo(Rect(24, 12, 16, 16)), btnReturn(Rect(44, 12, 16, 16)),
	btnPrev(Rect(64, 12, 16, 16)), btnNext(Rect(84, 12, 16, 16)),
	pbReader(Rect(4, 0, 248, 8)), lblProgress(Rect(216, 12, 40, 16))
{
	SetTransparent(true),
	SetRenderer(make_unique<BufferedRenderer>()),
	unseq_apply(ContainerSetter(*this),
		btnMenu, btnInfo, btnReturn, btnPrev, btnNext, pbReader, lblProgress);
	SetBufferRendererAndText(btnMenu, u"M"),
	SetBufferRendererAndText(btnInfo, u"I"),
	SetBufferRendererAndText(btnReturn, u"R"),
	SetBufferRendererAndText(btnPrev, u"←"),
	SetBufferRendererAndText(btnNext, u"→");
	pbReader.ForeColor = Color(192, 192, 64),
	lblProgress.SetRenderer(make_unique<BufferedRenderer>()),
	lblProgress.SetTransparent(true),
	lblProgress.Font.SetSize(12);
	yunseq(lblProgress.Text = u"--%", lblProgress.ForeColor = ColorSpace::Blue);
}

IWidget*
ReaderBox::GetTopWidgetPtr(const Point& pt, bool(&f)(const IWidget&))
{
	for(const auto pWidget : std::initializer_list<IWidget*>{&btnMenu,
		&btnInfo, &btnReturn, &btnPrev, &btnNext, &pbReader})
		if(auto p = CheckWidget(*pWidget, pt, f))
			return p;
	return nullptr;
}

void
ReaderBox::Refresh(PaintEventArgs&& e)
{
	unseq_apply(ChildPainter(e),
		btnMenu, btnInfo, btnReturn, btnPrev, btnNext, pbReader, lblProgress);
	e.ClipArea = Rect(e.Location, GetSizeOf(*this));
}

void
ReaderBox::UpdateData(DualScreenReader& reader)
{
	const auto ts(reader.GetTextSize());

	if(YB_LIKELY(ts != 0))
	{
		const auto tp(reader.GetTopPosition());
		char str[5];

		std::sprintf(str, "%2u%%", tp * 100 / ts);
		yunseq(lblProgress.Text = str,
			lblProgress.ForeColor = reader.GetBottomPosition() == ts
			? ColorSpace::Green : ColorSpace::Fuchsia);
		pbReader.SetMaxValue(ts),
		pbReader.SetValue(tp);
	}
	Invalidate(pbReader),
	Invalidate(lblProgress);
}


TextInfoBox::TextInfoBox()
	: DialogBox(Rect(32, 32, 200, 108)),
	lblEncoding(Rect(4, 20, 192, 18)),
	lblSize(Rect(4, 40, 192, 18)),
	lblTop(Rect(4, 60, 192, 18)),
	lblBottom(Rect(4, 80, 192, 18))
{
	unseq_apply(ContainerSetter(*this), lblEncoding, lblSize);
	FetchEvent<TouchMove>(*this) += OnTouchMove_Dragging;
}

void
TextInfoBox::Refresh(PaintEventArgs&& e)
{
	DialogBox::Refresh(std::move(e));

	unseq_apply(ChildPainter(e), lblEncoding, lblSize, lblTop, lblBottom);
	e.ClipArea = Rect(e.Location, GetSizeOf(*this));
}

void
TextInfoBox::UpdateData(DualScreenReader& reader)
{
	yunseq(lblEncoding.Text = "Encoding: " + to_string(reader.GetEncoding())
		+ ';',
		lblSize.Text = "Size: " + to_string(reader.GetTextSize()) + " B;",
		lblTop.Text = "Top: " + to_string(reader.GetTopPosition()) + " B;",
		lblBottom.Text = "Bottom: " + to_string(reader.GetBottomPosition())
		+ " B;");
	Invalidate(lblEncoding),
	Invalidate(lblSize);
}


SettingPanel::SettingPanel()
	: DialogPanel(Rect(Point::Zero, MainScreenWidth, MainScreenHeight)),
	lblAreaUp(Rect(20, 12, 216, 72)), lblAreaDown(Rect(20, 108, 216, 72)),
	btnFontSizeDecrease(Rect(20, 32, 80, 22)),
	btnFontSizeIncrease(Rect(148, 32, 80, 22)),
	btnSetUpBack(Rect(20, 64, 80, 22)), btnSetDownBack(Rect(148, 64, 80, 22)),
	btnTextColor(Rect(20, 96, 80, 22)),
	ddlFont(Rect(148, 96, 80, 22), FetchFontFamilyNames()),
	ddlEncoding(Rect(20, 128, 192, 22), share_raw(new
		TextList::ListType(Encodings | ystdex::get_value, (Encodings
		+ arrlen(Encodings)) | ystdex::get_value))),
	chkSmoothScroll(Rect(20, 162, 13, 13)),
	lblSmoothScroll(Rect(33, 160, 60, 20)),
	ddlScrollTiming(Rect(96, 160, 128, 22)),
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
		btnSetDownBack, btnTextColor, ddlFont, ddlEncoding, chkSmoothScroll,
		lblSmoothScroll, ddlScrollTiming),
	Add(boxColor, 112U),
	SetVisibleOf(boxColor, false);
	yunseq(
		btnFontSizeDecrease.Text = u"减小字体",
		btnFontSizeIncrease.Text = u"增大字体",
		btnSetUpBack.Text = u"上屏颜色...",
		btnSetDownBack.Text = u"下屏颜色...",
		btnTextColor.Text = u"文字颜色...",
		lblSmoothScroll.Text = u"平滑滚屏",
	//	FetchEvent<Paint>(lblColorAreaUp).Add(BorderBrush(BorderStyle),
	//		BoundaryPriority),
	//	FetchEvent<Paint>(lblColorAreaDown).Add(BorderBrush(BorderStyle),
	//		BoundaryPriority),
		FetchEvent<KeyDown>(*this) += OnEvent_StopRouting<KeyEventArgs>,
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
			pColor = &lblAreaUp.ForeColor;
			boxColor.SetColor(*pColor);
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
			if(auto p = FetchGlobalInstance().GetFontCache().GetFontFamilyPtr(
				ddlFont.Text.GetMBCS().c_str()))
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
		chkSmoothScroll.GetTicked() += [this](CheckBox::TickedArgs&& e){
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
			if(chkSmoothScroll.IsTicked())
				smooth_scroll_duration = milliseconds((e.Value + 1U) * 10);
			else
				scroll_duration = milliseconds((e.Value + 1U) * 100);
		},
		FetchEvent<TouchMove>(boxColor) += OnTouchMove_Dragging,
		FetchEvent<Click>(boxColor.btnOK) += [this](TouchEventArgs&&){
			if(pColor)
			{
				*pColor = boxColor.GetColor();
				lblAreaDown.ForeColor = lblAreaUp.ForeColor;
				Invalidate(lblAreaUp),
				Invalidate(lblAreaDown);
				pColor = nullptr;
			}
		}
	);
}

SettingPanel&
SettingPanel::operator<<(const ReaderSetting& s)
{
	yunseq(scroll_duration = s.ScrollDuration,
		smooth_scroll_duration = s.SmoothScrollDuration),
	chkSmoothScroll.Tick(s.SmoothScroll);
	return *this;
}

SettingPanel&
SettingPanel::operator>>(ReaderSetting& s)
{
	yunseq(
		s.SmoothScroll = chkSmoothScroll.IsTicked(),
		s.ScrollDuration = scroll_duration,
		s.SmoothScrollDuration = smooth_scroll_duration
	);
	return *this;
}

void
SettingPanel::UpdateInfo()
{
	lblAreaUp.Text = u"上屏文字大小: "
		+ String(to_string(lblAreaUp.Font.GetSize()) + " 。");
}


FileInfoPanel::FileInfoPanel()
	: Panel(Rect(Point::Zero, MainScreenWidth, MainScreenHeight)),
	lblPath(Rect(8, 20, 240, 16)),
	lblSize(Rect(8, 40, 240, 16)),
	lblAccessTime(Rect(8, 60, 240, 16)),
	lblModifiedTime(Rect(8, 80, 240, 16)),
	lblOperations(Rect(8, 120, 240, 16))
{
	Background = SolidBrush(ColorSpace::Silver);
	lblOperations.Text = "<↑↓> 滚动一行 <LR> 滚动一屏 <B>退出";
	AddWidgets(*this, lblPath, lblSize, lblAccessTime, lblModifiedTime,
		lblOperations);
}


ReadingList::ReadingList()
	: reading_list(), now_reading(reading_list.end())
{}

pair<bool, bool>
ReadingList::CheckBoundary()
{
	return make_pair(now_reading != reading_list.begin(),
		now_reading != reading_list.end());
}

void
ReadingList::DropSubsequent()
{
	reading_list.erase(now_reading, reading_list.end());
	now_reading = reading_list.end();
}

void
ReadingList::Insert(const IO::Path& pth, size_t pos)
{
	reading_list.emplace(now_reading, pth, pos);
}

BookMark
ReadingList::Switch(bool is_prev)
{
	if(is_prev)
	{
		--now_reading;
		--now_reading;
	}

	auto ret(std::move(*now_reading));

	reading_list.erase(now_reading++);
	return ret;
}


ShlReader::ShlReader(const IO::Path& pth)
	: ShlDS(),
	CurrentPath(pth), fBackgroundTask(), bExit()
{}

void
ShlReader::Exit()
{
	if(bExit)
		return;
	bExit = true;
	fBackgroundTask = nullptr;
	// TODO: use template %SetShellToNew;
//	SetShellToNew<ShlExplorer>();
	SetShellTo(ystdex::make_shared<ShlExplorer>(CurrentPath / u".."));
}

void
ShlReader::OnInput()
{
	PostMessage<SM_PAINT>(0xE0, nullptr);
	if(fBackgroundTask)
		PostMessage<SM_TASK>(0x20, fBackgroundTask);
}


ShlTextReader::ShlTextReader(const IO::Path& pth)
	: ShlReader(pth),
	LastRead(ystdex::parameterize_static_object<ReadingList>()),
	CurrentSetting(ystdex::get_init<>([]{
			return ReaderSetting{Color(240, 216, 192), Color(192, 216, 240),
				Color(), Font(FetchDefaultTypeface().GetFontFamily(), 14),
				true, milliseconds(1000), milliseconds(80)};
		})),
	tmrScroll(FetchTimerSetting(CurrentSetting)), tmrInput(), reader(),
	boxReader(Rect(0, 160, 256, 32)), boxTextInfo(), pnlSetting(),
	pTextFile(), mhMain(GetDesktopDown())
{
	using ystdex::get_key;

	const auto exit_setting([this](TouchEventArgs&&){
		auto& dsk_up(GetDesktopUp());

		yunseq(dsk_up.Background = pnlSetting.lblAreaUp.Background,
			GetDesktopDown().Background = pnlSetting.lblAreaDown.Background);
		RemoveWidgets(dsk_up, pnlSetting.lblAreaUp, pnlSetting.lblAreaDown),
		reader.SetVisible(true),
		boxReader.UpdateData(reader),
		boxTextInfo.UpdateData(reader),
		Show(boxReader);
	});

	SetVisibleOf(boxReader, false),
	SetVisibleOf(boxTextInfo, false),
	SetVisibleOf(pnlSetting, false);
	yunseq(
		reader.ViewChanged = [this]
		{
			if(IsVisible(boxReader))
				boxReader.UpdateData(reader);
			if(IsVisible(boxTextInfo))
				boxTextInfo.UpdateData(reader);
		},
		FetchEvent<TouchDown>(boxReader.btnMenu)
			+= OnEvent_StopRouting<TouchEventArgs>, //阻止菜单失去焦点。
		FetchEvent<Click>(boxReader.btnMenu) += [this](TouchEventArgs&& e)
		{
			if(mhMain.IsShowing(1u))
				mhMain.Hide(1u);
			else
				ShowMenu(1u, e);
		},
		FetchEvent<Click>(boxReader.btnInfo) += [this](TouchEventArgs&&)
		{
			Execute(MR_FileInfo);
		},
		FetchEvent<Click>(boxReader.btnReturn) += [this](TouchEventArgs&&)
		{
			Execute(MR_Return);
		},
		FetchEvent<Click>(boxReader.btnPrev) += [this](TouchEventArgs&&)
		{
			UpdateReadingList(true);
		},
		FetchEvent<Click>(boxReader.btnNext) += [this](TouchEventArgs&&)
		{
			UpdateReadingList(false);
		},
		FetchEvent<TouchDown>(boxReader.pbReader) += [this](TouchEventArgs&& e)
		{
			const auto s(reader.GetTextSize());

			if(YB_LIKELY(s != 0))
			{
				const auto old_pos(reader.GetTopPosition());

				reader.Locate(e.X * s / boxReader.pbReader.GetWidth());
				if(YB_LIKELY(old_pos != reader.GetTopPosition()))
				{
					LastRead.Insert(CurrentPath, old_pos);
					LastRead.DropSubsequent();
					UpdateButtons();
				}
			}
		},
		FetchEvent<Paint>(boxReader.pbReader) += [this](PaintEventArgs&& e){
			auto& pb(boxReader.pbReader);
			const auto mval(pb.GetMaxValue());
			const auto w(pb.GetWidth() - 2);
			auto& pt(e.Location);

			FillRect(e.Target, Point(pt.X + 1 + round(pb.GetValue() * w / mval),
				pt.Y + 1), Size(round((reader.GetBottomPosition()
				- reader.GetTopPosition()) * w / mval), pb.GetHeight() - 2),
				ColorSpace::Yellow);
		},
		FetchEvent<Click>(pnlSetting.btnClose) += exit_setting,
		FetchEvent<Click>(pnlSetting.btnOK) += [&, this](TouchEventArgs&&)
		{
			Switch(pnlSetting.current_encoding);
			reader.SetColor(pnlSetting.lblAreaUp.ForeColor),
			reader.SetFont(pnlSetting.lblAreaUp.Font);
			reader.UpdateView();
			pnlSetting >> CurrentSetting;
			tmrScroll.SetInterval(FetchTimerSetting(CurrentSetting));
		},
		FetchEvent<Click>(pnlSetting.btnOK) += exit_setting
	);
	{
		Menu& mnu(*(ynew Menu(Rect::Empty, shared_ptr<Menu::ListType>(new
			Menu::ListType{"返回", "设置...", "文件信息...", "向上一行",
			"向下一行", "向上一屏", "向下一屏"}), 1u)));

		mnu.GetConfirmed() += [this](IndexEventArgs&& e){
			Execute(e.Value);
		};
		mhMain += mnu;
	}
	ResizeForContent(mhMain[1u]);

	auto& dsk_up(GetDesktopUp());
	auto& dsk_dn(GetDesktopDown());

	reader.SetColor(CurrentSetting.FontColor),
	reader.SetFont(CurrentSetting.Font),
	yunseq(
		dsk_up.Background = SolidBrush(CurrentSetting.UpColor),
		dsk_dn.Background = SolidBrush(CurrentSetting.DownColor),
		FetchEvent<Click>(dsk_dn).Add(*this, &ShlTextReader::OnClick),
		FetchEvent<KeyDown>(dsk_dn).Add(*this, &ShlTextReader::OnKeyDown),
		FetchEvent<KeyHeld>(dsk_dn) += OnEvent_Call<KeyDown>
	);
	reader.Attach(dsk_up, dsk_dn),
	AddWidgets(dsk_dn, boxReader, boxTextInfo, pnlSetting);
	LoadFile(pth);
	LastRead.DropSubsequent();
	UpdateButtons();
	//置默认视图。
	// TODO: 关联视图设置状态使用户可选。
	OnClick(TouchEventArgs(dsk_dn));
	RequestFocusCascade(dsk_dn);
}

ShlTextReader::~ShlTextReader()
{
	LastRead.Insert(CurrentPath, reader.GetTopPosition());
}

void
ShlTextReader::Execute(IndexEventArgs::ValueType idx)
{
	switch(idx)
	{
	case MR_Return:
		Exit();
		break;
	case MR_Setting:
		reader.SetVisible(false),
		yunseq(pnlSetting.lblAreaUp.ForeColor = reader.GetColor(),
			pnlSetting.lblAreaUp.Background = GetDesktopUp().Background,
			pnlSetting.lblAreaUp.Font = reader.GetFont(),
			pnlSetting.lblAreaDown.ForeColor = reader.GetColor(),
			pnlSetting.lblAreaDown.Background = GetDesktopDown().Background,
			pnlSetting.lblAreaDown.Font = reader.GetFont(),
			pnlSetting.ddlFont.Text = reader.GetFont().GetFamilyName());
		pnlSetting.UpdateInfo();
		{
			auto& dsk_up(GetDesktopUp());

			dsk_up.Background = SolidBrush(ColorSpace::White);
			AddWidgets(dsk_up, pnlSetting.lblAreaUp, pnlSetting.lblAreaDown);
		}
		{
			const auto idx(std::find(Encodings | get_key,
				(Encodings + arrlen(Encodings)) | get_key,
				reader.GetEncoding()) - Encodings);

			yunseq(pnlSetting.lblAreaDown.Text = FetchEncodingString(idx),
				pnlSetting.ddlEncoding.Text = Encodings[idx].second);
		}
		StopAutoScroll(),
		Hide(boxReader),
		Show(pnlSetting << CurrentSetting);
		break;
	case MR_FileInfo:
		boxTextInfo.UpdateData(reader);
		Show(boxTextInfo);
		break;
	case MR_LineUp:
		reader.Execute(DualScreenReader::LineUpScroll);
		break;
	case MR_LineDown:
		reader.Execute(DualScreenReader::LineDownScroll);
		break;
	case MR_ScreenUp:
		reader.Execute(DualScreenReader::ScreenUpScroll);
		break;
	case MR_ScreenDown:
		reader.Execute(DualScreenReader::ScreenDownScroll);
		break;
	}
}

void
ShlTextReader::LoadFile(const IO::Path& pth)
{
	CurrentPath = pth;
	pTextFile = make_unique<TextFile>(pth);
	reader.LoadText(*pTextFile);
}

void
ShlTextReader::Scroll()
{
	if(tmrScroll.IsActive())
		if(YB_UNLIKELY(tmrScroll.Refresh()))
		{
			if(CurrentSetting.SmoothScroll)
				reader.ScrollByPixel(1U);
			else
				reader.Execute(DualScreenReader::LineDownScroll);
		}
}

void
ShlTextReader::ShowMenu(Menu::ID id, const Point&)
{
	if(!mhMain.IsShowing(id))
	{
		auto& mnu(mhMain[id]);

		SetLocationOf(mnu, Point()),
		mnu.ClearSelected();
		switch(id)
		{
		case 1u:
			mnu.SetItemEnabled(MR_LineUp, !reader.IsTextTop());
			mnu.SetItemEnabled(MR_LineDown, !reader.IsTextBottom());
			mnu.SetItemEnabled(MR_ScreenUp, !reader.IsTextTop());
			mnu.SetItemEnabled(MR_ScreenDown, !reader.IsTextBottom());
		}
		mhMain.Show(id);
	}
}

void
ShlTextReader::StopAutoScroll()
{
	reader.AdjustScrollOffset(),
	fBackgroundTask = nullptr,
	Deactivate(tmrScroll);
}

void
ShlTextReader::Switch(Encoding enc)
{
	if(enc != Encoding() && pTextFile && bool(*pTextFile)
		&& pTextFile->Encoding != enc)
	{
		pTextFile->Encoding = enc;
		reader.LoadText(*pTextFile);
	}
}

void
ShlTextReader::UpdateReadingList(bool is_prev)
{
	LastRead.Insert(CurrentPath, reader.GetTopPosition());

	const auto& bm(LastRead.Switch(is_prev));

	if(bm.Path != CurrentPath)
		LoadFile(bm.Path);
	reader.Locate(bm.Position);
	UpdateButtons();
}

void
ShlTextReader::UpdateButtons()
{
	const auto pr(LastRead.CheckBoundary());

	yunseq(Enable(boxReader.btnPrev, pr.first),
		Enable(boxReader.btnNext, pr.second));
}

void
ShlTextReader::OnClick(TouchEventArgs&&)
{
	if(tmrScroll.IsActive())
	{
		StopAutoScroll();
		Deactivate(tmrScroll);
		return;
	}
	if(IsVisible(boxReader))
	{
		Close(boxReader);
		reader.Stretch(0);
	}
	else
	{
		Show(boxReader);
		reader.Stretch(boxReader.GetHeight());
	}
}

void
ShlTextReader::OnKeyDown(KeyEventArgs&& e)
{
	using namespace Timers;
	using namespace KeyCodes;

	if(e.Strategy != RoutedEventArgs::Tunnel && !mhMain.IsShowing(1u)
		&& RepeatHeld(tmrInput, FetchGUIState().KeyHeldState,
		TimeSpan(240), TimeSpan(60)))
	{
		const auto ntick(HighResolutionClock::now());

		//这里可以考虑提供暂停，不调整视图。
		if(tmrScroll.IsActive())
		{
			StopAutoScroll();
			return;
		}

		const auto& k(e.GetKeys());

		if(k.count() != 1)
			return;
		if(k[YCL_KEY_Start])
		{
			fBackgroundTask = std::bind(&ShlTextReader::Scroll, this);
			tmrScroll.Reset();
			Activate(tmrScroll);
			return;
		}
		if(k[KeyCodes::Enter])
			reader.UpdateView();
		else if(k[Esc])
			Exit();
		else if(k[Up])
			reader.Execute(DualScreenReader::LineUpScroll);
		else if(k[Down])
			reader.Execute(DualScreenReader::LineDownScroll);
		else if(k[YCL_KEY(X)] || k[YCL_KEY(Y)])
		{
			auto size(reader.GetFont().GetSize());

			e.Handled = true;
			if(k[YCL_KEY(X)])
			{
				if(YB_LIKELY(size > Font::MinimalSize))
					--size;
				else
					return;
			}
			else if(YB_LIKELY(size < Font::MaximalSize))
				++size;
			else
				return;
			reader.SetFontSize(size);
			reader.UpdateView();
		}
		else if(k[YCL_KEY(L)])
		{
			if(YB_LIKELY(reader.GetLineGap() != 0))
				reader.SetLineGap(reader.GetLineGap() - 1);
		}
		else if(k[YCL_KEY(R)])
		{
			if(YB_LIKELY(reader.GetLineGap() != 12))
				reader.SetLineGap(reader.GetLineGap() + 1);
		}
		else if(k[Left])
	//	else if(k[PgUp])
			reader.Execute(DualScreenReader::ScreenUpScroll);
		else if(k[Right])
	//	else if(k[PgDn])
			reader.Execute(DualScreenReader::ScreenDownScroll);
		else
			return;
		tmrInput.Delay(HighResolutionClock::now() - ntick);
		e.Handled = true;
	}
}


ShlHexBrowser::ShlHexBrowser(const IO::Path& pth)
	: ShlReader(pth),
	HexArea(Rect(Point::Zero, MainScreenWidth, MainScreenHeight)), pnlFileInfo()
{
	HexArea.SetRenderer(make_unique<BufferedRenderer>(true));
	yunseq(
		FetchEvent<KeyDown>(HexArea) += [this](KeyEventArgs&& e){
			if(e.GetKeys() == 1 << KeyCodes::Esc)
			{
				Exit();
				e.Handled = true; //注意不要使 CallStored 被调用多次。
			}
		},
		HexArea.ViewChanged += [this](HexViewArea::ViewArgs&&){
			pnlFileInfo.lblSize.Text = u"当前位置： "
				+ String(to_string(HexArea.GetModel().GetPosition())
				+ " / " + to_string(HexArea.GetModel().GetSize()));
			Invalidate(pnlFileInfo.lblSize);
		}
	);

	auto& dsk_up(GetDesktopUp());
	auto& dsk_dn(GetDesktopDown());
	const auto& path_str(pth.GetNativeString());

	pnlFileInfo.lblPath.Text = u"文件路径：" + pth;

	struct ::stat file_stat;

	//在 DeSmuMe 上无效； iDSL + DSTT 上访问时间精确不到日，修改时间正常。
	::stat(path_str.c_str(), &file_stat);
	yunseq(pnlFileInfo.lblAccessTime.Text = u"访问时间："
		+ String(TranslateTime(file_stat.st_atime)),
		pnlFileInfo.lblModifiedTime.Text = u"修改时间："
		+ String(TranslateTime(file_stat.st_mtime)));
	dsk_up += pnlFileInfo;
	HexArea.Load(path_str.c_str());
	HexArea.UpdateData(0);
	HexArea.ViewChanged(HexViewArea::ViewArgs(HexArea, true));
	dsk_dn += HexArea;
	RequestFocusCascade(HexArea);
}

YSL_END_NAMESPACE(YReader)

