//v 0.2937; *Build 180 r57;
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
+= \vt @@ \dtor @@ \cl YListBox @@ \h YGUIComponent;

r2:
/ test for b181:
	/ \tr \impl @@ \mf void MDualScreenReader::PrintText() @@ \u DSReader;
/ @@ \u YGUIComponent:
	/ @@ \cl YListBox:
		+ private \m YVerticalScrollBar VerticalScrollBar;
		/ \tr \impl @@ \ctor;
		/ \tr \impl @@ \mf (DrawForeground & GetTopVisualControlPtr);
		+ \mf void FixLayout();
	/ (\decl & \impl) \cl YVerticalScrollBar;
	* \mf GetOrientation @@ \cl YHorizontalScrollBar;
/ \a \param.de 'const Rect& = Rect::FullScreen' -> 'const Rect& = Rect::Empty';

r3:
/ \st TFrmFileListSelecter @@ \cl ShlExplorer @@ \u Shells:
	/ \rem \m YHorizontalScrollBar sbTestH;
	+ \m YVerticalScrollBar sbTestV;
	/ !\rem \m YHorizontalTrack tkTestH;
	/ \rem \m YVerticalScrollBar sbTestV;
	/ \impl @@ \ctor;

r4-r8:
/= test 1;

r9:
* \impl @@ \ctor @@ \cl AScrollBar @@ \impl \u YGUIComponent;

r10:
/ @@ \impl \u YGUIComponent:
	/ @@ \cl AScrollBar:
		/ @@ \mf DrawForeground();
			/ \tr \impl @@ \as;
			* arrows output orientation;
		/ \tr \impl @@ \as
			@@ (\mf (DrawBackground & GetTopVisualControlPtr) & \ctor);
	/ \as @@ \ctor @@ \cl YHorizontalTrack >> \ctor @@ \cl YHorizontalScrollBar;
	/ \as @@ \ctor @@ \cl YVertictalTrack >> \ctor @@ \cl YVerticalScrollBar;
	+ \as @@ \ctor @@ \cl (YHorizontalTrack & YVertictalTrack);

r11:
/ @@ \un \ns @@ \impl \u YGUIComponent:
	* \impl @@ \f WndDrawArrow;
	/ \tr \impl @@ \f RectDrawArrow;
/ \st TFrmFileListSelecter @@ \cl ShlExplorer @@ \u Shells:
	/ \rem \m YHorizontalTrack tkTestH;
	/ !\rem \m YVerticalScrollBar sbTestH;
	/ \impl @@ \ctor;

r12-r13:
/= test 2;

r14:
* \impl @@ \ctor @@ \cl YHorizontalScrollBar @@ \impl \u @@ YGUIComponent;

r15-r16:
* \impl @@ \f WndDrawArrow @@ \un \ns @@ \impl \u YGUIComponent;

r17:
/ @@ \cl ShlSetting::TFormC @@ \u Shells:
	+ \m YButton btnD;
	/ \impl @@ \ctor;
	+ \mf void btnD_Click(TouchEventArgs&);
	+ \f void TestPause(const GHHandle<YDesktop>& = hDesktopDown) @@ \un \ns;
/ \a BlitToBuffer => BlitTo;
/ \a CopyToBuffer => CopyTo;
/ @@ \h YWidget:
	- namespace ColorSpace = Drawing::ColorSpace to avoid ambigity
		@@ \impl \u;
	/ 4 \tr \param @@ 2 \decl @@ \ctor;
/ \tr @@ \h DSReader @@ \impl \u YGUIComponent;

r18:
/ @@ \cl ShlSetting::TFormC @@ \impl \u Shells:
	/ \tr \impl @@ \ctor;
	/ \tr \impl @@ \mf void btnD_Click(TouchEventArgs&);

r19:
* \impl @@ \f TestPause @@ \un \ns @@ \impl \u Shells;

r20-r32:
/ test 3:
	/ \impl @@ \mf void btnD_Click(TouchEventArgs&)
		@@ \cl ShlSetting::TFormC @@ \impl \u Shells;

r33:
/ ((\mf (void btnD_Click(TouchEventArgs&) @@ \cl ShlSetting::TFormC)
	& \f TestPause) @@ \un \ns) @@ \impl \u Shells >> \impl \u file "GBKEX.cpp";
+ \inc \h Shells @@ \impl \u "GBKEX.cpp";

r34-r44:
/ test 4:
	/ \impl @@ \mf void btnD_Click(TouchEventArgs&)
		@@ \cl ShlSetting::TFormC @@ \impl \u "GBKEX.cpp";

r45:
/ @@ \impl \u YGDI:
	/ \tr \impl @@ \s \i \f biltAlphaPoint;
	/= 9 \s \f & >> \un \ns;

r47-r52:
/ test 5:
	/ \impl @@ \mf void btnD_Click(TouchEventArgs&)
		@@ \cl ShlSetting::TFormC @@ \impl \u "GBKEX.cpp";

r53:
/ @@ \u YGDI:
	/ @@ \cl BitmapBuffer:
		/ \i \vt \mf void
		CopyTo(BitmapPtr, Rotation = RDeg0,
			const Size& = Size::FullScreen,
			const Point& = Point::Zero, const Point& = Point::Zero,
			const Size& = Size::FullScreen) const;
			-> CopyTo(BitmapPtr, const Size& = Size::FullScreen,
			const Point& = Point::Zero, const Point& = Point::Zero,
			const Size& = Size::FullScreen, Rotation = RDeg0) const;
	/ @@ \cl BitmapBufferEx:
		/ \vt \mf void
		CopyTo(BitmapPtr, Rotation = RDeg0,
			const Size& = Size::FullScreen,
			const Point& = Point::Zero, const Point& = Point::Zero,
			const Size& = Size::FullScreen) const;
			-> CopyTo(BitmapPtr, const Size& = Size::FullScreen,
			const Point& = Point::Zero, const Point& = Point::Zero,
			const Size& = Size::FullScreen, Rotation = RDeg0) const;
		/ \vt \mf void
		BlitTo(BitmapPtr, Rotation = RDeg0,
			const Size& = Size::FullScreen,
			const Point& = Point::Zero, const Point& = Point::Zero,
			const Size& = Size::FullScreen) const;
			-> BlitTo(BitmapPtr, const Size& = Size::FullScreen,
			const Point& = Point::Zero, const Point& = Point::Zero,
			const Size& = Size::FullScreen, Rotation = RDeg0) const;
/ \impl @@ \mf void MDualScreenReader::PrintText() @@ \u DSReader;
/ \impl @@ \mf void YSimpleListBox::DrawForeground() @@ \u YGUIComponent;
/ \impl @@ \mf void MLabel::PaintText(Widget&, const Point&) @@ \u YWidget;

r54:
* \tr \impl @@ \mf void MDualScreenReader::PrintText() @@ \u DSReader;

r55-r56:
/ test 6:
	/ \impl @@ \mf void btnD_Click(TouchEventArgs&)
		@@ \cl ShlSetting::TFormC @@ \impl \u "GBKEX.cpp";

r57:
+ test fast \i \f blitAlphaPoint !^ alpha blending @@ \un \ns @@ \impl \u YGDI;


$DOING:

relative process:
2011-01-06:
-21.9d;

/ ...


$NEXT:

b181-b215:
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
* alpha blending platform independence;
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

/*	static YTimer Timer(1250);

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

