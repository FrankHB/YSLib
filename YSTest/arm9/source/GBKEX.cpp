// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-10-10 19:10 + 08:00;
// Version = 0.2718; *Build 161 r24;


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
\cp ::= copied
\ctor ::= constructors
\cv ::= const & volatile
\d ::= derived
\de ::= default
\def ::= definitions
\decl ::= declations
\dir ::= directories
\dtor ::= destructors
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
\lib ::= library
\m ::= members
\mac ::= macros
\mem ::= memory
\mf ::= member functions
\mo ::= member objects
\n ::= names
\ns ::= namespaces
\op ::= operators
\or ::= overridden
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
\un ::= unnamed
\v ::= volatile
\vt ::= virtual
\val ::= values

$using:
\u YWidget
{
	\cl MWidget;
	\cl YLabel;
}
\u YControl
{
	\cl MVisualControl;
	\cl YListBox;
	\cl YFileBox;
	\cl AButton;
}
\u YGUIComponent
{
	\cl YButton;
	\cl YListBox;
	\cl YFileBox;
}
\u YWindow
{
	\cl MWindow;
	\cl AWindow;
	\cl YFrameWindow;
}

$DONE:
r1:
+ \h "Adapter/config.h";
+ \h "Adapter/base.h";
/ \impl \h "Adapter/adapter.h":
	-= \inc <cstdio>;
	^ "config.h" & "base.h";
/ \impl \h "Core/ysdef.h"
/ \a YAdapter => YAdaptor;
/ \a Adapter => Adaptor;
/ \dir "Adapter" => "Adaptor";
/ \h "Adaptor/adapter.h" => "Adaptor/adaptor.h";
/ \mac INCLUDED_YADAPTER_H_ => INCLUDED_YADAPTOR_H_;

r2:
+ \h "Adaptor/cont.h" ^ yasli::vector @@ \lib Loki;
/ \impl;
/ \a std::vector -> vector;
/ \a std::list => list;
/ \a std::map => map;
/ \a std::stack => stack;
/ \a std::queue => queue;
/ \a std::basic_string => basic_string;
/ \a std::string => string;
/ \a std::set => set;
/ \a std::priority_queue => priority_queue;

r3:
/ \cl std::ustring @@ \u YString >> YSLib::ustring @@ \u Adaptor::Container;

r4:
/ @@ \h Adaptor::Container:
	* \inc Adaptor::Base -> Adaptor::YAdaptor;
	+ \inc <loki/flex/flex_string.h>;
	/ using std::string -> typedef flex_string<char> string;
	/ typedef std::basic_string<uchar_t> ustring -> typedef flex_string<uchar_t> ustring;
	+ \t \cl string_template<typename _tChar>;
/ @@ \u YFileSystem:
	/ typedef basic_string<NativePathCharType> NativeStringType
		-> typedef string_template<NativePathCharType>::basic_string NativeStringType;
	/ @@ \cl YPath:
		/ typedef basic_string<ValueType> StringType
			-> typedef string_template<ValueType>::basic_string StringType;

r5-r6:
/ \a string_template => SStringTemplate;
+ \mac YSL_USE_YASLI_VECTOR & YSL_USE_FLEX_STRING @@ \h Adaptor::Config;
^ \mac YSL_USE_YASLI_VECTOR & YSL_USE_FLEX_STRING @@ \h Adaptor::Container;
/ typedef flex_string<uchar_t> ustring -> typedef SStringTemplate<char>::basic_string string;
/ typedef flex_string<uchar_t> ustring -> typedef SStringTemplate<uchar_t>::basic_string ustring;

r7:
/ @@ \h Adaptor::YReference:
	/ \n YSP_OP_Simple -> Design::Policies::Operations::SmartPtr_Simple;
	/ \n YSP_OP_RefCounted -> Design::Policies::Operations::SmartPtr_RefCounted;
	/ \mac #define YHandleOP SmartPtr_Simple -> #define YHandleOP Design::Policies::Operations::SmartPtr_Simple;

r8-r11:
/ \impl @@ \h Adaptor::YReference;
/ \impl @@ \h Adaptor::Base;
/ \impl @@ \h Core::YSDefinition;
/ \impl @@ \h Adaptor::Reference;

r12-r13:
/ \impl @@ \h Adaptor::Container:
	^ policy \n CowStringOpt;
* \impl @@ \h YAdaptor::YASLIVectorStoragePolicy;

r14:
/= \impl @@ \h Adaptor::Config:
	+ \mac YSL_COPY_ON_WRITE;
	+ \rem \mac YSL_MULTITHREAD;

r15-r22:
/= test 1;

r23:
+ \mac YSL_OPT_SMALL_STRING_LENGTH @@ \h Adaptor::Config;
/ \impl @@ \h Adaptor::Container:
	^ \mac YSL_OPT_SMALL_STRING_LENGTH;
	^ VectorStringStorage -> AllocatorStringStorage;

r24:
/ \impl @@ Adaptor::Config:
	^ yasli::vector -> std::vector;
	^ flex_string -> std::basic_string;


$DOING:

/ ...

$NEXT:
b161-b190:
/ fully \impl \u DSReader;
	* moving text after setting lnGap;
* non-ASCII character filename error in FAT16;
+ \impl loading pictures;
+ \impl style on widgets;

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

