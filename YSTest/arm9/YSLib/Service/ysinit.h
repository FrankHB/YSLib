// YSLib::Service::YShellInitialization by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-10-21 23:15:08 + 08:00;
// UTime = 2010-10-24 16:52 + 08:00;
// Version = 0.1516;


#ifndef INCLUDED_YSINIT_H_
#define INCLUDED_YSINIT_H_

// YShellInitialization ：程序启动时的通用初始化代码段。

#include "../Core/ysdef.h"

YSL_BEGIN

//********************************
//名称:		InitYSConsole
//全名:		YSLib::InitYSConsole
//可访问性:	public 
//返回类型:	void
//修饰符:	
//功能概要:	初始化主控制台。
//备注:		
//********************************
void
InitYSConsole();


//默认字型路径和目录。
extern CPATH DEF_FONT_PATH;
extern CPATH DEF_FONT_DIRECTORY;

//********************************
//名称:		InitializeSystemFontCache
//全名:		YSLib::InitializeSystemFontCache
//可访问性:	public 
//返回类型:	void
//修饰符:	
//功能概要:	初始化系统字体缓存：
//			以默认字型路径创建默认字体缓存并加载默认字型目录中的字体文件。
//备注:		
//********************************
void
InitializeSystemFontCache();

//********************************
//名称:		DestroySystemFontCache
//全名:		YSLib::DestroySystemFontCache
//可访问性:	public 
//返回类型:	void
//修饰符:	
//功能概要:	注销系统字体缓存。
//备注:		
//********************************
void
DestroySystemFontCache();

//********************************
//名称:		EpicFail
//全名:		YSLib::EpicFail
//可访问性:	public 
//返回类型:	void
//修饰符:	
//功能概要:	初始化失败公用程序。
//备注:		
//********************************
void
EpicFail();

//********************************
//名称:		LibfatFail
//全名:		YSLib::LibfatFail
//可访问性:	public 
//返回类型:	void
//修饰符:	
//功能概要:	LibFAT 失败。
//备注:		
//********************************
void
LibfatFail();

//********************************
//名称:		CheckInstall
//全名:		YSLib::CheckInstall
//可访问性:	public 
//返回类型:	void
//修饰符:	
//功能概要:	检查安装完整性。
//备注:		
//********************************
void
CheckInstall();

//********************************
//名称:		CheckSystemFontCache
//全名:		YSLib::CheckSystemFontCache
//可访问性:	public 
//返回类型:	void
//修饰符:	
//功能概要:	检查系统字体缓存。
//备注:		
//********************************
void
CheckSystemFontCache();

YSL_END

#endif

