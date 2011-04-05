//v0.3077; *Build 198 r154;
/*
$Record prefix and abbrevations:
<statement> ::= statement;
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
	\cl AFrame;
	\cl YFrame;
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
r1-r2:
/ \def @ \f LoadFontFileDirectory @ \un \ns @ \impl \u YShellInitialization;
/= test 1;

r3:
/ @ \dir Adaptor:
	/ \mac GLYPH_CACHE_SIZE (512 << 10) @ \h YFont
		-> \c std::size_t GLYPH_CACHE_SIZE(1024 << 10) @ \ns Drawing;
	/ \mac AGG_INCLUDED => INCLUDED_AGG_H_ @ \h "agg.h";
- 3 \mac 'DEF_SHELL_*' @ \h YCommon;
/ \a Font::DefSize => Font::DefaultSize;
/ \a Font::MinSize => Font::MinimalSize;
/ \a Font::MaxSize => Font::MaximalSize;

r4:
/= test 2;

r5:
/ @ \u YFont:
	/ \c std::size_t GLYPH_CACHE_SIZE(1024 << 10)
		-> \c std::size_t GLYPH_CACHE_SIZE(128 << 10);
	/ @ \cl YFontCache:
		/ \tr @ \mf order;
		+ \i \mf void ResetGlyphCache();

r6-r11:
/= test 3;

r12:
/ \impl @ \f InitializeSystemFontCache @ \impl \u YShellInitialization;
/ \mf ResetDefaultTypeface => InitializeDefaultTypeface
	@ \cl YFontCache @ \u YFont;

r13-r18:
/= test 4;

r19:
/ @ \cl YFontCache @ \u YFont:
	- \m Font::SizeType curSize;
	/ \tr \impl @ \mf SetFontSize;
	/ \impl @ \ctor;
	/ \impl @ \i \mf GetFontSize;

r20:
/ \impl @ \mf LoadTypefaces @ \cl YFontCache @ \impl \u YFont;

r21:
/ @ \cl YFontCache @ \u YFont:
	- private \m Typeface* pFace;
	/ \tr \impl @ \i \mf GetTypefacePtr;
	/ \tr \impl @ \ctor;

r22:
/ @ \cl YFontCache @ \u YFont:
	/ \impl @ \mf FTC_SBit CharBitmap GetGlyph(fchar_t);
	- private \m FTC_SBit sbit;

r23:
/ @ \u YWindow:
	/ \mf void AWindow::Show() -> !\m \f bool Show(HWND);
	+ \f bool Hide(HWND);
/ @ \cl ShlSetting@ \u Shells:
	/ @ \cl TFormB:
		+ \mf btnB2_Click();
/ \a _handle => _tHandle;

r24:
/ @ \cl ShlSetting::TForm @ \u Shells:
	/ \impl @ \ctor @ \cl ShlSetting::TFormB ;
	* \decl @ \mf void btnB2_Click(InputEventArgs&)
		-> void btnB2_Click(TouchEventArgs&);

r25:
/ @ \u Shell_DS:
	/ \mg \a \mf @ \cl ShlGUI @ \ns Shells -> \cl ShlDS @ \ns DS;
	/ @ \cl ShlDS:
		- \mf DefGetter(HWND, UpWindowHandle, hWndUp);
		- \mfDefGetter(HWND, DownWindowHandle, hWndDown);
		/ \impl @ \mf ShlProc;
		/ \inh \cl Shells::ShlGUI -> Shells::YGUIShell;
	- \cl ShlGUI;
/ @ \cl ShlReader @ \u Shells:
	/ \inh \cl Shells::ShlGUI -> ShlDS;
	/ typedef ShlGUI ParentType -> typedef ShlDS ParentType;
	/ \ac @ \mf OnClick & OnKeyPress -> private ~ public;
	/ \tr \impl @ \ctor;

r26-r28:
* \impl @ \mf YListBox::OnViewChanged_TextListBox @ \impl \u ListBox;

r29:
/ @ \impl \u Scroll:
	* \tr \impl @ \mf void ScrollableContainer::DrawForeground();
* \impl @ \cl YListBox @ \impl \u ListBox:
	/ \impl @ \mf DrawForeground;
	/ \impl @ \mf OnViewChanged_TextListBox;

r30:
* @ \u ListBox:
	* \impl @ \cl YListBox:
		/ \impl @ \ctor;
	/ \ac @ \mf GetTextState @ \cl YSimpleListBox -> public ~ protected;

r31:
/ @ \h YReference:
	+ typedef @ \clt (GHStrong & GHWeak & GHHandle);
	* @ \ctor:
		- \ctor GHHandle(T&) @ \clt GHHandle;
		/ @ \clt GHWeak:
			- \ctor GHWeak(T&);
			/ \ctor GHWeak(RefToValue<GHStrong<T> >)
				-> 	/ \ctor GHWeak(RefToValue<GHStrong<StrongPtrType>)
			* \ctor GHWeak(GHStrong<T>) -> GHWeak(StrongPtrType p)
		- \ctor GHStrong(T&) @ \clt GHStrong;
	/ \a SPType => SPT;

r32:
/ @ \cl ShlDS @ \u Shell_DS:
	+ private \m GHHandle<YDesktop> hDskUp, hDskDown;
	/ \ctor ShlDS() -> ShlDS(GHHandle<YDesktop>
		= theApp.GetPlatformResource().GetDesktopUpHandle(), GHHandle<YDesktop>
		= theApp.GetPlatformResource().GetDesktopDownHandle());
	/ \impl @ \mf UpdateToScreen;
	/ \impl @ \mf OnDeactivated;
	+ \mf DefGetter(const GHHandle<YDesktop>&, DesktopUpHandle, hDskUp);
	+ \mf DefGetter(const GHHandle<YDesktop>&, DesktopDownHandle, hDskDown);

r33:
/ \impl @ \mf ShlDS::OnDeactivated @ \impl \u YShell_DS;

r34:
/ \impl (\mf (OnActivated & OnDeactivated & UpdateToScreen) @ \cl ShlReader
	& \mf ShlLoad::OnActivated) @ \impl \u Shells;

r35:
/ @ \h YUIContainer:
	/ @ \in IUIContainer:
		/ \amf void operator+=(IWidget&) -> void operator+=(IWidget*);
		/ \amf bool operator-=(IWidget&) -> bool operator-=(IWidget*)
		/ \amf void operator+=(IControl&) -> void operator+=(IControl*);
		/ \amf bool operator-=(IControl&) -> bool operator-=(IControl*)
		/ \amf void operator+=(GMFocusResponser<IControl>&)
			-> void operator+=(GMFocusResponser<IControl>*);
		/ \amf bool operator-=(GMFocusResponser<IControl>&)
			-> bool operator-=(GMFocusResponser<IControl>*)
	/ @ \cl MUIContainer:
		/ \mf void operator+=(IControl&) -> void operator+=(IControl*);
		/ \mf bool operator-=(IControl&) -> bool operator-=(IControl*);
		/ \mf void operator+=(GMFocusResponser<IControl>&)
			-> void operator+=(GMFocusResponser<IControl>*);
		/ \mf bool operator-=(GMFocusResponser<IControl>&)
			-> bool operator-=(GMFocusResponser<IControl>*);
	/ @ \cl YUIContainer:
		/ \mf void operator+=(IWidget&) -> void operator+=(IWidget*);
		/ \mf bool operator-=(IWidget&) -> bool operator-=(IWidget*);
		/ \mf void operator+=(IControl&) -> void operator+=(IControl*);
		/ \mf bool operator-=(IControl&) -> bool operator-=(IControl*);
		/ \mf void operator+=(GMFocusResponser<IControl>&)
			-> void operator+=(GMFocusResponser<IControl>*);
		/ \mf bool operator-=(GMFocusResponser<IControl>&)
			-> bool operator-=(GMFocusResponser<IControl>*);
/ @ \h YWindow:
	/ @ \cl AFrameWindow:
		/ \mf void operator+=(IWidget&) -> void operator+=(IWidget*);
		/ \mf bool operator-=(IWidget&) -> bool operator-=(IWidget*);
		/ \mf void operator+=(IControl&) -> void operator+=(IControl*);
		/ \mf bool operator-=(IControl&) -> bool operator-=(IControl*);
		/ \mf void operator+=(GMFocusResponser<IControl>&)
			-> void operator+=(GMFocusResponser<IControl>*);
		/ \mf bool operator-=(GMFocusResponser<IControl>&)
			-> bool operator-=(GMFocusResponser<IControl>*);
/ \tr \impl @ (\ctor & \dtor) @ \cl Control @ \impl \u YControl;
/ \tr \impl @ (\ctor & \dtor) @ \cl YUIContainer @ \impl \u YUIContainer;
/ \tr \impl @ (\ctor & \dtor) @ \cl YWidget @ \impl \u YWidget;
/ \tr \impl @ (\ctor & \dtor) @ \cl AFrameWindow @ \impl \u YWindow;
/ \a AFrameWindow => AFrame;
/ \a \cl YWindow => YFrame;

r36:
/= test 5 ^ \conf release;

r37:
/ @ \cl YGUIShell @ \u YGUI:
	/ \impl @ \mf void ClearScreenWindows(YDesktop&);
	/ \rem \mf HWND GetFirstWindowHandle() const;
	/ \rem \mf HWND GetTopWindowHandle() const;

r38:
/ \impl @ \mf int OnActivated(const Message&) @ \cl (ShlExplorer & ShlLoad);

r39:
/= test 6 ^ \conf release;

r40:
/ @ \cl YGUIShell @ \u YGUI:
	- (\rem \mf HWND GetFirstWindowHandle() const);
	- (\rem \mf HWND GetTopWindowHandle() const);
	- \mf void DispatchWindows();
	- \mf WNDs::size_type RemoveAll();
	- \mf void RemoveWindow();
	- \mf void operator+=(HWND);
	- \mf void operator-=(HWND);
	- private \m WNDs sWnds;
	- typedef list<HWND> WNDs;
	/ \tr \impl @ \ctor;
	/ \simp \impl @ \mf HWND GetTopWindowHandle(YDesktop&, const Point&) const;
	- \mf bool SendWindow(IWindow&);
/ \tr \impl @ \ft<class _type> HWND NewWindow() @ \h YShellHelper;
/ \tr \impl @ \mf int ShlDS::OnDeactivated(const Message&)
	@ \impl \u Shell_DS;
/ \tr \impl @ \mf int ShlSetting::OnDeactivated(const Message&)
	@ \impl \u Shells;

r41:
/ @ \cl YGUIShell @ \u YGUI:
	- \mf HWND GetTopWindowHandle(YDesktop&, const Point&) const;
	- \mf IWidget* GetCursorWidgetPtr(YDesktop&, const Point&) const;

r42:
/= test 7 ^ \conf release;

r43:
/ @ \u Shell_DS:
	/ \impl @ \mf ShlDS::OnDeactivated;
	- \f ShlClearBothScreen;
	- \i \f NowShellDrop;
/ \impl @ \mf ShlReader::OnDeactivated @ \impl \u Shells;
- \mf void YGUIShell::ClearScreenWindows(YDesktop&) @ \u YGUI;

r44:
/ @ \cl MUIContainer @ \u YUIContainer:
	- (\i & \vt) @ protected \vt void operator+=(IControl*);
	- (\i & \vt) @ protected \vt bool operator-=(IControl*);
	- \i @ protected void operator+=(GMFocusResponser<IControl>*);
	* (- \i & \impl) @ protected bool operator-=(GMFocusResponser<IControl>*);
- \i @ \mf void AFrame::operator+=(IWidget*) @ \u YWindow;

r45:
/ \simp \impl @ \mf IControl* MUIContainer::GetTopControlPtr(const Point&)
	@ \impl \u YUIContainer;
/ error message string;
/ @ \cl YDesktop @ \u YDesktop:
	/ \mf DOs::size_type RemoveAll(IControl&)
		-> \mf DOs::size_type RemoveAll(IControl*);
	/ \mf \vt void operator+=(IControl&) -> \vt void operator+=(IControl*);
	/ \mf \vt bool operator-=(IControl&) -> \vt bool operator-=(IControl*);
	* typedef YComponent ParentType -> typedef YFrame ParentType;
/ \tr \impl @ (\ctor & \dtor) @ \cl YFrame @ \impl \u YWindow;

r46:
/ \cl (ShlLoad & ShlExplorer & ShlSetting) @ \u Shells:
	+ private \m HWND hWndUp, hWndDown;
	((+ \mf) | (/ \impl)) \vt int OnDeactivated(const Message&);
/ @ \cl ShlDS @ u Shell_DS:
	/ \impl @ \mf int OnDeactivated(const Message&);
	- protected \m HWND hWndUp, hWndDown;
/ \mac DefEmptyDtor @ \h Base;
/ - \es @ \vt \dtor @ \cl YShell;
/ - \es @ \i \vt \dtor @ \cl (BitmapBuffer & BitmapBufferEx);
/ - \es @ \vt \dtor @ \cl (Control & AFrame & YFrame & YForm & YUIContainer);

r47:
- \a DefEmptyDtor(*) @ \a \cl \exc \cl ((FileList @ \u YFileSystem)
	& (Graphics @ \u YObject) & (AFocusRequester @ \u YFocus)
	& (PenStyle @ \u YGDI) & (ATextRenderer & \u YText)
	& (MUIContainer @ \u YUIContainer) & (Visual @ \u YWidget));
+ \mac ImplEmptyDtor(_type) @ \h Base;
* @ \cl AFocusRequester:
	/ \dtor -> \adtor ^ \mac (DeclIEntry & ImplEmptyDtor);
/ \simp @ \def @ \cl AFocusRequester:
	-= empty \ctor AFocusRequester;

r48:
/ ^ \conf release;
/= test 8;

r49-r64:
/= test 9 ^ \conf release;
/= test 10 ^ \conf debug;

r65:
/ @ \cl MUIContainer:
	+ protected \mf void \op+=(IWidget*);
	+ protected \mf bool \op-=(IWidget*);
	/ protected \impl @ \mf void \op+=(IControl*);
	/ protected \impl @ \mf void \op-=(IControl*);
* @ \cl AFrame:
	/ \impl @ \mf \vt void \op+=(IWidget*) ^ \mf @ \cl MUIContainer;
	/ \impl @ \mf \vt bool \op-=(IWidget*) ^ \mf @ \cl MUIContainer;

r66:
* \mf IControl* GetTopDesktopObjectPtr(const Point&) const
	-> IControl* GetTopVisibleDesktopObjectPtr(const Point&) const
	@ \cl YDesktop;
* \impl @ \mf (GetTopWidgetPtr & GetTopControlPtr) @ \cl MUIContainer;

r67:
/ @ \u YWindow:
	/ @ \cl YFrame:
		+ protected \mf bool DrawContensBackground();
		/ \impl @ \mf DrawWidgets ^ DrawContensBackground;
	/ \a DrawWidgets => DrawContents;
	/ \impl @ \mf AWindow::Draw;

r68-r71:
/ @ \cl YDesktop:
	/ \mf !\vt void DrawDesktopObjects() -> \vt bool DrawContents();
	- \mf Draw;

r72:
/ \impl @ mf bool YDesktop::DrawContents();

r73:
/ \cl Font @ \impl \u YFont:
	/ Font::DefaultSize(16) -> Font::DefaultSize(14);
	/ Font::DefaultSize(72) -> Font::DefaultSize(96);

r74:
/= test 11 ^ \conf release;

r75:
- \a \tp IUIBox* @ \a \ctor @ \ns Components @ \dir Shell;
/ \tr @ \u Shells;
/ @ \h YWidget:
	/ private \m IUIBox* pContainer -> mutable private \m @ \cl Widget;
	/ \ret \tp @ \amf GetContainerPtr @ \in IWidget -> IUIBox*& ~ IUIBox*;
/ \tr \decl @ \mf GetContainerPtr @ \cl (Widget & YWidget & Control
	& YUIContainer);

r76-r77:
/= test 12;

r78:
* @ \cl MUIContainer @ \impl \u YUIContainer:
	/ \impl @ \mf void operator+=(IWidget*);
	/ \impl @ \mf void operator+=(IControl*);
	/ \impl @ \mf bool operator-=(IWidget*);
	/ \impl @ \mf bool operator-=(IControl*);

r79-r80:
/= test 13;

r81:
* \tr \impl @ \impl \u YWindow;

r82-r86:
/= test 14;

r87:
* @ \cl MUIContainer @ \impl \u YUIContainer:
	/ undid r78;
* @ \cl YFrame @ \u YWindow:
	/ \impl @ \mf void operator+=(IWidget*);
	/ \impl @ \mf void operator+=(IControl*);
	/ \impl @ \mf bool operator-=(IWidget*);
	/ \impl @ \mf bool operator-=(IControl*);

r88-r89:
* \impl \mf (OnActivated & OnDeactivated) @ \cl (ShlLoad & ShlExplorer)
	@ \impl \u Shells;

r90-r96:
/= test 15;

r97:
* \impl @ \mf bool YDesktop::DrawContents() @ \impl \u YDesktop;

r98-r108:
/= test 16;

r109-r110:
* \impl @ \mf bool YDesktop::operator-=(IControl*) @ \impl \u YDesktop;

r111-r114:
/= test 17;

r115:
* @ \cl AFrame @ \impl \u YWindow:
	/ \impl @ \mf bool operator-=(IWidget*);
	/ \impl @ \mf bool operator-=(IControl*);
* + \inc \h <algorithm> @ \impl \u YDesktop;

r116:
* \impl \mf (OnActivated & OnDeactivated) @ \cl ShlSetting @ \impl \u Shells;

r117:
/ \impl @ \ctor ShlSetting::TFormB @ \impl \u Shells;

r118-r122:
/= test 18;

r123:
* \impl @ \mf int ShlSetting::OnDeactivated(const Message&) @ \impl \u Shells;

r124:
^ \conf release;
/= test 19;

r125:
/ @ \cl ShlLoad @ \u Shells:
	/ \m YLabel lblStatus => lblDetais @ \cl TFrmLoadDown;
	/ \a \m \exc \st @ \m (\st (TFrmLoadUp & TFrmLoadDown)) >> \cl ShlLoad;
	- private \m \st (TFrmLoadUp & TFrmLoadDown);
	- private \m HWND hWndUp, hWndDown;	
	+ \ctor ShlLoad();
	/ \impl @ \mf (OnActivated & OnDeactivated);

r126:
* \impl @ \mf (OnActivated & OnDeactivated) @ \cl ShlLoad @ \impl \u Shells;

r127:
/ \impl @ \mf void ShlDS::UpdateToScreen() @ \impl \u Shell_DS;

r128-r131:
/= test 20;

r132-r134:
^ \conf debug;
/ \impl \mf bool DrawContents() @ \cl YDesktop:
	+ \as;

r135:
/ @ \cl YDesktop:
	/ \impl \mf void operator+=(IControl*);
	/ \impl \mf bool operator-=(IControl*);

r136:
- \mf DOs::size_type RemoveAll() @ \cl YDesktop;
/ \tr \impl @ \dtor @ \cl YFrame;

r137-r138:
/ \impl \mf bool DrawContents() @ \cl YDesktop;
/ \impl @ \f void OnTouchMove_Dragging(IControl&, TouchEventArgs&)
	@ \impl \u YControl;

r139:
/ @ \cl ShlDS @ \u Shell_DS:
	+ \mf \vt int OnActivated(const Message&);
	/ 2 \as @ \ctor >> \mf OnActivated;
	/ \impl @ \mf OnDeactivated;
/ \tr \impl @ 3 \mf OnActivated @ \impl \u Shells;

r140:
/ \impl @ \mf Refresh @ \cl (AWindow & YDesktop);

r141:
/ \impl @ \mf int ShlDS::OnActivated(const Message&) @ \impl \u Shell_DS;

r142:
+ bool SetContainerBgRedrawedOf(IWidget&, bool) @ \u YUIContainer;
* \impl @ \f (Show & Hide) @ \impl \u YWindow ^ \f SetContainerBgRedrawedOf;

r143:
* \impl @ \cl ShlReader @ \impl \cl Shells:
	/ \impl @ \mf OnActivated;
	/ \impl @ \mf UpdateToScreen;

r144:
/= test 21 ^ \conf release;

r145:
/ \impl @ \mf AWindow::Refresh;
- \mf YDesktop::Refresh;

r146:
/ \mf void ClearDesktopObjects() @ \cl YDesktop -> void ClearContents();
- \dtor @ \cl (YWidget & YUIContainer);

r147:
/ \simp 3 \mf OnDeactivated @ \cl @ \impl \u Shells;

r148:
/ @ \cl ShlExplorer @ \u Shells:
	- \decl @ \mf void LoadNextWindows();
	/ \a \m \exc \st @ \m (\st (TFrmFileListMonitor
		& TFrmFileListSelecter)) >> \cl ShlLoad;
	- private \m \st (TFrmFileListMonitor & TFrmFileListSelecter);
	- private \m HWND hWndUp, hWndDown;	
	+ \ctor ShlExplorer();
	/ \impl @ \mf (OnActivated & OnDeactivated);
	/ \simp \impl @ \mf fb_ViewChanged;

r149:
* \impl @ \mf bool YDesktop::DrawContents();

r150:
* \ctor & \mf OnActivated @ \cl ShlExplorer @ \u Shells;

r151-r152:
/= test 22;

r153:
* @ \impl \u YDesktop:
	* \impl @ \mf void operator+=(IControl*);
	* \impl @ \mf bool operator-=(IControl*);
* \i \mf operator-=(_type&) @ \clt GMFocusResponser<_type>
	-> !\i \mf;

r154:
/= test  ^ \conf release;


$DOING:

$relative_process:
2011-04-05:
-24.6d;

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
/ improve efficiency @ \ft polymorphic_crosscast @ \h YCast;
+ correct DMA (copy & fill);
* platform-independence @ alpha blending:
	+ \impl general Blit algorithm;
/ user-defined bitmap buffer @ \cl YDesktop;
+ (compressing & decompressing) @ gfx copying;


$LOW_PRIOR_TODO:
r577-r864:
+ \impl styles @ widgets;
+ general component operations:
	+ serialization;
	+ designer;
+ database interface;


$NOTHING_TODO:
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
$modify_features /;
$remove_features -;
$using ^;

$transform $list ($list_member $pattern $all($exclude $pattern
	$direct_str "*")) +;

$now
(
* "font caching without default font file load successfully",
"showing and hiding windows",
* "%ListBox scroll bar length",
* "handle constructors",
/ "shells for DS" $=
	(
		- "class %ShlGUI in unit Shell_DS"
	),
/ "using pointers instead of references in parameters of container methods",
/ "simplified GUI shell" $=
	(
		/ "input points matching",
		- "windows list"
	),
/ "simplified destructors",
/ "simplified window drawing",
+ "desktop capability of non-control widget container",
- "contianer pointer parameter from constructor of widgets",
/ "desktops as window in shells"
),

b197
(
* "label alignment",
"%std::string based font cache"
),

b196
(
"controls: checkbox",
* "platform color type",
"horizontal text alignment in class %MLabel"
),

b195
(
* "makefiles",
"dependency events",
"simple events routing"
),

b170_b194
(
"controls: track",
"controls: scroll bar",
"controls: scrollable container",
"controls: listbox"
),

b159_b169
(
"controls: buttons": class ("%YThumb", "%YButton"),
"controls: listbox class",
"events",
),

b132_b158
(
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
),

b1_b131
(
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

