/*
	© 2015-2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Main.cpp
\ingroup MaintenanceTools
\brief 项目生成和更新工具。
\version r1223
\author FrankHB <frankhb1989@gmail.com>
\since build 599
\par 创建时间:
	2015-05-18 20:45:11 +0800
\par 修改时间:
	2023-05-16 07:50 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Tools::ProjectGenerator::Main

This is a command line tool to generate and update project files.
See readme file for details.
*/


#include <YSBuild.h>
#include YFM_NPL_SXML // for NPL, SXML, ystdex::path, vector, string,
//	string_view, MakeAttributeLiteral, to_pmr_string, InsertAttributeNode,
//	ystdex::call_using_allocator, InsertChildSyntaxNode, MakeXMLDoc,
//	YSLib::function, ystdex::seq_apply, std::string, make_string_view;
#include YFM_YCLib_Host // for platform_ex::DecodeArg, platform_ex::EncodeArg;
#include <iostream>
#include <map> // for std::map;
#include <vector> // for std::vector;

namespace
{

using namespace YSLib;
using namespace IO;
using namespace NPL;
using namespace SXML;

//! \since build 600
//!@{
using spath = ystdex::path<vector<string>>;

YB_ATTR_nodiscard YB_ATTR_returns_nonnull yconstfn YB_STATELESS
	PDefH(const char*, GetConfName, bool debug)
	ImplRet(debug ? "debug" : "release")

YB_ATTR_nodiscard YB_ATTR_returns_nonnull yconstfn YB_STATELESS
	PDefH(const char*, GetConfSuffix, bool debug)
	ImplRet(debug ? "d." : ".")
//!@}

//! \since build 974
//!@{
YB_ATTR_nodiscard YB_PURE inline
	PDefH(string, GetDLLName, string name, bool debug, bool win32 = true)
	ImplRet(std::move(name) + GetConfSuffix(debug) + (win32 ? "dll" : "so"))

YB_ATTR_nodiscard YB_PURE YB_NONNULL(1) inline
	PDefH(spath, GetLibPath, const char* name, string::allocator_type a,
	bool debug, bool dll = true, bool win32 = true)
	ImplRet({{name, a}, string(GetConfName(debug)), dll
		? GetDLLName({name, a}, debug, win32) : "lib" + string(name, a)
		+ GetConfSuffix(debug) + 'a'})

YB_ATTR_nodiscard YB_PURE inline
	PDefH(string, QuoteCommandPath, const spath& pth, char sep = '\\')
	ImplRet(ystdex::quote(to_string(pth, sep), " &quot;", "&quot;"))

YB_ATTR_nodiscard YB_NONNULL(2) YB_PURE NodeLiteral
MakeCommandLiteral(string::allocator_type a = {}, const char* jstr = "-j8")
{
	static const auto c_conf(" -f $makefile -r CONF=$target");
	const auto build([&, a] YB_LAMBDA_ANNOTATE((), , pure){
		return "$make " + string(jstr, a) + string(c_conf, a);
	});
	const auto clean([&, a] YB_LAMBDA_ANNOTATE((), , pure){
		return "$make" + string(c_conf, a) + " clean";
	});
	const auto make([a]
		YB_LAMBDA_ANNOTATE((const char* name, string&& val), , nonnull(2)){
		return MakeAttributeLiteral({name, a}, "command", std::move(val));
	});

	// TODO: Support diagnostics for showing in Visual Studio.
	return {{"MakeCommands", a}, {{make("Build", build())}, {make("CompileFile",
		"$make" + string(c_conf, a) + " $file")}, {make("Clean", clean())},
		{make("DistClean", clean())}, {make("AskRebuildNeeded", "$make -q"
		+ string(c_conf, a) + " rebuild")},
		{make("SilentBuild", build() + " &gt; $(CMD_NULL)")}}};
}

YB_ATTR_nodiscard YB_PURE NodeLiteral
MakeExtraCommandLiteral(bool debug, string_view proj, const spath& prefix)
{
	static const auto c_cmd("CMD /C ");
	const auto a(prefix.get_container().get_allocator());
	const auto& dst(prefix / to_pmr_string(proj, a) / GetConfName(debug));
	const auto& quo([&, debug, a]
		YB_LAMBDA_ANNOTATE((const char* name), , nonnull(2), pure){
		return QuoteCommandPath(dst / GetDLLName({name, a}, debug));
	});
	const auto del([&, a]
		YB_LAMBDA_ANNOTATE((const char* name), , nonnull(2), pure) -> string{
		return string(c_cmd, a) + "DEL" + quo(name) + " || EXIT";
	});
	const auto create([&, a]
		YB_LAMBDA_ANNOTATE((const char* name), , nonnull(2), pure) -> string{
		const auto& qd(quo(name));
		const auto& qs(QuoteCommandPath(prefix / ".." / ".."
			/ GetLibPath(name, a, debug)));
		const auto& qds(qd + qs);

		return string(c_cmd, a) + "MKLINK" + qds + " || MKLINK /H" + qds
			+ " || ECHO F | XCOPY /D /Y" + qs + qd;
	});
	const auto make([] YB_LAMBDA_ANNOTATE((const string& val), , pure){
		return MakeAttributeLiteral("Add", "before", val);
	});

	return {{"ExtraCommands", a}, {{make(del("YBase"))},
		{make(create("YBase"))}, {make(del("YFramework"))},
		{make(create("YFramework"))}}};
}
//!@}

/*!
\pre 参数的数据指针非空。
\since build 970
*/
//!@{
YB_ATTR_nodiscard YB_PURE inline
	PDefH(bool, IsDSARM, string_view platform) ynothrow
	ImplRet(platform == "DS_ARM7" || platform == "DS_ARM9")

YB_ATTR_nodiscard YB_PURE inline
	PDefH(bool, IsDS, string_view platform) ynothrow
	ImplRet(platform == "DS" || IsDSARM(platform))

//! \since build 974
YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_PURE const char*
LookupCompilerName(string_view platform) ynothrow
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
	PDefH(bool, CheckIfCustomMakefile, string_view platform)
	ImplRet(LookupCompilerName(platform) != string_view("gcc"))
//!@}


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


//! \since build 974
template<class _tNodeOrCon>
struct GAddOption final
{
	_tNodeOrCon&& c;

	template<typename _tLiteral = NodeLiteral, size_t _vN>
	void
	operator()(NodeLiteral(&&l)[_vN]) const
	{
		InsertAttributeNode(yforward(c), {"Option", c.get_allocator()},
			std::move(l), c.get_allocator());
	}
	template<typename... _tParams>
	inline void
	operator()(_tParams&&... args) const
	{
		InsertAttributeNode(yforward(c), {"Option", c.get_allocator()},
			yforward(args)...);
	}

	void
	ForCompiler(string_view platform) const
	{
		ystdex::call_using_allocator<string>(*this, c.get_allocator(),
			"compiler", LookupCompilerName(platform));
	}
};

//! \since build 974
YB_ATTR_nodiscard YB_PURE std::initializer_list<const char*>
GetCompilerOptionList(bool debug)
{
	static const auto dl = {"-g", "-D_GLIBCXX_DEBUG_PEDANTIC"},
		rl = {"-fomit-frame-pointer", "-fexpensive-optimizations", "-O3",
		"-flto=jobserver", "-fno-enforce-eh-specs", "-DNDEBUG"};

	return debug ? dl : rl;
}

//! \since build 974
//!@{
YB_ATTR_nodiscard bool
CheckExcluded(const string& path, string_view platform)
{
	static const std::map<string_view, std::vector<const char*>> elst{
		{"DS", {"XCB", "YCLib/Host", "JNI"}},
		{"Android", {"XCB"}}
	};
	static const pair<string_view, std::vector<const char*>> ilst[]{
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
InsertUnit(set<string>& res, const Path& pth, string_view platform)
{
	auto lpth((!platform.empty() && pth.size() > 2 && pth.front() == u".."
		&& *(pth.begin() + 1) == String(platform)
		? Path(pth.begin() + 2, pth.end()) : pth).VerifyAsMBCS('/'));

	if(CheckExcluded(lpth, platform))
		res.insert(std::move(lpth));
}

void
CheckUnit(set<string>& res, const Path& pth, const Path& opth,
	string_view platform)
{
	if(ufexists(string(pth).c_str()))
		InsertUnit(res, opth, platform);
}
void
CheckUnit(set<string>& res, const Path& pth, const Path& opth,
	const String& name, string_view platform)
{
	CheckUnit(res, pth / name, opth / name, platform);
}

void
FindUnits(set<string>& res, const Path& pth, const Path& opth,
	string_view platform)
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
	const Path& opth, const String& name, string_view platform)
	ImplExpr(FindUnits(res, pth / name, opth / name, platform))

void
SearchUnits(set<string>& res, const Path& pth, const Path& opth,
	bool custom_makefile, string_view platform)
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
//!@}

//! \since build 974
//!@{
YB_ATTR_nodiscard YB_PURE TermNode
MakeCBDocNode(string_view project, string_view platform, bool exe,
	const set<string>& units, bool custom_makefile)
{
	// NOTE: This path shall be consistent to the source tree layout. It is used
	//	only for %YFramework in platform %MinGW32. It can be extended to
	//	%MinGW64, which is not supported by this tool currently. Also note the
	//	directory name is previously 'lib-i686' before build 885.
	const auto host_lib_dir = "lib";
	const auto a(units.get_allocator());
	auto doc(ystdex::call_using_allocator<string>(MakeXMLDoc, a, "", "1.0",
		"UTF-8", "yes"));
	auto& file(Deref(InsertChildSyntaxNode(doc,
		NodeLiteral({"CodeBlocks_project_file", a}))));

	InsertAttributeNode(file, {"FileVersion", a}, {{{"major", a}, {"1", a}},
		{{"minor", a}, {"6", a}}});

	auto& proj(Deref(InsertChildSyntaxNode(file, NodeLiteral({"Project", a}))));
	const auto btype(exe ? BuildType::Executable : BuildType::Default);
	auto title(to_pmr_string(project, a));

	if(!platform.empty())
	{
		if(IsDSARM(platform))
			title += platform.substr(2);
		else
		{
			title += '_';
			title += platform;
		}
	}

	GAddOption<TermNode&> add{proj};

	add("title", title);
	if(custom_makefile)
		add("makefile_is_custom", "1");
	add.ForCompiler(platform);
	if(!custom_makefile)
		add("extended_obj_names", "1");

	auto& build(Deref(InsertChildSyntaxNode(proj, NodeLiteral({"Build", a}))));
	YSLib::function<void(TermNode&, bool, bool)> handler;

	if(!custom_makefile)
		handler = [&](TermNode& tm, bool debug, bool is_static){
			auto child(TransformToSyntaxNode(NodeLiteral({"Compiler", a})));
			const auto opt_add(
				[&, a] YB_LAMBDA_ANNOTATE((const char* str), , nonnull(2)){
				InsertAttributeNode(child, string("Add", a), "option",
					string(str, a));
			});
			const auto lib_add([&, a](spath pth){
				InsertAttributeNode(child, string("Add", a), "library",
					to_string(std::move(pth)));
			});

			if(debug && platform == "MinGW32")
				opt_add("-Wa,-mbig-obj");
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
				child = TransformToSyntaxNode(NodeLiteral({"Linker", a}));
				if(!debug)
					// XXX: Workarond for LTO bug on MinGW. See
					//	https://sourceware.org/bugzilla/show_bug.cgi?id=12762.
					ystdex::seq_apply(opt_add, "-s", "-flto=jobserver",
						"-Wl,-allow-multiple-definition");
				if(project != "YBase")
				{
					const spath pfx_w32({"..", "..", "build", "MinGW32"}, a);
					const spath w32lib({"..", "..", "YFramework", "MinGW32",
						host_lib_dir}, a);

					if(project != "YFramework")
					{
						if(is_static)
							ystdex::seq_apply(lib_add, pfx_w32 / GetLibPath(
								"YFramework", a, debug, {}), w32lib
								/ "libFreeImage.a", w32lib / "libfreetype.a",
								pfx_w32 / GetLibPath("YBase", a, debug, {}));
						else if(platform == "MinGW32")
							lib_add(
								pfx_w32 / GetLibPath("YFramework", a, debug));
					}
					else if(!is_static)
					{
						opt_add("-Wl,--no-undefined,--dynamic-list-data,"
							"--dynamic-list-cpp-new,--dynamic-list-cpp"
							"-typeinfo");
						if(project == "YFramework")
							ystdex::seq_apply(lib_add,
								spath({host_lib_dir, "libFreeImage.a"}, a),
								spath({host_lib_dir, "libfreetype.a"}, a));
					}
					if(platform == "MinGW32")
					{
						if(!is_static)
							lib_add(pfx_w32 / GetLibPath("YBase", a, debug));
						if(is_static != (project == "YFramework"))
							ystdex::seq_apply(lib_add, spath({"gdi32"}, a),
								spath({"imm32"}, a), spath({"quadmath"}, a));
					}
				}
				InsertChildSyntaxNode(tm, std::move(child));
			}
			if(exe && !is_static && platform == "MinGW32")
				InsertChildSyntaxNode(tm, MakeExtraCommandLiteral(
					debug, project, {{"..", "..", "build", "MinGW32"}, a}));
		};

	const auto ins([&, custom_makefile](bool debug, BuildType bt){
		const string target(GetConfName(debug), a);
		const auto proj_type(custom_makefile ? "4" : []
			YB_LAMBDA_ANNOTATE((BuildType b), , returns_nonnull) -> const char*{
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
		}(!debug && bool(bt & BuildType::Executable) ? bt
			| BuildType::GUI : bt));
		auto child(TransformToSyntaxNode(MakeAttributeLiteral("Target", "title",
			bool(bt & BuildType::Static) ? target : target + "_DLL")));
		GAddOption<TermNode&> cadd{child};

		if(!(platform.empty() || custom_makefile))
		{
			auto working_dir((string("../../build/", a).append(platform)
				+= '/').append(project));
			auto base_path((string(working_dir, a) += '/').append(target)
				+ '/');
			const auto
				output_dir(bool(bt & BuildType::Static) ? "static" : "dynamic");
			auto target_proj_name(to_pmr_string(project, a));

			if(bool(bt & BuildType::Executable))
			{
				if(bool(bt & BuildType::Static))
					target_proj_name += 's';
			}
			else if(debug)
				target_proj_name += 'd';
			cadd({{"output", string(base_path, a) + target_proj_name},
				{"prefix_auto", "1"}, {"extension_auto", "1"}});
			if(bool(bt & BuildType::Executable))
				cadd("working_dir", std::move(working_dir));
			cadd("object_output", std::move(base_path) + output_dir + '/');
		}
		cadd("type", string(proj_type, a));
		cadd.ForCompiler(platform);
		if(!custom_makefile)
		{
			if(bool(bt & BuildType::Executable))
			{
				if(debug)
					cadd("use_console_runner", "0");
			}
			else if(platform == "MinGW32")
				cadd("createDefFile", "1");
			handler(child, debug, bool(bt & BuildType::Static));
		}
		if(!platform.empty() && custom_makefile)
			InsertChildSyntaxNode(child, MakeCommandLiteral(a));
		InsertChildSyntaxNode(build, std::move(child));
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
		auto child(TransformToSyntaxNode(NodeLiteral({"Compiler", a})));
		const auto opt_add(
			[&, a] YB_LAMBDA_ANNOTATE((const char* str), , nonnull(2)){
			InsertAttributeNode(child, string("Add", a), "option",
				string(str, a));
		});
		const auto dir_add(
			[&, a] YB_LAMBDA_ANNOTATE((const char* str), , nonnull(2)){
			InsertAttributeNode(child, string("Add", a), "directory",
				string(str, a));
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
			"-mthreads", "-pipe", "-U__GXX_TYPEINFO_EQUALITY_INLINE",
			"-D__GXX_TYPEINFO_EQUALITY_INLINE=1",
			"-U__GXX_MERGED_TYPEINFO_NAMES", "-D__GXX_MERGED_TYPEINFO_NAMES=0");
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
		child = TransformToSyntaxNode(NodeLiteral({"Linker", a}));
		ystdex::seq_apply(opt_add, "-Wl,--gc-sections", "-pipe");
		InsertChildSyntaxNode(proj, std::move(child));
	}
	for(const auto& unit : units)
		InsertAttributeNode(proj, "Unit", "filename", unit);
	return doc;
}
YB_ATTR_nodiscard YB_PURE TermNode
MakeCBDocNode(const Path& pth, const Path& opth, string_view platform, bool exe)
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
	set<string> units(pth.get_allocator());

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
//!@}

} // unnamed namespace;


int
main(int argc, char* argv[])
{
	using std::cerr;
	using std::cout;
	using std::endl;

	platform::SetupBinaryStdIO();
	if(argc > 2)
	{
		using namespace YSLib;
		using std::clog;
		static pmr::new_delete_resource_t r;
		const string ptype(argv[1], &r);

		if(ptype != "c" && ptype != "cl")
			cerr << "ERROR: Unsupported type '" << argv[1] << "' found."
				<< endl;
		else
		{
			using namespace IO;
			using namespace platform_ex;

			clog << "Code::Blocks generator found." << endl;

			const auto
				in(NormalizeDirectoryPathTail(DecodeArg(string(argv[2]))));

			if(!VerifyDirectory(in))
			{
				cerr << "ERROR: BASE_PATH '" << in << "' is not existed."
					<< endl;
				return EXIT_FAILURE;
			}

			std::string platform;

			if(argc > 3)
				platform = DecodeArg(argv[3]);
			if(platform.empty())
				clog << "No platform specified";
			else
				clog << "Found platform '" << platform << '\'';

			const auto ipath(MakeNormalizedAbsolute(Path(in, &r)));

			YAssert(!ipath.empty(), "Empty path found.");
			clog << ".\nProject path '" << EncodeArg(to_string(ipath).GetMBCS())
				<< "' recognized." << endl;

			const auto plat_sv(make_string_view(platform));

			return FilterExceptions([&]{
				NPL::SXML::PrintSyntaxNode(cout, {ListContainer,
					{{MakeCBDocNode(ipath, platform.empty() || IsDSARM(
					plat_sv) ? Path(&r) : Path(u"..", &r), plat_sv,
					ptype == "c")}}});
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

