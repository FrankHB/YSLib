// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-09-26 22:42 + 08:00;
// Version = 0.2677; *Build 155 r43;


#include "../YCLib/ydef.h"

/*

$Record prefix and abbrevations:
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
<=> ::= swaped names
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
\mo ::= member objects
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
\vt ::= virtual
\val ::= values

$using:
\u YControl
{
	\cl MVisualControl;
	\cl YListBox;
	\cl YFileBox;
}

$DONE:
r1:
/ @@ \cl MVisualControl:
	/ \v \eh \mf void OnKeyHeld(const Runtime::MKeyEventArgs&);
/ \a STouchStatus => SInputStatus;
/ @@ \u YGUI:
	/ @@ \cl SInputStatus:
		+ public typedef enum { KeyFree = 0, KeyPressed = 1, KeyHeld = 2 } KeyHeldStateType;
		+ public \s \mo Timers::YTimer KeyTimer;
		+ public \s \mo KeyHeldStateType KeyHeldState;
		/ \a v_DragOffset => DragOffset;
		+ \s \mf ResetKeyHeldStatus();
	/ \impl @@ \f ResponseKeyHeldBase(MVisualControl& c, const MKeyEventArgs&) @@ unnamed \ns;
	+ \inc "../Service/ytimer.h";

r2:
/ \impl @@ \ctor @@ \cl YFileBox;

r3:
/ \tr \impl @@ \mf void MVisualControl::OnKeyHeld(const Runtime::MKeyEventArgs&);

r4:
* \impl bool ResponseKeyUpBase(MVisualControl& c, const MKeyEventArgs&) @@ unnamed \ns @@ \cl SInputStatus @@ \u YGUI;

r5-r6:
/= test 1;

r7:
/ @@ \cl YTimer @@ \u YTimer:
	/ \impl @@ \mf bool RefreshRaw();
	/ \a nStart => nBase;
	/ \mf DefGetter(u32, StartTick, nBase)
		-> DefGetter(u32, BaseTick, nBase)
r8:
/ @@ \cl YTimer @@ \u YTimer:
	* \impl @@ \mf void Activate();

r9:
+ \s \mf void RepeatKeyHeld(Components::Controls::MVisualControl&, const MKeyEventArgs&) @@ \cl SInputStatus @@ \u YGUI;
/ \impl @@ \mf void MVisualControl::OnKeyHeld(const Runtime::MKeyEventArgs&);

r10:
* \impl @@ void SInputStatus::RepeatKeyHeld(MVisualControl&, const MKeyEventArgs&) @@ \u YGUI;

r11-r14:
/= test 2;

r15:
* \impl @@ void SInputStatus::RepeatKeyHeld(MVisualControl&, const MKeyEventArgs&) @@ \u YGUI;

r16:
/ @@ \u YGUI:
	/ \mf ResetKeyHeldStatus => ResetKeyHeldState @@ \cl SInputStatus;
	/ \impl @@ \mf bool ResponseKeyUpBase(MVisualControl&, const MKeyEventArgs&);

r17:
/ \tr \impl @@ void SInputStatus::RepeatKeyHeld(MVisualControl&, const MKeyEventArgs&) @@ \u YGUI;

r18:
/ @@ \cl YListBox:
	/ \mf void OnKeyPress(const MKeyEventArgs&) -> void OnKeyDown(const MKeyEventArgs&);
	/ \impl @@ \ctor;

r19-r21:
/= test 3;

r22:
/ \a KeyStatus => KeyHeldState;

r23:
/ \a KeyStatusType => KeyHeldStateType;

r24-r31:
/= test 4;

r32:
/ \impl @@ \mf void SInputStatus::RepeatKeyHeld(MVisualControl&, const MKeyEventArgs&) @@ \u YGUI;

r33-r35:
/ test 5

r36:
/ @@ \u YText:
	+ \f void PrintCharEx(MBitmapBufferEx& buf, TextState& ts, fchar_t c);
	- \mf void TextRegion::PrintCharEx(fchar_t c);
	/ \impl @@ \mf u8 TextRegion::PutChar(fchar_t c);
	/ @@ \cl TextState:
		+ DefSetter(SPOS, PenX, penX);
		+ DefSetter(SPOS, PenY, penY);

r37:
/= \a PrintChar => PrintCharEx;

r38-r41:
+ \f void PrintCharEx(MBitmapBufferEx& buf, TextState& ts, fchar_t c) @@ \u YText;
/= test 6;

r42:
/ \a Validate => ValidateDirectory;
/ \impl @@ ListType::size_type LoadSubItems() @@ \cl MFileList @@ \u YFileSystem;

r43:
/ \tr \impl @@ \u YShell;


$DOING:

/ ...

$Debug message:
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 20765ac
_M_lower_bound
stl_tree.h:1020
??
stl_tree.h:1532

$NEXT:
* unknown fatal error;
/ fully \impl \u DSReader;
+ \impl loading pictures;

$TODO:

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

