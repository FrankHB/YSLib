//v0.2959; *Build 192 r47;
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
/ @@ \h YControl:
	/= \tr \decl @@ \st ScreenPositionEventArgs;
	/ \tr \decl @@ \st InputEventArgs:
		/ + \c @@ \s \m InputEventArgs Empty;
	/ \tr \decl @@ \st KeyEventArgs:
		/ + \c @@ \s \m KeyEventArgs Empty;
	/ \tr \decl @@ \st TouchEventArgs:
		/ + \c @@ \s \m TouchEventArgs Empty;

r2:
/ @@ \h YEvent:
	/ \clt<class _tEventSpace, class _tEvent = Event> GEventMap
		-> \clt<typename _tEventSpace> GEventMap;
	/ typedef typename _tEventSpace::EventID ID @@ \clt GEventMap;
		-> typedef _tEventSpace ID;
/ @@ \h YControl:
	/ DeclIEntry(Runtime::GEventMap<EControl>& GetEventMap() const)
		@@ \in IControl -> DeclIEntry(Runtime::GEventMap<EControl::EventID>&
		GetEventMap() const);
	/ ImplI1(IVisualControl) DefGetterBase(Runtime::GEventMap<EControl>&,
		EventMap, Control) @@ \cl AVisualControl -> ImplI1(IVisualControl)
		DefGetterBase(Runtime::GEventMap<EControl::EventID>&, EventMap,
		Control);
	/ @@ \cl Control:
		/ \m mutable Runtime::GEventMap<EControl> EventMap -> mutable
			Runtime::GEventMap<EControl::EventID> EventMap;
		/ DefGetter(Runtime::GEventMap<EControl>&, EventMap, EventMap)
			-> DefGetter(Runtime::GEventMap<EControl::EventID>&, EventMap,
			EventMap);

r3:
/ @@ \h YControl:
	/ \a EControl::EventID -> VisualEvent;
	/ \en EControl::EventID -> \en VisualEvent !@@ \st EControl;
	- \st EControl;
	/ \a EventSpace => VisualEventSpace;
/ \a "EControl::" -> "";

r4:
/ @@ \clt GEventMap @@ \h YEvent:
	+ typedef SmartPtr<ItemType> PointerType;
	+ typedef map<ID, PointerType> MapType;
	/ mutable map<ID, SmartPtr<ItemType> > m_map -> mutable MapType m_map;
	/ \tr \simp \impl @@ \mf GetEvent;
/ \simp \impl @@ \mf GetEvent;

r5:
/ \a GSEventTemplate => GSEvent;
/ \a TryEnter => TryEntering;
/ \a TryLeave => TryLeaving;
^ libnds 1.5.0;

r6:
/ @@ \clt GEventMap @@ \h YEvent:
	+ \mft<class _tEventHandler> void DoEvent(const ID& id,
		typename _tEventHandler::SenderType&,
		typename _tEventHandler::EventArgsType&) const;
	/= \tr \impl @@ \mft GetEvent;
	+ private typedef std::pair<typename MapType::iterator, bool>
		InternalPairType;
	+ private \mf InternalPairType GetSerachResult(const ID&) const;
	+ typedef std::pair<ID, PointerType> PairType;
+ \ft<VisualEvent id> \i typename Runtime::GSEvent<typename
	EventTypeMapping<id>::HandlerType>::EventType& CallEvent(IControl&,
	typename EventTypeMapping<id>::HandlerType::SenderType&, typename
	EventTypeMapping<id>::HandlerType::EventArgsType&) @@ \h YControl;

r7:
/  @@ \h YControl:
	+ \ft<VisualEvent id> \i typename Runtime::GSEvent<typename
		EventTypeMapping<id>::HandlerType>::EventType& CallEvent(
		IVisualControl&, typename EventTypeMapping<id>::HandlerType::
		EventArgsType&);
	* \a 2 \ret \tp @@ \ft CallEvent;
/ \impl @@ \f @@ \impl \u YGUI ^ \ft CallEvent;

r8:
/ @@ \h YControl:
	+ \n (Move & Resize) @@ \en VisualEventSpace;
	- event (Move & Resize) @@ \cl MVisualControl;
	/ DefDelegate(HFocusEvent, IControl, EventArgs)
		-> DefDelegate(HVisualEvent, IVisualControl, EventArgs);
	+ DefEventTypeMapping(Move, HVisualEvent);
	+ DefEventTypeMapping(Resize, HVisualEvent);
	/ \rem \def @@ HPointEvent;
	/ \rem \def @@ HSizeEvent;
	/ ImplI1(IVisualControl) (DefSetterBase(const Point&, Location, Visual)
		-> void SetLocation(const Point&));
	/ ImplI1(IVisualControl) (DefSetterBase(const Size&, Size, Visual)
		-> void SetSize(const Size&));

r9:
+ \f void RequestToTop(IWidget&) @@ \u YWidget;
/ \impl @@ \f IVisualControl* GetTouchedVisualControlPtr(IUIBox&, Point&)
	@@ \un \ns @@ \u YGUI;
- \a (\amf & \mf) RequestToTop;

r10-r11:
/ @@ \u YGUIComponent:
	+ \cl YScrollableContainer;
	/ @@ \cl YListBox ^ YScrollableContainer;

r12:
/ \inc "yguicomp.h"  @@ \h YWindow
	-> ("ycontrol.h" & "yuicont.h" & "../Core/yres.h");
/ \a YImage -> Drawing::YImage @@ \h (YWindow & YForm);
/ @@ \impl \u YLabel:
	+ \inc "ylabel.h";
	/ \tr \impl @@ \mf void MLabel::PaintText(Widget&, const Graphics&,
		const Point&);
/ @@ \h DSReader:
	/ \a TextRegion -> Drawing::TextRegion;
	/ \a Rotation -> Drawing::Rotation;
	/ \a Font -> Drawing::Font;
- using namespace Drawing @@ \h YGUIComponent;
+ \inc <YSLib/Shell/yguicomp.h> @@ "ysbuild.h";

r13:
/= \inc @@ \h:
	/ \inc <YSLib/Helper/shlds.h> @@ \h DSReader
		-> (<YSLib/Core/yapp.h> & <YSLib/Core/yftext.h>
		& <YSLib/Shell/ytext.h> & <YSLib/Shell/ydesktop.h>
		& <YSLib/Helper/yglobal.h> & <YSLib/Service/ytmgr.h>);
	/ !\rem \inc <YSLib/Helper/yglobal.h> @@ \h Shells;

r14:
/= \inc:
	/ @@ \h YApplication:
		- \a \inc;
		+ \inc "ysdef.h";
		+ \inc "yobject.h";
		+ \inc "yfilesys.h";
		+ \inc "ystring.h";
		+ \inc "yevt.hpp";
		+ \inc "ysmsg.h";
		+ \inc "yexcept.h";
	+ \inc "ysdef.h" @@ \h (YObject & YFileSystem & YEvent & YFile_(Text)
		& YShellMessageDefinition);
	+ \inc "ycounter.hpp" @@ \h YShellMessage;
	/ @@ \h YException:
		+ \inc \h YShellDefinition;
		/ \inc "yobject.h" -> "../Adaptor/ycont.h";
	/ \inc "yexcept.h" -> \h (YShellDefinition & YObject) @@ \h YDevice;
	+ \inc \h (YShellDefinition & YObject) @@ \h YFile;
	+= \inc "ysdef.h" & "ycouter.hpp" @@ \h YResource;
	+= \inc "ysdef.h" & "yobject.h" @@ \h YShell;
	+ \inc "../Core/ysdef.h" @@ \a \h @@ \dir (Service & Shell) @@ \lib YSLib;
	+ \inc "../Core/yobject.h" @@ \h (YTextManager & YComponent & YControl
		& YFocus & YGDI & YUIContainer & YGUIComponent);
	+ \inc "../Core/ycounter.hpp" @@ \h (YComponent & YForm & YLabel
		& YGUIComponent);
	/ @@ \h YComponent:
		/ \inc "../Core/yevt.hpp" -> "../Core/yshell.h";
	/ @@ \h YDesktop:
		+ \inc "../Core/ydevice.h"
		+ \inc "../Adaptor/ycont.h";
	+ \inc "../Core/yres.h" @@ \h (YForm & );
	+ \inc "../Core/ycutil.h" @@ \h (YFocus & YLabel);
	+ \inc "ywidget.h" @@ \h YWindow;
	+ \inc "../Core/yevt.hpp" @@ \h YControl;
	- \inc ("../Core/ystring.h" & "<cwctype>") @@ \h YText;
	+ \inc "../Adaptor/yfont.h" @@ \h (YText & YLabel & YGUIComponent);
	/ \inc ("../Core/yshell.h" & "ydesktop.h" & "yform.h")
		-> "ycomp.h" & "ycontrol.h" & "../Core/yfunc.hpp" @@ \h YGUI;
	+ \inc (<YSLib/Core/ysdef.h> & <YSLib/Core/ydevice.h>
		& <YSLib/Shell/ydesktop.h> & <YSLib/Shell/yform.h>) @@ \h "ysbuild.h";
	+ \inc "ywindow.h" & "ydesktop.h" @@ \impl \u YGUI;

r15:
*= + \inc \h <cwctype> @@ \h YText;
/= \inc @@ \impl \u:
	+ \inc "../Core/yfilesys.h" @@ \impl \u YFont;
	- \inc "yshell.h" @@ \impl \u YDevice;
	+ \inc "yshell.h" @@ \impl \u YShell;
	/ \inc "../Shell/ydesktop.h" -> "../Shell/ywindow.h"
		@@ \impl \u YShellMessage;

r16:
/= \inc @@ \impl \u:
	/= \inc "../Shell/yapp.h" -> "../Shell/yexcept.h"
		@@ \impl \u YTextManager;
	/= \inc "../ysbuild.h" -> ("../Core/yfilesys.h" & "../Core/yapp.h"
		& "../Core/yshell.h" & "../Core/ydevice.h" & "../Adaptor/yfont.h"
		& "../Adaptor/ysinit.h" & "../Shell/ydesktop.h") @@ \impl \u YGlobal;
	+= \inc "yuicont.h" @@ \impl \u YControl;
	/= \inc "ydesktop.h" -> ("yuicont.h" & "ywindow.h") @@ \impl \u YLabel;
	+= \inc ("yfocus.h" & "ycontrol.h") @@ \impl \u YFocus;
	+= \inc ("yuicont.h" & "ywindow.h") @@ \impl \u YUIContainer;
	-= \inc "ydesktop.h" @@ \impl \u YForm;
	+= \inc ("ywidget.h" & "yuicont.h" & "ywindow.h") @@ \impl \u YWidget;
	/= \inc "ycontrol.h" -> "ywindow.h" @@ \impl \u YWindow;

r17:
/ \mac (YCL_USE_YASSERT & YWidgetAssert) & \f YWidget @@ \ns Widgets
	@@ \u YWidget -> \u YUIContainer;

r18:
/ \a AVisualControl => VisualControl;
- \a typedef VisualControl ParentType;
- \a typedef Controls::VisualControl ParentType;
/ @@ \u YControl:
	* + \vt @@ \dtor @@ \cl VisualControl;
	/ + \i @@ dtor @@ \cl YVisualControl ^ \mac DefEmptyDtor;
	* + \inh IControl @@ \cl YControl;
	- \exp \ctor @@ \cl MVisualControl;
/ \tr \impl @@ \mf ((DrawForeground @@ \cl (ATrack & AScrollBar) @@ \impl \u
	YGUIComponent) & (AWindow::SetSize @@ \impl \u YWindow));

r19:
/ @@ \u YControl:
	- \cl MVisualControl;
	/ @@ \cl VisualControl:
		/ \inh MVisualControl -> Control;
		/ \impl @@ \ctor;

r20:
+ \u YUIContainerEx @@ \dir Shell;
+ \cl AUIBoxControl @@ \u YUIContainerEx;
+ \inc "yuicontx.h" @@ \h YGUIComponent;

r21:
- \inh "ywindow.h" @@ \impl \u YUIContainerEx;
/ - \i @@ \f FetchWindowPtr(const IWidget&) @@ \u YUIContainer;

r22:
/ @@ \u YGUIComponent:
	/ @@ \cl YScrollableContainer:
		/ \inh (YVisualControl & MSimpleFocusResponser)
			-> (YComponent & AUIBoxControl);
		- \inh IUIBox;
		/ typedef YVisualControl ParentType
			-> typedef YComponent ParentType;
		/ \impl @@ \ctor;
		/= \mf ImplI1(IUIBox) IVisualControl* GetTopVisualControlPtr(const
			Point&) -> ImplI1(AUIBoxControl) IVisualControl*
			GetTopVisualControlPtr(const Point&);
		- \a \mf with tag 'Impl?(IUIBox)';
		/ \tr \impl @@ \mf void DrawForeground();

r23:
- \vt @@ \inh IWindow @@ \cl AWindow @@ \h YWindow;

r24:
/ @@ \u YGUIComponent:
	/ \cl YScrollableContainer -> \cl ScrollableContainer:
		- \inh GMCounter<YScrollableContainer>;
		- \inh YComponent;
		- typedef YComponent ParentType;
		/ \impl @@ \ctor;
	/ @@ \cl YSimpleListBox:
		/ \inh YScrollableContainer -> (YComponent & AScrollableContainer);
		/ typedef YScrollableContainer ParentType
			-> typedef YComponent ParentType;
		/ \impl @@ \ctor;
		/ \tr \impl @@ \mf void DrawForeground();
		/ \tr \impl @@ \mf IVisualControl* GetTopVisualControlPtr(const Point&);

r25:
* + '-mtune=arm946e-s' @@ \mac CFLAGS @@ (YSTest & YCLib & CHRLib)
	ARM9 makefile;

r26:
/= test 1 ^ \conf release;

r27:
/ @@ \cl (ATrack & AScrollBar) @@ \u YGUIComponent:
	/ \inh (VisualControl & MSimpleFocusResponser & IUIBox)
		-> \inh AUIBoxControl;
	- \a \mf with tag ImplI1(IUIBox) \exc GetTopVisualControlPtr;
	/ tag @@ GetTopVisualControlPtr -> ImplI1(AUIBoxControl);
	/ \impl @@ \ctor;

r28:
/ @@ \u YGUIComponent:
	+ \f std::pair<bool, bool> FixScrollBarLayout(Size&, const Size&,
		SDST, SDST) @@ \un \ns;
	/ @@ \cl ScrollableContainer:
		/ private \mf void FixLayout()
			-> protected \mf void FixLayout(const Size&)
				^ \f FixScrollBarLayout;
		/ \impl @@ \ctor;
		/ \tr \impl @@ \mf DrawForeground;

r29:
* \impl @@ FixScrollBarLayout @@ \un \ns @@ \u YGUIComponent;

r30:
* \impl @@ \mf AScrollBar::DrawForeground() @@ \u YGUIComponent;

r31:
/ @@ \u YGUIComponent:
	+ \mf Size GetFullViewSize() const @@ \cl YSimpleListBox;
	/ \impl @@ \mf void DrawForeground() @@ \cl YListBox;

r32:
/ @@ \u YGUIComponent:
	/ \mf void FixLayout(const Size&) @@ \cl ScrollableContainer
		-> Size FixLayout(const Size&);
	/ \impl @@ \mf void DrawForeground() @@ \cl YListBox;

r33:
/ @@ \impl \u YGUIComponent:
	* \impl @@ FixScrollBarLayout @@ \un \ns;
	/ \impl @@ \mf Size ScrollableContainer::FixLayout(const Size&);
/ @@ \cl Visual @@ \h YWidget:
	+ !\vt \mf void SetX(SPOS);
	+ !\vt \mf void SetY(SPOS);
	+ !\vt \mf void SetWidth(SDST);
	+ !\vt \mf void SetHeight(SDST);

r34:
/ @@ \u YGUIComponent:
	- \a 'defMargin?' >> (\un \ns @@ \impl \u) ~ (\cl \decl @@ \h);
	/ @@ \un \ns:
		+ \c SDST defMinScrallBarWidth(16);
		+ \c SDST defMinScrallBarHeight(16);
	/ \impl @@ \cl ScrollableContainer
		^ defMinScrallBarWidth & defMinScrallBarHeight;
	* \impl @@ \mf Size ScrollableContainer::FixLayout(const Size&);

r35:
/ \impl @@ \mf Size ScrollableContainer::FixLayout(const Size&)
	@@ \impl \u YGUIComponent;

r36:
/ \impl @@ \ctor @@ \cl ScrollableContainer @@ \impl \u YGUIComponent;

r37-r45:
/= test 2;

r46:
/ @@ \u YGUIComponent:
	* invisible scroll bar shown:
		/ \impl @@ mf void YListBox::DrawForeground();
	* visible scroll bar not shown:
		/ @@ \cl YSimpleListBox:
			+ \c @@ \mf GetItemHeight;
			/ \impl @@ \mf GetTextState;
			/ \impl @@ \mf GetFullViewSize;
			/ \tr \impl @@ \mf DrawForeground;

r47:
/= test 3 ^ \conf release;


$DOING:

relative process:
2011-02-22:
-25.3d;

/ ...


$NEXT_TODO:

b192-b270:
/ scroll bars @@ listbox \cl;
/ text alignment;
/ GDI brushes;
/ fully \impl \u DSReader;
	* moved text after setting lnGap;
* non-ASCII character path error in FAT16;
/ non-ordinary operator usage in \clt GSequenceViewer @@ \h YComponent;
/ improve efficiency @@ \ft polymorphic_crosscast @@ \h YCast;

b271-b648:
+ data config;
/ impl 'real' RTC;
+ correct DMA (copy & fill);
* platform-independence @@ alpha blending:
	+ \impl general Blit algorithm;
/ user-defined bitmap buffer @@ \cl YDesktop;
+ (compressing & decompressing) @@ gfx copying;
+ \impl loading pictures;


$LOW_PRIOR_TODO:
r325-r768:
+ \impl styles @@ widgets;
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

