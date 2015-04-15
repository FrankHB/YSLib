/*
	© 2009-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Initialization.cpp
\ingroup Helper
\brief 程序启动时的通用初始化。
\version r2216
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-10-21 23:15:08 +0800
\par 修改时间:
	2015-04-13 02:42 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::Initialization
*/


#include "Helper/YModules.h"
#include YFM_Helper_Initialization
#include YFM_YSLib_Core_YApplication
#include YFM_Helper_GUIApplication
#include YFM_YCLib_Debug
#include YFM_CHRLib_MappingEx
#include YFM_YCLib_MemoryMapping
#include YFM_YSLib_Service_FileSystem
#include <ystdex/string.hpp> // for ystdex::sfmt;
#include <ystdex/exception.h> // for ystdex::handle_nested;
#include <cerrno> // for errno;
//#include <clocale>
#if YCL_DS
#	include YFM_DS_YCLib_DSVideo // for platform_ex::DSInitConsole;
#elif YCL_Android
#	include <unistd.h> // for ::access, F_OK;
#elif YCL_Win32
#	include YFM_MinGW32_YCLib_NLS
#endif
#include YFM_NPL_SContext

using namespace ystdex;
using namespace platform;

namespace YSLib
{

using namespace Drawing;
using namespace IO;

namespace
{

/*!
\def YF_Init_printf
\brief 初始化时使用的格式化输出函数。
\since build 505
*/
/*!
\def YF_Init_puts
\brief 初始化时使用的行输出函数。
\since build 505
*/
#if YCL_Android
#	define YF_Init_printf(_lv, ...) YTraceDe(_lv, __VA_ARGS__)
#	define YF_Init_puts(_lv, _str) YTraceDe(_lv, _str)
#else
#	define YF_Init_printf(_lv, ...) std::printf(__VA_ARGS__)
#	define YF_Init_puts(_lv, _str) std::puts(_str)
#endif

//! \since build 549
using platform::MappedFile;
//! \since build 425
//@{
stack<std::function<void()>> app_exit;
//! \since build 551
unique_ptr<ValueNode> p_root;
//! \since build 551
unique_ptr<Drawing::FontCache> p_font_cache;
//@}
#if !CHRLIB_NODYNAMIC_MAPPING
//! \since build 549
unique_ptr<MappedFile> p_mapped;
#endif
//! \since build 450
MIMEBiMapping* p_mapping;
//! \since build 450
const char TU_MIME[]{u8R"NPLA1(
(application
	(octet-stream "bin" "so")
	(x-msdownload "exe" "dll" "com" "bat" "msi")
	(x-font-ttf "ttf" "ttc")
	(xml "xml")
	(zip "zip")
)
(audio
	(midi "mid" "midi" "rmi")
	(mpeg "mp3")
	(x-mpegurl "m3u")
	(x-wav "wav")
)
(image
	(bmp "bmp")
	(gif "gif")
	(jpeg "jpeg" "jpg")
	(png "png")
	(tif "tif" "tiff")
)
(text
	(html "html" "htm")
	(plain "txt" "conf" "def" "ini" "log" "in")
	(x-c "c" "h" "inl")
	(x-c++ "cc" "cpp" "cxx" "h" "hh" "hpp" "hxx" "inl")
)
)NPLA1"};

#if YCL_DS
#	define ROOTW "/"
#	define DATA_DIRECTORY ROOTW "Data/"
#	define DEF_FONT_PATH ROOTW "Font/FZYTK.TTF"
#	define DEF_FONT_DIRECTORY ROOTW "Font/"
#	define CONF_PATH "yconf.txt"
#elif YCL_MinGW
#	define ROOTW ".\\"
#	define DATA_DIRECTORY ROOTW
#	define DEF_FONT_PATH "C:\\Windows\\Fonts\\SimSun.ttc"
#	define DEF_FONT_DIRECTORY ROOTW
#	define CONF_PATH "yconf.txt"
#elif YCL_Android
//! \since build 506
string
FetchWorkingRoot_Android()
{
	static struct Init
	{
		string Path;

		Init()
			: Path([]{
				const char*
					sd_paths[]{"/sdcard/", "/mnt/sdcard/", "/storage/sdcard0/"};

				for(const auto& path : sd_paths)
					if(uaccess(path, F_OK) == 0)
					{
						YTraceDe(Informative, "Successfully found SD card path"
							" '%s' as root path.", path);
						return path;
					}
					else
						YTraceDe(Informative,
							"Failed accessing SD card path '%s'.", path);
				throw GeneralEvent("Failed finding working root path.");
			}())
		{}
	} init;

	return init.Path;
}

//! \since build 506
inline string
FetchDataDirectory_Android()
{
	return FetchWorkingRoot_Android() + "Data/";
}
#	define ROOTW FetchWorkingRoot_Android()
#	define DATA_DIRECTORY FetchDataDirectory_Android()
#	define DEF_FONT_PATH "/system/fonts/DroidSansFallback.ttf"
#	define DEF_FONT_DIRECTORY "/system/fonts/"
#	define CONF_PATH (FetchWorkingRoot_Android() + "yconf.txt").c_str()
#elif YCL_Linux
#	define ROOTW "./"
#	define DATA_DIRECTORY ROOTW
#	define DEF_FONT_PATH ROOTW "SimSun.ttc"
#	define DEF_FONT_DIRECTORY ROOTW
#	define CONF_PATH "yconf.txt"
#else
#	error "Unsupported platform found."
#endif

//! \since build 549
unique_ptr<MappedFile>
LoadMappedModule(const string& path)
{
	TryRet(make_unique<MappedFile>(path))
	CatchExpr(..., std::throw_with_nested(
		GeneralEvent("Loading module '" + path + "' failed.")))
	YAssert(false, "Unreachable control found.");
}

//! \since build 591
void
Extract(const std::exception& e, string& res) ynothrow
{
	ExtractException(
		[&](const string& str, LoggedEvent::LevelType, size_t level){
		res += string(level, ' ') + "ERROR: " + str + '\n';
	}, e);
}

#if YCL_Win32
//! \since build 552
//@{

ucs2_t(*cp113_lkp_backup)(byte, byte);
const unsigned short* p_dbcs_off_936;

void
LoadCP936_NLS()
{
	using namespace platform_ex;

	p_dbcs_off_936 = FetchDBCSOffset(936);
	cp113_lkp_backup = CHRLib::cp113_lkp;
	CHRLib::cp113_lkp = [](byte seq0, byte seq1)->ucs2_t{
		return p_dbcs_off_936[p_dbcs_off_936[seq0] + seq1];
	};
}
//@}
#endif

void
LoadComponents(const ValueNode& node)
{
	const auto& data_dir(AccessChild<string>(node, "DataDirectory"));
	const auto& font_path(AccessChild<string>(node, "FontFile"));
	const auto& font_dir(AccessChild<string>(node, "FontDirectory"));

	if(!data_dir.empty() && !font_path.empty() && !font_dir.empty())
		YF_Init_printf(Notice, "Loaded default directory:\n%s\n"
			"Loaded default font path:\n%s\n"
			"Loaded default font directory:\n%s\n",
			data_dir.c_str(), font_path.c_str(), font_dir.c_str());
	else
		throw GeneralEvent("Empty path loaded.");
#if !CHRLIB_NODYNAMIC_MAPPING

	const string mapping_name(data_dir + "cp113.bin");

	YF_Init_printf(Notice, "Loading character mapping file '%s' ...\n",
		mapping_name.c_str());

#if YCL_Win32
	try
	{
#endif
		p_mapped = LoadMappedModule(data_dir + "cp113.bin");
		if(p_mapped->GetSize() != 0)
			CHRLib::cp113 = p_mapped->GetPtr();
		else
			throw GeneralEvent("Failed loading CHRMapEx.");
#if YCL_Win32
	}
	catch(std::exception&)
	{
		LoadCP936_NLS();
		YTraceDe(Notice,
			"Module cp113.bin loading failed, NLS CP936 used as fallback.");
	}
#endif
	YF_Init_puts(Notice, "CHRMapEx loaded successfully.");
#endif
	YF_Init_printf(Notice, "Trying entering directory '%s' ...\n",
		data_dir.c_str());
	if(!IO::VerifyDirectory(data_dir))
		throw GeneralEvent("Invalid default data directory found.");
	if(!(ufexists(font_path) || IO::VerifyDirectory(font_dir)))
		throw GeneralEvent("Invalid default font file path found.");
}

} // unnamed namespace;

void
HandleFatalError(const FatalError& e) ynothrow
{
#if YCL_DS
	platform_ex::DSConsoleInit({}, ColorSpace::White, ColorSpace::Blue);

	const char* line("--------------------------------");

	YF_Init_printf(Emergent, "%s%s%s\n%s\n%s\n", line, e.GetTitle(), line,
		e.GetContent().c_str(), line);
#else
#	if YCL_Win32
	using platform_ex::MBCSToWCS;

	TryExpr(::MessageBoxW({}, MBCSToWCS(e.GetContent()).c_str(),
		MBCSToWCS(e.GetTitle()).c_str(), MB_ICONERROR))
	CatchIgnore(...)
#	endif
	YF_Init_printf(Emergent, "%s\n%s\n", e.GetTitle(), e.GetContent().c_str());
#endif
	terminate();
}


ValueNode
LoadNPLA1File(const char* disp, const char* path, ValueNode(*creator)(),
	bool show_info)
{
	if(!ufexists(path))
	{
		YTraceDe(Debug, "Path '%s' access failed.", path);
		if(show_info)
			YF_Init_printf(Notice, "Creating %s '%s'...\n", disp, path);
		if(creator)
		{
			YTraceDe(Debug, "Creator found.");
			if(TextFile tf{path, std::ios_base::out | std::ios_base::trunc})
				tf << NPL::Configuration(creator());
			else
				throw GeneralEvent(ystdex::sfmt("Cannot create file,"
					" error = %d: %s.", errno, std::strerror(errno)));
			YTraceDe(Debug, "Created configuration.");
		}
		else
			return {};
	}

	TextFile tf(path);

	if(show_info)
		YF_Init_printf(Notice, "Found %s '%s'.\n", disp, path);
	return ReadConfiguration(tf);
}


ValueNode
ReadConfiguration(TextFile& tf)
{
	if(YB_LIKELY(tf))
	{
		YTraceDe(Debug, "Found accessible configuration file.");
		if(YB_UNLIKELY(tf.Encoding != Text::CharSet::UTF_8))
			throw GeneralEvent("Wrong encoding of configuration file.");

		NPL::Configuration conf;

		tf >> conf;
		YTraceDe(Debug, "Plain configuration loaded.");
		if(conf.GetNodeRRef().GetSize() != 0)
			return conf.GetNodeRRef();
		YTraceDe(Warning, "Empty configuration found.");
	}
	throw GeneralEvent("Invalid file found when reading configuration.");
}

void
WriteConfiguration(TextFile& tf, const ValueNode& node)
{
	if(YB_UNLIKELY(!tf))
		throw GeneralEvent("Invalid file found when writing configuration.");
	YTraceDe(Debug, "Writing configuration...");
	tf << NPL::Configuration(ValueNode(node.GetContainerRef()));
	YTraceDe(Debug, "Writing configuration done.");
}

ValueNode
LoadConfiguration(bool show_info)
{
	return LoadNPLA1File("configuration file", CONF_PATH, []{
		return PackNodes("YFramework", MakeNode("DataDirectory",
			string(DATA_DIRECTORY)), MakeNode("FontFile",
			string(DEF_FONT_PATH)), MakeNode("FontDirectory",
			string(DEF_FONT_DIRECTORY)));
	}, show_info);
}

void
SaveConfiguration(const ValueNode& node)
{
	TextFile tf(CONF_PATH, std::ios_base::out | std::ios_base::trunc);

	WriteConfiguration(tf, node);
}


void
InitializeEnvironment()
{
	//设置默认异常终止函数。
	std::set_terminate(terminate);
#if YCL_DS
	//启用设备。
	::powerOn(POWER_ALL);

	//启用 LibNDS 默认异常处理。
	::defaultExceptionHandler();

	//初始化主控制台。
	platform_ex::DSConsoleInit(true, ColorSpace::Lime);

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
		throw GeneralEvent("Call of std::setlocale() with %s failed.\n",
			locale_str);
#endif
}

ValueNode
InitializeInstalled()
{
	string res;

	YF_Init_puts(Notice, "Checking installation...");
	try
	{
		auto node(LoadConfiguration(true));

		if(node.GetName() == "YFramework")
			node = PackNodes("", std::move(node));
		LoadComponents(node.at("YFramework"));
		YF_Init_puts(Notice, "OK!");
		return node;
	}
	CatchExpr(std::exception& e, Extract(e, res))
	CatchExpr(..., res += "Unknown exception @ InitializeInstalled.\n")
	throw FatalError("      Invalid Installation      ",
		" Please make sure the data is\n"
		" stored in correct directory.\n" + res);
}

void
InitializeSystemFontCache(FontCache& fc, const string& fong_file,
	const string& font_dir)
{
	string res;

	YF_Init_puts(Notice, "Loading font files...");
	try
	{
		size_t nFileLoaded(fc.LoadTypefaces(fong_file) != 0);

		if(!font_dir.empty())
			//读取字体文件目录并载入目录下指定后缀名的字体文件。
			try
			{
				HDirectory dir{font_dir.c_str()};
				IO::PathNorm nm;

				std::for_each(FileIterator(&dir), FileIterator(),
					[&](const std::string& name){
					if(!nm.is_self(name)
						&& dir.GetNodeCategory() != NodeCategory::Directory
						/*&& IsExtensionOf(ext, dir.GetName())*/)
					{
						FontPath path(font_dir + dir.GetName());

						if(path != fong_file)
							nFileLoaded += fc.LoadTypefaces(path) != 0;
					}
				});
			}
			CatchIgnore(FileOperationFailure&)
		fc.InitializeDefaultTypeface();
		if(const auto nFaces = fc.GetFaces().size())
			YF_Init_printf(Notice, "%zu face(s) in %zu font file(s)"
				" are loaded\nsuccessfully.\n", nFaces, nFileLoaded);
		else
			throw GeneralEvent("No fonts found.");
		YF_Init_puts(Notice, "Setting default font face...");
		if(const auto* const pf = fc.GetDefaultTypefacePtr())
			YF_Init_printf(Notice, "\"%s\":\"%s\",\nsuccessfully.\n",
				pf->GetFamilyName().c_str(), pf->GetStyleName().c_str());
		else
			throw GeneralEvent("Setting default font face failed.");
		return;
	}
	CatchExpr(std::exception& e, Extract(e, res))
	CatchExpr(..., res += "Unknown exception @ InitializeSystemFontCache.\n")
	throw FatalError("      Font Caching Failure      ",
		" Please make sure the fonts are\n"
		" stored in correct path.\n" + res);
}

void
Uninitialize() ynothrow
{
	YF_Init_printf(Notice, "Uninitialization entered with"
		" %u handler(s) to be called.\n", unsigned(app_exit.size()));
	while(!app_exit.empty())
	{
		if(YB_LIKELY(app_exit.top()))
			app_exit.top()();
		app_exit.pop();
	}
#if !CHRLIB_NODYNAMIC_MAPPING
#	if YCL_Win32
	if(cp113_lkp_backup)
	{
		CHRLib::cp113_lkp = cp113_lkp_backup;
		cp113_lkp_backup = {};
	}
#	endif
	p_mapped.reset();
	YF_Init_puts(Notice, "Character mapping deleted.");
#endif
}


ValueNode&
FetchRoot()
{
	if(YB_UNLIKELY(!p_root))
	{
		p_root.reset(new ValueNode(InitializeInstalled()));
		app_exit.push([]{
			p_root.reset();
		});
	}
	return *p_root;
}

Drawing::FontCache&
FetchDefaultFontCache()
{
	if(YB_UNLIKELY(!p_font_cache))
	{
		p_font_cache.reset(new Drawing::FontCache());
		app_exit.push([]{
			p_font_cache.reset();
		});

		const auto& node(FetchRoot()["YFramework"]);

		InitializeSystemFontCache(*p_font_cache,
			AccessChild<string>(node, "FontFile"),
			AccessChild<string>(node, "FontDirectory"));
	}
	return *p_font_cache;
}

MIMEBiMapping&
FetchMIMEBiMapping()
{
	if(YB_UNLIKELY(!p_mapping))
	{
		p_mapping = ynew MIMEBiMapping;
		app_exit.push([]{
			ydelete(p_mapping);
		});
		AddMIMEItems(*p_mapping, LoadNPLA1File("MIME database",
			(AccessChild<string>(FetchRoot()["YFramework"], "DataDirectory")
			+ "MIMEExtMap.txt").c_str(), []{
			return
				NPL::LoadNPLA1(NPL::SContext::Analyze(NPL::Session(TU_MIME)));
		}, true));
	}
	return *p_mapping;
}

} // namespace YSLib;

