// v0.3217; *Build 221 r50;
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
\u YLabel
(
	\cl MLabel,
	\cl Label,
	\cl MTextList
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
/= \rem @ \h (YWidget & YUIContainer & YGUI);
/ \impl @ \mf ShlExplorer::TFormExtra::OnClick_btnTestEx @ \impl \u Shells;
/ @ \cl String @ \h YString:
	+ \exp \de copy \ctor;
	+ \exp \de move \ctor;
	+ \exp \i @ \exp \de \ctor;
	+ \exp \de !\vt \dtor;
/ @ \cl Path @ \h YFileSystem:
	+ \exp \de copy \ctor;
	+ \exp \de move \ctor;
	+ \exp \i @ \exp \de (\ctor & !\vt \dtor);

r2:
/ @ \impl \u Shells:
	/ \cl TestObj @ \mf ShlExplorer::TFormExtra::OnClick_btnTestEx >> \un \ns;
	/ \a !\s \mf @ \cl TestObj -> !\i \f;

r3:
/ @ \cl ShlExplorer @ \u Shells:
	- \smf OnKeyPress_fbMain;
	/ \tr \impl @ \ctor ^ lambda \expr;

r4:
/ keypad operation test:
	/ @ \cl ShlExplorer @ \impl \u Shells:
		/ \impl @ \mf OnActivated & OnKeyDown_frm & OnKeyUp_frm;

r5:
/ @ \h YEvent:
	/ @ \clt GEventWrapper:
		/ \inh \in GIHEvent<YObject, EventArgs>
			-> \in GIHEvent<typename _tEvent::SenderType, EventArgs>;
		/ !(\exp \i) \mf size_t operator()(SenderType&, EventArgs&&) const
			-> \exp \i \mf size_t operator()(SenderType&, EventArgs&&) const;
	/ @ \clt GEventMap:
	/ \clt<typename _tEventSpace> GEventMap
		-> \clt<typename _tSender, typename _tEventSpace> GEventMap;
	/ @ \clt GEventMap:
		/ typedef GIHEvent<YObject, EventArgs> ItemType
			-> typedef GIHEvent<_tSender, EventArgs> ItemType;
		* \impl @ GetSerachResult ^ \exp \n ystdex;
/ \tr @ \h YControl:
	/ typedef Runtime::GEventMap<VisualEvent> VisualEventMapType
		-> typedef Runtime::GEventMap<IControl, VisualEvent> VisualEventMapType;

r6:
/= test 1 ^ \conf release;

r7-r8:
^ devkitARM release 33;
/ \tr \mac for GCC 4.6 @ \YStandardExtend;
/ @ \ns platform @ \u YCommon:
	/ \tr \a diropen => opendir;
	/ \tr \a dirclose => closedir;
	/ \tr \impl @ \f direxists;
	/ \tr \impl @ \f mkdirs;
	/ \tr @ \cl HDirectory:
		/ typedef ::DIR_ITER* IteratorType -> typedef ::DIR* IteratorType;
		/ \impl @ \mf Reset;
		/ \impl @ \mf operator++();
	- \tr \es @ \cl YApplication;
/ \tr header search directory setting @ \a projects;
* @ \clt GMCounter:
	+ \i \exp \de move \ctor;
/ !\rem GCC diagnostic pragma @ \h YCLib::YStandardExtend::TypeOperation;
/ @ \h YEvent:
	/ !\rem @ \i \exp \de \mf GHEvent& operator=(GHEvent&&) @ \clt GHEvent;
	/ @ \clt GEvent:
		- \mf \i GEvent& operator=(GEvent&&);
		/ !\rem @ \i \exp \de \mf GEvent& operator=(GEvent&&);
	^ unique_ptr ~ shared_ptr @ \clt GEventMap @ \h YEvent avoiding patching
		libstdc++ for known associative container operation bugs:
		/ typedef shared_ptr<ItemType> PointerType
			-> typedef unique_ptr<ItemType> PointerType;

r9:
/ @ \cl HDirectory @ \impl \u YCommon:
	* \impl @ \mf operator++ $since r7;
	* \impl @ \mf Close;

r10:
/= test 2 ^ \conf release;

r11-r12:
/= test 3;

r13:
/ \simp @ \mf @ \clt GEventMap @ \h YEvent
	/ \simp \impl ^ %auto & \mac PDefH1;
	/ !(\exp \i) private \mf GetSearchResult => \exp \i private \mf Search;

r14:
/ @ \cl Control:
	+ public \m std::function<IControl*(const Runtime::Key&)> BoundControlPtr;
	/ \tr \impl @ \mf (OnKeyUp_Bound_TouchUpAndLeave
		& OnKeyDown_Bound_EnterAndTouchDown)
		^ BoundControlPtr ~ GetBoundControlPtr;
	/ \tr \impl @ \ctor;

r15:
/ \simp @ \cl ShlExplorer @ \u Shells:
	+ private \mf IControl* GetBoundControlPtr(const Runtime::Key&);
	- private \mf (OnKeyUp_frm & OnKeyDown_frm & OnKeyPress_frm);
	/ \impl @ \mf (OnActivated & OnDeactivated);

r16:
/ \impl @ \mf (OnActivated & OnDeactivated) @ \cl ShlExplorer @ \u Shells;

r17:
/ @ \impl \u YControl:
	^ \ref \decl ^ auto& @ \a !\m \f;
	^ \obj \exc \tp TouchEventArgs \decl ^ auto @ \a \mf;

r18:
+ \f IControl* FetchBoundControlPtr(Control&, KeyEventArgs&&) @ \un ns
	@ \impl \u YControl;
/ @ \cl Control:
	+ \mf void OnKeyPress_Bound_Click(KeyEventArgs&&);
	/ \simp \impl @ \mf (OnKeyUp_Bound_TouchUpAndLeave
		& OnKeyDown_Bound_EnterAndTouchDown & OnKeyPress_Bound_Click)
		^ FetchBoundControlPtr ~ BoundControlPtr;
/ \impl @ \mf (OnActivated & OnDeactivated) @ \cl ShlExplorer @ \u Shells;

r18:
* @ \impl \u Control:
	/ \f FetchBoundControlPtr @ \un \ns -> FetchEnabledBoundControlPtr;
	/ \tr \impl @ \mf (OnKeyUp_Bound_TouchUpAndLeave
		& OnKeyDown_Bound_EnterAndTouchDown & OnKeyPress_Bound_Click)
		@ \cl Control;

r19:
/= test 4 ^ \conf release;

r20:
/ \a Key => KeyCode @ \ns (Runtime & platform);

r21:
/ @ \u YControl:
	/ @ \cl InputEventArgs:
		/ typedef Runtime::KeyCode Key;
		/ \m Key key -> Runtime::KeyCode Key;
		/ \tr @ \mf;
		/ \mf GetKey => GetKeyCode;
		/ \ctor InputEventArgs(const Runtime::KeyCode& = 0,
			RoutingStrategy = Direct);
			-> InputEventArgs(Runtime::KeyCode = 0, RoutingStrategy = Direct);
	/ @ \cl KeyEventArgs:
		/ typedef Key InputType -> typedef Runtime::KeyCode InputType;
/ @ \u YGlobal:
	/= \impl @ \f @ \un \ns @ \impl \u;
	/ @ \cl InputContent @ \ns Messaging:
		/ \m Runtime::KeysInfo Key => Keys;
/ \tr \impl @ \f ((ResponseInput @ \impl \u Shell_DS)
	& (FetchEnabledBoundControlPtr @ \un \ns @ \impl \u YControl) & ();
/ \tr \impl @ \ctor @ \cl TextList;
/ \tr \impl @ \impl \u Shells;

r22:
/ \a @ \ns YSLib::Runtime >> \ns YSLib;
- \ns Runtime;

r23:
* \impl @ \mf (OnActivated & OnDeactivated) @ \cl ShlExplorer @ \impl \u Shells
	$since r18;

r24:
/ @ \u Control:
	/ \mf void Control::OnKeyUp_Bound_TouchUpAndLeave(KeyEventArgs&&)
		-> !\m \f OnKey_Bound_TouchUpAndLeave(IControl&, KeyEventArgs&&);
	/ \mf void Control::OnKeyDown_Bound_EnterAndTouchDown(KeyEventArgs&&)
		-> !\m \f OnKey_Bound_EnterAndTouchDown(IControl&, KeyEventArgs&&);
	/ \mf void Control::OnKeyPress_Bound_Click(KeyEventArgs&&)
		-> !\m \f OnKey_Bound_Click(IControl&, KeyEventArgs&&);
	/= \impl @ 4 \f 'On*';
	/ \f IControl* FetchEnabledBoundControlPtr(Control&, KeyEventArgs&&)
		-> IControl* FetchEnabledBoundControlPtr(IControl&, KeyEventArgs&&)
		^ \eh (std::bad_function_call & std::bad_cast) @ \un \ns @ \impl \u;
/ \tr \impl @ \ctor @ \cl AScrollBar;
/ \tr \impl @ \mf (OnActivated & OnDeactivated) @ \cl ShlExplorer
	@ \impl \u Shells;

r25-r27:
/= test 5;

r28:
/= test 6 ^ \conf release;

r29-r31:
/= test 7;

r32:
* \impl @ \mf ResponseTouch @ \cl YGUIShell $since b213;

r33:
/= test 8;

r34:
/ \impl @ \mf ResponseKey & ResponseTouch @ \cl YGUIShell:
	* \impl @ \mf ResponseKey $since b199;

r35:
/= test 9 ^ \conf release;

r36:
/ \impl @ \ctor @ \cl Menu;

r37:
/ @ \u Menu:
	+ \f void LocateMenu(Menu&, const Menu&, Menu::IndexType);
	/ @ \cl Menu:
		/ \tp @ \mf \op[] -> IndexType ~ size_t;
		/ \simp \impl @ \ctor ^ \f LocateMenu;

r38:
/ \mf bool ClearSelected() @ \clt GSequenceViewer @ \h YViewer
	-> \exp \i \mf void ClearSelected();
/ \tr \simp \impl \mf ClearSelected @ \cl TextList ^ \mac ImplBodyMember0
	~ \mac ImplRet;

r39:
/ @ \clt GSequenceViewer @ \h YViewer:
	/ typedef _tDifference IndexType -> typedef _tSize IndexType;
	/ \tp \param typename _tIndex = ssize_t
		-> typename _tDifference = typename _tContainer::difference_type;
	+ typedef _tDifference DifferenceType;
	/ \tr \impl @ \mf (Contains & RestrictSelected & RestrictView);
	- \tr \mf IndexType GetRelativeIndex() -> DifferenceType GetOffset();
/ \tr @ \cl TextList;
	/ \tr \impl @ \ctor ^ auto;
	/ \tr \impl @ \mf PaintItems ^ auto;
	/ \tr \impl @ \mf ViewerType::IndexType CheckPoint(SPos, SPos);
/ \tr \simp \impl @ \ctor @ \cl Menu;

r40:
* \impl @ \mf TextList::PaintItems;
/ \simp @ \clt GSequenceViewer @ \h YViewer:
	- typedef _tSize IndexType;
	/ \a IndexType -> SizeType;
/ \tr @ \h ListBox & \u TextList;

r41-r43:
/ @ \clt GSequenceViewer @ \h YViewer:
	/ \tp @ \param @ 2 \mf 'Increase*' & 2 \mf 'Decrease*' -> DefferenceType
	`	~ SizeType;
	/ \mf (IncreaseHead & IncreaseSelected) -> !\exp \i \mf;

r44:
/= test 10 ^ \conf release;

r45:
/ @ \cl Menu:
	+ \mf bool ShowSub(IndexType, Widgets::ZOrderType = DefaultMenuZOrder);
	/ \simp \impl @ \ctor ^ ShowSub;

r46:
/ \impl @ \mf Menu::ShowSub;

r47:
/ \de viewer length -> 1 ~ 0 @ \impl @ \ctor @ \clt GSequenceViewer;

r48:
* \impl @ \mf Contains @ \clt GSequenceViewer $since b171;

r49:
/ @ \cl Menu:
	/ \mf bool ShowSub(IndexType, Widgets::ZOrderType = DefaultMenuZOrder)
		-> Menu* ShowSub(IndexType, Widgets::ZOrderType = DefaultMenuZOrder);
	/ \impl @ \ctor;

r50:
/= test 11 ^ \conf release;


$DOING:

$relative_process:
2011-06-27:
-18.6d;
//Mercurial rev1-rev91: r4522;

/ ...


$NEXT_TODO:
b222-b288:
* non-ASCII character path error in FAT16;
+ key accelerators;
/ fully \impl \u DSReader;
	* moved text after setting lnGap;

b289-b768:
+ \impl styles @ widgets;
/ fully \cl Path;
+ data configuragion;
/ impl 'real' RTC;
/ text alignment;
+ \impl pictures loading;
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


$KNOWN_ISSUE:
* fatal \err @ b16x:
[
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
	-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 02055838
	ftc_snode_weight
	ftcsbits.c:271
]
* fatal \err @ since b177 when opening closed lid @ real DS:
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

$now
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
		+ "key events for submenu showing and hiding" $=
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

