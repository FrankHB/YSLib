// YSLib::Helper::YGlobal by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-22 15:14:57 + 08:00;
// UTime = 2010-10-24 17:50 + 08:00;
// Version = 0.1714;


#ifndef INCLUDED_YGLOBAL_H_
#define INCLUDED_YGLOBAL_H_

// YGlobal ：全局对象和函数定义。

#include "../Core/ysmsg.h"
#include "../Core/yexcept.h"

YSL_BEGIN

//平台相关的全局常量。
#define MAX_FILENAME_LENGTH MAXPATHLEN
#define MAX_PATH_LENGTH MAX_FILENAME_LENGTH
#define DEF_SHELL_FONTSIZE 16
#define DEF_SHELL_FONTSIZE_MIN 4
#define DEF_SHELL_FONTSIZE_MAX 72

const char DEF_PATH_DELIMITER = '/'; //文件路径分隔符。
const char* const DEF_PATH_SEPERATOR = "/"; //文件路径分隔字符串。
#define DEF_PATH_ROOT DEF_PATH_SEPERATOR

//平台相关的全局变量。
extern YScreen *pScreenUp, *pScreenDown;
extern YDesktop *pDesktopUp, *pDesktopDown;

//类型定义。
typedef char PATHSTR[MAX_PATH_LENGTH];
//typedef char PATHSTR[MAXPATHLEN];
typedef char FILENAMESTR[MAX_FILENAME_LENGTH];

struct Def
{
	//********************************
	//名称:		Idle
	//全名:		YSLib::Def::Idle
	//可访问性:	public static 
	//返回类型:	void
	//修饰符:	
	//功能概要:	默认消息发生函数。
	//备注:		
	//********************************
	static void
	Idle();

	//********************************
	//名称:		InitVideo
	//全名:		YSLib::Def::InitVideo
	//可访问性:	public static 
	//返回类型:	bool
	//修饰符:	
	//功能概要:	初始化视频输出。
	//备注:		
	//********************************
	static bool
	InitVideo();

	//********************************
	//名称:		InitScreenAll
	//全名:		YSLib::Def::InitScreenAll
	//可访问性:	public static 
	//返回类型:	void
	//修饰符:	
	//功能概要:	默认屏幕初始化函数。
	//备注:		
	//********************************
	static void
	InitScreenAll();

	//********************************
	//名称:		InitScrUp
	//全名:		YSLib::Def::InitScrUp
	//可访问性:	public static 
	//返回类型:	void
	//修饰符:	
	//功能概要:	默认上屏初始化函数。
	//备注:		
	//********************************
	static void
	InitScrUp();

	//********************************
	//名称:		InitScrDown
	//全名:		YSLib::Def::InitScrDown
	//可访问性:	public static 
	//返回类型:	void
	//修饰符:	
	//功能概要:	默认下屏初始化函数。
	//备注:		
	//********************************
	static void
	InitScrDown();

	//********************************
	//名称:		InitConsole
	//全名:		YSLib::Def::InitConsole
	//可访问性:	public static 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	YScreen &
	//形式参数:	Drawing::PixelType
	//形式参数:	Drawing::PixelType
	//功能概要:	以指定前景色和背景色初始化指定屏幕的控制台。
	//备注:		
	//********************************
	static bool
	InitConsole(YScreen&, Drawing::PixelType, Drawing::PixelType);

	//********************************
	//名称:		WaitForInput
	//全名:		YSLib::Def::WaitForInput
	//可访问性:	public static 
	//返回类型:	void
	//修饰符:	
	//功能概要:	默认输入等待函数。
	//备注:		
	//********************************
	static void
	WaitForInput();

	//********************************
	//名称:		Destroy
	//全名:		YSLib::Def::Destroy
	//可访问性:	public static 
	//返回类型:	void
	//修饰符:	
	//形式参数:	YObject &
	//形式参数:	const MEventArgs &
	//功能概要:	全局资源释放函数。
	//备注:		
	//********************************
	static void
	Destroy(YObject&, const MEventArgs&);

	//********************************
	//名称:		ShlProc
	//全名:		YSLib::Def::ShlProc
	//可访问性:	public static 
	//返回类型:	LRES
	//修饰符:	
	//形式参数:	HSHL
	//形式参数:	const Message &
	//功能概要:	公共消息处理函数。
	//备注:		
	//********************************
	static LRES
	ShlProc(HSHL, const Message&);
};

//全局函数。

YSL_BEGIN_SHELL(YSL_SHLMAIN_NAME)

LRES
YSL_SHLMAIN_SHLPROC(const Message&);

YSL_END_SHELL(YSL_SHLMAIN_NAME)


//********************************
//名称:		DefaultMainShlProc
//全名:		YSLib::DefaultMainShlProc
//可访问性:	public 
//返回类型:	LRES
//修饰符:	
//形式参数:	const Message & msg
//功能概要:	全局默认 Shell 消息处理函数。
//备注:		
//********************************
inline LRES
DefaultMainShlProc(const Message& msg)
{
	return YSL_MAIN_SHLPROC(msg);
}

YSL_END

#endif

