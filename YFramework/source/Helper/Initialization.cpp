/*
	© 2009-2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Initialization.cpp
\ingroup Helper
\brief 框架初始化。
\version r4103
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-10-21 23:15:08 +0800
\par 修改时间:
	2023-03-30 03:15 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::Initialization
*/


#include "Helper/YModules.h"
#include YFM_Helper_Initialization // for IO::Path, IO::FetchSeparator,
//	FetchCurrentWorkingDirectory, IO::MaxPathLength, GeneralEvent,
//	pmr::new_delete_resource_t, ystdex::nptr, pair, value_map, mutex,
//	lock_guard, IO::EnsureDirectory, IO::VerifyDirectories, PerformKeyAction,
//	IO::TraverseChildren, NodeCategory, NativePathView, String;
#if !(YCL_Win32 || YCL_Linux)
#	include <ystdex/string.hpp> // for ystdex::rtrim;
#	include YFM_YCLib_FileSystem // for platform::EndsWithNonSeperator;
#endif
#if YCL_Win32
#	include YFM_Win32_YCLib_NLS // for platform_ex::FetchModuleFileName,
//	platform_ex::FetchDBCSOffset, platform_ex::WCSToUTF8,
//	platform_ex::UTF8ToWCS;
#endif
#include YFM_CHRLib_MappingEx // for CHRLib::cp113_lkp;
#include YFM_NPL_Configuration // for NPL::Configuration, A1::NodeLoader;
#include YFM_YSLib_Service_TextFile // for Text::BOM_UTF_8, Text::CheckBOM;
#include <ystdex/string.hpp> // for ystdex::write_literal, ystdex::sfmt;
#include <cerrno> // for errno;
#include <ystdex/scope_guard.hpp> // for ystdex::swap_guard;
#include YFM_YSLib_Adaptor_YAdaptor // for FetchEnvironmentVariable;
#include YFM_Helper_GUIApplication // for FetchAppInstance,
//	Application::AddExit, FetchEnvironment;
#include YFM_YSLib_Core_YException // for ExtractAndTrace;
#include YFM_YSLib_Core_YStorage // for FetchStaticRef;
#include YFM_Helper_Environment // for complete type Environment introduced by
//	FetchEnvironment;

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

//! \since build 899
//!@{
#if YCL_Win32 || (YCL_Linux && !YCL_Android)
#	define YF_Helper_Initialization_UseFallbackConf_ true
#endif

struct RootPathCache
{
	string PathString;
	IO::Path Path;
	IO::Path Parent;

	// XXX: Similar to %FetchConfPaths as of the concurrent execution.
	//! \since build 957
	RootPathCache(string::allocator_type a)
		: PathString([&]{
#if YCL_Win32
			IO::Path image(platform::ucast(
				platform_ex::FetchModuleFileName({}, a).data()), a);

			if(!image.empty())
			{
				image.pop_back();

				const auto& dir(image.Verify());

				if(!dir.empty() && dir.back() == FetchSeparator<char16_t>())
					return dir.GetMBCS();
			}
#elif YCL_Android
			const char*
				sd_paths[]{"/sdcard/", "/mnt/sdcard/", "/storage/sdcard0/"};

			yunused(a);
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
#elif YCL_Linux
			// FIXME: What if link reading failed (i.e. permission denied)?
			// XXX: Link content like 'node_type:[inode]' is not supported.
			auto image(IO::ResolvePath<ystdex::path<vector<string>,
				IO::PathTraits>>(string_view("/proc/self/exe"), a));

			if(!image.empty())
			{
				image.pop_back();

				const auto& dir(IO::VerifyDirectoryPathTail(
					ystdex::to_string_d(image)));

				if(!dir.empty() && dir.back() == FetchSeparator<char>())
					return dir;
			}
#else
			// XXX: Trimming is necessary, because it is unspecified to have
			//	trailing slashes with %platform::ugetcwd as POSIX.1 2004.
			auto root_path(ystdex::rtrim(FetchCurrentWorkingDirectory<char>(
				IO::MaxPathLength, a), IO::FetchSeparator<char>()));

			YAssert(platform::EndsWithNonSeperator(root_path),
				"Invalid argument found.");
			root_path += IO::FetchSeparator<char>();
			return root_path;
// TODO: Add similar implemnetation for BSD family OS, etc.
#endif
			throw GeneralEvent("Failed finding working root path.");
		}()), Path(PathString), Parent(Path / u"..")
	{
		YTraceDe(Informative, "Initialized root directory path '%s'.",
			PathString.c_str());
	}
};

//! \since build 971
YB_ATTR_nodiscard YB_PURE pmr::new_delete_resource_t&
FetchPMRResourceRef() ynothrow
{
	static pmr::new_delete_resource_t r;

	return r;
}

YB_ATTR_nodiscard YB_PURE const RootPathCache&
FetchRootPathCache()
{
	static const RootPathCache cache(&FetchPMRResourceRef());

	return cache;
}

#if YF_Helper_Initialization_UseFallbackConf_
// XXX: Currently all platforms using fallback configurations have the same
//	image path and root path.
YB_ATTR_nodiscard YB_PURE inline const RootPathCache&
FetchImagePathCache()
{
	return FetchRootPathCache();
}
#endif

YB_ATTR_nodiscard YB_PURE bool
CheckLocalFHSLayoutImpl(const IO::Path& prefix, IO::Path bin)
{
	if(bin != prefix)
	{
		const IO::Path
			paths[]{prefix, std::move(bin), prefix / u"lib", prefix / u"share"};

		return IO::VerifyDirectories(paths);
	}
	return {};
}

YB_ATTR_nodiscard YB_PURE bool
CheckLocalFHSLayoutWithCache(const RootPathCache& cache)
{
#	if true
	// XXX: This is more efficient.
	return CheckLocalFHSLayoutImpl(cache.Parent, cache.Path);
#	else
	return CheckLocalFHSLayout(cache.PathString);
#	endif
}

YB_ATTR_nodiscard YB_PURE const string&
FetchPreferredConfPath()
{
#if YF_Helper_Initialization_UseFallbackConf_
	static string conf_path([]() -> string{
		auto& cache(FetchImagePathCache());

		if(CheckLocalFHSLayoutWithCache(cache))
		{
			YTraceDe(Informative, "FHS layout is detected, using"
				" '../var/YSLib/' relative to the program image path as the"
				" preferred configuration path.");
			return (cache.Parent / u"var" / u"YSLib").GetString().GetMBCS();
		}
		YTraceDe(Informative, "FHS layout is not detected, using the root path"
			" as the preferred configuration path.");

		const auto& r(FetchRootPathString());

		return string(r, r.get_allocator());
	}());

	return conf_path;
#else
	return FetchRootPathString();
#endif
}
//!@}

#if YCL_DS
#	define YF_Helper_Initialization_DataDirectory_ "/Data/"
#	define YF_Helper_Initialization_FontDirectory_ "/Font/"
//! \since build 971
#	define YF_Helper_Initialization_FontFile_(...) "/Font/FZYTK.TTF"
#elif YCL_Win32
#	define YF_Helper_Initialization_DataDirectory_ FetchRootPathString()
//! \since build 971
#	define YF_Helper_Initialization_FontFile_(a) \
	(FetchSystemFontDirectory_Win32(a) + "SimSun.ttc")
//! \since build 971
inline PDefH(string, FetchSystemFontDirectory_Win32, string::allocator_type a)
	// NOTE: Hard-coded as Shell32 special path with %CSIDL_FONTS or
	//	%CSIDL_FONTS. See https://msdn.microsoft.com/library/dd378457.aspx.
	ImplRet(
		platform_ex::WCSToUTF8(platform_ex::FetchWindowsPath(a), a) + "Fonts\\")
#elif YCL_Android
#	define YF_Helper_Initialization_DataDirectory_ \
	(FetchRootPathString() + "Data/")
#	define YF_Helper_Initialization_FontDirectory_ "/system/fonts/"
//! \since build 971
#	define YF_Helper_Initialization_FontFile_(...) \
	"/system/fonts/DroidSansFallback.ttf"
#elif YCL_Linux
#	define YF_Helper_Initialization_DataDirectory_ FetchRootPathString()
//! \since build 971
#	define YF_Helper_Initialization_FontFile_(...) "./SimSun.ttc"
#else
#	error "Unsupported platform found."
#endif
#ifndef YF_Helper_Initialization_DataDirectory_
#	error "Data directory shall be defined."
#endif
#ifndef YF_Helper_Initialization_FontDirectory_
#	define YF_Helper_Initialization_FontDirectory_ \
	YF_Helper_Initialization_DataDirectory_
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
		const auto mapping_name(to_std_string(data_dir) + "cp113.bin");

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
				CHRLib::cp113_lkp = []
					YB_LAMBDA_ANNOTATE((byte, byte), , noreturn) -> char16_t{
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
inline void
WriteNPLA1Stream(std::ostream& os, NPL::Configuration&& conf)
{
	ystdex::write_literal(os, Text::BOM_UTF_8) << std::move(conf);
}

//! \since build 971
//!@{
YB_ATTR_nodiscard ValueNode
TryReadRawNPLStream(std::istream& is, NPL::Configuration::allocator_type a)
{
	NPL::Configuration conf(a);

	is >> conf;
	YTraceDe(Debug, "Plain configuration loaded.");
	if(!conf.GetNodeRRef().empty())
		return conf.GetNodeRRef();
	YTraceDe(Warning, "Empty configuration found.");
	throw GeneralEvent("Invalid stream found when reading configuration.");
}

YB_ATTR_nodiscard YB_NONNULL(1, 2) ValueNode
LoadNPLA1FileDirect(const char* disp, const char* path,
	ValueNode::allocator_type a, bool show_info)
{
	yunused(disp);
	if(show_info)
		YTraceDe(Notice, "Found %s '%s'.", Nonnull(disp), path);
	// XXX: Race condition may cause failure, though file would not be corrupted
	//	now.
	// XXX: Exception thrown here would be caught by %TryInvoke in the call to
	//	%LoadNPLA1FileVec or %LoadNPLA1File, so there is no need to attach the
	//	additional filename information here.
	if(SharedInputMappedFileStream sifs{path})
	{
		YTraceDe(Debug, "Accessible configuration file found.");
		if(Text::CheckBOM(sifs, Text::BOM_UTF_8))
			return TryReadRawNPLStream(sifs, a);
		YTraceDe(Warning, "Wrong encoding of configuration file found.");
	}
	YTraceDe(Err, "Configuration corrupted.");
	return ValueNode(a);
}

YB_ATTR_nodiscard YB_NONNULL(1, 2, 3) ValueNode
LoadNPLA1FileCreate(const char* disp, const char* path,
	ValueNodeCreator creator, ValueNode::allocator_type a, bool show_info)
{
	if(show_info)
		YTraceDe(Notice, "Creating %s '%s'...", disp, path);

	ystdex::swap_guard<int, void, decltype(errno)&> gd(errno, 0);

	// XXX: Failed on race condition detected.
	if(UniqueLockedOutputFileStream uofs{path, std::ios_base::out
		| std::ios_base::trunc | platform::ios_noreplace})
		WriteNPLA1Stream(uofs, Nonnull(creator)(a));
	else
	{
		int err(errno);

		yunused(err);
		YTraceDe(Warning, "Cannot create file, possible error"
			" (from errno) = %d: %s.", err, std::strerror(err));
		YTraceDe(Warning,"Creating default file failed.");
		return ValueNode(a);
	}
	YTraceDe(Debug, "Created configuration.");
	return LoadNPLA1FileDirect(disp, path, a, show_info);
}

YB_ATTR_nodiscard YB_NONNULL(1) ValueNode
LoadNPLA1MemoryFallback(ValueNodeCreator creator, ValueNode::allocator_type a)
{
	YTraceDe(Notice, "Trying fallback in memory...");

	std::stringstream ss;

	ss << Nonnull(creator)(a);
	return TryReadRawNPLStream(ss, a);
}
//!@}

//! \since build 899
//!@{
yconstexpr const char* ConfFileDisp("configuration file");

#if YF_Helper_Initialization_UseFallbackConf_
using ValueNodeLoadEntry = pair<string, ValueNode(*)()>;
using VecRecordMap = value_map<string, string>;

mutex NPLA1PathVecRecordMutex;

YB_ATTR_nodiscard YB_PURE VecRecordMap&
FetchNPLA1PathVecRecordRef()
{
	static VecRecordMap m(&FetchPMRResourceRef());

	return m;
}

inline YB_NONNULL(1) void
AddNPLA1FileVecRecordItem(const char* disp, const string& conf_path)
{
	FetchNPLA1PathVecRecordRef().emplace(disp, conf_path);
}

//! \since build 971
YB_ATTR_nodiscard YB_NONNULL(1, 3, 4) ValueNode
LoadNPLA1FileVec(const char* disp, const vector<string>& conf_paths,
	const char* filename, ValueNodeCreator creator, ValueNode::allocator_type a,
	bool show_info)
{
	auto res(TryInvoke([=]() -> ValueNode{
		for(const auto& conf_path : conf_paths)
		{
			const auto e(conf_path + filename);
			const auto path(e.c_str());

			if(ufexists(path))
			{
				const lock_guard<mutex> gd(NPLA1PathVecRecordMutex);
				auto r(LoadNPLA1FileDirect(disp, path, a, show_info));

				if(r)
					AddNPLA1FileVecRecordItem(disp, conf_path);
				return r;
			}
			YTraceDe(Debug, "Path '%s' access failed.", path);
		}
		for(const auto& conf_path : conf_paths)
		{
			TryExpr(IO::EnsureDirectory(conf_path))
			catch(std::system_error&)
			{
				continue;
			}

			const auto e(conf_path + filename);
			const lock_guard<mutex> gd(NPLA1PathVecRecordMutex);
	
			if(auto conf{
				LoadNPLA1FileCreate(disp, e.c_str(), creator, a, show_info)})
			{
				AddNPLA1FileVecRecordItem(disp, conf_path);
				return conf;
			}
		}
		return {};
	}));

	return res ? res : LoadNPLA1MemoryFallback(creator, a);
}

const vector<string>&
FetchConfPaths()
{
	static const struct Init
	{
		vector<string> Paths;

		// XXX: This may be concurrently executed across multiple threads
		//	without %call_once. This is still assumed safe (free of race)
		//	because the result shall be the same in each thread.
		Init()
			: Paths([]() -> vector<string>{
				vector<string> res{FetchPreferredConfPath()};
				string var;

				if(FetchEnvironmentVariable(var, "HOME") && !var.empty())
				{
					AddPostfix(var);
					res.push_back(std::move(var));
					var.clear();
				}
#if YCL_Win32
				if(FetchEnvironmentVariable(var, "USERPROFILE") && !var.empty())
				{
					AddPostfix(var);
					res.push_back(std::move(var));
					var.clear();
				}
#endif
				return res;
			}())
		{
			YTraceDe(Informative,
				"Initialized enumerated configuration root paths.");
		}

	private:
		static inline void
		AddPostfix(string& var)
		{
			var += FetchSeparator<char>();
			var += ".YSLib";
			var += FetchSeparator<char>();
		}
	} init;

	return init.Paths;
}
#endif

YB_ATTR_nodiscard YB_PURE const string&
FetchConfPathForSave()
{
#if YF_Helper_Initialization_UseFallbackConf_
	const auto& vr(FetchNPLA1PathVecRecordRef());
	const lock_guard<mutex> gd(NPLA1PathVecRecordMutex);
	const auto i(vr.find(ConfFileDisp));

	return i != vr.end() ? i->second : FetchPreferredConfPath();
#else
	return FetchPreferredConfPath();
#endif
}
//!@}

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


bool
CheckLocalFHSLayout(const string& path_string)
{
	IO::Path bin(path_string);
	const auto prefix(bin / u"..");

	return CheckLocalFHSLayoutImpl(prefix, std::move(bin));
}

const IO::Path&
FetchLocalFHSRootPath()
{
#if true
	// XXX: This is more efficient.
	auto& cache(FetchRootPathCache());

	if(CheckLocalFHSLayoutWithCache(cache))
		return cache.Parent;
	return FetchStaticRef<const IO::Path>();
#else
	static const auto path(GetLocalFHSRootPathOf(FetchRootPathString()));

	return path;
#endif
}

const string&
FetchRootPathString()
{
	return FetchRootPathCache().PathString;
}

IO::Path
GetLocalFHSRootPathOf(const string& path_string)
{
#if true
	// XXX: This is more efficient.
	IO::Path bin(path_string);
	auto prefix(bin / u"..");

	if(CheckLocalFHSLayoutImpl(prefix, std::move(bin)))
		return prefix;
#else
	if(CheckLocalFHSLayout(path_string))
		return IO::Path(path_string) / u"..";
#endif
	return {};
}


ValueNode
LoadNPLA1File(const char* disp, const char* path, ValueNodeCreator creator,
	ValueNode::allocator_type a, bool show_info)
{
	auto res(TryInvoke([=]() -> ValueNode{
		if(ufexists(path))
			return LoadNPLA1FileDirect(disp, path, a, show_info);
		YTraceDe(Debug, "Path '%s' access failed.", path);
		return LoadNPLA1FileCreate(disp, path, creator, a, show_info);
	}));

	return res ? std::move(res) : LoadNPLA1MemoryFallback(creator, a);
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
LoadConfiguration(ValueNode::allocator_type a, bool show_info)
{
	ValueNode(*creator)(ValueNode::allocator_type)(
		[](ValueNode::allocator_type al){
		return ValueNode(NodeLiteral{{"YFramework", al},
			{{{"DataDirectory", al}, YF_Helper_Initialization_DataDirectory_},
			{{"FontFile", al}, YF_Helper_Initialization_FontFile_(al)},
			{{"FontDirectory", al}, YF_Helper_Initialization_FontDirectory_}}});
	});

#if YF_Helper_Initialization_UseFallbackConf_
	return LoadNPLA1FileVec(ConfFileDisp, FetchConfPaths(), "yconf.txt",
		creator, a, show_info);
#else
	return LoadNPLA1File(ConfFileDisp, (FetchPreferredConfPath() + "yconf.txt")
		.c_str(), creator, a, show_info);
#endif
#undef YF_Helper_Initialization_UseFallbackConf_
}

void
SaveConfiguration(const ValueNode& node)
{
	if(UniqueLockedOutputFileStream uofs{
		(FetchConfPathForSave() + "yconf.txt").c_str(),
		std::ios_base::out | std::ios_base::trunc})
	{
		YTraceDe(Debug, "Writing configuration...");
		WriteNPLA1Stream(uofs, ValueNode(node.GetContainer()));
	}
	else
		throw GeneralEvent("Invalid file found when writing configuration.");
	YTraceDe(Debug, "Writing configuration done.");
}

#undef YF_Helper_Initialization_DataDirectory_
#undef YF_Helper_Initialization_FontDirectory_
#undef YF_Helper_Initialization_FontFile_


void
InitializeSystemFontCache(FontCache& fc, const string& font_file,
	const string& font_dir)
{
	string res;

	YTraceDe(Notice, "Loading font files...");
	PerformKeyAction([&]{
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
FetchRoot()
{
	return FetchEnvironment().Root;
}

Drawing::FontCache&
FetchDefaultFontCache()
{
	using Drawing::FontCache;

	return FetchDefaultResource<FontCache>([]{
		return make_unique<FontCache>(+FontCache::DefaultGlyphCacheSize,
			FetchAppInstance().get_allocator());
	}, [](ValueNode& node, unique_ptr<FontCache>& locked) -> FontCache&{
		InitializeSystemFontCache(*locked, AccessChild<string>(node,
			"FontFile"), AccessChild<string>(node, "FontDirectory"));
		return *locked.get();
	});
}

MIMEBiMapping&
FetchMIMEBiMapping()
{
	return FetchDefaultResource<MIMEBiMapping>([]{
		return MIMEBiMapping(FetchAppInstance().get_allocator());
	}, [](ValueNode& node, MIMEBiMapping& locked) -> MIMEBiMapping&{
		AddMIMEItems(locked, LoadNPLA1File("MIME database", (AccessChild<string>
			(node, "DataDirectory") + "MIMEExtMap.txt").c_str(),
			[](ValueNode::allocator_type){
			return A1::NodeLoader(FetchEnvironment().Global).LoadNode(TU_MIME);
		}, node.get_allocator(), true));
		return locked;
	});
}

} // namespace YSLib;

