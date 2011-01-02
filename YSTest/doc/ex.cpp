//v 0.2935; *Build 178 r10;
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
\sf ::= non-member static functions
\sif ::= non-member inline static functions
\simp ::= simplified
\smf ::= static member functions
\sm ::= static member
\scm ::= static const member
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
\u YWidget
{
	\in IWidget;
	\in IUIBox;
	\cl Widget;
	\cl YUIContainer;
	\cl YLabel;
}
\u YControl
{
	\in IControl;
	\in IVisualControl;
	\cl MVisualControl;
	\cl YControl;
	\cl AVisualControl;
	\cl YVisualControl;
}
\u YGUIComponent
{
	\cl AButton;
	\cl YThumb;
	\cl YButton;
	\cl ATrack;
	\cl YHorizontalTrack;
	\cl YVerticalTrack;
	\cl MScrollBar;
	\cl AScrollBar;
	\cl YHorizontalScrollBar;
	\cl YVerticalScrollBar;
	\cl YListBox;
	\cl YFileBox;
}
\u YWindow
{
	\in IWindow;
	\cl MWindow;
	\cl AWindow;
	\cl YFrameWindow;
}

$DONE:
r1:
/= test 0;

r2:
/= private \m FTC_SBit bitmap -> NativeType bitmap
	@@ \cl CharBitmap @@ \h Adaptor::YFont;

r3:
/ \m GHWeak<TextRegion> pTextRegion => wpTextRegion \cl MLabel @@ \u YWidget;

r4:
+ {
	-Wextra -Winit-self -Wmissing-include-dirs \
	-Weffc++ -Wold-style-cast -Woverloaded-virtual 
} @@ \mac CXXFLAG @@ arm9 makefile;

r5:
/ @@ \h YCommon:
	*= -\v @@ \ret \tp @@ \f (GetRTC & timers2msRaw & timers2ms);
	* + \i copy \ctor HDirectory(const HDirectory&) @@ \cl HDirectory;
/ @@ \h YObject @@ \cl Graphics:
	* + \vt @@ DefEmptyDtor(Graphics);
	+ \i copy \ctor Graphics(const Graphics&);
* \tr @@ \i \ctor Path::iterator::iterator() @@ \h YFileSystem;
* + \inh NonCopyable @@ \cl (FontFamily & Typeface) @@ \h YFont;
/ @@ \h YGDI:
	* + \inh NonCopyable @@ \cl BitmapBuffer;
	* + \vt DefEmptyDtor(PenStyle) @@ \cl PenStyle;
	+ \es ythrow() @@ \dtor @@ \cl (BitmapBuffer & BitmapBufferEx);
/ @@ \h YText:
	/ \st TextState -> \cl TextState;
	* + \vt DefEmptyDtor(TextState) @@ \cl TextState;
	/ \em \vt \dtor @@ \cl TextRegion -> \vt DefEmptyDtor(TextRegion);
/ @@ \h YWidget:
	* + \inh NonCopyable @@ \cl Visual;
	/ @@ \cl MLabel:
		+ \inh NonCopyable;
		* \ac @@ \exp \ctor MLabel -> protected ~ public;
		- protected \dtor;
* + \inh NonCopyable @@ (\cl MSimpleFocusResponser & \clt GMFocusResponser)
	@@ \h YFocus;
/ \rem {
	-Weffc++ -Wold-style-cast -Woverloaded-virtual 
} @@ \mac CXXFLAG @@ arm9 makefile;
/ @@ \h YDevice:
	+ \vt DefEmptyDtor(YGraphicDevice) @@ \cl YGraphicDevice;
	+ \vt DefEmptyDtor(YScreen) @@ \cl YScreen;
+ \vt DefEmptyDtor(YImage) @@ \cl YImage @@ \h YResource;

r6:
/= test 1;

r7:
*= \rem 12 !^ \param @@ \u Shells;
*= - 1 !^ \param @@ \t<class _tN> _tRet operator()(_tN&, _tPara)
	@@ \clt ExpandMemberFirstBinder @@ \h YFunc;
*= \rem 2 !^ \param @@ \f YMain @@ \u Main;
*= \rem 4 !^ \param @@ 4 \ft<> codemap @@ \u CharacterMapping;
*= \rem 1 !^ \param @@ \u UTF16LEToMBCS @@ \u CharacterProcessing;
*= \rem 1 !^ \param @@ simpleFaceRequester @@ \u YFont;
*= \rem 1 !^ \param @@ !\impl @@ \f MakeAbsolute @@ \u YFileSystem;
*= \rem 1 !^ \param @@ !\impl @@ \f TranslateMessage @@ \u YShell;
*= - 1 !^ \param @@ \impl @@ \f OnDrag @@ \u YControl;
*= - 4 !^ \param @@ \u YGUIComponent;
/ \simp \impl @@ \mf bool SetLength(SizeType)
	@@ \clt GSequenceViewer @@ \h YComponent;
*= \rem 1 !^ \param @@ !\impl @@ \mf ExecuteCommand
	@@ \cl ShlCLI @@ \u ShellDS;
/ + proprecessing segments of #ifdef __GNUC__ & #pragma GCC diagnostic
	to supress warning 'vitrual base ambiguity' with option '-Wextra'
	@@ \h YCast;
+ -Wsign-promo @@ \mac CXXFLAG @@ arm9 makefile;
/ @@ \u YGDI:
	- 1st \param @@ \i \f (blitMaxX & blitMaxY) @@ \un \ns;
	/ \tr \impl @@ \f blitScale;

r8:
/ ^ #pragma GCC system_header ~ #pragma GCC diagnostic
	to suppress \a warnings @@ \h YCast;

r9:
/= test 2;

r10:
/ test for b179;
	/ \tr \impl @@ \mf DrawForeground @@ \YSimpleListBox @@ \u YGUIComponent;
	/ \tr \impl @@ \mf ShlSetting::TFormC::btnC_Click @@ \u Shells;

$DOING:

relative process:
2011-01-02:
-20.6d;

/ ...


$NEXT:
* screen output polluted @@ real machine;
* fatal \err @@ b177 when press L + click on the ListBox:
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 0201f484
GetTouchedVisualControlPtr
ygui.cpp:215
??
ygui.cpp:325


b179-b195:
/ scroll bars @@ listbox;
/ fully \impl \u DSReader;
	* moved text after setting lnGap;
* non-ASCII character path error in FAT16;
/ non-ordinary operator used in \clt GSequenceViewer @@ \h YComponent;

r196-r288:
+ \impl loading pictures;
+ \impl style on widgets;
/ user-defined bitmap buffer @@ \cl YDesktop;
/ general component operations:
	/ serialization;
	/ designer;
/ database interface;

$NOTHING_TODO:
* fatal \err @@ b16x:
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
	-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 02055838
	ftc_snode_weight
	ftcsbits.c:271

$TODO:

Clarify the log levels.

Make "UpdateFont()" more efficienct.

More efficient %YTextRegion output:
Use in-buffer background color rendering and function %CopyToScreen()
	to inplements %YTextRegion background;
Use pre-refershing to make font changing.

Consider to simplify the general window class: @YForm.

Rewrite system RTC.

Build a more advanced console wrapper.

Build a series set of robust gfx APIs.

GUI characteristics needed:
Icons;
Other controls.

Other stuff to be considered to append:
Design by contract: DbC for C/C++, GNU nana.

*/
//---- temp code;
/*

//! \brief 桌面对象模块。
class MDesktopObject
{
protected:
	GHHandle<YDesktop> hDesktop; //!< 桌面句柄。

public:
	//! \brief 构造：使用指定桌面句柄。
	explicit
	MDesktopObject(GHHandle<YDesktop>);

protected:
	DefEmptyDtor(MDesktopObject)

public:
	//判断从属关系。
	PDefH(bool, BelongsTo, GHHandle<YDesktop> hDsk) const
		ImplRet(hDesktop == hDsk)

	DefGetter(GHHandle<YDesktop>, DesktopHandle, hDesktop)
};

inline
MDesktopObject::MDesktopObject(GHHandle<YDesktop> hDsk)
	: hDesktop(hDsk)
{}

class RefCountedImpl
{
public:
	typedef uintptr_t CountType;

private:
	CountType* pCount;

protected:
	RefCountedImpl()
		: pCount_(static_cast<CountType*>(
		SmallObject<>::operator new(sizeof(CountType))))
	{
		assert(pCount_ != NULL);
		*pCount_ = 1;
	}

	inline
	RefCountedImpl(const RefCountedImpl& rhs)
		: pCount_(rhs.pCount_)
	{}

	inline CountType
	Clone()
	{
		return ++*pCount;
	}

	bool
	Release()
	{
		if(--*pCount_ == 0)
		{
			SmallObject<>::operator delete(pCount_, sizeof(CountType));
			pCount_ = NULL;
			return true;
		}
		return false;
	}

	void
	Merge(RefCountedImpl& rhs)
	{
		CountType count(*pCount_);

		SmallObject<>::operator delete(pCount_, sizeof(CountType));
		pCount_ = rhs.pCount_;
		*pCount_ += count;
		return true;
	}
};
*/

