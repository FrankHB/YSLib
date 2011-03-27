//v0.3032; *Build 196 r66;
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
\u YObject
{
	\cl YObject;
	\cl YCountableObject;
	\clt GDependence;
}
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
	\cl YWindow;
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
/ non-ordinary operator usage in \clt GSequenceViewer @@ \h YComponent:
	/ \i \mf operator>> => IncreaseHead;
	/ \i \mf operator<< => DecreaseHead;
	/ \i \mf operator+= => IncreaseSelected;
	/ \i \mf operator-= => DecreaseSelected;
/ \tr \impl @@ \mf YSimpleListBox::OnKeyDown @@ \impl \u ListBox;

r2:
+ \u CheckBox @@ \dir Shell:
	+ \cl YCheckBox public \inh \cl (GMCounter<YCheckBox> & YControl) @@ \ns
		Components::Controls;
/= \tr \decl @@ \cl YButton;

r3:
/ @@ \cl YCheckBox @@ \u CheckBox:
	+ \mf bool IsLockedByCurrentShell();
	* + \inc \h YWindow @@ \impl \u;
	+ \inc \h YShellHelper @@ \impl \u;
/ \impl @@ \ctor @@ \cl MLabel @@ \impl \u YLabel;
/ @@ \cl YGUIShell \u YGUI:
	/= \impl \vt \dtor ^ \mac DefEmptyDtor;
	/ \a \m bool bEntered => control_entered;
	+ \mf DefPredicate(ControlEntered, control_entered);
	+ \mf DefGetter(IControl*, KeyDownPtr, p_KeyDown);
	+ \mf DefGetter(IControl*, TouchDownPtr, p_TouchDown);

r4:
/ @@ \u YException:
	/ \def \cl Exception -> typedef std::exception Exception;
	* + \inc <exception> @@ \h;
	* + \inc <stdexcept> @@ \h;
	/ \def \cl GeneralEvent -> typedef std::runtime_error GeneralEvent;
	- \inc "../Adaptor/cont.h";
	- \inc <new> @@ \h;
	- \inc <typeinfo> @@ \h;
	/ @@ \cl LoggedEvent:
		/ \a u8 -> LevelType;
		+ typedef u8 LevelType;
	* + \inc <string> @@ \h;
/= \a std::runtime_error -> GeneralEvent @@ (\u (CheckBox & YObject
	& YUIContainer) & \impl \u Scroll);
+ \h YException @@ \h YObject;
-= \inc "ysdef.h" @@ \a file @@ \dir Core \exc ("ycutil.h" & "yexcept.h"
	& "ycounter.hpp");
-= \a \inc \h (YShellDefinition & YException & YCounter & YCoreUtilities)
	\a file @@ \dir (Shell & Service);
-= \inc \h (YControl & YUIContainer) @@ \h (ListBox & Scroll);

r5:
/ @@ \cl YCheckBox @@ \u CheckBox:
	/ \impl @@ \ctor;
	+ private \mf void OnClick(TouchEventArgs&);
	/ \inh YControl -> YThumb;
	+ \inc \h YButton @@ \h;
	/ typedef YControl ParentType -> typedef YThumb ParentType;
	* + \impl @@ \mf DrawForeground;
	+ \inc \h YGUI @@ \impl \u;
	+ \inc \h YStyle @@ \h;
/ \simp \impl @@ \mf (OnTouchDown & OnClick) @@ \cl YSimpleListBox
	@@ \impl \u ListBox;
/ \i \f FetchShellHandle @@ \u YShellHelper >> \h YComponent;
/ \f FetchGUIShellHandle @@ \u YShellHelper >> \u YGUI;
- \inc "../Helper/yshelper.h" @@ \impl \u
	(YGUI & CheckBox & Scroll & YControl);
+ \inc \h YGUI @@ \impl \u Scroll;
	
r6:
/ @@ \cl YCheckBox @@ \u CheckBox:
	/ \mf bool IsLockedByCurrentShell() >> !\m \f bool
		IsFocusedByShell(IControl&, GHHandle<YGUIShell> = FetchGUIShellHandle())
		@@ \u YGUI;
/ \cl MButton @@ \u YControl >> \u Button;
/ @@ \u YGUI:
	/ @@ \cl YGUIShell :
		- private \i \mf IControl* GetFocusedEnabledVisualControlPtr(YDesktop&);
		/ \tr \impl @@ \mf ResponseKey
	/ \impl @@ \f GetFocusedEnabledVisualControlPtr;
	
r7-r8:
/ \f bool IsFocusedByShell(IControl&, GHHandle<YGUIShell>
	= FetchGUIShellHandle()) -> \f bool IsFocusedByShell(const IControl&,
	GHHandle<YGUIShell> = FetchGUIShellHandle()) @@ \u YGUI;

r9:
/ @@ \cl YCheckBox @@ \u CheckBox:
	+ protected \m bool bTicked;
	/ \tr \impl @@ \ctor;
	/ \impl @@ \un \ns @@ \impl \u;
/ 2 \i \ft<u8 r, u8 g, u8 b> void transPixelEx() @@ \un \ns @@ \impl
	\u (CheckBox & Button) -> \i \ft<Color::MonoType r, Color::MonoType g,
	Color::MonoType b> void transform_pixel_ex() @@ \ns Drawing @@ \h YStyle;
/ \tr @@ \impl @@ \un \ns @@ \impl \u Button;
/ @@ \ns Drawing @@ \u YStyle:
	+ typedef Color rgb_t;
	+ \cl hsl_t;
	+ \f hsl_t rgb2hsl(rgb_t);
	+ \f rgb_t hsl2rgb(hsl_t);
	+ \i \f Color rgb2Color(const rgb_t&);
	+ \i \f rgb_t Color2rgb(const Color&);
+ \inc <YSLib/Shell/checkbox.h> @@ \h Build;
/ @@ \cl ShlExplorer::TFrmFileListSelecter @@ \u Shells:
	+ \m YCheckBox chkTest;
	/ \tr \impl @@ \ctor;
* @@ \h CheckBox:
	/ \mac INCLUDED_LISTBOX_H_ => INCLUDED_CHECKBOX_H_

r10-r12:
/= test 1;

r13:
* @@ \h YObject:
	/ \ctor \t<typename _tVec> \i \exp Size(const _tVec&)
		-> !\t \ctor \i \exp Size(const Vec&);

r14-r17:
/ \impl @@ \un \ns @@ \impl \u CheckBox; 

r18:
* \impl @@ \i \f (GetR & GetB) @@ \cl Color @@ \h YCommon;
* \impl @@ \f hsl_t rgb2hsl(rgb_t) @@ \impl \u YStyle;

r19:
/= test 2 ^ \conf Release;

r20:
* \impl @@ \f (hsl_t rgb2hsl(rgb_t) & rgb_t hsl2rgb(hsl_t)) @@ \impl \u YStyle;

r21:
/ \tr \impl @@ \ctor \cl ShlExplorer::TFrmFileListSelecter @@ \u Shells;

r22-r24:
	r(22, 24):
	* \impl @@ \f (hsl_t rgb2hsl(rgb_t) & rgb_t hsl2rgb(hsl_t))
		@@ \impl \u YStyle;
	r22-r24;
	/ \impl @@ \un \ns @@ \impl \u CheckBox;

r25-r26:
/= test 3;

r27:
/ \impl @@ \f bool DrawRect(const Graphics&, const Point&, const Size&, Color)
	@@ \impl \u YGDI;
/ \tr \impl @@ \f DrawWidgetBounds @@ \un \ns @@ \impl \u YGUI;

r28:
* \impl @@ \f bool DrawRect(const Graphics&, const Point&, const Size&, Color)
	@@ \impl \u YGDI;

r29-r30:
/ \impl @@ \un \ns @@ \impl \u CheckBox;

r31:
/ \tr \impl @@ \ctor \cl ShlExplorer::TFrmFileListSelecter @@ \u Shells;

r32:
/ {
	typedef s16 SPOS;
	typedef u16 SDST;
} >> \ns platform @@ \h YCommon ~ @@ \ns YSLib @@ \h YShellDefinition;
/ @@ \h YCommon:
	/= \a u16 -> SDST @@ \st CursorInfo;
+ using platform::SPOS & using platform::SDST @@ \ns YSLib @@ \h YAdaptor;
/ \a SPOS => SPos;
/ \a SDST => SDst;

r33:
/ \tr \impl @@ \f hsl2rgb @@ \impl \u YShell to remove warnings;
/= test 4 ^ \conf release;

r34-r42:
/ \impl @@ \un \ns @@ \impl \u CheckBox;

r43:
/ @@ \h YText:
	+ \cl EmptyTextRenderer;
	+ \f SDst FetchStringWidth(TextState&, String&);
/ \a GetPreviousLinePtr => FetchPreviousLineIterator;
/ \a GetNextLinePtr => FetchNextLineIterator;
/ \impl @@ \mf void PaintText(Widget&, const Graphics&, const Point&)
	@@ \cl MLabel @@ \impl \u YLabel;

r44:
* \impl @@ \mf void PaintText(Widget&, const Graphics&, const Point&)
	@@ \cl MLabel @@ \impl \u YLabel;

r45:
/ @@ \u YText:
	+ \f void MovePen(TextState&, fchar_t);
	* @@ \cl EmptyTeztRenderer:
		/ \i void \op()(fchar_t) -> !\i void op()(fchar_t) ^ \f MovePen;

r46-r53:
/= test 5;

r54:
* \impl @@ \f SPos FetchLastLineBasePosition(const TextState&, SDst)
	@@ \impl \u YText;
* \impl @@ \mf void PaintText(Widget&, const Graphics&, const Point&)
	@@ \cl MLabel @@ \impl \u YLabel;

r55-r61:
/= test 6;

r62:
* \impl @@ \mf void PaintText(Widget&, const Graphics&, const Point&)
	@@ \cl MLabel @@ \impl \u YLabel;

r63:
/ @@ \cl MLabel @@ \u YLabel:
	/ \rem \m bool AutoSize;
	/ \rem \m bool AutoEllipsis;
	+ typedef \en TextAlignmentStyle;
	+ \m TextAlignmentStyle Alignment;
	/ \exp \ctor MLabel(const Drawing::Font&) -> MLabel(const Drawing::Font&,
		TextAlignmentStyle = Left);
	/ \impl @@ \mf void PaintText(Widget&, const Graphics&, const Point&);
/ \impl @@ \ctor @@ \cl YButton @@ \impl \u Button;

r64:
/ \tr \impl @@ \mf YApplication::ResetFontCache;
/ \tr @@ \impl \u Shells:
	/ \tr \impl @@ \f GetGlobalImageRef @@ \un \ns;
	/ \impl @@ \ctor @@ \cl ShlExplorer::TFrmFileListSelecter;
	
r65:
/ \tr @@ \impl \u Shells:
	/ \impl @@ \ctor @@ \cl ShlSetting::TFormB;
	/ @@ \cl ShlSetting::TFormC:
		/ \impl @@ \ctor;
		/ \impl @@ \mf btnC_Click;

r66:
/= test 7 ^ \conf release;


$DOING:

relative process:
2011-03-27:
-27.3d;

/ ...


$NEXT_TODO:

b196-b240:
+ key routing events for the view tree;
/ fully \impl \u DSReader;
	* moved text after setting lnGap;
* non-ASCII character path error in FAT16;

b241-b576:
/ impl 'real' RTC;
+ data configuragion;
+ shared property: additional;
/ GDI brushes;
/ text alignment;
+ \impl pictures loading;
/ improve efficiency @@ \ft polymorphic_crosscast @@ \h YCast;
+ correct DMA (copy & fill);
* platform-independence @@ alpha blending:
	+ \impl general Blit algorithm;
/ user-defined bitmap buffer @@ \cl YDesktop;
+ (compressing & decompressing) @@ gfx copying;


$LOW_PRIOR_TODO:
r577-r864:
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

Build a more advanced console wrapper.

Build a series set of robust gfx APIs.

More GUI features needed:
Icons;
Other controls.

Other stuff to be considered to append:
Design by contract: DbC for C/C++, GNU nana.


$HISTORY:

$add_features +;
$fix_bugs *;
$transform $list ($lest_member $pattern "b*"."\(\"\*\"\)") +;

now:
{
"controls: checkbox",
* "platform color type",
"horizontal text alignment in class %MLabel"
},

b195:
{
* "makefiles",
"dependency events",
"simple events routing"
},

b170_b194:
{
"controls: track",
"controls: scroll bar",
"controls: scrollable container",
"controls: listbox"
},

b159_b169:
{
"controls: buttons": class ("%YThumb", "%YButton"),
"controls: listbox class",
"events",
},

b132_b158:
{
"core utility templates",
"smart pointers using Loki",
"Anti-Grain Geometry test",
"GUI focus",
"shells",
"base abbreviation macros",
"controls",
"virtual inheritance in control classes",
"exceptions",
"debug macros & functions",
"color type",
"class template %general_cast",
"timer class"
},

b1_b131:
{
"initial test with PALib & libnds",
"shell classes",
"the application class",
"CHRLib: character set management",
"fonts management using freetype 2",
"YCLib: platform independence",
"basic objects & counting",
"global objects",
"string class",
"file classes",
"dual screen text file reader",
"output devices & desktops",
"messaging",
"program initialization",
"simple GUI: widgets & windows",
"simple GDI",
"simple resource classes"
};


*/
//---- temp code;
/*

	static YTimer Timer(1250);

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

