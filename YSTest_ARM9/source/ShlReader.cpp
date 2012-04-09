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
\version r4024;
\author FrankHB<frankhb1989@gmail.com>
\since build 263 。
\par 创建时间:
	2011-11-24 17:13:41 +0800;
\par 修改时间:
	2012-04-08 14:51 +0800;
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

	/*!
	\brief 编码信息项目。
	\since build 290 。
	*/
	struct EncodingInfoItem
	{
		typedef Text::Encoding first_type;
		typedef const ucs2_t* second_type;

		first_type first;
		second_type second;
	};


	using namespace Text::CharSet;

	/*!
	\brief 编码信息。
	\since build 290 。
	*/
	yconstexpr EncodingInfoItem Encodings[] = {{UTF_8, u"UTF-8"}, {GBK, u"GBK"},
		{UTF_16BE, u"UTF-16 Big Endian"}, {UTF_16LE, u"UTF-16 Little Endian"},
		{UTF_32BE, u"UTF-32 Big Endian"}, {UTF_32LE, u"UTF-16 Little Endian"}};

	/*!
	\since build 290 。
	*/
	shared_ptr<TextList::ListType>
	FetchEncodingNames()
	{
		// TODO: use g++ 4.7 later;
	//	return make_shared<TextList::ListType>(Encodings | ystdex::get_value,
	//		(Encodings + arrlen(Encodings)) | ystdex::get_value);
		return share_raw(new TextList::ListType(Encodings | ystdex::get_value,
			(Encodings + arrlen(Encodings)) | ystdex::get_value));
	}

	/*!
	\brief 取编码字符串。
	\since build 290 。
	*/
	String
	FetchEncodingString(MTextList::IndexType i)
	{
		if(YCL_LIKELY(i < arrlen(Encodings)))
		{
			const auto enc(Encodings[i].first);
			const String ustr(Encodings[i].second);
			char str[32];

			std::sprintf(str, "%d: %s", enc,
				ustr.GetMBCS().c_str());

			return String(str);
		}
		return u"---";
	}


	shared_ptr<TextList::ListType>
	FetchFontFamilyNames()
	{
		const auto& mFamilies(FetchGlobalInstance().GetFontCache()
			.GetFamilyIndices());

		// TODO: use g++ 4.7 later;
	//	return make_shared<TextList::ListType>(mFamilies.cbegin()
	//		| ystdex::get_key, mFamilies.cend() | ystdex::get_key);
		return share_raw(new TextList::ListType(mFamilies.cbegin()
			| ystdex::get_key, mFamilies.cend() | ystdex::get_key));
	}


	/*!
	\since build 292 。
	*/
	shared_ptr<TextList::ListType>
	FetchScrollDurations(bool is_smooth)
	{
		const auto postfix(is_smooth ? u"毫秒/像素行" : u"毫秒/文本行");
		auto& lst(*new TextList::ListType(10U));
		const u16 delta(is_smooth ? 20 : 200);
		u16 t(delta);
		char str[10];

		std::generate(lst.begin(), lst.end(), [&, is_smooth, delta]{
			std::sprintf(str, "%u", t += delta);
			return String(str) + postfix;
		});
		return share_raw(&lst);
	}


	/*!
	\since build 292 。
	*/
	enum GRLs
	{
		GRL_EncodingNames,
		GRL_ScrollDurations,
		GRL_SmoothScrollDurations
	};


	/*!
	\since build 292 。
	*/
	shared_ptr<TextList::ListType>
	QueryList(enum GRLs res_id)
	{
		static shared_ptr<TextList::ListType> handles[3] = {
			FetchEncodingNames(), FetchScrollDurations(false),
			FetchScrollDurations(true)};

		switch(res_id)
		{
		case GRL_EncodingNames:
			return handles[0];
		case GRL_ScrollDurations:
			return handles[1];
		case GRL_SmoothScrollDurations:
			return handles[2];
		}

		YAssert(false, "No resources found @ QueryList;");

		return nullptr;
	};

	/*!
	\brief 更新列表。
	\since build 293 。
	*/
	void
	UpdateScrollDropDownList(DropDownList& ddl, bool b, const milliseconds& d,
		const milliseconds& d_s)
	{
		ddl.SetList(QueryList(b ? GRL_SmoothScrollDurations
			: GRL_ScrollDurations));
		ddl.Text = ddl.GetList()[(b ? d_s.count() / 20U : d.count() / 200U)
			- 2U],
		Invalidate(ddl);
	}

	/*!
	\since build 293 。
	*/
	milliseconds
	FetchTimerSetting(const ReaderSetting& s)
	{
		return s.SmoothScroll ? s.SmoothScrollDuration
			: s.ScrollDuration;
	}

	ReadingList&
	FetchLastRead()
	{
		static ReadingList* p;

		if(!p)
			p = new ReadingList();
		return *p;
	}

	/*!
	\brief 取当前设置。
	*/
	ReaderSetting&
	FetchCurrentSetting()
	{
		static ReaderSetting* p;

		if(!p)
		{
			//初始化设置。
			p = new ReaderSetting();
			p->Font.SetSize(14),
			yunseq(
				p->UpColor = Color(240, 216, 192),
				p->DownColor = Color(192, 216, 240),
				p->SmoothScroll = true,
				p->ScrollDuration = milliseconds(1000),
				p->SmoothScrollDuration = milliseconds(80)
			);
		}
		return *p;
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

		if(YCL_UNLIKELY(!p))
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


ReaderBox::ReaderBox(const Rect& r)
	: Control(r),
	btnMenu(Rect(4, 12, 16, 16)),
	btnInfo(Rect(24, 12, 16, 16)), btnReturn(Rect(44, 12, 16, 16)),
	btnPrev(Rect(64, 12, 16, 16)), btnNext(Rect(84, 12, 16, 16)),
	pbReader(Rect(4, 0, 248, 8)), lblProgress(Rect(216, 12, 40, 16))
{
	SetTransparent(true),
	SetRenderer(make_unique<BufferedRenderer>()),
	seq_apply(ContainerSetter(*this),
		btnMenu, btnInfo, btnReturn, btnPrev, btnNext, pbReader, lblProgress);
	btnMenu.SetRenderer(make_unique<BufferedRenderer>()),
	btnMenu.Text = "M",
	btnInfo.SetRenderer(make_unique<BufferedRenderer>()),
	btnInfo.Text = "I",
	btnReturn.SetRenderer(make_unique<BufferedRenderer>()),
	btnReturn.Text = "R",
	btnPrev.SetRenderer(make_unique<BufferedRenderer>()),
	btnPrev.Text = "←",
	btnNext.SetRenderer(make_unique<BufferedRenderer>()),
	btnNext.Text = "→",
	pbReader.ForeColor = Color(192, 192, 64),
	lblProgress.SetRenderer(make_unique<BufferedRenderer>()),
	lblProgress.SetTransparent(true),
	lblProgress.Font.SetSize(12);
	yunseq(lblProgress.Text = u"--%", lblProgress.ForeColor = ColorSpace::Blue);
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

	for(std::size_t i(0); i < arrlen(pWidgets); ++i)
		if(auto p = CheckWidget(*pWidgets[i], pt, f))
			return p;
	return nullptr;
}

void
ReaderBox::Refresh(PaintEventArgs&& e)
{
	seq_apply(ChildPainter(e),
		btnMenu, btnInfo, btnReturn, btnPrev, btnNext, pbReader, lblProgress);
	e.ClipArea = Rect(e.Location, GetSizeOf(*this));
}

void
ReaderBox::UpdateData(DualScreenReader& reader)
{
	const auto ts(reader.GetTextSize());

	if(YCL_LIKELY(ts != 0))
	{
		const auto tp(reader.GetTopPosition());
		char str[4];

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
	seq_apply(ContainerSetter(*this), lblEncoding, lblSize);
	FetchEvent<TouchMove>(*this) += OnTouchMove_Dragging;
}

void
TextInfoBox::Refresh(PaintEventArgs&& e)
{
	DialogBox::Refresh(std::move(e));

	seq_apply(ChildPainter(e), lblEncoding, lblSize, lblTop, lblBottom);
	e.ClipArea = Rect(e.Location, GetSizeOf(*this));
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
	: DialogPanel(Rect(Point::Zero, MainScreenWidth, MainScreenHeight)),
	lblAreaUp(Rect(20, 12, 216, 72)), lblAreaDown(Rect(20, 108, 216, 72)),
	btnFontSizeDecrease(Rect(20, 32, 80, 22)),
	btnFontSizeIncrease(Rect(148, 32, 80, 22)),
	btnSetUpBack(Rect(20, 64, 80, 22)), btnSetDownBack(Rect(148, 64, 80, 22)),
	btnTextColor(Rect(20, 96, 80, 22)),
	ddlFont(Rect(148, 96, 80, 22), FetchFontFamilyNames()),
	ddlEncoding(Rect(20, 128, 192, 22), QueryList(GRL_EncodingNames)),
	chkSmoothScroll(Rect(20, 160, 16, 16)),
	lblSmoothScroll(Rect(36, 160, 60, 20)),
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

	*this += btnFontSizeDecrease,
	*this += btnFontSizeIncrease,
	*this += btnSetUpBack,
	*this += btnSetDownBack,
	*this += btnTextColor,
	*this += ddlFont,
	*this += ddlEncoding,
	*this += chkSmoothScroll,
	*this += lblSmoothScroll,
	*this += ddlScrollTiming,
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
		FetchEvent<Click>(btnFontSizeDecrease) += [&, this](TouchEventArgs&&){
			auto size(lblAreaUp.Font.GetSize());

			if(YCL_LIKELY(size > Font::MinimalSize))
				set_font_size(--size);
		},
		FetchEvent<Click>(btnFontSizeIncrease) += [&, this](TouchEventArgs&&){
			auto size(lblAreaUp.Font.GetSize());

			if(YCL_LIKELY(size < Font::MaximalSize))
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
		chkSmoothScroll.GetTicked()
			+= [this](CheckBox::TickedArgs&& e){
			auto& ddl(ddlScrollTiming);

			UpdateScrollDropDownList(ddl, e.Value, scroll_duration,
				smooth_scroll_duration);
			Invalidate(ddl);
		},
		ddlScrollTiming.GetConfirmed() += [this](IndexEventArgs&& e){
			if(chkSmoothScroll.IsTicked())
				smooth_scroll_duration = milliseconds((e.Value + 2U) * 20);
			else
				scroll_duration = milliseconds((e.Value + 2U) * 200);
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
	chkSmoothScroll.SetTicked(s.SmoothScroll);
	UpdateScrollDropDownList(ddlScrollTiming, s.SmoothScroll, s.ScrollDuration,
		s.SmoothScrollDuration);
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
	char str[20];

	/*std::*/snprintf(str, 20, "%u 。", lblAreaUp.Font.GetSize());

	String ustr(str);

	lblAreaUp.Text = u"上屏文字大小: " + ustr;
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


ShlReader::ShlReader(const IO::Path& pth)
	: ShlDS(),
	CurrentPath(pth), fBackgroundTask()
{}

void
ShlReader::Exit()
{
	fBackgroundTask = nullptr;
	SetShellToNew<ShlExplorer>();
}

void
ShlReader::OnInput()
{
	ShlDS::OnInput();
	if(fBackgroundTask)
		PostMessage<SM_TASK>(FetchShellHandle(), 0x20, fBackgroundTask);
}


ShlTextReader::ShlTextReader(const IO::Path& pth)
	: ShlReader(pth),
	LastRead(FetchLastRead()), CurrentSetting(FetchCurrentSetting()),
	tmrScroll(FetchTimerSetting(CurrentSetting)), Reader(),
	boxReader(Rect(0, 160, 256, 32)), boxTextInfo(), pnlSetting(),
	pTextFile(), mhMain(GetDesktopDown())
{
	using ystdex::get_key;

	const auto exit_setting([this](TouchEventArgs&&){
		auto& dsk_up(GetDesktopUp());

		yunseq(dsk_up.Background = pnlSetting.lblAreaUp.Background,
			GetDesktopDown().Background
			= pnlSetting.lblAreaDown.Background,
		dsk_up -= pnlSetting.lblAreaUp,
		dsk_up -= pnlSetting.lblAreaDown);
		Reader.SetVisible(true),
		boxReader.UpdateData(Reader),
		boxTextInfo.UpdateData(Reader),
		Show(boxReader);
	});

	SetVisibleOf(boxReader, false),
	SetVisibleOf(boxTextInfo, false),
	SetVisibleOf(pnlSetting, false);
	yunseq(
		Reader.ViewChanged = [this]
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
			const auto s(Reader.GetTextSize());

			if(YCL_LIKELY(s != 0))
			{
				const auto old_pos(Reader.GetTopPosition());

				Reader.Locate(e.X * s / boxReader.pbReader.GetWidth());
				if(YCL_LIKELY(old_pos != Reader.GetTopPosition()))
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
				pt.Y + 1), Size(round((Reader.GetBottomPosition()
				- Reader.GetTopPosition()) * w / mval), pb.GetHeight() - 2),
				ColorSpace::Yellow);
		},
		FetchEvent<Click>(pnlSetting.btnClose) += exit_setting,
		FetchEvent<Click>(pnlSetting.btnOK) += [&, this](TouchEventArgs&&)
		{
			Switch(pnlSetting.current_encoding);
			Reader.SetColor(pnlSetting.lblAreaUp.ForeColor),
			Reader.SetFont(pnlSetting.lblAreaUp.Font);
			Reader.UpdateView();
			pnlSetting >> CurrentSetting;
			tmrScroll.SetInterval(FetchTimerSetting(CurrentSetting));
		},
		FetchEvent<Click>(pnlSetting.btnOK) += exit_setting
	);
	{
		auto hList(make_shared<Menu::ListType>());
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

	auto& dsk_up(GetDesktopUp());
	auto& dsk_dn(GetDesktopDown());

	Reader.SetColor(CurrentSetting.FontColor),
	Reader.SetFont(CurrentSetting.Font),
	yunseq(
		dsk_up.Background = SolidBrush(CurrentSetting.UpColor),
		dsk_dn.Background = SolidBrush(CurrentSetting.DownColor),
		FetchEvent<Click>(dsk_dn).Add(*this, &ShlTextReader::OnClick),
		FetchEvent<KeyDown>(dsk_dn).Add(*this, &ShlTextReader::OnKeyDown),
		FetchEvent<KeyHeld>(dsk_dn) += OnKeyHeld
	);
	Reader.Attach(dsk_up, dsk_dn),
	dsk_dn += boxReader,
	dsk_dn += boxTextInfo,
	dsk_dn += pnlSetting;
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
	LastRead.Insert(CurrentPath, Reader.GetTopPosition());

	auto& dsk_up(GetDesktopUp());
	auto& dsk_dn(GetDesktopDown());

	yunseq(
		CurrentSetting.UpColor
			= dsk_up.Background.target<SolidBrush>()->Color,
		CurrentSetting.DownColor
			= dsk_dn.Background.target<SolidBrush>()->Color,
		CurrentSetting.FontColor = Reader.GetColor(),
		CurrentSetting.Font = Reader.GetFont(),
		FetchEvent<Click>(dsk_dn).Remove(*this, &ShlTextReader::OnClick),
		FetchEvent<KeyDown>(dsk_dn).Remove(*this,
			&ShlTextReader::OnKeyDown),
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
ShlTextReader::Execute(IndexEventArgs::ValueType idx)
{
	switch(idx)
	{
	case MR_Return:
		Exit();
		break;
	case MR_Setting:
		Reader.SetVisible(false),
		yunseq(
			pnlSetting.lblAreaUp.ForeColor = Reader.GetColor(),
			pnlSetting.lblAreaUp.Background = GetDesktopUp().Background,
			pnlSetting.lblAreaUp.Font = Reader.GetFont(),
			pnlSetting.lblAreaDown.ForeColor = Reader.GetColor(),
			pnlSetting.lblAreaDown.Background
				= GetDesktopDown().Background,
			pnlSetting.lblAreaDown.Font = Reader.GetFont(),
			pnlSetting.ddlFont.Text = Reader.GetFont().GetFamilyName()
		);
		pnlSetting.UpdateInfo();
		{
			auto& dsk_up(GetDesktopUp());

			dsk_up.Background = SolidBrush(ColorSpace::White);
			dsk_up += pnlSetting.lblAreaUp,
			dsk_up += pnlSetting.lblAreaDown;
		}
		{
			const auto idx(std::find(Encodings | get_key,
				(Encodings + arrlen(Encodings)) | get_key,
				Reader.GetEncoding()) - Encodings);

			yunseq(pnlSetting.lblAreaDown.Text = FetchEncodingString(idx),
				pnlSetting.ddlEncoding.Text = Encodings[idx].second);
		}
		StopAutoScroll(),
		Hide(boxReader),
		Show(pnlSetting << CurrentSetting);
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
ShlTextReader::LoadFile(const IO::Path& pth)
{
	CurrentPath = pth;
	pTextFile = make_unique<TextFile>(pth.GetNativeString().c_str());
	Reader.LoadText(*pTextFile);
}

void
ShlTextReader::Scroll()
{
	if(tmrScroll.IsActive())
		if(YCL_UNLIKELY(tmrScroll.Refresh()))
		{
			if(CurrentSetting.SmoothScroll)
				Reader.ScrollByPixel(1U);
			else
				Reader.Execute(DualScreenReader::LineDownScroll);
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
			mnu.SetItemEnabled(MR_LineUp, !Reader.IsTextTop());
			mnu.SetItemEnabled(MR_LineDown, !Reader.IsTextBottom());
			mnu.SetItemEnabled(MR_ScreenUp, !Reader.IsTextTop());
			mnu.SetItemEnabled(MR_ScreenDown, !Reader.IsTextBottom());
		}
		mhMain.Show(id);
	}
}

void
ShlTextReader::StopAutoScroll()
{
	Reader.AdjustScrollOffset(),
	fBackgroundTask = nullptr,
	Deactivate(tmrScroll);
}

void
ShlTextReader::Switch(Encoding enc)
{
	if(enc != Encoding() && pTextFile && pTextFile->IsValid()
		&& pTextFile->Encoding != enc)
	{
		pTextFile->Encoding = enc;
		Reader.LoadText(*pTextFile);
	}
}

void
ShlTextReader::UpdateReadingList(bool is_prev)
{
	LastRead.Insert(CurrentPath, Reader.GetTopPosition());

	const auto& bm(LastRead.Switch(is_prev));

	if(bm.Path != CurrentPath)
		LoadFile(bm.Path);
	Reader.Locate(bm.Position);
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
		Reader.Stretch(0);
	}
	else
	{
		Show(boxReader);
		Reader.Stretch(boxReader.GetHeight());
	}
}

void
ShlTextReader::OnKeyDown(KeyEventArgs&& e)
{
	if(e.Strategy != RoutedEventArgs::Tunnel && !mhMain.IsShowing(1u))
	{
		using namespace KeyCodes;

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
			Reader.UpdateView();
		else if(k[Esc])
			Exit();
		else if(k[Up])
			Reader.Execute(DualScreenReader::LineUpScroll);
		else if(k[Down])
			Reader.Execute(DualScreenReader::LineDownScroll);
		else if(k[YCL_KEY(X)] || k[YCL_KEY(Y)])
		{
			auto size(Reader.GetFont().GetSize());

			if(k[YCL_KEY(X)] && size > Font::MinimalSize)
				--size;
			else if(k[YCL_KEY(Y)] && size < Font::MaximalSize)
				++size;
			else
				return;
			Reader.SetFontSize(size);
			Reader.UpdateView();
		}
		else if(k[YCL_KEY(L)])
		{
			if(YCL_LIKELY(Reader.GetLineGap() != 0))
				Reader.SetLineGap(Reader.GetLineGap() - 1);
		}
		else if(k[YCL_KEY(R)])
		{
			if(YCL_LIKELY(Reader.GetLineGap() != 12))
				Reader.SetLineGap(Reader.GetLineGap() + 1);
		}
		else if(k[Left])
			Reader.Execute(DualScreenReader::ScreenUpScroll);
		else if(k[Right])
			Reader.Execute(DualScreenReader::ScreenDownScroll);
/*
		case KeySpace::PgUp:
			Reader.Execute(DualScreenReader::ScreenUpScroll);
			break;
		case KeySpace::PgDn:
			Reader.Execute(DualScreenReader::ScreenDownScroll);
			break;
*/
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
			char str[80];

			std::sprintf(str, "当前位置： %u / %u",
				HexArea.GetModel().GetPosition(), HexArea.GetModel().GetSize());
			pnlFileInfo.lblSize.Text = str;
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
	pnlFileInfo.lblAccessTime.Text = u"访问时间："
		+ String(TranslateTime(file_stat.st_atime));
	pnlFileInfo.lblModifiedTime.Text = u"修改时间："
		+ String(TranslateTime(file_stat.st_mtime));
	dsk_up += pnlFileInfo;
	HexArea.Load(path_str.c_str());
	HexArea.UpdateData(0);
	HexArea.ViewChanged(HexViewArea::ViewArgs(HexArea, true));
	dsk_dn += HexArea;
	RequestFocusCascade(HexArea);
}

ShlHexBrowser::~ShlHexBrowser()
{
	auto& dsk_up(GetDesktopUp());
	auto& dsk_dn(GetDesktopDown());

	dsk_up -= pnlFileInfo;
	HexArea.Reset();
	dsk_dn -= HexArea;
}

YSL_END_NAMESPACE(YReader)

