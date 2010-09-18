// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-9-18;
// Version = 0.2617; *Build 150 r22;


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
r1-r11:
* fatal error:
	/ make default message queue dynamic at runtime:
		r1:
		/ @@ \h YShellDefinition @@ \u YGlobal:
			- YMessageQueue& DefaultMQ;
			- YMessageQueue& DefaultMQ_Backup;
		/ \i \f void InsertMessage(const Message&)
			& \i \f void InsertMessage(const HSHL&, const MSGID&, const MSGPRIORITY&, const WPARAM& = 0, const LPARAM& = 0, const SPoint& = SPoint::Zero)
			@@ \ns YSLib::Shells @@ \h YShellMessage ->
			\i \f void InsertMessage(const Message&)
			& \i \f void InsertMessage(const HSHL&, const Shells::MSGID&, const Shells::MSGPRIORITY&, const WPARAM& = 0, const LPARAM& = 0, const SPoint& = SPoint::Zero)
			@@ \ns YSLib @@ \h YApplication;
		- using Shells::InsertMessage @@ \h YShellMessage;
		- \i \f inline YMessageQueue::size_type ClearDefaultMessageQueue();
		/ @@ \cl YShell @@ \u YShell:
			- \i @@ \f LRES OnActivated(const Message&);
			- \i @@ \f LRES OnDeactivated(const Message&);
			/ \impl @@ void PostQuitMessage(int);
			/ \impl @@ IRES PeekMessage(Message&, HSHL, MSGID, MSGID, u32);
			/ \impl @@ IRES GetMessage(Message&, HSHL, MSGID, MSGID);
			/ \impl @@ ERRNO BackupMessage(const Message& msg);
			/ \impl @@ ERRNO RecoverMessageQueue();
		/ \impl @@ \f void WaitForGUIInput() @@ \ns @@ \u YGlobal;
		r2-r3:
		/ \u YApplication:
			+ \m private YMessageQueue* pMessageQueue;
			+ \m private YMessageQueue* pMessageQueueBackup;
			- \m public YMessageQueue DefaultMQ;
			- \m public YMessageQueue DefaultMQ_Backup;
			+ public \mf Initialize();
			+ public \mf Destroy() ythrow();
			/ public YFontCache* FontCache => pFontCache;
			+ public \mf YMessageQueue& GetDefaultMessageQueue() ythrow(Exceptions::LoggedEvent);
			+ public \mf YMessageQueue& GetBackupMessageQueue() ythrow(Exceptions::LoggedEvent);
		/ \impl @@ \f:
			/ void Def::Destroy(YObject&, const MEventArgs&) @@ \u YGlobal;
			/ void YInit() @@ unnamed \ns @@ \u YGlobal;
			/ \i void InsertMessage(const Message&) @@ \h YApplication;
			/ \i void InsertMessage(const HSHL&, const Shells::MSGID&, const Shells::MSGPRIORITY&, const WPARAM& = 0, const LPARAM& = 0, const SPoint& = SPoint::Zero) @@ \h YApplication;
			/ @@ \u YShell:
				/ IRES PeekMessage(Message&, HSHL, MSGID, MSGID, u32);
				/ IRES GetMessage(Message&, HSHL, MSGID, MSGID);
				/ ERRNO BackupMessage(const Message& msg);
				/ ERRNO RecoverMessageQueue();
			/ void WaitForGUIInput() @@ \ns @@ \u YGlobal;
		/ @@ \u YApplication:
			+ using namespace Exception @@ \impl;
		/ YFontCache*& pDefaultFontCache(theApp.FontCache)
			-> YFontCache*& pDefaultFontCache(theApp.pFontCache) @@ \u YGolbal;
		r4-r7:
		/= test 1;
		r8:
		/ \impl @@ \ctor @@ \cl YShell @@ \u YShell;
		r9-r11:
		/ test 2;
			/ @@ \cl YApplication:
				* \impl;
				+ public \mf Initialize();
				+ public \mf Destroy() ythrow();
				/+ ythrow() @@ \dtor;
			/ \impl @@ \f void YSInit() @@ unnamed \ns @@ \u YGolbal;

r12-r16:
+ \de \para (YScreen& = *pDefaultScreen) @@ \exp \ctor @@ \cl YConsole(YScreen&);
/= test 3;

r17:
* @@ \cl YListBox @@ \u YControl:
	/ \ret \tp @@ \mf \eh void _m_OnClick(const MTouchEventArgs&) -> bool;
	/ \ret \tp @@ \mf \eh void _m_OnKeyPress(const MTouchEventArgs&) -> bool;
	/ \impl @@ \mf \eh void OnClick(IVisualControl&, const MTouchEventArgs&);
	/ \impl @@ \mf \eh void OnKeyPress(IVisualControl&, const MTouchEventArgs&);

r18:
/= test 4;

r19:
/ simp @@ \cl YListBox @@ \u YControl:
	* undid r17;
	/ \impl @@ \mf \eh void _m_OnClick(const MTouchEventArgs&) -> bool;
	/ \impl @@ \mf \eh void _m_OnKeyPress(const MTouchEventArgs&) -> bool;

r20-r22:
/= test 5;


DOING:

/ ...

Debug message:

cmd = %DKP_HOME%\devkitARM\bin\arm-eabi-addr2line.exe -f -C -e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 2078824

void std::__push_heap<__gnu_cxx::__normal_iterator<YSLib::Shells::Message*, std:
:vector<YSLib::Shells::Message, std::allocator<YSLib::Shells::Message> > >, int,
 YSLib::Shells::Message, YSLib::Shells::YMessageQueue::cmp>(__gnu_cxx::__normal_
iterator<YSLib::Shells::Message*, std::vector<YSLib::Shells::Message, std::alloc
ator<YSLib::Shells::Message> > >, int, int, YSLib::Shells::Message, YSLib::Shell
s::YMessageQueue::cmp)
crtstuff.c:0


NEXT:
* fatal error;
+ fully \impl \u YFileSystem;
/ fully \impl \cl ShlReader;
/ fully \impl btnTest;

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

