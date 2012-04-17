/*
	Copyright (C) by Franksoft 2010 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup YReader YReader
\brief YReader 阅读器。
*/

/*!	\file Shells.h
\ingroup YReader
\brief Shell 框架逻辑。
\version r3680;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2010-03-06 21:38:16 +0800;
\par 修改时间:
	2012-04-17 09:24 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YReader::Shells;
*/


#ifndef INCLUDED_SHELLS_H_
#define INCLUDED_SHELLS_H_

#include <YSLib/Helper/shlds.h>
#include <YSLib/Helper/ShellHelper.h>
#include <YCLib/Debug.h>

YSL_BEGIN

typedef decltype(__LINE__) ResourceIndex;
typedef map<ResourceIndex, ValueObject> ResourceMap;

#define DeclResource(_name) const ResourceIndex _name(__LINE__);

YSL_END


YSL_BEGIN_NAMESPACE(YReader)

/*!
\defgroup YCL_KEY_OVERRIDE YCLib Key Overridden Values
\since build 299 。
*/
//@{
#ifndef YCL_DS
#	define YCL_KEY(X) 'X'
#	define YCL_KEY_Start 'P'
#else
#	define YCL_KEY(X) KeyCodes::X
#	define YCL_KEY_Start KeyCodes::Start
#endif
//@}

//全局常量。
//extern CPATH DEF_DIRECTORY;

using namespace YSLib;

using namespace Components;
using namespace Drawing;

using namespace DS;
using namespace DS::Components;

using platform::WaitForInput;
using platform::YDebugSetStatus;


shared_ptr<Image>&
FetchImage(size_t);


/*!
\brief 从全局消息队列中移除所有从属于指定 Shell 的后台 SM_TASK 消息。
\since build 295 。
*/
void
RemoveGlobalTasks(Shell&);


/*!
\brief 帧速率计数器。
*/
class FPSCounter
{
private:
	/*!
	\brief 内部计数。
	\note 单位为纳秒。
	\since build 291 。
	*/
	//@{
	u64 last_tick;
	u64 now_tick;
	//@}
	/*!
	\brief 刷新计数。
	\since build 295 。
	*/
	u32 refresh_count;

public:
	/*!
	\brief 计时间隔下界。
	\since build 295 。
	*/
	u64 MinimalInterval;

	/*!
	\brief 构造：使用指定计时间隔下界。
	\since build 295 。
	*/
	FPSCounter(u64 = 0);

	/*!
	\brief 取内部计数。
	\since build 291 。
	*/
	//@{
	DefGetter(const ynothrow, u64, LastTick, last_tick)
	DefGetter(const ynothrow, u64, NowTick, now_tick)
	//@}

	/*!
	\brief 刷新：更新计数器内部计数。
	\return 内部计数差值大于计时间隔下界时的每秒毫计数次数；否则为 0 。
	*/
	u32
	Refresh();
};


class ShlExplorer : public ShlDS
{
public:
	typedef ShlDS ParentType;

	struct TFormTest : public Form
	{
		Button btnEnterTest, btnMenuTest, btnShowWindow,
			btnPrevBackground, btnNextBackground;

		TFormTest();
	};

	struct TFormExtra : public Form
	{
		Button btnDragTest;
		Button btnTestEx;
		Button btnClose;
		Button btnExit;

		TFormExtra();
	};

public:
	Label lblTitle, lblPath;
	FileBox fbMain;
	Button btnTest, btnOK;
	CheckBox chkFPS, chkHex;
	unique_ptr<TFormTest> pWndTest;
	unique_ptr<TFormExtra> pWndExtra;
	Label lblA, lblB;
	MenuHost mhMain;
	FPSCounter fpsCounter;

	/*!
	\brief 构造：使用指定路径。
	\pre 参数为合法路径。
	\since build 301 。
	*/
	ShlExplorer(const IO::Path& = IO::GetNowDirectory());

	/*!
	\brief 处理绘制消息：更新到屏幕并刷新 FPS 。
	\since build 289 。
	*/
	virtual void
	OnPaint();

private:
	IWidget*
	GetBoundControlPtr(const KeyInput&);

	static void
	OnClick_ShowWindow(TouchEventArgs&&);
};

YSL_END_NAMESPACE(YReader)

#endif

