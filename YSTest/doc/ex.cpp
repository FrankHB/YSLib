//v0.3032; *Build 197 r107;
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
r1-r18:
/ @@ \cl ShlGUI @@ \u Shell_DS:
	/ \rem \mf void SendDrawingMessage();
/ @@ \u YGUI:
	+ \f GHHandle<YGUIShell> FetchGUIShellHandle(IWidget&);
	/ \f bool IsFocusedByShell(const IControl&,
		GHHandle<YGUIShell> = FetchGUIShellHandle())
		-> bool IsFocusedByShell(IControl&);
	/ \impl @@ \mf YGUIShell::ShlProc;
	/ \ren @@ \h;
/ @@ \impl \u Shell:
	- \inc \h YDesktop;
	/ \impl @@ \mf YShell::DefShlProc;
/= test 1;
/= \tr \decl @@ \u (YShell & YShellMessage);
/= test 2;
/ @@ \h Shells:
	+ typedef ShlDS ParentType @@ \cl (ShlLoad & ShlExplorer);

r19:
/ @@ \u YFont:
	/ @@ \cl FontFamily:
		/ private \mf void operator+=(Typeface&)
			-> \mf void operator+=(Typeface*);
		/ private \mf bool operator-=(Typeface&)
			-> \mf bool operator-=(Typeface*);
	/ \tr \impl @@ \mf YFontCache::LoadTypefaces;

r20:
/ @@ \u YFont:
	/ @@ \cl YFontCache:
		/ private \vt \mf void operator+=(const FontFile&)
			-> !\vt \mf void operator+=(const FontFile*);
		/ private \vt \mf bool operator-=(const FontFile&)
			-> !\vt \mf bool operator-=(const FontFile*);
	/ @@ \cl YFontCache;
		/ \impl @@ \mf void LoadFontFileDirectory(CPATH, CPATH = "ttf");
		/ \impl @@ \mf void LoadFontFile(CPATH path) ythrow()

r21:
/ @@ \u YFont:
	/ @@ \cl FontFile:
		/ \mf void ReloadFaces() -> void ReloadFaces(FT_Library&) const;
		- \m FT_Library library;
		/ \ctor FontFile(CPATH, FT_Library&) -> FontFile(CPATH);
		/ \ctor FontFile(CPATH, const char*, FT_Library&)
			-> FontFile(CPATH, const char*);
		/ \impl @@ \mf bool operator==(const FontFile&) const;
		/ \impl @@ \mf bool operator<(const FontFile&) const;
		+ \inh public NonCopyable;
		/ private \m FT_Long nFace -> mutable FT_Long nFace;
	/ @@ \cl YFontCache:
		/ \tr \impl @@ \mf LoadFontFile;
		/ \tr \impl @@ \mf LoadFontFileDirectory;
/ \simp \eh @@ \impl @@ \f main @@ \impl \u YGlobal;

r22:
/ @@ \cl YFontCache @@ \u YFont:
	/ \impl @@ \mf void LoadFontFileDirectory(CPATH, CPATH = "ttf");
	/ void LoadFontFile(CPATH path) ythrow()
		-> bool LoadFontFile(CPATH path) ythrow();
	/ \tr \impl @@ \ctor;
	+ \mf void ResetDefaultTypeface();
	* \impl @@ \mf bool operator-=(Typeface&);
/ \impl @@ \f void InitializeSystemFontCache() @@ \impl \u YShellInitialization;

r23:
/ \impl @@ \mf void DestroyFontCache() & \mf void ResetFontCache()
	@@ \cl YApplication;
/ @@ \u YFont:
	- \f (CreateFontCache & DestroyFontCache) @@ \ns Drawing ;
	* \tr \impl @@ bool YFontCache::LoadFontFile(CPATH) ythrow();

r24:
/ @@ \cl YFontCache @@ \u YFont:
	/ \impl @@ \ctor;
	/ \mf bool LoadFontFile(CPATH path) ythrow()
		-> bool LoadFontFile(CPATH path);

r25:
/ \impl @@ \ctor @@ \cl YFontCache @@ \impl \u YFont;

r26:
/ @@ \u YFont:
	-= \inc \h YCoreUtilities @@ \h;
	+ \inc \h <string>;
	/ @@ \cl FontFile:
		+ typedef std::string ParhType;
		/ private \m CPATH path -> PathType;;
		-= \ctor FontFile(CPATH, const char*);
		/ \ctor FontFile(CPATH) -> FontFile(const PathType&);
		/ \impl @@ bool operator==(const FontFile&) const;
		/ \impl @@ bool operator<(const FontFile&) const;
		+ \i @@ \mf \op== & \op<;
		- \sm MaxFontPathLength;
		/ \mf DefGetter(const char*, Path, path)
			-> DefGetter(PathType, Path, path);
		/ \tr \impl @@ \mf ReloadFaces;
	/ \tr \impl @@ \f simpleFaceRequester;
	/ \tr \impl @@ \mf LoadFontFileDirectory @@ \cl YFontCache;
/ \a EFontStyle => FontStyle;

r27-r28:
/ @@ \cl YFontCache @@ \u YFont:
	/ \mf void LoadFontFileDirectory(CPATH, CPATH = "ttf")
		-> !\m \f LoadFontFileDirectory(YFontCache&, CPATH, CPATH = "ttf")
		@@ \un \ns @@ \impl \u YShellInitialization;
	/ \impl @@ \ctor;
	/ \impl @@ \mf LoadFontFile;
/ @@ \impl \u YShellInitialization:
	/ \impl \f void InitializeSystemFontCache();
	/ @@ \ns YSLib:
		+ using namespace Drawing;
		+ using namespace IO;
/= test 3 ^ \conf release;

r29-r31:
/= test 4;

r32-r33:
* \impl @@ \mf void MLabel::PaintText(Widget&, const Graphics&, const Point&)
	@@ \impl \u YLabel;

r34:
/ @@ \u YText:
	/ \f SDst FetchStringWidth(TextState&, String&, SDst)
		-> \i SDst \i FetchStringWidth(TextState&, SDst, const String&);
	+ \tf template<typename _tIn, typename _tChar> SDst FetchStringWidth(Font&,
		_tIn, _tIn, _tChar = '\0');
	+ \i \f SDst FetchStringWidth(Font&, const String&);
	+ \tf template<typename _tIn, typename _tChar> SDst FetchStringWidth(
		TextState&, SDst, _tIn, _tIn, _tChar = '\0');
	/= \a 'class _tOut' -> 'typename _tIn';
	/= \a _tOut => _tIn;
	- \a duplicate \param @@ \t \def;
	+ \f SDst FetchCharWidth(Font&, fchar_t);
	+ \tf<typename _tIn> SDst FetchStringWidth(Font&, _tIn);
	+ \tf<typename _tIn> SDst FetchStringWidth(TextState&, SDst, _tIn);
/ \tr \impl @@ \mf void MLabel::PaintText(Widget&, const Graphics&,
	const Point&) @@ \impl \u YLabel;

r35:
* \impl @@ \mf void MLabel::PaintText(Widget&, const Graphics&, const Point&)
	@@ \impl \u YLabel;

r36:
/ \impl @@ \ctor @@ \cl ShlSetting::TFormC @@ \u Shells;

r37:
/ \impl @@ \mf void MLabel::PaintText(Widget&, const Graphics&, const Point&)
	@@ \impl \u YLabel;

r38:
/ undo r36;

r39-r43:
/ test 5 ^ \conf release:
	/ \impl @@ \mf void ShlSetting::TFormC::btnC_Click(TouchEventArgs&)
		@@ \impl \u Shells;

r44-r45:
/= test 6;

r46:
/ \impl @@ \ctor @@ \cl ShlSetting::TFormC @@ \u Shells;

r47:
* \impl @@ \f void InitializeSystemFontCache() @@ \impl \u YShellInitialization;
/ @@ \cl YFontCache @@ \h YFont:
	/ \ac @@ 2 \mf LoadTypefaces -> public ~ private;
	/ \impl @@ \mf void LoadTypefaces(const FontFile&);

r48:
* \impl @@ \f void InitializeSystemFontCache() @@ \impl \u YShellInitialization;

r49-r51:
/= test 7;

r52:
/ @@ \cl Font @@ \u YFont:
	/ \impl @@ \i \smf GetDefault;
	- \mf InitializeDefault;
	- \mf ReleaseDefault;
/ \impl @@ \f void InitializeSystemFontCache() @@ \impl \u YShellInitialization;
/ \impl @@ void YApplication::DestroyFontCache();

r53-r55:
/= test 8;

r56: 
/ @@ \u YFont:
	* \tr \impl @@ \f const Typeface& FetchDefaultTypeface()
		ythrow(LoggedEvent);
	/ @@ \cl FontFamily:
		+ typedef std::string NameType;
		/ private \m FT_String* family_name -> \m NameType family_name;
		/ \ctor FontFamily(YFontCache&, const FT_String*)
			-> FontFamily(YFontCache&, const NameType&);
		/ \dtor -> \i \dtor ^ \mac DefEmptyDtor;
		/ \tr @@ \mf GetFamilyName && \op== && op<;
	/ @@ \cl YFontCache:
		/ typedef map<const FT_String*, FontFamily*,
			ystdex::deref_str_comp<FT_String> > FFacesIndex
			-> 	typedef map<const FontFamily::NameType, FontFamily*>
			FFacesIndex;
		/ \tr @@ \mf GetFontFamilyPtr;
		/ \tr @@ \mf GetTypefacePtr;
		/ \tr \impl @@ \mf LoadTypefaces;
	/ \tr @@ \i \mf Typeface::GetFamilyName;
	+ \inc \h YStatic;
/ \tr \impl @@ \mf ShlSetting::TFormC::btnC_Click @@ \impl \u Shells;
/ \tr \impl @@ \f void InitializeSystemFontCache()
	@@ \impl \u YShellInitialization;

r57:
/ @@ \u YFont:
	/ @@ \cl Typeface:
		+ typedef std::string NameType;
		/ private \m FT_String* style_name -> \m NameType style_name;
		/ \tr \impl @@ \ctor;
		/ \dtor -> \i \dtor ^ \mac DefEmptyDtor;
		/ \tr @@ \mf GetStyleName;
	/ @@ \cl YFontCache:
		/ \tr @@ \mf GetTypefacePtr;
	/ @@ \cl FontFamily:
		/ \tr @@ \mf GetTypefacePtr;
		/ typedef map<const FT_String*, Typeface*,
			ystdex::deref_str_comp<FT_String> >
			-> typedef <const std::string, Typeface*> FTypesIndex;
	/ \tr @@ \mf Font::GetStyleName;
	/ @@ \mf Typeface::GetFamilyName;
	- \inh \h YStatic;
/ \tr \impl @@ \f void InitializeSystemFontCache()
	@@ \impl \u YShellInitialization;
/ \tr \impl @@ \mf ShlSetting::TFormC::btnC_Click @@ \impl \u Shells;

r58-r65:
/= test 9;

r66-r67:
* \impl @@ \mf void YFontCache::LoadTypefaces(const FontFile&)
	@@ \impl \u YFont;

r68-r70:
/= test 10;

r71:
* \tr \impl @@ \f simpleFaceRequester @@ \impl \u YFont;

r72-r78:
/= test 11;

r79-r103:
/= test 12;

r104:
/ @@ \cl YFontCache @@ \impl \u YFont:
	/ \tr \impl @@ \mf bool SetTypeface(Typeface*);
	* \impl @@ \mf void LoadTypefaces(const FontFile&);

r105:
/= test 13 ^ \conf release;

r106:
/ @@ \u YFont:
	/ @@ \cl YFontCache:
		/ private \mf \vt void operator+=(Typeface&)
			-> !\vt void operator+=(Typeface*);
		/ private \mf \vt bool operator-=(Typeface&)
			-> !\vt bool operator-=(Typeface*);
		/ private \mf \vt void operator+=(FontFamily&)
			-> !\vt void operator+=(FontFamily*);
		/ private \mf \vt bool operator-=(FontFamily&)
			-> !\vt bool operator-=(FontFamily*);
		/ \simp \impl @@ \mf bool operator-=(const FontFile*);

r107:
/= test 14 ^ \conf release;


$DOING:

relative process:
2011-03-30:
-26.6d;

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
* "label alignment",
"%std::string based font cache"
},

b196:
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

