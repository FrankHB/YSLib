// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-8-8;
// Version = 0.2614; *Build 139 r35;


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
= test 0;

r2:
/ @@ \cl MTextRegion @@ \u YText:
	/ \t \mf template<typename _constCharIteratorType> std::size_t PutString(_constCharIteratorType)
		-> template<typename _outIt> _outIt PutLine(_outIt);
	/ \t \mf template<typename _constCharIteratorType> std::size_t PutLine(_constCharIteratorType)
		-> template<typename _outIt> _outIt PutLine(_outIt);
+ (\i friend \f \op (==, !=) @@ \ns Text) @@ \cl MTextFileBuffer::TextIterator @@ \u YTextManager;
/ \impl @@ \u DSReader;

r3:
+ \i \mf std::size_t PutString(const MString&) @@ \cl MTextRegion @@ \u YText;
/ @@ \u YTextManager:
	/ \i friend \f bool operator==(const MTextFileBuffer::TextIterator&, const MTextFileBuffer::TextIterator&)
		-> bool operator==(const MTextFileBuffer::TextIterator&, const MTextFileBuffer::TextIterator&) ythrow();
	/ \i friend \f bool operator!=(const MTextFileBuffer::TextIterator&, const MTextFileBuffer::TextIterator&)
		-> bool operator!=(const MTextFileBuffer::TextIterator&, const MTextFileBuffer::TextIterator&) ythrow();

r4:
/ @@ \u YTextManager:
	+ friend \f bool operator<(const MTextFileBuffer::TextIterator&, const MTextFileBuffer::TextIterator&) ythrow();
	+ \i friend \f bool operator>(const MTextFileBuffer::TextIterator&, const MTextFileBuffer::TextIterator&) ythrow();
	+ \i friend \f bool operator<=(const MTextFileBuffer::TextIterator&, const MTextFileBuffer::TextIterator&) ythrow();
	+ \i friend \f bool operator>=(const MTextFileBuffer::TextIterator&, const MTextFileBuffer::TextIterator&) ythrow();
	/ \i friend \f bool operator<(const MTextFileBuffer::TextIterator&, const MTextFileBuffer::TextIterator&) ythrow() -> -\i;

r5:
/ @@ \u YText:
	/ \ret \ty @@ \f u32 GetPrevLnOff(const MTextRegion&, const uchar_t*, u32) -> const uchar_t;
	/ \ret \ty @@ \f u32 GetPrevLnOff(const MTextRegion&, const uchar_t*, u32, u16) -> const uchar_t;
	/ \ret \ty @@ \f u32 GetNextLnOff(const MTextRegion&, const uchar_t*, u32) -> const uchar_t;

r6:
/ @@ \u YText:
	/ \f u32 GetNextLnOff(const MTextRegion&, const uchar_t*, u32)
		-> u32 GetNextLnOff(const MTextRegion&, const uchar_t*);

r7:
/ @@ \u YText:
	/= \ns Drawing::{
		const uchar_t* GetPrevLnOff(const MTextRegion& r, const uchar_t* s, u32 n);
		const uchar_t* GetPrevLnOff(const MTextRegion& r, const uchar_t* s, u32 n, u16 l);
		const uchar_t* GetNextLnOff(const MTextRegion& r, const uchar_t* p);
		u32 ReadX(YTextFile& f, MTextRegion& txtbox, u32 n);
	}>> \ns Text;
	+{
		typedef std::size_t SizeType;
		typedef std::size_t IndexType;
	} @@ \ns Text;
/ \u YTextManager:
	/ @@ \cl MTextBuffer:
		/ typedef std::size_t SizeType -> typedef Text::SizeType SizeType;
		/ typedef std::size_t IndexType -> typedef Text::IndexType IndexType;
	- \inc \u (YString, YFile_(Text));
	+ \inc \u YText;

r8:
-= \simp \u YTextManager:
	-= @@ \cl YTextBuffer:
		-= typedef Text::SizeType SizeType;
		-= typedef Text::IndexType IndexType;
	-= @@ \cl YTextFileBuffer:
		-= typedef MTextBlock::SizeType SizeType;
		-= typedef MTextBlock::IndexType IndexType;
		-= @@ \cl TextIterator:
			-= typedef ContainerType::SizeType SizeType;
			-= typedef ContainerType::IndexType IndexType;

r9:
/ \ns Text::{
	typedef std::size_t SizeType;
	typedef std::size_t IndexType;
} >> \h "ysdef.h" ~ \u YText;
/ \u YTextManager:
	- \inc \u YText;
	+ \inc \u (YString, YFile_(Text));
* @@ \cl MTextBuffer @@ \u YTextManager:
	* \impl @@ \i \mf IndexType GetPrevNewline(SizeType) -> inline IndexType GetPrevNewline(IndexType);
	* \impl @@ \i \mf IndexType GetNextNewline(SizeType) -> inline IndexType GetNextNewline(IndexType);

r10:
/ @@ \cl MTextFileBuffer::TextIterator @@ \u YTextManager:
	/ \impl @@ \mf TextIterator& operator++() ythrow();
	/ \impl @@ \mf TextIterator& operator--() ythrow();
	/ \mf IndexType GetBlockLength() const + \es ythrow();
	/ \mf IndexType GetBlockLength(BlockIndexType) const + \es ythrow();

r11:
* \impl @@ IndexType MTextFileBuffer::TextIterator::GetBlockLength(BlockIndexType) const ythrow() @@ \u YTextManager;

r12-r29:
/ @@ \u YText:
	/ \f uchar_t* GetPrevLnOff(const MTextRegion&, const uchar_t*, u32)
		-> uchar_t* GetPreviousLinePtr(const MTextRegion&, const uchar_t*, const uchar_t*)
	/ \f uchar_t* GetPrevLnOff(const MTextRegion&, const uchar_t*, u32, u16)
		-> uchar_t* GetPreviousLinePtr(const MTextRegion&, const uchar_t*, const uchar_t*, u16);
	/ \f uchar_t* GetNextLnOff(const MTextRegion&, const uchar_t*)
		-> uchar_t* GetNextLinePtr(const MTextRegion&, const uchar_t*, const uchar_t*);
/ \impl @@ \u DSReader;

r30:
/ ^ libnds 201007;

r31-r32:
/ test 2:
	/ \tr @@ \u Shell;

r33:
/ @@ \u YReference
	- \mf operator u32() @@ \t \cl HHandle;
	/= \t \para @@ \t \i \f GetPointer:
		/= class _Tp -> typename _refType;
	+ \t \i \f template<typename _type, typename _refType> handle_cast(GHHandle<_refType>);
/ \tr @@ \h "ysdef.h";

r34-r35:
/ test 2:
	/ \tr @@ \u Shell;


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

