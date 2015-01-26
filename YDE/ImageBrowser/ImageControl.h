/*
	© 2013-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ImageControl.h
\ingroup UI
\brief 图像显示控件。
\version r599
\author FrankHB <frankhb1989@gmail.com>
\since build 436
\par 创建时间:
	2013-08-13 12:48:27 +0800
\par 修改时间:
	2015-01-25 13:51 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	ImageBrowser::ImageControl
*/


#ifndef INC_ImageBrowser_ImageControl_h_
#define INC_ImageBrowser_ImageControl_h_ 1

#include <ysbuild.h>

namespace ImageBrowser
{

using namespace YSLib;
using namespace Drawing;
using namespace UI;

//! \since build 555
yconstexpr const Size DefaultMinPanelSize(320, 240);
//! \since build 555
yconstexpr const Size DefaultMaxPanelSize(1200, 800);

class ImagePanel : public Panel
{
private:
	using Session = tuple<ImagePages, GAnimationSession<InvalidationUpdater>,
		Timers::Timer, vector<std::chrono::milliseconds>>;

	//! \since build 555
	unique_ptr<Session> session_ptr{};
	//! \since build 555
	Size min_panel_size, max_panel_size;

protected:
	//! \since build 446
	Thumb btnClose;

private:
	//! \since build 448
	HoverUpdater hover_updater{btnClose};
	//! \since build 555
	BorderResizer border;

public:
	//! \since build 555
	explicit
	ImagePanel(const Rect&, const Size& = DefaultMinPanelSize,
		const Size& = DefaultMaxPanelSize);

private:
	//! \since build 555
	DefGetter(ynothrow, ImagePages&, PagesRef, get<0>(Deref(session_ptr)))
	//! \since build 446
	PDefH(Point, CalcCloseButtonLocation, ) const
		ImplRet({GetWidth() - 8 - btnClose.GetWidth(), 8})

public:
	//! \since build 571
	DefSetter(bool, RootMode, border.RootMode)

	//! \since build 560
	void
	Load(ImagePages&&);
	//! \since build 560
	template<typename _type>
	void
	Load(const _type& arg)
	{
		Load(ImagePages(&arg[0], min_panel_size, max_panel_size));
	}

	void
	Unload();
};

} // namespace ImageBrowser;

#endif

