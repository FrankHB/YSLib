/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Initialization.cpp
\ingroup Adaptor
\brief 程序启动时的通用初始化。
\version r1933;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-10-21 23:15:08 +0800;
\par 修改时间:
	2012-04-12 20:20 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Helper::Initialization;
*/


#include "YSLib/Helper/Initialization.h"
#include "YSLib/Adaptor/Font.h"
#include "YSLib/Core/yapp.h"
#include "YSLib/Helper/DSMain.h"
#include "YCLib/Debug.h"
#include "YSLib/Service/yftext.h" // for BOM_UTF8;

using namespace ystdex;
using namespace platform;
using std::puts;

YSL_BEGIN

using namespace Drawing;
using namespace IO;

void
EpicFail()
{
	YDebugSetStatus();
	YDebugBegin();
}

namespace
{

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
	const char* title =
		"    Fontface Caching Failure    ";
	const char* warning =
		" Please make sure the fonts are "
		" stored in correct directory.   ";

	printFailInfo(title, warning);
	fatalError();
}

void
installFail(const char* str)
{
	const char* title =
		"      Invalid Installation      ";
	const char* warning =
		" Please make sure the data is   "
		" stored in correct directory.   ";

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

char def_dir[80], font_path[80], font_dir[80];

} // unnamed namespace;

void
LibfatFail()
{
	const char* title =
		"         LibFAT Failure         ";
	const char* warning =
		" An error is preventing the     "
		" program from accessing         "
		" external files.                "
		"                                "
		" If you're using an emulator,   "
		" make sure it supports DLDI     "
		" and that it's activated.       "
		"                                "
		" In case you're seeing this     "
		" screen on a real DS, make sure "
		" you've applied the correct     "
		" DLDI patch (most modern        "
		" flashcards do this             "
		" automatically).                "
		"                                "
		" Note: Some cards only          "
		" autopatch .nds files stored in "
		" the root folder of the card.   ";

	printFailInfo(title, warning);
	fatalError();
}


void
InitializeSystemFontCache()
{
	puts("Loading font files...");
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
	CheckSystemFontCache();
	std::printf("%u font file(s) are loaded\nsuccessfully.\n",
		fc.GetPaths().size());
	puts("Setting default font face...");
	if(const auto* const pf = fc.GetDefaultTypefacePtr())
		std::printf("\"%s\":\"%s\",\nsuccessfully.\n",
			pf->GetFamilyName().c_str(), pf->GetStyleName().c_str());
	else
	{
		puts("failed.");
		defFontFail();
	}
}

void
CheckInstall()
{
	puts("Checking installation...");
	try
	{
		CheckConfiguration();

		TextFile tf(CONF_PATH);

		if(YCL_LIKELY(tf.IsValid()))
		{
			if(YCL_UNLIKELY(tf.Encoding != Text::CharSet::UTF_8))
				throw LoggedEvent("Wrong encoding of configuration file!");

			const auto fp(tf.GetPtr());

			std::fgets(def_dir, 80, fp);
			std::fgets(font_path, 80, fp);
			std::fgets(font_dir, 80, fp);
			if(*def_dir && *font_path && *font_dir)
			{
				def_dir[std::strlen(def_dir) - 1] = '\0';
				font_path[std::strlen(font_path) - 1] = '\0';
				font_dir[std::strlen(font_dir) - 1] = '\0';
				std::printf("Loaded default directory:\n%s\n", def_dir);
				std::printf("Loaded default font path:\n%s\n", font_path);
				std::printf("Loaded default font directory:\n%s\n", font_dir);
			}
			else
				throw LoggedEvent("Empty path loaded!");
		}
		else
			throw LoggedEvent("Configuration loading failure.");
	}
	catch(LoggedEvent& e)
	{
		std::puts("Error occured.");
		std::puts(e.what());
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

void
CheckSystemFontCache()
{
	try
	{
		if(FetchDefaultFontCache().GetFaces().size() > 0)
			return;
	}
	catch(...)
	{}
	defFontFail();
}

YSL_END

