// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-7-11;
// Version = 0.2578; *Build 132 r9;


#include "../YCLib/ydef.h"

/*
TODO:

Make "UpdateFont()" more efficienct.

Use in-buffer background color rendering and function "CopyToScreen()" to inplements "TextRegion" background outputing more efficienct.

Consider to simplify the general window class: "YForm".

Rewrite system RTC.

Build a more advanced console system.

Build a series set of robust gfx APIs.

GUI characteristics needed:
Icons;
Buttons;
Other controls.

Other stuff to be considered to append:
Design by contract: DbC for C/C++, GNU nana.

DONE:

r2:
window deleting processing procedures
moved to @OnDeactive from @ShlDS::~ShlDS();
\template func @NewHWND
renamed to @NewWindow;
\inline func @NowShellClearDefaultGUI
renamed to @NowShellDrop;

r3-r:
fix \impl btnReturn;

DOING:
\impl \class ShlReader;

NEXT:
fully \impl btnTest;

*/

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

