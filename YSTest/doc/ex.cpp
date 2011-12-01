/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ex.cpp
\ingroup Documentation
\brief 设计规则指定和附加说明 - 存档与临时文件。
\version r3397; *build 265 rev 63;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-12-02 05:14:30 +0800;
\par 修改时间:
	2011-12-01 11:04 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	Documentation::Designation;
*/

/*
	NOTE: this is NOT a source file. The base name of this file is named
	intentionally to activate syntax highlight and other functions in an IDE
	(specially, Visual Studio). The content is mainly about log of the
	process of development, conformed with a set of virtual set of syntax rules
	which constitutes an informal pseudo-code-based language.
	This file shall be safe of deletion when building the projects.
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

$match_each m;

__unfold.(m($ctor, $dtor)) $= __pstate.behavior.ignore
	$= m($true, $false);
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
// $evaluating.structure;
// semi-colon is used for remarking eval-order-sensitive expressions;
, ::= non-sequenced seperater
... ::= ellipse
; ::= sequenced seperater(statement termination)
// $evaluating.content;
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
\dat ::= data
\de ::= default/defaulted
\def ::= definitions
\del ::= deleted/deletion
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
\loc ::= local
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
\reg ::= regular
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
	\cl ValueObject,
	\clt GDependency
),
\u YFileSystem
(
	\cl Path
),
\u YShell
(
	\cl Shell
),
\u YApplication
(
	\cl Log,
	\cl YApplication
),
\u YConsole
(
	\cl Console
),
\u YRender
(
	\cl WidgetRenderer,
	\cl BufferedWidgetRenderer
),
\h YWidgetView
(
	\cl Visual,
	\cl View
),
\h YWidgetEvent
(
	\cl BadEvent,
	\cl AController
),
\u YWidget
(
	\in IWidget,
	\cl Widget
),
\u YUIContainer
(
	\cl MUIContainer
),
\u YControl
(
	\cl Controller,
	\cl Control
),
\u YPanel
(
	\cl Panel,
),
\u YWindow
(
	\cl Window,
	\cl Frame
),
\u YGUI
(
	\cl GUIShell,
)
\u Label
(
	\cl MLabel,
	\cl Label,
	\cl MTextList
),
\u TextArea
(
	\cl TextArea,
	\cl BufferedTextArea
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
	\cl FileBox
),
\u Form
(
	\cl Form
),
\u YText
(
	\cl TextState,
	\cl EmptyTextRenderer,
	\cl ATextRenderer,
	\cl TextRegion
),
\u TextManager
(
	\cl TextFileBuffer
);


$DONE:
r1-r2:
/= test 1;

r3:
/ @ \u YWindow $=
(
	/ \mf AWindow::Refresh \mg -> Frame::Refresh;
	/ \mf \vt DrawContents @ \cl Frame \mg -> Refresh;
	- \tr protected \amf DrawContents @ \cl AWindow
);

r4:
/ @ \u YWindow $=
(
	/ \cl AWindow \mg -> AFrame;
	/ \cl AFrame => Window
);
/ \tr @ \h (YComponent, YMenu), \impl \u (Menu, YWidget);

r5:
/ \u Panel["panel.h", "panel.cpp"] @ \dir UI @ \lib YSLib
	=> YPanel["ypanel.h", "ypanel.cpp"];
/ \inc \h (YControl, YUIContainer) -> \h YPanel @ \h YWindow;
/ @ \cl Window $=
(
	/ \inh public Control, protected MBackground, protected MUIContainer
		-> \inh public Panel, protected MBackground;
	/ \tr \impl @ \ctor
),
/ \tr \h @ \h Build;

r6:
/= test 2 ^ \conf release;

r7-r11:
/= test 3;

r12:
/ \impl @ \mf Frame::Refresh;

r13:
/= test 4 ^ \conf release;

r14:
* \impl @ \mf Frame::Refresh $since r12;

r15-r18:
/ @ \u YRenderer $=
(
	/ \impl @ \f void Render(IWidget&, PaintContext&&);
	/ \impl @ \f void PaintChild(IWidget&, PaintEventArgs&&);
	- \f PaintIntersection
);

r19-r25:
/= test 5;

r26:
/ \impl @ \mf BufferedRenderer::Validate;
/ \simp \impl @ \mf Frame::Refresh;

r27:
/ \impl @ \mf Frame::Refresh;

r28-r31:
/= test 6;

r32:
/ @ \impl \u YRenderer $=
(
	/ \impl @ \mf Refresh @ \cl (Renderer, BufferedRenderer),
	/ \impl @ \f void Render(IWidget&, PaintContext&&)
);

r33-r36:
/= test 7;

r37:
/= test 8 ^ \conf release;

r38:
/ \impl @ \mf Frame::Refresh;

r39:
/ \mf DrawBackgroundImage @ \cl Window \mg -> \mf Refresh;

r40:
/ \impl @ \mf Frame::Refresh;

r41:
/ @ \u YRenderer $=
(
	- \mf BufferedRenderer::FillInvalidation;
	- \mf Renderer::FillInvalidation
);

r42:
/ \cl Frame @ \u YWindow \mg -> \cl Window;
/ \tr \a 'Frame' @ \u Form -> 'Window',
/ \tr @ \u Desktop,
/ \tr \simp @ \impl \u Shells;

r43:
/= test 9 ^ \conf release;

r44:
+ \ft<typename _tForward> _tForward stable_range_unique(_tForward, _tForward)
	@ \h Algorithm,
(
	+ \mf Rect PaintChildren(const PaintContext&) @ \cl MUIContainer;
	/ \simp \impl @ Frame::Refresh
);

r45:
/ \impl @ \mf Window::Refresh,
+ \mf Refresh @ \cl Panel;

r46:
- \mf Refresh @ \cl TextInfoPanel @ \u ShlReader;

r47:
/= test 10 ^ \conf release;

r48:
/ @ \u YText $=
(
	/ \f void RenderChar(ucs4_t, TextState&, const Graphics&, u8*)
		-> void RenderChar(ucs4_t, TextState&, const Graphics&,
		const Rect, u8*);
	/ @ \cl TextRenderer $=
	(
		+ \m Rect ClipArea;
		/ \tr \impl @ \ctor;
		+ \ctor TextRenderer(TextState&, const Graphics&),
		/ \impl @ \mf \op()
	),
	/ \tr \impl @ \mf TextRegion::\op()
);

r49:
/ @ \u Text $=
(
	+ \f void DrawClippedText(const Graphics&, TextState&, const String&,
		const Rect&);
	+ \f void DrawClippedText(const Graphics&, const Rect&, const Rect&,
		const String&, const Padding&, Color);
	/ \simp \impl @ \a \f DrawText#2
);

r50:
/ \impl @ \mf MLabel::PaintText ^ \f DrawClippedText ~ DrawText;

r51:
/ \mf \vt void PaintItem(const Graphics&, const Rect&, ListType::size_type)
	-> void PaintItem(const Graphics&, const Rect&, const Rect&,
	ListType::size_type) @ \cl (TextList, Menu) ^ DrawClippedText ~ DrawText;
/ \tr \impl @ \mf TextList::PaintItems;

r52:
/ \impl @ \mf TextList::PaintItems;

r53-r55:
/= test 11,
* \impl @ \f RenderChar $since r48;

r56:
/ @ \impl \u Button $=
(
	/ \f void RectDrawButton(const Graphics&, Point, Size, bool = false,
		bool = true) -> void RectDrawButton(const Graphics&, const Rect&,
			Point, Size, bool = false, bool = true);
	/ \impl @ \mf Thumb::Refresh
);

r57:
/= test 12 ^ \conf release;

r58:
* sender checking missing for event TouchMove @ \cl TextList $since b264 $=
(
	/ \impl @ \ctor
);
* wrong behavior for listbox on event TouchHeld when touching widget changed
	$since b219;

r59:
/ \impl @ \f Intersect @ \impl \u YGDIBase;

r60:
/ \a ^ @ vmax -> std::max,
/ \a ^ @ vmin -> std::min;
+ \tr using std::min, using std::max @ \ns YSLib @ \h YAdaptor,
- (vmin, vmax) @ \ns ystdex @ \lib YStandardEx,
/ \tr \simp @ \impl \u YGDIBase;

r61:
/= test 13 ^ \conf release;

r62:
- \vt @ \dtor @ \cl FontCache @ \u YFont

r63:
/= test 14 ^ \conf release;


$DOING:

$relative_process:
2011-12-01:
-16.1d;
//Mercurial rev1-rev136: r6441;

/ ...


$NEXT_TODO:
b266-b384:
/ fully \impl \u DSReader;
	* moved text after setting %lnGap;
/ partial invalidation support @ \f DrawRectRoundCorner;
+ partial invalidation support @ %(HexViewArea::Refresh);
+ dynamic character mapper loader for \u CharacterMapping;
+ 64-bit support for ystdex::fixed_point;

b385-b1089:
+ key accelerators;
+ abstraction of rectangular hit test;
/ fully \impl @ \cl Path;
+ clipping areas;
+ fully \impl styles @ widgets;
/ \impl 'real' RTC;
/ text alignment;
+ data configuragion;
+ GDI brushes;
+ dynamic widget prototypes;
+ \impl pictures loading;
/ improve efficiency @ \ft polymorphic_crosscast @ \h YCast;
+ correct DMA (copy & fill);
* platform-independence @ alpha blending:
	+ \impl general Blit algorithm;
+ shared property: additional;
/ user-defined bitmap buffer @ \cl Desktop;


$LOW_PRIOR_TODO:
b1090-b1800:
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
Use pre-refreshing to make font changing.

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
* "corrupted loading or fatal errors on loading like font file with embedded \
	bitmap glyph like simson.ttc" $since b185;
* "<cmath> cannot use 'std::*' names" @ "libstdc++ with g++4.6";


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
	+ "partial invalidation support for text rendering";
		// NOTE: it makes efficiency decreased obviously \
			for non-overlapped widgets.
	/ GUI $=
	(
		/ "window classes hierarchy" ^ "class %Panel",
		/ "refreshing algorithm" @ "class %Frame::Refresh" $=
		(
			+ "support for unbuffered windows",
			/ "minor efficiency improvement"
		),
		+ "member function %Panel::Refresh",
		+ "partial invalidation support" @ "class %(MLabel, TextList, Menu)",
		(
			* "sender checking missing for event %TouchMove" @ "class %TextList"
				$since b264;
			* wrong behavior for listbox on event %TouchHeld when touching \
				widget changed" $since b219
		)
	),
	"intersection algorithm improvement for non-trivial result"
),

b264
(
	/ "shells test example" $=
	(
		/ "format of time strings showed by infomation labels"
			@ "hexadecimal browser" ^ "custom functions" 
			~ "function %std::ctime",
		* "file infomation box cannot be shown" @ "text reader" $since b263
	),
	/ GUI $=
	(
		/ @ "class %ATrack"
		(
			* "scrolling overflow of value greater than about 0x1998"
				$since b260 $=
			(
				/ "value type" ^ "float" ~ "ystdex::fixed_point<u32, 16>"
			),
			(
				/ "value mapping for scrollable length" ~ "total value range";
				* "wrong value calculation when minimum thumb length reached"
					@ "class %ATrack" $since b193
			),
			/ "GUI shell" ^ "temporary argument" @ "event %Enter",
			(
				* "wrong touch coordinate for event %TouchHeld when touching \
					widget changed" $since b219;
				* "wrong behavior when the corresponding relative touch \
					coordinate component below zero on dragging"
					@ "class %ATrack" $since b219
			),
			/ $design "minor renderer interface"
		)
	)
),

b263
(
	/ @ "GUI" $=
	(
		* "event %Confirmed checking fail for items out of initial view scope"
			$since b262,
		/ $design "rearranged rendering and updating implementation"
	),
	* $design "minor unexpected name pollution" @ "header %algorithm.hpp"
		@ "library %YStandardEx" $since b254,
	/ @ "hexadecimal browser" @ "shells test example" $=
	(
		+ "infomation labels" @ "hexadecimal browser"
	)
),

b262
(
	/ @ "class %TextList" @ "GUI" $=
	(
		* "wrong bottom adjustment of alignment" $since b261;
		* "wrong behavior on page selection" $since b191
	)
),

b261
(
	/ @ "GUI" $=
	(
		/ $design "simplified interface" @ "class %ATrack";
		* "wrong alignment" @ "listbox when alignment is non-zero value \
			and scrolling down to end" $since b193
	)
),

b260
(
	/ @ "library %YBase" $=
	(
		+ $design "several operations for integer types",
		+ $design "reusable overloaded operators";
		+ "fixed-point arithmetic template %fixed_point"
	);
	/ @ "GUI" $=
	(
		* "improper underlying type of scroll event argument type" $since b201
	)
),

b259
(
	/ "GUI" $=
	(
		* "focusing pointer not removed when removing widgets"
			@ "class %(Panel, AFrame) $since b258;
		+ $design "subobject detaching of view class" @ "class %Widget",
		/ "simplified form class as typedef" ^ "class %Frame"
	);
	/ "shells test example" $=
	(
		* "wrong behavior after pressing down exit button" $since b258
	)
),

b258
(
	/ "GUI" $=
	(
		- "focus responder runtime substituting capability",
		+ "widget view class %WidgetView";
		/ "unified focus requesting interface"
		/ "focusing state stored" @ "class %WidgetView"
	)
),

b257
(
	/ "GUI" $=
	(
		/ $design "simplified entering/leaving event implementation",
		- $design "dependency events" @ "class %(ATrack, TextList)"
	),
	/ "shells test example" $=
	(
		/ "hexadecimal browser" $=,
		(
			* "value of vertical scroll bar" @ "class %HexViewArea \
				not reset when file reloaded" $since b254,
			+ $design "individual model class and view class"
		)
	),
	- $design suppout for language implementation without variadic macro;
	+ $design void expression macros for function implementation
),

b256
(
	/ "GUI" $=
	(
		* $design "undefined behavior when event arguments class not empty"
			$since b255
	),
	* "overloading error when using default template argument"
		@ "class template ExpandMemberFirstBinder" $since b171
	/ "event handling" $=
	(
		/ "first parameter for merged to second parameter",
	)
),

b255
(
	/ "shells test example" $=
	(
		/ "hexadecimal browser" $=,
		(
			* "displaying of tail bytes less than one line" $since b253,
			+ "horizontal alignment controlling in displaying"
		)
	),
	/ "GUI" $=
	(
		* "wrong value construction" @ "class %TouchEventArgs" $since b195
	)
),

b254
(
	/ $design "header dependencies",
	/ "shells test example" $=
	(
		/ "hexadecimal browser" $=,
		(
			+ "vertical scroll bar",
			+ "updating data for non-zero offset"
		)
	),
	/ "GUI" $=
	(
		+ $design "unsequenced evaluated expressions optimization"
			@ "widget class constructors" @ "directory %YSLib::UI"
	)
),

b253
(
	/ "shells test example" $=
	(
		+ "hexadecimal browser",
		+ $design "unsequenced evaluated expressions optimization"
			@ "unit Shells",
		+ "automatic desktop invalidation when checkbox unticked",

	),
	/ "GUI" $=
	(
		* "wrong control %OnLostFocus behavior" $since b240
	),
	^ "new character types" $=
	(
		^ "fundamental types %(char16_t, char32_t) ~ %(std::uint16_t,
			std::uint32_t) as basic types" @ "library %CHRLib",
		^ "literal syntax prefix u" ~ "macro _ustr"
	),
	* "size not refreshed when opening file excluded using constructor"
		@ "class File" $since $before '~b1x'(with timestamp 2009-12-01,
		$rev("yfile.cpp") = r221)
),

b252
(
	/ "libraries structure" $=
	(
		/ "project %YSLib" >> "%YFramework";
		/ "platform dependent library %YCLib" >> "%YFramework",
		/ "library %CHRLib" >> "%YFramework";
		/ "project %YCLib" >> "%YBase",
		/ "library %YStandardExtend" >> "%YStandardEx"
	);
	+ "POD type operations" @ "library %YStandardEx",
	/ "Doxygen file",
	+ $design "nested-use support" @ "macro %yunsequenced implementation",
	^ "updated devkitARM release 35" ~ "devkitARM release 34",
	* "implementation" @ "installation checking" $since b245
),

b251
(
	(
		+ $design "unsequenced evaluation macro %yunsequenced";
		+ $design "unsequenced evaluated expressions optimization" @ "library \
			%(CHRLib, YCLib, YSLib)"
	),
	- "buffered text blocks",
	* "wrong ending of text checking @ text buffer" $since b246,
	/ @ "library CHRLib" $=
	(
		/ "undereferencable conversion error treated as conversion faliure",
		/ "conversion functions returns non-zero if non-zero bytes read \
			when a conversion failure occured"
	)
),

b250
(
	+ $design "iterator checking operations" @ "library YCLib";
	/ @ "library CHRLib" $=
	(
		+ "valid conversion state support for %CharSet::UTF_8 \
			to %CharSet::UCS2 mapping function",
		+ "invalid conversion state support for %(CharSet::UTF_8, \
			CharSet::GBK) to %CharSet::UCS2 mapping function",
		+ "conversion state support for %(CharSet::UTF_16LE, \
			CharSet::UTF_16BE) to %CharSet::UCS2 mapping function"
	);
	* "EOF cannot be recognized by conversion routines" $since b248
),

b249
(
	/ @ "library CHRLib" $=
	(
		/ "encoding item names with MIB enums from IANA" @  $=
		(
			+ "more enum items",
			/ "aliases"
		),
		+ "conversion state formal parameter support for multibyte-to-Unicode \
			conversion functions";
		+ "valid conversion state support for %CharSet::GBK to %CharSet::UCS2 \
			mapping function"
	),
	/ $design "exception macros" $=
	(
		/ $design "exception specification macro" @ ("header config.h"
			@ "library YSLib::Adaptor") >> ("header ydef.h"
			@ "library YCLib::YStandardExtend");
		* $design "macro (ythrow, ynothrow) used without definition"
			@ "library %YStandardExtend" $since b209;
	)
),

b248
(
	/ $design "deleted copy constructor" @ "class %input_monomorphic_iterator \
		for safety",
	/ "GUI" $=
	(
		* "wrong return value" @ "member function %Refresh" @ "class %(Widget,
			Label, Progress, TextArea, Control, AWindow)" $since b226,
		* wrong overlapping condition @ (function %RenderChild,
			member function %Frame::DrawContents) $since b226;
		* "wrong invalidation on thumb of tracks after performing small \
			increase/decrease" $since b240
	)
),

r247
(
	/ "GUI" $=
	(
		+ $design "common widget member operation function %CheckWidget";
		/ $design "simplified scrolling widget implementation"
			^ "function %CheckWidget"
	),
	+ "statically-typed object proxy class %void_ref";
	+ "input iterator adaptor class %monomorphic_input_iterator";
	^ $design "simplified isomorphic character converter prototypes"
		^ "class %monomorphic_input_iterator"
),

b246
(
	+ $design ^ "selectable C++11 generalized constant expressions features",
	/ "file classes" $=
	(
		+ "mode selecting at opening files",
		/ "opening files" ^ "binary mode as default" @ "class %File";
		/ "initializing text files" ^ "text mode" @ "class %TextFile"
	),
	/ "simplified text file buffering" $=
	(
		- "automatic Unix style newline conversion at loading files",
	),
	/ "shells test example" $=
	(
		+ "file infomation panel"
	),
	* "character mapping for GBK" @ "library %CHRLib" $since b245
),

b245
(
	/ $design "interfaces" @ "library %CHRMap",
	/ $design "integer type definitions and string utilities"
		@ "library %(YCLib)",
	* "C-style string allocation" @ "library CHRMap" $since
		$before '~b10x'(with timestamp 2010-05-30, $rev("chrproc.cpp") = r1525),
	* $design "order of YCLib/YSLib in library linking command"
		@ "ARM9 makefile" $since b187;
	* "character mapping functionality implementation" @ "library %CHRLib"$=
	(
		* "unspecified subexpression evaluation order"
			@ "implementation unit chrmap.cpp",
		* "wrong behavior of mapping functions for platforms where char \
			is a signed type"
	) $since before '~b4x'(with timestamp 2009-11-22, $rev("chrproc.cpp")
		= r1319),
	+ "encoding conversion from UTF-8 to UCS-2" @ "library CHRLib";
	/ "shells test example" $=
	(
		* "path with non-ASCII characters cannot send to reader" $since b141
	)
),

b244
(
	/ "shells test example" $=
	(
		/ "reader panel functionality",
		/ "more text file extensions supported"
	),
	* "unsafe nullable dependency object" $since b242 $=
	(
		- "nullable features" @ "class template %GDependency"
	),
	* "path with non-ASCII characters wrongly displayed" $since b141,
	* "wrong value of unnamed namespace constant member FS_Parent_X"
		@ "file yfilesys.cpp" $since b156
),

b243
(
	/ $design "simplified class inheritance" @ "shell and application classes";
	- $design "inheritance GMCounter<Message>" @ "class %Message";
	- "class (YCountableObject; YObject)";
	/ "GUI" $=
	(
		+ "default GUI event %Paint for all widgets",
		+ "controller class %WidgetController for widgets \
			which %Paint is the only event permitted to call";
		/ "rendering of function %Render" ^ "mutable rvalue reference parameter \
			to store the result" ~ "returning";
		/ "constructor" @ "%WidgetControlle add event handler %Render";
		/ "rendering logic" @ "member function %Frame::DrawContents"
			^ "event %Paint" ~ "directly call function %Render";
		/ $design "simplfied updating" @ "renderer classes",
		/ $design "simplified refreshing of class %TextList"
	)
),

b242
(
	/ "GUI" $=
	(
		+ "class %PaintEventArgs",
		/ $design "simplified painting parameters" ^ "class %PaintEventArgs",
		- "automatic lifetime binding to containers" @ "class %Frame"
	),
	+ "nullable features" @ "class template %GDependency",
	/ "simplified event mapping" $=
	(
		+ "class template %GEventPointerWrapper",
		^ "non-member algorithm implementation" ~ ("member function"
			@ "class template %GEventMap");
		- "class template %GEventMap"
	),
	/ "shells test example" $=
	(
		/ $design ^ "lambda expressions" ~ "most of member function handlers"
	),
	/ "simplified screen object interface and implementation" $=
	(
		/ "class template %GBinaryGroup" ~ "class %(BinaryGroup, Point, Vec)",
		/ ("member function %(GetPoint, GetSize) return const references"
			~ "object type values") @ "class %Rect";
		/ ("platform independent const static member %Invalid"
			@ "(class template %GBinaryGroup, class %Size)")
			~ ("platform dependent %FullScreen" @ "class %(Point, Vec, Size)"
	)
),

b241
(
	/ "macros" @ "header ybase.h" $=
	(
		+ "new macros to simplify defaulted or deleted constructors \
			and destructors definition"
	),
	/ "GUI" $=
	(
		+ $design "%(MoveConstructible) support for all widget classes",
		+ $design "class %IController as controller interface type"
	),
	/ "event mapping interfaces" $=
	(
		+ "pointer type values confined as non-null",
		/ "member function %at returns %ItemType&" ~ "%PointerType&",
		/ $design "member function template %(GetEvent, DoEvent)"
			@ "class template %GEventMap" >> "unit %(YWidgetEvent, YControl) \
			as non-member"
	),
	* "contradict semantics of member template is_null" @ "header memory.h"
		@ "library YCLib" $since b222
),

b240
(
	/ "macros" @ "header ybase.h" $=
	(
		/ $design "simplified macro parameter names",
		+ "new macros for template declarations and forwarding constructor \
			templates",
		+ "macro %DefClone(_t, _n) for define member function for cloning"
	),
	+ "support for types are not %EqualityComparable" @ "class %ValueObject"
		^ "always-be-equal strategy",
	+ $design "copy constructor" @ "class template %GEventMap",
	+ $design "const static reference getter function %GetPrototype"
		@ "header ystatic.hpp",
	* $design "member function %Insert unavailable for %unique_ptr"
		@ "class template %GEventMap" $since b221,
	+ $design "%CopyConstructible, %MoveConstructible support"
		@ "class template %GEventMap",
	+ $design "%CopyConstructible and %MoveConstructible support"
		@ "class %(BitmapBuffer, BitmapBufferEx)";
	/ "GUI" $=
	(
		* @ "class template %GFocusResponser" $since b239 $=
		(
			+ "default constructor"
		),
		+ "virtual member function %Clone" @ "renderers, focus responsers \
			and widget classes",
		+ $design "prototype constructing of widgets" $=
		(
			+ $design "%(CopyConstructible, MoveConstructible) and clone \
				support" @ "class %(WidgetRenderer, BufferedWidgetRenderer)",
			+ $design "%(CopyConstructible, MoveConstructible) and clone \
				support" @ "class template %(GFocusResponser, \
				GCheckedFocusResponser)",
			+ $design "%(CopyConstructible, MoveConstructible) and clone \
				support" @ "class WidgetController";
			+ $design "%CopyConstructible and %MoveConstructible support"
				@ "class %(Widget; Control)",
			+ "class %ControlEventMap" @ "class %Control"
		)
	)
),

b239
(
	/ "GUI" $=
	(
		+ "dynamic focus responser switching",
		/ "simplified" @ "class %IWidget and derived classes",
		+ "common focus APIs shared by class %IWidget"
			~ "class %(Panel, AFrame, AUIBoxControl)" $=
		(
			+ "function %ClearFocusingPtrOf",
			+ "function %FetchFocusingPtr"
		)
	)
),

b238
(
	/ "GUI" $=
	(
		/ "simplified" @ "class %IWidget",
		/ $design "controller pointer" @ "class %Control" >> "class %Widget",
		/ "simplified" @ "class %Control",
		/ "simplified implementation" @ "focus responsers"
	)
),

b237
(
	/ "GUI" $=
	(
		/ "control functionality for widgets" ~ "for controls" $=
		(
			/ "support of events",
			/ "support of focusing",
			/ "enablity for all widgets"
		)
		/ "support of moving widget to the top" @ "class %MUIContainer"
			~ "class %Desktop";
		- "class %IControl",
		/ $design "member function %GetTopWidgetPtr" $=
		(
			- "containing test",
			+ "predicate parameter"
		),
		+ "automatic canceling pressing state when refreshing" @ "%class Thumb"
	)
),

b236
(
	^ "updated libnds 1.5.3 with default arm7 0.5.22"
		~ "libnds 1.5.4 with default arm 7 0.5.23",
	/ "GUI" $=
	(
		+ "controllers",
		/ "simplified %IControl interface",
		+ $design "rvalue forwarding and return value support"
			@ "function template %CallEvent",
		+ "limited focusing interfaces" @ "class %IWidget",
		/ "namespace %(Widgets, Controls, Forms) merged to parent \
			namespace %YSLib::Components",
		/ "simplified implementation and improved efficiency \
			in focus requesting" $=
		(
			^ $design "class %IControl" ~ "class %AFocusRequester"
				@ "member functions" @ "controls",
			- $design "class %AFocusRequester"
		)
	)
),

b235
(
	/ "shells test example" $=
	(
		/ "simplified background images loading",
		+ "reader panel" $=
		(
			+ "close button",
			+ "direct reading command controls"
		)
	),
	/ "GUI" $=
	(
		/ "efficiency improved" @ "member function %ATrack::Refresh" $=
		(
			- $design "redundant refreshing" $since b226
		),
		/ "GUI shell" $=
		(
			/ "a little improvement of efficiency"
				@ "entering and leaving events handling"
		),
		/ "pointer to containers as top elements returned accepted \
			and treated as null pointers",
		+ "label text display supporting for non-direct contained widgets",
		/ "invalidating canceled when enablity not changed"
			@ "function %SetEnableOf" @ "unit %YControl"
	)
),

b234
(
	/ "GUI" $=
	(
		/ "overwritable item enablity policy support" @ "class %TextList",
		/ "item enablity support" @ "class %Menu",
		* $design "recursively self call" @ "Control::Refresh"
			$since b230
	),
	/ "shells test example" $=
	(
		+ "checkbox to switch FPS visibility",
		- "checkbox to switch enablity of button",
		* "lost mapped global resource release call" $since b233,
		+ "empty panel test"
	)
),

b233
(
	/ "GUI" $=
	(
		/ "partial invalidation for hosted menus",
		/ "partial invalidation for member function %Desktop::MoveToTop",
		/ "partial invalidation for class %TextArea"
	),
	/ "shells test example" $=
	(
		/ "key input response" ^ "direct routing strategy"
			~ "all routing strategy",
		/ "menu of reader",
		+ "mapped global resource management"
	),
	/ $design "enhancement" @ "class %ValueObject"
	(
		+ "mutable access",
		+ "empty predicate",
		+ "constructing by pointers"
	)
),

b232
(
	/ "shells test example" $=
	(
		/ "background images" @ "class %ShlExplorer",
		/ $design "simplified reader refreshing implementation",
		- $design "member function ShlProc" @ "class (%ShlExplorer, ShlReader)",
	),
	/ "GUI" $=
	(
		* "invalid parent menu displayed on confirming submenus" $since b231,
		/ "menus functionality" $=
		(
			+ "hiding unrelated non-parent menus"
		),
		* "GUI class member function %ClearFocusingPtr implementation"
			$since b194 $=
		(
			@ "class AFrame" $since b194,
			@ "class Panel" $since b201,
			@ "class AUIBoxControl" $since b194
		)
	),
	/ $design "simplified shell classes" $=
	(
		/ "implementation" @ "shell message processing" @ "class %ShlDS"
			^ "default sending of %SM_PAINT message when processing %SM_INPUT"
	)
),

b231
(
	/ "GUI" $=
	(
		+ "updating with limited area" $=
		(
			+ "updating with invalidated area"
				@ "class %BufferedWidgetRenderer",
			+ "empty implemented updating with invalidated area"
				@ "class %WidgetRenderer",
			+ "updating with limited area" @ "function %Widgets::Update",
			* "wrong updating for (at least 3) buffered widgets overlapping"
				$since b228
		),
		+ "free function %SetInvalidationOf for class %IWidget"
			~ "member function AWindow::SetInvalidation"
	),
	/ $design "style of free setters/getters naming"
),

b230
(
	+ $design "move constructors and move assignment operators"
		@ "class template %(pseudo_iterator, pair_iterator)"
		@ "header YCLib::YStandardExtend::Iterator",
	/ $design "simplified GUI" $=
	(
		- "paint event for controls",
		- "DrawControl methods"
	),
	/ "shells test example" $=
	(
		/ "class %FPSCounter declared as external linkage in header",
		/ "updating FPS" $=
		(
			+ "when not getting input",
			/ $design "implementation" ^ "shell method %UpdateToScreen"
				~ "%OnActivated"
		)
	)
),

b229
(
	/ "GUI" $=
	(
		/ "simplified controls rendering implementation" $=
		(
			- "widget boundary drawing APIs",
			/ "control drawing" @ "class %(TextList, CheckBox)"
		),
		/ "simplified APIs" $=
		(
			- "locating functions concerned with interface %IWindow",
			- "interface %IWindow inheritance" @ "class %AWindow",
			- "interface %IWindow"
		),
		+ "dynamic renderer switching"
	)
	/ "shells test example" $=
	(
		+ "buffered renderer for listbox",
		^ "namespace %::YReader" ~ "namespace ::YSLib"
	),
	- "using namespace %platform" @ "namespace DS" @ "header yadaptor.h"
),

b228
(
	/ "GUI" $=
	(
		+ "runtime buffering control" $=
		(
			+ "renderer class %(BufferedWidgetRenderer, WidgetRenderer)",
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
	+ "child widget intersection confirming"
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
	/ "GUI" $=
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
		+ "support for synthetic emboldening and slanting of fonts",
		- "obsolete header /freetype/internal/pcftypes.h" $since b185
	),
	^ "updated libnds 1.5.1 with default arm7 0.5.21"
		~ "libnds 1.5.0 with default arm 7 0.5.20",
	^ "updated devkitARM release 34" ~ "devkitARM release 33"
),

b222
(
	* $design "minor faults detected by CppCheck" $=
	(
		/ "simplified implementation" @ "%YGUIShell::ResponseTouch" $since b195,
		* "implementation" @ "%ystdex::fexists" $since b151
	),
	+ "default GUI event %Paint",
	/ "controls drawing" ^ "event Paint" ~ "member function Draw"
		@ "class %(Frame, YGUIShell)",
	/ "GUI controls enhancement" $=
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
		/ "more efficient implementation"^ "%unique_ptr" ~ "%shared_ptr",
			// NOTE: old version of libstdc++ might fail in compiling
			// due to members with parameter of rvalue-reference type
			// are not supported.
	),
	* "lost move constructor" @ "class template %GMCounter" $since b210,
	/ "class %HDirectory" ^ "POSIX dirent API" ~ "libnds-specific API",
	+ $design "diagnostic pragma for GCC 4.6.0 and newer"
		@ "header type_op.hpp",
	* "class %HDirectory state not restored during operations" $since b175,
	/ "GUI" $=
	(
		/ "controls key-to-touch event mapping" @ "class %Control" $=
		(
			/ "custom handler" ^ "std::function" ~ "member function",
			+ "%KeyPress to %Click event mapping methods" @ "class %Control",
			* "calling mapped to disabled control" $since b217
		),
		/ "input response" $=
		(
			* "%YShell::ResponseTouch wrong behavior for container not \
				locating at (0, 0)" $since b213,
			* "%YShell::ResponceKey wrong behavior for tunnel event for \
				container" $since b199
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
		+ "multiple background switch test"
	),
	* "wrong default argument of desktop background color" $since b160,
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
		* "compile-error for non-trivially copy-assignable objects",
		/ "optimized implementation to avoid bloat code instantiated"
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
		* $design "operator new & delete comments" $since b203,
		/ "simplified Doxygen file excluded paths"
	),
	/ $design ^ "public %noncopyable inheritance"
		~ "all private %noncopyable inheritance",
	* "point containing test for zero width or height rectangle \
		turned out assertion failure" $since b204,
	* "constness of text width measuring" $since b197,
	/ "simplified focus operations interface" $=
	(
		- "unused parameter and argument"
	)
),

b213
(
	* $design "UI assertion strings",
	* "menu colors",
	* "touch event coordinate error" @ "container controls" $since b195,
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
	+ "Z order for widget overlapping" @ " UI containers",
	+ "menu laid at top level" ^ "Z order"
),

b211
(
	/ "implemented messages with general object content holder"
		^ "non-pointer member" ~ "%shared_ptr",
	+ "message content mapping",
	/ $design "messaging APIs" >> "unit YApplication" ~ "unit YShell",
	/ "shells test example" $=
	(
		/ "test menu fixed on the desktop"
	)
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
	+ $design "union %no_copy_t and union %any_pod_t for suppressing \
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
		+ "function %memcmp declaration" @ "namespace %ystdex"
	),
	* "strict ISO C++0x code compatibility" $=
	(
		* "implicit narrowing conversion(N3242 8.5.4/6)"
			@ "ISO C++0x(N3242 5.17/9)" ^ "explicit static_cast",
		/ "character types" @ "header platform.h"
	),
	/ "coding using limited C++0x features" $=
	(
		/ $design ^ "C++0x style nested template angle brackets",
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
	/ "improved windows painting efficiency",
	* "buffered coordinate delayed in painting dragged control" $since b169
),

b201
(
	/ "focused button style",
	+ "key holding response" @ "class %ShlReader",
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
	+ "event routing for %(KeyUp, KeyDown, KeyHeld)",
	* "event behavior with optimizing" $since b195,
	+ "keypad shortcut for file selector",
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
	/ "GUI" $=
	(
		/ $design "using pointers" ~ "references in parameters \
			of container methods",
		/ "simplified GUI shell" $=
		(
			/ "input points matching",
			- "windows list"
		)
	),
	/ $design "simplified destructors",
	/ "simplified window drawing",
	+ "desktop capability of non-control widget container",
	- "container pointer parameter" @ "constructor widgets",
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
	* "implementation of function %GetTouchedVisualControlPtr" @ "unit %YGUI"
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
		"header files and source files put in separated directories"
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
	- "DMA implantation" $=
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
	+ "controls: buttons" @ "class %(YThumb, YButton)",
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

	static Timers::Timer Timer(1250);

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
// ShlReader::OnActivated;

			FetchEvent<TouchDown>(mnu) += [&, this](TouchEventArgs&&){
				char strt[60];
				auto& dsk(this->GetDesktopDown());
				auto& g(dsk.GetScreen());
				using namespace ColorSpace;
				{
					const Rect r(0, 172, 72, 20);
					auto& evt(FetchEvent<TouchDown>(mnu));
					u32 t(evt.GetSize());

					siprintf(strt, "n=%u", t);
					FillRect(g, r, Blue);
					DrawText(g, r, strt, Padding(), White);
				}
				WaitForInput();
			};
			mhMain += *new Menu(Rect::Empty, GenerateList("a"), 1u);
			mhMain[1u] += make_pair(1u, &mhMain[2u]);
*/
/*
	auto cc(Reader.GetColor());
	Reader.SetColor(Color((cc & (15 << 5)) >> 2, (cc & 29) << 3,
		(cc&(31 << 10)) >> 7));
*/
/*
	YDebugBegin();
	iprintf("time : %u ticks\n", GetTicks());
	iprintf("Message : 0x%04X;\nPrior : 0x%02X;\nObj : %d\n"
		"W : %u;\nL : %lx;\n", msg.GetMessageID(), msg.GetPriority(),
		msg.GetObjectID(), msg.GetWParam(), msg.GetLParam());
	WaitForInput();
*/
/*
		InitYSConsole();
		iprintf("%d,(%d,%d)\n",msg.GetWParam(),
			msg.GetCursorLocation().X, msg.GetCursorLocation().Y);
*/

