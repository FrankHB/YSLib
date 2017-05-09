/*
	© 2015-2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Dependency.cpp
\ingroup NPL
\brief 依赖管理。
\version r466
\author FrankHB <frankhb1989@gmail.com>
\since build 623
\par 创建时间:
	2015-08-09 22:14:45 +0800
\par 修改时间:
	2017-05-08 18:57 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::Dependency
*/


#include "NPL/YModules.h"
#include YFM_NPL_Dependency // for std::placeholders, ystdex::bind1;
#include YFM_NPL_SContext
#include YFM_YSLib_Service_FileSystem // for YSLib::IO::*;
#include <iterator> // for std::istreambuf_iterator;
#include <limits> // for std::numeric_limits;
#include <cctype> // for std::isdigit, std::tolower;
#include <cerrno> // for errno, ERANGE;
#include <cstdio> // for std::puts;

using namespace YSLib;

namespace NPL
{

vector<string>
DecomposeMakefileDepList(std::streambuf& sb)
{
	using s_it_t = std::istreambuf_iterator<char>;
	// NOTE: Escaped spaces would be saved to prevent being used as delimiter.
	set<size_t> spaces;
	const auto sbuf(Session(s_it_t(&sb), s_it_t(),
		[&](LexicalAnalyzer& lexer, char c){
		lexer.ParseQuoted(c,
			[&](string& buf, const UnescapeContext& uctx, char) -> bool{
			const auto& escs(uctx.GetSequence());

			// NOTE: See comments in %munge function of 'mkdeps.c' from libcpp
			//	of GCC.
			if(escs.length() == 1)
			{
				if(uctx.Prefix == "\\")
					switch(escs[0])
					{
					case ' ':
						spaces.insert(buf.size());
					case '\\':
					case '#':
						buf += escs[0];
						return true;
					default:
						;
					}
				if(uctx.Prefix == "$" && escs[0] == '$')
				{
					buf += '$';
					return true;
				}
			}
			return {};
		}, [](char ch, string& pfx) -> bool{
			if(ch == '\\')
				pfx = "\\";
			else if(ch == '$')
				pfx = "$";
			else
				return {};
			return true;
		});
	}).GetBuffer());
	vector<string> lst;

	ystdex::split_if(sbuf.begin(), sbuf.end(), static_cast<int(&)(int)>(
		std::isspace), [&](string::const_iterator b, string::const_iterator e){
		lst.push_back(string(b, e));
	}, [&](string::const_iterator i){
		return !ystdex::exists(spaces, size_t(i - sbuf.cbegin()));
	});
	return lst;
}
vector<string>
DecomposeMakefileDepList(std::istream& is)
{
	return ystdex::call_value_or(static_cast<vector<string>(&)(
		std::streambuf&)>(DecomposeMakefileDepList), is.rdbuf());
}

bool
FilterMakefileDependencies(vector<string>& lst)
{
	const auto i_c(std::find_if(lst.cbegin(), lst.cend(),
		[](const string& dep){
		return !dep.empty() && dep.back() == ':';
	}));

	return i_c != lst.cend()
		&& (lst.erase(lst.cbegin(), i_c + 1), !lst.empty());
}


void
InstallFile(const char* dst, const char* src)
{
	using namespace YSLib::IO;

	// FIXME: Blocked. TOCTTOU access.
	try
	{
		if(HaveSameContents(dst, src))
			return;
	}
	CatchExpr(std::system_error& e, YTraceDe(Err, "Failed accessing file for"
		" comparison before installing file."), ExtractAndTrace(e, Err))
	CopyFile(dst, src, PreserveModificationTime);
}

void
InstallDirectory(const string& dst, const string& src)
{
	using namespace YSLib::IO;

	TraverseTree([](const Path& dname, const Path& sname){
		InstallFile(string(dname).c_str(), string(sname).c_str());
	}, Path(dst), Path(src));
}

void
InstallHardLink(const char* dst, const char* src)
{
	using namespace YSLib::IO;

	if(VerifyDirectory(src))
		throw std::invalid_argument("Source is a directory.");
	else
		uremove(dst);
	TryExpr(CreateHardLink(dst, src))
	CatchExpr(..., InstallFile(dst, src))
}

void
InstallSymbolicLink(const char* dst, const char* src)
{
	using namespace YSLib::IO;

	uremove(dst);
	TryExpr(CreateSymbolicLink(dst, src))
	CatchExpr(..., InstallFile(dst, src))
}

void
InstallExecutable(const char* dst, const char* src)
{
	InstallFile(dst, src);
}

namespace A1
{

namespace Forms
{

//! \since build 758
namespace
{

void
LoadSequenceSeparators(ContextNode& ctx, EvaluationPasses& passes)
{
	RegisterSequenceContextTransformer(passes, ctx, "$;", TokenValue(";"),
		true),
	RegisterSequenceContextTransformer(passes, ctx, "$,", TokenValue(","));
}

} // unnamed namespace;

void
LoadNPLContextForSHBuild(REPLContext& context)
{
	using namespace std::placeholders;
	auto& root(context.Root);

	LoadSequenceSeparators(root, context.ListTermPreprocess),
	AccessLiteralPassesRef(root)
		= [](TermNode& term, ContextNode&, string_view id) -> ReductionStatus{
		YAssertNonnull(id.data());
		if(!id.empty())
		{
			const char f(id.front());

			// NOTE: Handling extended literals.
			if(IsNPLAExtendedLiteralNonDigitPrefix(f) && id.size() > 1)
			{
				// TODO: Support numeric literal evaluation passes.
				if(id == "#t" || id == "#true")
					term.Value = true;
				else if(id == "#f" || id == "#false")
					term.Value = false;
				else if(id == "#n" || id == "#null")
					term.Value = nullptr;
				// XXX: Redundant test?
				else if(IsNPLAExtendedLiteral(id))
					throw InvalidSyntax(f == '#' ? "Invalid literal found."
						: "Unsupported literal prefix found.");
				else
					return ReductionStatus::Retrying;
			}
			else if(std::isdigit(f))
			{
				errno = 0;

				const auto ptr(id.data());
				char* eptr;
				const long ans(std::strtol(ptr, &eptr, 10));

				if(size_t(eptr - ptr) == id.size() && errno != ERANGE)
					// XXX: Conversion to 'int' might be implementation-defined.
					term.Value = int(ans);
				// TODO: Supported literal postfix?
				else
					throw InvalidSyntax("Literal postfix is unsupported.");
			}
			else
				return ReductionStatus::Retrying;
		}
		return ReductionStatus::Clean;
	};
	// NOTE: Primitive functions including binding constructs.
	RegisterStrict(root, "eval", Eval);
	RegisterForm(root, "$def!", DefineWithNoRecursion);
	RegisterForm(root, "$if", If);
	RegisterForm(root, "$lambda", Lambda);
	RegisterForm(root, "$vau", ystdex::bind1(Vau, _2, false));
	RegisterStrictUnary<ContextHandler>(root, "unwrap", Unwrap);
	// NOTE: Derived functions.
#if true
	// NOTE: Some combiners are provided here as host primitives for
	//	more efficiency and less dependencies.
	RegisterStrict(root, "list", ReduceToList);
#else
	// NOTE: They can be derived as Kernel does.
	context.Perform(u8R"NPL($def! list wrap ($vau x #ignore x))NPL");
//	context.Perform(u8R"NPL($def! list $lambda x x)NPL");
#endif
	context.Perform(u8R"NPL(
		$def! $set! $vau (exp1 formals .exp2) env eval
		(
			list $def! formals (unwrap eval) exp2 env
		) (eval exp1 env);
		$def! $defl! $vau (f formals .body) env eval
		(
			list $set! env f $lambda formals body
		) env;
		$def! $defv! $vau ($f formals senv .body) env eval
		(
			list $set! env $f $vau formals senv body
		) env;
	)NPL");
	RegisterForm(root, "$or?", Or);
	RegisterStrict(root, "eqv?", EqualValue);
	context.Perform("$defl! not (x) eqv? x #f");
	RegisterStrictUnary(root, "ref", ystdex::compose(ReferenceValue,
		ystdex::bind1(std::mem_fn(&TermNode::Value))));
	// NOTE: I/O library.
	RegisterStrictUnary<const string>(root, "puts", [&](const string& str){
		// FIXME: Use %EncodeArg.
		// XXX: Error is ignored.
		std::puts(str.c_str());
	});
	// NOTE: Interoperation library.
	RegisterStrictUnary<const string>(root, "env-get", [&](const string& var){
		string res;

		FetchEnvironmentVariable(res, var.c_str());
		return res;
	});
	RegisterStrict(root, "system", CallSystem);
	RegisterStrict(root, "value-of", ValueOf);
	// NOTE: String library.
	RegisterStrict(root, "++", std::bind(CallBinaryFold<string, ystdex::plus<>>,
		ystdex::plus<>(), string(), _1));
	RegisterStrictBinary(root, "string<-", [](TermNode& x, const TermNode& y){
		Access<string>(x) = Access<string>(y);
		return ValueToken::Unspecified;
	});
	RegisterStrictBinary<string>(root, "string-contains-ci?",
		[](string x, string y){
		// TODO: Extract 'strlwr'.
		const auto to_lwr([](string& s){
			for(auto& c : s)
				c = std::tolower(c);
		});

		to_lwr(x),
		to_lwr(y);
		return x == y;
	});
	RegisterStrictUnary<const TokenValue>(root, "symbol->string",
		SymbolToString);
	// NOTE: SHBuild builtins.
	DefineValue(root, "SHBuild_BaseTerminalHook_",
		ValueObject(std::function<void(const string&, const string&)>(
		[](const string& n, const string& val) ynothrow{
			// XXX: Error from 'std::printf' is ignored.
			std::printf("%s = \"%s\"\n", n.c_str(), val.c_str());
	})), {});
	RegisterStrictUnary<const string>(root, "SHBuild_BuildGCH_existed_",
		[](const string& str) -> bool{
		if(IO::UniqueFile
			file{uopen(str.c_str(), IO::omode_convb(std::ios_base::in))})
			return file->GetSize() > 0;
		return {};
	});
	RegisterStrictUnary<const string>(root, "SHBuild_BuildGCH_mkpdirp_",
		[](const string& str){
		IO::Path pth(str);

		if(!pth.empty())
		{
			pth.pop_back();
			EnsureDirectory(pth);
		}
	});
	RegisterStrict(root, "SHBuild_EchoVar", [&](TermNode& term){
		// XXX: To be overriden if %Terminal is usable (platform specific).
		CallBinaryAs<const string>([&](const string& n, const string& val){
			if(const auto p = FetchValuePtr(root, "SHBuild_BaseTerminalHook_"))
				if(const auto p_hook = AccessPtr<
					std::function<void(const string&, const string&)>>(*p))
					(*p_hook)(n, val);
		}, term);
	});
	RegisterStrict(root, "SHBuild_Install_HardLink", [&](TermNode& term){
		CallBinaryAs<const string>([](const string& dst, const string& src){
			InstallHardLink(dst.c_str(), src.c_str());
		}, term);
	});
	RegisterStrictUnary<const string>(root, "SHBuild_QuoteS_",
		[](const string& str){
		if(str.find('\'') == string::npos)
			return ystdex::quote(str, '\'');
		throw NPLException("Error in quoted string.");
	});
	RegisterStrictUnary<const string>(root, "SHBuild_RaiseError_",
		[](const string& str) YB_NORETURN{
		throw LoggedEvent(str);
	});
	RegisterStrictUnary<const string>(root, "SHBuild_SDot_",
		[](const string& str){
		auto res(str);

		for(auto& c : res)
			if(c == '.')
				c = '_';
		return res;
	});
	RegisterStrictUnary<const string>(root, "SHBuild_TrimOptions_",
		[](const string& str){
		string res;

		for(auto& s : Session(str,
			&LexicalAnalyzer::ParseRaw).Lexer.Literalize())
		{
			res += std::move(s);
			res += ' ';
		}
		if(!res.empty())
			res.pop_back();
		return res;
	});
	// NOTE: Params of %SHBuild_BuildGCH: header = path of header to be copied,
	//	inc = path of header to be included, cmd = tool to build header.
	context.Perform(u8R"NPL(
		$defl! SHBuild_EchoVar_N (var) SHBuild_EchoVar var
			(env-get (SHBuild_SDot_ var));
		$defl! SHBuild_BuildGCH (header inc cmd)
		(
			$def! pch (++ inc ".gch");
			$if (SHBuild_BuildGCH_existed_ pch)
				(puts (++ "PCH file \"" pch "\" exists, skipped building."))
				(
					SHBuild_BuildGCH_mkpdirp_ pch;
					puts (++ "Building precompiled file \"" pch "\" ...");
					SHBuild_Install_HardLink inc header;
					system (++ cmd " \"" header "\" -o" pch);
					puts (++ "Building precompiled file \"" pch "\" done.")
				)
		);
	)NPL");
}

} // namespace Forms;

} // namespace A1;

} // namespace NPL;

