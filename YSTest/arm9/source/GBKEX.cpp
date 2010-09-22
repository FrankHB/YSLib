// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-9-23;
// Version = 0.2617; *Build 152 r31;


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
\val ::= values

DONE:
r1:
/ \cl MFileList @@ \u YFileSystem:
	/= \i \mf const Path& GetDirectory() const
		-> DefGetter(const Path&, Directory, Directory);
	/= \i \mf const ListType& GetList() const
		-> DefGetter(const ListType&, List, List);
	- \mf void GoToRoot();
	- \mf v void GoToParent();

r2:
/ @@ \u YFileSystem:
	/ @@ \cl Path:
		+ \ctor Path(const ValueType*);
	/ @@ \cl MFileList:
		/ \ctor \cl MFileList() -> MFileList(CPATH = NULL);
		/= \tr \impl @@ \mf ListType::size_type LoadSubItems();
		/ \ret \tp @@ \mf void GoToSubDirectory(const std::string&) -> bool;
	+ \f bool Validate(const std::string&);
	+ \f bool Validate(const Path&);

r3:
/ @@ \cl Path @@ \u YFileSystem:
	- \mf void GoToPath(const Path&);
	/ \mf GoToSubDirectory => operator/=;
	+ \i \mf operator/=(const String&);
/ @@ \cl YFileBox @@ \u YControl:
	- \eh \s \mf void OnClick(IVisualControl&, const MTouchEventArgs&);
	+ \eh \s \mf void OnCobfirmed(IVisualControl&, const MIndexEventArgs&);
	/ \impl @@ \ctor;
/ @@ \u YString:
	/ @@ \cl String:
		/= \tr \impl @@ \i \ctor template<class _tChar> String(const _tChar*);
	/= \f String MBCSToString(const char*, const CSID& = CS_Local);
	+ \f std::string StringToMBCS(const String&, const CSID& = CS_Local);
/ @@ \u CHRProcessing:
	/ \a wchar_t => fchar_t;
	+ \f std::size_t wcslen(const fchar_t*) @@ unnamed \ns;
+ #define CHRLIB_WCHAR_T_SIZE 4 @@ \h <platform.h>;
+ #define FS(str) reinterpret_cast<const CHRLib::fchar_t*>(L##str) @@ \h CHRDefinition;
/ ^ FS @@ \h <Shells.h>;

r4-r8:
/= test 1;
/ \tr \impl @@ \u Shells;

r9:
+ \f bool IsAbsolute(CPATH) throw() @@ \u YCommon;
/ @@ \cl Path @@ \u YFileSystem:
	* DefBoolGetter(Empth, pathname.empty()) -> DefBoolGetter(Empty, pathname.empty());
	/ \impl \f bool IsRelative() -> DefBoolGetter(Relative, !IsAbsolute());
	/ \impl \f bool IsAbsolute() -> DefBoolGetter(Absolute, platform::IsAbsolute(GetNativeString().c_str()));

r10:
+ \f bool IsAbsolute(CPATH) throw() -> \f bool IsAbsolute(CPATH) @@ \u YCommon;

r11:
/ @@ \cl Path @@ \u YFileSystem:
	+ \cl iterator;
	+ typedef iterator const_iterator;
	+ \mf iterator begin() const;
	+ \mf iterator end() const;
	+ \s \m \c ValueType Slash(DEF_PATH_DELIMITER);

r12:
/ @@ \u YFileSystem:
	+ \inc \h <iterator>;
	/ @@ \cl Path:
		/ \cl iterator + \inh public std::iterator<std::bidirectional_iterator_tag, Path>;
		/ \impl @@ \cl iterator;
	/ \tr \impl @@ \f Path operator/(const Path& lhs, const Path& rhs);

r13-r15:
/ @@ \cl Path @@ \u YFileSystem:
	/ \impl @@ \mf operator/=(const Path&);
	/ \impl other \f;
+ \f std::size_t GetRootNameLength(CPATH) @@ \u YCommon;

r16:
* \impl @@ \mf (iterator& operator++()) & (iterator& operator--()) & (value_type operator*()) @@ \cl iterator @@ \cl Path @@ \u YFileSystem;

r17-r31:
/ test 2;
* @@ \cl Path @@ \u YFileSystem:
	* \impl @@ \mf Path& operator/=(const Path&);
	* \impl \mf (iterator& operator--()) & (value_type operator*()) @@ \cl iterator;


DOING:

/ ...

Debug message:
//

NEXT:
/ fully \impl \cl YListBox;
/ fully \impl \u YFileSystem;

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

