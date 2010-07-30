// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-7-30;
// Version = 0.2608; *Build 137 r26;


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

r2:
= test 1;

r3:
/ @@ \cl YLabel @@ \u YControl:
	- DefGetter(const MFont&, Font, Font);
	- DefSetter(const MFont&, Font, Font);

r4:
/ @@ \u YText:
	/ \mf void MTextRegion::PrintChar(u32);
	/ \tr \t \mf template<class _CharT> u32 MTextRegion::PutLine(const _CharT*);

r5:
/ @@ \u YText:
	- \m YFontCache* MTextState::pCache;
	/ \mf void MTextRegion::PrintChar(u32);
r6:
* \tr @@ \mf bool MFont::Update() @@ \u YFont;
/= \tr @@ \mf void MTextRegion::PrintChar(u32) @@ \u YText;

r7-r8:
* @@ \u YControl:
	* \tr @@ \mf void YLabel::DrawForeground();
	* \tr @@ \mf void YListBox::DrawForeground();

r9:
/ @@ \u YFont:
	/ \tr @@ \mf bool MFont::Update();
	/ \mf (bool YFontCache::SetFontSize(MFont::SizeType) -> void YFontCache::SetFontSize(MFont::SizeType));
	/ \mf (bool UpdateSize() -> void UpdateSize()) @@ \cl MFont;

r10:
* @@ \u YControl:
	* \tr @@ \mf void YListBox::DrawForeground();
	* \tr @@ \mf void YLabel::DrawForeground();

r11:
/= \simp \mf u8 MTextRegion::PutChar(u32 c) @@ \u YText;

r12:
= test 2;

r13-r14:
/ \impl buffered reader:
	/ @@ \cl MTextFileBuffer @@ \u YText:
		/ \tr @@ \mf \op [];
		+ \mf void SetPosition(SizeType);
		+ \m SizeType nLen @@ \cl MTextFileBuffer;
		+ \mf DefGetter(SizeType, TextLength, nLen);
		+ \mf DefGetter(SizeType, Position, nPos);
	/ \tr @@ \cl MDualScreenReader @@ \u DSReader;

r15:
/ \impl buffered reader:
	/ @@ \cl MTextFileBuffer @@ \u YText:
		+ \mf uchar_t* GetTextPtr();
	/ \tr @@ \cl MDualScreenReader @@ \u DSReader;

r16:
/ @@ \u YText:
	+ \mf SizeType YTextBuffer::LoadN(YTextFile&, SizeType);
	/ \tr @@ \mf MTextBlock& MTextFileBuffer::operator[](const IndexType&);
	+ \cl TextIterator @@ \cl MTextFileBuffer;

r17:
/ @@ \u YText:
	/ \cl TextIterator @@ \cl MTextFileBuffer;
	/ \tr @@ \cl MTextFileBuffer;

r18:
/ @@ \u YText:
	/ \tr @@ \cl TextIterator @@ \cl MTextFileBuffer;
	/ \cl MTextFileBuffer;
/= \mf u32 MDualScreenReader::TextFill(u32) @@ \u DSReader;

r19:
+ \mac of exception specification @@ \h "ysdef.h";
^ \mac ythrow @@ \u YText;
/ test:
	+ \mac of exception specification @@ \h "yadapter.h";

r20:
/ test: undo:
	/ \rem \mac of exception specification @@ \h "yadapter.h";

r21:
+ DefGetter(SizeType, Position, nPos) @@ \cl MTextFileBuffer::TextIterator @@ \u YTextManager;
/+ @@ \cl MTextRegion @@ \u YText:
	/+ template<class _CharT> u32 PutLine(_CharT) -> template<typename _constCharIteratorType> u32 PutLine(_constCharIteratorType);
	/+ template<class _charType> u32 PutString(const _charType*) -> template<typename _constCharIteratorType> u32 PutString(_constCharIteratorType);

r22:
+ @@ \cl MTextFileBuffer::TextIterator @@ \u YTextManager:
	+ \mf TextIterator operator+(std::ptrdiff_t);
	+ \i TextIterator operator-(std::ptrdiff_t);
	+ \mf TextIterator& operator+=(std::ptrdiff_t);
	+ \i \mf TextIterator& operator-=(std::ptrdiff_t);
/ u32 MDualScreenReader::TextFill(u32) @@ \u DSReader;

r23:
/ @@ \u YTextManager:
	/ \a \mf operator const uchar_t* -> GetTextPtr @@ \cl MTextFileBuffer;
	+ \i friend std::ptrdiff_t operator-(TextIterator, TextIterator) @@ \cl MTextFileBuffer::TextIterator;
	/ uchar_t MTextFileBuffer::TextIterator::operator*() ythrow();
/ \impl @@ u32 MDualScreenReader::TextFill(u32) @@ \u DSReader;

r24:
/= \impl @@ u32 MDualScreenReader::TextFill(u32) @@ \u DSReader;
	/ ^ \mf MTextFileBuffer::TextIterator::GetPosition @@ \u YTextManager;

r25:
- u32 nLoad @@ \cl MDualScreenReader @@ \u DSReader;

r26:
/ @@ \cl MTextFileBuffer @@ \u YTextManager:
	/ private (SizeType nLen -> const SizeType nLen);
	* \ctor;
	/ TextIterator TextIterator::operator+(std::ptrdiff_t);
	/ TextIterator& TextIterator::operator+=(std::ptrdiff_t);
	/ TextIterator& TextIterator::operator++() ythrow();
	/ TextIterator& TextIterator::operator--() ythrow();
	* TextIterator& TextIterator::operator++(int) ythrow();
	* TextIterator& TextIterator::operator--(int) ythrow();
/ @@ \u YText:
/ \mf >> \ns @Text ~ \cl MTextRegion:
	u32 MTextRegion::GetPrevLnOff(const uchar_t*, u32) const => u32 GetPrevLnOff(const MTextRegion&, const uchar_t*, u32);
	u32 MTextRegion::GetPrevLnOff(const uchar_t*, u32, u16) const => u32 GetPrevLnOff(const MTextRegion&, const uchar_t*, u32, u16);
	u32 MTextRegion::GetNextLnOff(const uchar_t*, u32) const => u32 GetNextLnOff(const MTextRegion&, const uchar_t*, u32);
/ @@ \u YReader;
	/ bool MDualScreenReader::LineUp();
	/ bool MDualScreenReader::LineDown();
	/ bool MDualScreenReader::ScreenUp();

DOING:

/ ...

NEXT:
+ \impl \cl ShlReader;
+ fully \impl btnTest;

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

