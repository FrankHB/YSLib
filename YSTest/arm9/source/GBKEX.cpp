// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-8-3;
// Version = 0.2612; *Build 138 r23;


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
/= DefGetter(u32, Pos, ftell() - bl) -> DefGetter(SizeType, Pos, ftell() - bl) @@ \cl YTextFile @@ \u YFile_(Text);
/ YFile::fseek(s32, int) const -> YFile::fseek(long, int) const @@ \cl YFile @@ \u YFile;
/ typedef u32 SizeType -> typedef size_t SizeType @@ \cl YFile @@ \u YFile;
/ @@ \cl YTextFile @@ \u YFile_(Text):
	/ \mf void Seek(s32, int) const -> void Seek(long, int) const;
	/ \mf u32 Read(void*, u32) -> std::size_t Read(void*, std::size_t);
	/ \mf u32 ReadS(uchar_t*, u32) const -> std::size_t ReadS(uchar_t*, std::size_t) const;
	/= \impl @@ \mf u8 CheckBOM(CSID&);
	/= \impl @@ \ctor;

r2:
/ @@ \cl YFile @@ \u YFile:
	/ \i \mf (int fread(void *p, int size, int count) -> \i \mf std::size_t fread(void* ptr, std::size_t size, std::size_t nmemb) const);
	/= \i \mf (int fseek(long offset, int origin) const => int fseek(long offset, int whence) const);
/ @@ \cl YTextFile @@ \u YFile_(Text):
	/= \mf (void Seek(long, int origin) const => void Seek(long, int whence) const);
	/ \mf (std::size_t Read(void* s, std::size_t n) -> std::size_t Read(void* s, std::size_t n) const); 

r3:
/ @@ \cl YFile @@ \u YFile:
	+ typedef std::ptrdiff_t OffType;
	/ \i \mf (int fseek(long offset, int whence) const -> int fseek(OffType, int) const);
	/ \i \mf (long ftell() const -> OffType ftell() const);
	/ \i \mf (std::size_t fread(void* ptr, std::size_t size, std::size_t nmemb) const -> SizeType fread(void* ptr, SizeType size, SizeType nmemb) const);
/ @@ \cl YTextFile @@ \u YFile_(Text):
	/ \i \mf (std::size_t Read(void* s, std::size_t n) const -> SizeType Read(void* s, SizeType n) const);
	/ \i \mf (std::size_t ReadS(uchar_t* s, std::size_t n) const -> SizeType ReadS(uchar_t* s, SizeType n) const);

r4:
/ @@ \cl YFile @@ \u YFile:
	/ DefGetter(SizeType, Length, fsize) => DefGetter(SizeType, Size, fsize);
/ @@ \cl YTextFile @@ \u YFile_(Text):
	/ private \m (u8 bl -> SizeType bl);
	/ DefGetter(u8, BOMLen, bl) => DefGetter(u8, BOMSize, bl);

r5-r6:
/ @@ \u YTextManager:
	/= @@ \cl MTextBuffer:
		/= \a SizeType => IndexType;
	/ @@ \cl MTextBlock:
		/ \i \ctor MTextBlock(IndexType, SizeType) -> MTextBlock(IndexType, size_t);
	- typedef MTextBuffer::SizeType SizeType @@ \cl MTextMap;
	/ @@ \cl MTextFileBuffer;
		- typedef ContainerType::SizeType SizeType @@ \cl TextIterator;
		/ \scm SizeType nBlockSize = 0x2000 -> size_t nBlockSize = 0x2000;
		/ \scm SizeType nBlockSize = 0x2000 -> size_t nBlockSize = MTextFileBuffer::nBlockSize @@ \cl TextIterator;
		+ typedef MTextBlock::IndexType TextIndexTpye;
		+ typedef MTextMap::IndexType BlockIndexType;
		/ MTextBlock& operator[](const IndexType&) -> MTextBlock& operator[](const BlockIndexType&);

r7:
/= @@ \u YTextManager:
	/= \a private \m nLen => nTextSize @@ \cl MTextFileBuffer;
	/= \a \mf TextLength => TextSize;
+ DefGetter(SizeType, TextSize, GetSize() - GetBOMSize()) @@ \cl YTextFile @@ \u YFile_(Text):
/ \tr @@ \impl @@ \ctor @@ \cl MTextFileBuffer @@ \u YTextManager;

r8:
/ @@ \u YTextManager:
	/ @@ \cl MTextBuffer:
		/ typedef size_t SizeType -> typedef std::size_t SizeType;
		+ typedef std::size_t SizeType;
	/ @@ \cl MTextBlock:
		/ typedef MTextBuffer::IndexType IndexType -> typedef MTextBuffer::IndexType TextIndexType;
		+ typedef u16 BlockIndexType;
		/ \ctor MTextBlock(IndexType, SizeType) -> MTextBlock(BlockIndexType, TextIndexType);;
		/ \m IndexType Index -> BlockIndexType Index;
	/ @@ \cl MTextMap:
		/ typedef u16 IndexType -> typedef MTextBlock::BlockIndexType BlockIndexType;
		/ typedef std::map<IndexType, MTextBlock*> MapType -> typedef std::map<BlockIndexType, MTextBlock*> MapType;
		/ \mf (MTextBlock* operator[](const IndexType&) -> MTextBlock* operator[](const BlockIndexType&));
		/ \mf (bool operator-=(const IndexType&) -> bool operator-=(const BlockIndexType&));
	/ @@ \cl MTextFileBuffer:
		/ typedef MTextMap::IndexType BlockIndexType -> typedef MTextMap::BlockIndexType BlockIndexType;
		/ typedef MTextBlock::IndexType TextIndexTpye -> typedef MTextMap::IndexType BlockIndexType;;
		+ typedef MTextBlock::SizeType SizeType;
	/ ^ SizeType;

r9:
/= \u YTextManager:
	/= \a TextIndexType => IndexType;
	- typedef MTextBuffer::IndexType IndexType @@ \cl MTextBlock;
	*= \a TextIndexTpye => IndexType;

r10:
/ \a ::size_t -> std::size_t;
/= @@ \u YShellHelper:
	/= \tr @@ \impl @@ \tf template<typename _pixelType> _pixelType* NewBitmapRaw(const _pixelType*, u32);
	/= \tf template<typename _pixelType> _pixelType* NewBitmapRaw(const _pixelType*, u32)
		=> template<typename _pixelType> _pixelType* NewBitmapRaw(const _pixelType*, std::size_t);

r11:
/ @@ \u YTextManager:
	/ @@ \cl MTextFileBuffer:
		+ private \m BlockIndexType nBlk;
		+ private \m IndexType nIdx;
		/ \tr @@ \impl @@ \mf operator[](const BlockIndexType&);
	+ @@ \cl MTextBuffer:
		+ uchar_t operator[](IndexType) ythrow(); 
		+ uchar_t at(IndexType) ythrow(std::out_of_range);

r12-r13:
/ + ythrow():
	/ @@ \dtor();
	/ + ythrow() @@ getters, including \mf DefGetter & \mf DefSetter;
/ + ythrow(...);

r14:
= test 1;

r15:
/ @@ \u YTextManager:
	+ @@ \cl MTextBuffer:
		+ \mf DefGetter(IndexType, MaxLength, mlen);
		+ \mf DefGetter(IndexType, Length, len);
	/ @@ \cl MTextFileBuffer::TextIterator:
		- \scm SizeType nBlockSize = MTextFileBuffer::nBlockSize;

r16:
/ @@ \u YTextManager:
	/ @@ \cl MTextBuffer:
		+ \m const BlockIndexType nBlock;
		/ @@ \cl TextIterator:
			/ \tr @@ \impl @@ \ctor;
			+ typedef ContainerType::SizeType SizeType;
			+ typedef ContainerType::IndexType IndexType;
			+ typedef ContainerType::BlockIndexType BlockIndexType;
			+ \mf IndexType GetBlockLength(BlockIndexType);
			+ \i \mf IndexType GetBlockLength();

r17:
/ @@ \cl MTextBuffer::TextIterator @@ \u YTextManager:
	/= \m BlockIndexType nBlk => blk;
	/= \m IndexType nIdx => idx;
	/ \mf const uchar_t* GetTextPtr() ythrow() -> const uchar_t* GetTextPtr() const ythrow();
	/ IndexType GetBlockLength() -> IndexType GetBlockLength() const;
	/ IndexType GetBlockLength(BlockIndexType) -> IndexType GetBlockLength(BlockIndexType) const;

r18:
/ @@ \u YTextManager:
	/ @@ \cl MTextBuffer::TextIterator:
		/ \impl @@ \mf const uchar_t* GetTextPtr() const ythrow();
		/ \impl @@ \ctor;
		/ \impl @@ \mf TextIterator& operator++() ythrow();
		/ \impl @@ \mf TextIterator& operator--() ythrow();
		/ \impl @@ \mf TextIterator operator+() ythrow();
		/ \impl @@ \mf TextIterator operator-() ythrow();
		/ \impl @@ \mf TextIterator& operator+=() ythrow();
		/ \impl @@ \mf TextIterator GetTextPtr() const ythrow();
		* \impl @@ \mf TextIterator GetBlockLength() const ythrow();
	/ @@ \cl MTextBuffer:
		/ \mf uchar_t at(IndexType) ythrow(std::out_of_range) -> uchar_t& at(IndexType) ythrow(std::out_of_range);
		/ \mf uchar_t operator[](IndexType) ythrow() -> uchar_t& operator[](IndexType) ythrow();

r19:
- @@ \cl MTextBuffer::TextIterator @@ \u YTextManager:
	- \m SizeType nPos;
	- \mf DefGetter(SizeType, Position, nPos);
	- friend \f std::ptrdiff_t operator-(TextIterator, TextIterator);
/ @@ \cl MTextRegion @@ \u YText:
	/ \t \mf template<typename _constCharIteratorType> u32 PutString(_constCharIteratorType)
		-> template<typename _constCharIteratorType> std::size_t PutLine(_constCharIteratorType);
	/ \t \mf template<typename _constCharIteratorType> u32 PutLine(_constCharIteratorType)
		-> template<typename _constCharIteratorType> std::size_t PutLine(_constCharIteratorType);
	/ \mf u32 PutLine(const MString&) -> std::size_t PutLine(const MString&);
/ \impl @@ \u DSReader;

r20:
/ \simp \impl @@ \mf MTextBuffer::TextIterator operator+() ythrow() @@ \u YTextManager;

r21:
/ @@ \u YTextManager:
	/ @@ \cl MTextBuffer::TextIterator:
		/ \ctor explicit TextIterator(MTextFileBuffer&, SizeType = 0) ythrow();
			-> explicit TextIterator(MTextFileBuffer&, BlockIndexType = 0, IndexType = 0) ythrow();
	+ @@ \cl MTextBuffer:
		+ \mf MTextBuffer::TextIterator begin() ythrow();
		+ \mf MTextBuffer::TextIterator end() ythrow();
/ \tr \impl @@ \u DSReader;

r22:
/ @@ \cl MDualScreenReader @@ \u DSReader;
	/ \mf u32 TextFill(u32) -> u32 TextFill();
	/ \tr \impl @@ \mf void TextInit();
	/ \tr \impl @@ \mf void Update();

r23:
* / \tr \impl @@ \mf void MDualScreenReader::TextFill() @@ \cl MDualScreenReader;

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

