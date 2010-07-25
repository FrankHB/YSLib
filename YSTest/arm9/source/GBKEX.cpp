// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-7-25;
// Version = 0.2604; *Build 135 r32;


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

r1:
* \decl \f char* stdex::@strdup_n => std::size_t stdex::@strlen_n @@ \u YCommon;
/= \a \mac @*Def => @*De \def @@ \h "ysdef.h";

r2:
+ \i \f char* strcpy_n(char*, const char*) @@ \u !@YCommon;
/= ^ stdc++ \h instead of stdc \h;
+ \i \f char* stpcpy_n(char*, const char*) @@ \u !@YCommon;
+ \i \f int stricmp_n(const char*, const char*) @@ \u !@YCommon;
- \a \i \f @stdex::isstrvalid @@ \u !@YCommon;

r3:
+ \mf const MTypeface* MFontFamily::GetTypefacePtr(EFontStyle) const;
+ assertion @@ \mf EFontStyle::GetName();
/ \tr @@ \mf MFont::Update();

r4:
+ \mf MFont::SetFont @@ \u YFont;
/ ^ \mac DefSetterMember to \impl \i \mf YPenstyle::SetFont @@\u YGDI;
+ ^ \mac DefGetter to \impl \mf YLabel::GetFont @@\u YControl;
+ ^ \mac DefSetterMember to \impl \mf YLabel::SetFont @@\u YControl;
* \mac @@ \h "ysdef.h";
/= ^ \mac to \impl \i \mf @GetFont @@ \cl YPenStyle @@ \u YGDI;

r5:
/ @@ \u YFont:
	/ \ret \tp of \mf YFontCache::SetTypeface(const MTypeface*): void -> bool;
	/ \ret \tp of \mf YFontCache::SetFontSize(MFont::SizeType): void -> bool;
	/ \ret \tp of \mf MFont::UpdateSize(): void -> bool;
	/ \ret \tp of \mf MFont::Update(): void -> bool;
/ @@ \u YGDI:
	- \i \mf void YPenStyle::UpdateFont();
	- \i \mf void YPenStyle::UpdateFontSize();
	/ \ret \tp of \i \mf YPenStyle::SetFontStyle(EFontStyle): void -> bool;
	/ \ret \tp of \i \mf YPenStyle::SetFontSize(MFont::SizeType): void -> bool;
- \mf YTextState& YTextState::operator=(const YTextState&) @@ \u YText;
/ \tr @@ \mf YTextRegion::PutNewline() @@ \u YText;

r6-r7:
* \impl @@ bool DrawRect(YGIC&, SPOS, SPOS, SPOS, SPOS, PixelType) @@ \u YGDI;

r8:
* @@ \u YGDI:
	* \impl @@ bool DrawHLineSeg(YGIC&, SPOS, SPOS, SPOS, PixelType);
	* \impl @@ bool DrawVLineSeg(YGIC&, SPOS, SPOS, SPOS, PixelType);
	* \impl @@ bool DrawRect(YGIC&, SPOS, SPOS, SPOS, SPOS, PixelType);
/= @@ \u YGDI:
	\= \sf bool DrawObliqueLine(YGIC&, SPOS, SPOS, SPOS, SPOS, PixelType) >> unnamed \ns;
	\= \i \sf SPOS blitMinX(SPOS, SPOS) >> unnamed \ns;
	\= \i \sf SPOS blitMinY(SPOS, SPOS) >> unnamed \ns;
	\= \i \sf SPOS blitMaxX(SPOS, SPOS) >> unnamed \ns;
	\= \i \sf SPOS blitMaxY(SPOS, SPOS) >> unnamed \ns;

r9:
/ @@ \u YControl:
	/ @@ \cl YLabel:
		/ \m Drawing::MFont& Font -> Drawing::MFont Font;
		/ \m Drawing::MPadding& Margin -> Drawing::MPadding Margin;
	/ @@ \cl YListBox:
		/ \m Drawing::MFont& Font -> Drawing::MFont Font;
		/ \m Drawing::MPadding& Margin -> Drawing::MPadding Margin;

r10-r11:
*= \tf template<class T> GHResource<T>& GetGlobalResource() @@ \u YResource;
/= ^ \mac @DefGetter @@ \cl YTextBuffer @@ \u YTextManager;
/= + \m typedef size_t SizeType @@ \cl YTextBuffer @@ \u YTextManager;
/= \tr @@ \cl MDualScreenReader;

r12:
/+ @@ \u YFont:
	/+= asseration @@ (\f const MTypeface* GetDefaultTypefacePtr() & \i \f const MFontFamily* GetDefaultFontFamilyPtr()) @@ \u YFont;
	+ \i \f inline const MFontFamily& GetDefaultFontFamily() with asseration;
/= \a ^ of \i \f GetDefaultFontFamilyPtr -> GetDefaultFontFamily; 
/ @@ \u YText:
	/ \exp \ctor YTextState(YFontCache*) -> YTextState(YFontCache&);
	/ \exp \ctor YTextRegion(YFontCache*) -> YTextRegion(YFontCache&);
	/= ^ \mac (DefGetter & DefSetter) @@ \cl YTextState;
	/= ^ \mac DefGetter @@ \cl YTextRegion;
/= \tr @@ \u YFont:
	- unused variable @@ \mf void YSLib::Drawing::YFontCache::LoadFontFileDirectory(const char*, const char*);
	* \init variable @@ \mf bool YSLib::Drawing::MFont::Update();
	*= \init variable @@ \mf void YSLib::Drawing::YFontCache::LoadTypefaces(const YSLib::Drawing::MFontFile&);

r13:
/ \m YFontCache* pfc -> YFontCache& fc @@ \cl MDualScreenReader;
	/ \ctor @@ \cl MDualScreenReader;

r14:
/= \tr @@ \decl @@ \cl MDualScreenReader @@ \h "DSReader.h";
+ \cl Design::NonCopyable @@ \u YCoreUtilities;
+ using Design::NonCopyable @@ \h "ycutil.h";
/ \cl Text::MTextBuffer \inh NonCopyable;

r15:
/= @@ \cl YApplication:
	- private copy \ctor;
	- private operator=;
	+ \inh NonCopyable;

r16;
+ @@ \u YTextManager:
	+ \cl MTextBlock : public MTextBuffer;
	+ \cl MTextMap;

r17:
+ \tb @@ \ctor @@ \cl MDualScreenReader;
/ \inh Text::MTextBuffer-> \m Text::MTextMap Blocks @@ \cl MDualScreenReader;

r18:
+/ @@ \u YTextManager;
	+ \cl Text::MTextFileBuffer;
	/ ^ \cl Text::MTextFileBuffer @@ \u YTextManager instead of \cl Text::MTextMap;
	+ typedef MTextBuffer::SizeType SizeType @@ \cl MTextMap;

r19:
+/ @@ \u YTextManager;
	/ private \m MapType Map -> \protected \m MapType Map @@ \cl MTextMap;
	+/ MTextBlock& operator[](const IndexType&) @@ \cl MTextFileBuffer;
	/ void operator+=(std::pair<const IndexType&, MTextBlock*>&) -> void operator+=(MTextBlock&) @@ \cl MTextMap;
	/ void operator-=(const IndexType&) -> bool operator-=(const IndexType&) @@ \cl MTextMap;

r20-r31:
/+ \tr range checking & \tb @@ MTextBlock& MTextFileBuffer::operator[](const IndexType&);
/ @@ \cl YFile @@ \u YFile
	/= typedef u32 SizeType;
	/ ^ \mac (DefBoolGetter & DefGetter);
	/ \tr @@ \mf @OpenFile;
-= \tb @@ \cl @MDualScreenReader;

r32:
/ @@ \cl YFile @@ \u YFile:
	+ \mf void Release();
	/ \mf OpenFile => Open;
	/ \i \mf GetFilePtr => GetPtr;
	/ \i \mf GetFileLen => GetLength;
	/ \i \mf fEOF => feof const;
	/ \i \mf int feof() -> int feof() const;

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

