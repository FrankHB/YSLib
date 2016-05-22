/*
	© 2009-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Initialization.cpp
\ingroup Helper
\brief 程序启动时的通用初始化。
\version r2940
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-10-21 23:15:08 +0800
\par 修改时间:
	2016-05-23 05:08 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::Initialization
*/


#include "Helper/YModules.h"
#include YFM_Helper_Initialization
#include YFM_YCLib_MemoryMapping // for MappedFile;
#include YFM_YSLib_Core_YException // for ExtractException;
#include YFM_CHRLib_MappingEx // for CHRLib::cp113_lkp;
#include YFM_YSLib_Service_TextFile // for Text::BOM_UTF_8, Text::CheckBOM;
#include YFM_NPL_Configuration // for NPL::Configuration;
#include YFM_Helper_Environment // for Environment::AddExitGuard;
#include <ystdex/string.hpp> // for ystdex::write_literal, ystdex::sfmt;
#include <cerrno> // for errno;
#include YFM_YSLib_Service_FileSystem // for IO::TraverseChildren;
#include YFM_Helper_GUIApplication // for FetchEnvironment;
//#include <clocale>
#if YCL_Android
#	include <unistd.h> // for F_OK;
#elif YCL_Win32
#	include YFM_Win32_YCLib_NLS // for platform_ex::FetchDBCSOffset;
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

#if YCL_DS
bool ShowInitializedLog(true);
#endif

namespace
{

//! \since build 551
unique_ptr<ValueNode> p_root;
//! \since build 450
yconstexpr const char TU_MIME[]{u8R"NPLA1(
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
#elif YCL_Win32
#	define ROOTW FetchExecutableImageDirectory_Win32().c_str()
#	define DATA_DIRECTORY ROOTW
#	define DEF_FONT_PATH (FetchSystemFontDirectory_Win32() + "SimSun.ttc").c_str()
#	define DEF_FONT_DIRECTORY ROOTW
// TODO: Reduce overhead?
#	define CONF_PATH (FetchExecutableImageDirectory_Win32() + "yconf.txt").c_str()

//! \since build 694
string
FetchExecutableImageDirectory_Win32()
{
	// XXX: Pedantic.
	// TODO: Reuse the string literal for other functions?
	const auto sp(L"/\\");
	const auto image(ystdex::rtrim(platform_ex::FetchModuleFileName(), sp));
	wstring_view sv(image);
	const auto epos(sv.find_last_of(sp));

	// TODO: Simplify?
	if(epos != wstring_view::npos)
		sv.remove_suffix(sv.size() - epos - 1);

	// XXX: Pedantic.
	ystdex::rtrim(sv, sp);
	return platform_ex::WCSToMBCS(sv) + '\\';
}
//! \since build 693
inline PDefH(string, FetchSystemFontDirectory_Win32, )
	// NOTE: Hard-coded as Shell32 special path with %CSIDL_FONTS or
	//	%CSIDL_FONTS. See https://msdn.microsoft.com/en-us/library/dd378457.aspx.
	ImplRet(platform_ex::WCSToMBCS(platform_ex::FetchWindowsPath()) + "Fonts\\")
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
inline PDefH(string, FetchDataDirectory_Android, )
	ImplRet(FetchWorkingRoot_Android() + "Data/")
#	define ROOTW FetchWorkingRoot_Android()
#	define DATA_DIRECTORY FetchDataDirectory_Android()
#	define DEF_FONT_PATH "/system/fonts/DroidSansFallback.ttf"
#	define DEF_FONT_DIRECTORY "/system/fonts/"
// TODO: Reduce overhead?
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

//! \since build 693
void
CreateDefaultNPLA1ForStream(std::ostream& os, ValueNode(*creator)())
{
	ystdex::write_literal(os, Text::BOM_UTF_8)
		<< NPL::Configuration(creator());
}

ValueNode
TryReadNPLStream(std::istream& is)
{
	array<char, 3> buf;

	is.read(buf.data(), 3);
	if(Text::CheckBOM(buf.data(), Text::BOM_UTF_8))
	{
		NPL::Configuration conf;

		is >> conf;
		YTraceDe(Debug, "Plain configuration loaded.");
		if(!conf.GetNodeRRef().empty())
			return conf.GetNodeRRef();
		YTraceDe(Warning, "Empty configuration found.");
		throw GeneralEvent("Invalid file found when reading configuration.");
	}
	else
		throw GeneralEvent("Wrong encoding of configuration file.");
}

YB_NONNULL(1, 2) bool
CreateDefaultNPLA1File(const char* disp, const char* path,
	ValueNode(*creator)(), bool show_info)
{
	YAssertNonnull(disp),
	YAssertNonnull(path);
	if(show_info)
		YTraceDe(Notice, "Creating %s '%s'...\n", disp, path);
	if(creator)
	{
		YTraceDe(Debug, "Creator found.");
		if(ofstream ofs{path, std::ios_base::out | std::ios_base::trunc})
			CreateDefaultNPLA1ForStream(ofs, creator);
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
ExtractInitException(const std::exception& e, string& res) ynothrow
{
	ExtractException(
		[&](const string& str, RecordLevel, size_t level){
		res += string(level, ' ') + "ERROR: " + str + '\n';
	}, e);
}

void
TraceForOutermost(const std::exception& e, RecordLevel lv) ynothrow
{
#if YCL_DS
	ShowInitializedLog = {};
#endif
	if(const auto p = dynamic_cast<const YSLib::FatalError*>(&e))
	{
#if YCL_Win32
		using platform_ex::MBCSToWCS;

		FilterExceptions([&]{
			::MessageBoxW({}, MBCSToWCS(p->GetContent()).c_str(),
				MBCSToWCS(p->GetTitle()).data(), MB_ICONERROR);
		});
#endif
		YTraceDe(Emergent, "%s\n\n%s", p->GetTitle(), p->GetContent().data());
		terminate();
	}
	else
		ExtractAndTrace(e, lv);
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
		YTraceDe(Notice, "Found %s '%s'.\n", Nonnull(disp), path);

	auto res(TryInvoke([&]() -> ValueNode{
		if(ifstream ifs{path})
		{
			YTraceDe(Debug, "Found accessible configuration file.");
			return TryReadNPLStream(ifs);
		}
		return {};
	}));

	if(res)
		return res;

	YTraceDe(Warning, "Newly created configuration corrupted,"
		" trying fallback in memory...");

	std::stringstream oss;

	CreateDefaultNPLA1ForStream(oss, creator);
	return TryReadNPLStream(oss);
}

void
LoadComponents(Application& app, const ValueNode& node)
{
	const auto& data_dir(AccessChild<string>(node, "DataDirectory"));
	const auto& font_path(AccessChild<string>(node, "FontFile"));
	const auto& font_dir(AccessChild<string>(node, "FontDirectory"));

	if(!data_dir.empty() && !font_path.empty() && !font_dir.empty())
		YTraceDe(Notice, "Loaded default directory:\n%s\n"
			"Loaded default font path:\n%s\n"
			"Loaded default font directory:\n%s\n",
			data_dir.c_str(), font_path.c_str(), font_dir.c_str());
	else
		throw GeneralEvent("Empty path loaded.");
#if !CHRLib_NoDynamicMapping

	static unique_ptr<MappedFile> p_mapped;
	const string mapping_name(data_dir + "cp113.bin");

	YTraceDe(Notice, "Loading character mapping file '%s' ...\n",
		mapping_name.c_str());
	try
	{
		p_mapped = LoadMappedModule(data_dir + "cp113.bin");
		if(p_mapped->GetSize() != 0)
			CHRLib::cp113 = p_mapped->GetPtr();
		else
			throw GeneralEvent("Failed loading CHRMapEx.");
		YTraceDe(Notice, "CHRMapEx loaded successfully.");
	}
	catch(std::exception& e)
	{
		YTraceDe(Notice, "Module cp113.bin loading failed, error: %s",
			e.what());
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
			YTraceDe(Warning, "CHRMapEx conversion calls would lead to"
				" exception thrown.");
		}
	}
	app.AddExitGuard([]() ynothrow{
#	if YCL_Win32
		if(cp113_lkp_backup)
		{
			CHRLib::cp113_lkp = cp113_lkp_backup;
			cp113_lkp_backup = {};
		}
#	endif
		p_mapped.reset();
		YTraceDe(Notice, "Character mapping deleted.");
	});
#endif
	YTraceDe(Notice, "Trying entering directory '%s' ...\n",
		data_dir.c_str());
	if(!IO::VerifyDirectory(data_dir))
		throw GeneralEvent("Invalid default data directory found.");
	if(!(ufexists(font_path.c_str()) || IO::VerifyDirectory(font_dir)))
		throw GeneralEvent("Invalid default font file path found.");
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
			<< NPL::Configuration(ValueNode(node.GetContainer()));
	}
	else
		throw GeneralEvent("Invalid file found when writing configuration.");
	YTraceDe(Debug, "Writing configuration done.");
}


void
InitializeSystemFontCache(FontCache& fc, const string& font_file,
	const string& font_dir)
{
	string res;

	YTraceDe(Notice, "Loading font files...");
	try
	{
		size_t loaded(fc.LoadTypefaces(font_file) != 0 ? 1 : 0);

		if(!font_dir.empty())
			try
			{
				IO::TraverseChildren(font_dir,
					[&](NodeCategory c, NativePathView npv){
					if(!bool(c & NodeCategory::Directory))
					{
						const FontPath path(font_dir + String(npv).GetMBCS());

						if(path != font_file && fc.LoadTypefaces(path) != 0)
							++loaded;
					}
				});
			}
			CatchIgnore(FileOperationFailure&)
		fc.InitializeDefaultTypeface();

		const auto faces(fc.GetFaces().size());

		if(faces != 0)
			YTraceDe(Notice, "%zu face(s) in %zu font file(s)"
				" are loaded\nsuccessfully.\n", faces, loaded);
		else
			throw GeneralEvent("No fonts found.");
		YTraceDe(Notice, "Setting default font face...");
		if(const auto pf = fc.GetDefaultTypefacePtr())
			YTraceDe(Notice, "\"%s\":\"%s\",\nsuccessfully.\n",
				pf->GetFamilyName().c_str(), pf->GetStyleName().c_str());
		else
			throw GeneralEvent("Setting default font face failed.");
		return;
	}
	CatchExpr(std::exception& e, ExtractInitException(e, res))
	CatchExpr(..., res += "Unknown exception @ InitializeSystemFontCache.\n")
	throw FatalError("      Font Caching Failure      ",
		" Please make sure the fonts are\n"
		" stored in correct path.\n" + res);
}


ValueNode&
FetchRoot() ynothrow
{
	return FetchEnvironment().Root;
}

Drawing::FontCache&
FetchDefaultFontCache()
{
	static unique_ptr<Drawing::FontCache> p_font_cache;

	if(YB_UNLIKELY(!p_font_cache))
	{
		p_font_cache.reset(new Drawing::FontCache());
		FetchAppInstance().AddExitGuard([&]() ynothrow{
			p_font_cache.reset();
		});

		const auto& node(FetchEnvironment().Root["YFramework"]);

		InitializeSystemFontCache(*p_font_cache,
			AccessChild<string>(node, "FontFile"),
			AccessChild<string>(node, "FontDirectory"));
	}
#if YCL_DS
	// XXX: Actually this should be set after %InitVideo call.
	ShowInitializedLog = {};
#endif
	return *p_font_cache;
}

MIMEBiMapping&
FetchMIMEBiMapping()
{
	static unique_ptr<MIMEBiMapping> p_mapping;

	if(YB_UNLIKELY(!p_mapping))
	{
		using namespace NPL;

		p_mapping.reset(new MIMEBiMapping());
		AddMIMEItems(*p_mapping, LoadNPLA1File("MIME database",
			(AccessChild<string>(FetchEnvironment().Root["YFramework"], "DataDirectory")
			+ "MIMEExtMap.txt").c_str(), []{
				return A1::LoadNode(SContext::Analyze(NPL::Session(TU_MIME)));
			}, true));
		FetchAppInstance().AddExitGuard([&]() ynothrow{
			p_mapping.reset();
		});
	}
	return *p_mapping;
}

} // namespace YSLib;

