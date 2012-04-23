/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Initialization.cpp
\ingroup Helper
\brief 程序启动时的通用初始化。
\version r2024;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-10-21 23:15:08 +0800;
\par 修改时间:
	2012-04-23 10:54 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	Helper::Initialization;
*/


#include "Helper/Initialization.h"
#include <YSLib/Adaptor/Font.h>
#include <YSLib/Core/yapp.h>
#include "Helper/DSMain.h"
#include <YCLib/Debug.h>
#include <YSLib/Service/yftext.h> // for BOM_UTF8;
//#include <clocale>

using namespace ystdex;
using namespace platform;
using std::puts;

YSL_BEGIN

using namespace Drawing;
using namespace IO;

namespace
{

/*!
\brief 初始化失败公用程序。
*/
void
EpicFail()
{
	YDebugSetStatus();
	YDebugBegin();
}

inline void
fatalError()
{
	terminate();
}

void
printFailInfo(const char* t, const char* s)
{
	EpicFail();

	const char* line =
		"--------------------------------";

	std::printf("%s%s%s\n%s\n%s", line, t, line, s, line);
}

void
defFontFail()
{
	yconstexpr const char* title =
		"    Fontface Caching Failure    ";
	yconstexpr const char* warning =
		" Please make sure the fonts are\n"
		" stored in correct directory.\n";

	printFailInfo(title, warning);
	fatalError();
}

void
installFail(const char* str)
{
	yconstexpr const char* title =
		"      Invalid Installation      ";
	yconstexpr const char* warning =
		" Please make sure the data is\n"
		" stored in correct directory.\n";

	printFailInfo(title, warning);
	std::printf(" %s\n cannot be found!\n", str);
	fatalError();
}


/*!
\brief 读取字体文件目录并载入目录下指定后缀名的字体文件。
*/
void
LoadFontFileDirectory(FontCache& fc,
	const_path_t path/*, const_path_t ext = "ttf"*/)
{
	HFileNode dir(path);

	if(dir.IsValid())
		while((++dir).LastError == 0)
			if(std::strcmp(dir.GetName(), FS_Now) != 0
				&& !dir.IsDirectory()
				/*&& IsExtendNameOf(ext, dir.GetName())*/)
				fc.LoadFontFile((FontPath(path) + dir.GetName()).c_str());
}

/*!
\since build 300 。
*/
void
CheckConfiguration()
{
#ifdef YCL_DS
#	define ROOTW
#	define DEF_DIRECTORY ROOTW "/Data/"
	//const char* DEF_FONT_NAME = ROOTW "方正姚体";
	//const_path_t DEF_FONT_PATH = ROOTW "/Font/FZYTK.TTF";
#	define DEF_FONT_PATH ROOTW "/Font/FZYTK.TTF"
#	define DEF_FONT_DIRECTORY ROOTW "/Font/"
#else
#	define ROOTW "H:\\NDS\\EFSRoot"
#	define DEF_DIRECTORY ROOTW "\\Data\\"
	//const char* DEF_FONT_NAME = "方正姚体";
	//const_path_t DEF_FONT_PATH = ROOTW "\\Font\\FZYTK.TTF";
#	define DEF_FONT_PATH ROOTW "\\Font\\FZYTK.TTF"
#	define DEF_FONT_DIRECTORY ROOTW "\\Font\\"
#endif
#define CONF_PATH "config.txt"

	if(!ufexists(CONF_PATH))
	{
		std::puts("Creating configuration file...");

		const auto fp(ufopen(CONF_PATH, "w"));

		if(!fp)
			throw LoggedEvent("Cannot create file.");

		std::fprintf(fp, "%s%s\n%s\n%s\n", BOM_UTF_8,
			DEF_DIRECTORY, DEF_FONT_PATH, DEF_FONT_DIRECTORY);
		std::fclose(fp);
	}
	else
		std::printf("Found configuration file '%s'.\n", CONF_PATH);
}

#ifdef YCL_DS
void
libfatFail()
{
	yconstexpr const char* title =
		"         LibFAT Failure         ";
	yconstexpr const char* warning =
		" An error is preventing the\n"
		" program from accessing\n"
		" external files.\n"
		"\n"
		" If you're using an emulator,\n"
		" make sure it supports DLDI\n"
		" and that it's activated.\n"
		"\n"
		" In case you're seeing this\n"
		" screen on a real DS, make sure\n"
		" you've applied the correct\n"
		" DLDI patch (most modern\n"
		" flashcards do this\n"
		" automatically).\n"
		"\n"
		" Note: Some cards only\n"
		" autopatch .nds files stored in\n"
		" the root folder of the card.\n";

	printFailInfo(title, warning);
	fatalError();
}
#endif

char def_dir[80], font_path[80], font_dir[80];

} // unnamed namespace;


void
InitializeEnviornment() ynothrow
{
	//设置默认异常终止函数。
	std::set_terminate(terminate);
	try
	{
#ifdef YCL_DS
		//启用设备。
		::powerOn(POWER_ALL);

		//启用 LibNDS 默认异常处理。
		::defaultExceptionHandler();

		//初始化主控制台。
		platform::YConsoleInit(true, ColorSpace::Lime);

		//初始化文件系统。
		//初始化 EFSLib 和 LibFAT 。
		//当 .nds 文件大于32MB时， EFS 行为异常。
#	ifdef USE_EFS
		if(!::EFS_Init(EFS_AND_FAT | EFS_DEFAULT_DEVICE, nullptr))
		{
			//如果初始化 EFS 失败则初始化 FAT 。
#	endif
			if(!::fatInitDefault())
				libfatFail();
#	ifdef USE_EFS
		}
#	endif
#endif
#if 0
		// TODO: review locale APIs compatibility;
		static yconstexpr char locale_str[]{"zh_CN.GBK"};

		if(!setlocale(LC_ALL, locale_str))
		{
			throw LoggedEvent("setlocale() with %s failed.\n", locale_str);
		}
#endif
	}
	catch(LoggedEvent& e)
	{
		puts(e.what());
		EpicFail();
	}
	catch(...)
	{
		EpicFail();
	}
}

void
InitializeSystemFontCache() ynothrow
{
	puts("Loading font files...");
	try
	{
		FetchGlobalInstance().ResetFontCache(font_path);
		// TODO: 使用不依赖于 YGlobal 未确定接口的实现。

		auto& fc(FetchDefaultFontCache());

		if(*font_path &&fc.LoadFontFile(FontPath(font_path)))
			fc.LoadTypefaces();
		if(*font_dir)
		{
			LoadFontFileDirectory(fc, font_dir);
			fc.LoadTypefaces();
		}
		fc.InitializeDefaultTypeface();
			if(FetchDefaultFontCache().GetFaces().size() == 0)
				throw LoggedEvent("No font loaded.");
		std::printf("%u font file(s) are loaded\nsuccessfully.\n",
			fc.GetPaths().size());
		puts("Setting default font face...");
		if(const auto* const pf = fc.GetDefaultTypefacePtr())
			std::printf("\"%s\":\"%s\",\nsuccessfully.\n",
				pf->GetFamilyName().c_str(), pf->GetStyleName().c_str());
		else
			throw LoggedEvent("failed.");
	}
	catch(LoggedEvent& e)
	{
		puts(e.what());
		defFontFail();
	}
	catch(...)
	{
		defFontFail();
	}
}

void
CheckInstall() ynothrow
{
	puts("Checking installation...");
	try
	{
		CheckConfiguration();

		TextFile tf(CONF_PATH);

		if(YCL_LIKELY(tf.IsValid()))
		{
			if(YCL_UNLIKELY(tf.Encoding != Text::CharSet::UTF_8))
				throw LoggedEvent("Wrong encoding of configuration file.");

			const auto fp(tf.GetPtr());

			std::fgets(def_dir, 80, fp);
			std::fgets(font_path, 80, fp);
			std::fgets(font_dir, 80, fp);
			if(*def_dir && *font_path && *font_dir)
			{
				def_dir[std::strlen(def_dir) - 1] = '\0';
				font_path[std::strlen(font_path) - 1] = '\0';
				font_dir[std::strlen(font_dir) - 1] = '\0';
				std::printf("Loaded default directory:\n%s\n"
					"Loaded default font path:\n%s\n"
					"Loaded default font directory:\n%s\n",
					def_dir, font_path, font_dir);
			}
			else
				throw LoggedEvent("Empty path loaded!");
		}
		else
			throw LoggedEvent("Configuration file loading failed.");
	}
	catch(LoggedEvent& e)
	{
		std::puts("Error occured:");
		std::puts(e.what());
		installFail("Loading configuration");
	}
	catch(...)
	{
		installFail("Loading configuration");
	}
	std::printf("Trying entering directory %s ...\n", def_dir);
	if(!direxists(def_dir))
		installFail("Default data directory");
	if(!(fexists(font_path)
		|| direxists(font_dir)))
		installFail("Default font");
	puts("OK!");
}

YSL_END

