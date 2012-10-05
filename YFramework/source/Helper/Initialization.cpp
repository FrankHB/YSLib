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
\version r1715
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-10-21 23:15:08 +0800
\par 修改时间:
	2012-10-05 21:46 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::Initialization
*/


#include "Helper/Initialization.h"
#include "YSLib/Adaptor/Font.h"
#include "YSLib/Core/yapp.h"
#include "Helper/DSMain.h"
#include "YCLib/Debug.h"
#include "CHRLib/MapEx.h"
#include "YCLib/MemoryMapping.h"
//#include <clocale>

using namespace ystdex;
using namespace platform;
using std::puts;

YSL_BEGIN

using namespace Drawing;
using namespace IO;

namespace
{

#if !CHRLIB_NODYNAMIC_MAPPING
//! \since build 324;
platform::MappedFile* p_mapped;
#endif

#if YCL_DS
#	define ROOTW
#	define DATA_DIRECTORY ROOTW "/Data/"
	//const char* DEF_FONT_NAME = ROOTW "方正姚体";
	//const_path_t DEF_FONT_PATH = ROOTW "/Font/FZYTK.TTF";
#	define DEF_FONT_PATH ROOTW "/Font/FZYTK.TTF"
#	define DEF_FONT_DIRECTORY ROOTW "/Font/"
#else
#	define ROOTW "H:\\NDS\\EFSRoot"
#	define DATA_DIRECTORY ROOTW "\\Data\\"
	//const char* DEF_FONT_NAME = "方正姚体";
	//const_path_t DEF_FONT_PATH = ROOTW "\\Font\\FZYTK.TTF";
#	define DEF_FONT_PATH ROOTW "\\Font\\FZYTK.TTF"
#	define DEF_FONT_DIRECTORY ROOTW "\\Font\\"
#endif
#define CONF_PATH "yconf.txt"

void
LoadComponents(const ValueNode& node)
{
	const auto& data_dir(AccessChild<string>(node, "DataDirectory"));
	const auto& font_path(AccessChild<string>(node, "FontFile"));
	const auto& font_dir(AccessChild<string>(node, "FontDirectory"));

	if(!data_dir.empty() && !font_path.empty() && !font_dir.empty())
		std::printf("Loaded default directory:\n%s\n"
			"Loaded default font path:\n%s\n"
			"Loaded default font directory:\n%s\n",
			data_dir.c_str(), font_path.c_str(), font_dir.c_str());
	else
		throw LoggedEvent("Empty path loaded.");
#if !CHRLIB_NODYNAMIC_MAPPING
	puts("Load character mapping file...");
	p_mapped = new MappedFile(data_dir + "cp113.bin");
	if(p_mapped->GetSize() != 0)
		CHRLib::cp113 = p_mapped->GetPtr();
	else
		throw LoggedEvent("CHRMapEx loading fail.");
	puts("CHRMapEx loaded successfully.");
#endif
	std::printf("Trying entering directory %s ...\n", data_dir.c_str());
	if(!udirexists(data_dir))
		throw LoggedEvent("Invalid default data directory found.");
	if(!(ufexists(font_path) || udirexists(font_dir)))
		throw LoggedEvent("Invalid default font file path found.");
}

} // unnamed namespace;

void
HandleFatalError(const FatalError& e) ynothrow
{
	YDebugSetStatus();
	YDebugBegin();

	const char* line("--------------------------------");

	std::printf("%s%s%s\n%s\n%s",
		line, e.GetTitle(), line, e.GetContent(), line);
	terminate();
}


ValueNode
ReadConfiguration(TextFile& tf)
{
	if(YB_LIKELY(tf))
	{
		if(YB_UNLIKELY(tf.Encoding != Text::CharSet::UTF_8))
			throw LoggedEvent("Wrong encoding of configuration file.");

		NPL::Configuration conf;

		tf >> conf;
		if(conf.GetNodeRRef().GetSize() != 0)
			return conf.GetNodeRRef();
	}
	throw LoggedEvent("Invalid file found when reading configuration.");
}

void
WriteConfiguration(TextFile& tf, const ValueNode& node)
{
	if(YB_UNLIKELY(!tf))
		throw LoggedEvent("Invalid file found when writing configuration.");
	tf << NPL::Configuration("", node.GetValue());
}

ValueNode
LoadConfiguration(bool bInfo)
{
	if(!ufexists(CONF_PATH))
	{
		if(bInfo)
			std::printf("Creating configuration file '%s'...\n", CONF_PATH);
		if(TextFile tf{CONF_PATH, std::ios_base::out | std::ios_base::trunc})
			tf << NPL::Configuration(PackNodes("YFramework", ValueNode(
				"DataDirectory", string(DATA_DIRECTORY)), ValueNode("FontFile",
				string(DEF_FONT_PATH)),
				ValueNode("FontDirectory", string(DEF_FONT_DIRECTORY))));
		else
			throw LoggedEvent("Cannot create file.");
	}

	TextFile tf(CONF_PATH);

	if(bInfo)
		std::printf("Found configuration file '%s'.\n", CONF_PATH);
	return ReadConfiguration(tf);
}

void
SaveConfiguration(const ValueNode& node)
{
	TextFile tf(CONF_PATH, std::ios_base::out | std::ios_base::trunc);

	WriteConfiguration(tf, node);
}


void
InitializeEnviornment()
{
	//设置默认异常终止函数。
	std::set_terminate(terminate);
#if YCL_DS
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
			throw FatalError("         LibFAT Failure         ",
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
				" the root folder of the card.\n");
#	ifdef USE_EFS
	}
#	endif
#endif
#if 0
	// TODO: Review locale APIs compatibility.
	static yconstexpr char locale_str[]{"zh_CN.GBK"};

	if(!std::setlocale(LC_ALL, locale_str))
		throw LoggedEvent("Call of std::setlocale() with %s failed.\n",
			locale_str);
#endif
}

ValueNode
InitializeInstalled()
{
	puts("Checking installation...");
	try
	{
		auto node(LoadConfiguration(true));

		if(node.GetName() == "YFramework")
			node = PackNodes("", std::move(node));
		LoadComponents(node.GetNode("YFramework"));
		puts("OK!");
		return std::move(node);
	}
	catch(std::exception& e)
	{
		std::printf("Error occurred: %s\n", e.what());
	}
	throw FatalError("      Invalid Installation      ",
		" Please make sure the data is\n"
		" stored in correct directory.\n");
}

void
InitializeSystemFontCache(const string& fong_file, const string& font_dir)
{
	puts("Loading font files...");
	try
	{
		auto& fc(FetchDefaultFontCache());
		size_t nFileLoaded(fc.LoadTypefaces(fong_file) != 0);

		if(!font_dir.empty())
			//读取字体文件目录并载入目录下指定后缀名的字体文件。
			if(HFileNode dir{font_dir.c_str()})
				while((++dir).LastError == 0)
					if(std::strcmp(dir.GetName(), FS_Now) != 0
						&& !dir.IsDirectory()
						/*&& IsExtensionOf(ext, dir.GetName())*/)
					{
						FontPath path(font_dir + dir.GetName());

						if(path != fong_file)
							nFileLoaded += fc.LoadTypefaces(path) != 0;
					}
		fc.InitializeDefaultTypeface();
		if(const auto nFaces = FetchDefaultFontCache().GetFaces().size())
			std::printf("%u face(s) in %u font file(s)"
				" are loaded\nsuccessfully.\n", nFaces, nFileLoaded);
		else
			throw LoggedEvent("No fonts found.");
		puts("Setting default font face...");
		if(const auto* const pf = fc.GetDefaultTypefacePtr())
			std::printf("\"%s\":\"%s\",\nsuccessfully.\n",
				pf->GetFamilyName().c_str(), pf->GetStyleName().c_str());
		else
			throw LoggedEvent("Setting default font face failed.");
		return;
	}
	// TODO: Use %std::nested_exception.
	catch(std::exception& e)
	{
		puts(e.what());
	}
	throw FatalError("      Font Caching Failure      ",
		" Please make sure the fonts are\n"
		" stored in correct path.\n");
}

void
Uninitialize() ynothrow
{
#if !CHRLIB_NODYNAMIC_MAPPING
	delete p_mapped;
#endif
}

YSL_END

