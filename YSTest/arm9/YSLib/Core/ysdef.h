// YSLib::Core::YShellDefinition by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-24 15:29:11 + 08:00;
// UTime = 2010-11-04 11:29 + 08:00;
// Version = 0.2328;


#ifndef INCLUDED_YSDEF_H_
#define INCLUDED_YSDEF_H_

// YShellDefinition ：宏定义和类型描述模块。

//适配器模块。
#include "../Adaptor/yadaptor.h"

#ifndef YSL_SHLMAIN_NAME
#	define YSL_SHLMAIN_NAME ShlMain
#endif

#ifndef YSL_SHLMAIN_SHLPROC
#	define YSL_SHLMAIN_SHLPROC ShlProc
#endif

#define YSL_MAIN_SHLPROC YSL_SHL_(YSL_SHLMAIN_NAME) YSL_SHLMAIN_SHLPROC

//间接访问类模块。
#include "../Adaptor/yref.hpp"

YSL_BEGIN

//前向声明和类型定义。

//空结构体类型。
struct EmptyType
{};

class YObject;
typedef EmptyType EventArgs; //事件参数基类。

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
PDeclInterface(IUIBox)
PDeclInterface(IUIContainer)
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
using Components::Widgets::IUIBox;
using Components::Widgets::IUIContainer;
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
typedef enum {RDeg0 = 0, RDeg90 = 1, RDeg180 = 2, RDeg270 = 3} ROT; \
	//逆时针旋转角度指示输出朝向。

//全局常量。
extern CSTR G_COMP_NAME, G_APP_NAME, G_APP_VER; \
	//制作机构名称，程序名称和版本号。
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

