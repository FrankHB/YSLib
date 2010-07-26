// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-7-26;
// Version = 0.2606; *Build 136 r36;


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
/ @@ \h "yfont.h";
	/ DefGetter(FFacesIndex::size_type, FacesN, mFacesIndex.size()) -> DefGetter(FFaces::size_type, FacesN, sFaces.size()) @@ \cl YFontCache;
	/= \exp \ctor MFont(const MFontFamily& = *GetDefaultFontFamilyPtr(), SizeType = DefSize, EFontStyle = EFontStyle::Regular)
		-> MFont(const MFontFamily& = GetDefaultFontFamily(), SizeType = DefSize, EFontStyle = EFontStyle::Regular) @@ \cl MFont;
	/ DefGetter(const FFacesIndex&, Faces, mFacesIndex) => DefGetter(const FFacesIndex&, FacesIndex, mFacesIndex);
	+ DefGetter(const FFaces&, Faces, sFaces);

r3:
+ DefGetterMember(YFontCache&, Cache, Font) @@ \cl YPenStyle @@ \u YGDI;
/ \tr @@ \i \mf YTextState& YTextState::operator=(const YPenStyle&) @@ \u YText;
/ @@ \cl MFont @@ \u YFont:

r4-r6:
/ \tr @@ \mf void YFontCache::LoadTypefaces(const MFontFile&) @@ \u YFont;

r7-r8:
= test 1;

r9:
* \tr @@ \cl ShlA::TFormC::lblC_Click @@ \u Shells;

r10-r15:
= test 2;

r16:
* \tr @@ \mf u8 YTextRegion::PutChar(u32) @@ \u YText;
/= \tr @@ \mf void YTextRegion::PrintChar(u32) @@ \u YText;
/ \tr @@ \ctor YLabel @@ \u YControl;
- \mf void YLabel::DrawBackground();
/ \tr @@ \mf void YTextRegion::PutNewline();
/= \h changed:
	- <wctype.h>,
	+ <cwctype>;

r17:
/ \cl MString @@ \u YString:
	- \inh YObject;
	- typedef YObject ParentType;
/= \a MString => MString;
- \i \dtor MString::~MString() @@ \u YString;

u18:
- \inh YObject @@ \cl YException @@ \u YException;
/= \a YException => MException;
/= \a YGeneralError => MGeneralError;
/= \a YLoggedError => MLoggedEvent;

u19:
- YObject::Empty @@ \cl YObject @@ \u YObject;
/ YObject::Empty -> GetZeroElement<YObject>;
/ YEventArgs::Empty -> GetZeroElement<YEventArgs>;

u20:
- YImage::Empty @@ \cl YImage @@ \u YResource;
- \inh YObject @@ \cl YGIC @@ \u YGDI;
/= \a YGIC => MGIC;

u21:
/ @@ \h "ysdef.h":
	+ struct EmptyType {};
	/ typedef YObject YEventArgs -> typedef EmptyType YEventArgs;

u22:
/= \a YEventArgs => MEventArgs;
/= \a YTouchEventArgs => MTouchEventArgs;
/= \a YKeyEventArgs => MKeyEventArgs;
/= \a YScreenPositionEventArgs => MScreenPositionEventArgs;

r23:
/= \a YIndexEventArgs => MIndexEventArgs;

r24:
- \inh YObject @@ \cl YPenStyle @@ \u YGDI;
- typedef YObject ParentType @@ \cl YPenStyle @@ \u YGDI;
/= \a YPenStyle => MPenStyle;
/= \a YTextState => MTextState;
/= \a YTextRegion => MTextRegion;

r25:
- \inh NonCopyable @@ \cl YApplication @@ \u YApplication;
+ \inh NonCopyable @@ \cl YObject @@ \u YObject;

r26-r34:
/ DefSetterMember(const MFont&, Font, Font) -> DefSetter(const MFont&, Font, Font) @@ \cl YLabel @@ \u YControl;
/ \simp @@ \cl MPenStyle @@ \u YGDI:
	/ \m (MFont Font & PixelType Color) accessibility: private -> public;
	- PDefHead(MFont&, GetFont) ImplRet(Font);
	- DefGetter(const MFont&, Font, Font);
	- DefGetter(MFont::SizeType, FontSize, Font.GetSize());
	- DefGetter(PixelType, Color, Color);
	- DefSetterMember(const MFont&, Font, Font);
	- \i \mf bool SetFontStyle(EFontStyle);
	- \i \mf bool SetFontSize(MFont::SizeType);
	- DefSetterDe(PixelType, Color, Color, ~0);
	/ \tr @@ \u (DSReader & YControl & MTextRegion);

r35:
\decl \mf void SetSize(SizeType) -> void SetSize(SizeType = DefSize) @@ \cl MFont @@ \u YFont;

r36:
/= \tr @@ \h ("ycutil.h" & "yref.h" & "yfont.h");
/= \a YTouchState => MTouchState;

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

