// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-7-13;
// Version = 0.2590; *Build 132 r31;


#include "../YCLib/ydef.h"

/*

Record prefix and abbrevations:
<statement> ::= statement;
; ::= statement termination
= ::= equivalent
+ ::= added
- ::= removed
* ::= fixed
/ ::= modified
% ::= reformed
~ ::= from
! ::= not
& ::= and
| ::= or
^ ::= used
>> ::= moved to
=> ::= renamed to
@ ::= identifier
@@ ::= in / belongs to
\c ::= classes
\e ::= exceptions
\eh ::= exception handling
\err ::= errors
\ev ::= events
\evh ::= event handling
\f ::= functions
\g ::= global
\gs ::= global scpoe
\i ::= inline
\if ::= inline functions
\impl ::= implementations
\init ::= initializations
\m ::= members
\mac ::= macros
\mf ::= member functions
\ns ::= namespaces
\para ::= parameters
\para.def ::= default parameters
\ptr ::= pointers
\rem ::= remarked
\s ::= static
\sf ::= non-member static functions
\smf ::= static member functions
\sm ::= static member
\snm ::= static non-member
\t ::= templates
\tc ::= class templates
\tf ::= function templates
\tr ::= trivial

DONE:

r2:
/ window deleting processing procedures
	>> @OnDeactive ~ @ShlDS::~ShlDS();
/ \tf @NewHWND
	=> @NewWindow;
/ \if @NowShellClearDefaultGUI
	=> @NowShellDrop;

r3-r15:
* \impl btnReturn;
	- windows background graphics;
/ background image \ptr
	>> <Shells.cpp> ~ <Shells.h>;

r16:
/ all \smf @Load <Shells.h>
	>> non-member unnamed \ns @@ <Shells.cpp>;

r17:
/ all \sf @@ <Shells.cpp>
	>> non-member unnamed \ns;
+ windows background @@ \init again;

r18-r22:
* image resources loading \f;
	/ @SetBackground @@ \c @Froms::AWindow \para type ~ @YImage& => @HResource<YImage>;
	+ @SetBackground @@ \c @Froms::AWindow \para.def = NULL;

r23:
+ \tf GetPointer for \tc @HResource;
/ \tc @HResource
	=> @GHResource;
/ \mf @SetBackground(GHResource<YImage>) @@ \c @Froms::AWindow
	>> \c @Forms::MWindow;
+ \mf @Froms::MWindow::GetBackground();
+ \mf @Froms::AWindow::SetBackground(GHResource<YImage>);
+ \mf @Froms::MWindow::GetBackground();
+ \mf @Froms::AWindow::GetBackground();

r24:
- \mf @ShlDS::DrawWindows() @@ !@Helper::ShlDS;
+ \mf @ShlGUI::OnDeactivated() @@ !@Helper::ShlDS;
+ \mf @ShlDS::OnDeactivated() @@ !@Helper::ShlDS;
- \mf @ShlLoad::OnDeactivated() @@ <Shell.cpp>;
- \mf @ShlS::OnDeactivated() @@ <Shell.cpp>;
/ \i \fun NowShellClearBothScreen() to ShlClearBothScreen(HSHL = NowShell());

r25:
/= ^ \mac @DefGetter \impl @ShlDS::UpWindowHandle @@ !@Helper::ShlDS;
/= ^ \mac @DefGetter \impl @ShlDS::DownWindowHandle @@ !@Helper::ShlDS;
/= \impl \f ShlA::OnDeactivated(const MMSG&) ^ ShlDS::OnDeactivated(const MMSG&);

r26:
+ \impl \c @ShlReader @@ <Shells.cpp>;
	- \sm @pdsr & \sm @ptf @@ \c @ShlReader;
	+ \snm @tf & \snm @dsr @@ \c @ShlReader;

r27-r28:
/ \impl \c @ShlReader @@ <Shells.cpp>;
	/ text drawing procedure
		>> @ShlReader::UpdateToScreen();
	+ reader refreshing procedure @@ @ShlReader::UpdateToScreen();

r29:
/ \rem reader refreshing procedure @@ @ShlReader::UpdateToScreen();

r30:
/ \m @mq >> @DefaultMQ @@ \c @YApplication ~ \gs;
/ \m @mq >> @DefaultMQ_Backup @@ \c @YApplication ~ \gs;
/ \m @mq => @DefaultMQ >> \gs ~ @@ \c @YApplication;
/ \m @log => @Log @@ \c @YApplication;

r31:
+ @YInit \eh;
+ @::main \eh;
+ \mf @YLog::operator<<(const char*);
+ \mf @YLog::Error(const char*);
+ \mf @YLog::FatalError(const char*);

DOING:
r31:
/ ...

NEXT:
+ \impl \c ShlReader;
+ fully \impl btnTest;

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

