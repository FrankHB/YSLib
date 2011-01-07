//v 0.2937; *Build 181 r13;
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
@@ ::= in / belonged to
\a ::= all
\ab ::= abstract
\ac ::= access
\amb ::= ambiguities
\amf ::= abstract/pure virtual member function
\as ::= assertions
\bg ::= background
\c ::= const
\cb ::= catch blocks
\cl ::= classes
\clt ::= class templates
\cp ::= copied
\ctor ::= constructors
\ctort ::= constuctor templates
\cv ::= const & volatile
\d ::= derived
\de ::= default
\def ::= definitions
\dep ::= dependencies
\decl ::= declations
\dir ::= directories
\dtor ::= destructors
\e ::= exceptions
\em ::= empty
\en ::= enums
\eh ::= exception handling
\err ::= errors
\es ::= exception specifications
\evt ::= events
\evh ::= event handling
\ex ::= extra
\exc ::= excluded
\ext ::= extended
\exp ::= explicit
\f ::= functions
\fn ::= \f
\ft ::= function templates
\fw ::= forward
\g ::= global
\gs ::= global scpoe
\h ::= headers
\i ::= inline
\impl ::= implementations
\in ::= interfaces
\init ::= initializations
\inc ::= included
\inh ::= inherited
\inv ::= invoke
\k ::= keywords
\lib ::= library
\ln ::= lines
\m ::= members
\mac ::= macros
\mem ::= memory
\mf ::= member functions
\mft ::= member function templates
\mo ::= member objects
\mt ::= member templates
\n ::= names
\ns ::= namespaces
\o ::= objects
\op ::= operators
\or ::= overridden
\param ::= parameters
\param.de ::= default parameters
\pre ::= prepared
\pt ::= points
\ptr ::= pointers
\q ::= qualifiers
\rem ::= remarked
\ren ::= renamed
\ref ::= references
\refact ::= refactorings
\res ::= resources
\ret ::= return
\s ::= static
\sf ::= non-member static functions
\sif ::= non-member inline static functions
\simp ::= simplified
\smf ::= static member functions
\sm ::= static member
\scm ::= static const member
\snm ::= static non-member
\spec ::= specifications
\st ::= structs
\str ::= strings
\stt ::= struct templates
\t ::= templates
\tb ::= try blocks
\tg ::= targets
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
	\in IWidget;
	\in IUIBox;
	\cl Widget;
	\cl YUIContainer;
	\cl YLabel;
}
\u YControl
{
	\in IControl;
	\in IVisualControl;
	\cl MVisualControl;
	\cl YControl;
	\cl AVisualControl;
	\cl YVisualControl;
}
\u YGUIComponent
{
	\cl AButton;
	\cl YThumb;
	\cl YButton;
	\cl ATrack;
	\cl YHorizontalTrack;
	\cl YVerticalTrack;
	\cl MScrollBar;
	\cl AScrollBar;
	\cl YHorizontalScrollBar;
	\cl YVerticalScrollBar;
	\cl YListBox;
	\cl YFileBox;
}
\u YWindow
{
	\in IWindow;
	\cl MWindow;
	\cl AWindow;
	\cl YFrameWindow;
}

$DONE:
r1:
/ \rem \mac YSL_FAST_BLIT @@ \impl \u YGDI;

r2:
/ !^ DMA @@ \f (mmbset & mmbcpy) @@ \impl \u YCommon;

r3:
/ fully recompiled ^ updated libnds 1.4.9:
	/ \a vramSetMainBanks => vramSetPrimaryBanks;

r4:
/ @@ \cl Message @@ \u YShellMessage:
	+ \mf void Swap(Message&);
	+ copy \ctor Message(const Message&);
	+ copy assignment \mf Message& \op=(const Message&);
	/ \mf \op== -> friend !\m \f op==;
	- \mf \op!=;

r5:
+ using ystdex::errno_t @@ \ns YSLib @@ \h YAdaptor;
/= \simp \a ystdex::errno_t -> errno_t @@ \ns YSLib;

r6:
+ \i @@ \f bool operator!=(const KeysInfo&, const KeysInfo&) @@ \un \ns
	@@ \impl \u YGlobal;

r7:
/ @@ \u YShellMessage:
	/ \f void Merge(YMessageQueue&, vector<Message>&)
		-> void Merge(YMessageQueue&, list<Message>&);
	/ \tr \impl @@ \f void Merge(YMessageQueue&, YMessageQueue&);
/ \impl @@ \f PeekMessage @@ \impl \u YShell;

r8:
/ @@ \u YShell:
	/ \f int PeekMessage(Message& msg, GHHandle<YShell> hShl = NULL,
		Messaging::ID wMsgFilterMin = 0, Messaging::ID wMsgFilterMax = 0,
		u32 wRemoveMsg = PM_NOREMOVE)
		-> int PeekMessage(Message& msg, GHHandle<YShell> hShl
		= GetCurrentShellHandle(), bool bRemoveMsg = false);
	/ \f int GetMessage(Message& msg, GHHandle<YShell> hShl = NULL,
		Messaging::ID wMsgFilterMin = 0, Messaging::ID wMsgFilterMax = 0)
		-> int GetMessage(Message& msg, GHHandle<YShell> hShl
		= GetCurrentShellHandle());
	- \mac PM_NOREMOVE;
	- \mac PM_REMOVE;
	+ \f GHHandle<YShell> GetCurrentShellHandle() ythrow();

r9:
/ \cl YMessageQueue @@ \u YShellMessage:
	/ \mf void GetMessage(Message&) -> Message GetMessage() ythrow();
	/ \m size_type => SizeType;
	/ \mf bool empty() const -> bool IsEmpty() const ythrow();
	/ \mf SizeType size() const -> SizeType() const ythrow();
/ \tr \impl @@ \f (PeekMessage & GetMessage) @@ \u YShell;
/ \tr \impl @@ \f void WaitForGUIInput() @@ \un \ns @@ \impl \u YGlobal;

r10:
/ \impl @@ \f PeekMessage @@ \impl \u YShell;

r11:
/ \impl @@ \mf YApplication::SetShellHandle(GHHandle<YShell>)
	@@ \impl \u YApplication;
/ \simp \impl @@ \f void PostQuitMessage(int, Priority) @@ \impl \u YShell;
/ \impl @@ \mf void ShlGUI::SendDrawingMessage() @@ \impl \u ShlDS;
/ \impl @@ \i \f NowShellInsertDropMessage(Messaging::Priority = 0x80)
	@@ \h ShlDS;
/ \impl @@ \f void WaitForGUIInput() @@ \un \ns @@ \impl \u YGlobal;
/ \impl @@ \i \f void SetShellTo(GHHandle<YShell>, Messaging::Priority = 0x80)
	@@ \h YShellHelper;

r12:
/ \impl @@ \f (PeekMessage & PostQuitMessage) @@ \impl \u YShell;

r13:
* GUI states invalid when changing shells;
	+ \f void ResetGUIStates() @@ \ns Components::Controls @@ \u YGUI;
	/ \impl @@ \mf int ShlGUI::OnDeactivated(const Message&) @@ \impl \u ShlDS;


$DOING:

relative process:
2011-01-07:
-21.1d;

/ ...


$NEXT:

b182-b215:
* screen output polluted @@ real machine;
* fatal \err @@ since b178 when closing lid:
[
b180 r56:
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 020BBC5C
YSLib::Messaging::Message::operator=(YSLib::Messaging::Message const&)
ysmsg.h:55

b180 r57:
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 020A0BB4
Loki::SmartPtr<YSLib::Shells::YShell, YSLib::Design::Policies::GeneralCastRefCou
nted, Loki::DisallowConversion, Loki::AssertCheck, Loki::DefaultSPStorage, Loki:
:DontPropagateConst>::operator=(Loki::SmartPtr<YSLib::Shells::YShell, YSLib::Des
ign::Policies::GeneralCastRefCounted, Loki::DisallowConversion, Loki::AssertChec
k, Loki::DefaultSPStorage, Loki::DontPropagateConst> const&)
SmartPtr.h:1217
]
* fatal \err before b170(after b158) when touching on the ListBox
	& key L being released:
[
b177
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 0201f484
GetTouchedVisualControlPtr
ygui.cpp:215
??
ygui.cpp:325

b180 r56:
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 0201E8BC
YSLib::Components::Controls::(anonymous namespace)::TryLeave(YSLib::Components::
Controls::IVisualControl&, YSLib::Components::Controls::TouchEventArgs&)
ygui.cpp:176
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 020D7D6C
__dynamic_cast
crtstuff.c:0
]
/ improve efficiency @@ \tf polymorphic_crosscast @@ \h YCast;
/ scroll bars @@ listbox;
/ fully \impl \u DSReader;
	* moved text after setting lnGap;
* non-ASCII character path error in FAT16;
/ non-ordinary operator used in \clt GSequenceViewer @@ \h YComponent;

r196-r288:
* alpha blending platform independence;
+ \impl loading pictures;
+ \impl style on widgets;
+ \impl general blit algorithm;
/ user-defined bitmap buffer @@ \cl YDesktop;
/ general component operations:
	/ serialization;
	/ designer;
/ database interface;


$NOTHING_TODO:
* fatal \err @@ b16x:
[
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
	-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 02055838
	ftc_snode_weight
	ftcsbits.c:271
]


$TODO:

Clarify the log levels.

Make "UpdateFont()" more efficienct.

More efficient %YTextRegion output:
Use in-buffer background color rendering and function %CopyToScreen()
	to inplements %YTextRegion background;
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
//---- temp code;
/*

/*	static YTimer Timer(1250);

	if(Timer.Refresh())
	{
	//	InitYSConsole();
	//	YDebugBegin();
		iprintf("time : %u ticks\n", GetTicks());
		iprintf("Message : 0x%04X;\nPrior : 0x%02X;\nObj : %d\n"
			"W : %u;\nL : %lx;\n", msg.GetMessageID(),
			msg.GetPriority(), msg.GetObjectID(), msg.GetWParam(), msg.GetLParam());
		WaitForInput();
	//	StartTicks();
	}*/
/*
	YDebugBegin();
	iprintf("time : %u ticks\n", GetTicks());
	iprintf("Message : 0x%04X;\nPrior : 0x%02X;\nObj : %d\n"
		"W : %u;\nL : %lx;\n", msg.GetMessageID(), msg.GetPriority(),
		msg.GetObjectID(), msg.GetWParam(), msg.GetLParam());
	WaitForInput();*/
/*
		InitYSConsole();
		iprintf("%d,(%d,%d)\n",msg.GetWParam(),
			msg.GetCursorLocation().X, msg.GetCursorLocation().Y);
*/
//! \brief 桌面对象模块。
class MDesktopObject
{
protected:
	GHHandle<YDesktop> hDesktop; //!< 桌面句柄。

public:
	//! \brief 构造：使用指定桌面句柄。
	explicit
	MDesktopObject(GHHandle<YDesktop>);

protected:
	DefEmptyDtor(MDesktopObject)

public:
	//判断从属关系。
	PDefH(bool, BelongsTo, GHHandle<YDesktop> hDsk) const
		ImplRet(hDesktop == hDsk)

	DefGetter(GHHandle<YDesktop>, DesktopHandle, hDesktop)
};

inline
MDesktopObject::MDesktopObject(GHHandle<YDesktop> hDsk)
	: hDesktop(hDsk)
{}

class RefCountedImpl
{
public:
	typedef uintptr_t CountType;

private:
	CountType* pCount;

protected:
	RefCountedImpl()
		: pCount_(static_cast<CountType*>(
		SmallObject<>::operator new(sizeof(CountType))))
	{
		assert(pCount_ != NULL);
		*pCount_ = 1;
	}

	inline
	RefCountedImpl(const RefCountedImpl& rhs)
		: pCount_(rhs.pCount_)
	{}

	inline CountType
	Clone()
	{
		return ++*pCount;
	}

	bool
	Release()
	{
		if(--*pCount_ == 0)
		{
			SmallObject<>::operator delete(pCount_, sizeof(CountType));
			pCount_ = NULL;
			return true;
		}
		return false;
	}

	void
	Merge(RefCountedImpl& rhs)
	{
		CountType count(*pCount_);

		SmallObject<>::operator delete(pCount_, sizeof(CountType));
		pCount_ = rhs.pCount_;
		*pCount_ += count;
		return true;
	}
};
*/

