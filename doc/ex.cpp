/*
	© 2009-2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ex.cpp
\ingroup Documentation
\brief 设计规则指定和附加说明 - 存档与临时文件。
\version r6434 *build 449 rev *
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-02 05:14:30 +0800
\par 修改时间:
	2013-10-07 13:30 +0800
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


$DONE:
r1-r*;


$DOING:

$relative_process:
// Mercurial local rev1-rev296+: r11348+;


$NEXT_TODO:
// ...


$TODO:
b[$undetermined];


$LOW_PRIOR_TODO:
^ $low_prior $for_labeled_scope;
b[$undetermined];


$FURTHER_WORK:
b[$undetermined];

#endif

