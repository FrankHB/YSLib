// v0.3229; *build 225 rev 96;
/*
$META:
//$configureation_for_custom_NPL_script_parser:
$parser
(
	$eval($ignore_not_impl);
	$eval($for_each $anything $in $.iterators $do_something) unfold;
);
$parser.$ __pstate;
$parser.unfold __unfold;
$parser.$.iterators __iterators;

__unfold.$ctor $=
(
	__pstate.behavior.ignore $= $true;
);
__unfold.$dtor $=
(
	__pstate.behavior.ignore $= $false;
);
__unfold __iterators.for_labeled_paragraph
(
	$DONE,
	$DOING,
	$NEXT_TODO,
	$LOW_PRIOR_TODO,
	$KNOWN_ISSUE,
	$TODO
);

$script_preprocessor_escapse:
//$Record prefix and abbrevations:
$parser.$preprocessor.$define_schema "<statement> ::= $statement_in_literal";
; ::= statement termination
= ::= equivalent
+ ::= added
- ::= removed
* ::= fixed
/ ::= modified
% ::= identifier
~ ::= from
! ::= not
& ::= and
| ::= or
^ ::= used
-> ::= changed to
>> ::= moved to
=> ::= renamed to
<=> ::= swapped names
@ ::= in / belonged to
\a ::= all
\ab ::= abstract
\ac ::= access
\adtor ::= abstract destructor
\amb ::= ambiguities
\amf ::= abstract/pure virtual member function
\arg ::= arguments
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
\doc ::= documents
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
\expr ::= expressions
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
\simp ::= simplified
\sm ::= static member
\smf ::= static member functions
\smft ::= static member function templates
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
(
	\cl YObject,
	\cl YCountableObject,
	\clt GDependence
),
\u YShell
(
	\cl YShell
),
\u YApplication
(
	\cl YLog,
	\cl YApplication
),
\u YConsole
(
	\cl Console
),
\u YWidget
(
	\in IWidget,
	\cl Visual,
	\cl Widget
),
\u YUIContainer
(
	\in IUIBox,
	\in IUIContainer,
	\cl UIContainer
),
\u YControl
(
	\in IControl,
	\cl Control
),
\u YPanel
(
	\in IPanel,
	\cl Panel
)
\u YWindow
(
	\in IWindow,
	\cl MWindow,
	\cl AWindow,
	\cl AFrame,
	\cl Frame
),
\u YGUI
(
	\cl YGUIShell,
)
\u Label
(
	\cl MLabel,
	\cl Label,
	\cl MTextList
),
\u TextArea
(
	\cl TextArea
),
\u Progress
(
	\cl ProgressBar
),
\u UIContainerEx
(
	\cl AUIBoxControl,
),
\u Form
(
	\cl Form;
),
\u Button
(
	\cl Thumb,
	\cl Button
),
\u CheckBox
(
	\cl CheckBox
),
\u TextList
(
	\cl TextList
),
\u Menu
(
	\u Menu
),
\u Scroll
(
	\cl ATrack,
	\cl HorizontalTrack,
	\cl VerticalTrack,
	\cl MScrollBar,
	\cl AScrollBar,
	\cl HorizontalScrollBar,
	\cl VerticalScrollBar
),
\u ListBox
(
	\cl ListBox,
	\cl FileBox,
);


$DONE:
r1:
/ @ \h YWidget:
	/ \amf GetContainerPtr => GetContainerPtrRef @ \in IWidget;
	+ \i \f IUIBox* FetchContainerPtr(const IWidget&);
	/ \tr @ \cl Widget;
/ \tr \impl @ 2 \ft FetchWidgetDirectNodePtr @ \h YUIContainer;
/ \tr \impl @ \impl \u (YUIContainer & Scroll & ListBox & YControl
	& YGUI & YPanel & YWidget & YWindow);

r2:
/ @ \h YWindow:
	/ \amf const Rect& GetInvalidatedArea() const
		-> Rect& GetInvalidatedAreaRef() const;
	/ \m Rect rInvalidated -> mutable Rect rInvalidated @ \cl MWindow;
	+ \i \f const Rect& FetchInvalidatedArea(const IWindow&);
	/ \tr @ \cl AWindow;
/ \tr \impl @ \impl \u (YWindow & YWidget);

r3:
/ @ \u YWindow:
	- \amf CommitInvalidatedArea;
	/ \mf void CommitInvalidatedArea(const Rect&) @ \cl AWindow
		-> !\m \f void CommitInvalidatedAreaTo(IWindow&, const Rect&);
/ \tr \impl @ \mf Widgets::Invalidate @ \impl \u YWidget;

r4:
/ @ \cl Rect @ \h YGDIBase:
	+ \exp \de Rect& operator=(const Rect&);
	+ \mf Rect& operator(const Point&);
	+ \mf Rect& operator(const Size&);
/ @ \u YWindow:
	+ \i \f void ResetInvalidatedAreaOf(IWindow&);
	/ \impl @ \mf AWindow::Invalidate ^ \f ResetInvalidatedAreaOf;
	+ \f void SetInvalidateonToParent(IWindow&);
	/ \impl @ \f (Show & Hide) ^ \f SetInvalidateonToParent;
	/ \simp \impl @ \ctor @ \cl AWindow;

r5:
/= test 1 ^ \conf release;

r6-r8:
/= test 2;

r9:
/ @ \u YWidget:
	/ \amf void Invalidate(const Rect&) @ \in IWidget
		-> \amf void Validate();
	+ !\m \f Invalidate(IWidget&, const Rect&);
	/ \tr @ \cl Widget:
		/ \mf Invalidate \mg -> !\m \f void Invalidate(IWidget&, const Rect&);
		+ \em \mf void Validate();
	/ \impl @ !\m \f void Invalidate(IWidget&);
/ \tr @ \cl AWindow @ \u YWindow:
	/ \mf Invalidate -> Validate;
/ \tr @ \cl Panel @ \h YPanel;
/= \a 'Widgets::Invalidate' -> 'Invalidate';

r10:
/= test 3 ^ \conf release;

r11:
/ @ \u YWidget:
	/ \amf void Refresh() -> void Refresh(const Graphics&, const Point&,
		const Rect&);
	/ \tr @ \cl Widget;
/ \impl @ \mf Frame::DrawContents @ \impl \u YWindow;
/ \a \mf void DrawControl() -> void DrawControl(const Graphics&,
	const Point&, const Rect&);
/ \tr @ \u (Label & Progress & TextArea & YControl & YWindow & Scroll
	& TextList & Button & CheckBox & ListBox);
/ \tr \impl @ \mf YGUIShell::ShlProc;
/ \tr \impl @ \cl Panel @ \h YPanel;

r12-r13:
/= test 4:

r14:
/ @ \impl \u Scroll:
	/ \simp \mf ScrollableContainer::DrawControl;
	* \impl @ \mf ScrollableContainer::DrawControl $since r11;
	* \impl @ \mf ATrack::DrawControl $since r11;

r15:
* \impl @ \mf ATrack::DrawControl @ \impl \u Scroll $since r11;

r16:
+ \f \i void DrawSubControl(Control&, const Graphics&, const Point&,
	const Rect&) @ \h YControl;
+ \f \i void RefreshSub(IWidget&, const Graphics&, const Point&, const Rect&)
	@ \h YWidget;
/ \simp \impl @ \impl \u Scroll ^ \f DrawSubControl ^ \RefreshSub;
/ \impl @ \mf AScrollBar::DrawControl;

r17:
/ \simp \impl @ \mf Widget::Refresh;
/ @ \u YUIContainer:
	- \f void Fill(IWidget&, Color);
	- \mac YWidgetAssert;
	- \f Components::yassert;
- \inc \h "yuicont.h" @ \impl \u (YWidget & Button & Label & TextArea);
- \inc \h "ywindow.h" @ \impl \u (Button & Label & TextArea & YControl
	& CheckBox & ListBox & Progress & Scroll & UIContainerEx
	& YFocus & YPanel & YGUI & YUIContainer & YWidget);
+ \inc \h "../Service/yblit.h" @ \impl \u (Button & CheckBox);
- \a 3 window \as \str @ \impl \u YWindow;

r18:
/= test 5 ^ \conf release;

r19:
/ \impl @ \mf Frame::DrawContents:
	+ valid graphics test;

r20:
+ Rect Intersection(const Rect&, const Rect&) @ \u YGDIBase;
+ \inc \h <algorithm> @ \impl \u YGDIBase;
/ \impl @ \mf Frame::DrawContents ^ \f Intersect;

r21:
/ \impl @ \mf AWindow::DrawBackgroundImage;

r22:
/ @ \cl AWindow:
	/ \impl @ \mf DrawRaw;
	/ \simp \impl @ \mf DrawBackgroundImage;

r23:
/= \impl @ \mf (TextRegion::ClearLine & ATextRenderer::ClearLine)
	@ \impl \u YText;
/= \rem @ \mf ShlExplorer::TFormExtra::OnClick_btnTestEx @ \impl \u Shells;

r24-r27:
/= test 6;

r28:
* \impl @ \ctor @ \cl AWindow $since r4;

r29:
/ \impl @ \f CommitInvalidatedAreaTo @ \impl \u YWindow;

r30:
/ \simp \impl @ \mf YMainShell::OnActivated @ \impl \u Shells;

r31-r39:
/= test 7;

r40:
/ \impl @ \mf YMainShell::OnActivated @ \impl \u Shells;

r41:
/ \simp \impl @ \mf ShlDS::UpdateToScreen ^ \mf Validate ~ \f Invalidate;

r42-r43:
/ \impl @ \mf ShlExplorer::OnActivated @ \impl \u Shells;

r44:
/ @ \impl \u Shells:
	/ @ \cl ShlExplorer:
		/ \impl @ \ctor @ \cl TFormTest;
		/ \impl @ \ctor @ \cl TFormExtra;
		/ \impl @ \mf OnActivated;
		/ \impl @ \mf OnTouchDown_FormExtra;
	/ \impl @ \mf ShlReader::OnActivated;

r45:
/= test 8;

r46:
/ \impl @ \f void Invalidate(IWidget&, const Rect&) @ \impl \u YWidget;
/ \impl @ \mf AWindow::Refresh;

r47-r50:
/= test 9;

r51:
/ \impl @ \mf YMainShell::OnActivated @ \impl \u Shells;

r52:
/ \impl @ \mf AWindow::DrawBackgroundImage;

r53:
/ \impl @ \mf Frame::DrawContents;

r54:
/ @ \impl \u YGDIBase:
	* \impl @ \f Intersect $since r20;
	- \inh \h <algorithm>;

r55:
/ \impl @ \f Intersect @ \impl \u YGDIBase;

r56-r59:
/= test 10;

r60:
* \impl @ \f Intersect @ \impl \u YGDI $since r20;

r61:
+ \f \i void SetEnabledOf(IControl&, bool = true);
/ @ \impl \u Shells ^ \f SetEnabledOf ~ \mf SetEnabled;

r62:
/ @ \impl \u Shells ^ \a \f SetEnabledOf ~ \mf SetEnabled;

r63:
/= test 11 ^ \conf release;

r64-r65:
/ \simp \impl @ \f Invalidate(IWidget&, const Rect&) @ \impl \u YWidget;

r66-r74:
/= test 12;

r75:
/ \impl @ \f Invalidate(IWidget&, const Rect&) @ \impl \u YWidget;

r76-r77:
/= test 13;

r78:
/ \impl @ \mf OnTouchMove_Dragging @ \impl \u YControl;

r79:
/ @ \u YGDIBase:
	/= \st Size -> \cl Size with (\ac public @ \a \m);
	+ \i \mf bool IsOrigin() const @ \cl Point ^ \mac DefPredicate;
	/ @ \cl Size:
		+ \i \mf bool IsEmpty() const;
		/ \simp \mf \op Vec ^ \mac DefConverter;
	/ \simp \mf \op Point @ \cl Vec ^ \mac DefConverter;
	+ \f Rect Unite(const Rect&, const Rect&);
	/ \simp @ \f Intersect;
/ @ \impl \u YWindow:
	/ \simp \impl @ \f CommitInvalidatedAreaTo ^ \f Unite;
	/ \simp \impl @ \f RequiresRefresh ^ \mf Size::IsEmpty;

r80:
/ \simp \impl @ \f CommitInvalidatedAreaTo @ \impl \u YWindow;
* \impl @ \f Unite @ \impl \u YGDIBase $since r79;

r81:
/= test 14 ^ \conf release;

r82-r86:
/ @ \cl ShlExplorer @ \impl \u Shells
	/ \impl @ \ctor;
	/ \impl @ \mf OnActivated;

r87:
* \impl @ \mf OnActivated @ \impl \u Shells $since r85;

r88-r90:
* \impl @ \mf Frame::DrawContents @ \impl \u YWindow $since r53;

r91:
/= test 15 ^ \conf release;

r92:
/ @ \u YWindow:
	/ @ \cl AWindow:
		/ \simp \impl @ \mf Update;
		/ \simp \impl @ \mf DrawRaw;
	/ @ \cl MWindow:
		- protected \m bool bUpdate;
		/ \tr \simp \impl @ \ctor;
	- \tr \m RequiresUpdate @ \cl (MWindow & AWindow) & \in IWindow;
/ \tr \impl @ \mf Desktop::Update;

r93:
/ @ \u YWidget:
	- \amf Validate @ \in IWidget;
	- \mf Validate @ \cl Widget;
- \mf Validate @ \cl Panel;
- \vt @ \mf Validate @ \cl AWindow;

r94-r95:
/= test 16;

r96:
/= test 17 ^ \conf release;


$DOING:

$relative_process:
2011-07-13:
-18.0d;
//Mercurial rev1-rev95: r4738;

/ ...


$NEXT_TODO:
b226-b256:
+ TextList invalidation support;
* non-ASCII character path error in FAT16;
/ fully \impl \u DSReader;
	* moved text after setting lnGap;

b257-b768:
+ key accelerators;
+ \impl styles @ widgets;
/ fully \cl Path;
/ impl 'real' RTC;
+ data configuragion;
+ \impl pictures loading;
/ text alignment;
+ clipping areas;
/ improve efficiency @ \ft polymorphic_crosscast @ \h YCast;
+ correct DMA (copy & fill);
* platform-independence @ alpha blending:
	+ \impl general Blit algorithm;
+ shared property: additional;
+ GDI brushes;
/ user-defined bitmap buffer @ \cl Desktop;


$LOW_PRIOR_TODO:
b769-b1536:
+ (compressing & decompressing) @ gfx copying;
+ Microsoft Windows port;
+ general component operations:
	+ serialization;
	+ designer;
+ database interface;


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


$KNOWN_ISSUE:
// obselete all resolved;
* corrupted embedded bitmap glyph loading $since b185;
* fatal error occured using "simson.ttc";


$HISTORY:

$parser.state.old_style $= $false;
$design; // features changing only made sense to library developers;
$doc; // for documents target;
$add_features +; // features added;
$fix_bugs *; // bugs fixed;
$modify_features /; // features modified;
$remove_features -; // features removed;
$using ^; // using;
$source_from ~; // features replacing from;
$belonged_to @;
$moved_to >>;
$renamed_to =>;

//$transform $list ($list_member $pattern $all($exclude $pattern \
//	$string_literal "*")) +;

$ellipse_refactoring;
$ellipse_debug_assertion;

$now
(
	/ "windows partial invalidation support" $=
	(
		/ "windows partial invalidation committing for class %IWindow"
			~ "class %AWindow",
		+ "widget validation abstract member function" @ "class %IWidget",
		/ "widget invalidation interface as non-member function"
			~ ("abstract member function" @ "class %IWindow")
	),
	+ "assignment operators" @ "class %Rect",
	/ "widget rendering efficiency improvement" $=
	(
		+ "parameters for member function %Refresh" @ "widgets",
		/ "background refreshing" ^ "invalidation" @ "class %AWindow",
		/ "invalidating of windows" $=
		(
			/ "without validating old invalidated areas"
		),
		+ "partial invalidation" @ "control default %TouchMove event handler \
			%OnTouchMove_Dragging"
	),
	+ "intersection and union calculation for class %Rect",
	/ "shells test example" $=
	(
		+ "opaque background" @ "pseudo-frame-per-second counter",
		/ "controls layout" @ "shell class %ShlExplorer",
		- "windows update state"
	)
),

b224
(
	* "wrong ascending when switching font size" $since b224 $=
	(
		^ "freetype 2.4.4 cache system" ~ "freetype 2.4.5 cache system"
			// NOTE: it seems a new bug in freetype 2.4.5 cache system.
	),
	+ "containing test" @ "class %Rect",
	+ "WinGDB settings" @ "main project file",
	+ "windows partial invalidation support" $=
	(
		/ "invalidation interface",
		+ "windows partial invalidation committing for class %AWindow"
	)
),

b223
(
	/ "UI implementation" $=
	(
		/ "DS painting" ^ "message %SM_PAINT"
			~ "directly calling of %ShlDS::UpdateToScreen"
	),
	* "uncleared application message queues on program exit" $since b174,
		// NOTE: this might cause memory leak.
	/ "shells test example" $=
	(
		+ "pseudo-frame-per-second counter",
		/ "button enabling" ^ "file extension matching in the file box"
	),
	* "declaration of function %GetStemFrom" @ "header yfilesys.h" $since b161,
	/ "updated freetype" $=
	(
		^ "updated freetype 2.4.5" ~ "freetype 2.3.4",
		+ "exact bounding box calculation",
		+ "path stroker",
		+ "support for synthetic embolding and slanting of fonts",
		- "obselete header /freetype/internal/pcftypes.h" $since b185
	),
	^ "updated libnds 1.5.1 with default arm7 0.5.21"
		~ "libnds 1.5.0 with default arm 7 0.5.20",
	^ "updated devkitARM release 34" ~ devkitARM release 33"
),

b222
(
	* $design "minor faults dectected by CppCheck" $=
	(
		/ "simplified implementation" @ "%YGUIShell::ResponseTouch" $since b195,
		* "implementation" @ "%ystdex::fexists" $since b151
	),
	+ "default GUI event %Paint",
	/ "controls drawing" ^ "event Paint" ~ "member function Draw"
		@ "class (%Frame, %YGUIShell)",
	/ "GUI controls inhancement" $=
	(
		+ "key events for text list controls including menu" $=
		(
			+ "state-based cyclic traversing" @ "class %TextList",
			+ "%TouchDown %KeySpace::Up and %TouchDown %KeySpace::Down \
				of cyclic traversing",
			+ "%TouchDown %KeySpace::Up and %TouchDown %KeySpace::Down \
				of initializing selection when unselected"
		)
	),
	/ "shells test example" $=
	(
		+ "menu" @ "class %ShlReader",
		^ "widget drawing" ^ "direct desktop background drawing"
	),
	+ "controls: buffered text area"
),

b221
(
	/ "shells test example" $=
	(
	`	/ "simplified direct-painting test",
		/ "simplified key-to-touch event mapping implementation"
			@ "class %Control"
	),
	^ "devkitARM release 33" ~ "devkitARM release 32",
	/ $design "header search path of VS2010 projects",
	/ "event map interfaces" $=
	(
		+ "sender type as template parameter type",
		/ "simplified sender argument forwarding"
		/ "more efficient implimentation"^ "%unique_ptr" ~ "%shared_ptr",
			// NOTE: old version of libstdc++ might fail in compiling
			// due to members with parameter of rvalue-reference type
			// are not supported.
	),
	* "move constructor lost" @ "class template %GMCounter" $since b210,
	/ "class %HDirectory" ^ "POSIX dirent API" ~ "libnds-specific API",
	+ $design "diagnostic pragma for GCC 4.6.0 and newer"
		@ "header type_op.hpp",
	* "class %HDirectory state not restored during operations" $since b175,
	/ "controls key-to-touch event mapping" @ "class %Control" $=
	(
		/ "custom handler" ^ "std::function" ~ "member function",
		+ "%KeyPress to %Click event mapping methods" @ "class %Control",
		* "calling mapped to disabled control" $since b217
	),
	/ "GUI input response" $=
	(
		* "%YShell::ResponseTouch wrong behavior for container not locating \
			at (0, 0)" $since b213,
		* "%YShell::ResponceKey wrong behavior for tunnel event for container"
			$since b199
	),
	/ "menus functionality" $=
	(
		+ "key events for submenus" $=
		(
			+ "%TouchDown %KeySpace::Left for hiding",
			+ "%TouchDown %KeySpace::Right for showing"
		),
		+ "automatically selecting when submenu shown using keypad"
	),
	/ "sequence viewer" $=
	(
		/ $design "default argument" ^ "unsigned integral type"
			~ "signed integral type" @ "class template %GSequenceViewer",
		+ "restricted value setting",
		* "assertion failed in containing test when the length is zero"
			$since b171
	)
),

b220
(
	+ "non-focused textlist border",
	/ "control focus interfaces" $=
	(
		+ "specified %IControl reference as sender parameter"
	),
	/ "menus functionality" $=
	(
		* "menu hiding when submenu shown and focus lost" $since b214,
		+ "automatically locating submenu when shown",
	),
	* "wrong text margin of widgets not entirely shown in the window"
		$since b190
),

b219
(
	/ $design "unit %YGUI decomposed for Blit and 2D API",
	/ "optimized widget-to-widget locating",
	/ $design "font cache moved to platform-dependent application class from
		class %YApplication",
	+ "several global helper functions as platform-independent interface",
	/ "shells test example" $=
	(
		+ "mutiple background switch test"
	),
	* "wrong default argument of desktop backgrond color" $since b160,
	+ "widgets: progress bar" @ "class %ProgressBar"
),

b218
(
	/ "shells test example" $=
	(
		+ "desktop background switch test"
	),
	+ $design "ownership tag structure",
	- $design "global resource helper functions",
	+ "DS screen class",
	^ "class %YMainShell for resource loading" ~ "class %ShlLoad",
	- "class %ShlLoad"
),

b217
(
	* "wrong lost focus behavior for menus \
		which had popped submenu(s) previously" $since b214,
	/ "improved compatibility" @ "class %GHEvent for types \
		not met EqualityComparable requirement interface \
		including closure types" $=
	(
		/ "result always true",
		* "compile-error for non-trivally copy-assignable objects",
		/ "optimized implementation to avoid bloat code instantialized"
	),
	+ $design "Code::Blocks project files" $=
	(
		+ "workspace file",
		+ "project files"
	),
	/ $design ^ "lambda expression" ~ "several private member function \
		as event handlers",
	+ "%KeyDown and %KeyHeld handlers for scroll bars \
		to perform mapped touch behaviors",
	+ "key-to-touch event mapping methods" @ "class %Control",
	* "wrong behavior" @ "track touch held event handler" $since b213 $=
	(
		* "wrong coordinate passed to touch focus captured control" $since b195;
	),
	+ $design "cscope file"
),

b216
(
	/ $design "header file dependencies",
	/ "focused boundary for controls",
	* "strict ISO C++2011 code compatibility" $=
	(
		* "deprecated keyword static using in ISO C++2003 now removed but"
			^ @ unit "YApplication when macro YSL_DEBUG_MSG defined"
	),
	/ "more efficient message queue implementation" ^ "%multiset"
		~ %priority_queue",
	/ "simplified shell example",
	+ "default windows Z order",
	+ "message fetching automatically calling %Idle \
		when size less or equal than a user-specified limit"
),

b215
(
	+ "disabled state style of control %Thumb",
	* "disabled control touch event handling" $b199,
	/ "shells test example" $=
	(
		+ "checkbox and disabling test"
	),
	+ "visible arrow indicating multi-level menus",
	/ "creating list" @ ("constructor of class %MTextList"
		~ "member function %GetList"),
	+ "event handler supporting for types not met EqualityComparable
		requirement interface including closure types",
	* $design "access of interitance of class std::function"
		@ "class template %GHEvent" $since b207,
	/ "shells test example" $=
	(
		+ "disabled behavior test" @ "%ShlExplorer"
	),
	/ "thumb/button boundary style",
	/ "global architecture" $=
	(
		/ "screen constant" >> "namespace scope" ~ "class %Global",
		+ "class %YDSApplication" ~ "class %Global",
		- "singleton mechanism" @ "class %YApplication"
	)
),

b214
(
	/ "functions of menus" $=
	(
		+ "hiding on confirmed",
		+ "resizing with width of text in list",
		/ "margin of menus",
		+ "submenus"
	),
	* $design "exception specification" @ "unit %YApplication",
	/ $doc $=
	(
		* "template parameter description" $since b189 $=
		(
			^ "\tparam" ~ "\param"
		),
		* "\defgroup description spell error" $since b209,
		* $design "operator new & delete conmments" $since b203,
		/ "simplified doxygen file excluded paths"
	),
	/ $design ^ "public %noncopyable inheritance"
		~ "all private %noncopyable inheritance",
	* "point containing test for zero width or height rectangle \
		turned out asseration failure" $since b204,
	* "constness of text width mersuring" $since b197,
	/ "simplified focus operations interface" $=
	(
		- "unused parameter and argument"
	)
),

b213
(
	* $design "UI assertion strings",
	* "menu colors",
	* "touch event coordinate error" @ "contianer controls" $since b195,
	+ "functions of menus" $=
	(
		+ "resizing for content",
		+ "hiding when focus lost"
	),
	+ "predicator %Contains" @ "UI container implementation"
),

b212
(
	/ $design "assertion strings",
	/ "menu constructor",
	+ "redundant menu state member" @ "class %Menu",
	/ "container member APIs" ^ "reference parameter" ~ "pointer parameter",
	* "constructor of class %Frame",
	+ "Z order for widget overlaying" @ " UI containers",
	+ "menu laid at top level" ^ "Z order"
),

b211
(
	/ "implemented messages with general object content holder"
		^ "non-pointer member" ~ "%shared_ptr",
	+ "message content mapping",
	/ $design "messaging APIs" >> "unit YApplication" ~ "unit YShell",
	/ "test menu fixed on the desktop"
),

b210
(
	+ "template %is_valid for checking pointers which cast bool explicitly",
	/ "part of template %raw moved to namespace %ystdex",
	- $design "some unused code",
	+ "macro %yforward for simply using std::forward and argument \
		type deduction",
	/ "handles and smart pointers rearranged" $=
	(

		- "%GHWeak",
		^ "%std::shared_ptr" ~ "%GHandle",
		- "%GHandle"
	),
	/ "implementation of messages" $=
	(
		/ "implementation of class %Message",
		- "message ID mapping"
	)
),

b209
(
	/ "merged library CHRLib" >> "library YCLib",
	/ $design "protected function inheritance" @ "class template %GHEvent",
	/ "using directive of namespace %ystdex" @ "library YSLib",
	+ "lost %Rect operations",
	* "strict ISO C++2003 code compatibility" $=
	(
		^ "fixed macros" ~ "variadic macros" @ "header YFont"
	),
	/ "renamed directory %Shell to %UI @ "library YSLib",
	/ "several memory utilities for std::shared_ptr and std::unique_ptr"
		>> "library YCLib::YStandardExtend"
),

b208
(
	^ "rvalue references as parameter types of event handlers",
	/ "set default font size smaller",
	+ "vertical alignment" @ "labeled controls",
	* "fatel error" @ "direct UI drawing testing"
),

b207
(
	/ $design "event handler implementation ^ std::function" ~ "Loki::Function",
	- "library AGG",
	- "library Loki",
	+ $design "union %no_copy_t and union %any_pod_t for supressing \
		static type checking",
	+ $design "polymorphic function object template and base class",
	- "single-cast event class template",
	^ "rvalue reference" @ "event class",
	* "ystdex algorithms",
	/ "shells test example" $=
	(
		* "minor event handler parameter type mismatch"
	),
	* "key events response(added as %KeyDown, but removed as %KeyPress)"
		@ "project YSTest_ARM9" $since b201,
	* "tunnel and direct key and touch response repeated" @ "class %YGUIShell"
		$since b198
),

b206
(
	+ "menu test button",
	/ "file API",
	* "strict ISO C++2003 code compatibility" $=
	(
		+ "function %memcmp declation" @ "namespace %ystdex"
	),
	* "strict ISO C++2011 code compatibility" $=
	(
		* "implicit narrowing conversion(N3242 8.5.4/6) \
			in C++2011(N3242 5.17/9)" ^ "explicit static_cast",
		/ "character types" @ "header platform.h"
	),
	/ "coding using limited C++2011 features" $=
	(
		/ $design ^ "C++2011 style nested template angle brackets",
		/ $design ^ "keyword %auto",
		/ ^ "C++2011 type_traits" @ "namespace std" ~ "std::tr1"
	),
	- "Loki type operations",
	/ ^ "namespace %ystdex" @ "namespace %YSLib"
),

b205
(
	/ "simplified widgets and controls inheritance",
	/ "simplified image resource type",
	/ $design "protected inheritance of module classes except \
		%MScreenPositionEventArgs",
	/ "listbox highlight text color"
),

b204
(
	/ "weak handle improvement",
	/ $design "exception specification macro",
	- "window handles" $=
	(
		- "type %HWND",
		^ "%IWindow*" ~ "%HWND"
	),
	* "track background painting ignored" $since b191,
	+ "class %nullptr_t" @ "namespace ystdex",
	^ "nullptr at YSLib",
	/ $design "widgets virtual member functions"
),

b203
(
	/ $design "units rearrangement",
	/ $design "global architecture" $=
	(
		+ "global instance function",
		- "all global objects",
		- "platform dependent global resource" @ "class %YApplication",
		- "global object" @ "unit YNew"
	),
	+ "class %MTextList",
	/ "class %YSimpleText List using class %MTextList inheritance",
	/ "class %YSimpleText renamed to %YMenu",
	/ "using std::tr1::shared_ptr" ~ "smart pointers" @ "library Loki"
),

b202
(
	/ $design "unit renaming",
	/ "improvoed windows painting efficiency",
	* "buffered coordinate delayed in painting dragged control" $since b169
),

b201
(
	/ "focused button style",
	+ "key helding response" @ "class %ShlReader",
	+ "GDI API %BlitTo",
	/ "widgets drawing",
	/ $design "GDI API %CopyTo",
	+ "controls: panel",
	/ $design "YCLib" $=
	(
		+ "partial version checking for compiler and library implementation",
		+ "minor macros" @ "library YCLib",
	),
	+ "type conversion helper template",
	+ $design "implicit member overloading by interface parameter with type \
		%IControl and %IWidget" @ "container classes"
),

b200
(
	* "direct drawing test",
	/ "test UI view",
	* "%KeyHeld response" $since b199,
	+ "direct desktop drawing for %ShlSetting"
),

b199
(
	+ "event routing for %KeyUp, %KeyDown and %KeyHeld",
	* "event behavior with optimizing" $since b195,
	+ "keypad shortcut for file selecter",
	+ $design "returning number of called handles in event calling"
),

b198
(
	* "font caching without default font file load successfully",
	+ "showing and hiding windows",
	* "%ListBox scroll bar length",
	* "handle constructors",
	/ $design "shells for DS" $=
	(
		- "class %ShlGUI" @ "unit Shell_DS"
	),
	/ $design "using pointers" ~ "references in parameters \
		of container methods",
	/ "simplified GUI shell" $=
	(
		/ "input points matching",
		- "windows list"
	),
	/ $design "simplified destructors",
	/ "simplified window drawing",
	+ "desktop capability of non-control widget container",
	- "contianer pointer parameter" @ "constructor widgets",
	/ "desktops as window" @ "shells"
),

b197
(
	* "label alignment",
	+ "%std::string based font cache"
),

b196
(
	+ "controls: checkbox",
	* "platform color type",
	+ "horizontal text alignment" @ "class %MLabel"
),

b195
(
	* $design "makefiles",
	+ "dependency events",
	+ "simple events routing"
),

b194
(
	/ "global architecture",
	/ "GUI shell class architecture" @ "unit %YGUI",
	/ "default control event handlers implementation",
	/ "implementation" @ "unit YInitialization",
	/ "controls always visual",
	- "class %IVisualControl",
	- "class %VisualControl",
	/ "container focus" @ "class %(AUIBoxControl, AWindow)"
),

b193
(
	/ "scroll controls implementation",
	/ "core utilities"
	/ "empty event arguments" $=
	(
		- "empty constant of event arguments",
		+ "inline function template GetStaticRef",
		/ "all event empty arguments" ^ "GetStaticRef"
	),
	/ "scroll controls and listbox implementation"
),

b192
(
	+ "empty constant of event arguments",
	/ "event interfaces",
	/ $design "header including",
	+ "unit %YUIContainerEx" @ "directory Shell",
	+ "class %AUIBoxControl" @ "unit YUIContainerEx",
	+ "controls: class %YScrollableContainer" @ "unit YGUIComponent",
	/ $design "function %FetchWindowPtr as non-inline" @ "unit YUIContainer",
	/ "class %IWindow as non-virtual inheritance" @ "class AWindow"
		@ "class YWindow",
	/ "arm9 makefile",
	/ "scroll controls implementation"
),

b191
(
	/ "listbox APIs implementation",
	/ "class template %GSequenceViewer" $=
	(
		* $design "declaration of member function %Contains" @ $since b171,
		/ "interfaces"
	),
	* "implememtation of function %GetTouchedVisualControlPtr" @ "unit %YGUI"
		$since b167,
	/ "simplified focus implementation"
),

b190
(
	* "strict ISO C++2003 code compatibility" $=
	(
		/ "fixed macros" ~ "variadic macros"
	),
	/ "text region and renderers APIs implementation",
	/ "widget, label and listbox APIs implementation"
),

b189
(
	+ "class %pair_iterator" @ "YCLib",
	/ "GDI blit and transformer implementations"
),

b188
(
	/ "shells test example",
	/ "GDI blit implementations",
	- "DLDI patch commands" @ "makefile",
	* "invalid listbox input when the list not beginning from the top"
		$since b171
),

b187
(
	/ "solution configuration" $=
	(
		"header files and source files put in seperated directories"
	),
	^ "-O3 & arm specified options compiled library libloki.a"
		@ "project YSTest",
	+ "minor templates" @ "YCLib",
	/ "GDI blit implementations"
),

b186
(
	/ "solution configuration",
	* "makefiles",
	/ $design "file including",
	* "some wrong Doxygen remarks"
),

b185
(
	^ "updated freetype 2.4.4" ~ "freetype 2.3.12",
	- "DMA implentation" $=
	(
		/ "remarked",
		^ "normal %memcpy and %memset"
	)
	/ "desktop window refreshing logic",
	/ $design "MDualScreenReader constructor" ^ "%ynew" ~ "%new"
),

b184
(
	* $design "some spell errors",
	/ "DMA implementation"
),

b170_b183
(
	+ "controls: tracks",
	+ "controls: scroll bars",
	+ "controls: scrollable container",
	+ "controls: listbox"
),

b159_b169
(
	+ "controls: buttons" @ "class (%YThumb, %YButton)",
	+ "controls: listbox class",
	+ "event mapping"
),

b134_b158
(
	+ $design "core utility templates",
	/ "smart pointers",
	+ "GUI focus",
	/ "shells test example" $=
	(
		+ "some shells"
	)
	+ "controls",
	+ "virtual inheritance" @ "control classes",
	+ "exceptions",
	+ "debug macros & functions",
	+ "color type",
	+ "class template %general_cast",
	+ "timer class"
),

b133
(
	/ $design "simplified primary types definition",
	/ "event interfaces",
	+ "several except classes",
	/ "exception handling" @ "some functions"
),

b132
(
	/ "log interfaces",
	+ "backup message queue object" @ "the application class",
	/ "automatically clearing screen windows when deactivating class %ShlGUI",
	/ "shells test example" $=
	(
		+ "background image indexing"
	)
),

b1_b131
(
	+ "initial test with PALib & libnds",
	+ "shell classes",
	+ "CHRLib: character set management",
	+ "fonts management using freetype 2",
	+ "YCLib: platform independence",
	+ "the application class",
	+ "basic objects & counting",
	+ "global objects",
	+ "string class",
	+ "file classes",
	+ "dual screen text file reader framework",
	+ "output devices & desktops",
	+ "messaging",
	+ "program initialization",
	+ "simple GUI: widgets & windows",
	+ "simple GDI",
	+ "base abbreviation macros",
	+ "events",
	+ "smart pointers using Loki",
	+ "Anti-Grain Geometry test",
	+ "simple resource classes"
);


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
			msg.GetPriority(), msg.GetObjectID(), msg.GetWParam(),
			msg.GetLParam());
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

