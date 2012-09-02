/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ex.cpp
\ingroup Documentation
\brief 设计规则指定和附加说明 - 存档与临时文件。
\version r4471; *build 335 rev 16;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-12-02 05:14:30 +0800;
\par 修改时间:
	2012-09-02 21:18 +0800;
\par 文本编码:
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
*/

#if 0

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

$match_each __m;

__unfold.(__m($ctor, $dtor)) $= __pstate.behavior.ignore
	$= __m($true, $false);
__unfold __iterators.for_labeled_paragraph
(
	$DONE,
	$DOING,
	$NEXT_TODO,
	$LOW_PRIOR_TODO,
	$KNOWN_ISSUE,
	$RESOLVED_ENVIRONMENT_ISSUE,
	$TODO
);

$script_preprocessor_escapse:
//$Record prefix and abbreviations:
$parser.$preprocessor.$define_schema "<statement> ::= $statement_in_literal";
// $evaluating.structure;
// semi-colon is used for remarking eval-order-sensitive expressions;
, ::= non-sequenced seperater
... ::= ellipse
; ::= sequenced seperater(statement termination)
// $evaluating.content;
= ::= equivalent/equal to
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
:= assignment as/is
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
\cmd ::= commands
\cond ::= conditions/conditional
\conf ::= configuration
\cp ::= copied
\ctor ::= constructors
\ctort ::= constuctor templates
\cv ::= const & volatile
\d ::= derived
\dat ::= data
\de ::= default/defaulted
\decl ::= declations
\def ::= definitions
\del ::= deleted/deletion
\dep ::= dependencies
\depr ::= deprecated
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
\fnl ::= final
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
\li ::= lists
\lib ::= library
\lit ::= literals
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
\pos ::= position
\post ::= postfix
\pre ::= prepared
\pref ::= prefix
\proj ::= projects
\pt ::= points
\ptr ::= pointers
\q ::= qualifiers
\rem ::= remarked/comments
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

$macro_platform_mapping:
\mac YCL_DS -> DS,
\mac YCL_MINGW32 -> MinGW32;

$using:
\u YObject
(
	(
		\cl ValueObject;
		\cl ValueNode,
	)
	\clt GDependency,
	\clt GRange
),
\u YGDIBase
(
	\clt GBinaryGroup;
	typedef Point, Vec,
	\cl Size;
	\cl Rect
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
	\cl Application
),
\u YConsole
(
	\cl Console
),
\u YRender
(
	\cl Renderer,
	\cl BufferedRenderer
),
\h YWidgetView
(
	\cl Visual,
	\cl View
),
\h YWidgetEvent
(
	\st UIEventArgs;
	\st RoutedEventArgs;
	\st InputEventArgs;
	(
		\st KeyEventArgs,
		\st TouchEventArgs
	),
	\clt GValueEventArgs,
	(
		\st PaintContext,
		\st PaintEventArgs
	),
	typedef \en VisualEvent,
	\stt EventTypeMapping,
	\st BadEvent,
	\cl AController;
	\cl WidgetController
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
	\cl Window
),
\u YStyle
(
	\cl Palette
),
\u YGUI
(
	\cl InputTimer,
	\cl GUIState
),
\u YBrush
(
	\cl SolidBrush,
	\cl ImageBrush,
	\cl BorderStyle,
	\cl BorderBrush
),
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
\u Button
(
	\cl Thumb,
	\cl Button
),
\u UIContainerEx
(
	\cl DialogBox,
	\cl DialogPanel
),
\u Selector
(
	\cl CheckBox;
	\cl CheckButton
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
	\cl ScrollEventArgs;
	\cl ATrack;
	\cl HorizontalTrack,
	\cl VerticalTrack;
	\cl AScrollBar;
	\cl HorizontalScrollBar,
	\cl VerticalScrollBar;
	\cl ScrollableContainer
),
\u ComboList
(
	\cl ListBox,
	\cl FileBox,
	\cl DropDownList
),
\u Form
(
	\cl Form
),
\u TextBase
(
	\cl PenStyle,
	\cl TextState
),
\u TextRenderer
(
	\cl EmptyTextRenderer,
	\clt GTextRendererBase,
	\cl TextRenderer
	\cl TextRegion
),
\u TextManager
(
	\cl TextFileBuffer
);


$DONE:
r1:
/ @ \proj YFramework $=
(
	* \inc \h form @ \lib Helper @ \proj YFramework $since b303
		$= (\a \inc \h @ (\a files \exc \impl \u ShellHelper ) ^ '""' ~ '<*>'),
	+ \lib NPL["NPL"]
		$= (+ \u LexicalAnalyzer["LexicalAnalyzer.h", "LexicalAnalyzer.cpp"]),
	/ \mac INCLUDED_CORE_YCOUNTER_H_ @ \h YCounter @ \lib YSLib
		=> YSL_INC_CORE_YCOUNTER_H_
),
(
	- \mac (YSL_, YSL) @ \h YBaseMacro,
	/ \a \mac 'YSL' -> 'YSLib',
	/ \a \mac 'YSL_' -> 'YSLib::'
);
/= test 1 @ platform MinGW32;

r2:
/ @ \cl LexicalAnalyzer @ \impl \u LexicalAnalyzer $=
(
	/ \impl @ \mf ParseByte, // Add '\f' support;
	/ \impl @ \mf HandleEscape, // Add ' ', '\f', '\r', '\t', '\v' support;
	+ \as @ \mf HandleEscape
);
/= test 2 @ platform MinGW32 ^ \conf release;

r3:
/ @ \cl LexicalAnalyzer @ \impl \u LexicalAnalyzer $=
(
	/ \impl @ \mf ParseByte, // Forbid non-literal escaping;
	/ \impl @ \mf HandleEscape, // Add '\a', '\b' support;
	+ \as @ \mf HandleEscape
);
/= test 3 @ platform MinGW32;

r4:
+ \f (CheckLiteral, MakeEscape) @ \u LexicalAnalyzer;
/= test 4 @ platform MinGW32 ^ \conf release;

r5:
+ (\inc \h <cctype>; \f yconstfn (IsGraphicalDelimeter, IsDelimeter))
	@ \h LexicalAnalyzer;
/= test 5 @ platform DS;

r6:
/= test 6 @ platform DS ^ \conf release;

r7:
/= test 7 @ platform MinGW32;

r8:
/= test 8 @ platform MinGW32 ^ \conf release;

r9:
+ \f (Decompose, Tokenize) @ \u LexicalAnalyzer;
/= test 9 @ platform MinGW32;

r10:
/ @ \lib NPL $=
(
	/ \u LexicalAnalyzer["LexicalAnalyzer.h", "LexicalAnalyzer.cpp"]
		=> \u Lexical["Lexical.h", "Lexical.cpp"],
	+ \u SContext["SContext.h", "SContext.cpp"];
);
/= test 10 @ platform MinGW32;

r11:
/ @ \u SContext $=
(
	/ \a \f Translate => \f Analyze,
	/ \a \f @ \un \ns @ \ns SContext @ \impl \u >> \f @ \ns SContext,
	/ \f Check => Validate
);
/= test 11 @ platform MinGW32 ^ \conf release;

r12:
+ \u Configuration["Configuration.h", "Configuration.cpp"] @ \lib NPL;
/= test 12 @ platform MinGW32;

r13:
/ \simp \impl @ (\ctor Session, \f SContext::(Validate, Reduce))
	@ \impl \u SContext;
/= test 13 @ platform MinGW32;

r14:
/= test 14 @ platform MinGW32 ^ \conf release;

r15:
/= test 15 @ platform DS;

r16:
/= test 16 @ platform DS ^ \conf release;


$DOING:

$relative_process:
2012-09-02 +0800:
-21.2d;
// Mercurial rev1-rev207: r9080;

/ ...


$NEXT_TODO:
b336-b348:
/ YReader $=
(
	/ \simp \impl @ \u (DSReader, ShlReader),
	+ bookmarks manager,
	+ configuration (serialization, unserialization)
);


$TODO:
b349-b400:
/ services $=
(
	+ \impl @ images loading
),
/ @ "GUI" $=
(
	+ viewer models,
	/ fully \impl @ \cl Form,
	+ icons,
	/ $low_prior more long list tests @ %DropDownList
),
/ $design $low_prior robustness and cleanness $=
(
	/ noncopyable GUIState,
	* (copy, move) @ \cl Menu,
	^ delegating \ctor as possible,
	/ strip away direct using @ Win32 types completely @ \h @ \lib YCLib,
	^ std::call_once to confirm thread-safe initialization
),
+ $design $low_prior helpers $=
(
	+ noinstance base class,
	+ nonmovable base class
),
/ $low_prior YReader $=
(
	+ settings manager,
	+ reading history,
	+ improved smooth scrolling with lower limit of scrolling cycle supported
),
/ project structure $=
(
	/ $low_prior build command @ \a \conf @ \proj YBase,
	+ Microsoft Windows(MinGW32) port with free hosted window size,
	/ improved tests and examples
);

b401-b895:
/ $low_prior @ \lib YCLib $=
(
	/ fully \impl @ memory mappaing APIs,
	+ block file loading APIs,
	+ shared memory APIs
),
/ @ CHRLib $=
(
	/ more accurate invalid conversion state handling,
	/ placeholders when character conversion failed @ string conversion,
	+ UTF-8 to GBK conversion
),
/ improving pedantic ISO C++ compatiblity $=
(
	/ \mac with no \arg
),
/ $design $low_prior robustness and cleanness $=
(
	/ @ \ns platform @ \u YCommon $=
	(
		/ \ac @ \inh touchPosition @ \cl CursorInfo,
		+ \exp \init @ \m @ \cl KeysInfo
	),
	+ recovery environment @ main \fn,
		// Try-catch, then relaunch the message loop.
	/ \impl @ \ctor \t fixed_point#2 @ \h Rational ^ 'std::llround'
		~ '::llround',
	/ \mf \vt Clone -> \amf @ \cl AController ^ g++ 4.7 later,
	+ 'yconstexpr' @ \s \m Graphics::Invalid,
	+ error code with necessary %thread_local @ \u YCommon,
	/ confirm correctness @ stat() @ Win32,
		// See comments @ src/fccache.c @ \proj fontconfig.
	/ consideration of mutable member @ class %Message,
	/ \n DSApplication::pFontCache
),
/ $low_prior performance $=
(
	/ \impl @ classes %(Message, MessageQueue)
),
/ completeness of core abstraction $=
(
	+ shell session;
	+ UI scenes,
	+ UI modes,
	+ UI subsessions,
	+ shell framework for plugins and devices,
	+ automatic shellizing,
),
/ services $=
(
	+ general resouce management,
	/ @ "GDI" $=
	(
		+ basic animation support,
		+ more GDI algorithms
	),
	/ fully \impl @ encoding checking
),
/ debugging $=
(
	/ more APIs,
	+ namespaces
),
/ $low_prior tests and examples $=
(
	+ overlapping test @ \cl Rect,
	+ partial invalidation support @ %(HexViewArea::Refresh)
),
/ @ "GUI" $=
(
	+ formal abstraction of rectangular hit test,
	+ key accelerators,
	+ widgets for RTC,
	/ GUI brushes $=
	(
		+ more base class templates,
		+ transformations
	),
	+ generic timers multiplexing for input holding events,
		// To resolve routed events repeating preemption.
	+ widget layout managers,
	+ widget-based animation support,
	* View position switch through scroll bar not accurate enough
		@ class %ListBox
);

b896-b1366:
^ \mac __PRETTY_FUNCTION__ ~ custom assertion strings @ whole YFramework
	when (^ g++),
/ memory fragment issues,
/ basic routines $=
(
	/ ystdex::fixed_point $=
	(
		* \impl @ \op/= for signed types,
		+ 64-bit integer underlying type support
	),
	+ u16printf,
	+ u32printf
),
/ completeness of core abstraction $=
(
	/ shell switching $=
	(
		+ locking;
		+ exceptional state and handlers
	),
	/ messaging $=
	(
		+ general predicates for removing message,
		^ timing triggers @ message loop,
		^ weak_ptr @ shell messages
	),
	/ \cl String interface;
	/ fully \impl @ \cl Path
),
/ services $=
(
	+ general \impl @ images processing interface,
	^ <chrono> to completely abstract system clocks,
	+ general data configuragion,
	/ general file type abstraction
),
/ @ "GUI" $=
(
	+ widget models,
	+ IMEs,
	+ widget layout \impl,
	+ modal widget behavior
);

b1367-b1898:
/ improve efficiency @ \ft polymorphic_crosscast @ \h YCast for \conf release,
/ platform dependent system functions $=
(
	+ correct DMA (copy & fill) @ DS
),
/ completeness of core abstraction $=
(
	/ messaging $=
	(
		+ general predicates for removing message,
	),
	/ fully \impl @ \cl Path,
	/ handles $=
	(
		- \a direct dereference operations of handle type,
		+ real handle type with no \op*
	),
	+ meta $=
	(
		+ meta data,
		+ meta language infrastructure
	)
),
/ services $=
(
	+ user-defined stream filters,
	/ improving \impl font switching,
	/ fully \impl logging $=
	(
		+ more clarified log Levels,
		+ log streams
	),
	/ @ "GDI" $=
	(
		+ basic shapes abstraction,
		+ spline nodes abstraction,
		/ more efficient Font switching,
		/ text alignment,
		/ advanced text layout like Unicode layout control
	)
),
/ @ "GUI" $=
(
	/ partial invalidation support @ \f DrawRectRoundCorner,
	+ document-view models,
	/ focusing $=
	(
		+ focus iteration,
		+ direct focus paths controling
	)
	+ more complex controls,
	+ fully \impl styles @ widgets,
	+ general widget decorators,
	+ clipping areas,
	+ dynamic widget prototypes
);


$LOW_PRIOR_TODO:
^ $low_prior $for_labeled_scope;
b1899-b5412:
+ advanced shell system $=
(
	+ dynamic loading and reloading,
	+ runtime resource redifinition and linking
),
+ general monomorphic iterator abstraction,
/ user-defined bitmap(mainly, shared with screen) buffer @ \cl Desktop,
+ additional shared property,
+ advanced console wrappers,
+ graphics APIs and adaptors,
+ general component operations $=
(
	+ serialization,
	+ designers
),
+ automatic adaptors for look and feels,
+ networking,
+ database interface;


$FURTHER_WORK:
+ other stuff to be considered to append $=
(
	+ design by contract: DbC for C/C++, GNU nana
);
+ (compressing & decompressing) @ resource copying,
+ resource allocation controlling;
+ documentation convention and modeling $=
(
	+ ISO directive or RFC2119 compliance,
	+ documentation indexing tools
);


$KNOWN_ISSUE:
// NOTE: Obsolete issues all resolved are ignored.
* "corrupted loading or fatal errors on loading font file with embedded \
	bitmap glyph like simson.ttc" $since b185,
	// freetype (2.4.6, 2.4.8, 2.4.9, 2.4.10) tested.
* "<cmath> cannot use 'std::*' names" @ "!defined %_GLIBCXX_USE_C99_MATH_TR1"
	@ "libstdc++ with g++ (4.6, 4.7) on devkitARM" @ "platform $DS"
	$before $future;
	// G++ 4.7.0 tested @ b301.
* "crashing after sleeping(default behavior of closing then reopening lid) on \
	real machine due to libnds default interrupt handler for power management"
	$since b279;
* "sorry, unimplemented: use of 'type_pack_expansion' in template \
	with libstdc++ std::thread" @ ^ "g++ (4.6, 4.7)" $before $future(g++4.7.2);
	// G++ 4.7.0 tested @ b300.
	// See http://gcc.gnu.org/bugzilla/show_bug.cgi?id=53872 .


$RESOLVED_ENVIRONMENT_ISSUE:
* "g++ 4.5.2 fails on compiling code with defaulted move assignment operator"
	@ $interval([b207, b221));
* "g++ 4.6.1 internal error for closure object in constructors"
	@ $interval([b253, b300));
	// G++ 4.6.2 tested @ b293. Fixed @ b301.
* "g++ 4.6.1 ignores non-explicit conversion templates when there exists \
	non-template explicit conversion function" @ $interval([b260, b314));
	// Fixed @ b315.


$HISTORY:

$parser.state.style $= $natral_NPL;
$dep_from; // take evaluation dependence from;
$dep_to; // put evaluation dependence to;
$label; // label for locating in code portions;

$design; // features changing probably only made sense to who needs to \
	reference or modify the implementation;
$dev; // issues concerned by developers, which end-users could ignore;
$lib; // issues only concerned with library(only implementation changing, \
	or interfaces modifying including no deletion unless some replacements \
	are provided, so no need fo library users to modify code using the library \
	interface to adapt to the upgrading), regaradless of the output targets;
$build; // issues on build;
$install; // issues on installing;
$depoly; // issues on deployment(other than installing);
$comp; // features consist of dependencies with no additional work;
$doc; // for documents target;
$add_features +; // features added;
$fix_bugs *; // bugs fixed;
$modify_features /; // features modified;
$remove_features -; // features removed;
$using ^; // using;
$not !; // not;
$source_from ~; // features replaced from;
$belonged_to @;
$changed_to ->;
$moved_to >>;
$renamed_to =>;

// Abbreviations.
DLD $dev $lib $design;
DLP $dev $lib $deploy;
DLB $dev $lib $build;

//$transform $list ($list_member $pattern $all($exclude $pattern \
//	$string_literal "*")) +;

$ellipse_refactoring;
$ellipse_debug_assertion;

$ref $=
(
b298 $=
(
$note "access violation examples"
/*
http://stackoverflow.com/questions/5955682/boostmake-shared-causes-access\
-violation
http://lists.cs.uiuc.edu/pipermail/cfe-dev/2011-October/017831.html
*/
)
)

$module_tree $=
(
	'YBase',
	(
		'YDefinition',
		'YStandardEx'
		(
			'TypeOperations',
			'Any',
			'CStandardIO',
			'CString',
			'Iterator',
			'Functional',
			'Algorithms',
			'Utilities',
			'Memory',
			'String',
			'Rational',
			'Operators'
		),
		'LibDefect'
		(
			'String'
		),
		'YTest'
		(
			'Timing'
		)
	),
	'YFramework'
	(
		'CHRLib'
		(
			'CharacterMapping',
			'encoding conversion'
		),
		'YCLib' $=
		(
			'Platform', // platform definition;
			'NativeAPI', // native APIs;
			'YCommon', // common utilities;
			'Input', // common input APIs;
			'Video', // common video APIs;
			'FileSystem', // common file system APIs;
			'Debug', // debug helpers;
			'MemoryMapping' //memory mapping APIs;
		),
		'YSLib'
		(
			'Adaptor', // adaptors;
			'Core' // core;
			(
				'YBaseMacro', // basic macros,
				'YObject', // basic objects;
				'YCoreUtilities', // core utilities;
				'YGDIBase', // GDI base;
				'YDevice', // devices;
				'YMessage', // messaging;
				'YMessageDefinition', // message definitions;
				'YEvent', // events;
				'YShell', // shell abstraction;
				'YFileSystem', // file system abstraction;
				'YApplication' // application abstraction;
			),
			'Service', // services;
			'GUI',
			'UI styles'
		)
		'Helper'
		(
			'YGlobal', // global helper unit;
			'Shell_DS'; // shells for DS;
			'DSMain', // DS main unit;
			'Initialization', // initialization;
			'InputManager' // input manager;
		),
		'NPL'
		(
			'Lexical';
			'SContext';
			'Configuration'
		)
	),
	'YReader'
	(
		'main',
		'file explorer',
		'shells test example',
		(
			'ReadingList',
			'ReaderSetting';
			'text reader'
		),
		'hexadecimal browser'
	)
);

$now
(
	/ %'YFramework' $=
	(
		* DLP "includ guard macros" @ %'Helper' $since b303,
		+ "library %NPL" $= (+ "units %(Lexical; SContext; Configuration)")
	)
	// Release binary image(.nds) of this version are strictly equal with b334 \
		on platform %DS.
),

b334
(
	/ %'YBase'.'YStandardEx' $=
	(
		+ "class template %identity" @ %'TypeOperations',
			// Same to %boost::identity or previously standardized %std::identity.
		* "implementation" @ "all 3 function template %any_cast"
			@ %'Any' $since b331
	),
	/= $dev $lib "normalized version numbers and format of TODO comments",
	/ %'YFramework' $=
	(
		/ %'YSLib'.'Core'.'YObject' $=
		(
			* "missing mutable access method" @ "non-const member function \
				template %ValueObject::Access" b306,
			+ "1 getter, 3 setters" @ "class %ValueNode",
			+ "2 function templates %AccessChild"
		),
		/ "function %IsAbsolute" @ %'YCLib'.'FileSystem' $=
		(
			/ @ "platform %DS" $=
			(
				* "missing checking for null pointer and duplicate ':/'"
					$since b152,
				/ "treated prefix '*:/' other than 'fat:/' or 'sd:/' also valid"
			),
			/ DLD ^ "%PathIsRelativeW" ~ "%PathIsRelativeA"
				@ "implementation" @ "platform %MinGW32"
		)
	)
	/ "all header macro guard defined as integer literal '1'" ~ "nothing",
	/ DLD "improved reader setting constructing" @ %'YReader'.'text reader'
),

b333
(
	/ %'YFramework'.'YSLib' $=
	(
		- DLD "template declaration related macros %('PDefTmplH*', \
			'DefFwdCtorTmpl*')" @ %'Core'.'YBaseMacro',
		/ %'YEvent' $=
		(
			+ "support for variant handler for initialization"
				@ "template class %GHEvent"
				// As same as %std::function.
			/ "improved template parameter declaration"
				^ "function type and parameter pack",
			/ "implementation" @ "macro %(DeclDelegate, EventT)"
		)
	),
	/ %'YBase'.'YStandardEx'.'TypeOperations' $=
	(
		/ %'TypeOperations' $=
		(
			/ $doc "Doxygen comments" $=
			(
				+ "group %metafunctions",
				/ 'UnaryTypeTrait' => 'unary_type_trait',
				/ 'BinaryTypeTrait' => 'binary_type_trait'
			)
			+ "unary type traits function templates %(is_returnable, \
				is_class_pointer, is_lvalue_class_reference, \
				is_rvalue_class_reference)"
		)
		+ "header %functional.hpp as %Function" $=
		(
			+ $lib "including header (<tuple>, <functional>, <string>)";
			+ "function templates %(make_parameter_tuple, return_of, \
				parameter_of, paramlist_size)"
		)
		/ "%(seq_apply, unseq_apply, ref_eq, xcrease_t, delta_assignment, \
			xcrease, delta_assign, deref_op, const_deref_op, deref_comp, \
			deref_str_comp)" @ %'Utilities' >> "header %Functional"
	)
),

b332
(
	/ %'YBase'.'YStandardEx'.'Any' $=
	(
		/ @ "class %any" $=
		(
			/ "split (copy, move) assignment" ~ "unifying assignment",
			/ "constructor %any(any_holder*)"
				-> "%any(any_holder*, std::nullptr_t)"
		),
		/ $lib "headers including" $=
		(
			- "header %TypeOperations",
			* "strict ISO C++11 code compatibility" $since b331
				$= (* "missing <typeinfo>")
				// The program temporarily is not ill-formed for libstdc++'s \
					<memory> including <typeinfo> under C++11 mode.
		)
		$dep_to "any"
	),
	/ %'YFramework'.'YSLib'.'Core' $=
	(
		(
			* "wrong implementation" @ "macro for derived interface declaration \
				(currently %DeclBasedI)" @ %'YBaseMacro' $since b240;
			$dep_to "interface declaration fix";
		),
		/ %'YObject' $=
		(
			(
				$dep_from "any",
				+ $lib "including header %Any" @ "header %YObject";
				(
					$dep_from "interface declaration fix";
					+ "interface class %IValueHolder"
				);
				+ "class templates %(IValueHolder, PointerHolder)";
				/ DLD "implementation" @ "class %ValueObject"  
			)
			/ DLD "simplified implementation" @ "class %ValueNode"
		),
		/ %'YEvent' $=
		(
			/ $lib "simplified implementation" @ "class template %GEvent",
			/ "all member function templates %AddUnique"
				@ "class template %GEvent" -> "non-member function templates"
		)
	)
),

b331
(
	/ $lib %'YFramework'.'YSLib' $=
	(
		/ %'Core' $=
		(
			/ "removed reference of parameter type for events" @ %'YEvent',
			/ "exception specifications" ^ "%(ythrow, ynothrow)" ~ "%throw",
			/ "merged copy and move assignments as unifying assignment"
				@ "classes %(ValueObject, Message)"
				// The no throw exception specification can be merged for \
					the copy is out of the block.
		)
		/ "add explicit rvalue reference of parameter for UI delegates"
			@ %'GUI',
	),
	/ %'YBase' $=
	(
		/ $lib "implementation" @ "macro %(ynothrow, ynoexcept)"
			@ %'YDefinition',
		/ "exception specifications" ^ "%ynothrow" ~ "%throw()"
			@ %'TypeOperations',
		/ %'YBase'.'Any' $=
		(
			(
				+ "class %any_holder";
				+ "class templates %(value_holder, pointer_holder)";
				+ "class %any"
			),
			+ "class bad_any_cast";
			+ "function templates %(any_cast, unsafe_any_cast)"
		),
		^ $dev $lib "qualified function and function template names"
			~ "unqualified function and function template names" @ "header \
			%(Algorithm, Cast, CString, String, Utilities) to prevent wrong ADL"
	)
),

b330
(
	/ %'YReader' $=
	(
		+ "setting button" @ "reader box" @ %'text reader',
		+ "full screen background preview" @ %'shells test example'
		/ DLD "macro guard names"
	),
	/ %'YFramework' $=
	(
		/ %'YSLib'.'Core'.'YObject' $=
		(
			/ %'Core'.'YObject' $=
			(
				/ "throwing %std::bad_cast when object pointer is null"
					@ "member function templates ValueObject::Access",
				(
					+ "class %ValueNode";
					+ "inline functions %(begin, end) for \
						class %ValueNode parameter"
				)
			),
			* DLD "strict ISO C++11 code compatibility" $since b261
				$= (* "missing names" @ "declarations of scoped enumeration")
		),
		/ DLD %'Helper' $=
		(
			/ "macro guard names";
			* $comp "wrong macro guard name" @ "header Initialization.h"
				$since b296
		),
		/ "support for GUI input" @ "function %WaitForInput"
			@ "platform %MinGW32" @ %'YCLib'.'Input'
	),
	(
		/ $lib "implementation" @ "(constructor, destructor)"
			@ "class %noncopyable" %'YBase'.'YStandardEx'.'Utilities'
		* $comp $dld "constexpr use" @ "derived classes of class %noncopyable"
			$since b246
	)
),

b329
(
	- DLD "redundant 'this->' for non-dependant names" @ "templates",
	/ $lib %'YFramework' $=
	(
		/ @ "unit %YFile" @ %'YSLib'.'Service' $=
		(
			+ "member functions %(Write, Flush)" @ "class %File",
			+ "2 operator<< for output",
			/ "ignore EOF check" @ "implementation"
				@ "1st operator<< for input";
		);
		/ DLD "simplified implementation" @ "function %CheckInstall"
			@ %'Helper'.'Initialization'
	)
	/ $lib %'YBase'.'YStandardEx' $=
	(
		* "missing declaration %ystdex::(is_dereferencable, \
			is_undereferencable)" %'Iterator' $since b250
			$= (/ "including header %<memory>" -> "header %memory.hpp"),
		/ %'CString' $=
		(
			/ "function template %sntctslen" => "%ntctslen",
			(
				(
					+ "including header %<string>";
					/ "returning types of function templates for NTCTS \
						comparing" ^ "%std::char_traits::int_type" ~ "%wint_t",
					/ "names of function templates for NTCTS",
				),
				+ "compile-time algorithm yconstexpr function template \
					%(const_ntctschr, const_ntctscnt, const_ntctslen, \
					const_ntctscmp, const_ntctschrn, const_ntctsstr)"
			)
		)
	),
	^ "%CheckButton" ~ "%CheckBox"
		@ %'YReader'.'shells test example'.'ReaderSetting'
),

b328
(
	+ $lib "console color enum" @ 'YCLib'.'Video',
	/ DLD "split unit" %'YReader',
	+ "class templates %(nifty_counter, call_once_init)"
		@ %'YBase'.'YStandardEx'.'Utilities'
),

b327
(
	/ %'YFramework' $=
	(
		+ "functions %(UTF8ToMBCS, WCSToMBCS)" @ "platform %MinGW32"
			@ %'YCLib''YCommon',
		/ DLD "simplified constructor template for functor"
			@ "class template %GEvent" @ %'Core'.'YEvent'
	),
	/ %'YBase' $=
	(
		+ "support for %MoveConstructible only function objects"
			@ "parameter type" @ "function templates"
			@ %('YTest'.'Timing', 'YStandardEx'.'Utilities'),
		/ %'Utilities' $=
		(
			* $dev $lib "macro %YB_HAS_BUILTIN_NULLPTR checking \
				when !YB_HAS_BUILTIN_NULLPTR" $since b319,
			* DLD "simplified implementation" @ "function template %unseq_apply"
		)
	)
),

b326
(
	(
		+ "ISO C/C++ standard input/output open mode conversion functions \
			%openmode_conv" @ %'YBase'.'YStandardEx'.'CStandardIO';
		$dep_to "openmode_conv";
	)
	/ %'YFramework' $=
	(
		/ %'YSLib'.'Service' $=
		(
			+ "string input operators for class %File",
			(
				$dep_from "openmode_conv";
				+ "ISO C/C++ standard input/output open mode arguments"
					@ "(constructor, member function %Open)"
					@ "classes %(File, TextFile)";
				$dep_to "open_file"
			),
			/ DLD "implementation" @ "function template %TransformRect"
				@ "header %YBlit" ^ "macro %YB_LIKELY"
		),
		/ %'Helper' $=
		(
			* "invalid FPS controlling" @ "platform %MinGW32"
				@ %'DSMain' $since b320,
			(
				$dep_from "open_file";
				/ DLD "simplified implementation" @ %'Initialization'
			)
		)
		+ "constructor template with string argument" @ "class %MappedFile"
			@ %'YCLib'.'MemoryMapping'
	),
	/ $doc "licenses" $=
	(
		/ "overall license %LICENSE.txt",
		+ "license for old version %LICENSE_HISTORY.txt",
		/ "%GPL.txt" => "%gpl-2.0.txt",
		+ "%gpl-3.0.txt",
		+ "%CC BY-SA 3.0 legalcode.txt"
	)
),

b325
(
	+ $dev $lib "macro %yoffsetof" @ %'YBase'.'YDefinition',
	/ %'YFramework'.'Helper'.'DSMain' $=
	(
		/ DLD "simplified host environment and screen implementation",
		* "host window procedure for painting not synchronized \
			with application initialization" $since b299
	)
),

b324
(
	/ %'YFramework' $=
	(
		/ %'CHRLib' $=
		(
			+ $dev "extended character encoding mapping unit %MappingEx",
			/ $deploy "GBK mapping" ^ "runtime loading" ~ "statically linking"
		),
		@ %'YCLib' $=
		(
			/ %'FileSystem' $=
			(
				(
					+ "4 unbuffered file functions %ufopen";
					$dep_to "ufopen"
				),
				+ $dev "exception specification %ynothrow"
					@ "free functions and function template %ufexists"
			),
			+ %'MemoryMapping' $=
			(
				$dep_from "ufopen";
				+ "%unit MemoryMapping"
				// Memory mapping can be used on Win32 or POSIX compliant \
					systems, while DS and other systems use unbuffered read \
					to simulate.
			);
		)
		/ %'Helper'.'Initialization' $=
		(
			/ "install checking implementation",
			+ "uninitialization"
		)
	)
),

b323
(
	/ %'YFramework'.'Helper' $=
	(
		+ 'InputManager'
			$= (+ "class %Devices::InputManager" @ "unit %InputManager");
		/ $dev $lib "implementation dispatching" ^ "class %InputManager"
			@ "class %ShlDS" @ %'DSMain',
		/ DLD "host implementation" @ "platform %MinGW32" @ 'DSMain',
		/ DLD "several header dependencies improved"
	),
	/ $dev $lib "duplicate entries removed" @ "Visual C++ project filters"
		@ "platform %DS",
	* $dev $lib "wrong position for directory %Helper"
		@ "Code::Blocks project %YFramework" $since b303,
	+ $doc "architecture document YFramework.vsd" @ "directory /doc/vsd"
		^ "Microsoft Visio 2010",
	/ DLD "member access control" @ "class %ShlTextReader"
		@ %'YReader'.'text reader'
),

b322
(
	/ %'YFramework' $=
	(
		/ %'Helper'.'DSMain' $=
		(
			* "%KeyTouch not raised by key input" $since b321,
			/ @ "class %DSScreen" @ "platform %MinGW32" $=
			(
				/ DLD "direct copy" ~ "pointer buffering";
				* DLD "thread-safety" @ "member function %Update" $since b299;
				$dep_to "screen thread safety";
			),
			- "screens checking and initialization UI",
				// Dynamic null pointer check is eliminated. CLI interface \
					is not held after screen initialization. \
					There can be a logo.
			/ $dev $lib "font cache initialization" $=
				// Explicit cache initialization moved to application \
					initialization.
			(
				/ @ "function %InitializeSystemFontCache" @ % 'Initialization',
				- "member function %DSApplication::ResetFontCache"
			)
		),
		/ %'YCLib' $=
		(
			+ $dev $lib "macro %YCL_MULTITHREAD" @ %'Platform',
			^ DLD "attribute %format 'ms_printf'" @ "function %yprintf"
				@ "platform %MinGW32" @ %'Debug',
			(
				$dep_from "removal dep of mmbcpy and mmbset";
				- "functions %ystdex::(mmbcpy, mmbset)" @ %'YCommon'
			)
		),
		/ %'YSLib' $=
		(
			/ $lib "function template %ClearSequence" @ "header %YCoreUtilities"
				^ "%std::fill_n" ~ "%ystdex::mmbset",
			+ $dev "exception specification %ynothrow" @ "function template \
				%ClearPixel" @ "header %YBlit",
			/ DLD "several functions implementations" ^ "%std::copy_n"
				~ "%ystdex::mmbcpy",
			* "implementation" @ "function template %CreateRawBitmap"
				@ "header %ShellHelper" $since $before b132;
			- $dep "using %ystdex::(mmbcpy, mmbset)" @ %'Adaptor'
			$dep_to "removal dep of mmbcpy and mmbset"
		)
	),
	(
		$dep_from "screen thread safety";
		* $comp "screen content corrupted" @ "platform MinGW32" $since b299
			// A serious bug though rarely occurred.
	),
	+ "function template %sfmt for format string output"
		@ %'YBase'.'YStandardEx'.'String'
		// Currently only %char output(%std::string) is supported.
),

b321
(
	+ $doc "comments about reentrancy and thread-safety"
		@ "project %(YBase, YFramework)",
	/ DLD "macro names" @ %'YBase'.'YStandardEx'.'Operators',
	/ DLP "library using" @ "platform %DS"
		$= (^ "updated devkitARM release 41" ~ "devkitARM release 40"),
	/ DLP "all std=c++0x" -> "c++11" @ "platform %MinGW32"
		@ "Code::Blocks project configuration";
	/ %'YFramework' $=
	(
		/ %'YCLib' $=
		(
			+ "macro %YCL_MULTITHREAD" @ %'Platform';
			/ %'Input' $=
			(
				* "thread-safety" @ "function %ClearKeyStates" $since b299,
				+ "new implementation fit for large key states"
					@ "platform %MinGW32",
				/ $lib "declaration" @ "class %CursorInfo"
			)
		);
		/ %'Helper' $=
		(
			+ "thread-safety" @ "function %DispatchInput" @ 'DSMain',
			^ "direct painting" ~ "asynchronous painting with message queue"
				@ %'Shell_DS'
				// Great loop performance increased for empty input.
		)
	)
),

b320
(
	/ %'YFramework'.'YSLib' $=
	(
		/ DLD "all member functions named %IsEmpty eliminated"
			-> "operator (bool, !)",
		/ 'Core' $=
		(
			/ %'YGDIBase' $=
			(
				+ "member functions %operator(&=, |=)" @ "class %Rect";
				/ "functions %(Intersect, Unite)"
					-> "functions %operator(&, |) for class %Rect"
			),
			/ %'YMessage' $=
			(
				- "message destination",
					// Some performance improved. If need to specify \
						destination, private messages for each shell could be \
						considered.
				/ $lib ^ "move constructor" @ ("class %Message";
					"function %PostMessage")
					// A little performance improved.
			)
		)
		* "performance" @ "member function %AScrollBar::Refresh "%'GUI'
			$since b295,
			// Avoiding refreshing repainted bottom button unnecessarily.
		/ "implementation" @ ("message loop", $design "input dispatching")
			%'Helper'.'DSMain'
			// Significantly performance improved.
	),
	* $dev $build $lib "missing including path Code::Blocks project \
		%(YBase_MinGW32, YFramework_MinGW32)" $since b299
),

b319
(
	^ "i686-mingw-w64-gcc-4.7.1" ~ "i686-mingw32-gcc-4.7.0" @ platform MinGW32,
	/ %'YBase' $=
	(
		/ %'YDefinition' $=
		(
			+ "environment configuration macro for keyword %noexcept";
			$dep_to "noexcept macro"
		),
		+ "library %YTest" $=
		(
			+ "header %timing.hpp" $=
			(
				+ "test timing function templates %(once, once_c, total, \
					total_c, average)";
				$dep_to "test timing"
			)
		)
	),
	/ %'YFramework' $=
	(
		/ DLD "all member functions named %IsValid eliminated"
			-> "operator (bool, !)",
		/ %'YCLib' $=
		(
			/ %'FileSystem' $=
			(
				+ DLD "no throw exception specification"
					@ "class %HFileNode",
				* "missing iterator reset" @ "member function %Close" @ "file \
					node iterator" @ %'FileSystem' $since b142
					// It causes double releasing file resources\
						when manually called.
			)
			+ DLD "no throw exception specification" @ (("class %Color",
				("function %ScreenSynchronize" @ "platform %DS")
				@ %'Video'), "header %Memory")
		),
		/ %'YSLib'.'Core' $=
		(
			/ %'Core' $=
			(
				+ DLD "no throw exception specification" @ "functions \
					declarations" @ "unit %(YExceptionYGDIBase, YFunc, \
					YCounter, YCoreUtilities, YDevice, YStorage)",
					// Some minor performance improvement.
				(
					$dep_from "noexcept macro";
					+ DLD "no throw exception specification"
						@ "functions declarations" @ "unit %YEvent"
				),
				* "type checking implementation" @ "class %ValueObject"
					@ %'YObject' $since b306
					// Checking method of equality and type-id tests were \
						wrongly swapped for %YCL_FUNCTION_NO_EQUALITY_GUARANTEE.
			),
			/ %'Service' $=
			(
				/ DLD "resource content setter" @ "class %Image"
					>> ("class %BitmapBuffer" @ "unit %YGDI"),
				/ DLD "minor implementation" @ "unit %YGDI"
			),
			+ DLD "no throw exception specification" @ "class %DSScreen"
				@ %'Helper'.'DSMain',
			/ @ "header %YWindow" %'GUI' $=
			(
				/ "simplified default arguments" @ "constructor"
					@ "class %Window";
					// Nullptr is equal to empty instance of %Image \
						for %ImageBrush.
				- "unnecessary header dependencies"
			)
		)
		/ %'YSLib'.'Core' $=
	),
	- DLB "-Wnoexcept" @ "macro %CXXFLAGS" @ "makefile"
		@ "project %(YFramework_DS, YSTest_ARM9)",
		// ISO C++11 5.1.2/6 doesn't specify conversion function for a closure \
			object to a function pointer shall have exception specification. \
			Thus G++ may complain when evaluating %noexcept expression. \
			It seems a defect in the standard, but here just turn the warning \
			off for convenience.
	/ %'YReader'.'shell test example' $=
	(
		$dep_from "test timing";
		+ "total time measuring for images loading"
	)
),

b318
(
	/ %'YFramkework'.'YSLib' $=
	(
		/ %'Core'.'YMessage' $=
		(
			/ DLD "improved performance of move constructor" @ "class $Message",
			- "redundant parameter" @ "function %PostMessage"
		)
		/ @ "unit %TextList" @ %'GUI' $=
		(
			/ DLD "simplified implementation" @ "key event handler"
				@ "class %TextList"
				// Get better performance in scrolling.
			* "wrong length set for class %TextList" $since b285 $=
			(
				* "invalid height not ignored"
					@ "member function %TextList::AdjustViewLength",
				* "missing adjusting viewer length"
					@ "function %ResizeForContent";
				$dep_to "wrong textlist length"
			)
		)
	)
	/ DLP "library using"
		$= (^ "updated library freetype 2.4.10" ~ "modified freetype 2.4.9"),
	^ DLB "parallel command line option '-j'"
		@ "all VC++ projects makefile command lines",
	/ %'YReader'.'shells test example' $=
	(
		$dep_from "wrong textlist length";
		* $comp "submenu test using right arrow key" $since b285
	)
),

b317
(
	/ %'YFramework' $=
	(
		/ %'YSLib' $=
		(
			/ "simplified interface and implementation for message output"
				@ "class %MemoryList" @ %'adaptor',
			/ %'Core' $=
			(
				/ @ "class %Shell" @ %'YCoreUtilities' $=
				(
					(
						/ "return type discarded" @ "member function \
							%OnGotMesssage";
						$dep_to "got msg ret tp"
					),
					+ "public inheritance %enabled_share_from_this"
				),
				/ @ "class %Application" %'YApplication' $=
				(
					+ "public inheritance %Shell";
					/ "member function %Dispatch merged"
						-> "member function overrider %OnGotMessage"
				)
				/ @ "class %MessageQueue" @ %'YMessage' $=
				(
					/ "iterator type exposed",
					/ "implementation" @ "member function %Peek for better \
						performance",
					/ DLD "implementation" @ "class %Message"
						^ "%std::multimap" ~ "%std::multiset",
						// It makes debug version significantly slower, \
							but better performance for release.
					- "redundant fields and interfaces of timing"
						@ "class %Message",
					* "missing non-implicit-deleted move constructor"
						@ "class %Message" $since b316
				)
			),
			* "missing calling event %LostFocus when focus moved through \
				requesting" @ %'GUI' $since b315,
		),
		/ %'Helper' $=
		(
			(
				$dep_from "got msg ret tp";
				/ "return type discarded" @ "member function %OnGotMesssage"
					@ "class %ShlDS"
			)
			/ @ "unit %DSMain" $=
			(
				/ DLD "implementation" @ "member function %DealMessage"
					@ "class %DSApplication",
				/ "implementation" @ "idle function"
					// A little better performance for broadcasting message.
			)
		),
		(
			$dep_from "got msg ret tp";
			/ "return type discarded" @ "member function %OnGotMesssage"
				@ "shell classes" @ %'YReader'
		)
	)
),

b316
(
	/ %'YBase'.'YStandardEx' $=
	(
		(
			+ "function templates %raw with parameter %weak_ptr" @ %'Memory';
			$dep_to "weak raw"
		),
		/ DLD "no throw specification" ^ "%ynothrow" ~ "%throw()" @ %"Rational",
		/ DLD "all parameter of type reference of %nullptr_t"
			-> "raw %nullptr_t"
	)
	/ %'YFramework'.'YSLib' $=
	(
		+ "member function %Find" @ "class %MTextList" @ %'GUI',
		/ %'Core'.'YMessage' $=
		(
			$dep_from "weak raw";
			/ "destination shell field" ^ "%weak_ptr<Shell>" 
				~ "%shared_ptr<Shell>",
			+ "field %bool %is_to_all designating broadcasting",
			+ "member function %MessageQueue::Push(Message&&)";
			/ "parameter designating destination shell" @ "APIs for posting \
				messages" ^ "weak_ptr<Shell>" ~ "shared_ptr<Shell>"
		),
		(
			/ @ "simplified class %FontCache" @ %'Adaptor' $=
			(
				/ "simplified implementation" @ "constructor",
					// Removed automatically default font path loading.
				/ "simplified implementation" @ "member function %LoadFontFile",
					// Ignored path when FreeType error occurs.
				/ "member function %LoadTypefaces",
					// Removed initialization default typeface on loading.
				- "path map"
					// Duty of paths management moved to initialization.
			)
			/ DLD "simplified implementation" @ %'Helper'.'Initialization'
		)
	),
),

b315
(
	/ %'YBase' $=
	(
		/ DLD "confirming no need for explicit conversion functions"
			@ 'YStandardEx'.'Rational',
		/ @ "header %ydef.h" $=
		(
			(
				+ "macros for alignment" @ "header %ydef.h";
				$dep_to "align macro"
			),
			+ $doc "several comments on macros"
		),
		/ "implementation" @ "class templates %(deref_comp, deref_str_comp)"
			@ %'Utilities'
	),
	/ %'YFramework' $=
	(
		/ %'YSLib'.'GUI' $=
		(
			+ "interfaces for raw focus requesting and releasing";
				// Response focus behavior without calling events.
		)
		/ DLD "simplified implementation" @ "destructor @ class %Widget",
		(
			$dep_from "align macro";
			+ DLD "static assertion of alignment equality between %wchar_t \
				and %CHRLib::ucs2_t" @ "platform MinGW32"
				@ %'YCLib'.'FileSystem'
		),
		/ DLD "simplified implementation" @ "function template %BlitScale"
			@ %'Service'
	)
),

b314
(
	(
		/ "platform macros definition";
		/ DLD "simplified platform macros conditional inclusion"
	),
	- "test of macro %DEBUG" @ %'YFramework',
	/ "detailed debug output" @ "platform %MinGW32"
	// Binaries of this version are strictly equal with b313 on platform %DS, \
		but some less size on platform %MinGW32.
),

b313
(
	/ %'YFramework'.'YCLib' $=
	(
		(
			+ "separate unit %Keys for keyboard/keypad constants",
			+ "separate unit %Timer for timers";
			/ "header including",
				// Making more loose coupling.
		),
		/ @ 'FileSystem' $=
		(
			- "using ::mkdir",
			/ "using ::chdir" -> "function %uchdir which compatible with \
				UTF-8 paths"
			/ DLD "simplified encoding conversion calls"
		)
	),
	/ "macro names prefix" -> "YB_" ~ "YCL_" @ %'YBase'
),

b312
(
	/ %'YFramework'
	(
		/ %'YCLib' $=
		(
			+ "separate unit %Video" @ 'Video',
			/ @ 'FileSystem' $=
			(
				+ "separate unit %FileSystem";
				+ "function %udirexists"
			),
			- "redundant types and macros definition" @ "header %NativeAPI.h",
			/ "header including"
				// Making less of directly including header %NativeAPI.h from \
					public headers for less pollution \
					in global namespace and macros.
		);
		/ %'YSLib'.'Adaptor' $= ("new including and using declarations")
	)
),

b311
(
	(
		+ "class %CheckButton" @ %'YFramework'.'YSLib'.'GUI';
		^ "%CheckButton" ~ "%CheckBox" @ 'shells test example'
	),
	^ DLD "contextual keyword %override" ~ "keyword %virtual for overriders"
),

b310
(
	/ %'YFramework'.'YSLib' $=
	(
		/ %'Service' $=
		(
			* "wrong margin threshold calculated" @ "character output routine"
				$since $before b132;
			* $comp "rightmost character not shown when using function \
				%DrawClippedText" @ "unit %CharRenderer" $since b309;
			$dep_to "rchar margin"
		),
		/ %'GUI' $=
		(
			(
				$dep_from "rchar margin";
				* $comp "rightmost character not shown when label text aligned \
					to right" $since b309
			),
			* "missing cascaded buffer background redrawing"
				@ "class %BufferedRenderer" $since b278
		)
	);
	/ "appearance" @ %'YReader'.'shells test example'
),

b309
(
	/ DLD "function %SwitchVisible" @ 'YReader' >> %'YFramework'.'Helper',
	/ %'YFramework'.'YSLib' $=
	(
		/ %'Service' $=
		(
			+ "getters" @ "class %TextRenderer";
			/ "text drawing functions" $=
			(
				/ "support for line wrapping";
				+ $doc "parameter descriptions"
			)
		);
		/ @ %'GUI' $=
		(
			+ "member %AutoWrapLine" @ "class %MLabel";
			/ DLD "implementation" @ "member function %PaintItem"
				@ "classes %(TextList, Menu)"
		)
	);
	/ "file information appearance" @ %'YReader'.'file explorer'
),

b308
(
	/ DLP "library using" @ "platform %DS" $=
	(
		^ "updated devkitARM release 40" ~ "devkitARM release 39",
		^ "updated libfat 1.0.10" ~ "libnds 1.0.11"
	),
	/ $dev $lib %'YBase' $= (+ "library %LibDefect"
		$= (+ "libstdc++ string conversion workaround %String"));
	/ $dev $design "simplified UI shells implementation" @ %'YReader'
),

b307
(
	(
		/ %'YFramework'.'YSLib'.'GUI' $=
		(
			* "%CheckBox tick interface" $since b306,
			(
				+ "member functions %(GetLocationRef, GetSizeRef)"
					@ "classes %(Visual; View)";
				+ "scroll bar resizing",
				+ "raising event %Resize" @ "member function \
					%ScrollableContainer::FixLayout",
				* "missing text list view updating" @ "member function \
					%ListBox::ResizeForPreferred" $since b282;
				* $comp "missing scroll bars refreshing" @ "(%ListBox \
					resizing; %DropDownList list showing)" $since b282
			)
		);
		/ @ "setting panel" @ %'YReader'.'text reader' $=
		(
			/ "scrolling interval value set",
			* "wrong displayed scrolling interval" $since b301
		)
	),
	/ $doc "Doxygen file" $=
	(
		* "wrong Doxygen root output path" $since b300,
		/ "input ignorance"
	),
	/ $design "some implementations" @ "unit %(Shells, ShlReader)"
		@ %'YReader' >> ("unit %ShellHelper" @ "library %Helper")
),

b306
(
	/ DLP "library using" @ "platform %DS" $=
	(
		^ "updated devkitARM release 39" ~ "devkitARM release 38",
		^ "updated libnds 1.5.7" ~ "libnds 1.5.5"
	),
	/ "class %ShlReader" @ %'YReader'.'text reader' $=
	(
		/ $design "simplified implementation",
		* "access out of range when process of 100% reached"
			@ "member function %ReaderBox::UpdateData" $since b271;
	),
	/ %'YFramework' $=
	(
		/ %'YSLib' $=
		(
			+ "member function %Tick" @ "class %CheckBox" @ %'GUI',
			+ "member function templates %Access" @ "class %ValueObject"
				@ %'core abstraction'
		),
		/ "macro %YSL_DLL" -> "macro %(YCL_DLL; \
			YCL_FUNCTION_NO_EQUALITY_GUARANTEE)"
	),
	/ %'YBase'.'YStandardEx'.'TypeOperations' $=
	(
		/ $doc "grouping",
		/ "traits according to ISO C++11" ^ "%integral_constant",
		+ "binary type trait %has_equality_operator"
	)
),

b305
(
	* "wrong drop down list contents" @ "setting panel"
		@ %'YReader'.'text reader' $since b301,
	/ %'YFramework' $=
	(
		+ "string duplicate conversion function %strdup"
			@ %'CHRLib'.'encoding conversion';
		+ "UCS-2LE support for parameters" @ %'YCLib'.'FileSystem';
		+ "%String parameter support" %'YSLib'.'Core'.'YFileSystem',
		/ DLD "simplified implementation" @ %'Helper'.'Initialization'
	),
	+ $doc "file %YCLib.txt for library % YCLib"
),

b304
(
	/ %'YFramework' $=
	(
		/ %'YSLib'.,
		(
			/ %'GUI' $= "check box appearance",
			(
				$dep_from "u16getcwd";
				/ "function %GetNowDirectory" @ %'Core'
					.'YFileSystem' "returning %String" ~ "%string";
				$dep_to "non-ASCII path correction"
			)
		),
		(
			+ "function %u16getcwd_n" @ %'YCLib'.'FileSystem';
			/ $dep_to "u16getcwd"
		)
	),
	/ %'YReader' $=
	(
		/ "size of check boxes",
		(
			$dep_from "non-ASCII path correction";
			* $comp "wrong non-ASCII path get" @ %'file explorer' @
				"platform %MinGW32" $since b299
		)
	)
	/ DLD "simplified assertion strings",
	/ %'YBase' $=
	(
		/ DLD "parameter" @ "function template %pod_fill" @ %'Algorithms',
			// Using const lvalue reference to reduce probable unnecessary \
				copy of instantiated template functions.
		* "wrong return type when instantiated with array version of \
			%std::unique_ptr" @ "template function %raw" @ %'Memory',
		+ "header %string.hpp" $=
		(
			+ "traits class template %string_traits",
			+ "string algorithms function templates %(ltrim, rtrim, trim, \
				get_mid, split)"
		),
		- "header <sys/types.h> dependency" @ "header %ydef.h"
	)
),

b303
(
	/ %'YBase' $=
	(
		(
			+ "variadic meta types and operations";
			+ "unsequenced call function template %unseq_apply"
		),
		* DLD "ambiguous overloading with empty template argument list"
			@ "function templates %(parameterize_static_object, get_init)"
			$since b301;
	);
	/ %'YFramework'.'YSLib' $=
	(
		/ %'YSLib' $=
		(
			/ %'GUI' $=
			(
				+ "function templates %(AddWidgets, AddWidgetsZ, RemoveWidgets)"
					@ "header %yuicont.h" @ %'GUI',
				(
					$dep_from "timer delay";
					+ "member function %Delay" @ "class %InputTimer"
					$dep_from "input timer delay";
				)
			),
			(
				+ "member function %Delay" @ "class %Timer" @ %'Service'
				$dep_to "timer delay";
			),
			(
				/ %'helpers' >> %$$'Helper';
				$dep_to "helpers"
			)
		),
		+ "adapting macros for DS and MinGW32" @ %'YCLib'.'Input',
		(
			$dep_from "helpers";
			/ DLD "simplified interfaces" @ %'Initialization'
		)
	);
	/ %'YReader' $=
	(
		/ $design "simplified implementation",
		/ %'text reader' $=
		(
			* @ "platform %MinGW32" $since b299
			(
				* $dev $design "wrong implementation" @ "macro %YCL_KEY(X)"
					@ "header Shells.h";
				* "key 'Y' no respond",
				* "wrong font size set when responding key ('X', 'Y') and \
				  boundary font size reached"
			)
			(
				$dep_from "input timer delay";
				* "%KeyDown responded more than once when the input waiting \
					interval is smaller than actual delay" $since b300;
					// ('X', 'Y') on DS delayed and led to over setting \
						of font size.
			),
		),
		- "direct UI drawing test" @ %'shells test example',
			// It has never been compatible on MinGW32.
		* "slash-ended item not recognized as directory" @ %'file explorer' 
			@ "platform %MinGW32" $since b299
	)
),

b302
(
	/ %'YFramework'.'YSLib' $=
	(
		/ %'GUI' $=
		(
			/ @ "unit %YStyle" $=
			(
				+ "typedef %hue" @ "unit %YStyle"
				* "overflow on boundary saturation or light value"
					@ "function %HSLToColor" $since b276,
			);
			/ @ "unit %Button"
			(
				+ "hue settings for button painting",
				- "class %CloseButton"
			),
			(
				+ "function %DrawArrow" @ "unit %YWidget";
				/ DLD "simplified arrow painting" @ "scroll bar buttons"
			)
		),
		/ "shell switching helper function" %'helpers'
			// Destination shell handle message post to set shell is set \
				to null to avoid potentional dead lock in Message loop when \
				called more than once.
	),
	* "infinite loop after pressing %Esc" @ "main UI" @ %'YReader'.'text reader'
		$since b300
),

b301
(
	/ DLD "all %inline member outside of class or class template definition"
		@ "headers" >> "class or class template definition"
	/ %'YBase'.'YStandardEx' $=
	(
		/ %'Utilities' $=
		(
			+ "function template %call_once",
				// Just like %std::call_once, but with no thread safety \
					guarantee.
			(
				+ "function template %parameterize_static_object";
				+ "function template %get_init";
				% dep_to "get_init"
			)
		),
		/ DLD "simplified implementation" ^ "constexpr brace-initializers"
			@ "class template %iterator_operations" @ %'Iterator'
	),
	/ %'YReader' $=
	(
		/ $design "simplified implementation" @ %'main',
		(
			/ $dep_from "get_init";
			/ $design "simplified implementation" @ %'text reader',
		),
		+ "recovering path on exiting reader" $=
		(
			+ "initialized with path argument" @ "class %ShlExplorer";
			/ "set shell path on exiting" @ "class %ShlReader"
		)
	)
	/ %'YFramework'.'Core' $=
	(
		* $doc @ "class template %safe_delete_obj" @ %'YCoreUtilities'
			$since b263
		/ "completed proper result type" @ "function %GetAreaOf"
			@ "header %ygdibase.h"
	)
),

b300
(
	/ %'YFramework' $=
	(
		/ %'YSLib' $=
		(
			/ %'GUI' $=
			(
				+ ("input timer class %InputTimer";
					"input timer API for repeating key input") @ "unit %YGUI",
				+ "function template %OnEvent_Call for forwarding event calling"
					@ "unit %YControl"
			),
			- "static const objects" @ "class %Application"
				@ %'Core'.'YApplication'
		),
		* "key updating" @ "platform %MinGW32"
			@ %'YCLib'.'Input' $since b299
			// It seems that sometimes %GetAsyncKeyState reads unexpected \
				virtual key value like '0xFF'.
	);
	/ %'YReader'.'text reader' $=
	(
		* "abnormal delaying on responding event %KeyHeld" $since b271,
			// Since the event routing strategy was not %Tunnel.
		* "crashing when setting font size" $since b297
	),
	(
		/ DLD "reduce several global objects";
		/ $dev $lib "deprecated unit %GBKEX.cpp" >> "library %YSLib"
			~ "project %YSTest_ARM9"
	),
	+ "dynamic linked library targets" @ "Code::Blocks projects"
		@ "platform %MinGW32",
	/ "directory configurations" >> "unit %Initialization" @ "directory %Helper"
		~ "file %GBKEX.cpp";
	- "deprecated file %GBKEX.cpp";
	/ $doc "directory %doc moved to top directory",
	- $doc @ "Code::Blocks project file",
	/ DLP "library using" @ "platform %DS" $=
	(
		^ "updated devkitARM release 38" ~ "devkitARM release 37",
		^ "updated libnds 1.5.5 with default arm7 0.5.24"
			~ "libnds 1.5.4 with default arm7 0.5.23" 
	)
),

b299
(
	/ %'YFramework' $=
	(
		/ %'YSLib' $=
		(
			/ %'helpers' $=
			(
				/ DLD "input dispatching" @ "member function \
					%ShlDS::OnGotMessage" >> %'DSMain' ~ %'Shell_DS',
				/ "function %InitConsole" @ %'YGlobal'
					>> 'DSMain',
				/ %'DSMain' $=
				(
					* "shells not destroyed before the application class \
						destruction" $since b269,
					* "wrong context buffer pointer initialized" @ "constructor"
						@ "class %DSScreen" @ "platform %MinGW32" $since b298
				)
			),
			/ "improved implementation" @ "function %IsAbsolute"
				@ %'YCLib'.'FileSystem' $=
			(
				+ "support for absolute path beginning with 'sd:/'"
					@ "platform %DS",
				+ "implementation" @ "platform %MinGW32"
			),
			* "destructor" @ "class %Shell" @ %'Core'.'YShell'
				$since $before b132 $= (- "wrong assertion"),
			* @ %GUI $since b298 $=
			(
				/ "constructor implementation" @ "class %TextList";
				* $comp "missing response to 'Enter' key"
					@ "classes %(TextList, ListBox, Menu, DropDownList)"
			),
			/ %'CHRLib' $=
			(
				+ DLD "static assertion for volatile type"
					@ "function template %FillByte" @ "header %smap.hpp",
					// To avoid undefined behavior.
				* "EOF not checked for distinguishing with invalid source \
					state and raising by conversion" @ "character conversion \
					for source type %std::FILE*" @ 'encoding conversion'
					$since b248
					// End valid byte in the file is always untouched. This \
						would probably cause infinite loop in buffering \
						file with size less than block size in text file \
						reader, for there is no check for EOF in the \
						loop for conversion while reading from the file.
			)
		),
		/ %'YCLib' $=
		(
			/ %'Input' $=
			(
				+ "header %Input.h for several input APIs";
				/ "APIs moved"
			),
			/ %'Debug' $=
			(
				+ "header %Debug.h for debugging and diagnose";
				/ "APIs moved"
			),
			(
				+ "wide character string path support"
					^ %'CHRLib'.'encoding conversion'
					@ %'FileSystem' @ "platform %MinGW32"
				$dep_to "wide character string path"
			)
		)
	),
	/ %'YReader'.'shells test example' $=
	(
		$dep_from "wide character string path";
		+ "wide character string path support" @ "platform %MinGW32"
	),
	+ "statically linked target" @ "platform %MinGW32" $=
		(+ $deploy "projects" @ "Code::Blocks workspace");
),

b298
(
	/ %'YFramework' $=
	(
		/ %'YCLib' $=
		(
			/ "native key types" @ %'Input',
			/ "file system APIs" @ %'NativeAPI' $=
			(
				+ DLD "assertions",
				* "missing directory validation" @ "function %opendir"
					@ "defined %YCL_MINGW32" $since b296
			);
			/% 'FileSystem' $=
			(
				/ "types and macros",
				(
					/ @ "file node iterator" $=
					(
						/ "simplified interface",
						/ "iteration performance improved for removal of \
							copying node names",
						* "broken member function %IsDirectory" $since b221,
						^ "class name %HFileNode" ~ "%HDirectory"
					)
				)
			)
		);
		/ %'YSLib' $=
		(
			/ %'GUI' $=
			(
				/ "key events response" @ "controls"
				* "implementation" @ "class %ListBox" $since b261
					$= (- "wrong assertion"),
				(
					$dep_from "file list interface";
					/ DLD "implementation" @ "class %FileBox"
				),
				+ "member function %Reset" @ "class template %GSequenceViewer",
				/ "default holding intervals"
					// Key: (240ms, 120ms) -> (240ms, 60ms); \
						Touch: (240ms, 60ms) -> (240ms, 80ms);
			),
			/ "key events response" @ "class %ShlDS" @ %'helpers'.'shells',
			/ %'Core'.'YFileSystem' $=
			(
				/ @ "class %Path" $=
				(
					* "directory path validation" $since b153;
					+ "member function %NormalizeTrailingSlash"
				);
				/ "class %FileList" @ $=
				(
					(
						/ "simplified interface";
						$dep_to "file list interface"
					),
					* "directory path validation" $since b153,
					/ "automatic normalizing trailing slash when validating \
						path string"
				)
			)
		)
		/ $design "file system APIs" %'YCLib',
		(
			$dep_from "make_shared",
			^ DLD "%ystdex::make_shared" ^ "%std::make_shared"
				// %std::make_shared with RTTI will enlarge size of object \
					files. Several implementations may cause access violation, \
					see $ref b298.
		),
		* DLD "implementation" @ "unit %YNew" @ %'Adaptor' $since b203
	),
	/ @ %'YBase' $=
	(
		+ $design "assertion macros",
		(
			+ "function template %ystdex::make_shared";
			$dep_to "make_shared"
		)
	),
	/ %'YReader' $=
	(
		/ $design "key event response" @ 'text reader',
		* "position argument overflow (wrapped by 512KB) when scrolling"
			@ %'hexadecimal browser' $since b268,
	)
),

b297
(
	/ %'YReader'. $=
	(
		/ %'text reader' %=
		(
			(
				(
					* "missing automatic scrolling response" $since b296;
					+ $comp "adjusting reader when key is down"
				);
				(
					/ "stopping automatic scrolling when setting panel entered";
					* $comp "smooth scrolling wrong behavior when the delay \
						set less than before" $since b292
				)
			),
			/ "default setting"
		),
		* "integer dividing by 0" @ "background" @ %'shells test example'
	),
	+ "function template %union_cast for type casting through anonymous union"
		@ %'YBase',
	/ %'YFramework' $=
	(
		/ "unit %YCommon" @ %'YCLib' $=
		(
			/ @ "defined %YCL_MINGW32" $=
			(
				/ "pixel format";
				/ DLD "implementation"
					@ "class %Color for efficiency bitmap transferring",
			)
			/ "function %ScreenSynchronize" @ "namespace %platform"
				>> "namespace %platform_ex"
		),
		/ DLD "implementation" @ "class %Palette" ^ "initializer lists"
			~ "assignments for efficiency" @ %'UI styles',
		+ "blit algorithm implementation without pixel format dependency"
			@ "unit YBlit" @ %'Service',
		* "using implicitly deleted copy constructor on events causing \
			ill-formed program" @ "class %BorderBrush" @ %'GUI' $since b295,
		/ %'Core' $=
		(
			* "missing virtual destructor" @ "class %GraphicDevice"
				@ %'YDevice' $since b296,
			/ "message %SM_INPUT value and parameter type"
				@ %'YMessageDefinition'
		),
		* "accessing uninitialized member" @ "font cache" $since $before b132,
		/ DLD "exposed message dealing interface" @ 'helpers',
	),
	/ DLP "VS2010 solution directories and filters",
	/ DLB (+ '-D_GLIBCXX_DEBUG' @ "compiler command line"
		@ "debug mode") @ "makefiles" @ "all projects"
),

b296
(
	/ %'YFramework' $=
	(
		/ %'YSLib' $=
		(
			/ %'Service' $=
			(
				/ @ "classes %(BitmapBuffer; BitmapBufferEx)" $=
				(
					* $doc "comments for destructor wrongly designed to copy \
						constructor" $since b240,
					* "implementation" @ "move constructor" $since b241;
					+ "member function %Swap with exception specification \
						%ynothrow";
						// The using of %std::swap for %BitmapBuffer forbids \
							the move assignment operator throwing exceptions \
							or the no throwing specification will be broken.
					* "missing assignment operators" $since $before b132
						$= (+ "copy and move assignment operators"),
						// The unification assignment cannot be used here \
							because the copy assignment may throw but move \
							assignment may not.
				),
				(
					/ DLD "accessors interface" @ "class %BitmapBuffer";
					$dep_to "simplified class %BitmapBuffer"
				),
				+ "explicitly defaulted copy and move constructors and \
					assignment operators" @ "class %TextRegion",
				- "member function %BitmapBuffer::BeFilledWith"
			),
			/ %'Core' $=
			(
				+ $dev "exception specification %ynothrow"
					@ "class %ValueObject" @ %'YObject';
				/ DLD @ "class %Message" @ %'message' $=
				(
					/ "unification operator" -> "copy assignment operator \
						without exception specification and move assignment \
						operator with exception specification %ynothrow";
					+ "exception specification %ynothrow" @ "member move \
						assignment operator with parameter type ValueObject&&"
				),
				(
					/ DLD "accessors interface" @ "class %GraphicDevice",
					$dep_from "simplified class %BitmapBuffer";
					- "polymorphisms" @ "class %Graphics"
				),
				* $doc @ "static member" @ "screen object types"
					$since $before b132,
				(
					- "default arguments for removal of %Size::FullScreen"
						@ "functions %(CopyTo, BlitTo)" @ "header %GDI",
					$dep_from "%Rect::FullScreen platform dependency from \
						widgets",
					$dep_from "%Rect::FullScreen platform dependency from \
						test";
					/ @ "static member %FullScreen" @ "classes %(Rect; Size)"
						-> "%Invalid";
					- DLD $comp "platform dependency" @ "%FullScreen"
				),
				/ $design "access control" @ "unit %YGDIBase" $=
				(
					+ "constructors with parameter const %Rect"
						@ "class template %GBinaryGroup, class %Size",
					/ @ "class %Rect" $=
					(
						/ "public base classes" -> "private bases",
						+ "several using members",
						+ "mutable member reference accessors"
					)
				)
			),
			/ %'GUI' $=
			(
				(
					/ "widget size bound to screen" @ "class %Desktop"
						~ "platform dependent %Rect::FullScreen";
					$dep_to "%Rect::FullScreen platform dependency from \
						widgets"
				),
				* $doc @ "function %LocateForParentContainer"
					@ "header yuicont.h" $since b242,
			),
			/ %'helpers' $=
			(
				+ %'DSMain';
				/ "class (DSScreen, DSApplication, MainShell)"
					>> %'DSMain'
			)
		),
		/ DLD %'YCLib' $=
		(
			/ "include guards macros renamed"
				@ "header %(NativeAPI.h, Platform.h)",
			- "unused macro %_ATTRIBUTE" @ "header Platform.h",
			/ ("several APIs" @ "namespace %platform"
				>> "namespace %platform_ex") @ "unit %YCommon",
			+ "file system interface adaptors for platform %MinGW32"
		)
	),
	/ $design @ %'YReader' $=
	(
		/ "simplified shells",
		(
			!^ "%Rect::FullScreen";
			$dep_to "%Rect::FullScreen platform dependency from test"
		)
	),
	/ %'YBase' $=
	(
		* "empty implementation" @ "macro %(YCL_LIKELY, YCL_UNLIKELY)"
			@ "header %ydef.h when '!(YCL_IMPL_GNUCPP >= 29600)'" @ $since b294,
		- "exception specification %ynothrow" @ "implementation"
			@ "macro %yunseq",
			// Allowing throwing exceptions is convenient but not safe, and \
				users now have resposibility for checking by self.
		+ "macro %YCL_ATTRIBUTE for implementation supported attributes"
	)
),

b295
(
	/ %'YFramework'.'YSLib' $=
	(
		/ %'GUI' $=
		(
			(
				/ "simplified refreshing" @ "class %Thumb",
				/ DLD "simplified refreshing" @ "classes %(CheckBox, ATrack, \
					AScrollBar, Panel, Window)",
				/ "improved refreshing" @ "class %AScrollBar";
				* $comp "background of buttons over refreshed" $since b294,
				* DLD $comp "background of scroll bars over refreshed"
					$since b294,
				/ $comp "refreshing performance improved"
			),
			+ DLD "partial invalidation support on refreshing"
				@ "class %BufferedTextArea",
			/ DLD "refreshing" ^ "border brush" @ "class %ProgressBar",
			* "memory leak" $since b243
				$= (+ "missing destructor" @ "class %AController"),
			+ DLD "copy and move assignment operators" @ "class %View"
		),
		/ %'Core'.'YShell' $=
		(
			/ "interface of shell switching";
			$dep_to "DS shell member functions",
			- "message %(SM_ACTIVATED, SM_DEACTIVATED)" @ %'YMessageDefinition'
		),
		/ %'helpers' $=
		(
			- "global desktop" @ %'YGlobal',
			/ %'Shell_DS' $=
			(
				^ "dynamically allocated individual desktop objects"
					~ "shared static desktop objects",
				- DLD "member functions %OnDeactivated",
				/ "member functions %OnActivated merged to constructors"
			);
			$dep_to "DS shell member functions"
		),
		/ DLD "more strict access controlling" @ "several classes",
	),
	/ %'YReader'.'shells test example' $=
	(
		(
			+ "refreshing interval" @ "class %FPSCounter";
			+ "FPS counter refreshing frequency limit",
		),
		/ $dev $design $dep_from "DS shell member functions" $=
		(
			- "all member functions %OnDeactivated",
			/ "all member functions %OnActivated merged to constructors",
		),
		/ $dev $design "shell objects" ^ "dynamic storage" ~ "static storage"
	)
),

b294
(
	/ %'YFramework'.'YSLib' $=
	(
		/ %'Core'.'YEvent' $=
		(
			+ "event priority type";
			/ "event container" ^ "%multimap" ~ "list";
			$dep_to "event with priority"
		),
		/ %'GUI' $=
		(
			/ "widget controller event %Paint delayed to initialize in \
				initialization of objects with class %Widget",
			+ "brush class %ImageBrush";
			/ "widget background painting" ^ ("new member %HBrush Background"
				@ "class %Widget") ~ "function %Components::Render",
			(
				$dep_from "event with priority";
				(
					+ "paint widget event priorities" @ "header %ywgtevt.h";
					$dep_to %'YReader'.'text reader'
				),
				/ DLD "border painting with priority"
					@ "classes %(TextList, DialogBox, DialogPanel)"
			),
			(
				* "missing clip area update" @ "class %BorderBrush" $since b284;
				* $comp "border overlapping between widgets" $since b284
				$dep_to %'YReader'.'text reader'
			),
			/ "widget refreshing interface" $=
			(
				^ "event %Paint" ~ "function %Components::Render";
				- "function %Components::Render"
			),
			+ "widget rendering state sharing between parent and children"
		),
		+ "protected boolean members to determine whether the desktops would \
			be updated" @ %'helpers'.'Shell_DS'
	),
	* $doc @ "function template %xcrease" @ %'YBase'.'YStandardEx'.'Utilities'
		$since b243,
	/ %'YReader'.'text reader'
	(
		/ $dev $design "border painting with priority" @ "class %ColorBox",
		* "border of setting panel overlapped by border of automatic scrolling \
			setting drop down list" $since b292,
		* "text encoding not synchronized with setting" $since b292
	),
	(
		/ "header %ydef.h" @ %'YBase' $=
		(
			+ $doc "comments for macros",
			+ DLD "branch predication hint macros"
		);
		^ $dev $design "branch predication hint macros" @ "whole project"
	)
),

b293
(
	/ DLD %'YBase' $=
	(
		* "broken implementation" @ "macro %yforward" @ "header %ydef.h"
			$since b245;
		+ "function templates %make_unique" @ "header %memory.hpp"
	);
	/ %'YFramework' $=
	(
		/ DLD "helper function usings" @ "header yref.hpp" @ %'Adaptor',
			// Using of std::make_shared leads to code bloat for RTTI.
		* "member function template parameter types missing '&&'"
			@ "header %(yevent.hpp, yfunc.hpp)" @ %'Core' $since b210,
		* "constructor template parameter type missing '&&'" @ "class %Widget"
			@ 'GUI' $since b258,
		/ %'GUI' $=
		(
			/ "rendering logic" @ "unit %(YRenderer, YWidget)" $=
				// Performance is improved a little.
			(
				/ "renderers" $=
				(
					/ "member function %Refresh" -> "%Paint",
					/ "member function %Validate" @ "class %BufferedRenderer"
				),
					// Now calling event %Paint instead of widget member \
						function %Refresh.
				/ "function %Render",
					// Now calling widget member function %Refresh instead of \
						renderer member function %Refresh.
				/ "function %PaintChild"
					// Now calling renderer member function %Paint instead of \
						event %Paint.
			),
			+ "brush class %SolidBrush",
			/ DLD "base controller methods" @ "class %AController"
		),
		/ "constructors" @ "class %Timer" @ %'Service',
		* DLD "missing 'const' when casting" @ "implementation"
			@ "class %ValueObject" $since b217
	),
	* "wrong pixel offset when stopping smooth scrolling"
		@ %'YReader'.'text reader' $since b292
),

b292
(
	/ %'YFramework' $=
	(
		* "implementation" @ "functions %(WaitForInput, WaitForKey)"
			@ "unit %YCommon" @ %'YCLib' $since b291,
		/ %'YSLib' $=
		(
			* DLD "strict ISO C++ code compatibility" $since b273
				$= (/ "assertion strings" @ "implementation unit %TextManager"
					@ %'Service' !^ "%__FUNCTION__"),
			/ %'GUI' $=
			(
				/ @ "class %DropDownList" $=
				(
					* "selection of top/bottom spaces to extract list to be \
						shown" @ $since b290,
					/ "fixed position of arrow and right margin",
					/ "refreshing as pressed down when list shown"
				),
				* "member function %ListBox::ResizeForPreferred when \
					limit width is zero" $since b282,
				(
					+ "container setter" @ "class template %GSequenceViewer";
					+ "member function %SetList" @ "classes %(MTextList, \
						TextList, ListBox, DropDownList)",
				)
			)
		)
	),
	/ %'YReader'.'text reader' $=
	(
		+ "implementation" @ "custom encoding switching UI",
		/ "contiguous repeated bookmarks would not be inserted to reading list \
		  when random locating",
		+ "smooth scrolling mode",
		+ "automatic scrolling duration settings"
	),
	/ DLP "library using"
		$=(^ "updated library freetype 2.4.9" ~ "modified freetype 2.4.8")
),

b291
(
	/ %'YBase'.'YStandardEx' $=
	(
		+ "helper 2 function templates %arrlen";
		$dep_to 'YReader'.'text reader'
	),
	/ %'YFramework' $=
	(
		/ DLD "all encoding parameter type" ^ 'Encoding' ~ 'const Encoding&'
			@ %'CHRLib',
		/ "timers" @ %'YCLib' $=
		(
			/ DLD "simplified implementation" ^ "libnds functions"
				~ "direct operations on registers";
				// Now only counter 2 is used by this library instead of \
					all the 4 counters on ARM9;
			+ "high resolution counter";
				// It provides at most resolution as nanoseconds.
			$dep_to %'YReader'.'shells test example',
			/ DLD "simplified implementation" @ "input functions"
		);
		/ "timers" @ '%YSLib'.'Service'
		(
			+ "class %HighResolutionClock" ^ "%std::(duration, time_point)",
			+ "duration and time point types",
			^ "high resolution counter" ~ "milliseconds" @ "class %Timer"
		)
	),
	/ DLP "library using"
		$= (^ "updated devkitARM release 37" ~ "devkitARM release 35"),
	/ %'YReader' $=
	(
		/ "more accurate FPS counter" @ %'shells test example',
		/ $dev $design "simplified implementation"
			^ "function template %ystdex::arrlen" @ %'text reader'
	)
),

b290
(
	/ %'YBase'.'YStandardEx' $=
	(
		/ DLD @ %'TypeOperation' $=
		(
			+ "meta operation %array_decay",
			(
				+ "meta operation %qualified_decay";
				$label "meta qualified"
			),
			+ "meta operation %array_ref_decay"
		),
		/ @ %'Iterator' $=
		(
			+ "class template %pointer_iterator";
			+ "meta operation to classify pointer types";
			+ "non-class type iterator support"
				@ "class template %transformed_iterator",
			(
				$dep_from "meta qualified";
				/ DLD "qualified or array type decaying"
					@ "function template %make_transform"
			)
		)
	),
	/ %'YFramework'.'YSLib' $=
	(
		+ "stream encoding checking without BOM" @ "class %TextFile"
			@ %'Service',
			// At most first 64 bytes would be read for checking, \
				now only for UTF-8 and GBK.
		$dep_to "no BOM checking";
		+ "top and down space comparing when list shown" @ "class %DropDownList"
	);
	/ %'YReader'.'text reader' $=
	(
		* "reader content and text information not refreshed after switched to \
			empty file using reading list" $since b289,
		(
			$dep_from "no BOM checking";
			+ $comp "automatic encoding checking of UTF-8 without BOM"
		),
		+ "custom encoding switching UI"
			// Actually switching is not implemented yet.
	)
),

b289
(
	/ "algorithm function templates %(erase_all, erase_all_if)"
		@ %'YBase'.'Algorithms' $=
	(
		/ "return value" @ "overloading for sequence containers",
		(
			+ "overloading template for associative containers";
			$dep_to %'YFramework'.'YSLib'.'Core'.'YMessage'
		)
	)
	/ %'YFramework'.'YSLib' $=
	(
		+ DLD "protected member to expose current message"
			@ "class %Application" @ %'Core'.'YApplication';
		/ %'helpers'.'YGlobal' $=
		(
			/ %'YGlobal' $=
			(
				+ "exposed const current message getter",
				/ DLD "implementation" @ "main message loop"
					@ "member function %DSApplication::Run"
			)
		),
		(
			/ DLD "bound control pointer assignment" @ "constructor"
				@ "class %Control" @ !^ "std::mem_fn" @ 'GUI';
			$dep_to "class %ShlExplorer" @ %'shells test example'
		),
		/ @ %'Core'.'YMessage' $=
		(
			+ "removing operation with specified bound and shell"
				@ "class %MessageQueue";
			$dep_to %'YReader'.'text reader'
		),
		/ @ "class %ProgressBar" %'GUI' $=
		(
			/ "refreshing efficiency improved",
			/ "zero max value automatic set as 1" @ "constructor"
		),
		/ %'Service' $=
		(
			/ @ "class %Timer" $=
			(
				(
					+ "member function %IsActive";
					$dep_to %'YReader'.'text reader'
				),
				/ "implementation" @ "friend functions %(Activate, Deactivate)"
			),
			/ @ "class %TextFileBuffer" $=
			(
				- $define "assertion of block number" @ "constructor";
				(
					/ "no block required for empty file";
					/ $comp "size of empty file shown as 0 rather than 1";
					$dep_to "reader box" @ %'YReader'.'text reader'
				)
			)
		)
	),
	/ %'YReader' $=
	(
		/ @ "class %ShlExplorer" @ %'shells test example',
		(
			- "invalidating area test",
			/ DLD "bound control pointer assignment" @ "constructor"
				@ !^ "std::mem_fn"
		),
		/ %'text reader' $=
		(
			+ "automatic scrolling",
			/ @ "reader box" $=
			(
				+ "buffered rendering",
				(
					+ "empty file check"
						@ "member function %UpdateData";
					* $comp "crash on opening empty file" $since b273
				),
				/ "action for empty file canceled"
			)
		),
		/ "set ignoring background on buffered rendering"
			@ %'hexadecimal browser'
	)
),

b288
(
	/ DLD %'YBase'.'YStandardEx' $=
	(
		/ %'TypeOperations' $=
		(
			+ "template class %n_tag";
			+ "typedef n_tag<0> %first_tag",
			+ "typedef n_tag<0> %second_tag"
		),
		/ %'Iterator' $=
		(
			(
				+ "class template %transformed_iterator";
				+ "function template %make_transform",
			),
			+ "class template %pair_iterate for selecting members";
			$dep_from 'TypeOperations';
			+ "manipulators for selection members";
			+ "pipe operators for selecting members"
		),
		$dep_to 'YReader'.'text reader'
	),
	/ %'YFramework'.'YSLib' $=
	(
		/ DLD "simplified implementation" @ "class %Thumb" @ %'GUI',
		* DLD "comments" @ "header %ycutil.h" $since b281,
		/ DLD "implementation" @ "idle handling",
		/ %'Core' $=
		(
			+ "member function %MessageQueue::GetMaxPriority"
				@ "implementation" @ "message peeking" @ %'YMessage',
			$dep_to "implementation" @ "main message loop";
			/ %'YMessageDefinition' $=
			(
				- "shell message identifier %SM_DROP",
				+ "shell message identifier %SM_TASK"
			)
		),
		/ DLD "implementation" @ "main message loop"
			@ "member function %DSApplication::Run" @ %'YGlobal'
	),
	/ %'YReader' $=
	(
		/ DLD "simplified event handlers" @ "class %ShlExplorer"
			@ %'shells test example' ^ "lambda expressions"
			~ "static member functions",
		/ %'text reader' $=
		(
			$dep_from %'YBase'.'YStandardEx'.'Iterator';
			/ DLD "simplified implementation",
			* "subsequent bookmarks not erased when loading a file"
				@ "reading list" $since b287
		)
	)
),

b287
(
	/ %'YFramework'.'YSLib' $=
	(
		/ %'GUI' $=
		(
			/ "right component of margin" @ "constructor"
				@ "class %DropDownList",
			* "wrong vertical alignment get"
				@ "member function %MLabel::PaintText" $since b208,
			+ "focus requesting/releasing" @ "functions %(Show, Hide)"
				@ "unit %YWidget",
			* $comp "wrong alignment when refreshing \
				widget" @ "class %DropDownList",
			/ "GUI state abstraction" $=
			(
				+ "class %GUIState",
				/ "common state implementation" ^ "static shared %GUIState"
					~ "state of GUIShell";
				- "class %GUIShell"
			)
		),
		/ "string and path interface" $=
		(
			+ "member function %String::GetMBCS",
			- "function %MBCSToString",
			/ DLD "inheritance" @ "class %Path"
				@ %'Core'.'YFileSystem' ^ "class %String",
			- "local encoding alias %CP_Local"
			/ "all pattern 'CP_*'" => "pattern 'CS_*'"
		),
		+ DLD "public inheritance %enabled_shared_from_this"
			@ "class %Shell" @ %'Core'.'YShell'
	);
	/ %'YReader'.'text reader' $=
	(
		* "functions of key %'Left' down" $since b286,
		/ "stopping handling key events when setting panel is shown",
		/ $dev $design "reader manager as sessions",
		+ "cross-session shared shell state support"
			@ ("last reading list", "settings")
	)
),

b286
(
	/ %'YFramework' $=
	(
		/ DLD "native path types" @ %'YCLib';
		/ %'YSLib' $=
		(
			/ DLD "simplified inheritance" @ "class %Path"
				@ %'Core'.'YFileSystem',
			+ "constructor %String(ucs2string&&)" @ "class %String",
			(
				/ "class %Padding as literal type";
				+ DLD "literal type object %DefaultMargin"
					@ "header %TextBase.h";
				/ "margin setting" @ "constructor" @ "class %TextBase",
				/ %'GUI' $=
				(
					+ "return value" @ "function %Enable" @ "unit %YControl",
					/ "margin setting" @ "constructor" @ "class %DropDownList"
				)
			)
		)
	),
	/ %'YReader'.'text reader' $=
	(
		(
			+ "class %BookMark";
			+ "class %ReadingList";
			+ "shell last reading list"
				// This list is only valid in the same shell activated lifetime.
		),
		/ $dev $design "static data members" @ "class %ReaderManager"
			>> "class %ShlReader",
		/ $design "simplified implementation of activating and deactivating",
		/ "functions of key %'(X, Y, L, R, Left, Right)' down",
		* "random locating error" @ "member function %DualScreenReader::Locate"
			$since b271
	)
),

b285
(
	/ %'YFramework'.'YSLib'.'GUI' $=
	(
		- "hosted menu referent pointer" @ "unit %Menu",
		+ "member functions %(SetSelected, ClearSelected)" @ "class %ListBox";
		+ "item text matching when list shown" @ "class %DropDownList",
		* "viewer length of text list no synchronized when initialized"
			$since b203
	),
	* "menu not hidden when touching reader box" @ %'YReader'.'text reader'
		$since b283
),

b284
(
	/ %'YFramework'.'YSLib' $=
	(
		/ %'GUI' $=
		(
			/ "border APIs" @ "unit Border" $=
			(
				+ "class BorderBrush",
				* "border shown when widget is transparent" $since b276
			),
			+ "auto request focus" @ "class %ListBox for proper border color",
			* "focus not cleared when container being touched down" $since b283
		),
		- "shell message on activating and deactivating on shells"
			@ "classes %(Shell, ShlCLI)"
	),
	/ "menu selected index cleared when shown" @ %'YReader'.'text reader'
),

b283
(
	/ %'YFramework'.'YSLib'.'GUI' $=
	(
		* @ "destructor" @ "class %Widget"
			$since b240 $= (- "calling event %LostFocus"),
			// It depends on itself to be a complete object in \
				its lifetime which does not fit for destructing.
		(
			/ @ "class %View"
			(
				+ "dependency pointer";
				/ "visible and transparent property setter and getter"
			);
			+ "top widget view state synchronizing" @ "class %DropDownList"
		),
		/ "focus operation for containers" @ "class %YGUIShell"
			>> "class %Control",
		/ "focus requesting on handling event %TouchDown of controls \
			only when the strategy is bubble";
		/ @ "class %DropDownList" $=
		(
			* "top widget not removed when destructing" $since b282,
			(
				+ "top widget focus requesting";
				* $comp "top widget not hid when touching again"
					$since b282
			)
		)
	);
	* $comp "crashing when switching shell after testing of drop down list"
		@ %'YReader'.'shells test example' $since b282
),

b282
(
	/ %'YFramework'.'YSLib' $=
	(
		/ %'GUI' $=
		(
			/ DLD "APIs for text list resizing for contents",
			(
				/ DLD "unit names" @ "directory %UI";
				+ "class %DropDownList" @ "unit %ComboList"
			),
			- "assertion" @ "constructor" @ "classes %(HorizontalScrollBar, \
				VerticalScrollBar),
			* "wrong comments" @ "constructors of track classes" $since b173
		),
		* "wrong result for function %Intersect" @ "unit %YGDIBase" $since b227
	);
	+ "drop down list test" @ "class %ShlExplorer"
		@ %'YReader'.'shells test example'
),

b281
(
	/ %'YFramework'.'YSLib' $=
	(
		/ "text list refreshing implementation" @ %'GUI' $=
		(
			* "text not fully shown at bottom" $since b190;
			/ "high light area height"
		),
		DLD
		(
			+ "deleting second member debug functor" @ "header %ycutil.h";
			/ "several implementations" @ "unit %(YTimer, Menu, \
				YUIContainer, YEvent, YMessage)" ^ "%std::for_each" ~ "for loop"
		),
		/ @ "unit %YString" $=
		(
			/ "enhancement of constructors of class %String";
			- "function %MBCSToString"
		),
		/ "event call catching %std::bad_function_call silently"
	),
	/ "unit %Shells" @ %'YReader' $=
	(
		/ "simplified testing implementation",
		+ DLD "font family names list" @ "class %ShlExplorer"
	),
	/ %'YBase' $=
	(
		* DLD "wrong Doxygen comments 'defgroup functors'"
			@ "header %utility.hpp" $since b193,
		* DLD "wrong Doxygen comments file 'brief'"
			@ "header %any.hpp" $since b243
	)
),

b280
(
	/ %'YReader'.'text reader' $=
	(
		+ "automatic updating font before print text";
		/ @ "setting panel" $=
		(
			* "button text" $since b279,
			/ "label text",
			+ "text size setting"
		)
	),
	/ %'YFramework'.'YSLib' $=
	(
		/ DLD "simplified font APIs",
		/ "font APIs" $=
		(
			/ DLD "several getters" @ "class %FontCache" >> "class %Font",
			/ DLD "stored state" @ "class %Font";
				// It brings the ability of automatically state synchronizing \
					with FT_Size with almost no efficiency changing, \
					but 'sizeof(Font)' grows obviously.
			/ "eliminating need of updating font"
		),
		* "undefined behavior raised when calculating advance of or rendering \
			a glyph and the small bitmap descriptor is null" $since before b132
	)
),

b279
(
	/ %'YFramework'.'YSLib' $=
	(
		/ %'GUI' $=
		(
			(
				/ "support widget moving to top" >> "class %Panel"
					~ "class %Window";
				/ "support for class %Panel" ~ "class %Window"
					@ "function %RequestToTop"
			)
			* DLD "assertion string" @ "4 functions %(MoveToTop, \
				MoveToBottom, MoveToLeft, MoveToRight)" $since b171,
			/ "member funtion %Desktop::ClearContents" >> "class %Panel"
		),
		/ DLD "default idle handler implementation"
			!^ "function %platform::AllowSleep to forbid sleeping \
			when the main message queue is not empty"
	),
	/ %'YReader'.'text reader' $=
	(
		/ $dev $design "simplified color APIs" @ "class %DualScreenReader";
		/ @ "setting panel" $=
		(
			/ "control appearance" !^ ("border", ^ "the top desktop")
			+ "text color setting"
		)
	)
),

b278
(
	/ %'YFramework' $=
	(
		+ "sleeping status configuring function %AllowSleep" @ %'YCLib';
		/ %'YSLib' $=
		(
			/ "font APIs" $=
			(
				* "memory leak when loading duplicate typefaces"
					@ "member function %FontCache::LoadTypefaces" $since b277,
				/ DLD "font file loading implementation" @ "constructor of \
					class %Typeface"
					~ "member function FontCache::LoadTypefaces"
			),
			/ 'GUI' $=
			(
				* "missing painting of background for parent of buffered widgets"
					$since b225,
				+ "forced background painting ignorance state"
					@ "class %BufferedRenderer"
			),
			+ DLD "sequence function application function template %seq_apply"
				@ "header %yfunc.hpp",
			+ "helper function object class %ContainerSetter",
			/ DLD "default idle handler implementation"
				^ "function %platform::AllowSleep to forbid sleeping \
				when the main message queue is not empty";
			* "VRAM not flushed or crashing when opening lid on real DS"
				$since $before b132
		)
	),
	/ DLD "simplified implementation" @ "file %main.cpp"
		@ "project %YSTest_ARM7",
	/ %'YBase' $=
	(
		/ DLD "implementation for meta programing constant"
			^ "struct %integral_constant" ~ "static yconstexpr objects",
		/ "unsequenced expression returning first expression"
			^ "macro %yforward"
	);
	/ %'YReader'.'text reader' $=
	(
		/ "reader box shown as default",
		/ $design "simplified container pointer setting implementation"
			^ "function template %seq_apply"
	)
),

b277
(
	/ %'YReader'.'text reader' $=
	(
		+ "labels" @ "class %ColorBox",
		+ "labels" @ "setting panel"
	),
	/ %'YFramework'.'YSLib' $=
	(
		* "rounding of alpha blending color component" $since $before b132,
		* DLD "platform-dependent bool type xor operation"
			@ "function template %Blit" $since b189,
		/ "font APIs" $=
		(
			/ DLD "struct %FontStyle reimplemented as enum class",
			/ DLD "simplified style names lookup function",
			* "wrong implementation" @ "all predicates" @ "class %Font"
				$since $before b132,
			- "class %FontFile",
			/ DLD "data structure" @ "class %FontCache"
		)
	)
),

b276
(
	/ %'YBase' $=
	(
		/ "template %fixed_point" $=
		(
			* "wrong implementation" @ "casting to floating types" $since b260,
			* DLD "wrong comments" $since b260,
			* "wrong implementation" @ "member function %operator*= \
				for signed types" $since b260,
			- "explicit @ casting operator"
		)
	);
	/ %'YFramework' $=
	(
		/ "storing 8-bit RGBA values in native color type class %Color"
			@ %'YCLib';
			// It spends more space but is more efficient in time.
		/ %'YSLib' $=
		(
			+ "adding/removing member function handlers with object type \
				distinct to class type" @ "class %GEvent",
			/ %'GUI' $=
			(
				+ DLD "member function %OnTouch_Close" @ "class %Control";
				+ "class %ToolBox",
				/ "simplified color conversion APIs" @ "unit %YStyle",
				/ "color conversion APIs" ^ "%ystdex::fixed_point",
				+ "border APIs",
				/ "default related border color" @ "class %ProgressBar",
				/ "intersection calculation" >> "function %PaintChild#1"
					~ "function %Render"
			)
		)
	);
	/ %'YReader'.'text reader' $=
	(
		/ $design "simplified implementation",
		/ "control appearance" @ "setting panel" ^ "border"
	)
),

b275
(
	/ %'YFramework' $=
	(
		/ %'YSLib' $=
		(
			/ DLD "decomposed unit %YText into unit %(TextBase.cpp, \
				CharRenderer, TextRenderer, TextLayout)",
			/ %'GUI' $=
			(
				+ "control default handler for requesting to top and focus \
					for event %TouchDown",
				- "%TouchDown handling for requesting to top and focus"
					@ "member function %GUIShell::ResponseTouch";
					// Clearing focusing pointer of containers is reserved in \
						%GUIShell::ResponseTouch, but now can only occur \
						before the control requesting for focus \
						in its parent widget.
					// It means that the requesting handler can be unloaded \
						by user manually.
				+ "hosted referent pointer" @ "class %MenuHost",
				/ @ "unit %YWidget" $=
				(
					* "no effect of functions %(Show, Hide) \
						when used with widgets not buffered" $since b229,
					+ "function %Close for hiding and releasing focus"
				)
				* "missing thumb refreshing when invalidated area not \
				  intersecting with thumb" @ "member function %ATrack::Refresh"
				  $since b235,
				* "wrong position argument" @ "implementation"
					@ "member function %Button::Refresh" $since b266
			),
			- DLD "dependency of header %platform.h" @ "header %ynew.h",
		),
		/ %'YCLib' $=
		(
			/ "efficiency improved" @ "function %ScreenSynchronize"
				^ "asynchronous DMA copy function %dmaCopyWordsAsynch"
				~ "function %dmaCopy"
		)
	);
	* DLD "trivial comments not removed for %(is_not_null, is_null)"
		$since b254,
	+ "color picker class %ColorBox";
	/ %'YReader'.'text reader' $=
	(
		/ "behavior on clicking menu button",
		+ "setting panel"
	),
	/ DLP "all code and document files" @ "project %YSTest"
		>> "project %YFramework",
	(
		- "string functions not strictly compliant to ISO" @ "all projects";
		/ DLB "Makefile" @ "project %(YBase, YFramework)"
			^ "-std=c++0x" ~ "-std=gnu++0x"
	)
),

b274
(
	/ %'YFramework'.'YSLib' $=
	(
		/ "text rendering" @ %'Service' $=
		(
			- DLD "all functions %SetMarginOf",
			* DLD "2 functions template %PrintLine cannot used with \
				iterator parameters without postfix operator++" $since b270,
			* DLD "assertion string"
				@ "function %FetchResizedBottomMargin" $since b273,
			* DLD "assertion string" @ "function %FetchResizedLineN"
				$since b252,
			* "implementation" @ "member function %TextLineNEx"
				@ "class template %GTextRendererBase" $since b267，
			* "implementation" @ "function %FetchLastLineBasePosition"
				$since $before b132 ^ "ascender" ~ "descender",
			* "implementation" @ "function %FetchResizedLineN" $since b252
		),
		/ DLD "lessened header dependencies"
	);
	/ %'YReader'.'text reader' $=
	(
		/ @ "class %DualScreenReader" $=
		(
			+ $design "common margin",
			* $design "wrong line number"
				@ "member functions %(UpdateView, Execute)" $since b270,
			* "wrong view area shown after set line gaps" $since $before b132,
			+ "reading area resizing interface",
			* "wrong behavior for scrolling line up when top margins differed"
				$since $before b132;
			+ "top and bottom margins balancing"
		);
		/ "appearance"
		(
			+ "reading area resizing when showing or hiding reader box",
			/ "boxes transparency"
		)
	)
),

b273
(
	+ DLD "class %pseudo_output" @ %'YBase'.'YStandardEx'.'Any',
	/ %'YFramework' $=
	(
		/ %'CHRLib' $=
		(
			+ "runtime character width getters",
			/ "encoding mapping APIs exposed",
			(
				/ DLD "UCS2 output type of encoding mapping";
				+ "pseudo-converting-to-UCS2 APIs for width calculation"
			),
			/ "UTF-8 to Unicode code point mapping" $=
			(
				* "wrong behavior for 2 byte sequence characters" $since b250,
				+ "invalid byte checking",
				+ "4 byte sequence characters support"
			),
			(
				+ "enum class %ConversionResult";
				/ "converting-to-UCS2 APIs" ^ "enum class %ConversionResult"
					~ "byte as return type"
			)
		);
		/ %'YSLib' $=
		(
			+ "mapping buffering" @ "class %TextFileBuffer"
				@ "unit %TextManager" @ %'Service',
			/ DLD "member functions and types" @ "classes %(File, TextFile)"
		),
	),
	/ "view" @ "class %TextInfoBox" @ %'YReader'.'text reader',
		// Efficiency for opening and random access operations to \
			large files improved obviously due to using of mapping buffering.
	* $doc $dev $design "several minor Doxygen warnings" @ $since b196
),

b272
(
	/ %'YFramework'.'YSLib' $=
	(
		/ %'Core' $=
		(
			/ "simplified implementation" @ %'YMessage';
			/ %'YApplication' $=
			(
				- "messages producer calling" @ "function FetchMessage"
				- "default log object" @ "class %Application",
				- DLD "try blocks" @ "all 2 functions %SendMessage";
				/ DLD "logging class %Log" >> "implementation unit %YGlobal"
					@ "directory %Helper"
			)
		);
		/ %'YGlobal' $=
		(
			/ "input message target type" ^ "class %InputContent"
				~ "shared_ptr<InputContent>";
			/ "efficiency improved" @ "function %Idle",
			/ DLD "simplified implementation" @ "function %::main"
		),
		^ "rounding" @ "member function %ProgressBar::Refresh" @ %'GUI'
	),
	/ %'YReader'.'text reader' $=
	(
		* "scrolling down disabled after text bottom reached" $since b271,
		+ "return focus back to reader when box controls closed",
		+ "bottom position shown as distinct color" @ "box controls",
		/ "widgets layout" @ "class %ReaderBox"
	)
),

b271
(
	/ %'YReader'.'text reader' $=
	(
		+ "text reading progress information shown on both information boxes",
		/ "reading information indicating" @ "class %ReaderBox"
			^ "progress bar" ~ "track";
		+ "random locating of text by progress bar",
		/ "key responding all the time except menu shown"
	),
	/ %'YFramework' $=
	(
		/ %'YSLib' $=
		(
			/ %'GUI' $=
			(
				/ DLD "implementation" @ "class %ScrollEventArgs"
					^ "class template %std::pair",
				/ @ "class %ProgressBar"
				(
					/ "main base class switched to class %Control"
						~ "class %Widget",
					/ "value type switched to float" ~ "u16"
				)
			),
			/ %'YMessage' $=
			(
				/ DLD "implementation of copying" @ "class %Message",
				/ @ "class MessageQueue" $=
				(
					- "redundant member functions",
					+ DLD "copy assignment and move assignment merged \
						as unifying assignment"
					(
						+ "minor efficiency",
						+ "ability to use copy-elision optimization",
						* "exception safety of move assignment" $since b211
					)
				)
			)
		),
		* "wrong state in file stream conversion" @ %'CHRLib'.'CharacterMapping'
			$since b249
	),
	/ "behavior like class template %std::istream_iterator"
		@ %'YBase'.'YStandardEx'.'CStandardIO'.'class %ifile_iterator'
),

b270
(
	/ $design %'YReader'.'hexadecimal browser' $=
	(
		/ "resource management" ^ "class %unique_ptr" ~ "built-in pointers",
		/ "minor model interface"
	),
	/ "text rendering functions 'Print*' ignoring newline characters";
	+ "macros for bitmask operations" @ "header %YBaseMacro",
	/ %'YReader'.'text reader' $=
	(
		// Using of unique_ptr and more contents of information box \
			at readers make it significantly slower than before in \
			%ShlExplorer when running on DeSmuMe at debug configuration.
		/ $design "resource management" ^ "class %unique_ptr"
			~ "built-in pointers"
		/ "unit %DSReader" $=
		(
			* "newline character cannot be print before EOF"
				$since $before b132,
			/ "more accurate indicating of text position when scrolling",
			/ "merge several interface as reading commands"
		),
		/ "size and content of file information box"
	),
	/ "implementation" @ "class %FontCache" $=
	(
		/ DLD "interface" @ "member function %GetGlyph";
		/ "avoiding unnecessary rendering" @ "member function %GetAdvance"
	)
),

b269
(
	/ %'YReader' $=
	(
		/ %'hexadecimal browser' $=
		(
			+ "event subscription of actively locating the view"
				@ "class %HexViewArea",
			/ "information updating instantly to panel",
			+ "view scrolling by pressing arrow/L/R",
			+ "exiting by pressing Esc",
			* "encoding of updated path information" $since b263
		),
		/ "focus automatically requesting"
	),
	/ %'YFramework' $=
	(
		/ DLD "simplified application instance management" $=
		(
			^ "assertion to check singleton constraint" ~ "access control",
			+ "destructor" @ "class %DSApplication",
			/ "main function cleanup"
		),
		* DLD "null function call" @ "destructor" @ "class %Application"
			$since b243,
		/ %'YFramework'.'YSLib'.'GUI' $=
		(
			- "unnecessary view updating for empty list on event %KeyDown"
				@ "class %TextList",
			/ DLD "simplified container control inheritance",
			+ "hiding by press Esc" @ "class %Menu"
		),
		/ DLD "message loop" @ "function %::main"
	)
),

b268
(
	/ %'YFramework'.'YSLib'.'GUI' $=
	(
		/ %'GUI' $=
		(
			+ "invalidation on event %(Move, Resize) as default"
				@ "class %Control";
			/ "unnecessary invalidation reduced"
				@ "function %OnTouchMove_Dragging" @ "unit %Control",
			/ "unnecessary invalidation reduced"
				@ "classes %(ATrack, Button, ListBox, MenuHost, TextList)",
			+ "partial invalidation support on event %Selected"
				@ "class %TextList",
			* "missing invalidation of the thumb before setting thumb position"
				@ "classes %(HorizontalTrack, VerticalTrack, ATrack)"
				$since b224,
			* "minor line segments length on background"
				@ "member function ATrack::Refresh" $since b167,
			- "widget visibility check" @ "function %BufferedRenderer::Refresh",
			/= DLD "confirmed no need of partial invalidation support \
				on event %Selected" @ "class %ATrack",
			(
				DLD
				(
					+ "simple generic UI event argument class template \
						%GValueEventArgs";
					/ "simplified implementation" @ "class %IndexEventArgs"
				);
				+ "event subscription of active locating the view"
					@ "class %TextList"
			)
		),
		* $design "ill-formed implementation of copy assignment function"
			@ "class template %GEvent" $since b207,
		/ $design "macro %ImplExpr for compatibility" @ "header %ybasemac.h"
	),
	/ %'YReader' $=
	(
		/ "unnecessary invalidation reduced" @ "class %HexReaderManager",
		* "invalid scrollbar area shown for refreshing" @ "class %HexViewer"
	),
),

b267
(
	/ %'YFramework'.'YSLib'.'GUI' $=
	(
		/ "simplified renderer interface",
		* "unstable length of scrolling thumb" @ "class %ATrack" $since b264
	),
	* "wrong buffered text rendering behavior" $since b266,
	^ "DMA copy on synchronizing to VRAM" @ "library %YCLib"
),

b266
(
	/ DLP "library using"
		$=(^ "updated modified freetype 2.4.8" ~ "modified freetype 2.4.5"),
	/ %'YFramework'.'YSLib' $=
	(
		/ %'GUI' $=
		(
			/ "invalidation algorithm",
			/= DLD "confirmed no need of partial invalidation support \
				on event %Selected" @ "class %Button",
			/ "simplified button invalidation",
		),
		(
			/ "CRTP static polymorphic macros" @ "header %ybasemac.h",
			/ DLD "simplified implementation"
				@ "header %(ydef.h, ybasemac.h, yevt.hpp)";
			/ "text renderers" ^ "CRTP" ~ "polymorphic classes"
		)
	)
),

b265
(
	/ %'YFramework'.'YSLib' $=
	(
		+ "partial invalidation support for text rendering";
			// It makes efficiency decreased obviously \
				for non-overlapped widgets.
		/ %'GUI' $=
		(
			/ "window classes hierarchy" ^ "class %Panel",
			/ "refreshing algorithm" @ "class %Frame::Refresh" $=
			(
				+ "support for unbuffered windows",
				/ "minor efficiency improvement"
			),
			+ "member function %Panel::Refresh",
			+ "partial invalidation support"
				@ "classes %(MLabel, TextList, Menu)",
			(
				* "sender checking missing for event %TouchMove"
					@ "class %TextList" $since b264;
				* "wrong behavior of listbox on event %TouchHeld when touching \
					widget changed" $since b219
			)
		),
		/ "intersection algorithm improvement for non-trivial result"
	)
),

b264
(
	/ %'YReader' $=
	(
		/ "format of time strings showed by information labels" @ "hexadecimal \
			browser" ^ "custom functions" ~ "function %std::ctime",
		* "file information box cannot be shown" @ "text reader" $since b263
	),
	/ %'YFramework'.'YSLib'.'GUI' $=
	(
		/ @ "class %ATrack"
		(
			* "scrolling overflow of value greater than about 0x1998"
				$since b260 $= (/ "value type" ^ "float"
				~ "ystdex::fixed_point<u32, 16>"),
			(
				/ "value mapping for scrollable length" ~ "total value range";
				* "wrong value calculation when minimum thumb length reached"
					@ "class %ATrack" $since b193
			),
			/ "GUI shell" ^ "temporary argument" @ "event %Enter",
			(
				* "wrong touch coordinate for event %TouchHeld when touching \
					widget changed" $since b219;
				* $comp "wrong behavior when the corresponding relative \
					touching coordinate component below zero on dragging"
					@ "class %ATrack" $since b219
			),
			/ DLD "minor renderer interface"
		)
	)
),

b263
(
	/ %'YFramework'.'YSLib'.'GUI' $=
	(
		* "event %Confirmed checking fail for items out of initial view scope"
			$since b262,
		/ DLD "rearranged rendering and updating implementation"
	),
	* DLD "minor unexpected name pollution" @ "header %algorithm.hpp"
		@ "library %YStandardEx" $since b254,
	+ "information labels" @ "hexadecimal browser" @ "shells test example"
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
	/ %'YFramework'.'YSLib'.'GUI' $=
	(
		/ DLD "simplified interface" @ "class %ATrack";
		* "wrong alignment" @ "listbox when alignment is non-zero value \
			and scrolling down to end" $since b193
	)
),

b260
(
	/ %'%YBase' $=
	(
		+ DLD "several operations for integer types",
		+ DLD "reusable overloaded operators";
		+ "fixed-point arithmetic template %fixed_point"
	);
	* "improper underlying type of scroll event argument type"
		@ %'YFramework'.'YSLib'.'GUI' $since b201
),

b259
(
	/ %'YFramework'.'YSLib'.'GUI' $=
	(
		* "focusing pointer not removed when removing widgets"
			@ "classes %(Panel, AFrame) $since b258;
		+ DLD "subobject detaching of view class" @ "class %Widget",
		/ "simplified form class as typedef" ^ "class %Frame"
	);
	* "wrong behavior after pressing down exit button" @ %'YReader' $since b258
),

b258
(
	/ %'YFramework'.'YSLib'.'GUI' $=
	(
		- "focus responder runtime substituting capability",
		+ "widget view class %WidgetView";
		/ "unified focus requesting interface"
		/ "focusing state stored" @ "class %WidgetView"
	)
),

b257
(
	/ %'YFramework'.'YSLib'.'GUI' $=
	(
		/ DLD "simplified entering/leaving event implementation",
		- DLD "dependency events" @ "classes %(ATrack, TextList)"
	),
	/ %'YReader' $=
	(
		/ %'hexadecimal browser' $=,
		(
			* "value of vertical scroll bar" @ "class %HexViewArea \
				not reset when file reloaded" $since b254,
			+ $design "individual model class and view class"
		)
	),
	- DLP
		"suppout for language implementation without variadic macro";
	+ $dev "void expression macros for function implementation"
),

b256
(
	* DLD "undefined behavior when event arguments class not empty"
		@ %'YFramework'.'YSLib'.'GUI' $since b255,
	* "overloading error when using default template argument"
		@ "class template ExpandMemberFirstBinder" $since b171
	/ "first parameter for merged to second parameter" @ "event handling"
),

b255
(
	/ %'YReader' $=
	(
		/ %'hexadecimal browser' $=,
		(
			* "displaying of tail bytes less than one line" $since b253,
			+ "horizontal alignment controlling in displaying"
		)
	),
	* "wrong value construction" @ "class %TouchEventArgs"
		@ %'YFramework'.'YSLib'.'GUI' $since b195
),

b254
(
	/ DLP "header %dependencies",
	/ %'YReader' $=
	(
		/ %'hexadecimal browser' $=,
		(
			+ "vertical scroll bar",
			+ "updating data for non-zero offset"
		)
	),
	+ DLD "unsequenced evaluated expressions optimization" @ "widget class \
		constructors" @ "directory %YSLib::UI" @ %'YFramework'.'YSLib'.'GUI'
),

b253
(
	/ %'YReader' $=
	(
		+ "hexadecimal browser",
		+ $dev $design "unsequenced evaluated expressions optimization"
			@ "unit %Shells",
		+ "automatic desktop invalidation when checkbox unticked"
	),
	* "wrong control %OnLostFocus behavior" @ %'YFramework'.'YSLib'.'GUI'
		$since b240,
	^ "new character types" $=
	(
		^ "fundamental types %(char16_t, char32_t) ~ %(std::uint16_t, \
			std::uint32_t) as basic types" @ "library %CHRLib",
		^ "literal syntax prefix u" ~ "macro _ustr"
	),
	* "size not refreshed when opening file excluded using constructor"
		@ "class %File" $since $before '~b1x'($with_timestamp 2009-12-01,
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
	+ DLD "nested-use support" @ "implementation" @ "macro %yunsequenced",
	/ DLP "libraries using" $=
	(
		^ "updated devkitARM release 35" ~ "devkitARM release 34",
		^ "updated libfat 1.0.10" ~ "libfat 1.0.9"
	),
	* "implementation" @ "installation checking" $since b245
),

b251
(
	(
		+ DLD "unsequenced evaluation macro %yunsequenced";
		+ DLD "unsequenced evaluated expressions optimization"
			@ "library %(CHRLib, YCLib, YSLib)"
	),
	- "buffered text blocks",
	* "wrong ending of text checking @ text buffer" $since b246,
	/ @ "library %CHRLib" $=
	(
		/ "undereferencable conversion error treated as conversion faliure",
		/ "conversion functions returns non-zero if non-zero bytes read \
			when a conversion failure occured"
	)
),

b250
(
	+ DLD "iterator checking operations" @ "library %YCLib";
	/ @ "library %CHRLib" $=
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
	/ @ "library %CHRLib" $=
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
	/ DLD "exception macros" $=
	(
		/ DLD "exception specification macro" @ ("header %config.h"
			@ "library %YSLib::Adaptor") >> ("header %ydef.h"
			@ "library %YCLib::YStandardExtend");
		* DLD "macro (ythrow, ynothrow) used without definition"
			@ "library %YStandardExtend" $since b209;
	)
),

b248
(
	/ DLD "deleted copy constructor" @ "class %input_monomorphic_iterator \
		for safety",
	/ %'YFramework'.'YSLib'.'GUI' $=
	(
		* "wrong return value" @ "member function %Refresh" @ "classes \
			%(Widget, Label, Progress, TextArea, Control, AWindow)" $since b226,
		* "wrong overlapping condition" @ "(function %RenderChild, \
			member function %Frame::DrawContents)" $since b226;
		* "wrong invalidation on thumb of tracks after performing small \
			increase/decrease" $since b240
	)
),

b247
(
	/ %'YFramework'.'YSLib'.'GUI' $=
	(
		+ DLD "common widget member operation function %CheckWidget";
		/ DLD "simplified scrolling widget implementation"
			^ "function %CheckWidget"
	),
	+ "statically-typed object proxy class %void_ref";
	+ "input iterator adaptor class %monomorphic_input_iterator";
	^ DLD "simplified isomorphic character converter prototypes"
		^ "class %monomorphic_input_iterator"
),

b246
(
	+ DLD ^ "selectable C++11 generalized constant expressions features",
	/ "file classes" $=
	(
		+ "mode selecting at opening files",
		/ "opening files" ^ "binary mode as default" @ "class %File";
		/ "initializing text files" ^ "text mode" @ "class %TextFile"
	),
	/ (- "automatic Unix style newline conversion at loading files")
		@ "simplified text file buffering",
	+ "file information panel" @ %'YReader',
	* "character mapping for GBK" @ "library %CHRLib" $since b245
),

b245
(
	/ $dev "interfaces" @ "library %CHRMap",
	/ DLD "integer type definitions and string utilities" @ "library %YCLib",
	* "C-style string allocation" @ "library %CHRMap" $since $before
		'~b10x'($with_timestamp 2010-05-30, $rev("chrproc.cpp") = r1525),
	* DLB "order of YCLib/YSLib in library linking command"
		@ "ARM9 makefile" $since b187;
	* "character mapping functionality implementation" @ "library %CHRLib"$=
	(
		* "unspecified subexpression evaluation order"
			@ "implementation unit chrmap.cpp",
		* "wrong behavior of mapping functions for platforms where char \
			is a signed type"
	) $since before '~b4x'($with_timestamp 2009-11-22, $rev("chrproc.cpp")
		= r1319),
	+ "encoding conversion from UTF-8 to UCS-2" @ "library %CHRLib";
	* "path with non-ASCII characters cannot send to reader" @ %'YReader'
		$since b141
),

b244
(
	/ %'YReader' $=
	(
		/ "reader panel functionality",
		/ "more text file extensions supported"
	),
	* "unsafe nullable dependency object" $since b242
		$= (- "nullable features" @ "class template %GDependency"),
	* "path with non-ASCII characters wrongly displayed" $since b141,
	* "wrong value of unnamed namespace constant member FS_Parent_X"
		@ "file yfilesys.cpp" $since b156
),

b243
(
	/ DLD "simplified class inheritance" @ "shell and application classes";
	- DLD "inheritance GMCounter<Message>" @ "class %Message";
	- "classes %(YCountableObject; YObject)";
	/ %'YFramework'.'YSLib'.'GUI' $=
	(
		+ "default GUI event %Paint for all widgets",
		+ "controller class %WidgetController for widgets which %Paint is the \
			only event permitted to call";
		/ "rendering of function %Render" ^ "mutable rvalue reference \
			parameter to store the result" ~ "returning";
		+ "event handler %Render" @ "constructor" @ "%WidgetController";
		/ "rendering logic" @ "member function %Frame::DrawContents"
			^ "event %Paint" ~ "directly call function %Render";
		/ DLD "simplified updating" @ "renderer classes",
		/ DLD "simplified refreshing" @ "class %TextList"
	)
),

b242
(
	/ %'YFramework'.'YSLib'.'GUI' $=
	(
		+ "class %PaintEventArgs",
		/ DLD "simplified painting parameters" ^ "class %PaintEventArgs",
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
	/ DLD ^ "lambda expressions" ~ "most of member function handlers"
		@ %'YReader',
	/ "simplified screen object interface and implementation" $=
	(
		/ "class template %GBinaryGroup" ~ "classes %(BinaryGroup, Point, Vec)",
		/ ("member functions %(GetPoint, GetSize) return const references"
			~ "object type values") @ "class %Rect";
		/ ("platform independent const static member %Invalid"
			@ "(class template %GBinaryGroup, class %Size)")
			~ ("platform dependent %FullScreen" @ "classes %(Point, Vec, Size)"
	)
),

b241
(
	+ "new macros to simplify defaulted or deleted constructors \
		and destructors definition" @ "header %ybase.h",
	/ %'YFramework'.'YSLib'.'GUI' $=
	(
		+ DLD "%(MoveConstructible) support for all widget classes",
		+ DLD "class %IController as controller interface type"
	),
	/ "event mapping interfaces" $=
	(
		+ "pointer type values confined as non-null",
		/ "member function %at returns %ItemType&" ~ "%PointerType&",
		/ DLD "member function templates %(GetEvent, DoEvent)"
			@ "class template %GEventMap" >> "unit %(YWidgetEvent, YControl) \
			as non-member"
	),
	* "contradict semantics of member template is_null" @ "header %memory.h"
		@ "library %YCLib" $since b222
),

b240
(
	/ "macros" @ "header %ybase.h" $=
	(
		/ DLD "simplified macro parameter names",
		+ "new macros for template declarations and forwarding constructor \
			templates",
		+ "macro %DefClone(_t, _n) for define member function for cloning"
	),
	+ "support for types are not %EqualityComparable" @ "class %ValueObject"
		^ "always-be-equal strategy",
	+ DLD "copy constructor" @ "class template %GEventMap",
	+ DLD "const static reference getter function %GetPrototype"
		@ "header %ystatic.hpp",
	* DLD "member function %Insert unavailable for %unique_ptr"
		@ "class template %GEventMap" $since b221,
	+ DLD "%CopyConstructible, %MoveConstructible support"
		@ "class template %GEventMap",
	+ DLD "%CopyConstructible and %MoveConstructible support"
		@ "classes %(BitmapBuffer, BitmapBufferEx)";
	/ %'YFramework'.'YSLib'.'GUI' $=
	(
		* @ "class template %GFocusResponser" $since b239
			$= (+ "default constructor"),
		+ "virtual member function %Clone" @ "renderers, focus responsers \
			and widget classes",
		+ DLD "prototype constructing of widgets" $=
		(
			+ DLD "%(CopyConstructible, MoveConstructible) and clone \
				support" @ "classes %(WidgetRenderer, BufferedWidgetRenderer)",
			+ DLD "%(CopyConstructible, MoveConstructible) and clone \
				support" @ "class templates %(GFocusResponser, \
				GCheckedFocusResponser)",
			+ DLD "%(CopyConstructible, MoveConstructible) and clone \
				support" @ "class %WidgetController";
			+ DLD "%CopyConstructible and %MoveConstructible support"
				@ "classes %(Widget; Control)",
			+ "class %ControlEventMap" @ "class %Control"
		)
	)
),

b239
(
	/ %'YFramework'.'YSLib'.'GUI' $=
	(
		+ "dynamic focus responser switching",
		/ "simplified" @ "class %IWidget and derived classes",
		+ "common focus APIs shared by class %IWidget"
			~ "classes %(Panel, AFrame, AUIBoxControl)" $=
		(
			+ "function %ClearFocusingPtrOf",
			+ "function %FetchFocusingPtr"
		)
	)
),

b238
(
	/ %'YFramework'.'YSLib'.'GUI' $=
	(
		/ "simplified" @ "class %IWidget",
		/ DLD "controller pointer" @ "class %Control" >> "class %Widget",
		/ "simplified" @ "class %Control",
		/ "simplified implementation" @ "focus responsers"
	)
),

b237
(
	/ %'YFramework'.'YSLib'.'GUI' $=
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
		/ DLD "member function %GetTopWidgetPtr" $=
		(
			- "containing test",
			+ "predicate parameter"
		),
		+ "automatic canceling pressing state when refreshing" @ "class %Thumb"
	)
),

b236
(
	^ "updated libnds 1.5.4 with default arm7 0.5.23"
		~ "libnds 1.5.1 with default arm7 0.5.21",
	/ %'YFramework'.'YSLib'.'GUI' $=
	(
		+ "controllers",
		/ "simplified %IControl interface",
		+ DLD "rvalue forwarding and return value support"
			@ "function template %CallEvent",
		+ "limited focusing interfaces" @ "class %IWidget",
		/ "namespace %(Widgets, Controls, Forms) merged to parent \
			namespace %YSLib::Components",
		/ "simplified implementation and improved efficiency \
			in focus requesting" $=
		(
			^ DLD "class %IControl" ~ "class %AFocusRequester"
				@ "member functions" @ "controls",
			- DLD "class %AFocusRequester"
		)
	)
),

b235
(
	/ %'YReader' $=
	(
		/ "simplified background images loading",
		+ "reader panel" $=
		(
			+ "close button",
			+ "direct reading command controls"
		)
	),
	/ %'YFramework'.'YSLib'.'GUI' $=
	(
		- DLD "redundant refreshing" $since b226
			@ "efficiency improved" @ "member function %ATrack::Refresh",
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
	/ %'YFramework'.'YSLib'.'GUI' $=
	(
		/ "overwritable item enablity policy support" @ "class %TextList",
		/ "item enablity support" @ "class %Menu",
		* DLD "recursively self call" @ "Control::Refresh"
			$since b230
	),
	/ %'YReader' $=
	(
		+ "checkbox to switch FPS visibility",
		- "checkbox to switch enablity of button",
		* "lost mapped global resource release call" $since b233,
		+ "empty panel test"
	)
),

b233
(
	/ %'YFramework'.'YSLib'.'GUI' $=
	(
		/ "partial invalidation for hosted menus",
		/ "partial invalidation for member function %Desktop::MoveToTop",
		/ "partial invalidation for class %TextArea"
	),
	/ %'YReader' $=
	(
		/ "key input response" ^ "direct routing strategy"
			~ "all routing strategy",
		/ "menu of reader",
		+ "mapped global resource management"
	),
	/ $dev $lib "enhancement" @ "class %ValueObject"
	(
		+ "mutable access",
		+ "empty predicate",
		+ "constructing by pointers"
	)
),

b232
(
	/ %'YReader' $=
	(
		/ "background images" @ "class %ShlExplorer",
		/ $design "simplified reader refreshing implementation",
		- $design "member function ShlProc"
			@ "classes %(ShlExplorer, ShlReader)",
	),
	/ %'YFramework'.'YSLib'.'GUI' $=
	(
		* "invalid parent menu displayed on confirming submenus" $since b231,
		/ "menus functionality" $=
		(
			+ "hiding unrelated non-parent menus"
		),
		* "GUI class member function %ClearFocusingPtr implementation"
			$since b194 $=
		(
			@ "class %AFrame" $since b194,
			@ "class %Panel" $since b201,
			@ "class %AUIBoxControl" $since b194
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
	/ %'YFramework'.'YSLib'.'GUI' $=
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
	/ DLD "style of free setters/getters naming"
),

b230
(
	+ DLD "move constructors and move assignment operators"
		@ "class templates %(pseudo_iterator, pair_iterator)"
		@ "header %YCLib::YStandardExtend::Iterator",
	/ DLD "simplified GUI" $=
	(
		- "paint event for controls",
		- "DrawControl methods"
	),
	/ %'YReader' $=
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
	/ %'YFramework'.'YSLib'.'GUI' $=
	(
		/ "simplified controls rendering implementation" $=
		(
			- "widget boundary drawing APIs",
			/ "control drawing" @ "classes %(TextList, CheckBox)"
		),
		/ "simplified APIs" $=
		(
			- "locating functions concerned with interface %IWindow",
			- "interface %IWindow inheritance" @ "class %AWindow",
			- "interface %IWindow"
		),
		+ "dynamic renderer switching"
	)
	/ %'YReader' $=
	(
		+ "buffered renderer for listbox",
		^ "namespace %::YReader" ~ "namespace ::YSLib"
	),
	- "using namespace %platform" @ "namespace DS" @ "header %yadaptor.h"
),

b228
(
	/ %'YFramework'.'YSLib'.'GUI' $=
	(
		+ "runtime buffering control" $=
		(
			+ "renderer classes %(BufferedWidgetRenderer, WidgetRenderer)",
			/ "window buffering control moved to renderer" ~ "class %Frame"
		),
		/ "buffering-concerned methods" @ "class %IWindow"
			>> "renderer classes and class %IWidget"
	)
),

b227
(
	(
		* "wrong result when one argument actually contained by another"
			@ "intersection calculation for class %Rect" $since b226;
		* "invalidation area error" $since b226
	),
	/ "simplified background rendering" @ "class %ScrollableContainer"
		@ "widget rendering efficiency improvement",
	* DLD "guard macro" @ "header %UIContainerEx" $since b203,
	/ "simplified UI class inheritance" $=
	(
		/ $dev $lib "implementation" ^ "class %IWidget" ~ "class %IUIBox",
		- "class %IUIBox",
		- "class %IUIContainer",
		/ $dev $lib "implementation" ^ "class %IControl" ~ "class %IPanel",
		- "class %IPanel"
	),
	* $lib "strict ISO C++03 code compatibility" $since b190
		$= (^ "fixed macros" ~ "variadic macros" @ "header %(YPanel, YWindow)"),
	/ "simplified widget fetcher and locating interfaces"
),

b226
(
	/ "widget rendering efficiency improvement" $=
	(
		+ "return value of actually drew area for member function %Refresh"
			@ "widgets",
		- "unnecessary drawing of overlaid windows" @ "windows refreshing",
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
	+ $dev $lib "assignment operators" @ "class %Rect",
	/ "widget rendering improvement" $=
	(
		+ $dev $lib "parameters for member function %Refresh" @ "widgets",
		/ "background refreshing" ^ "invalidation" @ "class %AWindow",
		/ "without validating old invalidated areas" @ "windows invalidating",
		+ "partial invalidation" @ "control default %TouchMove event handler \
			%OnTouchMove_Dragging"
	),
	+ "intersection and union calculation for class %Rect",
	/ %'YReader' $=
	(
		+ "opaque background" @ "pseudo-frame-per-second counter",
		/ "controls layout" @ "shell class %ShlExplorer",
		- "windows update state"
	)
),

b224
(
	* "wrong ascending when switching font size" $since b224
		$= (^ "freetype 2.4.4 cache system" ~ "freetype 2.4.5 cache system"),
		// It seems a new bug in freetype 2.4.5 cache system.
	+ "containing test" @ "class %Rect",
	+ DLP "WinGDB settings" @ "main project file",
	+ "partial invalidation support of windows" $=
	(
		/ "invalidation interface",
		+ "windows partial invalidation committing for class %AWindow"
	)
),

b223
(
	/ "DS painting" ^ "message %SM_PAINT" @ %'YFramework'.'YSLib'.'GUI'
			~ "directly calling of %ShlDS::UpdateToScreen",
	* "uncleared application message queues on program exit" $since b174,
		// This might cause memory leaks.
	/ %'YReader' $=
	(
		+ "pseudo-frame-per-second counter",
		/ "button enabling" ^ "file extension matching in the file box"
	),
	* "declaration of function %GetStemFrom" @ "header %yfilesys.h" $since b161,
	/ DLP "updated library freetype" @ "library using" $=
	(
		^ "updated freetype 2.4.5" ~ "freetype 2.4.4",
		+ "exact bounding box calculation",
		+ "path stroker",
		+ "support for synthetic emboldening and slanting of fonts",
		- "obsolete header %/freetype/internal/pcftypes.h" $since b185
	),
	^ "updated libnds 1.5.1 with default arm7 0.5.21"
		~ "libnds 1.5.0 with default arm 7 0.5.20",
	/ DLP "library using"
		$=(^ "updated devkitARM release 34" ~ "devkitARM release 33")
),

b222
(
	* DLP "minor faults detected by CppCheck" $=
	(
		/ "simplified implementation" @ "%YGUIShell::ResponseTouch" $since b195,
		* "implementation" @ "%ystdex::fexists" $since b151
	),
	+ "default GUI event %Paint",
	/ "controls drawing" ^ "event Paint" ~ "member function %Draw"
		@ "classes %(Frame, YGUIShell)",
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
	/ %'YReader' $=
	(
		+ "menu" @ "class %ShlReader",
		^ "widget drawing" ^ "direct desktop background drawing"
	),
	+ "controls: buffered text area"
),

b221
(
	/ %'YReader' $=
	(
	`	/ "simplified direct-painting test",
		/ "simplified key-to-touch event mapping implementation"
			@ "class %Control"
	),
	/ DLP "library using" $=
	(
		^ "updated devkitARM release 33" ~ "devkitARM release 32",
		^ "updated libnds 1.5.0 with default arm7 0.5.20"
			~ "libnds 1.4.8 with default arm 7 0.5.17"
	),
	/ DLP "header search path of VS2010 projects",
	/ "event map interfaces" $=
	(
		+ "sender type as template parameter type",
		/ "simplified sender argument forwarding"
		/ "more efficient implementation" ^ "%unique_ptr" ~ "%shared_ptr",
			// Old version of libstdc++ might fail in compiling \
				due to members with parameter of rvalue-reference type \
				are not supported.
	),
	* "lost move constructor" @ "class template %GMCounter" $since b210,
	/ "class %HDirectory" ^ "POSIX dirent API" ~ "libnds-specific API",
	+ DLB "diagnostic pragma for GCC 4.6.0 and newer"
		@ "header %type_op.hpp",
	* "class %HDirectory state not restored during operations" $since b175,
	/ %'YFramework'.'YSLib'.'GUI' $=
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
			/ DLD "default argument" ^ "unsigned integral type"
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
	/ DLD "unit %YGUI decomposed for %Blit and 2D API",
	/ "optimized widget-to-widget locating",
	/ DLD "font cache" >> "platform-dependent application class"
		~ "class %YApplication",
	+ "several global helper functions as platform-independent interface",
	/ %'YReader'
		$= (+ "multiple background switch test"),
	* "wrong default argument of desktop background color" $since b160,
	+ "widgets: progress bar" @ "class %ProgressBar"
),

b218
(
	/ %'YReader'
		$= (+ "desktop background switch test"),
	+ $dev $lib "ownership tag structure",
	- $dev $lib "global resource helper functions",
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
	+ DLP "Code::Blocks project files" $=
	(
		+ "workspace file",
		+ "project files"
	),
	/ DLD ^ "lambda expression" ~ "several private member function \
		as event handlers",
	+ "%KeyDown and %KeyHeld handlers for scroll bars \
		to perform mapped touch behaviors",
	+ "key-to-touch event mapping methods" @ "class %Control",
	(
		* "wrong coordinate passed to touch focus captured control" $since b195;
		* "wrong behavior" @ "track touch held event handler" $since b213
	),
	+ DLP "cscope files"
),

b216
(
	/ DLD "header %file dependencies",
	/ "focused boundary for controls",
	^ DLD "using unnamed namespace" ~ "keyword static" @ "unit %YApplication",
	/ "more efficient message queue implementation" ^ "%multiset"
		~ "%priority_queue",
	/ "simplified shell example",
	+ "default windows Z order",
	+ "message fetching automatically calling %Idle \
		when size less or equal than a user-specified limit"
),

b215
(
	+ "disabled state style of control %Thumb",
	* "disabled control touch event handling" $b199,
	/ + "checkbox and disabling test" @ %'YReader',
	+ "visible arrow indicating multi-level menus",
	/ "creating list" @ ("constructor of class %MTextList"
		~ "member function %GetList"),
	+ "event handler supporting for types not met EqualityComparable \
		requirement interface including closure types",
	* DLD "access of interitance of class std::function"
		@ "class template %GHEvent" $since b207,
	+ "disabled behavior test" @ "%ShlExplorer" @ %'YReader',
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
	* DLD "exception specification" @ "unit %YApplication",
	/ $doc $=
	(
		* "template parameter description" $since b189
			$= (^ "\tparam" ~ "\param"),
		* "\defgroup description spell error" $since b209,
		* DLD "operator new & delete comments" $since b203,
		/ "simplified Doxygen file excluded paths"
	),
	/ DLD ^ "public %noncopyable inheritance"
		~ "all private %noncopyable inheritance",
	* "point containing test for zero width or height rectangle \
		turned out assertion failure" $since b204,
	* "constness of text width measuring" $since b197,
	/ (- "unused parameter and argument") @
		"simplified focus operations interface"
),

b213
(
	* DLD "UI assertion strings",
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
	/ DLD "assertion strings",
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
	/ DLD "messaging APIs" >> "unit %YApplication" ~ "unit %YShell",
	/ "test menu fixed on the desktop" @ %'YReader'
),

b210
(
	+ "template %is_valid for checking pointers which cast bool explicitly",
	/ "part of template %raw moved to namespace %ystdex",
	- DLD "some unused code",
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
	/ "merged library CHRLib" >> "library %YCLib",
	/ DLD "protected function inheritance" @ "class template %GHEvent",
	/ "using directive of namespace %ystdex" @ "library %YSLib",
	+ "lost %Rect operations",
	* $lib "strict ISO C++03 code compatibility" $since b190
		$= (^ "fixed macros" ~ "variadic macros" @ "header %YFont"),
	/ "renamed directory %Shell to %UI" @ "library %YSLib",
	/ "several memory utilities for std::shared_ptr and std::unique_ptr"
		>> "library %YCLib::YStandardExtend"
),

b208
(
	^ "rvalue references as parameter types of event handlers",
	/ "set default font size smaller",
	+ "vertical alignment" @ "labeled controls",
	* "fatel error" @ "direct UI drawing test"
),

b207
(
	/ DLD "event handler implementation ^ %std::function" ~ "%Loki::Function",
	- $lib $build "library %AGG",
	- $lib $build "library %Loki",
	+ DLD "union %no_copy_t and union %any_pod_t for suppressing \
		static type checking",
	+ DLD "polymorphic function object template and base class",
	- "single-cast event class template",
	^ "rvalue reference" @ "event class",
	* "ystdex algorithms",
	* "minor event handler parameter type mismatch" @ %'YReader',
	* "key events response(added as %KeyDown, but removed as %KeyPress)"
		@ "project YSTest_ARM9" $since b201,
	* "tunnel and direct key and touch response repeated" @ "class %YGUIShell"
		$since b198
),

b206
(
	+ "menu test button",
	/ "file API",
	* DLD "strict ISO C++03 code compatibility" $since $before 132
		$= (+ "function %memcmp declaration" @ "namespace %ystdex"),
	* DLD "strict ISO C++0x code compatibility" $=
	(
		* "implicit narrowing conversion(N3242 8.5.4/6)"
			@ "ISO C++0x(N3242 5.17/9)" ^ "explicit static_cast",
		/ "character types" @ "header %platform.h"
	),
	^ "limited C++0x features" $=
	(
		^ $dev $build "C++0x style nested template angle brackets",
		^ $dev $build "keyword %auto",
		^ "C++2011 type_traits" @ "namespace std" ~ "std::tr1",
		^ "std::shared_ptr" ~ "std::tr1::shared_ptr"
	),
	- "Loki type operations",
	/ ^ "namespace %ystdex" @ "namespace %YSLib"
),

b205
(
	/ "simplified widgets and controls inheritance",
	/ "simplified image resource type",
	/ DLD "protected inheritance of module classes except \
		%MScreenPositionEventArgs",
	/ "listbox highlight text color"
),

b204
(
	/ "weak handle improvement",
	/ DLD "exception specification macro",
	- "window handles" $=
	(
		- "type %HWND",
		^ "%IWindow*" ~ "%HWND"
	),
	* "track background painting ignored" $since b191,
	+ "class %nullptr_t" @ "namespace ystdex",
	^ "nullptr at YSLib",
	/ DLD "widgets virtual member functions"
),

b203
(
	/ DLP "units rearrangement",
	/ DLD "global architecture" $=
	(
		+ "global instance function",
		- "all global objects",
		- "platform dependent global resource" @ "class %YApplication",
		- "global object" @ "unit %YNew"
	),
	+ "class %MTextList",
	/ "class %YSimpleText List using class %MTextList inheritance",
	/ "class %YSimpleText renamed to %YMenu",
	^ "std::tr1::shared_ptr" ~ ("smart pointers" @ "library %Loki")
),

b202
(
	/ DLP "unit %renaming",
	/ "improved windows painting efficiency",
	* "buffered coordinate delayed in painting dragged control" $since b169
),

b201
(
	/ "focused button style",
	+ "key holding response" @ "class %ShlReader",
	+ "GDI API %BlitTo",
	/ "widgets drawing",
	/ DLD "GDI API %CopyTo",
	+ "controls: panel",
	/ DLD "YCLib" $=
	(
		+ "partial version checking for compiler and library implementation",
		+ "minor macros" @ "library %YCLib",
	),
	+ "type conversion helper template",
	+ DLD "implicit member overloading by interface parameter with type \
		%IControl and %IWidget" @ "container classes"
),

b200
(
	* "direct UI drawing test",
	/ "test UI view",
	* "%KeyHeld response" $since b199,
	+ "direct desktop drawing for %ShlSetting"
),

b199
(
	+ "event routing for %(KeyUp, KeyDown, KeyHeld)",
	* "event behavior with optimizing" $since b195,
	+ "keypad shortcut for file selector",
	+ DLD "returning number of called handles in event calling"
),

b198
(
	* "font caching without default font file load successfully",
	+ "showing and hiding windows",
	* "%ListBox scroll bar length",
	* "handle constructors",
	- "class %ShlGUI" @ "unit %Shell_DS" @ $design "shells for DS",
	/ %'YFramework'.'YSLib'.'GUI' $=
	(
		/ DLD "using pointers" ~ "references in parameters \
			of container methods",
		/ "simplified GUI shell" $=
		(
			/ "input points matching",
			- "windows list"
		)
	),
	/ DLD "simplified destructors",
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
	* DLB "makefiles",
	+ "dependency events",
	+ "simple events routing"
),

b194
(
	/ "global architecture",
	/ "GUI shell class architecture" @ "unit %YGUI",
	/ "default control event handlers implementation",
	/ "implementation" @ "unit %YInitialization",
	/ "controls always visual",
	- "class %IVisualControl",
	- "class %VisualControl",
	/ "container focus" @ "classes %(AUIBoxControl, AWindow)"
),

b193
(
	/ "scroll controls implementation",
	/ "core utilities"
	/ "empty event arguments" $=
	(
		- "empty constant of event arguments",
		+ "inline function template %GetStaticRef",
		/ "all event empty arguments" ^ "%GetStaticRef"
	),
	/ "scroll controls and listbox implementation"
),

b192
(
	+ "empty constant of event arguments",
	/ "event interfaces",
	/ DLP "header %including",
	+ "unit %YUIContainerEx" @ "directory Shell",
	+ "class %AUIBoxControl" @ "unit %YUIContainerEx",
	+ "controls: class %YScrollableContainer" @ "unit %YGUIComponent",
	/ DLD "function %FetchWindowPtr as non-inline" @ "unit %YUIContainer",
	/ "class %IWindow as non-virtual inheritance" @ "class %AWindow"
		@ "class %YWindow",
	/ DLB "arm9 makefile",
	/ "scroll controls implementation",
	^ "updated libnds 1.4.9" ~ "libnds 1.5.0",
	^ "updated default arm7 0.5.20" ~ "default arm7 0.5.18",
	^ "updated libfat 1.0.9" ~ "libfat 1.0.7"
),

b191
(
	/ "listbox APIs implementation",
	/ "class template %GSequenceViewer" $=
	(
		* DLD "declaration of member function %Contains" @ $since b171,
		/ "interfaces"
	),
	* "implementation of function %GetTouchedVisualControlPtr" @ "unit %YGUI"
		$since b167,
	/ "simplified focus implementation"
),

b190
(
	* $lib "strict ISO C++03 code compatibility" $since $before b132
		$= (/ "fixed macros" ~ "variadic macros"),
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
	- DLB "DLDI patch commands" @ "makefile",
	* "invalid listbox input when the list not beginning from the top"
		$since b171
),

b187
(
	/ DLP "header %files and source files put in separated \
		directories" @ "solution configuration",
	^ $lib $build "-O3 & arm specified options compiled library %libloki.a"
		@ "project YSTest",
	+ "minor templates" @ "YCLib",
	/ "GDI blit implementations"
),

b186
(
	/ "solution configuration",
	* DLB "makefiles",
	/ DLP "file including",
	* "some wrong Doxygen comments"
),

b185
(
	/ "library using"
		$= (^ "updated freetype 2.4.4" ~ "freetype 2.3.12"),
	/ "basic memory operations" $=
	(
		- "DMA implantation";
		^ "normal %memcpy and %memset"
	)
	/ "desktop window refreshing logic",
	/ DLD "MDualScreenReader constructor" ^ "%ynew" ~ "%new"
),

b184
(
	* $design "some spell errors",
	/ "DMA operations implementation"
),

b170_b183
(
	/ "GUI" $=
	(
		+ "controls: tracks",
		+ "controls: scroll bars";
		+ "controls: scrollable container",
		+ "controls: listbox"
	)
),

b159_b169
(
	+ "controls: buttons" @ "classes %(YThumb, YButton)",
	+ "controls: listbox class",
	+ "event mapping"
),

b134_b158
(
	+ DLD "core utility templates",
	/ "smart pointers",
	+ "GUI focus",
	+ "shells" @ %'YReader' $=
	+ "controls",
	+ "virtual inheritance" @ "control classes",
	+ "exceptions",
	+ "debug macros & functions",
	+ "color type",
	+ "class template %general_cast",
	+ "class %Timer"
),

b133
(
	/ DLD "simplified primary types definition",
	/ "event interfaces",
	+ "several exception classes",
	/ "exception handling" @ "some functions"
),

b132
(
	/ "basic log interfaces",
	+ "backup message queue object" @ "the application class",
	/ "automatically clearing screen windows when deactivating class %ShlGUI",
	+ "background image indexing" @ %'YReader'
),

b1_b131
(
	+ "initial test with PALib & libnds",
	+ "character set management library %CHRLib",
	+ "fonts management" ^ "freetype 2",
	+ "shell classes",
	+ "platform abstraction library %YCLib",
	+ "the application class",
	+ "basic objects & counting",
	+ "global objects",
	+ "string class %String",
	+ "file classes",
	+ "dual screen text file reader framework",
	+ "output devices & desktops",
	+ "messaging",
	+ "program initialization",
	+ "simple GUI: widgets & windows",
	+ "simple GDI",
	+ "base abbreviation macros",
	+ "events",
	+ "smart pointers" ^ "library %Loki",
	+ "library %'Anti-Grain Geometry' test",
	+ "simple resource classes"
);


//---- temp code;
	// TODO: remove '*printf';

#endif

