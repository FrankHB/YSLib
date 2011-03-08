//v0.3017; *Build 194 r42;
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
\conf ::= configuration
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
\mg ::= merged
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
\proj ::= projects
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
\scm ::= static const member
\sf ::= non-member static functions
\sif ::= non-member inline static functions
\simp ::= simplified
\sm ::= static member
\smf ::= static member functions
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
\u YShell
{
	\cl YShell;
}
\u YApplication
{
	\cl YLog;
	\cl YApplication;
}
\u YWidget
{
	\in IWidget;
	\cl Visual;
	\cl Widget;
	\cl YWidget;
}
\u YLabel
{
	\cl YLabel;
}
\u YUIContainer
{
	\in IUIBox;
	\in IUIContainer;
	\cl YUIContainer;
}
\u YUIContainerEx
{
	\cl AUIBoxControl;
}
\u YControl
{
	\in IControl;
	\cl Control;
	\cl YControl;
}
\u YWindow
{
	\in IWindow;
	\cl MWindow;
	\cl AWindow;
	\cl YFrameWindow;
}
\u YForm
{
	\cl YForm;
}
\u YGUI
{
	\cl YGUIShell;
}
\u Button
{
	\cl AButton;
	\cl YThumb;
	\cl YButton;
}
\u Scroll
{
	\cl ATrack;
	\cl YHorizontalTrack;
	\cl YVerticalTrack;
	\cl MScrollBar;
	\cl AScrollBar;
	\cl YHorizontalScrollBar;
	\cl YVerticalScrollBar;
}
\u ListBox
{
	\cl YSimpleListBox;
	\cl YListBox;
	\cl YFileBox;
}


$DONE:
r1:
/ @@ \st TFrmFileListSelecter @@ \cl ShlExplorer @@ \u Shells:
	- \m YHorizontalScrollBar sbTestH;
	- \m YVerticalScrollBar sbTestV;
	/ \impl @@ \ctor;

r2:
* \tr \impl @@ \ctor @@ \st TFrmFileListSelecter @@ \cl ShlExplorer
	@@ \impl \u Shells;

r3:
/ @@ \cl YApplication @@ \u YApplication:
	- \m YLog& Log;
/ \g \o YLog DefaultLog @@ \h YShellDefinition & \impl \u YGlobal
	-> \m YLog Log @@ \cl YApplication @@ \u YApplication;

r4:
/ @@ \u YGlobal:
	+ \cl PlatformResource;
	+= \inc \h YShellDefinition @@ \h;
	+ \inc \h YCoreUtilities @@ \h;
	/ extern \g \o (GHHandle<YScreen> hScreenUp & GHHandle<YScreen> hScreenDown
		& GHHandle<YDesktop> hDesktopUp & GHHandle<YDesktop> hDesktopDown
		-> !extern \m @@ \cl PlatformResource;
	/ \impl @@ \f (YInit & YDestroy) @@ \un \ns;
	/ \tr \init @@ \g \o YApplication& theApp;
	/ \tr \impl @@ \f (InitConsole & InitAllScreens);
/ @@ \u YApplication:
	+ \pre \decl \cl PlatformResource @@ \h;
	/ @@ \cl YApplication:
		+ \m PlatformResource* Resource;
		+ \mf PlatformResource& GetPlatformResource() ythrow();
		/ \impl @@ \dtor;
		- \m GHHandle<YScreen>& hDefaultScreen;
		- \m GHHandle<YDesktop>& hDefaultDesktop;
		/ \ctor -> YApplication()
			~ YApplication(GHHandle<YScreen>&, GHHandle<YDesktop>&);
		/ private \smf YApplication* GetInstancePtr(GHHandle<YScreen>&,
			GHHandle<YDesktop>&) -> YApplication* GetInstancePtr() ythrow();
		/ \i \smf YApplication& GetApp(GHHandle<YScreen>&,
			GHHandle<YDesktop>&) -> !\i YApplication& GetInstance() ythrow();
/ @@ \h YComponent:
	+ \inc \h "../Helper/yglobal.h";
	/ \tr \param.de @@ \exp \ctor @@ YConsole;
/ \tr \impl @@ \i \f (4 'OnTouch*' & 3 'OnKey*') @@ \h Shell_DS;
/ \tr \impl @@ \impl \u Shell_DS;
/ \tr \impl @@ \cl MDualScreenReader @@ \impl \u DSReader;
/ \tr \impl @@ \impl \u GBKEX;

r5:
/= test 1 ^ \conf release;

r6:
/ \a PlatformResource => PlatformResources;
/ extern const SDST SCRW, SCRH @@ \h YShellDefinition
	>> \sm @@ \cl PlatformResources @@ \u YGlobal;
/ @@ \cl PlatformResources @@ \u YGlobal:
	/ \m SCRW => MainScreenWidth;
	/ \m SCRH => MainScreenHeight;
+ \inc \h YGlobal @@ (\h YResource & \impl \u YObject);
+ \inc \h YShell @@ \impl \u YObject;

r7:
/= \a PlatformResource => Global;

r8:
/ (\cl YGUIShell @@ \ns Shells & \decl using Shells::YGUIShell)
	>> \u YGUI ~ \u YComponent;
/ @@ \h YGUI:
	+ \inc \h YShell;
	- \inc \h "ycomp.h";

r9:
/ !\m \f IWidget* GetCursorWidgetPtr(GHHandle<YGUIShell>, YDesktop&,
	const Point&) @@ \ns Components::Controls @@ \u YGUI -> \c \mf IWidget*
	GetCursorWidgetPtr(YDesktop&, const Point&)
	@@ \cl YGUIShell @@ \ns Shells;

r10:
/ @@ \u YGUI:
	/ \a extern \o & \f @@ \ns InputStatus >> \cl YGUIShell;
	- \ns InputStatus;
	/ \impl @@ \ctor @@ \cl YGUIShell;
	/ \tr \impl @@ \un \ns;
	+ \inc \h YShellHelper @@ \impl \u;
	/ \impl @@ \f ResetGUIStatus;
/ @@ \impl \u YControl:
	+ \inc \h YShellHelper;
	/ \impl @@ \f (OnKeyHeld & OnTouchHeld & OnTouchMove
		& OnTouchMove_Dragging);
/ @@ \impl \u YGUIComponent:
	/ \impl @@ \mf OnTouchMove_Thumb_Horizontal @@ \cl YHorizontalTrack;
	/ \impl @@ \mf OnTouchMove_Thumb_Vertical @@ \cl YVerticalTrack;
	+ \inc \h YShellHelper;

r11:
/ @@ \u YGUI:
	/ \a (\o & \en & \f) @@ \un \ns >> \cl YGUIShell as private \m;
	/ @@ \cl YGUIShell:
		\impl @@ \ctor @@ \cl YGUIShell;
		+ prefix 'ExOp_' @@ \exc NoOp @@ \m @@ \en ExOp @@ \cl YGUIShell;
		/ \mf void ResponseKeyBase(YDesktop&, HKeyCallback)
			-> void ResponseKeyBase(YDesktop&, KeyEventArgs&,
			bool(YGUIShell::*)(IVisualControl&, KeyEventArgs&));
		/ \mf void ResponseTouchBase(IUIBox&, HTouchCallback)
			-> void ResponseTouchBase(IUIBox&, TouchEventArgs&,
			bool(YGUIShell::*)(IVisualControl&, TouchEventArgs&));
	\f (ResetGUIStates & 3 \f 'ResponseKey*' & 3 \f 'ResponseTouch*')
		@@ \ns Components::Controls >> \cl YGUIShell;
	- typedef FKeyCallback* PFKeyCallback;
	- typedef FTouchCallback* PFTouchCallback;
	- typedef @@ FKeyCallback;
	- typedef @@ FTouchCallback;
	- typedef @@ AHEventCallback;
	- typedef @@ AHKeyCallback;
	- typedef @@ AHTouchCallback;
	- \st HKeyCallback;
	- \st HTouchCallback;
/ \tr \decl & \impl @@ \u Shell_DS;
+ \inc \h "yshell.h" @@ \impl \u YResource;

r12:
* \impl @@ \dtor @@ \cl VisualControl;

r13:
/ \a EFontStyle => FontStyle;

r14-r16:
/ \a IControl => IControlableComponent;
/ \a YControl => YControlableComponent;
/ \a Control => ControlableComponent;
/ \a IVisualControl => IControl;
/ \a VisualControl => Control;
/ \a YVisualControl => YControl;
/ \a GetTopVisualControlPtr => GetTopControlPtr;
/ \a VisualControlLocation => ControlLocation;
/ \a LastVisualControlLocation => LastControlLocation;

r17:
/= test 2 ^ \conf release;

r18:
/ @@ \u YControl:
	/ \a \m >> \in IControl ~ \in IControlableComponent;
	- \cl YControlableComponent;
	/ \a \m >> \cl Control ~ \cl ControlableComponent;
	/ \impl @@ \ctor @@ \cl Control;
	- \vt \inh \in IControlableComponent @@ \in IControl;
	/ \a \param \tp IControlableComponent -> IControl;
	- \in IControlableComponent;
	- \inh \cl ControlableComponent @@ \cl Control;
- \a \decl @@ \in IControlableComponent @@ \h YComponent;

r19:
- \vt @@ \inh GIFocusRequester<GMFocusResponser> @@ \in IControl;

r20:
* \tr \impl @@ \ctor @@ \cl Control;

r21:
- \vt @@ \a \inh \exc \in IWidget @@ \in (IWindow & IUIContainer);
+ \vt @@ \a \inh \in GIFocusRequester<GMFocusResponser>;
+ \vt @@ \a \inh \in IControl;

r22:
/ @@ \h YFocus:
	- \param.de @@ \in \t GIFocusRequester;
	/ \in \t GIFocusRequester -> !\t \in IFocusRequester;
	/ \decl @@ AFocusRequester;
/ \tr \decl @@ \h YControl;

r23:
/ @@ \cl AFocusRequester @@ \h YFocus:
	- \vt \inh \in IFocusRequest;
	- \amf bool IsFocused() const;
	- \mf bool IsFocusOfContainer(GMFocusResponser<AFocusRequester>&) const;
	- \in IFocusRequester;
+ \amf bool IsFocused() const @@ \in IControl @@ \h YControl;
- \vt @@ \a \inh \in IFocusRequester;

r24:
/ \ac @@ \inh AFocusRequester @@ \cl Control ~ public -> protected;
/ \ac @@ \inh MSimpleFocusResponser @@ \cl AUIBoxControl ~ public -> protected;

r25:
/ @@ \u YApplication:
	/ @@ \cl YApplication:
		/ \ac @@ \m YFontCache* pFontCache -> private ~ public;
		+ \mf YFontCache& GetFontCache() const ythrow(LoggedEvent);
		+ \mf void ResetFontCache(CPATH) ythrow(LoggedEvent);
	+ \h "../Adaptor/yfont.h" @@ \impl \u;
/ \tr \decl @@ \param.de @@ \ctor @@ \cl MDualScreenReader @@ \h DSReader;
/ \impl @@ \mf void ShlSetting::TFormC::btnC_Click(TouchEventArgs&)
	@@ \impl \u Shells;
/ @@ \u YFont:
	/ \f const Typeface* GetDefaultTypefacePtr() ythrow() -> const Typeface&
		FetchDefaultTypeface() ythrow(LoggedEvent);
	/ \f const FontFamily& GetDefaultFontFamily() ythrow() -> const FontFamily&
		FetchDefaultFontFamily() ythrow(LoggedEvent);
	/ \inc \h (YString -> YCoreUtilites & YObject & YException);
	/ \mf const Typeface* YFontCache::GetDefaultTypefacePtr() const
		-> const Typeface* YFontCache::GetDefaultTypefacePtr() const
		ythrow(LoggedEvent);
- \inc <cstring> @@ \h YCoreUtilities;
/ @@ \u YInitialization:
	/ \tr \impl @@ \f InitializeSystemFontCache;
	/ \f DestroySystemFontCache -> \mf YApplication::DestroyFontCache;
	/ \tr \impl @@ \f InitializeSystemFontCache;
	/ \impl @@ \f void CheckSystemFontCache();
/ \tr \impl \f @@ void YDestroy() @@ \un \ns @@ \impl \u YGlobal;

r26-r27:
/= test 3;

r28:
* \ac @@ \inh AFocusRequester @@ \cl Control:
	/ protected -> public;

r29-r31:
/= test 4;

r32-r40:
/ test 5:
	* \impl @@ \mf IControl* YGUIShell::GetTouchedVisualControlPtr(IUIBox&,
		Point&);

r41:
* \impl @@ \mf ClearFocusingPtr @@ \cl (AUIBoxControl & AWindow);

r42:
/= test 6 ^ \conf release;


$DOING:

relative process:
2011-03-08:
-23.9d;

/ ...


$NEXT_TODO:

b195-b270:
/ fully \impl \u DSReader;
	* moved text after setting lnGap;
* non-ASCII character path error in FAT16;
/ non-ordinary operator usage in \clt GSequenceViewer @@ \h YComponent;
/ impl 'real' RTC;

b271-b648:
/ GDI brushes;
/ text alignment;
+ \impl pictures loading;
/ improve efficiency @@ \ft polymorphic_crosscast @@ \h YCast;
+ data config;
+ correct DMA (copy & fill);
* platform-independence @@ alpha blending:
	+ \impl general Blit algorithm;
/ user-defined bitmap buffer @@ \cl YDesktop;
+ (compressing & decompressing) @@ gfx copying;


$LOW_PRIOR_TODO:
r325-r768:
+ \impl styles @@ widgets;
+ general component operations:
	+ serialization;
	+ designer;
+ database interface;


$NOTHING_TODO:
* fatal \err @@ b16x:
[
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
	-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 02055838
	ftc_snode_weight
	ftcsbits.c:271
]
* fatal \err @@ since b177 when opening closed lid @@ real DS:
[
b185:
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 02037F04
guruMeditationDump
gurumeditation.c:229
b186 r17[realease]:
pc: 09529334, addr: 09529334;
b187 r18[debug]:
pc: 020227C8, addr: 0380FDA4;
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 020227C8
guruMeditationDump
gurumeditation.c:254
]


$LAST_SUCCESSFULLY_FIXED:

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

