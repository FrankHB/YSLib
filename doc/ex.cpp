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
\version r6109 *build 414 rev *
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-02 05:14:30 +0800
\par 修改时间:
	2013-06-17 21:17 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Documentation::Ex
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
;


$DONE:
r1-r16;


$DOING:

$relative_process:
2013-06-17 +0800:
-35.4d;
// Mercurial local rev1-rev286: r11035;

/ ...


$NEXT_TODO:
b[$current_rev]-b421:
/ @ explorer $=
(
	+ extension & MIME options,
	+ filesystem & partition view @ explorer
),
/ @ \proj YBase $=
(
	+ null_deleter
);


$TODO:
b[1076]:
/ $extern external dependencies $=
(
	/ recompile freetype with MinGW g++ 4.8.0,
	* freetype 2.4.12 performance @ DS
);
/ YBase $=
(
	+ ABI dependent APIs(e.g. name demangling),
	/ $low_prior consider: ifile_iterator & ifilebuf_iterator,
	+ consider: compile-time integer sequence;
		// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3493.html
	+ \s \as | \mac for (__has_feature(cxx_exceptions)
		&& __has_feature(cxx_rtti)) for Clang++ @ \h YDefinition,
	+ comparison between different cv-qualified any_iterator types
		@ \h AnyIterator,
	+ ystdex::noinstance base class,
	/ $extern adjusted ystdex::any to std::tr2::any,
	/ resolved 'scaler' \term and %is_scalar(e.g. for fixed point numbers),
	+ adaptive seriazation (to text/binary),
	+ round to 2^n integer arithmetics,
	/ $low_prior consider merge: boost.iterator,
	+ tag-based type operations,
	/ ystdex::fixed_point $=
	(
		* \impl @ \op/= for signed types,
		+ 64-bit integer underlying type support
	),
	+ $low_prior C-style string output routines (u16printf, u32printf),
	/ consider: improve efficiency @ \ft polymorphic_crosscast @ \h YCast
		@ \conf release,
),
/ YFramework.YSLib.Adaptor $=
(
	/ consider: + final @ \cl (Font, CharBitmap) @ \u Font,
	+ external image \lib binding
),
/ YFramework.YSLib.Service $=
(
	+ \impl @ images I/O and conversions,
	/ \impl YFileSystem ^ tr2::filesystem,
	// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3505.html .
	/ fully \impl @ encoding checking,
	+ general \impl @ images processing interface,
	^ <chrono> to completely abstract system clocks,
	+ general data configuragion,
	/ general file type abstraction,
	/ fully \impl @ \cl Path
),
/ YFramework.YSLib.UI $=
(
	/ split %GSeqeunceViewer to 2 class templates,
	+ icons,
	/ $low_prior more long list tests @ %DropDownList,
	* previous frame form buffered renderer of desktop did not be handled
		properly for clipping area enlarged when updating $since b?,
		// Namely, the actual painted area is not the same as accumulated \
			invalidated bounding region, which essentially cause over \
			painted.
	+ formal abstraction of rectangular hit test,
	+ widgets opacity,
	+ bidirectional widget iterators support,
	+ key accelerators,
	+ widgets for RTC,
	/ refactored shared GUI mapping for menus and other widgets,
	+ safe widget cloning,
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
		@ class %ListBox,
	+ widget models,
	/ sessions $=
	(
		+ $doc session id,
		+ session shells,
		+ \cl FrameworkSession
	),
	+ UI scenes, modes, subsessions,
),
/ YFramework.YSLib.Core $=
(
	/ messaging $=
	(
		/ consider: mutable member @ class %Message,
		+ \t \spec swap<YSLib::Message> and other framework \tp;
		+ general predicates for removing message,
		^ timing triggers @ message loop,
		/ consider: ^ weak_ptr @ shell messages
	)
),
/ YFramework.Helpers $=
(
	+ general shell switching clueanup $=
	(
		+ filtering background task or clearing application message queue,
		+ more switching helper functions,
		+ locking;
		+ exceptional state and handlers
	),
	/ host environment $=
	(
		/ consider: CreateCompatibleBitmap double buffering,
		/ revised thread safety check for WndProc,
		/ \impl @ \f MinGW32::TestFramework @ platform MinGW32,
		/ split hosted message loop as a new thread distinct to host \
			initialization,
		+ consider: \conv \f between Drawing::Rect, ::RECT @ Helper,
		+ window hypervisor
	)
),
/ @ \lib CHRLib $=
(
	/ more accurate invalid conversion state handling,
	/ placeholders when character conversion failed @ string conversion,
	+ UTF-8 to GBK conversion
),
/ @ \lib YCLib $=
(
	/ consider: necessity of %thread_local for error codes, etc,
	/ stripping away direct using @ Win32 types completely @ \h
),
/ $low_prior YReader $=
(
	+ settings manager,
	/ \simp \impl @ \u (DSReader, ShlReader),
	+ improved smooth scrolling with lower limit of scrolling cycle supported
	+ overlapping test @ \cl Rect,
	+ partial invalidation support @ %(HexViewArea::Refresh)
),
/ project structure and documentation $=
(
	/ Microsoft Windows(MinGW32) port $=
	(
		+ free hosted window styles,
		+ host desktop abstraction
	),
	+ \conf profile for DS;
		// http://lectem.fr/2012/10/profiling-on-the-nintendo-ds/ .
	/ improved tests and examples,
	/ consider: split Roadmap.txt form ex.cpp to record todo issues and \
		fulfilments
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
	/ consider: ::OutputDebugStringA,
	+ debugging namespaces,
	^ \mac __PRETTY_FUNCTION__ ~ custom assertion strings @ whole YFramework
		when (^ g++),
	+ uniform \mac for function attribute (format, ms_format, gnu_format)
),
/ $design $low_prior robustness and cleanness $=
(
	/ \ac @ \inh touchPosition @ \cl CursorInfo @ \ns platform @ \u YCommon,
	+ consider: 'yconstexpr' @ \s \m Graphics::Invalid,
	/ confirm correctness @ stat() @ Win32,
		// See comments @ src/fccache.c @ \proj fontconfig.
	/ $low_prior \impl @ \ctor \t fixed_point#2 @ \h Rational ^ 'std::llround'
		~ '::llround',
	+ $low_prior freestanding memory management and new_handler to avoid
		high-level memory allocation failure,
	+ proper move support @ \cl Menu,
	^ delegating \ctor as possible,
	^ std::call_once to confirm thread-safe initialization,
	/ keeping pedantic ISO C++ compatiblity(for C++14, Clang++, G++),
	/ consider using std::common_type for explicit template argument
		for (min, max),
	+ macros for 'deprecated' and other attributes,
	^ C++11 generlized attributes,
	* stdout thread safety
),
/ $low_prior improving performance $=
(
	/ \impl @ classes %(Message, MessageQueue),
	/ more specific \impl @ NPL context,
	/ memory fragment issues
),
+ copyright notice reproducing/displaying for licenses @ binaries;

b[866]:
/ @ \proj YBase $=
(
	+ function composition
),
/ @ \lib YCLib $=
(
	/ fully \impl @ memory mappaing,
	+ block file caching and loading,
	+ shared memory,
	/ consider: + correct DMA (copy & fill) @ DS
),
/ YFramework.YSLib.Core $=
(
	/ implicit encoding conversion @ \cl String,
	+ shell framework for plugins and devices,
	+ automatic shellizing,
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
/ YFramework.YSLib.Service $=
(
	/ text manager @ services $=
	(
		* text file buffer boundray for encoding for text size not fit
			for char length,
		/ $low_prior more effiecent batch \impl @ \f CopySliceFrom,
		/ consider: const @ \mf @ \cl TextManager
	),
	+ user-defined stream filters,
	/ improving \impl font switching,
	/ fully \impl logging $=
	(
		+ more clarified log Levels,
		+ log streams
	),
	+ general resouce management helpers,
	/ @ "GDI" $=
	(
		/ unifying u8 and PixelType::AlphaType @ YBlit,
		/ refactoring text rendering APIs $=
		(
			/ refactoring current APIs,
			+ $low_prior user-defined rendering APIs,
			/ unifying the basic model for glyphrun and widgets rendering
		),
		+ basic media types and animation support,
		+ more drawing algorithms,
		+ basic backends adaptors,
		+ clipping algorithms,
		+ basic shapes abstraction,
		+ spline nodes abstraction,
		/ more efficient Font switching,
		/ text alignment,
		/ advanced text layout like Unicode layout control
	)
),
/ @ YFramework.YSLib.UI $=
(
	+ IMEs,
	+ widget layout \impl,
	+ modal widget behavior,
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

#endif

