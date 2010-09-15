// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-9-15;
// Version = 0.2614; *Build 147 r25;


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
/ @@ \u YFont:
	+ \cl CharBitmap;
	/ FTC_SBit GetGlyph(u32) -> CharBitmap GetGlyph(u32);
/= ^ \cl CharBitmap @@ \u YText;

r2:
/ @@ \h CHRDef:
	+ typedef std::char16_t uchar_t;
	+ typedef std::char32_t fchar_t;
/ + std::char16_t && char32_t @@ \h <platform.h>;
/= \a uchardiff_t >> uint_t;
/= ^ fchar_t;
/@@ \h YAdapter:
	/ @@ \ns YSLib:
		+ using Text::fchar_t;
		+ using Text::uint_t;
	/ @@ \ns YSLib:
		+ using Text::uint_t;

r3:
/= @@ \u YSInitialization:
	/= \tr \impl @@ \i \f void fatalError() @@ unnamed \ns;
	/= \tr \impl @@ \f void InitializeSystemFontCache();
/= \tr \impl @@ \f void YConsoleInit(u8, PIXEL, PIXEL) @@ \u YCommon;

r4:
/ @@ \u SHlDS:
	/ ShlGUI >> \ns Shells @@ \ns YSLib;
	+ \cl ShlCLI @@ \ns YSLib::Shells;
	/+ \i @@ \ctor && \dtor @@ \cl ShlGUI;
/= \tr @@ \u (ShlDS & Shells);

r5:
/ @@ \cl MIndexEventArgs @@ \ns Controls @@ \u YControl:
	+ typedef std::ptrdiff_t IndexType;
	/ \m int index -> IndexType index;
	/ \ctor MIndexEventArgs(IVisualControl&, const int&)
		-> MIndexEventArgs(IVisualControl&, IndexType);
/ \a GMSequenceViewer => GSequenceViewer;
* @@ \t \cl GSequenceViewer @@ \cl YModule:
	/ size_type => SizeType;
	+ typedef std::ptrdiff_t IndexType;
	/ SizeType nIndex -> IndexType nIndex;
	/ DefGetter(SizeType, Index, nIndex) -> DefGetter(IndexType, Index, nIndex);
	* inline GSequenceViewer& operator>>(SizeType) -> inline GSequenceViewer& operator>>(IndexType);
	* inline GSequenceViewer& operator<<(SizeType) -> inline GSequenceViewer& operator<<(IndexType);
	* inline GSequenceViewer& operator+=(SizeType) -> inline GSequenceViewer& operator+=(IndexType);
	* inline GSequenceViewer& operator-=(SizeType) -> inline GSequenceViewer& operator-=(IndexType);

r6:
* @@ \cl YListBox @@ \u YControl:
	+ typedef GSequenceViewer<ListType> ViewerType;
	/ DefGetterMember(ListType::size_type, Index, Viewer) -> DefGetterMember(ViewerType::IndexType, Index, Viewer);
	/ DefGetterMember(ListType::size_type, Selected, Viewer) -> DefGetterMember(ViewerType::size_type, Selected, Viewer);
	/ \mf void SetSelected(ListType::size_type) ->  void SetSelected(ViewerType::IndexType);
	/ \ret \tp @@ protected \mf ListType::size_type CheckPoint(SPOS, SPOS) -> ViewerType::IndexType;
	/ GetItemPtr(ListType::size_type) -> GetItemPtr(ViewerType::IndexType);
	* \impl @@ \mf void _m_OnKeyPress(const MKeyEventArgs&);
	* \impl @@ \mf ViewerType::IndexType::CheckPoint(SPOS, SPOS);
	* \impl @@ \mf void SetSelected(YListBox::ViewerType::IndexType);
	* \impl @@ \mf void DrawForeground();
* @@ \t \cl GSequenceViewer @@ \u YModule:
	* bool SetIndex(SizeType) -> bool SetIndex(IndexType);
	* bool SetSelected(SizeType) -> bool SetSelected(IndexType);
	* \impl @@ \mf bool SetLength(SizeType);
	* \impl @@ \mf bool RestrictSelected();
	* \impl @@ \mf bool RestrictViewer();

r7:
* @@ \cl YListBox @@ \u YControl:
	* \impl @@ \mf void DrawForeground();
* @@ \t \cl GSequenceViewer @@ \u YModule:
	* \impl @@ \mf bool RestrictSelected();
	* \impl @@ \mf bool RestrictViewer();

r8:
/= \tr @@ \h "ysdef.h";
+ extern YScreen *pScreenUp, *pScreenDown @@ \h "yglobal.h";

r9-r14:
/= test 1;

r15-r21:
/ \impl @@ \mf void ShlReader::UpdateToScreen() @@ \u Shells;

r22:
/ \impl @@ \ctor @@ \cl MDualScreenReader @@ \u DSReader;

r23-r25:
/ \impl @@ \mf void ShlReader::OnKeyPress(const MKeyEventArgs&) @@ \u Shells;


DOING:

/ ...

NEXT:
+ fully \impl \u YFileSystem;
/ fully \impl \cl ShlReader;
/ fully \impl btnTest;

TODO:

Clarify the log levels.

Make "UpdateFont()" more efficienct.

More efficient  @YTextRegion output:
Use in-buffer background color rendering and function @CopyToScreen() to inplements @YTextRegion background;
Use pre-refershing to make font changing.

Consider to simplify the general window class: @YForm.

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

