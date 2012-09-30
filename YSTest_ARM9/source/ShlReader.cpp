/*
	Copyright (C) by Franksoft 2011 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ShlReader.cpp
\ingroup YReader
\brief Shell 阅读器框架。
\version r3888
\author FrankHB<frankhb1989@gmail.com>
\since build 263
\par 创建时间:
	2011-11-24 17:13:41 +0800
\par 修改时间:
	2012-09-27 01:17 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::ShlReader
*/


#include <ShlReader.h>
#include <ystdex/iterator.hpp>

YSL_BEGIN_NAMESPACE(YReader)

namespace
{
//	ResourceMap GlobalResourceMap;

using namespace Text;

/*
\brief 文本阅读器菜单项。
\since build 303
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
	btnMenu(Rect(4, 12, 16, 16)), btnSetting(Rect(24, 12, 16, 16)),
	btnInfo(Rect(44, 12, 16, 16)), btnReturn(Rect(64, 12, 16, 16)),
	btnPrev(Rect(84, 12, 16, 16)), btnNext(Rect(104, 12, 16, 16)),
	pbReader(Rect(4, 0, 248, 8)), lblProgress(Rect(216, 12, 40, 16))
{
	SetTransparent(true),
	SetRenderer(make_unique<BufferedRenderer>()),
	unseq_apply(ContainerSetter(*this), btnMenu, btnSetting,
		btnInfo, btnReturn, btnPrev, btnNext, pbReader, lblProgress);
	SetBufferRendererAndText(btnMenu, u"M"),
	SetBufferRendererAndText(btnSetting, u"S"),
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
		&btnSetting, &btnInfo, &btnReturn, &btnPrev, &btnNext, &pbReader})
		if(auto p = CheckWidget(*pWidget, pt, f))
			return p;
	return nullptr;
}

void
ReaderBox::Refresh(PaintEventArgs&& e)
{
	unseq_apply(ChildPainter(e), btnMenu, btnSetting,
		btnInfo, btnReturn, btnPrev, btnNext, pbReader, lblProgress);
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


FileInfoPanel::FileInfoPanel()
	: Panel(Rect(Point(), MainScreenWidth, MainScreenHeight)),
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
	// TODO: Use template %SetShellToNew.
//	SetShellToNew<ShlExplorer>();
	SetShellTo(ystdex::make_shared<ShlExplorer>(CurrentPath / u".."));
}

ReaderSetting
ShlReader::LoadGlobalConfiguration()
{
	try
	{
		return ReaderSetting(FetchGlobalInstance().Root.GetNode("YReader"));
	}
	catch(std::exception& e) // TODO: Logging.
	{}
	return ReaderSetting();
}

void
ShlReader::OnInput()
{
	PostMessage<SM_PAINT>(0xE0, nullptr);
	if(fBackgroundTask)
		PostMessage<SM_TASK>(0x20, fBackgroundTask);
}

void
ShlReader::SaveGlobalConfiguration(const ReaderSetting& rs)
{
	try
	{
		FetchGlobalInstance().Root["YReader"] += rs.operator ValueNode();
	}
	catch(std::exception& e) // TODO: Logging.
	{}
}


ShlTextReader::ShlTextReader(const IO::Path& pth)
	: ShlReader(pth),
	LastRead(ystdex::parameterize_static_object<ReadingList>()),
	CurrentSetting(LoadGlobalConfiguration()), tmrScroll(
	CurrentSetting.GetTimerSetting()), tmrInput(), reader(),
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
		FetchEvent<Click>(boxReader.btnSetting) += [this](TouchEventArgs&&)
		{
			Execute(MR_Setting);
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
			tmrScroll.SetInterval(CurrentSetting.GetTimerSetting());
		},
		FetchEvent<Click>(pnlSetting.btnOK) += exit_setting
	);
	{
		Menu& mnu(*(ynew Menu(Rect(), shared_ptr<Menu::ListType>(new
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
	// TODO: Associate view setting state for user selection.
	OnClick(TouchEventArgs(dsk_dn));
	RequestFocusCascade(dsk_dn);
}

ShlTextReader::~ShlTextReader()
{
	SaveGlobalConfiguration(CurrentSetting);
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
			using ystdex::get_key;

			const auto idx(std::find(Encodings | get_key,
				(Encodings + arrlen(Encodings)) | get_key,
				reader.GetEncoding()) - Encodings);

			yunseq(pnlSetting.lblAreaDown.Text = FetchEncodingString(idx),
				pnlSetting.ddlEncoding.Text = Encodings[idx].second);
		}
		StopAutoScroll(),
		Hide(boxReader),
		Hide(boxTextInfo),
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
	HexArea(Rect(Point(), MainScreenWidth, MainScreenHeight)), pnlFileInfo()
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

