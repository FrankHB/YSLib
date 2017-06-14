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
\version r817
\author FrankHB <frankhb1989@gmail.com>
\since build 623
\par 创建时间:
	2015-08-09 22:14:45 +0800
\par 修改时间:
	2017-06-13 16:01 +0800
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
#include <regex> // for std::regex, std::regex_match;

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
						YB_ATTR_fallthrough;
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

//! \since build 794
//@{
void
CopyEnvironmentDFS(Environment& d, const Environment& e)
{
	const auto copy_parent([&](Environment& dst, const Environment& parent){
		auto p_env(make_shared<Environment>());

		CopyEnvironmentDFS(*p_env, parent);
		dst.ParentPtr = std::move(p_env);
	});
	const auto copy_parent_ptr(
		[&](Environment& dst, const ValueObject& vo) -> bool{
		if(const auto p = AccessPtr<weak_ptr<Environment>>(vo))
		{
			if(const auto p_parent = p->lock())
				copy_parent(dst, *p_parent);
			// XXX: Failure of locking is ignored.
			return true;
		}
		else if(const auto p_e = AccessPtr<shared_ptr<Environment>>(vo))
		{
			if(const auto p_parent = *p_e)
				copy_parent(dst, *p_parent);
			// XXX: Empty parent is ignored.
			return true;
		}
		return {};
	});
	auto& m(d.GetMapRef());

	copy_parent_ptr(d, e.ParentPtr);
	for(const auto& b : e.GetMapRef())
		m.emplace(b.CreateWith([&](const ValueObject& vo) -> ValueObject{
			Environment dst;

			if(copy_parent_ptr(dst, vo))
				return ValueObject(std::move(dst));
			return vo;
		}), b.GetName(), b.Value);
}

void
CopyEnvironment(TermNode& term, ContextNode& ctx)
{
	auto p_env(make_shared<NPL::Environment>());

	CopyEnvironmentDFS(*p_env, ctx.GetRecordRef());
	term.Value = ValueObject(std::move(p_env));
}
//@}

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
	// NOTE: This is named after '#inert' in Kernel, but essentially
	//	unspecified in NPLA.
	root.GetRecordRef().Define("inert", ValueToken::Unspecified, {});
	// NOTE: Primitive features, listed as RnRK, except mentioned above.
/*
	The primitives are provided here to maintain acyclic dependencies on derived
		forms, also for simplicity of semantics.
	The primitives are listed in order as Chapter 4 of Revised^-1 Report on the
		Kernel Programming Language. Derived forms are not ordered likewise.
	There are some difference of listed primitives.
	See $2017-02 @ %Documentation::Workflow::Annual2017.
*/
	RegisterStrict(root, "eqv?", EqualValue);
	// NOTE: Like Scheme but not Kernel, %'$if' treats non-boolean value as
	//	'#f', for zero overhead principle.
	RegisterForm(root, "$if", If);
	RegisterStrictUnary(root, "null?", IsEmpty);
	// NOTE: Though NPLA does not use cons pairs, corresponding primitives are
	//	still necessary.
	// NOTE: Since NPL has no con pairs, it only added head to existed list.
	RegisterStrict(root, "cons", Cons);
	// NOTE: The applicative 'copy-es-immutable' is unsupported currently due to
	//	different implementation of control primitives.
	RegisterStrict(root, "eval", Eval);
	// NOTE: This is now be primitive since in NPL environment capture is more
	//	basic than vau.
	RegisterStrict(root, "get-current-environment", GetCurrentEnvironment);
	RegisterStrict(root, "copy-environment", CopyEnvironment);
	RegisterStrict(root, "make-environment",
		[](TermNode& term, ContextNode& ctx){
		// FIXME: Parent environments?
		GetCurrentEnvironment(term, ctx);
	});
	// NOTE: Environment mutation is optional in Kernel and supported here.
	// NOTE: Lazy form '$deflazy!' is the basic operation, which may bind
	//	parameter as unevaluated operands. For zero overhead principle, the form
	//	without recursion (named '$def!') is preferred. The recursion variant
	//	(named '$defrec!') is exact '$define!' in Kernel, and is used only when
	//	necessary.
	RegisterForm(root, "$deflazy!", DefineLazy);
	RegisterForm(root, "$def!", DefineWithNoRecursion);
	RegisterForm(root, "$defrec!", DefineWithRecursion);
	RegisterForm(root, "$lambda", Lambda);
	RegisterForm(root, "$vau", Vau);
	RegisterStrictUnary<ContextHandler>(root, "wrap", Wrap);
	RegisterStrictUnary<ContextHandler>(root, "unwrap", Unwrap);
	// NOTE: Derived functions.
#if true
	// NOTE: Some combiners are provided here as host primitives for
	//	more efficiency and less dependencies.
	// NOTE: The sequence operator is also available as infix ';' syntax sugar.
	RegisterForm(root, "$sequence", ReduceOrdered);
	RegisterStrict(root, "list", ReduceToList);
#else
	// NOTE: They can be derived as Kernel does.
	// XXX: The current environment is better to be saved by
	//	'$lambda () () get-current-environment'.
	// TODO: Avoid redundant environment copy.
	context.Perform(u8R"NPL(
		$def! $sequence
		(
			wrap
			(
				$vau ($seq2) #ignore
				(
					$seq2
					(
						$defrec! $aux $vaue (() copy-environment) (head .tail)
							env
						(
							$if (null? tail)
							(eval head env)
							(
								$seq2 (eval head env)
									(eval (cons $aux tail) env)
							)
						)
					)
					(
						$vaue (() copy-environment) body env
						(
							$if (null? body)
							inert
							(eval (cons $aux body) env)
						)
					)
				)
			)
		)
		(
			$vau (first second) env
				(wrap ($vau #ignore #ignore (eval second env))) (eval first env)
		);
	)NPL");
	context.Perform(u8R"NPL($def! list wrap ($vau x #ignore x))NPL");
//	context.Perform(u8R"NPL($def! list $lambda x x)NPL");
#endif
	context.Perform(u8R"NPL(
		$def! first $lambda ((x .)) x;
		$def! rest $lambda ((#ignore .x)) x;
		$def! apply $lambda (appv arg .opt)
			eval (cons () (cons (unwrap appv) arg))
				($if (null? opt) (() make-environment) (first opt));
		$defrec! list* $lambda (head .tail)
			$if (null? tail) head
				(cons head (apply list* tail));
		$defrec! $cond $vau clauses env $sequence
		(
			$def! aux $lambda ((test .body) .clauses)
				$if (eval test env)
					(apply (wrap $sequence) body env)
					(apply (wrap $cond) clauses env)
		)
		($if (null? clauses) inert (apply aux clauses));
		$def! $set! $vau (expr1 formals .expr2) env eval
			(list $def! formals (unwrap eval) expr2 env) (eval expr1 env);
		$def! $defl! $vau (f formals .body) env eval
			(list $set! env f $lambda formals body) env;
		$def! $defv! $vau ($f formals senv .body) env eval
			(list $set! env $f $vau formals senv body) env;
		$def! $defw! $vau (f formals senv .body) env eval
			(list $set! env f wrap (list* $vau formals senv body)) env;
		$defl! first-null? (l) null? (first l);
		$defl! list-rest (x) list (rest x);
		$defl! accl (l pred? base head tail sum)
		(
			$defrec! aux $lambda (l base)
				$if (pred? l) base (aux (tail l) (sum (head l) base));
			aux l base
		);
		$defl! accr (l pred? base head tail sum)
		(
			$defrec! aux $lambda (l)
				$if (pred? l) base (sum (head l) (aux (tail l)));
			aux l
		);
		$defl! foldr1 (kons knil l) accr l null? knil first rest kons;
		$defw! map1 (appv l) env foldr1
			($lambda (x xs) cons (apply appv (list x) env) xs) () l;
		$defv! $let (bindings .body) env
			eval (list* () (list* $lambda (map1 first bindings) body)
				(map1 list-rest bindings)) env;
		$defrec! $let* $vau (bindings .body) env
			eval ($if (null? bindings) (list* $let bindings body)
				(list $let (list (first bindings))
				(list* $let* (rest bindings) body))) env;
	)NPL");
	// NOTE: Use of 'eqv?' is more efficient than '$if'.
	context.Perform(u8R"NPL(
		$defl! not? (x) eqv? x #f;
		$defv! $when (test .vexpr) env $if (eval test env)
			(eval (list* $sequence vexpr) env);
		$defv! $unless (test .vexpr) env $if (not? (eval test env))
			(eval (list* $sequence vexpr) env);
	)NPL");
	RegisterForm(root, "$and?", And);
	RegisterForm(root, "$or?", Or);
	context.Perform(u8R"NPL(
		$defl! unfoldable? (l) accr l null? (first-null? l) first-null? rest
			$or?;
		$def! map-reverse wrap
		(
			$defl! cxrs (ls cxr) accl ls null? () ($lambda (l) cxr (first l))
				rest cons;
			$vau (appv .ls) env accl ls unfoldable? ()
				($lambda (ls) cxrs ls first) ($lambda (ls) cxrs ls rest)
				($lambda (x xs) cons (apply appv x env) xs)
		);
		$defw! for-each-ltr ls env $sequence (apply map-reverse ls env) inert;
	)NPL");
	RegisterStrictUnary(root, "ref", ystdex::compose(ReferenceValue,
		ystdex::bind1(std::mem_fn(&TermNode::Value))));
	// NOTE: Environments.
	RegisterStrictUnary(root, "bound?",
		[](TermNode& term, const ContextNode& ctx){
		return ystdex::call_value_or([&](string_view id){
			return CheckSymbol(id, [&](){
				return bool(ResolveName(ctx, id));
			});
		}, AccessPtr<string>(term));
	});
	context.Perform(u8R"NPL(
		$defv! $binds1? (expr s) env
			eval (list (unwrap bound?) (symbol->string s)) (eval expr env);
	)NPL");
	RegisterStrict(root, "value-of", ValueOf);
	// NOTE: String library.
	RegisterStrict(root, "++", std::bind(CallBinaryFold<string, ystdex::plus<>>,
		ystdex::plus<>(), string(), _1));
	RegisterStrictUnary<const string>(root, "string-empty?",
		std::mem_fn(&string::empty));
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
		return x.find(y) != string::npos;
	});
	RegisterStrictUnary<const string>(root, "string->symbol", StringToSymbol);
	RegisterStrictUnary<const TokenValue>(root, "symbol->string",
		SymbolToString);
	RegisterStrictUnary<const string>(root, "string->regex",
		[&](const string& str){
		return std::regex(str);
	});
	RegisterStrict(root, "regex-match?", [](TermNode& term){
		auto i(std::next(term.begin()));
		const auto& str(Access<const string>(Deref(i)));
		const auto& r(Access<const std::regex>(Deref(++i)));

		term.ClearTo(std::regex_match(str, r));
	}, ystdex::bind1(RetainN, 2));
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
	context.Perform(u8R"NPL(
		$defl! env-empty? (n) string-empty? (env-get n);
	)NPL");
	RegisterStrict(root, "system", CallSystem);
	// NOTE: SHBuild builtins.
	root.GetRecordRef().Define("SHBuild_BaseTerminalHook_",
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
	RegisterStrictUnary<const string>(root, "SHBuild_EnsureDirectory_",
		[](const string& str){
		EnsureDirectory(IO::Path(str));
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
			if(const auto p = FetchValuePtr(root.GetRecordRef(),
				"SHBuild_BaseTerminalHook_"))
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
		[](const string& src){
		string res;

		for(const auto& str
			: Session(src, [&](LexicalAnalyzer& lexer, char c){
			lexer.ParseByte(c, NPLUnescape, IgnorePrefix);
		}).Lexer.Literalize())
		{
			using iter_t = string::const_iterator;

			ystdex::split_l(str.cbegin(), str.cend(), static_cast<int(&)(int)>(
				std::isspace), [&](iter_t b, iter_t e){
				string s(b, e);

				ystdex::trim(s);
				if(!s.empty())
				{
					res += s;
					res += ' ';
				}
			});
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

