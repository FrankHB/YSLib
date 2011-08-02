// v0.3229; *build 228 rev 72;
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
\u Panel
(
	\cl Panel
)
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
),
\u Form
(
	\cl Form;
);


$DONE:
r1:
/= test 0;

r2:
/= \rem @ \in IWidget;

r3:
/ \inc \h YPanel -> (YControl & YResource & YUIContainer) @ \h YWindow;
/ \u YPanel["ypanel.h", "ypanel.cpp"] => Panel["panel.h", "panel.cpp"]
	@ \dir UI;

r4:
/= test 1 ^ \conf release;

r5:
*= \rem @ \f (FetchParentWindowPtr & FetchWindowPtr & FetchDesktopPtr)
	@ \h YUIContainer $since b227;

r6:
/ \impl @ \f InvalidateCascade @ \impl \u YWidget !^ FetchWidgetNodePtr;

r7:
* \impl @ \f InvalidateCascade @ \impl \u YWidget $since r6;

r8:
+ \amf void UpdateInvalidation(Rect&) @ \in IWidget;
+ \mf ImplI1(IWidget) void UpdateInvalidation(Rect&) @ \cl Widget;
+ \mf ImplI1(IWindow) void UpdateInvalidation(Rect&) @ \cl AWindow;
/ \simp \impl @ \f InvalidateCascade @ \impl \u YWidget ^ \mf UpdateInvalidate
	~ \f (CommitInvalidatedAreaTo & FetchInvalidatedArea);

r9:
/ @ \u YWindow:
	/ \f (SetInvalidationOf & SetInvalidationToParent & CommitInvalidatedAreaTo)
		-> \mf @ \cl AWindow;
	+ \amf void SetInvalidationToParent() @ \in IWindow;
	+ ImplI(IWindow) @ \mf SetInvalidationToParent @ \cl AWindow;
	/ \tr \impl @ \impl \u;
	* spell error $since b:
		/ \a SetInvalidateonToParent => SetInvalidationToParent;
/ \tr \impl @ \impl \u (Shells & YDesktop);
/ \a SetInvalidationOf => SetInvalidation;
/ \simp \impl @ \f InvalidateCascade @ \impl \u YWidget;

r10:
/= test 2 ^ \conf release;

r11-r14:
/= test 3;

r15:
/ @ \cl AWindow:
	/ \mf CommitInvalidatedAreaTo => CommitInvalidation;
	/ \tr \impl;
	
r16:
/ @ \u YWindow:
	+ \amf void CommitInvalidation(const Rect&) @ \in IWindow;
	/ @ \cl AWindow:
		/ \mf !\vt CommitInvalidation(const Rect&) @ \cl
			-> ImplI1(IWindow) CommitInvalidation(const Rect&);
		/ \simp \impl @ \mf SetInvalidationToParent;

r17:
/= test 4;

r18:
/ @ \u YWindow:
	+ \cl BufferedWidgetRenderer;
	/ @ \cl MWindow:
		/ \m mutable Rect rInvalidated
			-> \m unique_ptr<BufferedWidgetRenderer> pRenderer;
		/ \tr @ \impl @ \ctor;
	/ @ \cl AWindow:
		/ \tr \impl @ \ctor;
		/ \tr \impl @ \mf GetInvalidatedAreaRef ^ \mac DefGetterMember
			~ \mac DefGetter;
		/ \mf void SetInvalidationToParent()
			-> !\m \f void SetInvalidationToParent(IWindow&);
		/ \tr \impl @ \mf \i (FetchInvalidatedArea & ResetInvalidatedAreaOf);
	- \amf void SetInvalidationToParent() @ \in IWindow;
	/ \tr \impl @ \f (Show & Hide);

r19:
/ @ \u YWindow:
	/ @ \in IWindow:
		- \amf void CommitInvalidation(const Rect&);
		+ \amf BufferedWidgetRenderer& GetRenderer() const;
		- \amf Rect& GetInvalidatedAreaRef() const;
	+ \mf \vt void CommitInvalidation(const Rect&) @ \cl BufferedWidgetRenderer;
	/ @ \cl AWindow:
		- \mf \vt CommitInvalidation;
		/ \tr \impl @ \mf SetInvalidation ^ GetRenderer;
		+ \mf ImplI1(IWindow) BufferedWidgetRenderer& GetRenderer() const;
		- \mf InvalidatedAreaRef;
	/ \tr \impl @ \f SetInvalidationToParent;
	/ \tr \impl @ \f UpdateInvalidation;
	/ \tr \impl @ \f DrawContents;

r20:
/ @ \u YWindow:
	- \f \i ResetInvalidation;
	/ @ \cl BufferedWidgetRenderer:
		+ \mf void ResetInvalidation;
		/ \tr \impl @ \mf Validate;

r21:
/ @ \u YWindow:
	/ \mf ResetInvalidation => ClearInvalidation @ \cl BufferedWidgetRenderer;
	/ \tr \impl @ \mf AWindow::Validate;

r22:
/ @ \u YWindow:
	/ \m protected Drawing::BitmapBuffer Buffer & \mf \i (GetContext
		& SetBufferSize) >> \cl AWindow ~ \cl Frame;
	/ \tr \impl @ \ctor @ \cl (AWindow & AFrame);
	/ @ \cl AWindow:
		- \tr \amf SetBufferSize;
		/= \tr \a ImplI1(AWindow) -> ImplI1(IWindow);
		- \vt @ \mf SetBufferSize;

r23:
/ @ \u YWindow:
	/ \m protected Drawing::BitmapBuffer Buffer & \mf \i SetBufferSize
		>> \cl MWindow ~ \cl AWindow;
	/ \tr @ \ctor @ \cl (MWindow & AWindow);
	* \rem @ \ctor @ \cl AWindow $since b175;

r24:
/ @ \u YWindow:
	/ @ \cl MWindow:
		- \mf SetBufferSize;
		/ \m protected Drawing::BitmapBuffer Buffer 
			-> public @ \cl BufferedWidgetRenderer;
		/ \tr \impl @ \ctor;
	+ \mf \vt void SetSize(const Size&) @ \cl BufferedWidgetRenderer;
	/ \tr \impl @ \mf (GetContext & SetSize) @ \cl AWindow;

r25:
/ @ \h YWindow:
	/ \mf Rect& GetInvalidatedAreaRef() const @ \cl BufferedWidgetRenderer
		-> const Rect& GetInvalidatedArea() const ^ \mac DefGetter;
	/ \tr @ \f \i FetchInvalidatedArea;

r26:
/ @ \u YWindow:
	+ \mf \vt const Graphics& GetContext() const ^ \mac DefGetter
		@ \cl BufferedWidgetRenderer;
	/ \tr \simp \impl @ \mf GetContext @ \cl AWindow ^ \mac DefGetterMember
		~ \mac DefGetter;

r27:
/ @ \u YWindow:
	/ (protected \mf DrawBackgroundImage & public \mf GetBackgroundPtr)
		@ \cl AWindow >> \cl MWindow;
	- \mf (BeFilledWith & ClearBackground) @ \cl AWindow;
	/ \tr \impl @ \mf DrawBackgroundImage @ \cl MWindow;

r28:
/ @ \u YWindow:
	/ @ \cl MWindow:
		+ \mf GetRenderer ^ \mac DefGetter;
		/ \ac @ \m pRenderer -> private ~ protected;
	/ @ \cl AWindow:
		/ \tr \impl @ \mf SetSize
		/ \tr \impl @ \mf GetRenderer ^ \mac DefGetterBase ~ \mac DefGetter;
		/ \tr \impl @ \mf GetContext;
		- \mf GetBackgroundImagePtr;
		+ using MWindow::GetBackgroundImagePtr;
		+ using MWindow::GetBackgroundPtr;

r29:
/ @ \u YWindow:
	+ protected \mf void FillInvalidation(Color) @ \cl MWindow;
	/ \simp \impl @ \mf DrawRaw ^ \mf FillInvalidation @ \cl AWindow;
	+ \mf \vt bool RequiresRefresh() const @ \cl BufferedWidgetRenderer;
	- \f bool RequiresRefresh(const IWindow&);
	/ \tr \impl @ \mf (Update & Validate) @ \cl AWindow;
	/ \tr \impl @ Frame::DrawContents;
/ \tr \impl @ \mf Desktop::Update;

r30-r31:
/= test 5;

r32:
/ @ \u YWindow:
	+ \mf \vt void GetInvalidatedArea(Rect&) const @ \cl BufferedWidgetRenderer;
	- \f FetchInvalidatedArea;
	/ \tr \impl @ \mf UpdateInvalidation @ \cl AWindow;
	/ \tr \impl @ \mf Frame::DrawContents;
/ \tr \impl @ \impl \u Shells;

r33:
/ @ \h YWindow:
	- \amf GetContext @ \in IWindow;
	+ \f \i const Graphics& GetContextFrom(const IWindow&);
	- \mf GetContext @ \cl AWindow;
/ \tr \impl @ \impl \u (YWindow & YDesktop & YGUI & YStyle & Shells);

r34:
/= test 6 ^ \conf release;

r35-r36:
/ @ \h YWidget:
	+ \f Rect Render(IWidget&, const Graphics&, const Point&, const Rect&);
	/ \f RefreshChild -> RenderChile;
/ \impl @ \mf Frame::DrawContents ^ \f Render ~ \mf Refresh;
/ \tr \impl @ \impl \u (Scroll & ListBox);

r37:
/ \cl BufferedWidgetRenderer @ \ns Components::Forms @ \u YWindow
	>> \ns Components @ \u YWidget;
+ \cl WidgetRenderer @ \ns Components @ \u YWidget;
+ \inc \h YGDI @ \h YWidget;

r38:
/= test 7 ^ \conf release;

r39:
+ \mf \vt void FillInvalidation(Color) @ \cl (WidgetRenderer
	& BufferedWidgetRenderer) @ \u YWidget;
/ @ \u YWindow:
	- \tr \mf FillInvalidation @ \cl MWindow;
	/ \tr \impl @ \mf AWindow::DrawRaw;

r40:
/ @ \h YWindow:
	/ @ \cl MWindow:
		/ \m private unique_ptr<BufferedWidgetRenderer> pRenderer 
			-> unique_ptr<WidgetRenderer> pRenderer;
		/ \mf DefGetter(BufferedWidgetRenderer&, Renderer, *pRenderer)
			-> DefGetter(WidgetRenderer&, Renderer, *pRenderer);
		/ \tr \impl @ \mf DrawBackgroundImage;
	/ \amf BufferedWidgetRenderer& GetRenderer() const @ \in IWindow
		-> WidgetRenderer& GetRenderer() const;
	/ \tr \ret \tp @ \mf GetRenderer @ \cl AWindow;
/ @ \h YWidget;
	+ (\vt \dtor & \inh noncopyable) @ \cl WidgetRenderer;
	+ \inh WidgetRenderer @ \cl BufferedWidgetRenderer;

r41:
/= test 8 ^ \conf release;

r42:
/ @ \u YWindow:
	/ (\m private unique_ptr<WidgetRenderer> pRenderer & \mf GetRenderer)
		@ MWindow >> \cl Widget @ \u YWidget;
	/ \amf GetRenderer @ \in IWindow >> \in IWidget @ \h YWidget;
	/ \tr \impl @ \ctor @ \cl (MWindow & AWindow);
	/ \mf DrawBackgroundImage @ \cl MWindow >> \cl AWindow;
/ @ \cl Widget:
	+ \mf void SetRenderer(unique_ptr<WidgetRenderer>&&);
	+ 'ImplI1(IWidget)' @ \mf !\vt GetRenderer;
	/ \tr \impl @ \ctor;

r43:
/= test 9 ^ \conf release;

r44:
/ \f const Graphics& GetContextFrom(const IWindow&) @ \ns Forms @ \h YWindow
	>> \f const Graphics& FetchContext(const IWidget&) @ \ns Components
	@ \h YWidget;
/ \tr \a 'Forms::GetContextFrom' -> 'Widgets::FetchContext';

r45:
/ @ \u YWidget:
	/ \impl @ \f InvalidateCascade @ \impl \u;
	- \amf UpdateInvalidation @ \in IWidget;
	- \mf UpdateInvalidation @ \cl Widget;
- \mf UpdateInvalidation @ \cl AWindow;

r46:
/ \a 'Components::Widgets::FetchContext' -> 'FetchContext';
/ \a 'Widgets::FetchContext' -> 'FetchContext';
/= \simp \impl @ (\ctor & \dtor) @ \cl Frame ^ "auto" ~ "Desktop*";

r47:
/= test 10 ^ \conf release;

r48-52:
/= test 11;

r53:
/ @ \u YWidget:
	+ \em \mf \vt void UpdateTo(const Graphics&, const Point&) const
		@ \cl WidgetRenderer;
	+ \mf \vt void UpdateTo(const Graphics&, const Point&) const
		@ \cl BufferedWidgetRenderer;
/ \impl @ \mf AWindow::UpdateTo;

r54:
/ @ \cl AWindow:
	- \mf UpdateToDesktop;
	- \amf Update @ \in IWindow;
	/= 'ImplI1' -> 'virtual' @ \mf Update @ \cl AWindow;

r55:
/ @ \cl AWindow:
	/ \mf !\vt UpdateTo -> \f void Update(const IWidget&, const Graphics&,
		const Point& = Point::Zero) @ \ns Widgets @ \u YWidget;
	/ \mg \mf UpdateToWindow -> \mf Update;

r56:
/= test 12 ^ \conf release;

r57-r61:
/ @ \impl \u YWindow:
	/ \impl @ \mf AWindow::Refresh;
	/ \simp \impl @ \mf Frame::DrawContent;

r62:
/ \impl @ \f Render @ \impl \u YWidget;

r63:
* \impl @ \f Render @ \impl \u YWidget $since r62;

r64:
+ \f void Validate(IWidget&) @ \u YWidget;
- \mf Validate @ \cl AWindow;
/ \tr \impl @ \impl \u (Shell_DS & Shells);

r65:
* \impl @ \f Validate @ \impl \u YWidget $since r64;

r66-r68:
/= test 13;

r69:
/ \simp \impl @ \mf Frame::DrawContents;

r70:
/= test 14 ^ \conf release;

r71:
/ \mf DrawRaw \mg -> \mf Refresh @ \cl AWindow;

r72:
/= test 15 ^ \conf release;


$DOING:

$relative_process:
2011-08-02:
-21.2d;
//Mercurial rev1-rev99: r4958;

/ ...


$NEXT_TODO:
b229-b256:
+ TextList invalidation support;
* non-ASCII character path error in FAT16;
/ fully \impl \u DSReader;
	* moved text after setting lnGap;

b257-b768:
+ key accelerators;
+ \impl styles @ widgets;
/ fully \cl Path;
/ \impl 'real' RTC;
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
	/ "GUI" $=
	(
		+ "runtime buffering control" $=
		(
			+ "renderer class (%BufferedWidgetRenderer, %WidgetRenderer)",
			/ "window buffering control moved to renderer" ~ "class %Frame"
		),
		/ "buffering-concerned methods" @ "class %IWindow"
			>> "renderer classes and class %IWidget"
	)
),

b227
(
	* "invalidation area error" $since b226 $=
	(
		* "wrong result when one argument actually contained by another"
			@ "intersection calculation for class %Rect" $since b226
	),
	/ "widget rendering efficiency improvement" $=
	(
		/ "simplified background rendering" @ "class %ScrollableContainer",

	),
	* $design "guard macro" @ "header UIContainerEx" $since b203,
	/ "simplified UI class inheritance" $=
	(
		/ $design "implementation" ^ "class %IWidget" ~ "class %IUIBox",
		- "class %IUIBox",
		- "class %IUIContainer",
		/ $design "implementation" ^ "class %IControl" ~ "class %IPanel",
		- "class %IPanel"
	),
	* "strict ISO C++2003 code compatibility" $since b190 $=
	(
		^ "fixed macros" ~ "variadic macros" @ "header (YPanel, YWindow)"
	),
	/ "simplified widget fetcher and locating interfaces"
),

b226
(
	/ "widget rendering efficiency improvement" $=
	(
		+ "return value of actually drew area for member function %Refresh"
			@ "widgets",
		/ "refreshing of windows" $=
		(
			- "unnecessary drawing of overlaid windows"
		),
		+ "partial refreshing optimization" @ "class %Widget"
	),
	* "wrong result when height of arguments are equal"
		@ "intersection calculation for class %Rect" $since b225,
	+ "child widget intersection comfirming"
),

b225
(
	/ "windows partial invalidation support" $=
	(
		/ "windows partial invalidation committing for class %IWindow"
			~ "class %AWindow",
		+ "widget validation abstract member function" @ "class %IWidget",
		/ "widget invalidation interface as non-member function"
			~ ("abstract member function" @ "class %IWindow")
	),
	+ $design "assignment operators" @ "class %Rect",
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
	+ $design "WinGDB settings" @ "main project file",
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
		^ "updated freetype 2.4.5" ~ "freetype 2.4.4",
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
	+ $design "cscope files"
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
	* "strict ISO C++2003 code compatibility" $since b190 $=
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

