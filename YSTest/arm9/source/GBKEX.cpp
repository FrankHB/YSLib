/*
	Copyright (C) by Franksoft 2009 - 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file GBKEX.cpp
\ingroup YReader
\brief 测试文件。
\version 0.2860; *Build 171 r6;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11;
\par 修改时间:
	2010-11-19 10:33 + 08:00;
\par 字符集:
	ANSI / GBK;
\par 模块名称:
	YReader::GBKEX;
\deprecated 临时测试用途。
*/


#include "../YCLib/ydef.h"

// GBK coded definitions:

namespace YSLib{
	CPATH DEF_DIRECTORY = "/Data/";
	//const char* DEF_FONT_NAME = "方正姚体";
	//CPATH DEF_FONT_PATH = "/Font/FZYTK.TTF";
	CPATH DEF_FONT_PATH = "/Font/FZYTK.TTF";
	CPATH DEF_FONT_DIRECTORY = "/Font/";
/*
using namespace Components;
using namespace Widgets;
using namespace Controls;

*/
}

char gstr[128] = "你妹喵\t的= =ijkAB DEǎ。i:みま╋㊣F2\t3Xsk\nw\vwwww";

