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
\version 0.2845; *Build 170 r11;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11;
\par 修改时间:
	2010-11-12 22:56 + 08:00;
\par 字符集:
	ANSI / GBK;
\par 模块名称:
	YReader::GBKEX;
\deprecated 临时测试用途。
*/


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
@@ ::= in / belonged to
\a ::= all
\ab ::= abstract
\ac ::= access
\amb ::= ambiguities
\amf ::= abstract/pure virtual member function
\as ::= assertions
\bg ::= background
\c ::= const
\cb ::= catch blocks
\cl ::= classes
\cp ::= copied
\ctor ::= constructors
\cv ::= const & volatile
\d ::= derived
\de ::= default
\def ::= definitions
\dep ::= dependencies
\decl ::= declations
\dir ::= directories
\dtor ::= destructors
\e ::= exceptions
\em ::= empty
\en ::= enums
\eh ::= exception handling
\err ::= errors
\es ::= exception specifications
\evt ::= events
\evh ::= event handling
\ex ::= extra
\ext ::= extended
\exp ::= explicit
\f ::= \fn = functions
\g ::= global
\gs ::= global scpoe
\h ::= headers
\i ::= inline
\impl ::= implementations
\in ::= interfaces
\init ::= initializations
\inc ::= included
\inh ::= inherited
\inv ::= invoke
\k ::= keywords
\lib ::= library
\ln ::= lines
\m ::= members
\mac ::= macros
\mem ::= memory
\mf ::= member functions
\mo ::= member objects
\n ::= names
\ns ::= namespaces
\o ::= objects
\op ::= operators
\or ::= overridden
\param ::= parameters
\param.de ::= default parameters
\pt ::= points
\ptr ::= pointers
\q ::= qualifiers
\rem ::= remarked
\ren ::= renamed
\ref ::= references
\refact ::= refactorings
\res ::= resources
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
\tg ::= targets
\tr ::= trivial
\tp ::= types
\u ::= units
\un ::= unnamed
\v ::= volatile
\vt ::= virtual
\val ::= values

$using:
\u YWidget
{
	\in IWidget;
	\in IUIBox;
	\cl Widget;
	\cl YUIContainer;
	\cl YLabel;
}
\u YControl
{
	\in IControl;
	\in IVisualControl;
	\cl MVisualControl;
	\cl YControl;
	\cl AVisualControl;
	\cl YVisualControl;
}
\u YGUIComponent
{
	\cl AButton;
	\cl YThumb;
	\cl YButton;
	\cl ATrack;
	\cl YHorizontalTrack;
	\cl YVerticalTrack;
	\cl MScrollBar;
	\cl AScrollBar;
	\cl YHorizontalScrollBar;
	\cl YVerticalScrollBar;
	\cl YListBox;
	\cl YFileBox;
}
\u YWindow
{
	\in IWindow;
	\cl MWindow;
	\cl AWindow;
	\cl YFrameWindow;
}

$DONE:
r1:
/ @@ \u YGUIComponent:
	/ \mf YHorizontalTrack::OnDrag_Thumb -> \mf ATrack::OnDrag_Thumb_Horizontal;
	/ \mf YVerticalTrack::OnDrag_Thumb -> \mf ATrack::OnDrag_Thumb_Vertical;
	/ \impl @@ \ctor @@ \cl YHorizontalTrack & \cl YVerticalTrack;

r2:
/ @@ \cl YLabel @@ \u YWidget:
	/ \ctor template<class _tChar>
		YLabel(HWND, const _tChar*, const Rect& = Rect::FullScreen,
		const Drawing::Font& = Drawing::Font::GetDefault(),
		IUIBox* = NULL, GHResource<Drawing::TextRegion> = NULL)
		-> \exp YLabel(HWND = NULL, const Rect& = Rect::FullScreen,
		const Drawing::Font& = Drawing::Font::GetDefault(),
		IUIBox* = NULL, GHResource<Drawing::TextRegion> = NULL);
/ @@ \cl MLabel @@ \u YWidget:
	/ \ctor template<class _tChar>
		MLabel(const _tChar*,
		const Drawing::Font& = Drawing::Font::GetDefault(),
		GHResource<Drawing::TextRegion> = NULL)
		-> \exp	MLabel(const Drawing::Font& = Drawing::Font::GetDefault(),
		GHResource<Drawing::TextRegion> = NULL);
/ @@ \cl YButton @@ \u YGUIComponent:
	/ \ctor template<class _tChar>
		YButton(HWND, const _tChar*, const Rect& = Rect::FullScreen,
		const Drawing::Font& = Drawing::Font::GetDefault(), IUIBox* = NULL,
		GHResource<Drawing::TextRegion> = NULL)
		-> \exp YButton(HWND = NULL, const Rect& = Rect::FullScreen,
		const Drawing::Font& = Drawing::Font::GetDefault(), IUIBox* = NULL,
		GHResource<Drawing::TextRegion> = NULL);
/ \impl @@ \u Shells;

r3:
/ \mf {'Activate', 'Deactive'} >> !\m friend \fn;

r4:
/ @@ \u YWidget:
	+ typedef enum
		{
			Horizontal = 0,
			Vertical = 1
		} Orientation @@ \ns Widgets;
	+ \cl MOriented;
/ @@ \u YGUIComponent:
	/ @@ \cl ATrack:
		+ \amf Widgets::Orientation GetOrientation() const;
		+ DefPredicate(Horizontal, GetOrientation() == Widgets::Horizontal);
		+ DefPredicate(Vertical, GetOrientation() == Widgets::Vertical);
		+ DefGetter(SDST, WidgetLength,
			IsHorizontal() ? GetWidth() : GetHeight());
		+ DefGetter(SDST, ThumbLength, IsHorizontal()
			? Thumb.GetWidth() : Thumb.GetHeight());
		+ DefGetter(SDST, ThumbPosition, IsHorizontal()
			? Thumb.GetLocation().X : Thumb.GetLocation().Y);
		- DeclIEntry(SDST GetThumbLength() const);
		- DeclIEntry(SDST GetThumbPosition() const);
		+ DeclIEntry(void SetThumbLength(SDST)) -> void SetThumbLength(SDST);
		+ DeclIEntry(void SetThumbPosition(SDST))
			-> void SetThumbPosition(SDST);
		+ \mf SetThumbLength;
	/ @@ \cl YHorizontalTrack & \cl YVerticalTrack:
		- \i \mf GetThumbLength;
		- \i \mf GetThumbPosition;
		- \mf SetThumbLength;
		- \i \mf SetThumbPosition;
		/ \impl @@ \ctor;
		+ \mf GetOrientation;
	/ \mf YHorizontalTrack::OnTouchDown >> ATrack::OnTouchDown_Horizontal;
	/ \mf YVerticalTrack::OnTouchDown >> ATrack::OnTouchDown_Vertical;

r5:
* \tr @@ \ctor @@ \cl YHorizontalTrack & \cl YVerticalTrack;

r6:
/ \st EArea -> \en Area @@ \cl ATrack;

r7:
/ @@ \cl YGUIComopnent:
	- \cl MScrollBar;
	/ \cl AScrollBar;
	/ @@ \cl ATrack:
		+ \vt @@ \mf SetThumbLength;
		+ \vt @@ \mf SetThumbPosition;
	/ \cl YHorizontalScrollBar;
/ @@ \u Shells;
/ @@ \u YObject:
	+ \fn
	{
		SPOS SelectFrom(const BinaryGroup&, bool = true);
		SDST SelectFrom(const Size&, bool = true);
		SPOS& SelectRefFrom(BinaryGroup&, bool = true);
		SDST& SelectRefFrom(Size&, bool = true);
		void UpdateTo(BinaryGroup&, SPOS, bool = true);
		void UpdateTo(Size&, SDST, bool = true);
	}
/= \mf \vt bool DrawWidgets -> ImplI(IWindow) bool DrawWidgets
	@@ \cl YFrameWindow @@ \u YWindow;

r8:
/ @@ \u YGUIComponent:
	/ @@ \cl AScrollBar:
		+ \mf \vt IVisualControl* GetTopVisualControlPtr(const Point&);
		+ \mf \vt SDST GetTrackLength() ythrow();
		- \mf SDST GetScrollAreaLength() const ythrow();
		- \mf SDST GetScrollAreaFixedLength() const ythrow();
		- \mf SetThumbLength;
		/ \mf SetThumbPosition;
		/ \impl @@ \mf DrawForeground;
	/ @@ \cl ATrack:
		/ \mf GetWidgetLength => GetTrackLength;
		/ + \vt @@ \mf SDST GetTrackLength() ythrow();
		/ \tr \impl @@ \mf SetThumbLength;
		/ \tr \impl @@ \mf SetThumbPosition;

r9:
* \impl @@ \fn bool DrawRect(const Graphics&, const Point&, const Size&, Color)
	@@ \u YGDI;

r10:
/ DoxyGen \rem applied to header @@ \a \f @@ \lib CHRLib & \lib YCLib
	& \dir "include/" & \dir "/source" & Adaptor::YAdaptor;
- \f "CHRLib/chrlib.cpp";
/ \tr @@ \u DSReader;
- \f "include/DSRMsg.h";
- \em \f "Service/ysutil.cpp";

r11:
/ DoxyGen \rem applied \a (\fn & \cl) @@ \a header & source \f;
- \h Service::YSystemUtilities;
- \f "Core/ycutil.cpp";
/ @@ \h CHRDefinition:
	/ \mac _CHRLIB => CHRLIB;
	/ \mac _CHRLIB_ => CHRLIB_;
* \mf \i int ShlCLI::ExecuteCommand(const String&) @@ \u ShlDS;


$DOING:

/ ...


$NEXT:
* blank-clicked \evt OnClick @@ ListBox;

b170-b190:
/ fully \impl \u DSReader;
	* moving text after setting lnGap;
* non-ASCII character filename error in FAT16;
+ \impl loading pictures;
+ \impl style on widgets;
* fatal \err:
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
	-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 02055838
	ftc_snode_weight
	ftcsbits.c:271

$TODO:

Clarify the log levels.

Make "UpdateFont()" more efficienct.

More efficient @YTextRegion output:
Use in-buffer background color rendering and function @CopyToScreen()
	to inplements @YTextRegion background;
Use pre-refershing to make font changing.

Consider to simplify the general window class: @YForm.

Rewrite system RTC.

Build a more advanced console wrapper.

Build a series set of robust gfx APIs.

GUI characteristics needed:
Icons;
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

