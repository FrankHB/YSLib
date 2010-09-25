// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-9-25;
// Version = 0.2617; *Build 154 r34;


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
\ac ::= access
\bg ::= background
\c ::= const
\cb ::= catch blocks
\cl ::= classes
\ctor ::= constructors;
\cv ::= const & volatile
\d ::= derived
\de ::= default
\def ::= definitions
\decl ::= declations
\dtor ::= destructors;
\e ::= exceptions
\en ::= enums
\eh ::= exception handling
\err ::= errors
\es ::= exception specifications
\ev ::= events
\evh ::= event handling
\ex ::= extra
\ext ::= extended
\exp ::= explicit
\f ::= functions
\g ::= global
\gs ::= global scpoe
\h ::= headers
\i ::= inline
\impl ::= implementations
\init ::= initializations
\inc ::= included
\inh ::= inherited
\inv ::= invoke
\m ::= members
\mac ::= macros
\mem ::= memory
\mf ::= member functions
\ns ::= namespaces
\op ::= operators
\para ::= parameters
\para.def ::= default parameters
\ptr ::= pointers
\rem ::= remarked
\ref ::= references
\ret ::= return
\s ::= static
\sf ::= non-member static functions
\sif ::= non-member inline static functions
\simp ::= simplified
\smf ::= static member functions
\sm ::= static member
\scm ::= static const member
\snm ::= static non-member
\st ::= structs
\str ::= strings
\t ::= templates
\tb ::= try blocks
\tc ::= class templates
\tf ::= function templates
\tr ::= trivial
\tp ::= types
\u ::= units
\v ::= volatile
\val ::= values

DONE:
r1:
/ \a @@ \ns Exceptions => \n YSLib;
- \ns Exceptions;
/ \a GeneralError => GeneralEvent;

r2:
* @@ \cl YListBox @@ \u YControl:
	+ private \mf void _m_TouchDown(const Runtime::MTouchEventArgs&);
	+ \s \eh void OnTouchDown(IVisualControl&, const Runtime::MTouchEventArgs& = Runtime::MTouchEventArgs::Empty);
	/ \impl @@ private \mf void _m_OnClick(const Runtime::MTouchEventArgs&);
	/ protected \mf (void Init_() -> void _m_init());
		/ \tr \impl @@ 2 \ctor;

r3:
/= \tr @@ \mf void _m_init() @@ \cl YListBox @@ \u YControl;

r4:
/ \simp @@ \cl YListBox @@ \u YControl:
	- \s \eh void OnTouchDown(IVisualControl&, const Runtime::MTouchEventArgs& = Runtime::MTouchEventArgs::Empty);
	- \s \eh void OnClick(IVisualControl&, const Runtime::MTouchEventArgs& = Runtime::MTouchEventArgs::Empty);
	- \s \eh void OnKeyPress(IVisualControl&, const Runtime::MKeyEventArgs& = Runtime::MKeyEventArgs::Empty);
	/ private \mf void _m_OnTouchDown(const Runtime::MTouchEventArgs&) => public \eh OnTouchDown;
	/ private \mf void _m_OnClick(const Runtime::MTouchEventArgs&) => public \eh OnClick;
	/ private \mf void _m_OnKeyPress(const Runtime::MKeyEventArgs&) => public \eh OnKeyPress;
	/ \tr \impl @@ \mf void _m_init();

r5:
/ \simp @@ \cl MVisualControl @@ \u YControl:
	- \s \eh void OnTouchHeld(IVisualControl&, const Runtime::MTouchEventArgs& = Runtime::MTouchEventArgs::Empty);
	- \s \eh void OnTouchMove(IVisualControl&, const Runtime::MTouchEventArgs& = Runtime::MTouchEventArgs::Empty);
	/ private \mf void _m_OnTouchHeld(const Runtime::MTouchEventArgs&) => public \eh OnTouchHeld;
	/ private \mf void _m_OnTouchMove(const Runtime::MTouchEventArgs&) => public \eh OnTouchMove;
	/ \tr \impl @@ \ctor;
	/ \impl @@ \mf void OnTouchHeld(const Runtime::MTouchEventArgs&);
/ @@ \cl ShlA @@ \u YShel:
	/ \tr \impl @@ \ctor @@ \cl TFormB;
	/ \tr \impl @@ \ctor @@ \cl TFormC;

r6:
/ @@ \u YControl:
	/ \simp @@ \cl YListBox:
		/ \s \eh void OnSelected(IVisualControl&, const MIndexEventArgs&)
			-> !\s \eh void OnTouchHeld(const MIndexEventArgs&);
		/ \s \eh void OnConfirmed(IVisualControl&, const MIndexEventArgs&)
			-> !\s \eh void OnConfirmed(const MIndexEventArgs&);
		/ \tr \impl @@ \mf void _m_init();
	/ \simp @@ \cl MVisualControl:
		/ \s \eh void OnTouchDown(IVisualControl&, const Runtime::MTouchEventArgs& = Runtime::MTouchEventArgs::Empty)
			-> !\s \eh void OnTouchDown(const Runtime::MTouchEventArgs& = Runtime::MTouchEventArgs::Empty);
		/ \s \eh void OnGotFocus(IControl&, const MEventArgs& = GetZeroElement<MEventArgs>())
			-> !\s \eh void OnTouchDown(const MEventArgs& = GetZeroElement<MEventArgs>());
		/ \s \eh void OnLostFocus(IControl&, const MEventArgs& = GetZeroElement<MEventArgs>())
			-> !\s \eh void OnTouchDown(const MEventArgs& = GetZeroElement<MEventArgs>());
		/ \tr \impl @@ \ctor;
	/ \simp @@ \cl MVisualControl:
		/ \s \eh void OnConfirmed(IVisualControl&, const MIndexEventArgs&)
			-> !\s \eh void OnConfirmed(const MIndexEventArgs&);
		/ \tr \impl @@ \ctor;
/ @@ \cl ShlA @@ \u YShel:
	/ \tr \impl @@ \ctor @@ \cl TFormB;

r7:
/ \impl @@ \mf void OnConfirmed(const MIndexEventArgs&) @@ \cl YFileBox @@ \u YControl;
/ \impl @@ \mf bool operator/=(const std::string&) @@ \cl MFileList @@ \u YFileSystem;

r8-r9:
* \impl @@ \mf void OnConfirmed(const MIndexEventArgs&) @@ \cl YFileBox @@ \u YControl;

r10:
+ \s \mf IsDirectory() @@ \cl HDirectory @@ \u YCommon;
/ \impl @@ \mf ListType::size_type LoadSubItems() @@ \cl MFileList @@ \u YFileSystem:
	/ ^ \s \mf HDirectory::IsDirectory();
/ \impl @@ \mf void LoadFontFileDirectory(CPATH, CPATH) @@ \cl YFontCache @@ \u YFont:
	/ ^ \s \mf HDirectory::IsDirectory();

r11-r13:
/ @ \u YFileSystem:
	/= \simp \impl @@ \f bool Validate(std::string&);
	* \impl @@ \mf bool operator/=(const std::string&) @@ \cl MFileList;

r14-r21:
/= test 1;

r22:
* \impl @@ \mf void OnConfirmed(const MIndexEventArgs&) @@ \cl YFileBox @@ \u YControl;

r23:
+ \mf IO::Path GetPath() const @@ \cl YFileBox @@ \u YControl;
/ \impl @@ \mf void ShlS::TFrmFileListSelecter::btnTest_Click(const MTouchEventArgs&) @@ \u Shells;

r24-r25:
/ test 2;

r26:
* impl @@ \mf IO::Path GetPath() const @@ \cl YFileBox @@ \u YControl;

r27-r28:
/ test 3;

r29:
* PDefHead(const ValueType*, c_str) + \c @@ \cl YPath @@ \u YFileSystem;

r30-r31:
/ test 4;

r32:
* impl @@ \mf IO::Path GetPath() const @@ \cl YFileBox @@ \u YControl;

r33:
/ impl @@ \mf Path& operator/=(const Path& path) @@ \cl Path @@ \ns IO @@ \u YFileSystem;

r34
/ \impl @@ \mf ListType::size_type LoadSubItems() @@ \cl MFileList @@ \u YFileSystem;


DOING:

/ ...

Debug message:
//

NEXT:
/ fully \impl \cl YListBox;
/ fully \impl \u YFileSystem;

TODO:

Clarify the log levels.

Make "UpdateFont()" more efficienct.

More efficient @YTextRegion output:
Use in-buffer background color rendering and function @CopyToScreen() to inplements @YTextRegion background;
Use pre-refershing to make font changing.

Consider to simplify the general window class: @YForm.

Rewrite system RTC.

Build a more advanced console wrapper.

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

