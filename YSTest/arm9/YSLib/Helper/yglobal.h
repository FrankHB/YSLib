// YSLib::Helper::YGlobal by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-22 15:14:57 + 08:00;
// UTime = 2010-10-22 13:21 + 08:00;
// Version = 0.1626;


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
	//默认消息发生函数。
	static void
	Idle();

	//初始化屏幕。
	static bool
	InitVideo();

	//默认屏幕初始化函数。
	static void
	InitScreenAll();

	//默认上屏初始化函数。
	static void
	InitScrUp();

	//默认下屏初始化函数。
	static void
	InitScrDown();

	//以指定前景色和背景色初始化指定屏幕的控制台。
	static bool
	InitConsole(YScreen&, Drawing::PixelType, Drawing::PixelType);

	//资源释放函数。
	static void
	Destroy(YObject&, const MEventArgs&);

	//公共消息处理函数。
	static LRES
	ShlProc(HSHL, const Message&);
};

//全局函数。

YSL_BEGIN_SHELL(YSL_SHLMAIN_NAME)

LRES
YSL_SHLMAIN_SHLPROC(const Message&);

YSL_END_SHELL(YSL_SHLMAIN_NAME)


//默认 Shell 消息处理函数。
inline LRES
DefaultMainShlProc(const Message& msg)
{
	return YSL_MAIN_SHLPROC(msg);
}

YSL_END

#endif

