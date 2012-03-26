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
\version r1845;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-10-21 23:15:08 +0800;
\par 修改时间:
	2012-03-25 15:57 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Helper::Initialization;
*/


#include "YSLib/Helper/Initialization.h"
#include "YSLib/Adaptor/Font.h"
#include "YSLib/Core/yapp.h"
#include "YSLib/Helper/DSMain.h"

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
		HDirectory dir(path);

		if(dir.IsValid())
			while((++dir).LastError == 0)
				if(std::strcmp(HDirectory::Name, FS_Now) != 0
					&& !HDirectory::IsDirectory()
					/*&& IsExtendNameOf(ext, HDirectory::Name)*/)
					fc.LoadFontFile((FontPath(path)
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
	FetchGlobalInstance().ResetFontCache(DEF_FONT_PATH);
	// TODO: 使用不依赖于 YGlobal 未确定接口的实现。

	auto& fc(FetchDefaultFontCache());

	if(DEF_FONT_PATH && fc.LoadFontFile(FontPath(DEF_FONT_PATH)))
		fc.LoadTypefaces();
	if(DEF_FONT_DIRECTORY)
	{
		LoadFontFileDirectory(fc, DEF_FONT_DIRECTORY);
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

	const auto& dir_str(Application::CommonAppDataPath.GetNativeString());
	const auto dir(dir_str.c_str());

	std::printf("Trying entering directory %s ...\n", dir);
	if(!direxists(dir))
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
		if(FetchDefaultFontCache().GetFaces().size() > 0)
			return;
	}
	catch(...)
	{}
	defFontFail();
}

YSL_END

