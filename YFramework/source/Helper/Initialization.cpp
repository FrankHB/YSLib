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
\version r2421
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-10-21 23:15:08 +0800
\par 修改时间:
	2015-10-19 16:27 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::Initialization
*/


#include "Helper/YModules.h"
#include YFM_Helper_Initialization // for ystdex::handle_nested;
#include YFM_YSLib_Core_YApplication
#include YFM_Helper_GUIApplication
#include YFM_YCLib_Debug
#include YFM_CHRLib_MappingEx
#include YFM_YCLib_MemoryMapping
#include YFM_YSLib_Service_FileSystem
#include <ystdex/string.hpp> // for ystdex::sfmt;
#include <cerrno> // for errno;
//#include <clocale>
#if YCL_DS
#	include YFM_DS_YCLib_DSVideo // for platform_ex::DSConsoleInit;
#elif YCL_Android
#	include <unistd.h> // for F_OK;
#elif YCL_Win32
#	include YFM_Win32_YCLib_NLS
#endif
#include YFM_NPL_SContext

using namespace ystdex;
using namespace platform;
//! \since build 600
using namespace NPL;

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
#if YCL_DS
// \since build 608
bool at_init(true);
#	define YF_Init_printf(_lv, ...) (at_init ? std::printf(__VA_ARGS__) : 0)
#	define YF_Init_puts(_lv, _str) (at_init ? std::puts(_str) : 0)
#elif YCL_Android
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
#if !CHRLib_NoDynamicMapping
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
		[&](const string& str, RecordLevel, size_t level){
		res += string(level, ' ') + "ERROR: " + str + '\n';
	}, e);
}

#if YCL_Win32
//! \since build 641
char16_t(*cp113_lkp_backup)(byte, byte);
//! \since build 552
//@{
const unsigned short* p_dbcs_off_936;

void
LoadCP936_NLS()
{
	using namespace platform_ex;

	if((p_dbcs_off_936 = FetchDBCSOffset(936)))
	{
		cp113_lkp_backup = CHRLib::cp113_lkp;
		CHRLib::cp113_lkp = [](byte seq0, byte seq1) ynothrowv -> char16_t{
			return p_dbcs_off_936[p_dbcs_off_936[seq0] + seq1];
		};
	}
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
#if !CHRLib_NoDynamicMapping

	const string mapping_name(data_dir + "cp113.bin");

	YF_Init_printf(Notice, "Loading character mapping file '%s' ...\n",
		mapping_name.c_str());
	try
	{
		p_mapped = LoadMappedModule(data_dir + "cp113.bin");
		if(p_mapped->GetSize() != 0)
			CHRLib::cp113 = p_mapped->GetPtr();
		else
			throw GeneralEvent("Failed loading CHRMapEx.");
		YF_Init_puts(Notice, "CHRMapEx loaded successfully.");
	}
	catch(std::exception&)
	{
		YTraceDe(Notice, "Module cp113.bin loading failed.");
#	if YCL_Win32
		LoadCP936_NLS();
		if(p_dbcs_off_936)
			YTraceDe(Notice, "NLS CP936 used as fallback.");
		else
#	endif
		{
			CHRLib::cp113_lkp = [](byte, byte) YB_ATTR(noreturn) -> char16_t{
				throw LoggedEvent("Failed calling conversion for CHRMapEx.");
			};
			YF_Init_puts(Warning, "CHRMapEx conversion calls would lead to"
				" exception thrown.");
		}
	}
#endif
	YF_Init_printf(Notice, "Trying entering directory '%s' ...\n",
		data_dir.c_str());
	if(!IO::VerifyDirectory(data_dir))
		throw GeneralEvent("Invalid default data directory found.");
	if(!(ufexists(font_path.c_str()) || IO::VerifyDirectory(font_dir)))
		throw GeneralEvent("Invalid default font file path found.");
}

YB_NONNULL(1, 2) bool
CreateDefaultNPLA1File(const char* disp, const char* path,
	ValueNode(*creator)(), bool show_info)
{
	YAssertNonnull(disp),
	YAssertNonnull(path);
	if(show_info)
		YF_Init_printf(Notice, "Creating %s '%s'...\n", disp, path);
	if(creator)
	{
		YTraceDe(Debug, "Creator found.");
		if(ofstream ofs{path, std::ios_base::out | std::ios_base::trunc})
			ystdex::write_literal(ofs, Text::BOM_UTF_8)
				<< NPL::Configuration(creator());
		else
			throw GeneralEvent(ystdex::sfmt("Cannot create file,"
				" error = %d: %s.", errno, std::strerror(errno)));
		YTraceDe(Debug, "Created configuration.");
		return {};
	}
	return true;
}

} // unnamed namespace;

void
HandleFatalError(const FatalError& e) ynothrow
{
#if YCL_DS
	platform_ex::DSConsoleInit({}, ColorSpace::White, ColorSpace::Blue);

	const char* line("--------------------------------");

	YF_Init_printf(Emergent, "%s%s%s\n%s\n%s", line, e.GetTitle(), line,
		e.GetContent().data(), line);
#else
#	if YCL_Win32
	using platform_ex::MBCSToWCS;

	FilterExceptions([&]{
		::MessageBoxW({}, MBCSToWCS(e.GetContent()).c_str(),
			MBCSToWCS(e.GetTitle()).c_str(), MB_ICONERROR);
	});
#	endif
	YF_Init_printf(Emergent, "%s\n%s\n", e.GetTitle(), e.GetContent().data());
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
		if(CreateDefaultNPLA1File(disp, path, creator, show_info))
			return {};
	}
	if(show_info)
		YF_Init_printf(Notice, "Found %s '%s'.\n", Nonnull(disp), path);
	if(ifstream ifs{path, std::ios_base::in})
	{
		array<char, 3> buf;

		YTraceDe(Debug, "Found accessible configuration file.");
		ifs.read(buf.data(), 3);
		if(Text::CheckBOM(buf.data(), Text::BOM_UTF_8))
		{
			NPL::Configuration conf;

			ifs >> conf;
			YTraceDe(Debug, "Plain configuration loaded.");
			if(!conf.GetNodeRRef().empty())
				return conf.GetNodeRRef();
			YTraceDe(Warning, "Empty configuration found.");
		}
		else
			throw GeneralEvent("Wrong encoding of configuration file.");
	}
	throw GeneralEvent("Invalid file found when reading configuration.");
}

ValueNode
LoadConfiguration(bool show_info)
{
	return LoadNPLA1File("configuration file", CONF_PATH, []{
		return ValueNode(NodeLiteral{"YFramework",
			{{"DataDirectory", DATA_DIRECTORY}, {"FontFile", DEF_FONT_PATH},
			{"FontDirectory", DEF_FONT_DIRECTORY}}});
	}, show_info);
}

void
SaveConfiguration(const ValueNode& node)
{
	if(ofstream ofs{CONF_PATH, std::ios_base::out | std::ios_base::trunc})
	{
		YTraceDe(Debug, "Writing configuration...");
		ystdex::write_literal(ofs, Text::BOM_UTF_8)
			<< NPL::Configuration(ValueNode(node.GetContainerRef()));
	}
	else
		throw GeneralEvent("Invalid file found when writing configuration.");
	YTraceDe(Debug, "Writing configuration done.");
}


void
InitializeEnvironment()
{
	std::set_terminate(terminate);
#if YCL_DS
	::powerOn(POWER_ALL);
	::defaultExceptionHandler();
	platform_ex::DSConsoleInit(true, ColorSpace::Lime);
	if(!platform_ex::InitializeFileSystem())
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
#endif
#if 0
	// TODO: Review locale APIs compatibility.
	static yconstexpr const char locale_str[]{"zh_CN.GBK"};

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
			node = PackNodes(string(), std::move(node));
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

				std::for_each(FileIterator(&dir), FileIterator(),
					[&](const string& name){
					if(!PathTraits::is_self(name)
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
	YF_Init_printf(Notice, "Uninitialization entered with %zu handler(s) to be"
		" called.\n", app_exit.size());
	while(!app_exit.empty())
	{
		if(YB_LIKELY(app_exit.top()))
			app_exit.top()();
		app_exit.pop();
	}
#if YCL_DS
	// XXX: Ignored error.
	platform_ex::UninitializeFileSystem();
#endif
#if !CHRLib_NoDynamicMapping
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
#if YCL_DS
	// XXX: Actually this should be set after %InitVideo call.
	at_init = {};
#endif
	return *p_font_cache;
}

MIMEBiMapping&
FetchMIMEBiMapping()
{
	if(YB_UNLIKELY(!p_mapping))
	{
		p_mapping = ynew MIMEBiMapping();
		AddMIMEItems(*p_mapping, LoadNPLA1File("MIME database",
			(AccessChild<string>(FetchRoot()["YFramework"], "DataDirectory")
			+ "MIMEExtMap.txt").c_str(), []{
			return
				NPL::LoadNPLA1(NPL::SContext::Analyze(NPL::Session(TU_MIME)));
			}, true));
		app_exit.push([]() ynothrow{
			ydelete(p_mapping);
		});
	}
	return *p_mapping;
}

} // namespace YSLib;

