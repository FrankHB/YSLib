// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-9-26;
// Version = 0.2628; *Build 154 r24;


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

DONE:
r1:
/= \a ShlA => ShlSetting;
/= \a ShlS => ShlExplorer;
/ @@ \cl MDualScreenReader @@ \u DSReader:
	/ \m YTextFile& tf -> YTextFile* pTextFile;
	/ \ctor MDualScreenReader(YTextFile&, u16 = 0, u16 = SCRW,
		u16 = 0, u16 = SCRH, u16 = 0, u16 = SCRH, YFontCache& = *pDefaultFontCache) ->
		MDualScreenReader(YTextFile*, u16 = 0, u16 = SCRW,
		u16 = 0, u16 = SCRH, u16 = 0, u16 = SCRH, YFontCache& = *pDefaultFontCache);
	/ \tr \impl @@ \cl ShlReader @@ \u Shells;

r2:
/ \u DSReader:
	+ \cl BlockedText @@ \ns DS::Components;
/@@ \cl TextFileBuffer @@ \u YTextManager:
	/ @@ \cl HText:
		/ \m pBuf => pBuffer;
		/ \ctor HText(TextFileBuffer&, BlockIndexType = 0, IndexType = 0) ythrow()
			-> HText(TextFileBuffer* = NULL, BlockIndexType = 0, IndexType = 0) ythrow();
		/ \impl @@ \mf HText& HText::operator++() ythrow();
		/ \impl @@ \mf HText& HText::operator--() ythrow();
		/ \impl @@ \mf HText operator+(std::ptrdiff_t);
		/ \impl @@ \mf HText& operator+=(std::ptrdiff_t);
		/ \impl @@ \mf const uchar_t* GetTextPtr() const ythrow();
		/ \impl @@ \mf IndexType GetBlockLength(BlockIndexType i) const ythrow();
	/ \mf \i HText begin() ythrow();
	/ \mf \i HText end() ythrow();
/ \tr \impl @@ \ctor @@ \cl MDualScreenReader @@ \u DSReader;

r3:
- \exp @@ \ctor @@ \cl TextFileBuffer::HText @@ \u YTextManager:
/ @@ \cl MDualScreenReader @@ \u DSReader:
	/ \impl @@ \ctor;
	/ \mf void InitText() -> void LoadText();
	+ \mf void UnloadText();
	+ \m BlockedText* pText;
	- \dtor;

r4:
* \impl @@ \cl MDualScreenReader @@ \u DSReader;
	^ GHResource<TextRegion>;
	/ \m TextRegion& ptUp -> GHResource<TextRegion> pTrUp;
	/ \m TextRegion& ptDn -> GHResource<TextRegion> pTrDn;
* \mac @@ \h "ysdef.h";

r5:
/= \a $BoolGetter$ => $Predicate$;

r6:
/ @@ \cl MDualScreenReader @@ \u DSReader:
	- \m Text::TextFileBuffer Blocks;
	/ \impl ^ pTest->Blocks;

r7:
/ @@ \u DSReader:
	/ @@ \cl MDualScreenReader:
		- \m YTextFile* pTextFile;
		/ void LoadText() -> void LoadText(YTextFile&);
		/ \impl @@ \ctor;
		/ \tr \impl;
	- \cl YWndDSReader;
/ @@ \cl ShlReader @@ \u Shells:
	/ \m YTextFile TextFile -> YTextFile* pTextFile;
	/ \impl @@ \ctor;
	/ \impl @@ \mf OnActivated;
	/ \impl @@ \mf OnDeactivated;

r8:
/ @@ \cl ShlExplorer::TFrmFileListSelecter @@ \u Shells:
	/ \impl void fb_Selected(const MIndexEventArgs&);
	/ \impl void btnOK_Click(const MTouchEventArgs&);

r9:
/ \impl @@ \mf void SetSelected(ViewerType::IndexType) @@ \cl YListBox @@ \u YControl;

r10-r11:
/= test 1;

r12:
* \mf HText& operator++() ythrow() @@ \cl TextFileBuffer::HText @@ \u YTextManager;

r13:
* \mf \i HText& end() ythrow() @@ \cl TextFileBuffer @@ \u YTextManager;

r14:
* @@ \cl MDualScreenReader @@ \u DSReader:
	* \impl @@ \mf bool IsTextBottom();
	/ \tr \impl @@ \mf void LoadText(YTextFile&);

r15-r16:
/= test 2;

r17:
/ @@ \u YControl:
	/ \a \eh \mf + \v;
	/ \tr @@ \ctor @@ \cl YListBox;
	/ \tr @@ \ctor @@ \cl YFileBox;

r18-r19:
/= test 3;

r20:
/ @@ \cl FileBox @@ \u YControl:
	+ \vt \mf void OnTouchHeld(const Runtime::MTouchEventArgs&);

r21:
/ \impl @@ void ShlExplorer::TFrmFileListSelecter::btnTest_Click(const MTouchEventArgs&) @@ \u Shells;
/ \impl @@ \ctor @@ \cl MVisualControl @@ \u YControl;
/ \impl @@ \ctor @@ \cl ShlExplorer::TFormB @@ \u Shells;
/ \a ShlExplorer <=> ShlSetting;

r22:
/ \impl @@ \ctor @@ \cl YFileBox @@ \u YContorl;

r23:
/ \impl @@ \mf void ShlExplorer::TFrmFileListSelecter::btnOK_Click(const MTouchEventArgs&) @@ \u Shells;

r24:
/ \tr \impl @@ \i \ctor @@ \cl ShlExplorer::TFrmFileListMonitor;


DOING:

/ ...

Debug message:
//

NEXT:
* unknown fatal error;
/ fully \impl \cl YListBox: keypad helding operations;
/ fully \impl \u DSReader;

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

