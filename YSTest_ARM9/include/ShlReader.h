/*
	Copyright by FrankHB 2011 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ShlReader.h
\ingroup YReader
\brief Shell 阅读器框架。
\version r1782
\author FrankHB <frankhb1989@gmail.com>
\since build 263
\par 创建时间:
	2011-11-24 17:08:33 +0800
\par 修改时间:
	2013-08-05 19:49 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::ShlReader
*/


#ifndef INC_YReader_ShlReader_h_
#define INC_YReader_ShlReader_h_ 1

#include "Shells.h"
#include "DSReader.h"
#include "HexBrowser.h"
#include "ReaderSettingUI.h"
#include "BookmarkUI.h"

namespace YReader
{

class ReaderBox : public Control
{
public:
	//! \since build 357
	using Iterator = ystdex::subscriptive_iterator<ReaderBox, IWidget>;

	/*!
	\brief 弹出菜单按钮。
	\since build 274
	*/
	Button btnMenu;
	/*!
	\brief 设置按钮。
	\since build 330
	*/
	Button btnSetting;
	/*!
	\brief 显示信息框按钮。
	\since build 274
	*/
	Button btnInfo;
	/*!
	\brief 书签管理按钮。
	\since build 391
	*/
	Button btnBookmark;
	/*!
	\brief 返回按钮。
	\since build 274
	*/
	Button btnReturn;
	/*!
	\brief 后退按钮。
	\since build 286
	*/
	Button btnPrev;
	/*!
	\brief 前进按钮。
	\since build 286
	*/
	Button btnNext;
	ProgressBar pbReader;
	Label lblProgress;

	ReaderBox(const Rect&);

	//! \since build 357
	DefWidgetMemberIteration(btnMenu, btnSetting, btnInfo, btnBookmark,
		btnReturn, btnPrev, btnNext, pbReader, lblProgress)

private:
	//! \since build 374
	void
	InitializeProgress();

public:
	/*!
	\brief 更新进度数据。
	\since build 271
	*/
	void
	UpdateData(DualScreenReader&);
};


class TextInfoBox : public DialogBox
{
public:
	Label lblEncoding;
	Label lblSize;
	/*!
	\brief 指示迭代器位置。
	\since build 273
	*/
	//@{
	Label lblTop;
	Label lblBottom;
	//@}

	TextInfoBox();

	/*!
	\brief 刷新：按指定参数绘制界面并更新状态。
	\since build 294
	*/
	virtual void
	Refresh(PaintEventArgs&&);

	void
	UpdateData(DualScreenReader&);
};


class FileInfoPanel : public Panel
{
public:
	Label lblPath, lblSize, lblAccessTime, lblModifiedTime, lblOperations;

	FileInfoPanel();
};


class ShlReader : public ShlDS
{
protected:
	/*!
	\brief 当前路径。
	\since build 296
	*/
	IO::Path CurrentPath;
	/*!
	\brief 背景任务：用于滚屏。
	\since build 297
	*/
	std::function<void()> fBackgroundTask;
	/*!
	\brief 退出标识。
	\since build 302

	用于保证对一个实例只进行一次有效切换的标识。
	*/
	bool bExit;

public:
	/*!
	\brief 构造：使用指定路径和上下桌面。
	\sa ShlDS::ShlDS
	\since build 395
	*/
	ShlReader(const IO::Path&,
		const shared_ptr<Desktop>& = {}, const shared_ptr<Desktop>& = {});

	/*!
	\brief 退出阅读器：停止后台任务并发送消息准备切换至 ShlExplorer 。
	\since build 295
	*/
	void
	Exit();

	//! \since build 399
	static BookmarkList
	LoadBookmarks(const string&);

	/*!
	\brief 载入阅读器配置。
	\note 若失败则使用默认初始化。
	\since build 344
	*/
	static ReaderSetting
	LoadGlobalConfiguration();

	/*!
	\brief 处理输入消息：发送绘制消息，当处于滚屏状态时自动执行滚屏。
	\since build 289
	*/
	void
	OnInput() override;

	//! \since build 399
	static void
	SaveBookmarks(const string&, const BookmarkList&);

	/*!
	\brief 保存阅读器配置。
	\since build 344
	*/
	static void
	SaveGlobalConfiguration(const ReaderSetting&);
};


/*!
\brief 文本阅读器 Shell 。
\since build 296
*/
class ShlTextReader : public ShlReader
{
private:
	//! \since build 391
	//@{
	class BaseSession : public GShellSession<ShlTextReader>
	{
	private:
		//! \since build 416
		bool reader_box_shown;

	public:
		BaseSession(ShlTextReader&);
		~BaseSession() override;
	};
	class SettingSession : public BaseSession
	{
	public:
		SettingSession(ShlTextReader&);
		~SettingSession() override;
	};
	class BookmarkSession : public BaseSession
	{
	public:
		BookmarkSession(ShlTextReader&);
	};
	//@}

public:
	/*!
	\brief 近期浏览记录。
	\since build 296
	*/
	ReadingList& LastRead;
	/*!
	\brief 当前阅读器设置。
	\since build 334
	*/
	ReaderSetting CurrentSetting;

protected:
	/*!
	\brief 滚屏计时器。
	\since build 289
	*/
	Timers::Timer tmrScroll;
	/*!
	\brief 滚屏计时器激活状态。
	\since build 405
	*/
	bool tmrScrollActive;
	/*!
	\brief 按键计时器。
	\note 独立计时，排除路由事件干扰。
	\since build 300
	*/
	InputTimer tmrInput;
	/*!
	\brief 记录点击次数。
	\since build 416
	*/
	size_t nClick;
	//! \since build 323
	//@{
	DualScreenReader reader;
	ReaderBox boxReader;
	TextInfoBox boxTextInfo;
	//! \brief 设置面板。
	SettingPanel pnlSetting;
	unique_ptr<TextFile> pTextFile;
	MenuHost mhMain;
	//@}
	/*!
	\brief 书签管理面板。
	\since build 391
	*/
	BookmarkPanel pnlBookmark;

private:
	//! \since build 391
	unique_ptr<BaseSession> session_ptr;

public:
	/*!
	\brief 构造：使用指定路径和上下桌面。
	\sa ShlDS::ShlDS
	\since build 395
	*/
	ShlTextReader(const IO::Path&,
		const shared_ptr<Desktop>& = {}, const shared_ptr<Desktop>& = {});
	/*!
	\brief 析构：释放资源。
	\since build 286
	*/
	~ShlTextReader() override;

	//! \since build 394
	DefGetter(const ynothrow, Bookmark::PositionType, ReaderPosition,
		reader.GetTopPosition())

	//! \since build 397
	string
	GetSlice(Bookmark::PositionType, string::size_type);

private:
	/*!
	\brief 执行阅读器命令。
	\since build 274
	*/
	void
	Execute(IndexEventArgs::ValueType);

public:
	/*!
	\brief 读取文件。
	\note 不刷新按钮状态。
	\since build 286
	*/
	void
	LoadFile(const IO::Path&);

	/*!
	\brief 定位到文本中的指定位置：更新阅读器状态、阅读列表和按钮状态。
	\return 是否成功：在有效范围内且和原位置不同。
	\since build 394
	*/
	bool
	Locate(Bookmark::PositionType);

	/*!
	\brief 当自动滚屏有效状态为 true 时超时自动滚屏。
	\since build 289
	*/
	void
	Scroll();

private:
	void
	ShowMenu(Menu::ID, const Point&);

	/*!
	\brief 开始自动滚屏。
	\since build 416
	*/
	void
	StartAutoScroll();

	/*!
	\brief 停止自动滚屏。
	\since build 297
	*/
	void
	StopAutoScroll();

	/*!
	\brief 切换编码。
	\note 若文本文件无效、参数为 Encoding() 或与当前编码相同则忽略。
	\since build 292
	*/
	void
	Switch(Text::Encoding);

	/*!
	\brief 更新近期浏览记录并更新按钮状态。
	\param 是否后退。
	\since build 286
	*/
	void
	UpdateReadingList(bool);

public:
	/*!
	\brief 更新按钮状态。
	\note 检查近期浏览记录状态确定可用性。
	\since build 286
	*/
	void
	UpdateButtons();

private:
	//! \since build 422
	void
	OnClick(CursorEventArgs&&);

	void
	OnKeyDown(KeyEventArgs&&);
};


/*!
\brief 十六进制浏览器 Shell 。
\since build 296
*/
class ShlHexBrowser : public ShlReader
{
public:
	HexViewArea HexArea;
	FileInfoPanel pnlFileInfo;

	/*!
	\brief 构造：使用指定路径和上下屏幕。
	\sa ShlDS::ShlDS
	\since build 395
	*/
	ShlHexBrowser(const IO::Path&,
		const shared_ptr<Desktop>& = {}, const shared_ptr<Desktop>& = {});
};

} // namespace YReader;

#endif

