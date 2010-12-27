//v 0.2935; *Build 176 r194;
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
r1-r36:
/= test 1;

r37:
/ \impl @@ \ctor @@ \cl YFrameWindow @@ \u YWindow;

r38-r75:
/= test 2;

r76-r79:
/ @@ \u YGDI:
	* \impl @@ (\ctor BitmapBuffer(ConstBitmapPtr, SDST, SDST))
		& (\mf void SetSize(SDST, SDST)) @@ \cl BitmapBuffer;
	/ \tr \rem;
	* \impl @@ \mf void SetSize(SDST, SDST) @@ \cl BitmapBufferEx;

r80:
+ \as @@ \impl @@ \mf void SetSize(SDST, SDST)
	@@ \cl (BitmapBuffer & BitmapBufferEx) @@ \u YGDI;


r81-r160:
/= test 3;

r161-r162:
/ \h YReference >> \dir Adaptor ~ Core;
/ @@ \h YReference:
	/ @@ \clt GHHandle:
		* \impl @@ \mf Reset;
		/ \impl @@ \mf Release;
	/ (\impl & + \i) @@ \ft<typename _type, typename _tReference>
		GHHandle<_type> dynamic_handle_cast(GHHandle<_tReference>);
	+ \i \mf \t<typename P1> P Clone(const P1&) @@ \clt RawOwnership<P>;
/ @@ \h YAdaptor:
	- {
		using Loki::Release;
		using Loki::ReleaseAll;
	};
/ \a RawOwnership => GeneralCastRawOwnership;

r163:
/= test 4;

r164:
/ @@ \u Shells:
	- \a 11 \i \mf & \ctor;
	/= \em \vt \dtor @@ \cl ShlReader -> \vt DefEmptyDtor(ShlReader);

r165-r174:
/= test 5;

r175:
* \impl @@ \mf void ShlSetting::ShowString(String&) @@ \u Shells;
* \decl @@ \stt has_nonempty_virtual_base & has_common_nonempty_virtual_base
	@@ \h YCast;
	+ 6 \es @@ \m \st;

r176:
/ @@ \h YReference:
	/ \inc Design::Policies::GeneralCastRawOwnership
		-> \clt Design::Policies::GeneralCastRefCounted @@ \clt GHHandle;
		* ynew memory leaks;
	- \clt GeneralCastRawOwnership;

r177
/ \a dynamic_handle_cast => general_handle_cast;
/ \a FS => _ustr;
/ @@ \cl MDualScreenReader @@ \h DSReader:
	/ \mf DefGetter(u8, LnGapUp, pTrUp->LineGap)
		-> DefGetter(u8, LineGapUp, pTrUp->LineGap);
	/ \mf DefGetter(u8, LnGapDn, pTrDn->LineGap)
		-> DefGetter(u8, LineGapDn, pTrDn->LineGap);
	/ \mf DefSetterDe(u8, LnGapUp, pTrUp->LineGap, 0)
		-> DefSetterDe(u8, LineGapUp, pTrUp->LineGap, 0);
	/ \mf DefSetterDe(u8, LnGapDn, pTrDn->LineGap, 0)
		-> DefSetterDe(u8, LineGapDn, pTrDn->LineGap, 0);
	/ \mf DefGetter(u8, LineGap, GetLnGapUp())
		-> DefGetter(u8, LineGap, GetLineGapUp());

r178:
/ @@ \cl MDualScreenReader @@ \u DSReader:
	/ \mf DefGetter(u16, ColorUp, pTrUp->Color)
		-> DefGetter(Color, ColorUp, pTrUp->Color);
	/ \mf DefGetter(u16, ColorDn, pTrDn->Color)
		-> DefGetter(Color, ColorDn, pTrDn->Color);
	/ \mf DefGetter(u16, Color, GetColorUp())
		-> DefGetter(Color, Color, GetColorUp());
	/ \a \tp u16 -> \tp SDST;
	- {
		#define SM_DSR_INIT				0x8001
		#define SM_DSR_REFRESH			0x8002
		#define SM_DSR_PRINTTEXT		0x8003
	};
	/ \rem \mf void Scroll(PFVOID pCheck);
	- {
		typedef void FVOID(void);
		typedef FVOID *PFVOID;
	};

r179:
/ ^ \mac ynew @@ \mf void MDualScreenReader::LoadText(YTextFile&)
	@@ \cl MDualScreenReader @@ \u DSReader;

r180-r181:
* \a INCLUDED_YOUTPUT_H_ => INCLUDED_YDEVICE_H_;
/ \cl Graphics @@ \h YGDI => \h YObject;
/ @@ \cl Graphics @@ \h YObject:
	+ protected \mf DefSetter(const Drawing::Size&, Size, Size);
	+ public \dtor DefEmptyDtor(Graphics);
	+ protected \mf DefSetter(BitmapPtr, BufferPtr, pBuffer);

r182:
/ @@ \u YDevice:
	/ @@ \cl YGraphicDevice:
		- \mf DefGetter(const Size&, Size, *this);
		- \vt \mf DefGetter(Drawing::BitmapPtr, Ptr, ptr);
			-> !\vt \mf DefGetter(Drawing::BitmapPtr, BufferPtr, ptr);
		- \vt \mf DefSetter(Drawing::BitmapPtr, Ptr, ptr);
			-> !\vt \mf DefSetter(Drawing::BitmapPtr, BufferPtr, ptr);
	/ @@ \cl YScreen:
		/ \vt \i \mf Drawing::BitmapPtr GetPtr()
			=> \vt !\i \mf Drawing::BitmapPtr GetCheckedBufferPtr();
		+ \as @@ \mf Drawing::BitmapPtr GetCheckedBufferPtr();
		/ \tr \impl @@ \mf void Update(BitmapPtr);
		/ \tr \impl @@ \mf void Update(Color);
/ \mf DefGetter(BitmapPtr, BackgroundPtr, Screen.GetPtr())
	@@ \cl YDesktop @@ \h YDesktop
	-> DefGetter(BitmapPtr, BackgroundPtr, Screen.GetCheckedBufferPtr());
/ \tr \impl @@ \f bool InitAllScreens() @@ \u YGlobal;

r183:
/ @@ \u YDevice:
	/ @@ \cl YGraphicDevice:
		/ protected \inh Drawing::Size -> public \inh Drawing::Graphics;
		- protected \m Drawing::BitmapPtr ptr;
		- \mf DefGetter(Drawing::BitmapPtr, BufferPtr, ptr);
		- \mf DefSetter(Drawing::BitmapPtr, BufferPtr, ptr);
		/ \impl @@ \i \ctor;
	/ \tr \impl @@ \i \mf Drawing::BitmapPtr GetPtr() const ythrow()
		@@ \cl YScreen;

r184:
/ @@ \u YGDI:
	/ \m img => pBuffer @@ \cl BitmapBuffer;
	/ \m imgAlpha => pBufferAlpha @@ \cl BitmapBufferEx;
	/ \a imgNew => pBufferNew;
	/ \a imgAlphaNew => pBufferAlphaNew;

r185-r187:
/ @@ \u YGDI:
	/ @@ \cl BitmapBuffer:
		- \mf \op Graphics;
		+ \mf DefGetter(SDST, Width, Size::Width);
		+ \mf DefGetter(SDST, Height, Size::Height);
	/ @@ \cl BitmapBufferEx:
		- friend \f operator==;
/ @@ \cl Graphics @@ \h YObject:
	/ \ctor Graphics(BitmapPtr, const Drawing::Size&);
		-> \exp \ctor Graphics(BitmapPtr = NULL,
		const Drawing::Size& = Drawing::Size::Zero);
	/ \ac @@ \a private \m -> protected;
	- protected \mf DefSetter(BitmapPtr, BufferPtr, pBuffer);
	- protected \mf DefSetter(const Drawing::Size&, Size, Size);
/ \tr \impl @@ \mf bool LineUp() & bool LineDown() @@ \cl MDualScreenReader
	@@ \u DSReader;
/ \impl @@ \f bool InitAllScreens() @@ \u YGlobal;

r188:
/ @@ \u YText:
	/ @@ \cl TextRegion:
		/ \mf DefGetter(SDST, BufWidthN, Width - GetHorizontalFrom(Margin))
			-> DefGetter(SDST, BufWidthN,
			GetWidth() - GetHorizontalFrom(Margin));
		/ \mf DefGetter(SDST, BufHeightN, Height - GetVerticalFrom(Margin))
			-> DefGetter(SDST, BufHeightN,
			GetHeight() - GetVerticalFrom(Margin));
		/ \tr \impl @@ \mf SDST GetMarginResized() const;
		/ \tr \impl @@ \mf SDST GetBufferHeightResized() const;
		/ \tr \impl @@ \mf SPOS GetLineLast() const;
		/ \tr \impl @@ \mf void ClearLine(u16, SDST);
		/ \tr \impl @@ \mf void ClearLnLast();
		/ \tr \impl @@ \mf void Move(s16);
		/ \tr \impl @@ \mf void Move(s16, SDST);
		/ \tr \impl @@ \mf void PutChar(fchar_t);

r189:
/ \impl @@ \mf void YSimpleTextListBox::DrawForeground() @@ \u YGUIComponent;
/ @@ \cl AWindow @@ \u YWindow:
	/ \impl @@ \mf void UpdateToScreen(YDesktop&) const;
	/ \impl @@ \mf void UpdateToWindow(IWindow&) const;
+ \mf DefGetter(const Size&, Size, *this) @@ \cl BitmapBuffer @@ \cl YGDI;
/ @@ \u YText:
	/ \impl @@ \f void PrintChar(BitmapBuffer&, TextState&, fchar_t);
	/ \impl @@ \f void PrintCharEx(BitmapBufferEx&, TextState&, fchar_t);
/ @@ \cl Graphics @@ \h YObject:
	/ \ctor Graphics(BitmapPtr, const Drawing::Size&);
		-> \exp \ctor Graphics(BitmapPtr = NULL,
		const Drawing::Size& = Drawing::Size::Zero);

r190:
/ @@ \u YGDI:
	/ @@ \cl BitmapBuffer:
		/ \a 'GetAreaFrom(*this)' -> 'GetAreaFrom(Size)';
		- \m pBuffer;
		/ \impl @@ \2 \ctor;
		/ \impl @@ \vt \mf void SetSize(SDST, SDST);
		/ \impl @@ \mf CopyToBuffer;
		/ \inh Size -> \inh Graphics;
		- \mf DefGetter(const Size&, Size, *this);
		- \mf DefGetter(SDST, Width, Size::Width);
		- \mf DefGetter(SDST, Height, Size::Height);
		- \mf DefConverter(Graphics, Graphics(pBuffer, *this));
		- \mf DefGetter(BitmapPtr, BufferPtr, pBuffer);
		/ \impl @@ \mf void SetSizeSwap();
	/ @@ \cl BitmapBufferEx:
		/ \a 'GetAreaFrom(*this)' -> 'GetAreaFrom(Size)';
		/ \impl @@ \mf CopyToBuffer;
		/ \impl @@ \mf BlitToBuffer;
		/ \impl @@ \vt \mf void SetSize(SDST, SDST);
/ \mf DefConverterMember(Graphics, Buffer) @@ \cl AWindow @@ \u YWindow
		-> \mf DefConverter(Graphics, Buffer);

r191:
/ @@ \u YWindow:
	/ @@ \in IWindow:
		/ \amf DeclIEntry(operator Graphics() const)
			-> DeclIEntry(operator const Graphics&() const);
		+ \amf DeclIEntry(operator Graphics&());
	/ @@ \cl AWindow:
		/ \vt \mf DefConverter(Graphics, Buffer)
			-> \mf DefConverter(const Graphics&, Buffer);
		+ \i \vt \mf operator Graphics&();
+ \mac PDefConverter(_type) @@ \h Base @@ \dir Adaptor;

r192:
/ \a \tp (Graphic & const Graphic) \o \def -> \tp const Graphics&;
/ \impl @@ \mf YThumb::DrawForeground() @@ \cl YGUIComponent;
/ @@ \h YWindow:
	- DeclIEntry(operator Graphics&()) @@ \in IWindow;
	- \mf operator Graphic& @@ \cl AWindow;

r193:
/= test 6;

r194:
/ @@ \cl Graphics @@ \u YObject:
	+ \mf BitmapPtr operator[](std::size_t) ythrow();
	+ \mf BitmapPtr at(std::size_t);
/= \tr \impl @@ \mf TextRegion::ClearLine(u16, SDST) @@ \u YText;

$DOING:


relative process:
2010-12-27:
-20.4d;

/ ...


$NEXT:

b177-b195:
/ graphic interface context @@ \cl YScreen;
/ user-defined bitmap buffer @@ \cl YDesktop;
* store more than one newed objects in smart pointers(see effc++ 3rd item17)
	@@ \ctor @@ \dir Shell;
/ fully \impl \u DSReader;
	* moved text after setting lnGap;
* non-ASCII character path error in FAT16;
+ \impl loading pictures;
+ \impl style on widgets;
/ - const_cast;
/ memcpy optimizing ^ DMA copy;
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

