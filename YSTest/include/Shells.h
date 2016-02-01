/*
	© 2010-2016 FrankHB.

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
\version r2936
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-03-06 21:38:16 +0800
\par 修改时间:
	2016-01-28 10:55 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::Shells
*/


#ifndef INC_YReader_Shells_h_
#define INC_YReader_Shells_h_ 1

#include <ysbuild.h>
#include YFM_DS_Helper_Shell_DS
#include YFM_Helper_ShellHelper
#include YFM_YCLib_Input
#include YFM_YSLib_UI_Loader

namespace YSLib
{

using ResourceIndex = decltype(__LINE__);
using ResourceMap = map<ResourceIndex, ValueObject>;

#define DeclResource(_name) const ResourceIndex _name(__LINE__);

} // namespace YSLib;


namespace YReader
{

#define G_COMP_NAME u"Franksoft"
#define G_APP_NAME u"YReader"
#define G_APP_VER u"Build PreAlpha"

using namespace YSLib;

using namespace UI;
using namespace Drawing;

using namespace DS;
using namespace DS::UI;

using platform::WaitForInput;


//! \since build 404
yconstexpr const size_t Image_N(3);


/*!
\brief 全局资源清理。
\since build 400
*/
void
Cleanup();

shared_ptr<Image>&
FetchImage(size_t);

//! \since build 390
double
FetchImageLoadTime();


//! \since build 434
WidgetLoader&
FetchWidgetLoader();

/*!
\brief 向标签栏和节点添加按钮。
\since build 667
*/
void
AddButtonToTabBar(TabControl&, ValueNode&, const string&, const String&,
	SDst = 64);

} // namespace YReader;

#endif

