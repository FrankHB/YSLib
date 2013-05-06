/*
	Copyright by FrankHB 2009 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ex.cpp
\ingroup Documentation
\brief 设计规则指定和附加说明 - 存档与临时文件。
\version r5548 *build 402 rev *
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-02 05:14:30 +0800
\par 修改时间:
	2013-05-04 19:32 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Documentation::Designation
*/

/*
	NOTE: This is NOT a source file. The base name of this file is named
	intentionally to activate syntax highlight and other functions in an IDE
	(specially, Visual Studio). The content is mainly about log of the
	process of development, conformed with a set of virtual set of syntax rules
	which constitutes an informal pseudo-code-based language.
	This file shall be safe of deletion when building the projects.

	The version number is updated when the change is out of temporary sections
	($DONE, $DOING, $NEXT_TODO).
*/

#if 0

$import META;
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
	$TODO,
	$LOW_PRIOR_TODO,
	$FURTHER_WORK,
	$KNOWN_ISSUE,
	$TO_BE_REVIEWED_ENVIRONMENT_ISSUE,
	$RESOLVED_ENVIRONMENT_ISSUE,
	$HISTORY
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
\app ::= applications
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
\dest ::= destinations
\dir ::= directories
\doc ::= documents
\dst ::= destinations
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
\grp ::= groups
\gs ::= global scpoe
\h ::= headers
\i ::= inline
\impl ::= implementations
\in ::= interfaces
\init ::= initializations
\inc ::= included
\inh ::= inherited
\inst ::= instances/instantiated
\inv ::= invoke
\k ::= keywords
\li ::= lists
\lib ::= library
\lit ::= literals
\ln ::= lines
\loc ::= local
\lst ::= lists
\m ::= members
\mac ::= macros
\mem ::= memory
\mf ::= member functions
\mft ::= member function templates
\mg ::= merged
\mo ::= member objects
\msg ::= messages
\mt ::= member templates
\n ::= names
\ns ::= namespaces
\num ::= numbers
\o ::= objects
\op ::= operators
\opt ::= optiaonal/options
\or ::= overridden/overriders
\param ::= parameters
\pos ::= position
\post ::= postfix
\pre ::= prepared
\pref ::= prefix
\proj ::= projects
\proto ::= prototypes
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
\spec ::= speclizations/specifications
\src ::= sources
\st ::= structs
\str ::= strings
\stt ::= struct templates
\t ::= templates
\tb ::= try blocks
\term ::= terms/terminology
\tg ::= targets
\tr ::= trivial
\tp ::= types
\u ::= units
\un ::= unnamed
\v ::= volatile
\val ::= values
\ver ::= versions
\vt ::= virtual


$parser.state.style $= $natral_NPL;
$macro_platform_mapping:
\mac YCL_DS -> DS,
\mac YCL_MINGW32 -> MinGW32;

$using:
\u YObject
(
	\cl ValueObject, \clt (GDependency, GRange)
),
\u YGDIBase
(
	\clt GBinaryGroup; typedef Point, Vec, \cl Size; \cl Rect
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
	\cl (Log, Application)
),
\u YConsole
(
	\cl Console
),
\u YRender
(
	\cl (Renderer; BufferedRenderer)
),
\h YWidgetView
(
	\cl (Visual; View)
),
\h YWidgetEvent
(
	\st (UIEventArgs; RoutedEventArgs; InputEventArgs);
	\st (KeyEventArgs, TouchEventArgs),
	\clt GValueEventArgs,
	\st (PaintContext; PaintEventArgs),
	typedef \en VisualEvent,
	\stt EventTypeMapping,
	\st BadEvent,
	\cl (AController; WidgetController)
),
\u YWidget
(
	\in IWidget; \cl Widget
),
\u YUIContainer
(
	\cl MUIContainer
),
\u YControl
(
	\cl (Controller; Control)
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
	\cl (InputTimer; GUIState)
),
\u YBrush
(
	\cl (SolidBrush, ImageBrush, (BorderStyle; BorderBrush))
),
\u Label
(
	\cl (MLabel; Label, MTextList)
),
\u TextArea
(
	\cl (TextArea; BufferedTextArea)
),
\u Progress
(
	\cl ProgressBar
),
\u Button
(
	\cl (Thumb; Button)
),
\u UIContainerEx
(
	\cl (DialogBox, DialogPanel)
),
\u Selector
(
	\cl (CheckBox; CheckButton)
),
\u TextList
(
	\cl TextList
),
\u Menu
(
	\cl Menu, MenuHost
),
\u Scroll
(
	\cl (ScrollEventArgs; ATrack; HorizontalTrack, VerticalTrack; AScrollBar;
		HorizontalScrollBar, VerticalScrollBar; ScrollableContainer)
),
\u ComboList
(
	\cl (ListBox, FileBox, DropDownList)
),
\u Form
(
	\cl Form
),
\u TextBase
(
	\cl (PenStyle; TextState)
),
\u TextRenderer
(
	\cl EmptyTextRenderer, \clt GTextRendererBase, \cl TextRenderer;
	\cl TextRegion
),
\u TextManager
(
	\cl TextFileBuffer
);


$DONE:
r1-r?;


$DOING:
r8

$relative_process:
2013-05-06 +0800:
-43.0d;
// Mercurial local rev1-rev273: r10716;

/ ...


$NEXT_TODO:
b[$current_rev]-b428:
/ text reader @ YReader $=
(
	/ \simp \impl @ \u (DSReader, ShlReader)
),
/ @ explorer $=
(
	+ extension & MIME options,
	+ filesystem & partition view @ explorer
),
/ @ \proj YBase $=
(
	+ ABI dependent APIs(e.g. name demangling),
	+ null_deleter,
	/ $low_prior consider: ifile_iterator & ifilebuf_iterator,
	/ consider: ^ compile-time integer sequence:
		http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3493.html
);


$TODO:
b[377]:
/ external dependencies $=
(
	/ recompile freetype with MinGW g++ 4.8.0,
	/ consider: recompile DS freetype without '-g' CFLAGS option
);
/ services $=
(
	+ \impl @ images loading
),
/ consider: ^ %std::this_thread::sleep_for ~ YSLib::Delay,
+ \s \as | \mac for (__has_feature(cxx_exceptions) && __has_feature(cxx_rtti))
	for Clang++ @ \h YDefinition,
/ @ "GUI" $=
(
	/ split %GSeqeunceViewer to 2 class templates,
	/ mark (YDesktop; YWindow) as unnecessary files,
	+ icons,
	/ $low_prior more long list tests @ %DropDownList,
	/ refactored shared GUI mapping for menus and other widgets,
	+ safe widget cloning,
	* previous frame form buffered renderer of desktop did not be handled
		properly for clipping area enlarged when updating $since b?,
		// Namely, the actual painted area is not the same as accumulated \
			invalidated bounding region, which essentially cause over painted.
),
/ sessions $=
(
	+ $doc session id,
	+ session shells,
	+ \cl FrameworkSession
),
/ $low_prior YReader $=
(
	+ settings manager,
	+ improved smooth scrolling with lower limit of scrolling cycle supported
),
+ general shell switching clueanup $=
(
	+ filtering background task or clearing application message queue,
	+ helper functions
),
/ project structure $=
(
	/ Microsoft Windows(MinGW32) port $=
	(
		+ free hosted window styles,
		+ host desktop abstraction
	),
	/ improved tests and examples
);
+ comparison between different cv-qualified any_iterator types @ \h AnyIterator;
/ $design $low_prior robustness and cleanness $=
(
	/ \ac @ \inh touchPosition @ \cl CursorInfo @ \ns platform @ \u YCommon,
	+ consider: 'yconstexpr' @ \s \m Graphics::Invalid,
	/ confirm correctness @ stat() @ Win32,
		// See comments @ src/fccache.c @ \proj fontconfig.
	/ consideration of mutable member @ class %Message
),
/ $low_prior improving performance $=
(
	/ \impl @ classes %(Message, MessageQueue),
	/ more specific \impl @ NPL context,
	/ higher FPS
),
/ text manager @ services $=
(
	* text file buffer boundray for encoding for text size not fit
		for char length,
	/ $low_prior more effiecent batch \impl @ \f CopySliceFrom,
	/ consider: const @ \mf @ \cl TextManager
),
+ BSD/GPL/... copyright notice reproducing/displaying @ binaries;

b[427]:
/ YBase $=
(
	/ $low_prior \impl @ \ctor \t fixed_point#2 @ \h Rational ^ 'std::llround'
		~ '::llround',
	/ $low_prior consider merge: boost::transform,
	+ noinstance base class,
	/ \mft<_type> any& \op=(const _type&) -> \mft<_type> any& \op=(_type),
	/ resolved 'scaler' \term and %is_scalar(e.g. for fixed point numbers),
	+ adaptive seriazation (to text/binary),
	+ round to 2^n integer arithmetics
),
+ $low_prior freestanding memory management and new_handler to avoid
	high-level memory allocation failure,
/ Core $=
(
	+ \t \spec swap<YSLib::Message>,
	/ \impl YFileSystem ^ tr2::filesystem
),
/ host environment $=
(
	+ thread safety check for WndProc,
	/ \impl @ \f MinGW32::TestFramework @ platform MinGW32,
	/ split hosted message loop as a new thread distinct to host initialization,
	+ consider: \conv \f between Drawing::Rect, ::RECT @ Helper,
	+ window hypervisor
),
/ @ \lib YCLib $=
(
	+ error code with necessary %thread_local,
	/ stripping away direct using @ Win32 types completely @ \h,
	/ consider: ::OutputDebugStringA,
	/ fully \impl @ memory mappaing,
	+ block file loading,
	+ shared memory
),
/ @ \lib CHRLib $=
(
	/ more accurate invalid conversion state handling,
	/ placeholders when character conversion failed @ string conversion,
	+ UTF-8 to GBK conversion
),
/ completeness of core abstraction $=
(
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
		/ refactoring text rendering APIs $=
		(
			/ refactoring current APIs,
			+ $low_prior user-defined rendering APIs,
			/ unifying model for glyphrun and widgets rendering
		),
		+ basic animation support,
		+ more GDI algorithms
	),
	/ fully \impl @ encoding checking
),
/ resumable exception handling $=
(
	+ general fundamental reusable exception \tp and filtering;
		// Or following Common Lisp conditional handling model, etc?
	+ recovery environment @ main \fn
		// Try-catch, then relaunch the message loop.
),
/ debugging $=
(
	+ more debug APIs,
	+ debugging namespaces
),
/ $design $low_prior robustness and cleanness $=
(
	+ proper move support @ \cl Menu,
	^ delegating \ctor as possible,
	^ std::call_once to confirm thread-safe initialization,
	/ keeping pedantic ISO C++ compatiblity,
	/ consider using std::common_type for explicit template argument
		for (min, max),
	+ macros for 'deprecated' and other attributes,
	^ C++11 generlized attributes,
	* stdout thread safety
),
/ $low_prior YReader tests and examples $=
(
	+ overlapping test @ \cl Rect,
	+ partial invalidation support @ %(HexViewArea::Refresh)
),
/ @ "GUI" $=
(
	+ formal abstraction of rectangular hit test,
	+ widgets opacity,
	/ bidirectional widget iterators support,
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

b[492]:
^ \mac __PRETTY_FUNCTION__ ~ custom assertion strings @ whole YFramework
	when (^ g++),
+ uniform \mac for function attribute (format, ms_format, gnu_format),
/ memory fragment issues,
+ tag-based type operations,
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

b[621]:
/ improve efficiency @ \ft polymorphic_crosscast @ \h YCast for \conf release,
+ function composition,
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
		+ basic backends adaptors,
		+ clipping algorithms,
		+ basic shapes abstraction,
		+ spline nodes abstraction,
		/ more efficient Font switching,
		/ text alignment,
		/ advanced text layout like Unicode layout control
	)
),
/ @ "GUI" $=
(
	+ viewer models,
	/ fully \impl @ \cl Form,
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
	+ dynamic widget \proto with copy \ctor
);


$LOW_PRIOR_TODO:
^ $low_prior $for_labeled_scope;
b[3513]:
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

//---- temp code;
	// TODO: remove '*printf';

#endif

