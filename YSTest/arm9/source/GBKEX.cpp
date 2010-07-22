// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-7-22;
// Version = 0.2601; *Build 134 r34;


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
\dtor ::= destructors;
\e ::= exceptions
\en ::= enums
\eh ::= exception handling
\err ::= errors
\ev ::= events
\evh ::= event handling
\f ::= functions
\g ::= global
\gs ::= global scpoe
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
\u ::= unit
\v ::= volatile
\val ::= values

DONE:

r1:
/ make \a @YAssert call expressions about null pointer comparison strict.
- \f @stdex::strnlen(const char*) @@ \u !@YCommon;
- \f @stdex::strnlen2(const char*) @@ \u !@YCommon;
- \f @stdex::strcmpx(const char*, const char*) @@ \u !@YCommon;
+ \i \f @stdex::isstrvalid(char*) @@ \u !@YCommon;
+ \i \f @stdex::isstrvalid(const char*) @@ \u !@YCommon;
+ \i \f @stdex::isstrvalid(char*, const char*) @@ \u !@YCommon;
/ \f char* @stdex::strcatdup(const char*, const char*) -> char* @strcatdup(const char*, const char*, void*(*)(size_t) = std::malloc) @@ \u !@YCommon;

r2:
/= \a @isstrvalid => @isvalidstr;
+ \f char* @stdex::strdup_n(const char*) @@ \u !@YCommon;
* ^ \a \f @strdup @@ !(\u !@)YCommon -> \f @strdup_n;
+ \f char* @stdex::strlen_n(const char*) @@ \u !@YCommon;

r3:
- \i \f void @CreateDefFontCache(CPATH) @@ \u !@YShellUtilities;
/ \a \i \f @DestroyDefFontCache => @DestroySystemFontCache;
/ (\i \f @DestroySystemFontCache @@ \u !@YShellUtilities) => (!\i \f @DestroySystemFontCache @@ \u !@YShellInitialization);

r4:
/= \m @u8 @fontSize => \m @YFont::@SizeType @fontSize @@ \c @YFontCache;
/= ^ \mac (@DefGetter & @DefGetterMember & @DefSetterDef) to \impl (getters & setters) @@ \c @YPenStyle;

r5:
/= \a \m @pType => \m @pFace @@ \c @YFontCache @@ \u !@YFont;
/= \a \m @fontSize => \m @curSize @@ \c @YFontCache @@ \u !@YFont;
/= ^ C++ style casting @@ \u !@YFont;
/= YFontFamily::YFontFamily(YFontCache&, FT_String*) -> YFontFamily::YFontFamily(YFontCache&, const FT_String*);
/= YFontFile::YFontFile(const CPATH, FT_Library&) -> YFontFile::YFontFile(CPATH, FT_Library&);
/= YFontFile::YFontFile(const CPATH, const char*, FT_Library&) -> YFontFile::YFontFile(CPATH, const char*, FT_Library&);
/= YFont::YFont(const YFontFamily& = *GetDefaultFontFamilyPtr(), const SizeType = DefSize, YFontStyle = Regular)
	-> YFont::YFont(const YFontFamily& = *GetDefaultFontFamilyPtr(), SizeType = DefSize, YFontStyle = Regular);
/ \mf const YTypeface* @YFontCache::SetTypeface(const YTypeface&) -> \mf void @YFontCache::SetTypeface(const YTypeface*);
/= \a \tp \c @CPATH -> \a \tp @CPATH @@ \u !@YFont;

r6:
/ @@ \cl @YFontCache:
	/ public \m:
		typedef std::set<YFontFile*, deref_comp<YFontFile> > FFiles
			-> typedef std::set<const YFontFile*, deref_comp<const YFontFile> > FFiles,
		typedef std::set<YTypeface*, deref_comp<YTypeface> > FTypes
			-> typedef std::set<const YTypeface*, deref_comp<const YTypeface> > FTypes;
	/ private \mf:
		void operator+=(YFontFile&) -> void operator+=(const YFontFile&),
		void operator-=(YFontFile&) -> void operator-=(const YFontFile&),
		void operator+=(YTypeface&) -> void operator+=(const YTypeface&),
		void operator-=(YTypeface&) -> void operator-=(const YTypeface&);
	/ \impl GIContainer<YFontFile>, implements GIContainer<YTypeface>, implements GIContainer<YFontFamily>
		->GIContainer<const YFontFile>, implements GIContainer<const YTypeface>, implements GIContainer<YFontFamily>;

r7:
/= \a @YFont => @MFont;
/= \a @YFontFamily => @MFontFamily;
/= \a @YTypeface => @MTypeface;
/= \a @YFontFile => @MFontFile;
+ \cl @YFontCache \inh public \cl @YObject;

r8:
+ style \str "Underline" & "Strikeout" @@ \mf @YFont::@GetStyleName();
/ \tp \en @YFontStyle wrapped to \st @EFontStyle::Styles;
\+ \mf @YFont::@GetStyleName() >> @EFontStyle::GetName();
+ \mf \mac @DefConverter to \type \c @Styles& @@ \st @EFontStyle;
\+ \impl @YFont::@GetStyleName();
+ \mf \op @Styles&() @@ \st @EFontStyle;
+ \ctor @EFontStyle(Styles = Regular) @@ \st @EFontStyle;
/ GetStyleName() -> DefGetter(const FT_String*, StyleName, Style.GetName()) @@ \cl @MFont @@ \u !@YFont;

r9:
* reader not auto-refresh on re-activated by touch screen;

r10:
/= \a \f @InitialSystemFontCache => \f @InitializeSystemFontCache;
/= \a \f @checkSystemFontCache => \f @CheckSystemFontCache;
/= (\sf @printFailInfo >> @@ unnamed \ns) @@ \u !@YShellInitialization;
/= (\sf @defFontFail >> @@ unnamed \ns) @@ \u !@YShellInitialization;
/= (\sf @defFontFail >> @@ unnamed \ns) @@ \u !@YShellInitialization;
/= (\sif @fatalError) >> @@ unnamed \ns) @@ \u !@YShellInitialization;
/= \f (@libfatFail => @LibfatFail) @@ \u !@YShellInitialization;
/= (\sf @installFail >> @@ unnamed \ns) @@ \u !@YShellInitialization;
/= (\f @printFailInfo >> @@ unnamed \ns) @@ \u !@YShellInitialization;
/= \a \f @epicFail => \f @EpicFail;
/= \a \f @checkInstall => \f @CheckInstall;
/ \tr output modified @@ \f @InitializeSystemFontCache @@ \u !@YShellInitialization;
/+ \tb @ \smf YFont::@InitialDefault;
/ (\smf (void InitialDefault() -> bool InitialDefault())) @@ \cl @MFont @@ \u !@YFont;
/= \a \smf @InitialDefault => \smf @InitializeDefault;

r11:
/ \tr @@ \mf @YFontCache::LoadFontFile @@ \u !@YFont:
	+ \tb;

r12:
/ @@ \u !@YFont:
	/@@ \c MFontFamily:
		/= \a \m @FTypes => @FTypesIndex:
			/= private \m FTypes sTypes => FTypesIndex sTypes;
		/ typedef std::map<const FT_String*, MTypeface*, deref_str_comp<FT_String> > FTypesIndex
			-> typedef std::map<const FT_String*, const MTypeface*> FTypesIndex:
			/= private \m FTypesIndex sTypes => FTypesIndex mTypesIndex;
		+ typedef std::set<const MTypeface*> FTypes;
		+ FTypes sTypes;
		+ private \m FTypes sTypes => FTypesIndex mTypesIndex;
			void operator+=(MTypeface&) -> void operator+=(const MTypeface&),
			void operator-=(MTypeface&) -> void operator-=(const MTypeface&);
/= \a @FFaces => @FFacesIndex:
	/= \m @sFaces => @mFacesIndex @@ \cl @YFontCache @@ !@YFont; 

r13:
*- \i \mf bool @YObject::@ReferenceEquals(const YObject&, const YObject&) @@ \u !@ YObject;
/+ \f bool @ReferenceEquals(const YObject&, const YObject&) @@ \u !@ YObject >>+ \tf template<typename _type> bool ReferenceEquals(const _type&, const _type&) @@ \u !@YCommonUtilities;
* excessive \f @strdup_n \inv lead to \mem leak @@ \mf @YFontCache::@LoadTypefaces @@ \u !@YFont;
/= \a @sFaces => @mFacesIndex;
/ @@ \u !@YFont:
	/@@ \c YFontCache:
		+ typedef std::set<MFontFamily*> FFaces;
		+ \m FFaces sFaces;
		/ typedef std::set<const MFontFile*, deref_comp<const MFontFile> > FFiles
			-> typedef std::set<const MFontFile*> FFiles,
		/ typedef std::set<const MTypeface*, deref_comp<const MTypeface> > FFiles
			-> typedef std::set<const MTypeface*> FFiles;
		/ \mf LoadTypefaces:
			+ 2-level combined \tb & \cb,
			/ \tr;

r14-r15:
= font loading testing 1;

r16:
/= \tr ^ iterator operator-> instead of (operator. & operator*) @@ \u (!@YFont & !@YTemer);
/ @@ \cl @YFontCache @@ \u !@YFont:
/= \tr @@ \mf YFontCache::@LoadFontFileDirectory @@ \u !@YFont;
/= \tr @@ \mf YFontCache::@LoadFontFile @@ \u !@YFont;
* \tr @@ \mf YFontCache::@LoadTypefaces @@ \u !@YFont;

r17:
= font loading testing 2;

r18:
/ \tr @@ \mf LoadTypefaces;
	/ \tb & \cb;
/ \tr @@ \mf LoadFontFileDirectory;
	+ \tb & \cb;

r19:
/ \tr @ShlA::TFormC @@ \u !@Shells;

r20:
* \tr @@ \mf YFontCache::@LoadTypefaces @@ \u !@YFont;

r21:
= font loading testing 3;

r22:
* \tr @@ \cl YFontCache @@ \u !@YFont;
	/ typedef std::set<const MFontFile*> FFiles
		-> typedef std::set<const MFontFile*, deref_comp<const MFontFile> > FFiles,
	/ typedef std::set<const MTypeface*> FFiles
		-> typedef std::set<const MTypeface*, deref_comp<const MTypeface> > FFiles;
	/ typedef std::set<MFontFamily*> FFiles
		-> typedef std::set<MFontFamily*, deref_comp<MFontFamily> > FFiles;

r23-r33:
= font loading testing 4;

r34:
/ \mf LoadTypefaces @@ \cl YFontCache @@ \u !@YFont:
	*+ 3-level combined \tb & \cb,

DOING:

/ ...

NEXT:
+ \impl \cl ShlReader;
+ fully \impl btnTest;

TODO:

Make "UpdateFont()" more efficienct.

Use in-buffer background color rendering and function "CopyToScreen()" to inplements "TextRegion" background outputing more efficienct.

Consider to simplify the general window class: "YForm".

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

