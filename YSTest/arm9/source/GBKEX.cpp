// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-8-15;
// Version = 0.2614; *Build 140 r24;


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

r2-r8:
/ @@ \u YText:
	+ \sf const uchar_t* rfind(YFontCache&, SDST , const uchar_t*, const uchar_t*, uchar_t);
	/ \impl @@ \f uchar_t* GetPreviousLinePtr(const MTextRegion&, const uchar_t*, const uchar_t*):
		^ \sf rfind;

r9:
/ @@ \u YText:
	/ \impl @@ \f uchar_t* GetPreviousLinePtr(const MTextRegion&, const uchar_t*, const uchar_t*, u16):
		^ \sf rfind;

r10:
/ @@ \u YText:
	/ merge uchar_t* GetPreviousLinePtr(const MTextRegion&, const uchar_t*, const uchar_t*)
		& uchar_t* GetPreviousLinePtr(const MTextRegion&, const uchar_t*, const uchar_t*, u16)
		-> uchar_t* GetPreviousLinePtr(const MTextRegion&, const uchar_t*, const uchar_t*, u16 = 1);
	/ (\sf -> \f) @@ const uchar_t* rfind(YFontCache& cache, SDST nw, const uchar_t* p, const uchar_t* g, uchar_t f);

r11:
/ @@ \u YText:
	+= \f const uchar_t* rfind(YFontCache&, SDST, const uchar_t*, const uchar_t*, uchar_t);
		-> \tf template<typename _outIt, typename _charT> _outIt rfind(YFontCache&, SDST, _outIt, _outIt, _charT);
	+= \f const uchar_t* GetPreviousLinePtr(const Drawing::MTextRegion& r, const uchar_t*, const uchar_t*, u16 l = 1);
		-> \tf template<typename _outIt> _outIt GetPreviousLinePtr(const Drawing::MTextRegion&, _outIt, _outIt, u16 l = 1);
	+= \f const uchar_t* GetNextLinePtr(const Drawing::MTextRegion& r, const uchar_t*, const uchar_t*);
		-> \tf template<typename _outIt> _outIt GetNextLinePtr(const Drawing::MTextRegion&, _outIt, _outIt);

r12:
/ \tr @@ \impl @@ \tf GetPreviousLinePtr @@ \u Yext;

r13:
/ @@ \h "chrdef.h":
	+ \inc \h <platform.h>;
	/ typedef unsigned short uchar_t -> typedef u16 uchar_t;
	/ typedef signed int uchardiff_t -> typedef s32 uchardiff_t;

r14-r16:
/ \impl @@ \cl MDualScreenReader @@ \u DSReader:
	+ \m Text::MTextFileBuffer::TextIterator itUp, itDn;
	- \m s32 offUp, offDn;
	/ \mf u32 TextFill() -> void FillText();
	/ \tr \impl;
	/ \mf u32 TextInit() -> void InitText();

r17:
/ test 1;
/ \tr \impl @@ \ctor ShlReader::ShlReader() @@ \u Shells;

r18:
/ \tr \impl @@ \mf InitText \cl MDualScreenReader @@ \u DSReader;

r19:
/ @@ \cl MTextRegion @@ \u YText:
	+ \m \tf template<typename _outIt, tpename _charT> PutLine(_outIt, _outIt, _charT = '\0');
	+ \m \tf template<typename _outIt, tpename _charT> PutString(_outIt, _outIt, _charT = '\0');
	/ \ret \ty @@ \i \mf std::size_t PutLine(const MString&) -> MString::size_type;
	/ \ret \ty @@ \i \mf std::size_t PutString(const MString&) -> MString::size_type;
/ \tr \impl @@ \mf InitText \cl MDualScreenReader @@ \u DSReader;

r20:
/ @@ \u CHRMap:
	+^ \tf @@ code mapping functions;
	- !\t code mapping functions;
/ @@ \u CHRProc:
	/ \impl @@ \f ubyte_t ToUTF(const char*, uchar_t&, const CSID&) & \f ubyte_t ToUTF(FILE*, uchar_t&, const CSID&);
	/= \a getc => std::getc;

r21:
/ @@ \u CHRMap:
	+ typedef uchar_t CMF(ubyte_t&, const char*);
	+ typedef uchar_t CMF_File(ubyte_t&, FILE*);
/ @@ \u CHRProc:
	+ \tf template<_codemapFuncType> _codemapFuncType* GetCodeMapFuncPtr(const CSID&) @@ unnamed \ns;
	/ \impl @@ \f ubyte_t ToUTF(const char*, uchar_t&, const CSID&) & \f ubyte_t ToUTF(FILE*, uchar_t&, const CSID&);
/= \a _charType => _charT;

r22-r24:
/ test 2:
	* \impl @@ \tf template<_codemapFuncType> _codemapFuncType* GetCodeMapFuncPtr(const CSID&) @@ unnamed \ns @@ \u CHRProc;

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

