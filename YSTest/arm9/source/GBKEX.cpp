// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-7-16;
// Version = 0.2595; *Build 133 r37;


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
-> ::= changed to
>> ::= moved to
=> ::= renamed to
@ ::= identifier
@@ ::= in / belongs to
\a ::= all
\bg ::= background
\c ::= const
\cl ::= classes
\cv ::= const & volatile
\d ::= derived
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
\ref ::= references
\s ::= static
\sf ::= non-member static functions
\simp ::= simplified
\smf ::= static member functions
\sm ::= static member
\snm ::= static non-member
\t ::= templates
\tc ::= class templates
\tf ::= function templates
\tr ::= trivial
\ty ::= types
\v ::= volatile
\val ::= values

DONE:

r1:
/= \m @tf => @TextFile @@ @ShlReader;
/= \m @dsr => @Reader @@ @ShlReader;

r2:
- \simp: !@YCLib::Definition \mac;
- \simp: !@YGDI \mac;
/ \a @UINT ~ @u32;
/ \a @CHAR => @char;
/ \a @LPTSTR => @STR;
/ \a @CSTR => @CSTR;

r3:
- \simp: windows @@ @ShlReader;

r4:
- \simp: null \eh @@ <Shells.cpp>;

r5:
+ \eh @@ @std::bad_alloc @@ !@::main;
+ \cl @Exceptions::YLoggedError \d \cl @Exceptions::YGeneralError @@ !@YException;
/ \a \cl @Exceptions::YLoggedError ~ @Exceptions::YGeneralError !@@ !@YException;
+ \eh @@ @std::bad_cast @@ !@::main;
- \simp: \a null \eh @@ <Shells.cpp>;
/ \a \eh @@ @std::bad_cast&|@std::bad_alloc& ~ @...;

r6-r7:
+ \m GHResource<YImage> hUp, hDn @@ \cl @ShlReader to store desktop \bg images;
+ procedure @@ @ShlReader::OnActive & @ShlReader::OnDeactive to store & recover desktop \bg images;

r8-r10:
/ reader colors -> black words & light \bg;
+ \impl key \eh @@ \cl @ShlReader;
	+ \mf \eh @OnKeyPress(const YKeyEventArgs&) @@ \cl @ShlReader, linked to @OnActived & @OnDeactivated;
/= @OnTouchHeld >> before @OnKeyUp @@ <shlds.h>;

r11:
/ \sf @ResponseKeyBase @@ !@YGUI restricted desktop focus;
/= \a \sf @@ !@YGUI >> unnamed \ns;

r12:
/= \a @GIEventCallback => @GAHEventCallback;
/= \a @IEventCallback => @AHEventCallback;
/= \a @ITouchCallback => @AHTouchCallback;
/= \a @IKeyCallback => @AHKeyCallback;

r13-r25:
*= testing invalid input @@ @ShlReader::ShlProc;

r26:
/= \simp: @IRES @GetMessage(MMSG&, HSHL, MSGID, MSGID);
* @WaitForGUIInput @@ !@YGlobal;
/= @IRES @PeekMessage(MMSG&, HSHL, MSGID, MSGID, MSGID = PM_NOREMOVE)
	-> @IRES @PeekMessage(MMSG&, HSHL = NULL, MSGID = 0, MSGID = 0, u32 = PM_NOREMOVE);
/= @IRES @GetMessage(MMSG&, HSHL, MSGID, MSGID)
	-> @IRES @GetMessage(MMSG&, HSHL = NULL, MSGID = 0, MSGID = 0);
/= \simp: @::main(int, char*[]);
/= \a \sf @@ !@YGlobal >> unnamed \ns;

r27:
+ \m bool @bgDirty @@ \cl @ShlReader;

r28-r29:
+ desktop @SetRefresh() @@ @ShlReader::UpdateToScreen();

r30:
/= @SetFontSize(u16 = YFont::DefSize) -> @SetFontSize(u16 = YFont::DefSize) @@MDualScreenReader;
/= @SetColor(u16 = ARGB16(1, 0, 0, 0)) -> SetColor(PixelType = 0) @@MDualScreenReader;

r31-r35:
/= \simp \a \i \mf getters (^ \mac (@DefGetter & @DefBoolGetter)) @@ \cl !@YFont;
/ +\i \smf @GetDefault() @@ \cl @YFont;
/= \simp \a \i \mf getters (^ \mac (@DefStaticGetter & @DefGetterMember)) @@ \cl !@YFont;
/= \simp \a \i \mf setters (^ \mac @DefSetter) @@ \cl !@YFont;

r36:
/= DefSetterDef(SDST, Left, left, 0) -> DefSetterDef(SDST, Left, left, 0) @@ \cl @MDualScreenReader;
/ \mf SetFontSize(u16 = YFont::DefSize) -> SetFontSize(YFont::SizeType = YFont::DefSize);
/= \mf \impl SetFontSize(u8 s) -> SetFontSize(YFont::SizeType s) @@ \cl @YFontCache;

DOING:
r36:
/ ...

NEXT:
+ \impl \cl ShlReader;
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

