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
\version r2866
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-03-06 21:38:16 +0800
\par 修改时间:
	2012-12-31 18:35 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::Shells
*/


#ifndef INC_YREADER_SHELLS_H_
#define INC_YREADER_SHELLS_H_ 1

#include <Helper/shlds.h>
#include <Helper/ShellHelper.h>
#include <YCLib/Debug.h>

YSL_BEGIN

typedef decltype(__LINE__) ResourceIndex;
typedef map<ResourceIndex, ValueObject> ResourceMap;

#define DeclResource(_name) const ResourceIndex _name(__LINE__);

YSL_END


YSL_BEGIN_NAMESPACE(YReader)

#define G_COMP_NAME u"Franksoft"
#define G_APP_NAME u"YReader"
#define G_APP_VER u"Build PreAlpha"

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
\brief 关于窗体。
\since build 363
*/
class FrmAbout : public Form
{
protected:
	Label lblTitle, lblVersion, lblCopyright;
	Button btnClose, btnExit;

public:
	FrmAbout();
};


class ShlExplorer : public ShlDS
{
protected:
	//! \since build 363
	//@{
	Label lblTitle, lblPath;
	/*!
	\brief 信息标签：说明用户操作内容等。
	*/
	Label lblInfo;
	FileBox fbMain;
	Button btnTest, btnOK;
	Button btnMenu;
	DialogPanel pnlSetting;
	CheckButton cbHex, cbFPS;
	/*!
	\brief 复选按钮：指定开启切换背景时的预览。
	*/
	CheckButton cbPreview;
	Label lblDragTest;
	Button btnEnterTest, btnTestEx, btnPrevBackground, btnNextBackground;
	unique_ptr<FrmAbout> pFrmAbout;
	MenuHost mhMain;
	FPSCounter fpsCounter;
	//@}

public:
	/*!
	\brief 构造：使用指定路径。
	\pre 参数为合法路径。
	\since build 301
	*/
	ShlExplorer(const IO::Path& = IO::GetNowDirectory());

	/*!
	\brief 处理绘制消息：更新到屏幕并刷新 FPS 。
	\since build 289
	*/
	void
	OnPaint() override;

private:
	IWidget*
	GetBoundControlPtr(const KeyInput&);
};

YSL_END_NAMESPACE(YReader)

#endif

