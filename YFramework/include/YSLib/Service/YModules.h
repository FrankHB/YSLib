/*
	© 2013-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup Service Service
\ingroup YSLib
\brief YSLib 服务模块。
*/

/*!	\file YModules.h
\ingroup YSLib
\brief YSLib 服务模块配置文件。
\version r78
\author FrankHB <frankhb1989@gmail.com>
\since build 462
\par 创建时间:
	2013-12-23 21:57:26 +0800
\par 修改时间:
	2015-02-01 08:37 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YModules
*/


#ifndef YSL_INC_Service_YModules_h_
#define YSL_INC_Service_YModules_h_ 1

#include "../../YModules.h"
#include "../Core/YModules.h"

//! \since build 461
//@{
#define YFM_YSLib_Service_YTimer <YSLib/Service/YTimer.h>
#define YFM_YSLib_Service_YResource <YSLib/Service/yres.h>
#define YFM_YSLib_Service_YPixel <YSLib/Service/YPixel.h>
#define YFM_YSLib_Service_YBlit <YSLib/Service/yblit.h>
#define YFM_YSLib_Service_YDraw <YSLib/Service/ydraw.h>
#define YFM_YSLib_Service_YGDI <YSLib/Service/YGDI.h>
//! \since build 555
#define YFM_YSLib_Service_YBrush <YSLib/Service/YBrush.h>
//! \since build 462
//@{
#define YFM_YSLib_Service_TextBase <YSLib/Service/TextBase.h>
#define YFM_YSLib_Service_TextLayout <YSLib/Service/TextLayout.h>
#define YFM_YSLib_Service_TextRenderer <YSLib/Service/TextRenderer.h>
//@}
//! \since build 473
//@{
#define YFM_YSLib_Service_FileSystem <YSLib/Service/FileSystem.h>
#define YFM_YSLib_Service_File <YSLib/Service/File.h>
#define YFM_YSLib_Service_TextFile <YSLib/Service/TextFile.h>
//@}
#define YFM_YSLib_Service_TextManager <YSLib/Service/TextManager.h>
#define YFM_YSLib_Service_CharRenderer <YSLib/Service/CharRenderer.h>
#define YFM_YSLib_Service_ContentType <YSLib/Service/ContentType.h>
#define YFM_YSLib_Service_AccessHistory <YSLib/Service/AccessHistory.hpp>
//@}
//! \since build 554
#define YFM_YSLib_Service_ImageProcessing <YSLib/Service/ImageProcessing.h>

#endif

