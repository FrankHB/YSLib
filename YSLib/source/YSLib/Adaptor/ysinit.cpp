/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ysinit.cpp
\ingroup Service
\brief 程序启动时的通用初始化。
\version 0.1792;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-10-21 23:15:08 +0800;
\par 修改时间:
	2011-06-08 18:14 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Adaptor::YShellInitialization;
*/


#include "ysinit.h"
#include "yfont.h"
#include "../Core/yapp.h"
#include "../Helper/yglobal.h"

using namespace ystdex;
using namespace platform;
using std::puts;

YSL_BEGIN

using namespace Drawing;
using namespace IO;

void
InitYSConsole()
{
	YConsoleInit(true, ColorSpace::Lime);
}

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

		iprintf("%s%s%s\n%s\n%s", line, t, line, s, line);
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
		iprintf(" %s\n cannot be found!\n", str);
		fatalError();
	}


	/*!
	\brief 读取字体文件目录并载入目录下指定后缀名的字体文件。
	*/
	void
	LoadFontFileDirectory(FontCache& fc,
		const_path_t path/*, const_path_t ext = "ttf"*/)
	{
		HDirectory dir(path);

		if(dir.IsValid())
			while((++dir).LastError == 0)
				if(std::strcmp(HDirectory::Name, FS_Now) != 0
					&& !HDirectory::IsDirectory()
					/*&& IsExtendNameOf(ext, HDirectory::Name)*/)
					fc.LoadFontFile((FontFile::PathType(path)
						+ HDirectory::Name).c_str());
	}
}

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
	FetchAppInstance().ResetFontCache(DEF_FONT_PATH);

	FontCache& fc(FetchAppInstance().GetFontCache());

	if(DEF_FONT_PATH)
	{
		if(fc.LoadFontFile(DEF_FONT_PATH))
		{
			fc.LoadTypefaces();
			fc.InitializeDefaultTypeface();
		}
	}
	if(DEF_FONT_DIRECTORY)
	{
		LoadFontFileDirectory(fc, DEF_FONT_DIRECTORY);
		fc.LoadTypefaces();
	}
	fc.InitializeDefaultTypeface();
	CheckSystemFontCache();
	std::printf("%u font file(s) are loaded\nsuccessfully.\n", fc.GetFilesN());
	puts("Setting default font face...");

	const Drawing::Typeface* const
		pf(fc.GetDefaultTypefacePtr());

	if(pf)
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
	if(!direxists(Text::StringToMBCS(YApplication::CommonAppDataPath).c_str()))
		installFail("Default data directory");
	if(!(fexists(DEF_FONT_PATH)
		|| direxists(DEF_FONT_DIRECTORY)))
		installFail("Default font");
	puts("OK!");
}

void
CheckSystemFontCache()
{
	try
	{
		if(FetchAppInstance().GetFontCache().GetTypesN() > 0)
			return;
	}
	catch(...)
	{}
	defFontFail();
}

YSL_END

