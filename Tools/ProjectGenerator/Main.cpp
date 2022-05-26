/*
	© 2015-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Main.cpp
\ingroup MaintenanceTools
\brief 项目生成和更新工具。
\version r866
\author FrankHB <frankhb1989@gmail.com>
\since build 599
\par 创建时间:
	2015-05-18 20:45:11 +0800
\par 修改时间:
	2022-05-17 12:04 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Tools::ProjectGenerator::Main

This is a command line tool to generate and update project files.
See readme file for details.
*/


#include <YSBuild.h>
#include YFM_NPL_SXML // for NPL, SXML, MakeAttributeLiteral,
//	InsertAttributeNode, InsertChildSyntaxNode, ystdex::seq_apply;
#include YFM_YCLib_Host // for platform_ex::DecodeArg, platform_ex::EncodeArg;
#include <iostream>

using namespace YSLib;
using namespace IO;
using namespace NPL;
using namespace SXML;

namespace
{

const string C_CONF(" -f $makefile -r CONF=$target");
const string C_CMD("CMD /C ");

//! \since build 600
//@{
using spath = ystdex::path<vector<string>>;

YB_ATTR_nodiscard yconstfn YB_STATELESS
	PDefH(const char*, GetConfName, bool debug)
	ImplRet(debug ? "debug" : "release")

YB_ATTR_nodiscard yconstfn YB_STATELESS PDefH(const char*, GetConfSuffix, bool debug)
	ImplRet(debug ? "d." : ".")

YB_ATTR_nodiscard YB_PURE inline
	PDefH(string, GetDLLName, const string& name, bool debug, bool win32 = true)
	ImplRet(name + GetConfSuffix(debug) + (win32 ? "dll" : "so"))

YB_ATTR_nodiscard YB_PURE inline PDefH(spath, GetLibPath, const string& name,
	bool debug, bool dll = true, bool win32 = true)
	ImplRet({name, string(GetConfName(debug)), dll
		? GetDLLName(name, debug, win32) : "lib" + name + GetConfSuffix(debug)
		+ 'a'})

YB_ATTR_nodiscard YB_PURE inline PDefH(string, QuoteCommandPath,
	const spath& pth, const string& separator = "\\")
	ImplRet(ystdex::quote(to_string(pth, separator), " &quot;", "&quot;"))
//@}

YB_ATTR_nodiscard NodeLiteral
MakeCommandLiteral(const string& jstr = "-j8")
{
	const auto Build("$make " + jstr + C_CONF);
	const auto Clean("$make" + C_CONF + " clean");
	const auto make([](const string& name, const string& val){
		return MakeAttributeLiteral(name, "command", val);
	});

	// TODO: Support diagnostics for showing in Visual Studio.
	return {"MakeCommands", {{make("Build", Build)}, {make("CompileFile",
		"$make" + C_CONF + " $file")}, {make("Clean", Clean)}, {make(
		"DistClean", Clean)}, {make("AskRebuildNeeded", "$make -q" + C_CONF
		+ " rebuild")}, {make("SilentBuild",  Build + " &gt; $(CMD_NULL)")}}};
}

//! \since build 600
YB_ATTR_nodiscard NodeLiteral
MakeExtraCommandLiteral(bool debug, const string& proj, const spath& prefix)
{
	const auto& dst(prefix / proj / GetConfName(debug));
	const auto& quo([&, debug](const string& name) -> string{
		return QuoteCommandPath(dst / GetDLLName(name, debug));
	});
	const auto del([&](const string& name) -> string{
		return C_CMD + "DEL" + quo(name) + " || EXIT";
	});
	const auto create([&](const string& name) -> string{
		const auto& qd(quo(name));
		const auto& qs(QuoteCommandPath(prefix / ".." / ".."
			/ GetLibPath(name, debug)));
		const auto& qds(qd + qs);

		return C_CMD + "MKLINK" + qds + " || MKLINK /H" + qds
			+ " || ECHO F | XCOPY /D /Y" + qs + qd;
	});
	const auto make([](const string& val){
		return MakeAttributeLiteral("Add", "before", val);
	});

	return {"ExtraCommands", {{make(del("YBase"))}, {make(create("YBase"))},
		{make(del("YFramework"))}, {make(create("YFramework"))}}};
}

//! \since build 850
//@{
YB_ATTR_nodiscard YB_PURE inline
	PDefH(bool, IsDSARM, const string& platform) ynothrow
	ImplRet(platform == "DS_ARM7" || platform == "DS_ARM9")

YB_ATTR_nodiscard YB_PURE inline
	PDefH(bool, IsDS, const string& platform) ynothrow
	ImplRet(platform == "DS" || IsDSARM(platform))

YB_ATTR_nodiscard YB_PURE string
LookupCompilerName(const string& platform) ynothrow
{
	if(platform.empty())
		return "null";
	if(IsDS(platform))
		return "gnu_armeabi_gcc_compiler";
	if(platform == "Android")
		return "gnu_gcc_compiler_for_android";
	return "gcc";
}

YB_ATTR_nodiscard YB_PURE inline
	PDefH(bool, CheckIfCustomMakefile, const string& platform)
	ImplRet(LookupCompilerName(platform) != "gcc")
//@}


enum class BuildType : yimpl(size_t)
{
	Default = 0,
	Static = 1,
	GUI = 2,
	Executable = 4,
	StaticExecutable = Executable | Static,
	GUIExecutable = GUI | Executable,
	StaticGUIExecutable = GUIExecutable | Static
};

DefBitmaskEnum(BuildType)


template<class _tNodeOrCon>
inline void
AddOptionNode(_tNodeOrCon&& node_or_con, std::initializer_list<NodeLiteral> il)
{
	InsertAttributeNode(yforward(node_or_con), "Option", il);
}
template<class _tNodeOrCon, typename... _tParams>
inline void
AddOptionNode(_tNodeOrCon&& node_or_con, _tParams&&... args)
{
	InsertAttributeNode(yforward(node_or_con), "Option", yforward(args)...);
}

template<class _tNodeOrCon>
inline void
AddCompilerNode(_tNodeOrCon&& node_or_con, const string& platform)
{
	AddOptionNode(yforward(node_or_con), "compiler",
		LookupCompilerName(platform));
}

using HostHandler = std::function<void(TermNode&, bool, bool)>;

//! \since build 852
void
InsertTargetNode(TermNode& term, const string& project,
	const string& platform, bool custom_makefile, bool debug, BuildType btype,
	HostHandler hosted_handler = {})
{
	const string& target(GetConfName(debug));
	const string& proj_type(custom_makefile ? "4"
		: [](BuildType b) -> const char*{
		switch(b)
		{
		case BuildType::GUIExecutable:
		case BuildType::StaticGUIExecutable:
			return "0";
		case BuildType::Executable:
		case BuildType::StaticExecutable:
			return "1";
		case BuildType::Static:
			return "2";
		default:
			return "3";
		}
	}(!debug && bool(btype & BuildType::Executable) ? btype
		| BuildType::GUI : btype));
	auto child(TransformToSyntaxNode(MakeAttributeLiteral("Target",
		"title", bool(btype & BuildType::Static) ? target : target + "_DLL")));

	if(!(platform.empty() || custom_makefile))
	{
		const auto& working_dir("../../build/" + platform + '/' + project);
		const auto& base_path(working_dir + '/' + target + '/');
		const auto&
			output_dir(bool(btype & BuildType::Static) ? "static" : "dynamic");
		string target_proj_name(project);

		if(bool(btype & BuildType::Executable))
		{
			if(bool(btype & BuildType::Static))
				target_proj_name += 's';
		}
		else if(debug)
			target_proj_name += 'd';
		AddOptionNode(child, {{"output", base_path + target_proj_name},
			{"prefix_auto", "1"}, {"extension_auto", "1"}});
		if(bool(btype & BuildType::Executable))
			AddOptionNode(child, "working_dir", working_dir);
		AddOptionNode(child, "object_output", base_path + output_dir + '/');
	}
	AddOptionNode(child, "type", proj_type);
	AddCompilerNode(child, platform);
	if(!custom_makefile)
	{
		if(bool(btype & BuildType::Executable))
		{
			if(debug)
				AddOptionNode(child, "use_console_runner", "0");
		}
		else if(platform == "MinGW32")
			AddOptionNode(child, "createDefFile", "1");
		if(hosted_handler)
			hosted_handler(child, debug, bool(btype & BuildType::Static));
	}
	if(!platform.empty() && custom_makefile)
		InsertChildSyntaxNode(child, MakeCommandLiteral());
	InsertChildSyntaxNode(term, std::move(child));
}

YB_ATTR_nodiscard YB_PURE vector<string>
GetCompilerOptionList(bool debug)
{
	if(debug)
		return {"-g", "-D_GLIBCXX_DEBUG_PEDANTIC"};
	return {"-fomit-frame-pointer", "-fexpensive-optimizations", "-O3",
		"-flto=jobserver", "-fno-enforce-eh-specs", "-DNDEBUG"};
}

YB_ATTR_nodiscard bool
CheckExcluded(const string& path, const string& platform)
{
	static const map<string, vector<string>> elst{
		{"DS", {"XCB", "YCLib/Host", "JNI"}},
		{"Android", {"XCB"}}
	};
	static const vector<pair<string, vector<string>>> ilst{
		{"DS", {"default_font.bin", "DS/data"}},
		{"Android", {"AndroidManifest.xml"}}
	};
	const auto i(elst.find(platform));

	if(i != elst.end())
		for(const auto& str : i->second)
			if(ystdex::exists_substr(path, str))
				return {};
	for(const auto& plat : ilst)
		if(plat.first != platform)
			for(const auto& str : plat.second)
				if(ystdex::exists_substr(path, str))
					return {};
	return true;
}

void
InsertUnit(set<string>& res, const Path& pth, const string& platform)
{
	auto lpth((!platform.empty() && pth.size() > 2 && pth.front() == u".."
		&& *(pth.begin() + 1) == String(platform)
		? Path(pth.begin() + 2, pth.end()) : pth).VerifyAsMBCS('/'));

	if(CheckExcluded(lpth, platform))
		res.insert(std::move(lpth));
}

void
CheckUnit(set<string>& res, const Path& pth, const Path& opth,
	const string& platform)
{
	if(ufexists(string(pth).c_str()))
		InsertUnit(res, opth, platform);
}
void
CheckUnit(set<string>& res, const Path& pth, const Path& opth,
	const String& name, const string& platform)
{
	CheckUnit(res, pth / name, opth / name, platform);
}

void
FindUnits(set<string>& res, const Path& pth, const Path& opth,
	const string& platform)
{
	if(VerifyDirectory(pth))
		TraverseChildren(pth, [&](NodeCategory c, NativePathView npv){
			const String name(npv);

			if(name[0] != u'.')
			{
				const auto& ofpth(opth / name);

				if(bool(c & NodeCategory::Directory))
					FindUnits(res, pth / name, ofpth, platform);
				else
					InsertUnit(res, ofpth, platform);
			}
		});
}
inline PDefH(void, FindUnits, set<string>& res, const Path& pth,
	const Path& opth, const String& name, const string& platform)
	ImplExpr(FindUnits(res, pth / name, opth / name, platform))

void
SearchUnits(set<string>& res, const Path& pth, const Path& opth,
	bool custom_makefile, const string& platform)
{
	if(custom_makefile)
	{
		CheckUnit(res, pth, opth, u"Makefile", platform);
		if(platform == "Android")
			CheckUnit(res, pth, opth, u"AndroidManifest.xml", platform);
	}
	for(const auto& dir : {u"data", u"include", u"source"})
		FindUnits(res, pth, opth, dir, platform);
}

//! \since build 852
//@{
YB_ATTR_nodiscard YB_PURE TermNode
MakeCBDocNode(const string& project, const string& platform, bool exe,
	const set<string>& units, bool custom_makefile)
{
	// NOTE: This path shall be consistent to the source tree layout. It is used
	//	only for %YFramework in platform %MinGW32. It can be extended to
	//	%MinGW64, which is not supported by this tool currently. Also note the
	//	directory name is previously 'lib-i686' before build 885.
	const auto host_lib_dir = "lib";
	auto doc(MakeXMLDoc({}, "1.0", "UTF-8", "yes"));
	auto& file(Deref(
		InsertChildSyntaxNode(doc, NodeLiteral("CodeBlocks_project_file"))));
	InsertAttributeNode(file, "FileVersion", {{"major", "1"}, {"minor", "6"}});

	auto& proj(Deref(InsertChildSyntaxNode(file, NodeLiteral("Project"))));
	const auto btype(exe ? BuildType::Executable : BuildType::Default);
	string title(project);

	if(!platform.empty())
	{
		if(IsDSARM(platform))
			title += platform.substr(2);
		else
			title += '_' + platform;
	}
	AddOptionNode(proj, "title", title);
	if(custom_makefile)
		AddOptionNode(proj, "makefile_is_custom", "1");
	AddCompilerNode(proj, platform);
	if(!custom_makefile)
		AddOptionNode(proj, "extended_obj_names", "1");

	auto& build(Deref(InsertChildSyntaxNode(proj, NodeLiteral("Build"))));
	HostHandler handler;

	if(!custom_makefile)
		handler = [&](TermNode& tm, bool debug, bool is_static){
			auto child(TransformToSyntaxNode(NodeLiteral("Compiler")));
			const auto opt_add([&child](const string& str){
				InsertAttributeNode(child, "Add", "option", str);
			});
			const auto lib_add([&child](const spath& pth){
				InsertAttributeNode(child, "Add", "library", to_string(pth));
			});

			for(auto& opt : GetCompilerOptionList(debug))
				opt_add(opt);
			if(!is_static)
			{
				if(project == "YBase")
					opt_add("-DYB_BUILD_DLL");
				else
				{
					opt_add("-DYB_DLL");
					opt_add(project == "YFramework" ? "-DYF_BUILD_DLL"
						: "-DYF_DLL");
				}
			}
			InsertChildSyntaxNode(tm, std::move(child));
			if(!debug || (project != "YBase"
				&& !(project == "YFramework" && is_static)))
			{
				child = TransformToSyntaxNode(NodeLiteral("Linker"));
				if(!debug)
					// XXX: Workarond for LTO bug on MinGW. See https://sourceware.org/bugzilla/show_bug.cgi?id=12762.
					ystdex::seq_apply(opt_add, "-s", "-flto=jobserver",
						"-Wl,-allow-multiple-definition");
				if(project != "YBase")
				{
					const spath pfx_w32{"..", "..", "build", "MinGW32"};
					const spath w32lib{"..", "..", "YFramework", "MinGW32",
						host_lib_dir};

					if(project != "YFramework")
					{
						if(is_static)
						{
							ystdex::seq_apply(lib_add, pfx_w32 / GetLibPath(
								"YFramework", debug, {}), w32lib
								/ "libFreeImage.a", w32lib / "libfreetype.a",
								pfx_w32 / GetLibPath("YBase", debug, {}));
						}
						else if(platform == "MinGW32")
							lib_add(pfx_w32 / GetLibPath("YFramework", debug));
					}
					else if(!is_static)
					{
						opt_add("-Wl,--no-undefined,--dynamic-list-data,"
							"--dynamic-list-cpp-new,--dynamic-list-cpp"
							"-typeinfo");
						if(project == "YFramework")
							ystdex::seq_apply(lib_add,
								spath{host_lib_dir, "libFreeImage.a"},
								spath{host_lib_dir, "libfreetype.a"});
					}
					if(platform == "MinGW32")
					{
						if(!is_static)
							lib_add(pfx_w32 / GetLibPath("YBase", debug));
						if(is_static != (project == "YFramework"))
							ystdex::seq_apply(lib_add, spath{"gdi32"},
								spath{"imm32"});
					}
				}
				InsertChildSyntaxNode(tm, std::move(child));
			}
			if(exe && !is_static && platform == "MinGW32")
				InsertChildSyntaxNode(tm, MakeExtraCommandLiteral(
					debug, project, {"..", "..", "build", "MinGW32"}));
		};

	const auto ins([&, custom_makefile](bool d, BuildType bt){
		InsertTargetNode(build, project, platform, custom_makefile, d, bt,
			handler);
	});

	ins(true, btype | BuildType::Static);
	ins({}, btype | BuildType::Static);
	if(!(platform.empty() || custom_makefile))
	{
		ins(true, btype);
		ins({}, btype);
	}
	if(!custom_makefile)
	{
		auto child(TransformToSyntaxNode(NodeLiteral("Compiler")));
		const auto opt_add([&child](const string& str){
			InsertAttributeNode(child, "Add", "option", str);
		});
		const auto dir_add([&child](const string& str){
			InsertAttributeNode(child, "Add", "directory", str);
		});

		ystdex::seq_apply(opt_add, "-Wnon-virtual-dtor", "-Wshadow",
			"-Wredundant-decls", "-Wcast-align", "-Wmissing-declarations",
			"-Wmissing-include-dirs", "-Wzero-as-null-pointer-constant",
			"-pedantic-errors", "-Wextra", "-Wall", "-std=c++11",
			"-Wctor-dtor-privacy", "-Winvalid-pch", "-Wsign-promo",
			"-Wdeprecated", "-Wdeprecated-declarations", "-Wformat=2",
			"-Wmultichar", "-Wno-format-nonliteral", "-Wdouble-promotion",
			"-Wfloat-equal", "-Wlogical-op", "-Wsign-conversion",
			"-Wtrampolines", "-Wconditionally-supported", "-Wno-noexcept-type",
			"-Wstrict-null-sentinel", "-fdata-sections", "-ffunction-sections",
			"-mthreads", "-pipe", "-U__GXX_MERGED_TYPEINFO_NAMES",
			"-D__GXX_MERGED_TYPEINFO_NAMES=1");
		if(project == "YFramework")
			opt_add("-DFREEIMAGE_LIB");
		dir_add("../include");
		if(project != "YBase")
		{
			if(project == "YFramework")
				ystdex::seq_apply(dir_add, "../DS/include", "../Win32/include");
			else
				ystdex::seq_apply(dir_add, "../../YFramework/DS/include",
					"../../YFramework/Win32/include",
					"../../YFramework/include");
			ystdex::seq_apply(dir_add, "../../YBase/include",
				"../../3rdparty/include", "../../3rdparty/freetype/include");
		}
		InsertChildSyntaxNode(proj, std::move(child));
		child = TransformToSyntaxNode(NodeLiteral("Linker"));
		ystdex::seq_apply(opt_add, "-Wl,--gc-sections", "-pipe");
		InsertChildSyntaxNode(proj, std::move(child));
	}
	for(const auto& unit : units)
		InsertAttributeNode(proj, "Unit", "filename", unit);
	return doc;
}
YB_ATTR_nodiscard YB_PURE TermNode
MakeCBDocNode(const Path& pth, const Path& opth, const string& platform,
	bool exe)
{
	const bool is_DS_ARM(IsDSARM(platform));
	auto project(pth.back().GetMBCS());

	if(is_DS_ARM)
	{
		if(project == platform && pth.size() > 1)
			project = Deref(pth.end() - 2).GetMBCS();
	}
	if(project.find_first_of(":/\\<>") != string::npos)
		throw LoggedEvent("Invalid project name found.");
	if(platform.find_first_of(":/\\<>") != string::npos)
		throw LoggedEvent("Invalid platform name found.");

	const bool custom_makefile(CheckIfCustomMakefile(platform));
	set<string> units;

	if(platform != "DS" || !exe)
		SearchUnits(units, pth, opth, is_DS_ARM, platform);
	if(project == "YFramework")
	{
		const auto& pth_3rd(pth / u".." / u"3rdparty");
		const auto& opth_3rd(opth / u".." / u"3rdparty");
		FindUnits(units, pth_3rd / u"include", opth_3rd / u"include", platform),
		FindUnits(units, pth_3rd / u"freetype" / u"include",
			opth_3rd / u"freetype" / u"include", platform);
	}
	if(!platform.empty())
	{
		SearchUnits(units, pth / platform, opth / platform, custom_makefile,
			platform);
		if(platform == "DS_ARM9")
			SearchUnits(units, pth / u"..", opth / u"..", {}, platform);
		else
		{
			if(!IsDS(platform))
				SearchUnits(units, pth / u"DS", opth / u"DS", {}, platform);
			if(platform == "MinGW32")
				SearchUnits(units, pth / u"Win32", opth / u"Win32", {},
					platform);
		}
	}
	return MakeCBDocNode(project, platform, exe, units, custom_makefile);
}
//@}

} // unnamed namespace;


int
main(int argc, char* argv[])
{
	using std::cerr;
	using std::clog;
	using std::cout;
	using std::endl;
	using namespace platform_ex;

	if(argc > 2)
	{
		const string ptype(argv[1]);

		if(ptype != "c" && ptype != "cl")
			cerr << "ERROR: Unsupported type '" << argv[1] << "' found."
				<< endl;
		else
		{
			platform::SetupBinaryStdIO();
			clog << "Code::Blocks generator found." << endl;

			const auto
				in(NormalizeDirectoryPathTail(DecodeArg(string(argv[2]))));

			if(!VerifyDirectory(in))
			{
				cerr << "ERROR: BASE_PATH '" << in << "' is not existed."
					<< endl;
				return EXIT_FAILURE;
			}

			const auto ipath(MakeNormalizedAbsolute(Path(in)));
			string platform;

			YAssert(!ipath.empty(), "Empty path found.");
			if(argc > 3)
				platform = DecodeArg(argv[3]);
			if(platform.empty())
				clog << "No platform specified";
			else
				clog << "Found platform '" << platform << '\'';
			clog << ".\nProject path '" << EncodeArg(to_string(ipath).GetMBCS())
				<< "' recognized." << endl;
			return FilterExceptions([&]{
				SXML::PrintSyntaxNode(cout, {YSLib::ListContainer,
					{{MakeCBDocNode(ipath, platform.empty() || IsDSARM(platform)
					? Path() : Path(u".."), platform, ptype == "c")}}});
				cout << endl;
				clog << "Conversion finished." << endl;
			}, yfsig) ? EXIT_FAILURE : EXIT_SUCCESS;
		}
	}
	else if(argc == 1)
	{
		cout << "Usage: \"" << *argv << "\" TYPE BASE_PATH [PLATFORM]\n"
			"\nThis program prints project files for specified targets in the"
			" standard output stream.\n\n"
			"TYPE\n\tThe generated project types. Currently accepted types:\n\n"
			"  c\n\tCode::Blocks project file for executables.\n\n"
			"  cl\n\tCode::Blocks project file for libraries.\n\n"
			"BASE_PATH\n\tThe source directory to be searched.\n\n"
			"PLATFORM\n\tThe platform name.\n\tIf specified and not empty, the"
			" program would search the corresponding subdirectory.\n" << endl;
		return EXIT_SUCCESS;
	}
	else
		cerr << "ERROR: No sufficient arguments found." << endl;
	return EXIT_FAILURE;
}

