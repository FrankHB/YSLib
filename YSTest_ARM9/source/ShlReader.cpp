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
\version r3314;
\author FrankHB<frankhb1989@gmail.com>
\since build 263 。
\par 创建时间:
	2011-11-24 17:13:41 +0800;
\par 修改时间:
	2012-02-25 21:41 +0800;
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

YSL_BEGIN_NAMESPACE(YReader)

namespace
{
//	ResourceMap GlobalResourceMap;

	shared_ptr<TextList::ListType>
	FetchFontFamilyNames()
	{
		const auto& mFamilies(FetchGlobalInstance().GetFontCache()
			.GetFamilyIndices());

		return share_raw(new TextList::ListType(mFamilies.cbegin()
			| ystdex::get_key, mFamilies.cend() | ystdex::get_key));
	}


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
	btnPrev(Rect(64, 12, 16, 16)), btnNext(Rect(84, 12, 16, 16)),
	pbReader(Rect(4, 0, 248, 8)), lblProgress(Rect(216, 12, 40, 16))
{
	SetTransparent(true),
	seq_apply(ContainerSetter(*this),
		btnMenu, btnInfo, btnReturn, btnPrev, btnNext, pbReader, lblProgress);
	btnMenu.Text = "M",
	btnInfo.Text = "I",
	btnReturn.Text = "R",
	btnPrev.Text = "←",
	btnNext.Text = "→",
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
	IWidget* const pWidgets[] = {&btnMenu, &btnInfo, &btnReturn, &btnPrev,
		&btnNext, &pbReader};

	for(std::size_t i(0); i < sizeof(pWidgets) / sizeof(*pWidgets); ++i)
		if(auto p = CheckWidget(*pWidgets[i], pt, f))
			return p;
	return nullptr;
}

Rect
ReaderBox::Refresh(const PaintContext& pc)
{
	Widget::Refresh(pc);

	seq_apply(ChildPainter(pc),
		btnMenu, btnInfo, btnReturn, btnPrev, btnNext, pbReader, lblProgress);
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
	seq_apply(ContainerSetter(*this), lblEncoding, lblSize);
	FetchEvent<TouchMove>(*this) += OnTouchMove_Dragging;
}

Rect
TextInfoBox::Refresh(const PaintContext& pc)
{
	DialogBox::Refresh(pc);

	seq_apply(ChildPainter(pc),
		lblEncoding, lblSize, lblTop, lblBottom);
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
	lblAreaUp(Rect(20, 12, 216, 72)), lblAreaDown(Rect(20, 108, 216, 72)),
	btnFontSizeDecrease(Rect(20, 32, 80, 24)),
	btnFontSizeIncrease(Rect(148, 32, 80, 24)),
	btnSetUpBack(Rect(20, 64, 80, 24)), btnSetDownBack(Rect(148, 64, 80, 24)),
	btnTextColor(Rect(20, 96, 80, 24)),
	ddlFont(Rect(148, 96, 80, 24), FetchFontFamilyNames()),
	boxColor(Point(4, 80)), pColor()
{
	const auto set_font_size([this](FontSize size){
		lblAreaUp.Font.SetSize(size),
		lblAreaDown.Font.SetSize(size);
		UpdateInfo();
		Invalidate(lblAreaUp),
		Invalidate(lblAreaDown);
	});

	*this += btnFontSizeDecrease,
	*this += btnFontSizeIncrease,
	*this += btnSetUpBack,
	*this += btnSetDownBack,
	*this += btnTextColor,
	*this += ddlFont,
	Add(boxColor, 112U),
	SetVisibleOf(boxColor, false);
	yunseq(
		btnFontSizeDecrease.Text = u"减小字体",
		btnFontSizeIncrease.Text = u"增大字体",
		btnSetUpBack.Text = u"上屏颜色...",
		btnSetDownBack.Text = u"下屏颜色...",
		btnTextColor.Text = u"文字颜色...",
	//	FetchEvent<Paint>(lblColorAreaUp) += BorderBrush(BorderStyle),
	//	FetchEvent<Paint>(lblColorAreaDown) += BorderBrush(BorderStyle),
		FetchEvent<KeyDown>(*this) += OnEvent_StopRouting<KeyEventArgs>,
		FetchEvent<Click>(btnFontSizeDecrease) += [=, this](TouchEventArgs&&){
			auto size(lblAreaUp.Font.GetSize());

			if(size > Font::MinimalSize)
				set_font_size(--size);
		},
		FetchEvent<Click>(btnFontSizeIncrease) += [=, this](TouchEventArgs&&){
			auto size(lblAreaUp.Font.GetSize());

			if(size < Font::MaximalSize)
				set_font_size(++size);
		},
		FetchEvent<Click>(btnTextColor) += [this](TouchEventArgs&&){
			pColor = &lblAreaUp.ForeColor;
			boxColor.SetColor(*pColor);
			Show(boxColor);
		},
		FetchEvent<Click>(btnSetUpBack) += [this](TouchEventArgs&&){
			pColor = &lblAreaUp.BackColor;
			boxColor.SetColor(*pColor);
			Show(boxColor);
		},
		FetchEvent<Click>(btnSetDownBack) += [this](TouchEventArgs&&){
			pColor = &lblAreaDown.BackColor;
			boxColor.SetColor(*pColor);
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

void
SettingPanel::UpdateInfo()
{
	char str[20];

	/*std::*/snprintf(str, 20, "%u 。", lblAreaUp.Font.GetSize());

	String ustr(str);

	yunseq(
		lblAreaUp.Text = u"上屏文字大小: " + ustr,
		lblAreaDown.Text = u"下屏文字大小: " + ustr
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


ReaderSession::ReaderSession(ShlReader& shl)
	: Shell(shl)
{}


TextReaderSession::TextReaderSession(ShlReader& shl)
	: ReaderSession(shl),
	Reader(), boxReader(Rect(0, 160, 256, 32), shl),
	boxTextInfo(shl), pnlSetting(), pTextFile(), mhMain(shl.GetDesktopDown())
{
	const auto exit_setting([this](TouchEventArgs&&){
		auto& dsk_up(Shell.GetDesktopUp());

		yunseq(dsk_up.BackColor = pnlSetting.lblAreaUp.BackColor,
			Shell.GetDesktopDown().BackColor
			= pnlSetting.lblAreaDown.BackColor,
		dsk_up -= pnlSetting.lblAreaUp,
		dsk_up -= pnlSetting.lblAreaDown);
		Reader.SetVisible(true),
		Show(boxReader);
	});

	SetVisibleOf(boxReader, false),
	SetVisibleOf(boxTextInfo, false),
	SetVisibleOf(pnlSetting, false);
	yunseq(
		Reader.ViewChanged = [this]()
		{
			if(IsVisible(boxReader))
				boxReader.UpdateData(Reader);
			if(IsVisible(boxTextInfo))
				boxTextInfo.UpdateData(Reader);
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
			Shell.LastRead.Insert(path, Reader.GetTopPosition());
			Shell.LastRead.DropSubsequent();
			UpdateButtons();
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
		FetchEvent<Click>(pnlSetting.btnClose) += exit_setting,
		FetchEvent<Click>(pnlSetting.btnOK) += [this](TouchEventArgs&&)
		{
			Reader.SetColor(pnlSetting.lblAreaUp.ForeColor),
			Reader.SetFont(pnlSetting.lblAreaUp.Font);
			Reader.UpdateView();
		},
		FetchEvent<Click>(pnlSetting.btnOK) += exit_setting
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

	auto& dsk_up(Shell.GetDesktopUp());
	auto& dsk_dn(Shell.GetDesktopDown());

	Reader.SetColor(Shell.CurrentSetting.FontColor),
	Reader.SetFont(Shell.CurrentSetting.Font),
	yunseq(
		dsk_up.BackColor = Shell.CurrentSetting.UpColor,
		dsk_dn.BackColor = Shell.CurrentSetting.DownColor,
		FetchEvent<Click>(dsk_dn).Add(*this, &TextReaderSession::OnClick),
		FetchEvent<KeyDown>(dsk_dn).Add(*this, &TextReaderSession::OnKeyDown),
		FetchEvent<KeyHeld>(dsk_dn) += OnKeyHeld
	);
	Reader.Attach(dsk_up, dsk_dn),
	dsk_dn += boxReader,
	dsk_dn += boxTextInfo,
	dsk_dn += pnlSetting;
	LoadFile(ShlReader::CurrentPath);
	Shell.LastRead.DropSubsequent();
	UpdateButtons();
	//置默认视图。
	// TODO: 关联视图设置状态使用户可选。
	OnClick(TouchEventArgs(dsk_dn));
	RequestFocusCascade(dsk_dn);
}

TextReaderSession::~TextReaderSession()
{
	Shell.LastRead.Insert(path, Reader.GetTopPosition());
	Reader.UnloadText();
	pTextFile = nullptr;
	mhMain.Clear();

	auto& dsk_up(Shell.GetDesktopUp());
	auto& dsk_dn(Shell.GetDesktopDown());

	yunseq(
		Shell.CurrentSetting.UpColor = dsk_up.BackColor,
		Shell.CurrentSetting.DownColor = dsk_dn.BackColor,
		Shell.CurrentSetting.FontColor = Reader.GetColor(),
		Shell.CurrentSetting.Font = Reader.GetFont(),
		FetchEvent<Click>(dsk_dn).Remove(*this, &TextReaderSession::OnClick),
		FetchEvent<KeyDown>(dsk_dn).Remove(*this,
			&TextReaderSession::OnKeyDown),
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
TextReaderSession::Execute(IndexEventArgs::ValueType idx)
{
	switch(idx)
	{
	case MR_Return:
		CallStored<ShlExplorer>();
		break;
	case MR_Setting:
		Reader.SetVisible(false),
		yunseq(
			pnlSetting.lblAreaUp.ForeColor = Reader.GetColor(),
			pnlSetting.lblAreaUp.BackColor
				= Shell.GetDesktopUp().BackColor,
			pnlSetting.lblAreaUp.Font = Reader.GetFont(),
			pnlSetting.lblAreaDown.ForeColor = Reader.GetColor(),
			pnlSetting.lblAreaDown.BackColor
				= Shell.GetDesktopDown().BackColor,
			pnlSetting.lblAreaDown.Font = Reader.GetFont(),
			pnlSetting.ddlFont.Text = Reader.GetFont().GetFamilyName()
		);
		pnlSetting.UpdateInfo();
		{
			auto& dsk_up(Shell.GetDesktopUp());

			dsk_up.BackColor = ColorSpace::White;
			dsk_up += pnlSetting.lblAreaUp,
			dsk_up += pnlSetting.lblAreaDown;
		}
		Hide(boxReader),
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
TextReaderSession::LoadFile(const IO::Path& pth)
{
	path = pth;
	pTextFile = unique_raw(new TextFile(pth.GetNativeString().c_str()));
	Reader.LoadText(*pTextFile);
}

void
TextReaderSession::ShowMenu(Menu::ID id, const Point&)
{
	if(!mhMain.IsShowing(id))
	{
		auto& mnu(mhMain[id]);

		SetLocationOf(mnu, Point()),
		mnu.ClearSelected();
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
TextReaderSession::UpdateReadingList(bool is_prev)
{
	Shell.LastRead.Insert(path, Reader.GetTopPosition());

	const auto& bm(Shell.LastRead.Switch(is_prev));

	if(bm.Path != path)
		LoadFile(bm.Path);
	Reader.Locate(bm.Position);
	UpdateButtons();
}

void
TextReaderSession::UpdateButtons()
{
	const auto pr(Shell.LastRead.CheckBoundary());

	yunseq(Enable(boxReader.btnPrev, pr.first),
		Enable(boxReader.btnNext, pr.second));
}

void
TextReaderSession::OnClick(TouchEventArgs&&)
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
TextReaderSession::OnKeyDown(KeyEventArgs&& e)
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
/*
		case KeySpace::PgUp:
			Reader.Execute(DualScreenReader::ScreenUpScroll);
			break;
		case KeySpace::PgDn:
			Reader.Execute(DualScreenReader::ScreenDownScroll);
			break;
*/
		case KeySpace::X:
		case KeySpace::Y:
			{
				auto size(Reader.GetFont().GetSize());
				if(k == KeySpace::X && size > Font::MinimalSize)
					--size;
				else if(k == KeySpace::Y && size < Font::MaximalSize)
					++size;
				else
					break;
				Reader.SetFontSize(size);
				Reader.UpdateView();
			}
			break;
		case KeySpace::L:
			if(Reader.GetLineGap() != 0)
				Reader.SetLineGap(Reader.GetLineGap() - 1);
			break;
		case KeySpace::R:
			if(Reader.GetLineGap() != 12)
				Reader.SetLineGap(Reader.GetLineGap() + 1);
			break;
		case KeySpace::Left:
			Reader.Execute(DualScreenReader::ScreenUpScroll);
			break;
		case KeySpace::Right:
			Reader.Execute(DualScreenReader::ScreenDownScroll);
		default:
			break;
		}
	}
}


HexReaderSession::HexReaderSession(ShlReader& shl)
	: ReaderSession(shl),
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

	auto& dsk_up(Shell.GetDesktopUp());
	auto& dsk_dn(Shell.GetDesktopDown());
	const auto& path(ShlReader::CurrentPath.GetNativeString());

	pnlFileInfo.lblPath.Text = u"文件路径：" + ShlReader::CurrentPath;

	struct ::stat file_stat;

	//在 DeSmuMe 上无效； iDSL + DSTT 上访问时间精确不到日，修改时间正常。
	::stat(path.c_str(), &file_stat);
	pnlFileInfo.lblAccessTime.Text = u"访问时间："
		+ String(TranslateTime(file_stat.st_atime));
	pnlFileInfo.lblModifiedTime.Text = u"修改时间："
		+ String(TranslateTime(file_stat.st_mtime));
	dsk_up += pnlFileInfo;
	HexArea.Load(path.c_str());
	HexArea.UpdateData(0);
	UpdateInfo();
	dsk_dn += HexArea;
	RequestFocusCascade(HexArea);
}

HexReaderSession::~HexReaderSession()
{
	auto& dsk_up(Shell.GetDesktopUp());
	auto& dsk_dn(Shell.GetDesktopDown());

	dsk_up -= pnlFileInfo;
	HexArea.Reset();
	dsk_dn -= HexArea;
}

void
HexReaderSession::UpdateInfo()
{
	char str[80];

	std::sprintf(str, "当前位置： %u / %u", HexArea.GetModel().GetPosition(),
		HexArea.GetModel().GetSize());
	pnlFileInfo.lblSize.Text = str;
	Invalidate(pnlFileInfo.lblSize);
}


IO::Path ShlReader::CurrentPath;
bool ShlReader::CurrentIsText(false);

ShlReader::ShlReader()
	: ShlDS(),
	hUp(), hDn(), pManager(), LastRead(), CurrentSetting()
{
	// TODO: use entity tree to store properties;
	yunseq(
		CurrentSetting.UpColor = Color(240, 216, 192),
		CurrentSetting.DownColor = Color(192, 216, 240)
	);
}

int
ShlReader::OnActivated(const Message& msg)
{
	ParentType::OnActivated(msg);

	auto& dsk_up(GetDesktopUp());
	auto& dsk_dn(GetDesktopDown());

	if(ShlReader::CurrentIsText)
		pManager = unique_raw(new TextReaderSession(*this));
	else
		pManager = unique_raw(new HexReaderSession(*this));
	std::swap(hUp, dsk_up.GetBackgroundImagePtr()),
	std::swap(hDn, dsk_dn.GetBackgroundImagePtr());
	SetInvalidationOf(dsk_up),
	SetInvalidationOf(dsk_dn);
	UpdateToScreen();
	return 0;
}

int
ShlReader::OnDeactivated()
{
	auto& dsk_up(GetDesktopUp());
	auto& dsk_dn(GetDesktopDown());

	std::swap(hUp, dsk_up.GetBackgroundImagePtr());
	std::swap(hDn, dsk_dn.GetBackgroundImagePtr());
	reset(pManager);
	ParentType::OnDeactivated();
	return 0;
}

YSL_END_NAMESPACE(YReader)

