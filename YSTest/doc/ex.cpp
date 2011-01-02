//v 0.2937; *Build 179 r11;
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
/ test for b180 part 1:
	/ \tr \impl @@ \mf DrawForeground @@ \YSimpleListBox @@ \u YGUIComponent;
	/ \tr \impl @@ \mf ShlSetting::TFormC::btnC_Click @@ \u Shells;

r2:
/ \a YSimpleTextListBox => YSimpleListBox;
+ 2 \tf polymorphic_crosscast @@ \ns ystdex @@ \h YCast;

r3-r5:
* \impl @@ \tf<class _tDst, class _tSrc> \i _tDstpolymorphic_crosscast(_tSrc*)
	@@ \h YCast;
/ @@ \impl \u YWidget:
	- \tf Fill @@ \un \ns;
	/ \impl \f void Fill(IWidget&, Color);
	- \f void Fill(Widget&, Color) ^ ystdex::polymorphic_crosscast;;

r6:
/ \impl @@ \mf void Widget::Refresh() @@ \impl \u YWidget
	^ ystdex::polymorphic_crosscast;

r7:
/ @@ \h YWidget:
	- \ft<class _tWidget> IWindow* FetchDirectWindowPtr(_tWidget&);
	/ \impl @@ \i \f IWindow* FetchDirectWindowPtr(IWidget&);
	- \ft<class _tWidget> IUIBox* FetchDirectContainerPtr(_tWidget&);
	/ \impl @@ \i \f IUIBox* FetchDirectContainerPtr(IWidget&);
	- \ft<class _tWidget> IWindow* FetchWindowPtr(const _tWidget&);
	/ \impl @@ \i \f IWindow* FetchWindowPtr(const IWidget&);
	/ \impl @@ 2 \mac YWidgetAssert ^ ystdex::general_cast;
	/ \impl @@ \mf MLabel::PaintText;

r8:
- \i @@ \f IUIBox* FetchDirectContainerPtr(IWidget&) @@ \u YWidget;

r9:
/ \ft<class _tWidget> IWindow* FetchWidgetDirectWindowPtr(_tWidget*)
	-> \f IWindow* FetchWidgetDirectWindowPtr(IWidget*) @@ \u YWidget;

r10:
/ @@ \u YWidget:
	/ merge \impl @@ \f (FetchWidgetDirectWindowPtr
		& FetchWidgetDirectWindowPtr)
		-> \ft<class _tNode> _tNode* FetchWidgetDirectNodePtr(IWidget*);
	/ \impl @@ \f (FetchDirectContainerPtr
		& FetchDirectWindowPtr & FetchDirectDesktopPtr);

r11:
/ @@ \cl YSimpleListBox @@ \u YGUIComponent:
	/ \m GHWeak<Drawing::TextRegion> pTextRegion => wpTextRegion;
	/ \tr \impl @@ \mf DrawForeground;
/ '-O2' -> '-O0' @@ \impl @@ \mac CFLAGS @@ arm9 Makefile;
/ @@ \impl \u YCommon:
	* extern u8 default_font_bin[] => extern u8 default_font[]
		>> extern "C" @@ \un \ns;
	/= \st PrintConsole mainConsole & \f PrintConsole* consoleMainInit()
		>> \un \ns ~ static;


$DOING:

relative process:
2011-01-02:
-19.9d;

/ ...


$NEXT:

b180-b215:
* screen output polluted @@ real machine;
* fatal \err @@ b177 when press L + click on the ListBox:
[
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 0201f484
GetTouchedVisualControlPtr
ygui.cpp:215
??
ygui.cpp:325
]
/ improve efficiency @@ \tf polymorphic_crosscast @@ \h YCast;
/ scroll bars @@ listbox;
/ fully \impl \u DSReader;
	* moved text after setting lnGap;
* non-ASCII character path error in FAT16;
/ non-ordinary operator used in \clt GSequenceViewer @@ \h YComponent;

r196-r288:
+ \impl loading pictures;
+ \impl style on widgets;
+ \impl general blit algorithm;
/ user-defined bitmap buffer @@ \cl YDesktop;
/ general component operations:
	/ serialization;
	/ designer;
/ database interface;


$NOTHING_TODO:
* fatal \err @@ b16x:
[
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
	-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 02055838
	ftc_snode_weight
	ftcsbits.c:271
]


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

