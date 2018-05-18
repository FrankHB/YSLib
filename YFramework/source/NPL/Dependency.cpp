/*
	© 2015-2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Dependency.cpp
\ingroup NPL
\brief 依赖管理。
\version r1209
\author FrankHB <frankhb1989@gmail.com>
\since build 623
\par 创建时间:
	2015-08-09 22:14:45 +0800
\par 修改时间:
	2018-05-09 02:43 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::Dependency
*/


#include "NPL/YModules.h"
#include YFM_NPL_Dependency // for ystdex::isspace, std::placeholders,
//	ystdex::isdigit, ystdex::tolower, ystdex::bind1;
#include YFM_NPL_SContext
#include YFM_YSLib_Service_FileSystem // for YSLib::IO::*;
#include <iterator> // for std::istreambuf_iterator;
#include <limits> // for std::numeric_limits;
#include <cerrno> // for errno, ERANGE;
#include <cstdio> // for std::puts;
#include <regex> // for std::regex, std::regex_match;
#include <ystdex/string.hpp> // for ystdex::begins_with, ystdex::erase_left;

using namespace YSLib;

namespace NPL
{

// NOTE: The following options provide documented alternative implementations.
//	Except implementation of '$sequence', they shall still be conforming. Native
//	implementation of forms should provide better performance in general.

// NOTE: For general native implementations.
#define YF_Impl_NPLA1_Enable_NativeForms true
// NOTE: For awareness of strong ownership of environments.
#define YF_Impl_NPLA1_Enable_LockEnvironment true

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

	ystdex::split_if(sbuf.begin(), sbuf.end(), ystdex::isspace,
		[&](string::const_iterator b, string::const_iterator e){
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

//! \since build 823
void
LoadSequenceSeparators(EvaluationPasses& passes)
{
	RegisterSequenceContextTransformer(passes, TokenValue(";"), true),
	RegisterSequenceContextTransformer(passes, TokenValue(","));
}

//! \since build 794
//@{
void
CopyEnvironmentDFS(Environment& d, const Environment& e)
{
	// TODO: Support more implementations?
	const auto copy_parent([&](Environment& dst, const Environment& parent){
		auto p_env(make_shared<Environment>());

		CopyEnvironmentDFS(*p_env, parent);
		dst.Parent = std::move(p_env);
	});
	const auto copy_parent_ptr(
		[&](Environment& dst, const ValueObject& vo) -> bool{
		if(const auto p = AccessPtr<EnvironmentReference>(vo))
		{
			if(const auto p_parent = p->Lock())
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

	copy_parent_ptr(d, e.Parent);
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
	auto& root_env(root.GetRecordRef());

	LoadSequenceSeparators(context.ListTermPreprocess),
	root.EvaluateLiteral
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
			else if(ystdex::isdigit(f))
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
	root_env.Define("inert", ValueToken::Unspecified, {});
	// NOTE: This is like '#ignore' in Kernel, but with the symbol type. An
	//	alternative definition is by evaluating '$def! ignore $quote #ignore'
	//	(see below for '$def' and '$quote').
	root_env.Define("ignore", TokenValue("#ignore"), {});
	// NOTE: Primitive features, listed as RnRK, except mentioned above.
/*
	The primitives are provided here to maintain acyclic dependencies on derived
		forms, also for simplicity of semantics.
	The primitives are listed in order as Chapter 4 of Revised^-1 Report on the
		Kernel Programming Language. Derived forms are not ordered likewise.
	There are some difference of listed primitives.
	See $2017-02 @ %Documentation::Workflow::Annual2017.
*/
	RegisterStrict(root, "eq?", Equal);
	RegisterStrict(root, "eql?", EqualLeaf);
	RegisterStrict(root, "eqr?", EqualReference);
	RegisterStrict(root, "eqv?", EqualValue);
	// NOTE: Like Scheme but not Kernel, '$if' treats non-boolean value as
	//	'#f', for zero overhead principle.
	RegisterForm(root, "$if", If);
	RegisterStrictUnary(root, "null?", ComposeReferencedTermOp(IsEmpty));
	RegisterStrictUnary(root, "nullpr?", IsEmpty);
	// NOTE: Though NPLA does not use cons pairs, corresponding primitives are
	//	still necessary.
	// NOTE: Since NPL has no con pairs, it only added head to existed list.
	RegisterStrict(root, "cons", Cons);
	RegisterStrict(root, "cons&", ConsRef);
	// NOTE: The applicative 'copy-es-immutable' is unsupported currently due to
	//	different implementation of control primitives.
	RegisterStrict(root, "eval", Eval);
	RegisterStrict(root, "eval&", EvalRef);
	// NOTE: This is now be primitive since in NPL environment capture is more
	//	basic than vau.
	RegisterStrict(root, "copy-environment", CopyEnvironment);
	RegisterStrict(root, "make-environment", MakeEnvironment);
	RegisterStrict(root, "get-current-environment", GetCurrentEnvironment);
	RegisterStrictUnary<shared_ptr<Environment>>(root, "weaken-environment",
		[](const shared_ptr<Environment>& p) ynothrow{
		return EnvironmentReference(p);
	});
	RegisterStrictUnary<const EnvironmentReference>(root, "lock-environment",
		std::mem_fn(&EnvironmentReference::Lock));
	// NOTE: Environment mutation is optional in Kernel and supported here.
	// NOTE: Lazy form '$deflazy!' is the basic operation, which may bind
	//	parameter as unevaluated operands. For zero overhead principle, the form
	//	without recursion (named '$def!') is preferred. The recursion variant
	//	(named '$defrec!') is exact '$define!' in Kernel, and is used only when
	//	necessary.
	RegisterForm(root, "$deflazy!", DefineLazy);
	RegisterForm(root, "$def!", DefineWithNoRecursion);
	RegisterForm(root, "$defrec!", DefineWithRecursion);
	// NOTE: 'eqv? (() get-current-environment) (() ($vau () e e))' shall
	//	be evaluated to '#t'.
	RegisterForm(root, "$vau", Vau);
	RegisterForm(root, "$vau&", VauRef);
	RegisterForm(root, "$vaue", VauWithEnvironment);
	RegisterForm(root, "$vaue&", VauWithEnvironmentRef);
	RegisterStrictUnary<ContextHandler>(root, "wrap", Wrap);
	RegisterStrictUnary<ContextHandler>(root, "unwrap", Unwrap);
	// NOTE: Derived core functions.
#if YF_Impl_NPLA1_Enable_NativeForms
	RegisterStrict(root, "list", ReduceBranchToListValue);
#else
	context.Perform(u8R"NPL($def! list wrap ($vau &x #ignore x))NPL");
//	context.Perform(u8R"NPL($def! list $lambda &x x)NPL");
#endif
	RegisterStrict(root, "list&", ReduceBranchToList);
	context.Perform(u8R"NPL(
		$def! $quote $vau (&x) #ignore x;
		$def! $set! $vau (&expr1 &formals .&expr2) env eval
			(list $def! formals (unwrap eval) expr2 env) (eval expr1 env);
		$def! $defv! $vau (&$f &formals &senv .&body) env eval
			(list $set! env $f $vau formals senv body) env;
		$def! $defv&! $vau (&$f &formals &senv .&body) env eval
			(list $set! env $f $vau& formals senv body) env;
	)NPL");
#if YF_Impl_NPLA1_Enable_NativeForms
	RegisterForm(root, "$lambda", Lambda);
	RegisterForm(root, "$lambda&", LambdaRef);
#else
	context.Perform(u8R"NPL(
		$defv! $lambda (&formals .&body) env
			wrap (eval (cons $vau (cons formals (cons ignore body))) env);
		$defv! $lambda& (&formals .&body) env
			wrap (eval (cons $vau& (cons formals (cons ignore body))) env);
	)NPL");
#endif
	context.Perform(u8R"NPL(
		$defv! $setrec! (&expr1 &formals .&expr2) env eval
			(list $defrec! formals (unwrap eval) expr2 env) (eval expr1 env);
		$defv! $defl! (&f &formals .&body) env eval
			(list $set! env f $lambda formals body) env;
		$defv! $defl&! (&f &formals .&body) env eval
			(list $set! env f $lambda& formals body) env;
		$defl! first ((&x .)) x;
		$defl! rest ((#ignore .&x)) x;
		$defl! apply (&appv &arg .&opt) eval (cons () (cons (unwrap appv) arg))
			($if (null? opt) (() make-environment) (first opt));
		$defl! list* (&head .&tail)
			$if (null? tail) head (cons head (apply list* tail));
		$defv! $defw! (&f &formals &senv .&body) env eval
			(list $set! env f wrap (list* $vau formals senv body)) env;
		$defv! $defw&! (&f &formals &senv .&body) env eval
			(list $set! env f wrap (list* $vau& formals senv body)) env;
		$defv! $lambdae (&e &formals .&body) env
			wrap (eval (list* $vaue e formals ignore body) env);
		$defv! $lambdae& (&e &formals .&body) env
			wrap (eval (list* $vaue& e formals ignore body) env);
	)NPL");
#if YF_Impl_NPLA1_Enable_NativeForms
	// NOTE: Some combiners are provided here as host primitives for
	//	more efficiency and less dependencies.
	// NOTE: The sequence operator is also available as infix ';' syntax sugar.
	RegisterForm(root, "$sequence", Sequence);
#else
	// TODO: Support move-only types at end?
	// TODO: PTC support.
	context.Perform(u8R"NPL(
		$def! $sequence
			($lambda (&cenv)
				($lambda #ignore $vaue cenv &body env
					$if (null? body) inert (eval (cons $aux body) env))
				($set! cenv $aux $vaue (weaken-environment cenv) (&head .&tail)
					env $if (null? tail) (eval head env)
						(($vau (&t) e ($lambda #ignore (eval t e))
							(eval head env)) (eval (cons $aux tail) env))))
			(make-environment (() get-current-environment));
	)NPL");
#endif
	context.Perform(u8R"NPL(
		$defv! $cond &clauses env
			$if (null? clauses) inert (apply ($lambda ((&test .&body) .clauses)
				$if (eval test env) (eval body env)
					(apply (wrap $cond) clauses env)) clauses);
	)NPL");
#if YF_Impl_NPLA1_Enable_LockEnvironment
	context.Perform(u8R"NPL(
		$def! make-standard-environment
			$lambda () lock-environment (() get-current-environment);
	)NPL");
#else
	context.Perform(u8R"NPL(
		$def! make-standard-environment
			($lambda (&cenv &env)
				($lambda #ignore $vaue cenv () #ignore (make-environment senv))
				($set! cenv senv env))
			(make-environment (() get-current-environment))
			(() get-current-environment);
	)NPL");
#endif
	// NOTE: Use of 'eql?' is more efficient than '$if'.
	context.Perform(u8R"NPL(
		$defl! not? (&x) eql? x #f;
		$defv! $when (&test .&vexpr) env $if (eval test env)
			(eval (list* $sequence vexpr) env);
		$defv! $unless (&test .&vexpr) env $if (not? (eval test env))
			(eval (list* $sequence vexpr) env);
	)NPL");
	RegisterForm(root, "$and?", And);
	RegisterForm(root, "$or?", Or);
	context.Perform(u8R"NPL(
		$defl! first-null? (&l) null? (first l);
		$defl! list-rest (&x) list (rest x);
		$defl! accl (&l &pred? &base &head &tail &sum) $sequence
			($defl! aux (&l &base)
				$if (pred? l) base (aux (tail l) (sum (head l) base)))
			(aux l base);
		$defl! accr (&l &pred? &base &head &tail &sum) $sequence
			($defl! aux (&l) $if (pred? l) base (sum (head l) (aux (tail l))))
			(aux l);
		$defl! foldr1 (&kons &knil &l) accr l null? knil first rest kons;
		$defw! map1 (&appv &l) env foldr1
			($lambda (&x &xs) cons (apply appv (list x) env) xs) () l;
		$defl! list-concat (&x &y) foldr1 cons y x;
		$defl! append (.&ls) foldr1 list-concat () ls;
		$defv! $let (&bindings .&body) env
			eval (list* () (list* $lambda (map1 first bindings) body)
				(map1 list-rest bindings)) env;
		$defv! $let* (&bindings .&body) env
			eval ($if (null? bindings) (list* $let bindings body)
				(list $let (list (first bindings))
				(list* $let* (rest bindings) body))) env;
	)NPL");
	context.Perform(u8R"NPL(
		$defl! unfoldable? (&l) accr l null? (first-null? l) first-null? rest
			$or?;
		$def! map-reverse $let ((&cenv () make-standard-environment)) wrap
			($sequence
				($set! cenv cxrs $lambdae (weaken-environment cenv) (&ls &cxr)
					accl ls null? () ($lambda (&l) cxr (first l)) rest cons)
				($vaue cenv (&appv .&ls) env accl ls unfoldable? ()
					($lambda (&ls) cxrs ls first) ($lambda (&ls) cxrs ls rest)
						($lambda (&x &xs) cons (apply appv x env) xs)));
		$defw! for-each-ltr &ls env $sequence (apply map-reverse ls env) inert;
	)NPL");
	// NOTE: Object interoperation.
	RegisterStrict(root, "ref", [](TermNode& term){
		CallUnary([&](TermNode& tm){
			LiftToReference(term, tm);
		}, term);
		return CheckNorm(term);
	});
	// NOTE: Environments.
	RegisterStrictUnary(root, "bound?",
		[](TermNode& term, const ContextNode& ctx){
		return ystdex::call_value_or([&](string_view id){
			return CheckSymbol(id, [&](){
				return bool(ResolveName(ctx, id).first);
			});
		}, AccessTermPtr<string>(term));
	});
	context.Perform(u8R"NPL(
		$defv! $binds1? (&expr &s) env
			eval (list (unwrap bound?) (symbol->string s)) (eval expr env);
	)NPL");
	RegisterStrict(root, "value-of", ValueOf);
	// NOTE: String library.
	RegisterStrict(root, "++", std::bind(CallBinaryFold<string, ystdex::plus<>>,
		ystdex::plus<>(), string(), _1));
	RegisterStrictUnary<const string>(root, "string-empty?",
		std::mem_fn(&string::empty));
	RegisterStrictBinary(root, "string<-", [](TermNode& x, const TermNode& y){
		AccessTerm<string>(x) = AccessTerm<string>(y);
		return ValueToken::Unspecified;
	});
	RegisterStrictBinary<string>(root, "string-contains-ci?",
		[](string x, string y){
		// TODO: Extract 'strlwr'.
		const auto to_lwr([](string& s){
			for(auto& c : s)
				c = ystdex::tolower(c);
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
		const auto& str(AccessTerm<const string>(Deref(i)));
		const auto& r(AccessTerm<const std::regex>(Deref(++i)));

		term.ClearTo(std::regex_match(str, r));
	}, ystdex::bind1(RetainN, 2));
	// NOTE: I/O library.
	RegisterStrictUnary<const string>(root, "puts", [&](const string& str){
		// FIXME: Use %EncodeArg?
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
		$defl! env-empty? (&n) string-empty? (env-get n);
	)NPL");
	RegisterStrict(root, "system", CallSystem);
	// NOTE: SHBuild builtins.
	root_env.Define("SHBuild_BaseTerminalHook_",
		ValueObject(std::function<void(const string&, const string&)>(
		[](const string& n, const string& val) ynothrow{
			// XXX: Error from 'std::printf' is ignored.
			std::printf("%s = \"%s\"\n", n.c_str(), val.c_str());
	})), {});
	RegisterStrictUnary<const string>(root, "system-quote", [](const string& w){
		return !w.empty() ? ((CheckLiteral(w) == char() && (w.find(' ')
			!= string::npos || w.find('\t') != string::npos))
			|| (w.front() == '\'' || w.front() == '"' || w.back() == '\''
			|| w.back() == '"') ? ystdex::quote(w, '"') : w) : "\"\"";
	});
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
			if(const auto p = GetValuePtrOf(root_env.LookupName(
				"SHBuild_BaseTerminalHook_")))
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
	RegisterStrictBinary<const string>(root, "SHBuild_RemovePrefix_",
		[&](const string& str, const string& pfx) -> string{
		if(ystdex::begins_with(str, pfx))
			return str.substr(pfx.length());
		return str;
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
		Session sess(src, [&](LexicalAnalyzer& lexer, char c){
			lexer.ParseByte(c, NPLUnescape, IgnorePrefix);
		});
		const auto& lexer(sess.Lexer);
		const auto& left_qset(lexer.GetLeftQuotes());
		typename string::size_type l(0);

		for(const auto& str : lexer.Literalize())
			if(!str.empty())
			{
				using iter_t = string::const_iterator;

				// XXX: As %NPL::Tokenize.
				if(str.front() != '\'' && str.front() != '"')
					ystdex::split_l(str.cbegin(), str.cend(), ystdex::isspace,
						[&](iter_t b, iter_t e){
						string s(b, e);

						ystdex::trim(s);
						if(!s.empty())
						{
							res += s;
							res += ' ';
						}
					});
				else
				{
					if(!res.empty() && l != 0 && left_qset.count(l) != 0
						&& !ystdex::isspace(src[l - 1]))
						res.pop_back();
					res += str;
					res += ' ';
				}
				l += str.length();
			}
		if(!res.empty())
			res.pop_back();
		return res;
	});
}

} // namespace Forms;

} // namespace A1;

} // namespace NPL;

