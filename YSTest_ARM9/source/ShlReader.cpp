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
\version r2932;
\author FrankHB<frankhb1989@gmail.com>
\since build 263 。
\par 创建时间:
	2011-11-24 17:13:41 +0800;
\par 修改时间:
	2012-01-14 19:06 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YReader::ShlReader;
*/


#include <ShlReader.h>

////////
//测试用声明：全局资源定义。
//extern char gstr[128];

using namespace ystdex;

YSL_BEGIN_NAMESPACE(YReader)

/*namespace
{
	ResourceMap GlobalResourceMap;
}*/


namespace
{
	// MR -> MNU_READER;
	yconstexpr Menu::IndexType MR_Return(0),
		MR_Setting(1),
		MR_FileInfo(2),
		MR_LineUp(3),
		MR_LineDown(4),
		MR_ScreenUp(5),
		MR_ScreenDown(6);
}


ReaderBox::ReaderBox(const Rect& r, ShlReader& shl)
	: Control(r),
	Shell(shl), btnMenu(Rect(4, 12, 16, 16)),
	btnInfo(Rect(24, 12, 16, 16)), btnReturn(Rect(44, 12, 16, 16)),
	pbReader(Rect(4, 0, 248, 8)), lblProgress(Rect(216, 12, 40, 16))
{
	SetTransparent(true),
	SetContainerPtrOf(btnMenu, this),
	SetContainerPtrOf(btnInfo, this),
	SetContainerPtrOf(btnReturn, this),
	SetContainerPtrOf(pbReader, this),
	SetContainerPtrOf(lblProgress, this),
	btnMenu.Text = "M",
	btnInfo.Text = "I",
	btnReturn.Text = "R",
	pbReader.ForeColor = Color(192, 192, 64),
	lblProgress.SetTransparent(true),
	lblProgress.Font.SetSize(12);
}

IWidget*
ReaderBox::GetTopWidgetPtr(const Point& pt,
	bool(&f)(const IWidget&))
{
/*
#define DefTuple(_n, ...) \
	const auto _n = std::make_tuple(__VA_ARGS__);

DefTuple(pWidgets,
		&ShlReader::ReaderPanel::btnMenu,
		&ShlReader::ReaderPanel::pbReader,
		&ShlReader::ReaderPanel::lblProgress
	)
*/
	IWidget* const pWidgets[] = {&btnMenu, &btnInfo, &btnReturn, &pbReader};

	for(std::size_t i(0); i < sizeof(pWidgets) / sizeof(*pWidgets); ++i)
		if(auto p = CheckWidget(*pWidgets[i], pt, f))
			return p;
	return nullptr;
}

Rect
ReaderBox::Refresh(const PaintContext& pc)
{
	Widget::Refresh(pc);

	IWidget* const pWidgets[] = {&btnMenu, &btnInfo, &btnReturn, &pbReader,
		&lblProgress};

	for(std::size_t i(0); i < sizeof(pWidgets) / sizeof(*pWidgets); ++i)
		PaintChild(*pWidgets[i], pc);
	return Rect(pc.Location, GetSizeOf(*this));
}

void
ReaderBox::UpdateData(DualScreenReader& reader)
{
	char str[4];

	const auto ts(reader.GetTextSize());
	const auto tp(reader.GetTopPosition());

	std::sprintf(str, "%2u%%", tp * 100 / ts);
	yunseq(lblProgress.Text = str,
		lblProgress.ForeColor = reader.GetBottomPosition() == ts
		? ColorSpace::Green : ColorSpace::Fuchsia);
	pbReader.SetMaxValue(ts),
	pbReader.SetValue(tp);
	Invalidate(pbReader),
	Invalidate(lblProgress);
}


TextInfoBox::TextInfoBox(ShlReader& shl)
	: DialogBox(Rect(32, 32, 200, 108)),
	Shell(shl),
	lblEncoding(Rect(4, 20, 192, 18)),
	lblSize(Rect(4, 40, 192, 18)),
	lblTop(Rect(4, 60, 192, 18)),
	lblBottom(Rect(4, 80, 192, 18))
{
	SetContainerPtrOf(lblEncoding, this),
	SetContainerPtrOf(lblSize, this);
	FetchEvent<TouchMove>(*this) += OnTouchMove_Dragging;
}

Rect
TextInfoBox::Refresh(const PaintContext& pc)
{
	DialogBox::Refresh(pc);

	IWidget* const pWidgets[] = {&lblEncoding, &lblSize, &lblTop, &lblBottom};

	for(size_t i(0); i < sizeof(pWidgets) / sizeof(*pWidgets); ++i)
		PaintChild(*pWidgets[i], pc);
	return Rect(pc.Location, GetSizeOf(*this));
}

void
TextInfoBox::UpdateData(DualScreenReader& reader)
{
	char str[40];

	std::sprintf(str, "Encoding: %d;", reader.GetEncoding());
	lblEncoding.Text = str;
	std::sprintf(str, "Size: %u B;", reader.GetTextSize());
	lblSize.Text = str;
	std::sprintf(str, "Top: %u B;", reader.GetTopPosition());
	lblTop.Text = str;
	std::sprintf(str, "Bottom: %u B;", reader.GetBottomPosition());
	lblBottom.Text = str;
	Invalidate(lblEncoding),
	Invalidate(lblSize);
}


SettingPanel::SettingPanel()
	: DialogPanel(Rect::FullScreen),
	lblColorAreaUp(Rect(32, 32, 64, 20)),
	lblColorAreaDown(Rect(160, 32, 64, 20)),
	ColorAreaUp(Rect(32, 64, 48, 24)), ColorAreaDown(Rect(160, 64, 48, 24)),
	boxColor(Point(4, 80)), pSetting()
{
	*this += lblColorAreaUp,
	*this += lblColorAreaDown,
	*this += ColorAreaUp,
	*this += ColorAreaDown,
	*this += boxColor,
	SetVisibleOf(boxColor, false);
	yunseq(
		lblColorAreaUp.Text = "上屏背景色：",
		lblColorAreaDown.Text = "下屏背景色：",
		FetchEvent<Click>(ColorAreaUp) += [this](TouchEventArgs&&){
			boxColor.SetColor(ColorAreaUp.BackColor);
			Show(boxColor);
			pSetting = &ColorAreaUp;
		},
		FetchEvent<Paint>(ColorAreaUp).Add(Border, &BorderStyle::OnPaint),
		FetchEvent<Click>(ColorAreaDown) += [this](TouchEventArgs&&){
			boxColor.SetColor(ColorAreaDown.BackColor);
			Show(boxColor);
			pSetting = &ColorAreaDown;
		},
		FetchEvent<Paint>(ColorAreaDown).Add(Border, &BorderStyle::OnPaint),
		FetchEvent<TouchMove>(boxColor) += OnTouchMove_Dragging,
		FetchEvent<Click>(boxColor.btnOK) += [this](TouchEventArgs&&){
			if(pSetting)
			{
				pSetting->BackColor = boxColor.GetColor();
				Invalidate(*pSetting);
				pSetting = nullptr;
			}
		}
	);
}

FileInfoPanel::FileInfoPanel()
	: Panel(Rect::FullScreen),
	lblPath(Rect(8, 20, 240, 16)),
	lblSize(Rect(8, 40, 240, 16)),
	lblAccessTime(Rect(8, 60, 240, 16)),
	lblModifiedTime(Rect(8, 80, 240, 16)),
	lblOperations(Rect(8, 120, 240, 16))
{
	BackColor = ColorSpace::Silver;
	lblOperations.Text = "<↑↓> 滚动一行 <LR> 滚动一屏 <B>退出";
	*this += lblPath,
	*this += lblSize,
	*this += lblAccessTime,
	*this += lblModifiedTime,
	*this += lblOperations;
}


string ReaderManager::path;
bool ReaderManager::is_text(false);

ReaderManager::ReaderManager(ShlReader& shl)
	: Shell(shl)
{}


TextReaderManager::TextReaderManager(ShlReader& shl)
	: ReaderManager(shl),
	Reader(),
	boxReader(Rect(0, 160, 256, 32), shl), boxTextInfo(shl), pnlSetting(),
	pTextFile(), mhMain(shl.GetDesktopDown())
{
	yunseq(
		Reader.ViewChanged = [this]()
		{
			if(IsVisible(boxReader))
				boxReader.UpdateData(Reader);
			if(IsVisible(boxTextInfo))
				boxTextInfo.UpdateData(Reader);
		},
		FetchEvent<TouchDown>(boxReader) -= OnTouchDown_RequestToTopFocused,
		FetchEvent<Click>(boxReader.btnMenu) += [this](TouchEventArgs&& e)
		{
			if(mhMain.IsShowing(1u))
				mhMain.Hide(1u);
			else
			{
				mhMain.Referent = &boxReader.btnMenu;
				ShowMenu(1u, e);
			}
		},
		FetchEvent<Click>(boxReader.btnInfo) += [this](TouchEventArgs&&)
		{
			Execute(MR_FileInfo);
		},
		FetchEvent<Click>(boxReader.btnReturn) += [this](TouchEventArgs&&)
		{
			Execute(MR_Return);
		},
		FetchEvent<TouchDown>(boxReader.pbReader) += [this](TouchEventArgs&& e)
		{
			Reader.Locate(e.X * Reader.GetTextSize()
				/ boxReader.pbReader.GetWidth());
		},
		FetchEvent<Paint>(boxReader.pbReader) += [this](PaintEventArgs&& e){
			auto& pb(boxReader.pbReader);
			const auto mval(pb.GetMaxValue());
			const auto w(pb.GetWidth() - 2);
			auto& pt(e.Location);

			FillRect(e.Target, Point(pt.X + 1 + round(pb.GetValue() * w / mval),
				pt.Y + 1), Size(round((Reader.GetBottomPosition()
				- Reader.GetTopPosition()) * w / mval), pb.GetHeight() - 2),
				ColorSpace::Yellow);
		},
		FetchEvent<Click>(pnlSetting.btnClose) += [this](TouchEventArgs&&)
		{
			Reader.SetVisible(true);
		},
		FetchEvent<Click>(pnlSetting.btnOK) += [this](TouchEventArgs&&)
		{
			yunseq(Shell.GetDesktopUp().BackColor
				= pnlSetting.ColorAreaUp.BackColor,
				Shell.GetDesktopDown().BackColor
				= pnlSetting.ColorAreaDown.BackColor);
			Reader.SetVisible(true);
		}
	);
	{
		auto hList(share_raw(new Menu::ListType));
		auto& lst(*hList);

		static yconstexpr const char* mnustr[] = {"返回", "设置...",
			"文件信息...", "向上一行", "向下一行", "向上一屏", "向下一屏"};

		ystdex::assign(lst, mnustr);

		Menu& mnu(*(ynew Menu(Rect::Empty, std::move(hList), 1u)));

		mnu.GetConfirmed() += [this](IndexEventArgs&& e){
			Execute(e.Value);
		};
		mhMain += mnu;
	}
	ResizeForContent(mhMain[1u]);
}

void
TextReaderManager::Activate()
{
	auto& dsk_up(Shell.GetDesktopUp());
	auto& dsk_dn(Shell.GetDesktopDown());

	yunseq(
		dsk_up.BackColor = Color(240, 216, 192),
		dsk_dn.BackColor = Color(192, 216, 240),
		FetchEvent<Click>(dsk_dn).Add(*this, &TextReaderManager::OnClick),
		FetchEvent<KeyDown>(dsk_dn).Add(*this, &TextReaderManager::OnKeyDown),
		FetchEvent<KeyHeld>(dsk_dn) += OnKeyHeld
	);
	Reader.Attach(dsk_up, dsk_dn),
	dsk_dn += boxReader,
	dsk_dn += boxTextInfo,
	dsk_dn += pnlSetting;
	SetVisibleOf(boxReader, false),
	SetVisibleOf(boxTextInfo, false),
	SetVisibleOf(pnlSetting, false);
	pTextFile = unique_raw(new TextFile(path.c_str()));
	Reader.LoadText(*pTextFile);
	RequestFocusCascade(dsk_dn);
}

void
TextReaderManager::Deactivate()
{
	auto& dsk_up(Shell.GetDesktopUp());
	auto& dsk_dn(Shell.GetDesktopDown());

	Reader.UnloadText();
	pTextFile = nullptr;
	mhMain.Clear();
	yunseq(
		FetchEvent<Click>(dsk_dn).Remove(*this, &TextReaderManager::OnClick),
		FetchEvent<KeyDown>(dsk_dn).Remove(*this,
			&TextReaderManager::OnKeyDown),
		FetchEvent<KeyHeld>(dsk_dn) -= OnKeyHeld
	);
	Reader.Detach();
	yunseq(
		dsk_up -= boxReader,
		dsk_dn -= boxTextInfo,
		dsk_dn -= pnlSetting
	);
}

void
TextReaderManager::Execute(IndexEventArgs::ValueType idx)
{
	switch(idx)
	{
	case MR_Return:
		CallStored<ShlExplorer>();
		break;
	case MR_Setting:
		Reader.SetVisible(false),
		pnlSetting.ColorAreaUp.BackColor = Shell.GetDesktopUp().BackColor,
		pnlSetting.ColorAreaDown.BackColor = Shell.GetDesktopDown().BackColor;
		Show(pnlSetting);
		break;
	case MR_FileInfo:
		boxTextInfo.UpdateData(Reader);
		Show(boxTextInfo);
		break;
	case MR_LineUp:
		Reader.Execute(DualScreenReader::LineUpScroll);
		break;
	case MR_LineDown:
		Reader.Execute(DualScreenReader::LineDownScroll);
		break;
	case MR_ScreenUp:
		Reader.Execute(DualScreenReader::ScreenUpScroll);
		break;
	case MR_ScreenDown:
		Reader.Execute(DualScreenReader::ScreenDownScroll);
		break;
	}
}

void
TextReaderManager::ShowMenu(Menu::ID id, const Point&)
{
	if(!mhMain.IsShowing(id))
	{
		auto& mnu(mhMain[id]);

		SetLocationOf(mnu, Point());
		switch(id)
		{
		case 1u:
			mnu.SetItemEnabled(MR_LineUp, !Reader.IsTextTop());
			mnu.SetItemEnabled(MR_LineDown, !Reader.IsTextBottom());
			mnu.SetItemEnabled(MR_ScreenUp, !Reader.IsTextTop());
			mnu.SetItemEnabled(MR_ScreenDown, !Reader.IsTextBottom());
		}
		mhMain.Show(id);
	}
}

void
TextReaderManager::OnClick(TouchEventArgs&& e)
{
	if(IsVisible(boxReader))
	{
		Reader.Stretch(0);
		Close(boxReader);
	}
	else
	{
		Reader.Stretch(boxReader.GetHeight());
		boxReader.UpdateData(Reader);
		Show(boxReader);
	}
}

void
TextReaderManager::OnKeyDown(KeyEventArgs&& e)
{
	if(e.Strategy != RoutedEventArgs::Tunnel && !mhMain.IsShowing(1u))
	{
		u32 k(static_cast<KeyEventArgs::InputType>(e));

		switch(k)
		{
		case KeySpace::Enter:
			Reader.UpdateView();
			break;
		case KeySpace::Esc:
			CallStored<ShlExplorer>();
			break;
		case KeySpace::Up:
			Reader.Execute(DualScreenReader::LineUpScroll);
			break;
		case KeySpace::Down:
			Reader.Execute(DualScreenReader::LineDownScroll);
			break;
		case KeySpace::PgUp:
			Reader.Execute(DualScreenReader::ScreenUpScroll);
			break;
		case KeySpace::PgDn:
			Reader.Execute(DualScreenReader::ScreenDownScroll);
			break;
		case KeySpace::X:
			Reader.SetLineGap(5);
			break;
		case KeySpace::Y:
			Reader.SetLineGap(8);
			break;
		case KeySpace::Left:
			//Reader.SetFontSize(Reader.GetFontSize()+1);
			if(Reader.GetLineGap() != 0)
				Reader.SetLineGap(Reader.GetLineGap() - 1);
			break;
		case KeySpace::Right:
			if(Reader.GetLineGap() != 12)
				Reader.SetLineGap(Reader.GetLineGap() + 1);
		default:
			break;
		}
	}
}


HexReaderManager::HexReaderManager(ShlReader& shl)
	: ReaderManager(shl),
	HexArea(Rect::FullScreen), pnlFileInfo()
{
	HexArea.SetRenderer(unique_raw(new BufferedRenderer()));
	yunseq(
		FetchEvent<KeyDown>(HexArea) += [](KeyEventArgs&& e){
			if(e.GetKeyCode() == KeySpace::Esc)
			{
				CallStored<ShlExplorer>();
				e.Handled = true; //注意不要使 CallStored 被调用多次。
			}
		},
		HexArea.ViewChanged += [this](HexViewArea::ViewArgs&&){
			UpdateInfo();
		}
	);
}

namespace
{
	yconstexpr const char* DefaultTimeFormat("%04u-%02u-%02u %02u:%02u:%02u");

	inline void
	snftime(char* buf, size_t n, const std::tm& tm,
		const char* format = DefaultTimeFormat)
	{
		// FIXME: correct behavior for time with BC date(i.e. tm_year < -1900);
		// FIXME: snprintf shall be a member of namespace std;
		/*std*/::snprintf(buf, n, format, tm.tm_year + 1900,
			tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	}

	const char*
	TranslateTime(const std::tm& tm, const char* format = DefaultTimeFormat)
	{
		static char str[80];

		/*
		NOTE: 'std::strftime(str, sizeof(str), "%Y-%m-%d %H:%M:%S", &tm)'
			is correct but make the object file too large;
		*/
		snftime(str, 80, tm, format);
		return str;
	}
	const char*
	TranslateTime(const std::time_t& t,
		const char* format = DefaultTimeFormat) ythrow(GeneralEvent)
	{
		auto p(std::localtime(&t));

		if(!p)
			throw GeneralEvent("Get broken-down time object failed"
				" @ TranslateTime#2;");
		return TranslateTime(*p, format);
	}
}

void
HexReaderManager::Activate()
{
	auto& dsk_up(Shell.GetDesktopUp());
	auto& dsk_dn(Shell.GetDesktopDown());

	pnlFileInfo.lblPath.Text = u"文件路径："
		+ Text::MBCSToString(ReaderManager::path, IO::CP_Path);

	struct ::stat file_stat;

	//在 DeSmuMe 上无效； iDSL + DSTT 上访问时间精确不到日，修改时间正常。
	::stat(ReaderManager::path.c_str(), &file_stat);
	pnlFileInfo.lblAccessTime.Text = u"访问时间："
		+ Text::MBCSToString(TranslateTime(file_stat.st_atime));
	pnlFileInfo.lblModifiedTime.Text = u"修改时间："
		+ Text::MBCSToString(TranslateTime(file_stat.st_mtime));
	dsk_up += pnlFileInfo;
	HexArea.Load(path.c_str());
	HexArea.UpdateData(0);
	UpdateInfo();
	dsk_dn += HexArea;
	RequestFocusCascade(HexArea);
}

void
HexReaderManager::Deactivate()
{
	auto& dsk_up(Shell.GetDesktopUp());
	auto& dsk_dn(Shell.GetDesktopDown());

	dsk_up -= pnlFileInfo;
	HexArea.Reset();
	dsk_dn -= HexArea;
}

void
HexReaderManager::UpdateInfo()
{
	char str[80];

	std::sprintf(str, "当前位置： %u / %u", HexArea.GetModel().GetPosition(),
		HexArea.GetModel().GetSize());
	pnlFileInfo.lblSize.Text = Text::MBCSToString(str, Text::CP_Default);
	Invalidate(pnlFileInfo.lblSize);
}


ShlReader::ShlReader()
	: ShlDS(),
	hUp(), hDn(), pManager()
{}

int
ShlReader::OnActivated(const Message& msg)
{
	ParentType::OnActivated(msg);

	auto& dsk_up(GetDesktopUp());
	auto& dsk_dn(GetDesktopDown());

	if(ReaderManager::is_text)
		pManager = unique_raw(new TextReaderManager(*this));
	else
		pManager = unique_raw(new HexReaderManager(*this));
	pManager->Activate();
	std::swap(hUp, dsk_up.GetBackgroundImagePtr()),
	std::swap(hDn, dsk_dn.GetBackgroundImagePtr());
	SetInvalidationOf(dsk_up),
	SetInvalidationOf(dsk_dn);
	UpdateToScreen();
	return 0;
}

int
ShlReader::OnDeactivated(const Message& msg)
{
	auto& dsk_up(GetDesktopUp());
	auto& dsk_dn(GetDesktopDown());

	std::swap(hUp, dsk_up.GetBackgroundImagePtr());
	std::swap(hDn, dsk_dn.GetBackgroundImagePtr());
	pManager->Deactivate();
	ParentType::OnDeactivated(msg);
	return 0;
}

YSL_END_NAMESPACE(YReader)

