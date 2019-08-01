/*
	© 2009-2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Initialization.cpp
\ingroup Helper
\brief 框架初始化。
\version r3475
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-10-21 23:15:08 +0800
\par 修改时间:
	2019-08-01 12:58 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::Initialization
*/


#include "Helper/YModules.h"
#include YFM_Helper_Initialization // for ystdex::nptr, function;
#include YFM_YSLib_Core_YException // for ExtractException, ExtractAndTrace;
#include YFM_CHRLib_MappingEx // for CHRLib::cp113_lkp;
#include YFM_YSLib_Service_TextFile // for Text::BOM_UTF_8, Text::CheckBOM;
#include YFM_NPL_Configuration // for NPL::Configuration;
#include <ystdex/string.hpp> // for ystdex::write_literal, ystdex::sfmt;
#include <ystdex/scope_guard.hpp> // for ystdex::swap_guard;
#include <cerrno> // for errno;
#include YFM_YSLib_Service_FileSystem // for IO::TraverseChildren,
//	NativePathView;
#include YFM_Helper_GUIApplication // for FetchEnvironment, FetchAppInstance,
//	Application::AddExit, Application::AddExitGuard;
#include YFM_Helper_Environment // for Environment;
#if YCL_Win32
#	include YFM_Win32_YCLib_NLS // for platform_ex::FetchDBCSOffset,
//	platform_ex::WCSToUTF8, platform_ex::UTF8ToWCS;
#endif

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
yconstexpr const char TU_MIME[]{R"NPLA1(
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

#undef CONF_PATH
#undef DATA_DIRECTORY
#undef DEF_FONT_DIRECTORY
#undef DEF_FONT_PATH
#if YCL_Win32 || YCL_Linux
//! \since build 695
const string&
FetchWorkingRoot()
{
	static const struct Init
	{
		string Path;

		Init()
			: Path([]{
#	if YCL_Win32
				IO::Path image(platform::ucast(
					platform_ex::FetchModuleFileName().data()));

				if(!image.empty())
				{
					image.pop_back();

					const auto& dir(image.Verify());

					if(!dir.empty() && dir.back() == FetchSeparator<char16_t>())
						return dir.GetMBCS();
				}
#	elif YCL_Android
				const char*
					sd_paths[]{"/sdcard/", "/mnt/sdcard/", "/storage/sdcard0/"};

				for(const auto& path : sd_paths)
					if(IO::VerifyDirectory(path))
					{
						YTraceDe(Informative, "Successfully found SD card path"
							" '%s' as root path.", path);
						return path;
					}
					else
						YTraceDe(Informative,
							"Failed accessing SD card path '%s'.", path);
#	elif YCL_Linux
				// FIXME: What if link reading failed (i.e. permission denied)?
				// XXX: Link content like 'node_type:[inode]' is not supported.
				// TODO: Use implemnetation for BSD family OS, etc.
				auto image(IO::ResolvePath<ystdex::path<vector<string>,
					IO::PathTraits>>(string_view("/proc/self/exe")));

				if(!image.empty())
				{
					image.pop_back();

					const auto& dir(IO::VerifyDirectoryPathTail(
						ystdex::to_string_d(image)));

					if(!dir.empty() && dir.back() == FetchSeparator<char>())
						return dir;
				}
#	else
#		error "Unsupported platform found."
#	endif
				throw GeneralEvent("Failed finding working root path.");
			}())
		{
			YTraceDe(Informative, "Initialized root directory path '%s'.",
				Path.c_str());
		}
	} init;

	return init.Path;
}

// TODO: Reduce overhead?
#	define CONF_PATH (FetchWorkingRoot() + "yconf.txt").c_str()
#endif
#if YCL_DS
#	define DATA_DIRECTORY "/Data/"
#	define DEF_FONT_DIRECTORY "/Font/"
#	define DEF_FONT_PATH "/Font/FZYTK.TTF"
#elif YCL_Win32
#	define DATA_DIRECTORY FetchWorkingRoot()
#	define DEF_FONT_PATH (FetchSystemFontDirectory_Win32() + "SimSun.ttc")
//! \since build 693
inline PDefH(string, FetchSystemFontDirectory_Win32, )
	// NOTE: Hard-coded as Shell32 special path with %CSIDL_FONTS or
	//	%CSIDL_FONTS. See https://msdn.microsoft.com/en-us/library/dd378457.aspx.
	ImplRet(platform_ex::WCSToUTF8(platform_ex::FetchWindowsPath()) + "Fonts\\")
#elif YCL_Android
#	define DATA_DIRECTORY (FetchWorkingRoot() + "Data/")
#	define DEF_FONT_DIRECTORY "/system/fonts/"
#	define DEF_FONT_PATH "/system/fonts/DroidSansFallback.ttf"
#elif YCL_Linux
#	define DATA_DIRECTORY FetchWorkingRoot()
#	define DEF_FONT_PATH "./SimSun.ttc"
#else
#	error "Unsupported platform found."
#endif
#ifndef CONF_PATH
#	define CONF_PATH "yconf.txt"
#endif
#ifndef DATA_DIRECTORY
#	define DATA_DIRECTORY "./"
#endif
#ifndef DEF_FONT_DIRECTORY
#	define DEF_FONT_DIRECTORY DATA_DIRECTORY
#endif

#if !CHRLib_NoDynamic_Mapping
//! \since build 861
MappedFile
LoadMappedModule(const std::string& path)
{
	TryRet(MappedFile(path))
	CatchExpr(..., std::throw_with_nested(
		GeneralEvent("Loading module '" + path + "' failed.")))
	YAssert(false, "Unreachable control found.");
}

#	if YCL_Win32
//! \since build 552
const unsigned short* p_dbcs_off_936;
#	endif

//! \since build 725
class CMap final
{
private:
	MappedFile mapped{};
#	if YCL_Win32
	nptr<char16_t(*)(byte, byte)> cp113_lkp_backup = {};
#	endif

public:
	CMap(const string& data_dir)
	{
		const std::string mapping_name(to_std_string(data_dir) + "cp113.bin");

		YTraceDe(Notice, "Loading character mapping file '%s' ...",
			mapping_name.c_str());
		try
		{
			mapped = LoadMappedModule(mapping_name);
			if(mapped.GetSize() != 0)
				CHRLib::cp113 = mapped.GetPtr();
			else
				throw GeneralEvent("Failed loading CHRMapEx.");
			YTraceDe(Notice, "CHRMapEx loaded successfully.");
		}
		catch(std::exception& e)
		{
			YTraceDe(Notice, "Module cp113.bin loading failed, error: %s",
				e.what());
#	if YCL_Win32
			if((p_dbcs_off_936 = platform_ex::FetchDBCSOffset(936)))
			{
				cp113_lkp_backup = CHRLib::cp113_lkp;
				CHRLib::cp113_lkp = [](byte seq0, byte seq1) ynothrowv
					-> char16_t{
					return p_dbcs_off_936[size_t(p_dbcs_off_936[size_t(seq0)])
						+ size_t(seq1)];
				};
			}
			if(p_dbcs_off_936)
				YTraceDe(Notice, "NLS CP936 used as fallback.");
			else
#	endif
			{
				CHRLib::cp113_lkp
					= [](byte, byte) YB_ATTR_LAMBDA(noreturn) -> char16_t{
					throw
						LoggedEvent("Failed calling conversion for CHRMapEx.");
				};
				YTraceDe(Warning, "CHRMapEx conversion calls would lead to"
					" exception thrown.");
			}
		}
	}
	~CMap()
	{
#	if YCL_Win32
		if(cp113_lkp_backup)
			CHRLib::cp113_lkp = cp113_lkp_backup.get();
#	endif
		if(mapped)
		{
			mapped = MappedFile();
			YTraceDe(Notice, "Character mapping deleted.");
		}
	}

	DefDeMoveCtor(CMap)
};
#endif

//! \since build 721
void
WriteNPLA1Stream(std::ostream& os, NPL::Configuration&& conf)
{
	ystdex::write_literal(os, Text::BOM_UTF_8) << std::move(conf);
}

//! \since build 724
ValueNode
TryReadRawNPLStream(std::istream& is)
{
	NPL::Configuration conf;

	is >> conf;
	YTraceDe(Debug, "Plain configuration loaded.");
	if(!conf.GetNodeRRef().empty())
		return conf.GetNodeRRef();
	YTraceDe(Warning, "Empty configuration found.");
	throw GeneralEvent("Invalid stream found when reading configuration.");
}

//! \since build 725
template<typename _type, typename _fLoader, typename _func>
_type&
FetchDefaultResource(_fLoader load, _func f)
{
	static mutex mtx;
	static observer_ptr<_type> p_res;
	lock_guard<mutex> lck(mtx);

	if(YB_UNLIKELY(!p_res))
	{
		// TODO: Simplify?
		const auto p_locked(FetchAppInstance().LockAddExit(load()));

		p_res = make_observer(&f(FetchRoot()["YFramework"], *p_locked));
	}
#if YCL_DS
	// XXX: Actually this should be set after %InitVideo call.
	ShowInitializedLog = {};
#endif
	return *p_res;
}

} // unnamed namespace;

void
InitializeKeyModule(function<void()> f, const char* sig,
	const char* t, string_view sv)
{
	string res;

	try
	{
		f();
		return;
	}
	CatchExpr(std::exception& e, ExtractException(
		[&](const string& str, RecordLevel, size_t level){
		res += string(level, ' ') + "ERROR: " + str + '\n';
	}, e))
	CatchExpr(..., res += string("Unknown exception @ ") + sig + ".\n")
	throw FatalError(t, string(sv) + res);
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
		using platform_ex::UTF8ToWCS;

		FilterExceptions([&]{
			::MessageBoxW({}, UTF8ToWCS(p->GetContent()).c_str(),
				UTF8ToWCS(p->GetTitle()).data(), MB_ICONERROR);
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
	auto res(TryInvoke([=]() -> ValueNode{
		if(!ufexists(path))
		{
			YTraceDe(Debug, "Path '%s' access failed.", path);
			if(show_info)
				YTraceDe(Notice, "Creating %s '%s'...", disp, path);

			ystdex::swap_guard<int, void, decltype(errno)&> gd(errno, 0);

			// XXX: Failed on race condition detected.
			if(UniqueLockedOutputFileStream uofs{path, std::ios_base::out
				| std::ios_base::trunc | platform::ios_noreplace})
				WriteNPLA1Stream(uofs, Nonnull(creator)());
			else
			{
				int err(errno);

				YTraceDe(Warning, "Cannot create file, possible error"
					" (from errno) = %d: %s.", err, std::strerror(err));
				YTraceDe(Warning,"Creating default file failed.");
				return {};
			}
			YTraceDe(Debug, "Created configuration.");
		}
		if(show_info)
			YTraceDe(Notice, "Found %s '%s'.", Nonnull(disp), path);
		// XXX: Race condition may cause failure, though file would not be
		//	corrupted now.
		if(SharedInputMappedFileStream sifs{path})
		{
			YTraceDe(Debug, "Accessible configuration file found.");
			if(Text::CheckBOM(sifs, Text::BOM_UTF_8))
				return TryReadRawNPLStream(sifs);
			YTraceDe(Warning, "Wrong encoding of configuration file found.");
		}
		YTraceDe(Err, "Configuration corrupted.");
		return {};
	}));

	if(res)
		return res;
	YTraceDe(Notice, "Trying fallback in memory...");

	std::stringstream ss;

	ss << Nonnull(creator)();
	return TryReadRawNPLStream(ss);
}

void
LoadComponents(Application& app, const ValueNode& node)
{
	const auto& data_dir(AccessChild<string>(node, "DataDirectory"));
	const auto& font_path(AccessChild<string>(node, "FontFile"));
	const auto& font_dir(AccessChild<string>(node, "FontDirectory"));

	if(!data_dir.empty() && !font_path.empty() && !font_dir.empty())
		YTraceDe(Notice, "Loaded default directory:\n%s\nLoaded default"
			" font path:\n%s\nLoaded default font directory:\n%s",
			data_dir.c_str(), font_path.c_str(), font_dir.c_str());
	else
		throw GeneralEvent("Empty path loaded.");
#if !CHRLib_NoDynamicMapping
	app.AddExit(CMap(data_dir));
#endif
	YTraceDe(Notice, "Trying entering directory '%s' ...", data_dir.c_str());
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
	if(UniqueLockedOutputFileStream
		uofs{CONF_PATH, std::ios_base::out | std::ios_base::trunc})
	{
		YTraceDe(Debug, "Writing configuration...");
		WriteNPLA1Stream(uofs, ValueNode(node.GetContainer()));
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
	InitializeKeyModule([&]{
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
			CatchExpr(std::system_error& e, YTraceDe(Warning,
				"Failed loading font directory."), ExtractAndTrace(e, Warning))
		fc.InitializeDefaultTypeface();

		const auto faces(fc.GetFaces().size());

		if(faces != 0)
			YTraceDe(Notice, "%zu face(s) in %zu font file(s)"
				" are loaded\nsuccessfully.\n", faces, loaded);
		else
			throw GeneralEvent("No fonts found.");
		YTraceDe(Notice, "Setting default font face...");
		if(const auto pf = fc.GetDefaultTypefacePtr())
			YTraceDe(Notice, "\"%s\":\"%s\",\nsuccessfully.",
				pf->GetFamilyName().c_str(), pf->GetStyleName().c_str());
		else
			throw GeneralEvent("Setting default font face failed.");
	}, yfsig, "      Font Caching Failure      ",
		" Please make sure the fonts are\n stored in correct path.\n");
}


ValueNode&
FetchRoot() ynothrow
{
	return FetchEnvironment().Root;
}

Drawing::FontCache&
FetchDefaultFontCache()
{
	return FetchDefaultResource<Drawing::FontCache>([]{
		return make_unique<Drawing::FontCache>();
	}, [](ValueNode& node, unique_ptr<Drawing::FontCache>& locked)
		-> Drawing::FontCache&{
		InitializeSystemFontCache(*locked,
			AccessChild<string>(node, "FontFile"),
			AccessChild<string>(node, "FontDirectory"));
		return *locked.get();
	});
}

MIMEBiMapping&
FetchMIMEBiMapping()
{
	return FetchDefaultResource<MIMEBiMapping>([]{
		return MIMEBiMapping();
	}, [](ValueNode& node, MIMEBiMapping& locked) -> MIMEBiMapping&{
		AddMIMEItems(locked, LoadNPLA1File("MIME database", (AccessChild<string>
			(node, "DataDirectory") + "MIMEExtMap.txt").c_str(), []{
				return A1::LoadNode(SContext::Analyze(NPL::Session(TU_MIME)));
			}, true));
		return locked;
	});
}

} // namespace YSLib;

