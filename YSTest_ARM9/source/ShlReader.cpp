/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ShlReader.cpp
\ingroup YReader
\brief Shell 阅读器框架。
\version r2564;
\author FrankHB<frankhb1989@gmail.com>
\since build 263 。
\par 创建时间:
	2011-11-24 17:13:41 +0800;
\par 修改时间:
	2011-11-30 14:44 +0800;
\par 字符集:
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
		MR_Panel(1),
		MR_FileInfo(2),
		MR_LineUp(3),
		MR_LineDown(4),
		MR_ScreenUp(5),
		MR_ScreenDown(6);
}


ReaderBox::ReaderBox(const Rect& r, ShlReader& shl)
	: AUIBoxControl(r),
	Shell(shl), btnClose(Rect(232, 4, 16, 16)),
	trReader(Rect(8, 4, 192, 16)), lblProgress(Rect(204, 4, 24, 16))
{
	btnClose.GetView().pContainer = this;
	trReader.GetView().pContainer = this;
	lblProgress.GetView().pContainer = this;
	btnClose.Text = "×";
	lblProgress.Text = "0%";
	lblProgress.ForeColor = ColorSpace::Fuchsia;
	lblProgress.Font.SetSize(12);
	FetchEvent<Click>(btnClose) += [this](TouchEventArgs&&){
		Hide(*this);
	};
}

IWidget*
ReaderBox::GetTopWidgetPtr(const Point& pt,
	bool(&f)(const IWidget&))
{
/*
#define DefTuple(_n, ...) \
	const auto _n = std::make_tuple(__VA_ARGS__);

DefTuple(pWidgets,
		&ShlReader::ReaderPanel::btnClose,
		&ShlReader::ReaderPanel::trReader,
		&ShlReader::ReaderPanel::lblProgress
	)
*/
	IWidget* const pWidgets[] = {&btnClose, &trReader, &lblProgress};

	for(int i(0); i < 3; ++i)
		if(auto p = CheckWidget(*pWidgets[i], pt, f))
			return p;
	return nullptr;
}

Rect
ReaderBox::Refresh(const PaintContext& pc)
{
	Widget::Refresh(pc);

	IWidget* const pWidgets[] = {&btnClose, &trReader, &lblProgress};

	for(int i(0); i < 3; ++i)
		PaintChild(*pWidgets[i], pc);
	return Rect(pc.Location, GetSizeOf(*this));
}


TextInfoBox::TextInfoBox(const Rect& r, ShlReader& shl)
	: AUIBoxControl(r),
	Shell(shl), btnClose(Rect(GetWidth() - 20, 4, 16, 16)),
	lblInfo(Rect(4, 20, GetWidth() - 8, GetHeight() - 24))
{
	btnClose.GetView().pContainer = this;
	lblInfo.GetView().pContainer = this;
	btnClose.Text = "×";
	FetchEvent<Click>(btnClose) += [this](TouchEventArgs&&){
		Hide(*this);
	};
	FetchEvent<TouchMove>(*this) += OnTouchMove_Dragging;
}

IWidget*
TextInfoBox::GetTopWidgetPtr(const Point& pt, bool(&f)(const IWidget&))
{
	IWidget* const pWidgets[] = {&btnClose, &lblInfo};

	for(int i(0); i < 2; ++i)
		if(auto p = CheckWidget(*pWidgets[i], pt, f))
			return p;
	return nullptr;
}

Rect
TextInfoBox::Refresh(const PaintContext& pc)
{
	Widget::Refresh(pc);
	IWidget* const pWidgets[] = {&btnClose, &lblInfo};

	for(int i(0); i < 2; ++i)
		PaintChild(*pWidgets[i], pc);
	return Rect(pc.Location, GetSizeOf(*this));
}

void
TextInfoBox::UpdateData(DualScreenReader& reader)
{
	char str[20];

	siprintf(str, "Encoding: %d;", reader.GetEncoding());
	lblInfo.Text = str;
}


FileInfoPanel::FileInfoPanel()
	: Panel(Rect::FullScreen),
	lblPath(Rect(8, 20, 240, 16)),
	lblSize(Rect(8, 40, 240, 16)),
	lblAccessTime(Rect(8, 60, 240, 16)),
	lblModifiedTime(Rect(8, 80, 240, 16))
{
	BackColor = ColorSpace::Silver;
	*this += lblPath,
	*this += lblSize,
	*this += lblAccessTime,
	*this += lblModifiedTime;
}


string ReaderManager::path;
bool ReaderManager::is_text(false);

ReaderManager::ReaderManager(ShlReader& shl)
	: Shell(shl)
{}

TextReaderManager::TextReaderManager(ShlReader& shl)
	: ReaderManager(shl),
	Reader(), boxReader(Rect(0, 168, 256, 24), shl),
	boxTextInfo(Rect(32, 32, 128, 64), shl),
	pTextFile(), mhMain(shl.GetDesktopDown())
{
	{
		auto hList(share_raw(new Menu::ListType));
		auto& lst(*hList);

		lst.reserve(7);
		lst.push_back("返回");
		lst.push_back("显示面板");
		lst.push_back("文件信息...");
		lst.push_back("向上一行");
		lst.push_back("向下一行");
		lst.push_back("向上一屏");
		lst.push_back("向下一屏");

		Menu& mnu(*(ynew Menu(Rect::Empty, std::move(hList), 1u)));

		mnu.GetConfirmed() += [this](IndexEventArgs&& e){
			ExcuteReadingCommand(e.Index);
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

	yunsequenced(
		dsk_up.BackColor = Color(240, 216, 192),
		dsk_dn.BackColor = Color(192, 216, 240)
	);
	yunsequenced(
		FetchEvent<Click>(dsk_dn).Add(*this, &TextReaderManager::OnClick),
		FetchEvent<KeyDown>(dsk_dn).Add(*this, &TextReaderManager::OnKeyDown),
		FetchEvent<KeyHeld>(dsk_dn) += OnKeyHeld
	);
	dsk_up += Reader.AreaUp,
	dsk_dn += Reader.AreaDown,
	dsk_dn += boxReader,
	dsk_dn += boxTextInfo;
	SetVisibleOf(boxReader, false),
	SetVisibleOf(boxTextInfo, false);
	pTextFile = ynew TextFile(path.c_str());
	Reader.LoadText(*pTextFile);
}

void
TextReaderManager::Deactivate()
{
	auto& dsk_up(Shell.GetDesktopUp());
	auto& dsk_dn(Shell.GetDesktopDown());

	Reader.UnloadText();
	safe_delete_obj()(pTextFile);
	mhMain.Clear();
	yunsequenced(
		FetchEvent<Click>(dsk_dn).Remove(*this, &TextReaderManager::OnClick),
		FetchEvent<KeyDown>(dsk_dn).Remove(*this,
			&TextReaderManager::OnKeyDown),
		FetchEvent<KeyHeld>(dsk_dn) -= OnKeyHeld
	);
	yunsequenced(
		dsk_up -= Reader.AreaUp,
		dsk_dn -= Reader.AreaDown,
		dsk_up -= boxReader,
		dsk_dn -= boxTextInfo
	);
}

void
TextReaderManager::ExcuteReadingCommand(IndexEventArgs::IndexType idx)
{
	switch(idx)
	{
	case MR_Return:
		CallStored<ShlExplorer>();
		break;
	case MR_Panel:
		Show(boxReader);
		break;
	case MR_FileInfo:
		boxTextInfo.UpdateData(Reader);
		Show(boxTextInfo);
		break;
	case MR_LineUp:
		Reader.LineUp();
		break;
	case MR_LineDown:
		Reader.LineDown();
		break;
	case MR_ScreenUp:
		Reader.ScreenUp();
		break;
	case MR_ScreenDown:
		Reader.ScreenDown();
		break;
	}
}

void
TextReaderManager::ShowMenu(Menu::ID id, const Point& pt)
{
	auto& mnu(mhMain[id]);

	SetLocationOf(mnu, pt);
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

void
TextReaderManager::OnClick(TouchEventArgs&& e)
{
	ShowMenu(1u, e);
}

void
TextReaderManager::OnKeyDown(KeyEventArgs&& e)
{
	if(e.Strategy == RoutedEventArgs::Direct)
	{
		u32 k(static_cast<KeyEventArgs::InputType>(e));

		switch(k)
		{
		case KeySpace::Enter:
			//Reader.Invalidate();
			Reader.Update();
			break;
		case KeySpace::Esc:
			CallStored<ShlExplorer>();
			break;
		case KeySpace::Up:
		case KeySpace::Down:
		case KeySpace::PgUp:
		case KeySpace::PgDn:
			{
				switch(k)
				{
				case KeySpace::Up:
					Reader.LineUp();
					break;
				case KeySpace::Down:
					Reader.LineDown();
					break;
				case KeySpace::PgUp:
					Reader.ScreenUp();
					break;
				case KeySpace::PgDn:
					Reader.ScreenDown();
					break;
				}
			}
			break;
		case KeySpace::X:
			Reader.SetLineGap(5);
			Reader.Update();
			break;
		case KeySpace::Y:
			Reader.SetLineGap(8);
			Reader.Update();
			break;
		case KeySpace::Left:
			//Reader.SetFontSize(Reader.GetFontSize()+1);
			if(Reader.GetLineGap() != 0)
			{
				Reader.SetLineGap(Reader.GetLineGap() - 1);
				Reader.Update();
			}
			break;
		case KeySpace::Right:
			if(Reader.GetLineGap() != 12)
			{
				Reader.SetLineGap(Reader.GetLineGap() + 1);
				Reader.Update();
			}
			break;
		default:
			return;
		}
	}
}


HexReaderManager::HexReaderManager(ShlReader& shl)
	: ReaderManager(shl),
	HexArea(Rect::FullScreen), pnlFileInfo()
{
	HexArea.SetRenderer(unique_raw(new BufferedRenderer()));
	yunsequenced(
		FetchEvent<Click>(HexArea) += [](TouchEventArgs&&){
			CallStored<ShlExplorer>();
		},
		FetchEvent<Paint>(HexArea) += [this](PaintEventArgs&&){
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
		::sniprintf(buf, n, format, tm.tm_year + 1900,
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
		+ Text::MBCSToString(ReaderManager::path);

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
	dsk_dn += HexArea;
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

	::siprintf(str, "当前位置： %u / %u", HexArea.GetSource().GetPosition(),
		HexArea.GetSource().GetSize());
	pnlFileInfo.lblSize.Text = Text::MBCSToString(str, Text::CP_Default);
	Invalidate(pnlFileInfo);
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
	RequestFocusCascade(dsk_dn);
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

/*
void ShlReader::OnPaint(EventArgs&&)
{
	Reader.PrintTextUp(FetchContext(GetDesktopUp()));
	Reader.PrintTextDown(FetchContext(GetDesktopDown()));
}
*/

YSL_END_NAMESPACE(YReader)

