// YSLib::Core::YShellDefinition by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-24 15:29:11;
// UTime = 2010-9-18 8:36;
// Version = 0.2216;


#ifndef INCLUDED_YSDEF_H_
#define INCLUDED_YSDEF_H_

// YShellDefinition ：类型描述模块。

/*
 ISO/IEC C++ 未确定宏定义内 # 和 ## 操作符求值顺序。
 GCC 中，宏定义内 ## 操作符修饰的形式参数为宏时，该宏不会被展开。详见： http://gcc.gnu.org/onlinedocs/cpp/Concatenation.html 。
解决方案来源： https://www.securecoding.cert.org/confluence/display/cplusplus/PRE05-CPP.+Understand+macro+replacement+when+concatenating+tokens+or+performing+stringification 。
*/
#define _yJOIN(x, y) x ## y


//适配器模块。
#include "../Adapter/yadapter.h"


//异常规范宏。

// ythrow = "yielded throwing";
#ifdef YSL_USE_EXCEPTION_SPECIFICATION
#	define ythrow throw
#else
#	define ythrow(...)
#endif


//接口类型宏。

#define __interface struct
#define _yInterface __interface

#define implements public

#define _yInterfaceHead(_name) { \
	virtual ~_name() {}

//prefix "PDecl" = Pre-declare;
#define PDeclInterface(_name) _yInterface _name;

#define DeclInterface(_name) _yInterface _name _yInterfaceHead(_name)

#ifdef __GNUC__
//对于基接口需要显示指定访问权限和继承方式。由于接口定义为 struct 类型，因此通常只需指定是否为 virtual 继承。
#	define DeclBasedInterface(_name, _base...) \
	_yInterface _name : _base _yInterfaceHead(_name)
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#	define DeclBasedInterface(_name, _base, ...) \
	_yInterface _name : _base, __VA_ARGS__ _yInterfaceHead(_name)
#else
//注意 ISO/IEC C++ 不支持宏的可变参数列表，因此无法实现接口多继承。
#	define DeclBasedInterface(_name, _base) \
	_yInterface _name : _base _yInterfaceHead(_name)
#endif

//"DeclIEntry" = Declare Interface Entry;
#define DeclIEntry(_signature) virtual _signature = 0;

#define EndDecl };


//函数宏。

//通用函数头定义。
//prefix "PDef" = Partially Define;
#ifdef __GNUC__
#	define PDefHead(_type, _name, _paralist...) _type _name(_paralist)
#	define PDefOpHead(_type, _op, _paralist...) _type operator _op(_paralist)
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#	define PDefHead(_type, _name, _paralist, ...) _type _name(_paralist, __VA_ARGS__)
#	define PDefOpHead(_type, _op, _paralist, ...) _type operator _op(_paralist, __VA_ARGS__)
#else
#	define PDefHead(_type, _name, _paralist) _type _name(_paralist)
#	define PDefOpHead(_type, _op, _paralist) _type operator _op(_paralist)
#endif


//简单通用函数实现。
//prefix "Impl" = Implement;
#define ImplExpr(_expr) { _expr; }
#define ImplRet(_expr) { return _expr; }

//基类同名函数映射和成员同名函数映射实现。
//prefix "Impl" = Implement;
#ifdef __GNUC__
#	define ImplBodyBase(_base, _name, _arglist...) { return _base::_name(_arglist); }
#	define ImplBodyBaseVoid(_base, _name, _arglist...) { _base::_name(_arglist); }
#	define ImplBodyMember(_member, _name, _arglist...) { return _member._name(_arglist); }
#	define ImplBodyMemberVoid(_member, _name, _arglist...) { _member._name(_arglist); }
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#	define ImplBodyBase(_base, _name, _arglist, ...) { return _base::_name(_arglist, __VA_ARGS__); }
#	define ImplBodyBaseVoid(_base, _name, _arglist, ...) { _base::_name(_arglist, __VA_ARGS__); }
#	define ImplBodyMember(_member, _name, _arglist...) { return _member._name(_arglist); }
#	define ImplBodyMemberVoid(_member, _name, _arglist...) { _member._name(_arglist); }
#else
#	define ImplBodyBase(_base, _name, _arglist) { return _base::_name(_arglist); }
#	define ImplBodyBaseVoid(_base, _name, _arglist) { _base::_name(_arglist); }
#	define ImplBodyMember(_member, _name, _arglist) { return _member._name(_arglist); }
#	define ImplBodyMemberVoid(_member, _name, _arglist) { _member._name(_arglist); }
#endif


//简单通用成员函数定义。
//prefix "Def" = Define;
#define DefConverter(_type, _expr) operator _type() const ythrow() { return _expr; }
#define DefConverterBase(_type, _base) operator _type() const ythrow() { return _base::operator _type(); }
#define DefConverterMember(_type, _member) operator _type() const ythrow() { return _member.operator _type(); }

#define DefGetter(_type, _name, _member) _type _yJOIN(Get, _name)() const ythrow() { return _member; }
#define DefGetterBase(_type, _name, _base) _type _yJOIN(Get, _name)() const ythrow() { return _base::_yJOIN(Get, _name)(); }
#define DefGetterMember(_type, _name, _member) _type _yJOIN(Get, _name)() const ythrow() { return _member._yJOIN(Get, _name)(); }

#define DefBoolGetter(_name, _member) bool _yJOIN(Is, _name)() const ythrow() { return _member; }
#define DefBoolGetterBase(_name, _base) bool _yJOIN(Is, _name)() const ythrow() { return _base::_yJOIN(Is, _name)(); }
#define DefBoolGetterMember(_name, _member) bool _yJOIN(Is, _name)() const ythrow() { return _member._yJOIN(Is, _name)(); }

#define DefStaticGetter(_type, _name, _member) static _type _yJOIN(Get, _name)() ythrow() { return _member; }
#define DefStaticGetterBase(_type, _name, _base) static _type _yJOIN(Get, _name)() ythrow() { return _base::_yJOIN(Get, _name)(); }

#define DefStaticBoolGetter(_name, _member) static bool _yJOIN(Is, _name)() ythrow() { return _member; }
#define DefStaticBoolGetterBase(_name, _base) static bool _yJOIN(Is, _name)() ythrow() { return _base::_yJOIN(Is, _name)(); }

#define DefSetter(_type, _name, _member) void _yJOIN(Set, _name)(_type _tempArgName) ythrow() { _member = _tempArgName; }
#define DefSetterDe(_type, _name, _member, _defv) void _yJOIN(Set, _name)(_type _tempArgName = _defv) ythrow() { _member = _tempArgName; }
#define DefSetterBase(_type, _name, _base) void _yJOIN(Set, _name)(_type _tempArgName) ythrow() { _base::_yJOIN(Set, _name)(_tempArgName); }
#define DefSetterBaseDe(_type, _name, _base, _defv) void _yJOIN(Set, _name)(_type _tempArgName = _defv) ythrow() { _base::_yJOIN(Set, _name)(_tempArgName); }
#define DefSetterMember(_type, _name, _member) void _yJOIN(Set, _name)(_type _tempArgName) ythrow() { _member._yJOIN(Set, _name)(_tempArgName); }
#define DefSetterMemberDe(_type, _name, _member, _defv) void _yJOIN(Set, _name)(_type _tempArgName = _defv) ythrow() { _member._yJOIN(Set, _name)(_tempArgName); }
#define DefSetterEx(_type, _name, _member, _tempArgName, _expr) void _yJOIN(Set, _name)(_type _tempArgName) ythrow() { _member = _expr; }
#define DefSetterDeEx(_type, _name, _member, _defv, _tempArgName, _expr) void _yJOIN(Set, _name)(_type _tempArgName = _defv) ythrow() { _member = _expr; }


#define YSL_BEGIN	namespace YSLib {
#define YSL_END		}
#define YSL_			::YSLib::
#define YSL			::YSLib

#define YSL_BEGIN_NAMESPACE(s)	namespace s {
#define YSL_END_NAMESPACE(s)		}

#define YSL_BEGIN_SHELL(s)	namespace _yJOIN(_YSHL_, s) {
#define YSL_END_SHELL(s)	}
#define YSL_SHL_(s)		YSL_ _yJOIN(_YSHL_, s)::
#define YSL_SHL(s)		YSL_ _yJOIN(_YSHL_, s)


#ifndef YSL_SHLMAIN_NAME
#	define YSL_SHLMAIN_NAME ShlMain
#endif

#ifndef YSL_SHLMAIN_SHLPROC
#	define YSL_SHLMAIN_SHLPROC ShlProc
#endif

#define YSL_MAIN_SHLPROC YSL_SHL_(YSL_SHLMAIN_NAME) YSL_SHLMAIN_SHLPROC

// YSLib 选项。
/*
YSLib No_Cursor
定义无指针设备。
消息成员列表中不会包含 SPoint pt 。
*/
//#define YSLIB_NO_CURSOR

// YSLib Debug 选项。
/*
YSLib MSG Debug
bit0 ： InsertMessage ；
bit1 ： PeekMessage 。
注意： PeekMessage 包含 InsertMessage 。
*/
#define YSLIB_DEBUG_MSG 0

//间接访问类模块。
#include "../Adapter/yref.hpp"

YSL_BEGIN

//抽象描述接口。

//值类型相等关系。
template<typename T>
DeclInterface(GIEquatable)
	DeclIEntry(bool operator==(const T&) const)
	virtual bool
	operator!=(const T& rhs) const
	{
		return !this->operator==(rhs);
	}
EndDecl

//值类型小于关系。
template<typename T>
DeclInterface(GILess)
	DeclIEntry(bool operator<(const T&) const)
EndDecl

//容器。
template<typename T>
DeclInterface(GIContainer)
	DeclIEntry(void operator+=(T&))
	DeclIEntry(bool operator-=(T&))
EndDecl

//对象复制构造性。
template<typename T>
DeclInterface(GIClonable)
	DeclIEntry(T* Clone() const)
EndDecl


//前向声明和类型定义。

//空结构体类型。
struct EmptyType
{};

class YObject;
typedef EmptyType MEventArgs; //事件参数基类。

YSL_BEGIN_NAMESPACE(Device)
class YScreen;
YSL_END_NAMESPACE(Device)

YSL_BEGIN_NAMESPACE(Drawing)
class YFontCache;
YSL_END_NAMESPACE(Drawing)

YSL_BEGIN_NAMESPACE(Shells)
class YMessageQueue;
class YShell;
class YShellMain;
YSL_END_NAMESPACE(Shells)

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
PDeclInterface(IWidgetContainer)
YSL_END_NAMESPACE(Widgets)

class YDesktop;

YSL_END_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Text)
typedef std::size_t SizeType; //字符大小类型。
typedef usize_t IndexType; //字符索引类型。
class String;
YSL_END_NAMESPACE(Text)

class YApplication;
class YLog;

using Components::Controls::IControl;
using Components::Controls::IVisualControl;
using Components::Forms::IWindow;
using Components::Forms::YForm;
using Components::Forms::YFrameWindow;
using Components::Widgets::IWidget;
using Components::Widgets::IWidgetContainer;
using Components::YDesktop;

using Device::YScreen;

using Drawing::YFontCache;

using Shells::YMessageQueue;
using Shells::YShell;
using Shells::YShellMain;

using Text::String;


#ifdef YSL_USE_SIMPLE_HANDLE
#define DeclareHandle(type, handle) typedef type* handle
#define handle_cast reinterpret_cast
#else
#define DeclareHandle(type, handle) typedef GHHandle<type> handle
#endif


//类型定义。
typedef s16 SPOS; //屏幕坐标度量。
typedef u16 SDST; //屏幕坐标距离。
DeclareHandle(IWindow, HWND);
DeclareHandle(YShell, HSHL);
DeclareHandle(YApplication, HINSTANCE);
typedef enum {RDeg0 = 0, RDeg90 = 1, RDeg180 = 2, RDeg270 = 3} ROT; //逆时针旋转角度指示输出朝向。

//全局常量。
extern CSTR G_COMP_NAME, G_APP_NAME, G_APP_VER; //制作机构名称，程序名称和版本号。
extern CPATH DEF_DIRECTORY;
extern const SDST SCRW, SCRH;

//全局变量。
extern YLog DefaultLog; //全局日志。

//访问全局程序实例对象。
extern YScreen*& pDefaultScreen;
extern YDesktop*& pDefaultDesktop;
extern YApplication& theApp;
extern YFontCache*& pDefaultFontCache;
extern YApplication* const pApp;
extern const HSHL hShellMain;

YSL_END

#endif

