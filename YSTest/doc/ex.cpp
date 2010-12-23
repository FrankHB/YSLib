//v 0.2910; *Build 175 r229;
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
\cp ::= copied
\ct ::= class templates
\ctor ::= constructors
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
\mo ::= member objects
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
/ @@ \h Base:
	- #define __interface struct;
	/ #define _yInterface __interface
		-> #define _yInterface struct;
* \param @@ \f \t (4 YReset & 1 YDelete_Debug) @@ \h YReference;
* \impl @@ \dtor @@ \cl YApplication @@ \u YApplication:
	* mutable use of key @@ set;
* \impl @@ \mf LRES YShell::DefShlProc(const Message&) @@ \u YShell:
	* mutable use of rvalue;

r2:
/ @@ \h YReference:
	/ \f \t<typename _type> \i bool YReset(GHStrong<_type>&) ythrow()
		-> \f \t<typename _type> bool YReset(GHStrong<_type>&) ythrow();
	/ \a AllowReset -> CantResetWithStrong;
	- \mf (Release & Reset) @@ \cl \t GHStrong;

r3-r4:
/= test 1;

r5:
/ \simp \impl @@ \f \t @@ \h YReference;
- \a \mac YSL_USE_SIMPLE_HANDLE ... @@ \h YShellDefinition & \h YReference;

r6:
/ \i \smf YApplication::GetInstancePtr(YScreen*&, YDesktop*&)
	-> !\i \smf YApplication::GetInstancePtr(YScreen*&, YDesktop*&);
	/ ^ (\s -> !\s) singleton;

r7:
/ inh YShellMain -> YShell @@ \cl ShlCLI @@ \u ShlDS;
/ @@ \h YShellDefinition:
	- DeclareHandle(YShell, HSHL);
	- DeclareHandle(YApplication, HINSTANCE);
	/= \tr @@ DeclareHandle(type, handle) & HWND;
/ \a HSHL -> GHHandle<YShell>;

r8:
/ \a YShellMain => YMainShell;
/ @@ \h YShellDefinition:
	+ \pre \decl \cl YGUIShell;
	+ using \cl YGUIShell @@ \ns YSLib;
/ @@ \u YShell:
	+ \cl YGUIShell;
	+ typedef YObject ParentType @@ \cl YShell;
/ inh YMainShell -> YGUIShell @@ \cl ShlGUI @@ \u ShlDS;

r9:
/= test 2;

r10:
* @@ \u YShell:
	/ \ctor @@ YShell() -> YShell(YApplication& = theApp) @@ \cl YShell;
	/ \impl @@ \ctor @@ \cl YMainShell;

r11:
/ @@ \dir Shell:
	/ \a GHHandle<YShell> -> GHHandle<YGUIShell>;
	/ \a \de \param ::YSLib::theApp.GetShellHandle() -> NULL;
	/ \impl @@ \ctor @@ \cl YFrameWindow @@ \u YWindow;
/ @@ \u Shells:
	/ \a GHHandle<YShell> -> GHHandle<YGUIShell>;
/ @@ \u YShellHelper:
	/ \f \t HWND NewWindow(GHHandle<YShell>)
		-> \f \t HWND NewWindow(GHHandle<YGUIShell>);
/ \impl @@ \ctor @@ \cl YDesktop @@ \u YDesktop;
/ @@ \u YReference:
	+ \ft static_handle_cast;
	+ \ft dynamic_handle_cast;

r12-r14:
/= test 3;

r15:
* \dtor @@ \cl YFrameWindow @@ \u YWindow;

r16:
- \a \in \t GIContainer;
- \a \in \t GILess;
- \a \in \t GIEquatable;

r17:
/ @@ \cl \t GHHandle:
	* \ctor \t<class C> \exp GHHandle(GHHandle<C>&)
		->\ctor \t<class C> \exp GHHandle(const GHHandle<C>&);
/ @@ \cl YApplication @@ \u YApplication:
	/ \mf \vt: void operator+=(YShell&) -> void operator+=(GHHandle<YShell>);
	/ \mf \vt: void operator-=(YShell&) -> void operator-=(GHHandle<YShell>);
/ \impl @@ \ctor @@ YShell @@ \u YShell;
/ @@ \u YShellHelper:
	/ \impl @@ \i \f GHHandle<YShell> NowShellAdd(GHHandle<YShell>);
	/ \impl @@ \i \f GHHandle<YShell> NowShellAdd(YShell&);
	/ \simp \impl @@ \f \t<class _type> HWND NewWindow(GHHandle<YGUIShell>);

r18:
/ @@ \dir Shells:
	/ @@ \u YWindow:
		/ \param order @@ \ctor @@ \cl AWindow;
		/ \param order @@ \ctor @@ \cl YFrameWindow;
	/ @@ \u YForm:
		/ \param order @@ \ctor @@ \cl YForm;
	/ \tr \impl @@ \ctor @@ \cl YDesktop @@ \u YDesktop;
/ 7 \tr \impl @@ form \ctor @@ \u Shells;

r19:
/ @@ \u YShell:
	/ @@ \cl YShell:
		/ 10 \mf >> \cl YGUIShell;
		/ \m WNDs >> \cl YGUIShell;
		/ \m typedef list<HWND> WNDs >> \cl YGUIShell;
		/ \impl @@ \ctor;
	/ @@ \cl YGUIShell:
		/ \impl @@ \ctor;
/ @@ \u YShellHelper:
	+ !\i \f GHHandle<YGUIShell> NowGUIShell();
/ @@ \u ShellDS:
	\i \f void ShlClearBothScreen(GHHandle<YShell> = NowShell())
		-> !\i \f void ShlClearBothScreen(GHHandle<YGUIShell> = NowGUIShell());

r20-r22:
/= test 4;

r23:
/ @@ \u YComponent:
	/ \cl YGUIShell @@ \ns Shells @@ \u YShell >> \ns Shells ;
	/ \inc "ycomp.h" -> "ydesktop.h";
	+ using namespace Components::Widgets @@ \ns Shells;
/ @@ \u YShell:
	/ \inc \h Shell::YWindow;
	/ \inc \h Shell::YControl;

r24:
/ @@ \h YShellDefinition:
	- \pre \decl @@ \cl YGUIShell;
	/ {
		YSL_BEGIN_NAMESPACE(Components)

		YSL_BEGIN_NAMESPACE(Controls)
		PDeclInterface(IControl)
		PDeclInterface(IVisualControl)
		YSL_END_NAMESPACE(Controls)

		YSL_BEGIN_NAMESPACE(Forms)
		PDeclInterface(IWindow)
		class YFrameWindow;
		class YForm;
		YSL_END_NAMESPACE(Forms)

		YSL_BEGIN_NAMESPACE(Widgets)
		PDeclInterface(IWidget)
		PDeclInterface(IUIBox)
		PDeclInterface(IUIContainer)
		YSL_END_NAMESPACE(Widgets)

		YSL_END_NAMESPACE(Components)

		using Components::Controls::IControl;
		using Components::Controls::IVisualControl;
		using Components::Forms::IWindow;
		using Components::Forms::YForm;
		using Components::Forms::YFrameWindow;
		using Components::Widgets::IWidget;
		using Components::Widgets::IUIBox;
		using Components::Widgets::IUIContainer;
		using Shells::YGUIShell;
		DeclareHandle(IWindow, HWND)
	} >> \h YComponent;
	/ typedef enum {RDeg0 = 0, RDeg90 = 1, RDeg180 = 2, RDeg270 = 3} ROT
		>> \h YGDI;

r25-r31:
/= test 5;

r32:
/ \a GStaticCache<_tShl> -> GStaticCache<_tShl, GHHandle<YShell> >
	@@ \h YShellHelper;

r33-r36:
/= test 6;

r37:
* \impl @@ \f void ReleaseShells() @@ \u Sbells;

r38:
+ \h "xwrapper.hpp" @@ \lib YCLib;
+ \inc "xwrapper.hpp" @@ \h YCommon;
+	{
		using stdex::reinterpret_auto_cast;
		using stdex::reinterpret_integral_cast;
	} @@ \ns YSLib @@ \h YReference;

r39:
/ \a handle_cast => handle2int;
+ \f \t int2handle @@ \h YReference;

r40:
/ @@ \u YShellMessage:
	/ a @@ \ns Shells >> \ns Messaging @@ \ns YSLib;
	/ extern const time_t DEF_TIMEOUT
		-> !extern const std::time_t DefTimeout;
	/ using Shells::Message -> using Messaging::Message;
	/ \a MSGPRIORITY => Priority;
	/ \a MSGID => ID;
	/ \m ID msg => id @@ \cl Message;
/ \decl @@ \h YShellDefinition;
/ \tr \impl @@ \u (ShellDS & Shells)

r41:
/ @@ \u YShellMessage:
	+ \in IContext;
	+ \cl OldContext;
	/ @@ \cl Message:
		- \m wParam;
		- \m lParam;
		- \m pt;
		- \a (\mf & \ctor) @@ (#ifdef & #ifndef) \mac YSLIB_NO_CURSOR;
		/ \impl \ctor;
		/ \mf DefGetter(WPARAM, WParam, wParam)
			-> !\i WPARAM GetWParam() const ythrow();
		/ \mf DefGetter(LPARAM, LParam, lParam)
			-> !\i LPARAM GetLParam() const ythrow();
		/ \mf DefGetter(const Point&, CursorLocation, pt)
			->  !\i const Point& GetCursorLocation() const ythrow();
		/ \impl @@ \mf bool operator==(const Message& m) const;
		- \mf void SetParam(WPARAM, LPARAM);
		+ \mf DefGetter(SmartPtr<IContext>, ContextPtr, pContext);
		+ private \mf \t<class _type> const _type* GetContextRawPtr()
			const ythrow();
+ \f \t<typename _type> \i _type* GetPointer(SmartPtr<_type>) @@ \h YReference;

r42:
/= test 7;

r43:
/ \simp @@ \cl Message @@ \u YShellMessage:
	- \mf DefGetter(std::clock_t, Timestamp, timestamp);
	- \mf DefSetter(std::clock_t, Timestamp, timestamp);
	/ \ac @@ \m timestamp -> public ~ private;
	/ \impl @@ \i \f void UpdateTimestamp();

r44-r45:
/ @@ \u YShellMessage:
	/ @@ \cl Message:
		/ \ctor Message(GHHandle<YShell> = NULL, ID = SM_NULL, Priority = 0,
				WPARAM = 0, const LPARAM = 0, const Point& pt = Point::Zero)
				-> \ctor Message(GHHandle<YShell> = NULL, ID = SM_NULL,
				Priority = 0, SmartPtr<IContext> = NULL);
		/ private \mf \t<class _type> const _type* GetContextRawPtr()
			const ythrow() -> public \mf \t<class _type>
			_type* GetContextRawPtr() const ythrow();
	/ \cl OldContext @@ \ns Messaging >> \ns Shells @@ \u YApplication;
/ @@ \u YShell:
	/ \tr \impl @@ \f void PostQuitMessage(int, Messaging::Priority);
/ @@ \u Main:
	/ \s \f void YSDebug_MSG_Print(const Message&)
		->!\s \f @@ \un ns;
	/ \impl @@ \g \ns \f int YMain(int, char*[]);
/ @@ \u YApplication:
	/ \impl @@ \mf bool YApplication::SetShellHandle(GHHandle<YShell>);
/ \impl @@ \f void WaitForGUIInput() \un \ns @@ \u YGlobal;

r46:
/ \cl Message @@ \u YShellMessage:
	/ \mf \t<class _type> _type* GetContextRawPtr() const ythrow()
		-> !\m \f \t<class _type> _type* FetchContextRawPtr(const Message&);
	/ \impl @@ \mf
		{
			WPARAM
			GetWParam() const ythrow();
			LPARAM
			GetLParam() const ythrow();
			const Point&
			GetCursorLocation() const ythrow();
		};
/ @@ \u YApplication:
	/ \i \f void InsertMessage(const GHHandle<YShell>&,
		const Messaging::ID&, const Messaging::Priority&,
		const WPARAM& = 0, const LPARAM& = 0, const Point& pt = Point::Zero)
		-> void InsertMessage(GHHandle<YShell>, Messaging::ID,
		Messaging::Priority, WPARAM = 0, LPARAM = 0,
		const Point& pt = Point::Zero);
/ + \as @@ \mf void ShlGUI::UpdateToScreen() @@ \u ShellDS;

r47:
/ \cl Message @@ \u YShellMessage: 
	- @@ \mf
		{
			WPARAM
			GetWParam() const ythrow();
			LPARAM
			GetLParam() const ythrow();
			const Point&
			GetCursorLocation() const ythrow();
		};
/ @@ \u YShell:
	/ \impl @@ \mf LRES YShell::DefShlProc(const Message&);
	/ \impl @@ \f void PostQuitMessage(int nExitCode, Messaging::Priority);
/ @@ \u ShelDS:
	/ \impl @@ \f void ResponseInput(const Message&);
/ \impl @@ \f void WaitForGUIInput() \un \ns @@ \u YGlobal;

r48-r50:
/ test 8;
	* \impl @@ \f void WaitForGUIInput() @@ \un \ns @@ \u YGlobal;

r51:
/ @@ \h YShellMessage:
	/ #include "ysmsgdef.h" >> \h YShell;
	/ \tr \param.de @@ \ctor @@ \cl Message;

r52:
/ @@ \h YShellMessageDefinition:
	+ \h YShellMessage;
	+ \ns Messaging
		+ \mac \def DefMessageTypeMapping(_name, _tContext);
		+ \st EStandard;
		+ \st \t MessageTypeMapping;
		/ \mac 'SM_*';
		+ \cl \t GObjectContext;
		+ \cl \t GHandleContext;
		+ \i \ft CastMessage;
		- \mac SM_SCRREFRESH & ...;
		+ \pre \decl @@ \cl InputContext;
		+ \pre \decl @@ \cl HWND;
		+ \mac
			{
				DefMessageTypeMapping(EStandard::Null, IContext)
				DefMessageTypeMapping(EStandard::Create,
					GHandleContext<GHHandle<YShell> >)
				DefMessageTypeMapping(EStandard::Destroy,
					GHandleContext<GHHandle<YShell> >)
				DefMessageTypeMapping(EStandard::Set,
					GHandleContext<GHHandle<YShell> >)
				DefMessageTypeMapping(EStandard::Drop,
					GHandleContext<GHHandle<YShell> >)
				DefMessageTypeMapping(EStandard::Activated,
					GHandleContext<GHHandle<YShell> >)
				DefMessageTypeMapping(EStandard::Deactivated,
					GHandleContext<GHHandle<YShell> >)
				DefMessageTypeMapping(EStandard::Paint,
					GHandleContext<GHHandle<YScreen*> >)
				DefMessageTypeMapping(EStandard::Quit, IContext)
				DefMessageTypeMapping(EStandard::Input, InputContext)
			};
- \inc \h YShellMessage @@ \h YShell;
/ \tr \impl @@ \mf LRES YShell::DefShlProc(const Message&) @@ \u YShell;

r53:
/ @@ \h YShellDefinition:
	- {
		\mac SM_WNDCREATE,
		\mac SM_WNDDESTROY,
		\mac SM_WNDDROP,
		DefMessageTypeMapping(EStandard::WindowCreate, GHandleContext<HWND>),
		DefMessageTypeMapping(EStandard::WindowDestroy, GHandleContext<HWND>),
		DefMessageTypeMapping(EStandard::WindowDrop, GHandleContext<HWND>),
		Messaging::EStandard::WindowCreate,
		Messaging::EStandard::WindowDestroy,
		Messaging::EStandard::WindowDrop,
		\pre \decl \cl HWND
	};
/ \tr \impl @@ (\ctor & \dtor) @@ \cl YFrameWindow @@ \u YWindow;
/ \tr \impl @@ \mf LRES YShell::DefShlProc(const Message&) @@ \u YShell;
/ \a _PLATFORM_H_ => INCLUDED_PLATFORM_H_;
/ \a _DSREADER_H_ => INCLUDED_DSREADER_H_;
/ \a YS_YSBUILD_H => YSL_YSBUILD_H;
/ \a YC_YCOMMON_H => YCL_YCOMMON_H;
/ \a YSLIB_DEBUG_MSG => YSL_DEBUG_MSG;
/ @@ \h YDefinition:
	- \mac
	{
		_nullterminated,
		LOBYTE,
		HIBYTE,
		LOWORD,
		HIWORD,
		MAKEWORD,
		MAKELONG
	};

r54:
/ \impl @@ \mf LRES YShell::DefShlProc(const Message&) @@ \u YShell;

r55:
/ @@ \h YMessageDefinition:
	/ DefMessageTypeMapping(EStandard::Quit, IContext)
		-> DefMessageTypeMapping(EStandard::Quit, GObjectContext<int>);
	+\f \t<ID id> typename MessageTypeMapping<id>::ContextType*
		CastMessage(const Message&);
	* \impl @@ \mf bool operator==(const IContext&) const
		@@ \cl \t (GObjectContext & GHandleContext);
/ \impl @@ \mf LRES YShell::DefShlProc(const Message&) @@ \u YShell;
/ \impl @@ \f void PostQuitMessage(int nExitCode, Messaging::Priority)
	@@ \u Shells;

r56:
/ @@ \h YApplication:
	+ \i \f void SendMessage(GHHandle<YShell>, Messaging::ID,
		Messaging::Priority, Messaging::IContext* = NULL);
	/ \a \f InsertMessage => SendMessage;
/ @@ \cl YMessageQueue \u YShellMessage:
	/ \mf InsertMessage => Insert;

r57:
/ \impl @@ \mf void ShlGUI::SendDrawingMessage() @@ \u ShellDS;
/ \impl @@ \mf LRES YShell::DefShlProc(const Message&) @@ \u YShell;
/ @@ \h YShellMessageDefinition:
	* DefMessageTypeMapping(EStandard::Paint,
		GHandleContext<GHHandle<YScreen*> >) ->
		DefMessageTypeMapping(EStandard::Paint, GHandleContext<YDesktop*>);
	* \impl @@ \mf bool operator==(const IContext&) const
		@@ \cl \t GHandleContext;

r58:
/ @@ \u YShell:
	/ \impl @@ (\ctor & \dtor) @@ \cl YShell;
	/ \impl @@ \mf LRES YShell::DefShlProc(const Message&);
	/ \impl @@ \f void PostQuitMessage(int nExitCode, Messaging::Priority);
/ @@ \h YShellHelper:
	/ \impl @@ \i \f void SetShellTo(GHHandle<YShell>,
		Messaging::Priority = 0x80);
/ @@ \h ShellDS:
	/ \impl @@ \i \f void NowShellInsertDropMessage(Messaging::Priority = 0x80);

r59:
/ \impl @@ \mf bool YApplication::SetShellHandle(GHHandle<YShell>)
	@@ \u YApplication;

r60:
/ @@ \u YGlobal:
	+ \cl InputContext @@ \ns Messaging;
	/ @@ \un \ns:
		+ \f bool operator==(const KeysInfo&, const KeysInfo&);
		/ \impl @@ \f bool operator!=(const KeysInfo&, const KeysInfo&);

r61:
/ \impl @@ \f void WaitForGUIInput() \un \ns @@ \u YGlobal;
/ \impl @@ \f void ResponseInput(const Message&) @@ \u ShellDS;

r62:
/ @@ \u YApplication:
	- \cl OldContext;
	- \i \f void SendMessage(GHHandle<YShell>, Messaging::ID,
		Messaging::Priority, WPARAM = 0, LPARAM = 0,
		const Point& = Point::Zero);
/ @@ \h YDefinition:
	- typedef UINT_PTR WPARAM;
	- typedef LONG_PTR LPARAM;

r63:
/ \s \i \f \t<typename _tChar>usize_t StrToANSI(char*, const _tChar*, char= ' ')
	-> !\s !\i \f \t @@ \un \ns @@ \u CHRProcessing;

r64:
/ @@ \u YCommon:
	- \i @@ \f void yassert(bool, const char*, const char*, int, const char*);
	- \i @@ \mf (IsDirectory & Open & Close & Reset);
/ @@ \u YFont:
	* \tr \as const FontFamily& GetDefaultFontFamily() ythrow();
	/ @@ \cl YFont:
		- \i @@ \mf SetFont;
	- \i @@ \f void CreateFontCache(YFontCache*&, CPATH);
/ @@ \cl YTimer @@ \u YTimer:
	- \i @@ \mf SetInterval;
/ @@ \u YTextManager:
	/= \tr \impl @@ \i \mf void TextBuffer::ClearText();
	/ @@ \cl TextFileBuffer:
		- \i @@ \mf (begin & end);
/ @@ \h ShellDS:
	/ @@ \cl ShlCLI:
		/= \dtor -> \vt DefEmptyDtor(ShlCLI);

r65:
* + \mac #ifdef @@ \def @@ \f yassert @@ \u YCommon;
- \i @@ \f Components::Widgets::yassert @@ \u YWidget;
/ \a YC_USE_YASSERT => YCL_USE_YASSERT;
+ {
	#ifndef NDEBUG
	#	define YCL_USE_YASSERT
	#endif
} @@ \h YDefinition;

r66:
/ @@ \u YApplication:
	/ (-\i & + ythrow()) @@ 2 \i \f SendMessage;
	/ \impl @@ \mf FatalError @@ \cl YLog;
+ \f void ShowFatalError(const char*) @@ \u YGlobal;

r67:
/ DefMessageTypeMapping(EStandard::Destroy, GHandleContext<GHHandle<YShell> >)
	-> DefMessageTypeMapping(EStandard::Destroy, IContext)
	@@ \h YShellMessageDefinition;
/ @@ \u YShell:
	/ @@ \cl YShell:
		/ \impl @@ \ctor;
		* \impl @@ \dtor;
	/ \impl @@ \mf LRES YShell::DefShlProc(const Message&);

r68-r69:
/= test 9;
/ @@ \h YReference:
	- \mac YHandleOP;
	/ \param 2 @@ \cl \t GHHandle -> Design::Policies::SmartPtr_Simple;
/a YReset => YReset;

r70;
/ @@ \h YReference:
	+ \mf (T* Release() & bool Reset(T* = NULL)) @@ \cl \t GHStrong;
	/ ((\simp \impl ^ GHStorng::Reset) & +\i)
		@@ \f \t<_type> YReset(GHStrong<_type>&);
	+ \mf (T* Release() & bool Reset(T* = NULL)) @@ \cl \t GHHandle;
	/ ((\simp \impl ^ GHHandle::Reset) & +\i)
		@@ \f \t<_type> YReset(GHHandle<_type>&);

r71-r109:
/= test 10;

r110:
/ @@ \u YWindow:
	/ @@ \cl MWindow:
		- \m GHHandle<YGUIShell> hShell;
		/ \ctor;
		- \mf DefGetter(GHHandle<YGUIShell>, ShellHandle, hShell);
	/ @@ \cl AWindow:
		/ \ctor;
		- \mf BelongsTo;
		- \mf DefGetterBase(GHHandle<YGUIShell>, ShellHandle, MWindow);
	/ @@ \cl YFrameWindow:
		/ \ctor;
/ \ctor @@ \cl YForm @@ \u YForm;
/ \impl @@ \ctor @@ \cl YDesktop @@ \u YDesktop;
/ \f \t<class _type> HWND NewWindow(GHHandle<YGUIShell>)
	-> \f \t<class<_type> > NewWindow(YGUIShell&) @@ \h YShellHelper;
/ @@ \u Shells:
	/ 7 \ctor @@ forms @@ \h;
	/ \impl @@ 3 \mf OnActivated;
	/ \impl @@ 3 \mf ^ HandleCast(NowShell) ~ HandleCast(hShell);

r111:
* \impl @@ \mf ShlSetting::OnDeactivated(const Message&) @@ \u Shells;
* \impl @@ \mf LRES ShlDS::OnDeactivated(const Message&) @@ \u ShellDS;

r112:
/ @@ \h YReference:
	* \i \ft GHHandle<_type> static_handle_cast(GHHandle<_tReference>)
		-> !\i \ft GHHandle<_type> static_handle_cast(GHHandle<_tReference>);
	* \i \ft GHHandle<_type> dynamic_handle_cast(GHHandle<_tReference>)
		-> !\i \ft GHHandle<_type> dynamic_handle_cast(GHHandle<_tReference>);
	/ @@ \st \t SmartPtr_Simple:
		+ \i @@ \a \m;
		+ \s @@ \i \mf void Swap(SmartPtr_Simple&);
		+ \i \smf void Merge(SmartPtr_Simple&);
	+ \cl \t MergableRefCounted;
/ \a SmartPtr_Simple => RawOwnership;

r113-r114:
/= test 11;
/ @@ \h YReference:
	/ @@ \cl \t MergableRefCounted:
		/ \ac @@ \m uintptr_t* pCount_ -> public ~ private;

r115-r127:
/= test 12;

r128:
/ @@ \h YShell:
	/= \i \vt \dtor ythrow() @@ \cl YMainShell -> \vt DefEmptyDtor(YMainShell);
	/ \decl extern LRES MainShlProc(const Message&) -> \u ~ \h;
	- \i @@ \mf YMainShell::ShlProc(const Message&);
/ @@ \u YDefinition:
	- {
		#undef FALSE
		#define FALSE 0

		#undef TRUE
		#define TRUE 1

		typedef intptr_t INT_PTR;
		typedef uintptr_t UINT_PTR;

		#ifndef BASETYPES
		#define BASETYPES
		typedef unsigned char UCHAR, *PUCHAR;
		typedef unsigned short USHORT, *PUSHORT;
		typedef unsigned long ULONG, *PULONG;
		typedef char* PSZ;
		#endif

		typedef int             BOOL;
		typedef unsigned char   BYTE;
		typedef unsigned short  WORD;
		typedef unsigned long   DWORD;

		typedef INT_PTR			IRES;
		typedef UINT_PTR		URES;
		typedef LONG_PTR		LRES;

		typedef char* STR;
		typedef const char* CSTR;
	};
	/ {
		typedef int INT_PTR;
		typedef unsigned int UINT_PTR;
		typedef long LONG_PTR;
		typedef unsigned long ULONG_PTR;
	}
	-> {
		typedef int intptr_t;
		typedef unsigned int uintptr_t;
	};
	/ {
		typedef STR PATH;
		typedef CSTR CPATH;
	}
	-> {
		typedef char* PATH;
		typedef const char* CPATH;
	};
	/ {
		typedef void FVOID(void);
		typedef FVOID *PFVOID;
	} >> \h DSReader;
	/ typedef IRES			ERRNO
	-> {
		namespace stdex
		{
			typedef int errno_t;
		}
	}
/ \a IRES & LRES -> int;
/ \a ERRNO -> stdex::errno_t;
/ \mf DefGetter(count_t, ID, nID) -> DefGetter(count_t, ObjectID, nID)
	@@ \cl \t GMCounter @@ \h YCounter;
/ \a GetID => GetObjectID;
/ \mf DefGetter(ID, MsgID, id) -> DefGetter(ID, MessageID, id)
	@@ \cl Message @@ \h YMessage;
/ \a GetMsgID => GetMessageID;
/ \a CSTR -> const char*;
/ \a _ARM9_MAIN_CPP_ YSL_ARM9_MAIN_CPP_;
/ \a _MAIN_CPP_ YSL_MAIN_CPP_;

r129-r143:
/= test 13;

r144:
/ @@ \ns Shells @@ \u YShell:
	/ \mf bool YShell::Activate() -> !\m \f bool Acticate(GHHandle<YShell>);
/ \impl @@ \i \f stdex::errno_t NowShellTo(GHHandle<YShell>) @@ \h YShellHelper;

r145-r150:
/= test 14;

r151:
/ \f \t<class _type> HWND NewWindow(YGUIShell&) -> \f \t<class _type>
	HWND NewWindow() @@ \h YShellHelper;
/ 7 NewWindow call @@ \impl @@ \u Shells;

r152-r154:
/= test 15;

r155:
/ @@ \h YShellHelper:
	- 2 \i \f NowShellAdd;
/ \a NowGUIShell => FetchGUIShellHandle;
/ \a NowShell => FetchShellHandle;

r156:
/ @@ \cl YGUIShell @@ \u YComponent:
	/ \mf \vt void operator+=(IWindow&) -> void operator+=(HWND);
	/ \mf \vt bool operator-=(IWindow&) -> void operator-=(HWND);
	/ \mf WNDs::size_type RemoveAll(IWindow&)
		-> WNDs::size_type RemoveAll(HWND);
/ \impl @@ \mf ShlSetting::OnDeactivated(const Message&) @@ \u Shells;
/ \impl @@ \ft<class _type> HWND NewWindow() @@ \h YShellHelper;
/ \impl @@ \mf int ShlDS::OnDeactivated(const Message&) @@ \u ShellDS;
/ \impl @@ \mf void YApplication::operator+=(GHHandle<YShell>)
	@@ \cl YApplication @@ \u YApplication;

r157:
/= test 16;

r158:
/ \impl @@ int MainShlProc(const Message&) @@ \u YShell;

r159:
/ @@ \cl YShell @@ \u YShell:
	/ \impl @@ \mf int DefShlProc(const Message&);
	/ \impl @@ (\ctor & \dtor);

r160:
/ @@ \h YShellMessageDefinition:
	- {
		#define SM_CREATE				Messaging::EStandard::Create
		#define SM_DESTROY				Messaging::EStandard::Destroy
		DefMessageTypeMapping(EStandard::Create,
			GHandleContext<GHHandle<YShell> >)
		DefMessageTypeMapping(EStandard::Destroy, IContext)
	};
	- {
		Create = 0x0001,
		Destroy = 0x0002,
	} @@ \en MessageSpace @@ \st EStandard;

r161-r164:
/= test 17;

r165:
* \impl @@ \mf bool YApplication::SetShellHandle(GHHandle<YShell>)
	@@ \u YApplication;

r166-r173:
/= test 18;

r174:
/ @@ \u YWidget:
	/ \ft<class _tWidget> HWND FetchWindowHandle(const _tWidget&)
		-> \ft<class _tWidget> IWindow* FetchWindowPtr(const _tWidget&);
	/ \i \f HWND FetchWindowHandle(const IWidget&)
		-> \f IWindow* FetchWindowPtr(const IWidget&)
	/ \ft<class _tWidget> HWND FetchWidgetDirectWindowHandle(const _tWidget*)
		-> \ft<class _tWidget> IWindow*
		FetchWidgetDirectWindowPtr(const _tWidget*);
	/ \i \f HWND FetchDirectWindowHandle(const IWidget&)
		-> IWindow* FetchDirectWindowPtr(const IWidget&);
	/ \ft<class _tWidget> HWND etchDirectWindowPtr(const _tWidget&)
		-> \ft<class _tWidget> IWindow* FetchDirectWindowPtr(const _tWidget&);
	/ \impl @@ \f Point LocateForParentWindow(const IWidget&);
	/ \impl @@ \f Point LocateForWindow(IWidget&);
	/ \impl @@ \f Point LocateForParentWindow(const IWidget&);
	/ \impl @@ \ft<class _tWidget> void Fill(_tWidget&, Color) @@ \un \ns;
	/ \impl @@ \mf void Widget::Refresh();
	/ \impl @@ \mf void MLabel::PaintText(Widget&, const Point&);
/ @@ \u YGUI:
	/ \impl @@ \f void DrawWidgetOutline(IWidget&, Color);
	/ \f void DrawWindowBounds(HWND, Color)
		-> void DrawWindowBounds(IWindow*, Color);
	/ \f void DrawWidgetBounds(HWND, const Point&, const Size&, Color)
		-> void DrawWidgetBounds(IWindow&, const Point&, const Size&, Color)
		@@ \un \ns;
	/ \impl @@ void DrawWidgetBounds(IWidget&, Color);
/ @@ \u YGUIComponent:
	/ \f void WndDrawFocus(HWND, const Size&) @@ \un \ns
		-> void WndDrawFocus(IWindow*, const Size&);
	/ \impl @@ \mf void YThumb::DrawForeground();
	/ \impl @@ \mf void YSimpleTextListBox::DrawForeground();

r175-r213:
/= test 18;

r214:
/ \a pDesktopUp => hDesktopUp;
/ \a pDesktopDown => hDesktopDown;
/ \a pScreenUp => hScreenUp;
/ \a pScreenDown => hScreenDown;
/ @@ \u YGlobal:
	/ {
		extern YScreen* hScreenUp;
		extern YScreen* hScreenDown;
		extern YDesktop* hDesktopUp;
		extern YDesktop* hDesktopDown;
	}
	->
	{
		extern GHHandle<YScreen> hScreenUp;
		extern GHHandle<YScreen> hScreenDown;
		extern GHHandle<YDesktop> hDesktopUp;
		extern GHHandle<YDesktop> hDesktopDown;
	}
	/ \impl @@ \f void YDestroy() @@ \un \ns;
/ @@ \cl MWindow @@ \u YWindow:
	/ \m YDesktop* pDesktop -> GHHandle<YDesktop> hDesktop;
	/ \exp \ctor MDesktopObject(YDesktop*)
		-> \exp \ctor MDesktopObject(GHHandle<YDesktop>);
	/ \mf DefGetter(YDesktop*, DesktopPtr, pDesktop)
		->  DefGetter(GHHandle<YDesktop>, DesktopHandle, hDesktop)
	/ \mf bool BelongsTo(YDesktop*) const
		->bool BelongsTo(GHHandle<YDesktop>) const;
/ @@ \h YReference:
	* \tf<typename _type, typename _tReference> GHHandle<_type>
		static_handle_cast(GHHandle<_tReference>)
		-> \i \tf<typename _type, typename _tReference> GHHandle<_type>
		static_handle_cast(const GHHandle<_tReference>&);
	/ \tf<typename _type, typename _tReference> GHHandle<_type>
		dynamic_handle_cast(GHHandle<_tReference>)
		-> \tf<typename _type, typename _tReference> GHHandle<_type>
		dynamic_handle_cast(GHHandle<_tReference>&);
	/ (- \exp & + \i & \simp \impl) @@ \ctor \t<class C>
		GHHandle(const GHHandle<C>&);
/ \tr \impl @@ 7 form \ctor @@ \h Shells;
/ \inh \h YWindow -> \h YDesktop @@ \u (YShellMessage & YFocus);
+ \inh \h YDesktop @@ \u YFont;
/ @@ \h YShellMessageDefinition:
	/ DefMessageTypeMapping(EStandard::Paint, GHandleContext<YDesktop*>)
		-> DefMessageTypeMapping(EStandard::Paint,
		GHandleContext<GHHandle<YDesktop> >);
/ \impl @@ \mf void ShlGUI::SendDrawingMessage() @@ \u ShellDS;
/ \impl @@ \mf int YShell::DefShlProc(const Message&) @@ \u YShell;
/ @@ \cl YApplication:
	/ \smf YApplication& GetApp(YScreen*&, YDesktop*&)
		-> YApplication& GetApp(GHHandle<YScreen>&, GHHandle<YDesktop>&)
	/ \smf YApplication* GetInstancePtr(YScreen*&, YDesktop*&)
		-> YApplication* GetInstancePtr(GHHandle<YScreen>&,
		GHHandle<YDesktop>&);
	/ \ctor YApplication(YScreen*&, YDesktop*&)
		-> YApplication(GHHandle<YScreen>&, GHHandle<YDesktop>&);
	/ \m YScreen*& pDefaultScreen -> GHHandle<YScreen>& hDefaultScreen;
	/ \m YDesktop*& pDefaultDesktop -> GHHandle<YDesktop>& hDefaultDesktop;

r215:
* 4 \impl @@ form \ctor @@ \h Shells;

r216:
* \tr \impl @@ \f void YInit() @@ \un \ns @@ \u YGlobal;

r217:
/ @@ \h XWrapper:
	+ 2 \ft class_cast;
	+ \ns _impl with 6 \cl \t _class_cast_helper;
	+ \inc <tr1\type_traits>;
/ @@ \h YReference:
	- \cl \t MergableRefCounted;
	+ using stdex::class_cast;
/ \a reinterpret_auto_cast => auto_integral_cast;
/ \a reinterpret_integral_cast => integral_auto_cast;

r218-r219:
	r218:
	/ \a class_cast => general_cast;
	/ @@ \h XWrapper:
		/ \a _class_cast_helper => _general_cast_helper;
		+ 2 \ft general_cast with \c \param;
	/ \h XWrapper => YCast;
	/ @@ \h YReference:
		+ \cl \t GeneralCastRefCounted @@ \Policies;
/= test 19;

r220:
/ @@ \h YCast:
	/ @@ \ns stdex:
		+ \ft polymorphic_cast;
		+ 2 \ft polymorphic_downcast;
		+ \st \t has_nonempty_virtual_base;
		+ \st \t has_common_nonempty_virtual_base;
		/ \st \t _general_cast_helper @@ \ns impl;
		- 1 @@ 4 \ft general_cast;
	+ \inc <typeinfo>
- {
	using stdex::auto_integral_cast;
	using stdex::integral_auto_cast;
	using stdex::general_cast;
} @@ \h YReference;

r221-r226:
/= test 20;

r227:
+ (\h & \u) YStandardExtend @@ \lib YCLib;
/ \a stdex => ystdex;
/ \inc <cstdlib> @@ \h YCommon => \h YStandardExtend;
/ @@ \h YStandardExtend:
	+ {
		using std::tr1::add_const;
		using std::tr1::add_cv;
		using std::tr1::add_pointer;
		using std::tr1::add_reference;
		using std::tr1::add_volatile;
		using std::tr1::aligned_storage;
		using std::tr1::alignment_of;
		using std::tr1::extent;
		using std::tr1::false_type;
		using std::tr1::has_nothrow_assign;
		using std::tr1::has_nothrow_constructor;
		using std::tr1::has_nothrow_copy;
		using std::tr1::has_trivial_assign;
		using std::tr1::has_trivial_constructor;
		using std::tr1::has_trivial_copy;
		using std::tr1::has_trivial_destructor;
		using std::tr1::has_virtual_destructor;
		using std::tr1::integral_constant;
		using std::tr1::is_abstract;
		using std::tr1::is_arithmetic;
		using std::tr1::is_array;
		using std::tr1::is_base_of;
		using std::tr1::is_class;
		using std::tr1::is_compound;
		using std::tr1::is_const;
		using std::tr1::is_convertible;
		using std::tr1::is_empty;
		using std::tr1::is_enum;
		using std::tr1::is_floating_point;
		using std::tr1::is_function;
		using std::tr1::is_fundamental;
		using std::tr1::is_integral;
		using std::tr1::is_member_function_pointer;
		using std::tr1::is_member_object_pointer;
		using std::tr1::is_member_pointer;
		using std::tr1::is_pod;
		using std::tr1::is_pointer;
		using std::tr1::is_polymorphic;
		using std::tr1::is_reference;
		using std::tr1::is_same;
		using std::tr1::is_scalar;
		using std::tr1::is_signed;
		using std::tr1::is_union;
		using std::tr1::is_unsigned;
		using std::tr1::is_void;
		using std::tr1::is_volatile;
		using std::tr1::rank;
		using std::tr1::remove_all_extents;
		using std::tr1::remove_const;
		using std::tr1::remove_cv;
		using std::tr1::remove_extent;
		using std::tr1::remove_pointer;
		using std::tr1::remove_reference;
		using std::tr1::remove_volatile;
		using std::tr1::true_type;
	}
	+ \inc <cstddef>;
	+ \inc "ydef.h";
/ @@ \h YCast:
	+ \inc \h YStandardExtend;
	/ \inc (<cassert> & <tr1/type_traits>) => \h YStandardExtend;
	/ - 'std::tr1::' @@ \impl;
- \inc <stack> @@ \u YCommon;
+ \inc (<cstring> & <cstdio>) @@ \u YStandardExtend;

r228:
/ @@ \cl YApplication @@ \u YApplication:
	- \m typedef set<GHHandle<YShell> > SHLs;
	- \m SHLs sShls;
	- \mf DefGetter(const SHLs, ShellSet, sShls);
	/ \simp \impl @@ \dtor;
	- \vt \mf void operator+=(GHHandle<YShell>);
	- \vt \mf void operator-=(GHHandle<YShell>);
	- \mf bool Contains(GHHandle<YShell>) const;
	/ \simp \impl @@ bool SetShellHandle(GHHandle<YShell>);
/ @@ \cl YShell @@ \u YShell:
	/ \ctor Shell(YApplication&) -> YShell();
	/ \simp \impl @@ \dtor;
	
r229:
/= test 21;

$DOING:


relative process:
2010-12-23:
-20.5d;

/ ...


$NEXT:

b177-b190:
* store more than one newed objects in smart pointers(see effc++ 3rd item17)
	@@ \ctor @@ \dir Shell;
/ fully \impl \u DSReader;
	* moving text after setting lnGap;
* non-ASCII character path error in FAT16;
+ \impl loading pictures;
+ \impl style on widgets;
/ - const_cast;

$NOTHING_TODO:
* fatal \err @@ b16x:
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
	-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 02055838
	ftc_snode_weight
	ftcsbits.c:271

$TODO:

Clarify the log levels.

Make "UpdateFont()" more efficienct.

More efficient @YTextRegion output:
Use in-buffer background color rendering and function @CopyToScreen()
	to inplements @YTextRegion background;
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

