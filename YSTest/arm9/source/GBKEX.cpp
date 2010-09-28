// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-09-29 03:34 + 08:00;
// Version = 0.2677; *Build 156 r40;


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
\mo ::= member objects
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

$using:
\u YControl
{
	\cl MVisualControl;
	\cl YListBox;
	\cl YFileBox;
}

$DONE:
r1:
/= test 0;

r2:
* \impl @@ \mf void MDualScreenReader::UnloadText() @@ \cl DSReader;

r3-r6:
/= test 1;

r7:
+ \i \ctor Path(const String&) @@ \cl Path @@ \u YFileSystem;
/= \simp \impl @@ \mf IO::Path YFileBox::GetPath() const;

r8-r10:
/= test 2;

r11:
/ @@ \cl Path @@ \u YPath:
	/ typedef NativePathCharType ValueType -> typedef uchar_t ValueType;
	+ typedef std::basic_string<NativePathCharType> NativeStringType;
	/ DefGetter(const StringType&, NativeString, pathname)
		-> DefGetter(NativeStringType, NativeString, Text::MBCSToString(pathname));
	+ DefGetter(const StringType&, String, pathname);
	- DefGetter(StringType, GeneralString, pathname);
	/ \impl @@ \mf const ValueType*, c_str() const;
	/ \i \ctor Path(const String&) -> Path(const NativeStringType&);
	+ \i \ctor Path(const NativePathCharType*);
	/ \impl @@ \mf Path& operator/=(const Path&);
	/ \impl @@ \mf Path GetRootName() const;
	+ \s \m \c Path Now;
/ std::string StringToMBCS(const String&, const CSID& = CS_Local)
	-> std::string StringToMBCS(const stdex::ustring&, const CSID& = CS_Local) @@ \u YString;
/ @@ \u Shells;
	/ \impl @@ \mf void ShlExplorer::TFrmFileListSelecter::fb_Selected(const MIndexEventArgs&);

r12
/= test 3;

r13:
/ @@ \cl Path @@ \ns IO @@ \u YFileSystem:
	/ typedef std::basic_string<NativePathCharType> NativeStringType => ..;
	/ + public \inh stdex::ustring;
	- private \m StringType pathname;
	- DefPredicate(Empty, pathname.empty());
	- DefGetter(const StringType&, String, pathname);
	/ DefGetter(NativeStringType, NativeString, Text::StringToMBCS(pathname))
		-> DefGetter(NativeStringType, NativeString, Text::StringToMBCS(*this));
	- \i \mf void clear();
	- \i \mf void swap() ythrow();
	- \i copy \ctor;
	/ ^ stdex::ustring @@ \impl @@ \i \ctor;
	- \i \mf Path& operator=(const Path&);
	- \i \mf template<class _tString> Path& Path::operator=(const _tString&);
	/ \impl @@ \mf Path& operator/=(const Path&);
	/ \impl @@ \mf Path GetRootName() const;
	/ \impl @@ \mf Path GetParentPath() const;
	/ \impl @@ \mf Path GetFilename() const;
	/ \impl @@ \mf Path& ReplaceExtension(const Path&);
	/ \impl @@ \mf operator++() & operator--() & operator*() @@ \cl iterator;
	/ \impl @@ \mf Path GetRelativePath() const;
	/ \impl @@ \i \mf bool HasRootPath() const;
	/ \impl @@ \i \mf bool HasRelativePath() const;
	/ \impl @@ \i \mf bool HasParentPath() const;
	/ \impl @@ \i \mf bool HasFilename() const;
	/ \impl @@ \i \mf bool HasRootDirectory() const;
	/ \impl @@ \i \mf bool HasRootName() const;
	/ \impl @@ \i \mf bool HasStem() const;
	/ \impl @@ \i \mf bool HasExtension() const;
	- \mf const ValueType* c_str() const;
/ @@ \u Shells:
	/ \impl @@ void ShlExplorer::TFrmFileListSelecter::fb_Selected(const MIndexEventArgs&);

r14:
/ @@ \u YTextManager:
	* \tr \impl @@ \ctor @@ \cl TextFileBuffer;
	/ @@ \cl TextBuffer:
		/ \m const SizeType mlen -> const SizeType capacity;
		/ \m IndexType len -> SizeType len;
		/ DefGetter(IndexType, MaxLength, mlen)
			-> DefGetter(SizeType, Capacity, capacity);
		/ DefGetter(IndexType, Length, len)
			-> DefGetter(SizeType, Length, len);
		/ \ret \ty @@ \mf IndexType GetPrevChar(IndexType, uchar_t) -> SizeType;
		/ \ret \ty @@ \mf IndexType GetNextChar(IndexType, uchar_t) -> SizeType;
		/ \mf IndexType GetPrevNewline(IndexType)
			-> GetPrevNewline(SizeType);
		/ \mf IndexType GetNextNewline(IndexType)
			-> GetPrevNewline(SizeType);
		/ \mf uchar_t& at(IndexType) ythrow(std::out_of_range)
			-> uchar_t& at(SizeType) ythrow(std::out_of_range);
		/ \ret \ty @@ \mf IndexType Load(YTextFile&) -> SizeType;
		/ IndexType Load(YTextFile&, IndexType) -> SizeType Load(YTextFile&, SizeType);
		/ IndexType LoadN(YTextFile&, IndexType) -> SizeType Load(YTextFile&, SizeType);
		/ \mf bool Output(uchar_t*, IndexType, IndexType) const
			-> bool Output(uchar_t*, SizeType, SizeType) const;
		/= \tr \impl @@ \mf bool Output(uchar_t*, SizeType, SizeType) const;
		/ \ex \ctor TextBuffer(IndexType)
			-> TextBuffer(SizeType);
		/ \mf uchar_t& operator[](IndexType) ythrow()
			-> uchar_t& operator[](SizeType) ythrow();
		/ DefGetter(SizeType, SizeOfBuffer, sizeof(uchar_t) * mlen)
		-> DefGetter(SizeType, SizeOfBuffer, sizeof(uchar_t) * capacity);
		/ \tr \impl @@ \i \mf bool Load(const uchar_t*);
		/ \tr \impl @@ \i \mf SizeType Load(YTextFile&);
	/ \a BlockIndexType -> BlockSizeType;
	/ \a Index => SizeType'

r15:
/ @@ \cl TextBuffer @@ \u YTextManager:
	/= \tr \impl @@ \mf bool Load(const uchar_t*, SizeType);
	/ \impl @@ \mf SizeType Load(YTextFile&, SizeType);

r16:
/ \i \e \ctor YConsole(YScreen& = *pDefaultScreen) @@ \cl YConsole @@ \u YComponent
	-> YConsole(YScreen& = *pDefaultScreen, bool = true, Drawing::PixelType = Drawing::Color::White, Drawing::PixelType = Drawing::Color::Black);

r17:
/ @@ \cl TextFileBuffer::HText @@ \u YTextManager:
	+ \mf DefGetter(TextFileBuffer*, BufferPtr, pBuffer);
	+ \mf DefGetter(BlockSizeType, BlockN, blk);
	+ \mf DefGetter(SizeType, IndexN, idx);

r18-r19:
/= test 4;

r20-r21:
/ @@ \cl TextFileBuffer::HText @@ \u YTextManager:
	* \impl \mf @@ HText& operator++() ythrow();
	* \impl \mf @@ HText& operator--() ythrow();

r22-r25:
/= test 5;

r26-r29:
* \impl @@ \mf bool MDualScreenReader::ScreenDown() @@ \u DSReader;

r30:
/^ Microsoft Visual 2010;

$DOING:
r30


/ ...



$NEXT:
/ fully \impl \u DSReader;
+ \impl loading pictures;

$TODO:

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

