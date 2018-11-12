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
\version r2120
\author FrankHB <frankhb1989@gmail.com>
\since build 623
\par 创建时间:
	2015-08-09 22:14:45 +0800
\par 修改时间:
	2018-11-10 22:16 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::Dependency
*/


#include "NPL/YModules.h"
#include YFM_NPL_Dependency // for ystdex::isspace, std::istream,
//	YSLib::unique_ptr, ystdex::isdigit, ystdex::bind1, std::placeholders,
//	ystdex::tolower, ystdex::swap_dependent;
#include YFM_NPL_SContext
#include YFM_YSLib_Service_FileSystem // for YSLib::IO::*;
#include <ystdex/iterator.hpp> // for std::istreambuf_iterator,
//	ystdex::make_transform;
#include YFM_YSLib_Service_TextFile // for IO::SharedInputMappedFileStream,
//	Text::OpenSkippedBOMtream;
#include <cerrno> // for errno, ERANGE;
#include <cstdio> // for std::puts;
#include <regex> // for std::regex, std::regex_match;
#include YFM_YSLib_Core_YCoreUtilities // for FetchCommandOutput;
#include <ystdex/string.hpp> // for ystdex::begins_with;

using namespace YSLib;

namespace NPL
{

// NOTE: The following options provide documented alternative implementations.
//	Except implementation of '$sequence', they shall still be conforming. Native
//	implementation of forms should provide better performance in general.

//! \since build 837
//@{
// NOTE: For general native implementations.
#define YF_Impl_NPLA1_Native_Forms true
// NOTE: For environment primitive native implemantations.
#define YF_Impl_NPLA1_Native_EnvironmentPrimitives true
// NOTE: For '$vau' in 'id' derivations instead of '$lambda'.
#define YF_Impl_NPLA1_Use_Id_Vau true
// NOTE: For awareness of strong ownership of environments.
#define YF_Impl_NPLA1_Use_LockEnvironment true
//@}

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

YB_NONNULL(1) YSLib::unique_ptr<std::istream>
OpenFile(const char* filename)
{
	return Text::OpenSkippedBOMtream<IO::SharedInputMappedFileStream>(
		Text::BOM_UTF_8, filename);
}

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

//! \since build 842
void
LoadBasicProcessing(ContextState& ctx)
{
	ctx.EvaluateLiteral
		= [](TermNode& term, ContextNode&, string_view id) -> ReductionStatus{
		YAssertNonnull(id.data());
		if(!id.empty())
		{
			const char f(id.front());

			// NOTE: Extended literals handling.
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
					throw InvalidSyntax(sfmt(f != '#' ? "Unsupported literal"
						" prefix found in literal '%s'."
						: "Invalid literal '%s' found.", id.data()));
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
}

//! \since build 834
//@{
namespace Ground
{

void
LoadObjects(Environment& env)
{
	// NOTE: This is named after '#inert' in Kernel, but essentially
	//	unspecified in NPLA.
	env.Define("inert", ValueToken::Unspecified, {});
	// NOTE: This is like '#ignore' in Kernel, but with the symbol type. An
	//	alternative definition is by evaluating '$def! ignore $quote #ignore'
	//	(see '$def!' and '$quote').
	env.Define("ignore", TokenValue("#ignore"), {});
}

namespace Primitive
{

void
LoadEquals(ContextNode& ctx)
{
	RegisterStrict(ctx, "eq?", Equal);
	RegisterStrict(ctx, "eql?", EqualLeaf);
	RegisterStrict(ctx, "eqr?", EqualReference);
	RegisterStrict(ctx, "eqv?", EqualValue);
}

void
LoadControl(ContextNode& ctx)
{
	// NOTE: Like Scheme but not Kernel, '$if' treats non-boolean value as
	//	'#f', for zero overhead principle.
	RegisterForm(ctx, "$if", If);
}

void
LoadLists(ContextNode& ctx)
{
	RegisterStrictUnary(ctx, "null?", ComposeReferencedTermOp(IsEmpty));
	RegisterStrictUnary(ctx, "nullpr?", IsEmpty);
	RegisterStrictUnary(ctx, "reference?", IsReferenceTerm);
	RegisterStrictUnary(ctx, "lvalue?", IsLValueTerm);
	RegisterStrict(ctx, "move", [](TermNode& term){
		RetainN(term);
		LiftTerm(term, ReferenceTerm(Deref(std::next(term.begin()))));
		LiftToReturn(term);
		return CheckNorm(term);
	});
	// NOTE: Though NPLA does not use cons pairs, corresponding primitives are
	//	still necessary.
	// NOTE: Since NPL has no con pairs, it only added head to existed list.
	RegisterStrict(ctx, "cons", Cons);
	RegisterStrict(ctx, "cons%", ConsRef);
	// NOTE: Like '$set-car!' in Kernel, with no references.
	RegisterStrict(ctx, "set-first!", SetFirst);
	// NOTE: Like '$set-car!' in Kernel.
	RegisterStrict(ctx, "set-first%!", SetFirstRef);
	// NOTE: Like '$set-cdr!' in Kernel, with no references.
	RegisterStrict(ctx, "set-rest!", SetRest);
	// NOTE: Like '$set-cdr!' in Kernel.
	RegisterStrict(ctx, "set-rest%!", SetRestRef);
}

void
LoadEnvironments(ContextNode& ctx)
{
	// NOTE: The applicative 'copy-es-immutable' is unsupported currently due to
	//	different implementation of control primitives.
	RegisterStrict(ctx, "eval", Eval);
	RegisterStrict(ctx, "eval%", EvalRef);
	RegisterStrictUnary<const TokenValue>(ctx, "resolve-identifier",
		[](string_view id, const ContextNode& c){
		return CheckSymbol(id, [&]{
			return ResolveIdentifier(c, id).first;
		});
	});
	// NOTE: This is now be primitive since in NPL environment capture is more
	//	basic than vau.
	RegisterStrict(ctx, "copy-environment", CopyEnvironment);
	RegisterStrictUnary<const EnvironmentReference>(ctx, "lock-environment",
		std::mem_fn(&EnvironmentReference::Lock));
	RegisterStrict(ctx, "make-environment", MakeEnvironment);
	RegisterStrictUnary<const shared_ptr<Environment>>(ctx,
		"weaken-environment", [](const shared_ptr<Environment>& p) ynothrow{
		return EnvironmentReference(p);
	});
	// NOTE: Environment mutation is optional in Kernel and supported here.
	// NOTE: Lazy form '$deflazy!' is the basic operation, which may bind
	//	parameter as unevaluated operands. For zero overhead principle, the form
	//	without recursion (named '$def!') is preferred. The recursion variant
	//	(named '$defrec!') is exact '$define!' in Kernel, and is used only when
	//	necessary.
	RegisterForm(ctx, "$deflazy!", DefineLazy);
	RegisterForm(ctx, "$def!", DefineWithNoRecursion);
	RegisterForm(ctx, "$defrec!", DefineWithRecursion);
}

void
LoadCombiners(ContextNode& ctx)
{
	// NOTE: For ground environment applicatives (see below for
	//	'get-current-environment'), the result of evaluation of expression
	//	'eqv? (() get-current-environment) (() ($vau () d d))' shall be '#t'.
	RegisterForm(ctx, "$vau/e", VauWithEnvironment);
	RegisterForm(ctx, "$vau/e%", VauWithEnvironmentRef);
	RegisterStrictUnary<ContextHandler>(ctx, "wrap", Wrap);
	RegisterStrictUnary<ContextHandler>(ctx, "unwrap", Unwrap);
}

void
Load(ContextNode& ctx)
{
	// NOTE: Primitive features, listed as RnRK, except mentioned above.
/*
	The primitives are provided here to maintain acyclic dependencies on derived
		forms, also for simplicity of semantics.
	The primitives are listed in order as Chapter 4 of Revised^-1 Report on the
		Kernel Programming Language. Derived forms are not ordered likewise.
	There are some difference of listed primitives.
	See $2017-02 @ %Documentation::Workflow::Annual2017.
*/
	LoadEquals(ctx);
	LoadControl(ctx);
	LoadLists(ctx);
	LoadEnvironments(ctx);
	LoadCombiners(ctx);
}

} // namespace Primitive;

namespace Derived
{

//! \since build 839
//@{
void
LoadPrimitive(REPLContext& context)
{
	auto& renv(context.Root.GetRecordRef());

	// NOTE: Some combiners are provided here as host primitives for
	//	more efficiency and less dependencies.
#if YF_Impl_NPLA1_Native_Forms || YF_Impl_NPLA1_Native_EnvironmentPrimitives
	RegisterStrict(renv, "get-current-environment", GetCurrentEnvironment);
	RegisterStrict(renv, "lock-current-environment", LockCurrentEnvironment);
#endif
#if YF_Impl_NPLA1_Native_Forms || !YF_Impl_NPLA1_Native_EnvironmentPrimitives
	RegisterForm(renv, "$vau", Vau);
	RegisterForm(renv, "$vau%", VauRef);
#endif
#if YF_Impl_NPLA1_Native_Forms
	RegisterStrict(renv, "id", [](TermNode& term){
		RetainN(term);
		LiftTerm(term, Deref(std::next(term.begin())));
		return CheckNorm(term);
	});
	RegisterStrict(renv, "idv", [](TermNode& term){
		RetainN(term);
		LiftTerm(term, Deref(std::next(term.begin())));
		LiftToReturn(term);
		return CheckNorm(term);
	});
	RegisterStrict(renv, "list", ReduceBranchToListValue);
	RegisterStrict(renv, "list%", ReduceBranchToList);
	RegisterForm(renv, "$lambda", Lambda);
	RegisterForm(renv, "$lambda%", LambdaRef);
	// NOTE: The sequence operator is also available as infix ';' syntax sugar.
	RegisterForm(renv, "$sequence", Sequence);
#else
#	if YF_Impl_NPLA1_Native_EnvironmentPrimitives
	context.Perform(u8R"NPL(
		$def! $vau $vau/e (() get-current-environment) (&formals &e .&body) d
			eval (cons $vau/e (cons d (cons formals (cons e body)))) d;
		$def! $vau% $vau (&formals &senv .&body) d
			eval (cons $vau/e% (cons d (cons formals (cons senv body)))) d;
	)NPL");
	RegisterForm(renv, "$vau%", VauRef);
#	else
	context.Perform(u8R"NPL(
		$def! get-current-environment (wrap ($vau () d d));
		$def! lock-current-environment (wrap ($vau () d lock-environment d));
	)NPL");
#	endif
	// XXX: The operative '$set!' is same to following derivations.
	context.Perform(u8R"NPL(
		$def! $set! $vau (&e &formals .&expr) d
			eval (list $def! formals (unwrap eval) expr d) (eval e d);
	)NPL");
#	if YF_Impl_NPLA1_Use_Id_Vau
	// NOTE: The parameter shall be in list explicitly as '(.x)' to lift
	//	elements by value rather than by reference (as '&x'), otherwise resulted
	//	'list' is wrongly implemented as 'list%' with undefined behavior becuase
	//	it is not guaranteed the operand is alive to access without lifting.
	//	This is not required in Kernel as it does not differentiate lvalues
	//	(first-class referents) from prvalues and all terms can be accessed as
	//	objects with arbitrary longer lifetime.
	context.Perform(u8R"NPL(
		$def! id wrap ($vau% (%x) #ignore x);
		$def! idv wrap ($vau (&x) #ignore x);
		$def! list wrap ($vau (.x) #ignore x);
		$def! list% wrap ($vau &x #ignore x);
	)NPL");
	// XXX: The operative '$defv!' is same to following derivations.
	context.Perform(u8R"NPL(
		$def! $defv! $vau (&$f &formals &senv .&body) d
			eval (list $set! d $f $vau formals senv body) d;
		$defv! $lambda (&formals .&body) d
			wrap (eval (cons $vau (cons formals (cons ignore body))) d);
		$defv! $lambda% (&formals .&body) d
			wrap (eval (cons $vau% (cons formals (cons ignore body))) d);
	)NPL");
#	else
	RegisterForm(renv, "$lambda", Lambda);
	RegisterForm(renv, "$lambda%", LambdaRef);
	context.Perform(u8R"NPL(
		$def! id $lambda% (%x) x;
		$def! idv $lambda (&x) x;
		$def! list $lambda (.x) x;
		$def! list% $lambda &x x;
	)NPL");
#	endif
	// TODO: Support move-only types at end?
	context.Perform(u8R"NPL(
		$def! $sequence
			($lambda (&cenv)
				($lambda #ignore $vau/e% cenv &body d
					$if (null? body) inert (eval% (cons% $aux body) d))
				($set! cenv $aux
					$vau/e% (weaken-environment cenv) (&head .&tail) d
						$if (null? tail) (eval% head d)
							(($vau% (&t) e ($lambda% #ignore (eval% t e))
								(eval% head d)) (eval% (cons% $aux tail) d))))
			(make-environment (() get-current-environment));
	)NPL");
#endif
}

void
LoadCore(REPLContext& context)
{
	auto& renv(context.Root.GetRecordRef());

	context.Perform(u8R"NPL(
		$def! $quote $vau (&x) #ignore x;
		$def! $set! $vau (&e &formals .&expr) d
			eval (list $def! formals (unwrap eval) expr d) (eval e d);
		$def! $defv! $vau (&$f &formals &senv .&body) d
			eval (list $set! d $f $vau formals senv body) d;
		$defv! $defv%! (&$f &formals &senv .&body) d
			eval (list $set! d $f $vau% formals senv body) d;
		$defv! $defv/e! (&$f &e &formals &senv .&body) d
			eval (list $set! d $f $vau/e e formals senv body) d;
		$defv! $defv/e%! (&$f &e &formals &senv .&body) d
			eval (list $set! d $f $vau/e% e formals senv body) d;
		$defv! $setrec! (&e &formals .&expr) d eval
			(list $defrec! formals (unwrap eval) expr d) (eval e d);
		$defv! $defl! (&f &formals .&body) d eval
			(list $set! d f $lambda formals body) d;
		$defv! $defl%! (&f &formals .&body) d eval
			(list $set! d f $lambda% formals body) d;
		$defl%! forward (&x)
			$if (lvalue? (resolve-identifier ($quote x))) x (idv x);
		$defl! first ((&x .)) x;
		$defl%! first% ((%x .)) forward x;
		$defl%! first& ((&x .)) x;
		$defl! rest ((#ignore .x)) x;
		$defl! rest% ((#ignore .%x)) x;
		$defl! rest& ((#ignore .&x)) x;
		$defl%! apply (&appv &arg .&opt)
			eval% (cons% () (cons% (unwrap appv) arg))
				($if (null? opt) (() make-environment) (first& opt));
		$defl! list* (&head .&tail)
			$if (null? tail) head (cons head (apply list* tail));
		$defl%! list*% (%head .%tail) $if (null? tail) (forward head)
			(idv (cons% (forward head) (apply list*% tail)));
		$defv! $defw! (&f &formals &senv .&body) d eval
			(list $set! d f wrap (list* $vau formals senv body)) d;
		$defv! $defw%! (&f &formals &senv .&body) d eval
			(list $set! d f wrap (list* $vau% formals senv body)) d;
		$defv! $defw/e! (&f &e &formals &senv .&body) d eval
			(list $set! d f wrap (list* $vau/e e formals senv body)) d;
		$defv! $defw/e%! (&f &e &formals &senv .&body) d eval
			(list $set! d f wrap (list* $vau/e% e formals senv body)) d;
		$defv! $lambda/e (&e &formals .&body) d
			wrap (eval (list* $vau/e e formals ignore body) d);
		$defv! $lambda/e% (&e &formals .&body) d
			wrap (eval (list* $vau/e% e formals ignore body) d);
		$defv! $defl/e! (&f &e &formals .&body) d eval
			(list $set! d f $lambda/e e formals body) d;
		$defv! $defl/e%! (&f &e &formals .&body) d eval
			(list $set! d f $lambda/e% e formals body) d;
	)NPL");
	context.Perform(u8R"NPL(
		$defv%! $cond &clauses d
			$if (null? clauses) inert (apply ($lambda% ((&test .&body) .clauses)
				$if (eval test d) (eval% body d)
					(apply (wrap $cond) clauses d)) clauses);
	)NPL");
#if YF_Impl_NPLA1_Use_LockEnvironment
	context.Perform(u8R"NPL(
		$def! make-standard-environment
			$lambda () () lock-current-environment;
	)NPL");
#else
	context.Perform(u8R"NPL(
		$def! make-standard-environment
			($lambda (&cenv &env)
				($lambda #ignore $vau/e cenv () #ignore (make-environment senv))
				($set! cenv senv env))
			(make-environment (() get-current-environment))
			(() get-current-environment);
	)NPL");
#endif
	// NOTE: Use of 'eql?' is more efficient than '$if'.
	context.Perform(u8R"NPL(
		$defl! not? (&x) eql? x #f;
		$defv%! $when (&test .&vexpr) d
			$if (eval test d) (eval% (list*% $sequence vexpr) d);
		$defv%! $unless (&test .&vexpr) d
			$if (not? (eval test d)) (eval% (list*% $sequence vexpr) d);
	)NPL");
	RegisterForm(renv, "$and?", And);
	RegisterForm(renv, "$or?", Or);
	context.Perform(u8R"NPL(
		$defl! first-null? (&l) null? (first% l);
		$defl! list-rest% (&x) list% (rest% x);
		$defl%! accl (&l &pred? &base &head &tail &sum) $sequence
			($defl%! aux (&l &base) $if (pred? l) (forward base)
				(aux (tail l) (sum (head l) (forward base))))
			(forward (aux l base));
		$defl%! accr (&l &pred? &base &head &tail &sum) $sequence
			($defl%! aux (&l) $if (pred? l) (forward base)
				(sum (head l) (aux (tail l))))
			(forward (aux l));
		$defl%! foldr1 (&kons &knil &l)
			forward (accr l null? (forward knil) first% rest% kons);
		$defw%! map1 (&appv &l) d forward (foldr1
			($lambda (&x &xs) cons% (apply appv (list% x) d) xs) () l);
		$defl! list-concat (&x &y) foldr1 cons% y x;
		$defl! append (.&ls) foldr1 list-concat () ls;
		$defv%! $let (&bindings .&body) d forward (eval% (list*% ()
			(list*% $lambda% (map1 first% bindings) forward (list body))
			(map1 list-rest% bindings)) d);
		$defv%! $let/d (&bindings &ef .&body) d forward (eval% (list*% ()
			(list% wrap
				(list*% $vau% (map1 first% bindings) ef forward (list body)))
			(map1 list-rest% bindings)) d);
		$defv%! $let/e (&e &bindings .&body) d forward (eval% (list*% ()
			(list*% $lambda/e% e (map1 first% bindings) forward (list body))
			(map1 list-rest% bindings)) d);
		$defv%! $let* (&bindings .&body) d forward
			(eval% ($if (null? bindings) (list*% $let bindings (idv body))
				(list% $let (list (first% bindings))
				(list*% $let* (rest% bindings) body))) d);
		$defv%! $letrec (&bindings .&body) d forward
			(eval% (list $let () $sequence (list% $def! (map1 first% bindings)
				(list*% () list (map1 rest% bindings))) body) d);
		$defv! $bindings/p->environment (&parents .&bindings) denv $sequence
			($def! res apply make-environment (map1 ($lambda% (x) eval% x denv)
				parents))
			(eval% (list% $set! res (map1 first% bindings)
				(list*% () list (map1 rest% bindings))) denv)
			res;
		$defv! $bindings->environment (.&bindings) denv
			eval (list*% $bindings/p->environment
				(list (() make-standard-environment)) bindings) denv;
		$defv! $provide! (&symbols .&body) d $sequence
			(eval (list% $def! symbols (list $let () $sequence (list% $set!
				(() get-current-environment) ($quote res) (list% ()
				lock-current-environment)) body (list* () list symbols))) d)
			res;
		$defv! $provide/d! (&symbols &ef .&body) d $sequence
			(eval (list% $def! symbols (list $let/d () ef $sequence (list% $set!
				(() get-current-environment) ($quote res) (list% ()
				lock-current-environment)) body (list* () list symbols))) d)
			res;
		$defv! $import! (&e .&symbols) d
			eval% (list $set! d symbols (list* () list symbols)) (eval e d);
		$defl! unfoldable? (&l)
			forward (accr l null? (first-null? l) first-null? rest% $or?);
		$def! map-reverse $let ((&cenv () make-standard-environment)) wrap
			($sequence
				($set! cenv cxrs $lambda/e (weaken-environment cenv) (&ls &cxr)
					accl ls null? () ($lambda (&l) cxr (first l)) rest cons)
				($vau/e cenv (&appv .&ls) d accl ls unfoldable? ()
					($lambda (&ls) cxrs ls first) ($lambda (&ls) cxrs ls rest)
						($lambda (&x &xs) cons (apply appv x d) xs)));
		$defw! for-each-ltr &ls env $sequence (apply map-reverse ls env) inert;
	)NPL");
}

void
LoadObjectInteroperations(REPLContext& context)
{
	RegisterStrict(context.Root, "ref", [](TermNode& term){
		CallUnary([&](TermNode& tm){
			LiftToReference(term, tm);
		}, term);
		return CheckNorm(term);
	});
}

void
LoadEncapsulations(REPLContext& context)
{
	RegisterStrict(context.Root, "make-encapsulation-type",
		MakeEncapsulationType);
}

void
Load(REPLContext& context)
{
	LoadPrimitive(context);
	LoadCore(context);
	LoadObjectInteroperations(context);
	LoadEncapsulations(context);
}
//@}

} // namespace Derived;

//! \since build 839
void
Load(REPLContext& context)
{
	auto& rctx(context.Root);

	LoadObjects(rctx.GetRecordRef());
	Primitive::Load(rctx);
	Derived::Load(context);
}

} // namespace Ground;
//@}

} // unnamed namespace;

void
LoadGroundContext(REPLContext& context)
{
	LoadSequenceSeparators(context.ListTermPreprocess),
	LoadBasicProcessing(context.Root);
	Ground::Load(context);
}

void
LoadModule_std_environments(REPLContext& context)
{
	auto& renv(context.Root.GetRecordRef());

	RegisterStrictUnary(renv, "bound?",
		[](TermNode& term, const ContextNode& ctx){
		return ystdex::call_value_or([&](string_view id){
			return CheckSymbol(id, [&]{
				return bool(ResolveName(ctx, id).first);
			});
		}, AccessTermPtr<string>(term));
	});
	context.Perform(u8R"NPL(
		$defv/e! $binds1? (make-environment
			(() get-current-environment) std.strings) (&e &s) d
			eval (list (unwrap bound?) (symbol->string s)) (eval e d);
	)NPL");
	RegisterStrict(renv, "value-of", ValueOf);
	RegisterStrict(renv, "get-current-repl",
		ystdex::bind1([](TermNode& term, REPLContext& rctx){
		RetainN(term, 0);
		term.Value = ValueObject(rctx, OwnershipTag<>());
	}, std::ref(static_cast<REPLContext&>(context))));
	RegisterStrict(renv, "eval-string", EvalString);
	RegisterStrict(renv, "eval-string%", EvalStringRef);
	RegisterStrict(renv, "eval-unit", EvalUnit);
}

void
LoadModule_std_strings(REPLContext& context)
{
	auto& renv(context.Root.GetRecordRef());

	RegisterStrict(renv, "++",
		std::bind(CallBinaryFold<string, ystdex::plus<>>, ystdex::plus<>(),
		string(), std::placeholders::_1));
	RegisterStrictUnary<const string>(renv, "string-empty?",
		std::mem_fn(&string::empty));
	RegisterStrictBinary(renv, "string<-", [](TermNode& x, const TermNode& y){
		AccessTerm<string>(x) = AccessTerm<const string>(y);
		return ValueToken::Unspecified;
	});
	RegisterStrictBinary<string, string>(renv, "string-contains-ci?",
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
	RegisterStrictUnary<const string>(renv, "string->symbol", StringToSymbol);
	RegisterStrictUnary<const TokenValue>(renv, "symbol->string",
		SymbolToString);
	RegisterStrictUnary<const string>(renv, "string->regex",
		[](const string& str){
		return std::regex(str);
	});
	RegisterStrict(renv, "regex-match?", [](TermNode& term){
		auto i(std::next(term.begin()));
		const auto& str(AccessTerm<const string>(Deref(i)));
		const auto& r(AccessTerm<const std::regex>(Deref(++i)));

		term.ClearTo(std::regex_match(str, r));
	}, ystdex::bind1(RetainN, 2));
}

void
LoadModule_std_io(REPLContext& context)
{
	auto& renv(context.Root.GetRecordRef());

	RegisterStrictUnary<const string>(renv, "puts", [](const string& str){
		// FIXME: Use %EncodeArg?
		// XXX: Error is ignored.
		std::puts(str.c_str());
		return ValueToken::Unspecified;
	});
	RegisterStrictUnary<const string>(renv, "load", [&](const string& filename){
		auto p_is(OpenFile(filename.c_str()));

		TryLoadSource(context, filename.c_str(), *p_is);
		return ValueToken::Unspecified;
	});
}

void
LoadModule_std_system(REPLContext& context)
{
	auto& renv(context.Root.GetRecordRef());

	RegisterStrict(renv, "cmd-get-args", [](TermNode& term){
		RetainN(term, 0);
		term.GetContainerRef() = []{
			const auto p_cmd_args(LockCommandArguments());
			TermNode::Container con{};

			for(const auto& arg : p_cmd_args->Arguments)
				TermNode::AddValueTo(con, MakeIndex(con),
					ystdex::in_place_type<string>, arg);
			return con;
		}();
		return ReductionStatus::Retained;
	});
	RegisterStrictUnary<const string>(renv, "env-get", [](const string& var){
		string res;

		FetchEnvironmentVariable(res, var.c_str());
		return res;
	});
	RegisterStrictBinary<const string, const string>(renv, "env-set",
		[&](const string& var, const string& val){
		SetEnvironmentVariable(var.c_str(), val.c_str());
	});
	context.Perform(u8R"NPL(
		$defl/e! env-empty?
			(make-environment (() get-current-environment) std.strings) (&n)
			string-empty? (env-get n);
	)NPL");
	RegisterStrict(renv, "system", CallSystem);
	RegisterStrict(renv, "system-get", [](TermNode& term){
		CallUnaryAs<const string>([&](const string& cmd){
			TermNode::Container con;
			auto res(FetchCommandOutput(cmd.c_str()));

			TermNode::AddValueTo(con, MakeIndex(0),
				ystdex::trim(std::move(res.first)));
			TermNode::AddValueTo(con, MakeIndex(1), res.second);
			ystdex::swap_dependent(con, term.GetContainerRef());
		}, term);
		return ReductionStatus::Retained;
	});
	RegisterStrictUnary<const string>(renv, "system-quote",
		[](const string& w){
		return !w.empty() ? ((CheckLiteral(w) == char() && (w.find(' ')
			!= string::npos || w.find('\t') != string::npos))
			|| (w.front() == '\'' || w.front() == '"' || w.back() == '\''
			|| w.back() == '"') ? ystdex::quote(w, '"') : w) : "\"\"";
	});
}

void
LoadModule_SHBuild(REPLContext& context)
{
	auto& renv(context.Root.GetRecordRef());

	// NOTE: SHBuild builtins.
	renv.Define("SHBuild_BaseTerminalHook_",
		ValueObject(std::function<void(const string&, const string&)>(
		[](const string& n, const string& val) ynothrow{
			// XXX: Error from 'std::printf' is ignored.
			std::printf("%s = \"%s\"\n", n.c_str(), val.c_str());
	})), {});
	RegisterStrictUnary<const string>(renv, "SHBuild_BuildGCH_existed_",
		[](const string& str) -> bool{
		if(IO::UniqueFile
			file{uopen(str.c_str(), IO::omode_convb(std::ios_base::in))})
			return file->GetSize() > 0;
		return {};
	});
	RegisterStrictUnary<const string>(renv, "SHBuild_EnsureDirectory_",
		[](const string& str){
		EnsureDirectory(IO::Path(str));
	});
	RegisterStrictUnary<const string>(renv, "SHBuild_BuildGCH_mkpdirp_",
		[](const string& str){
		IO::Path pth(str);

		if(!pth.empty())
		{
			pth.pop_back();
			EnsureDirectory(pth);
		}
	});
	RegisterStrict(renv, "SHBuild_EchoVar", [&](TermNode& term){
		// XXX: To be overriden if %Terminal is usable (platform specific).
		CallBinaryAs<const string, const string>(
			[&](const string& n, const string& val){
			// NOTE: Since root environment can be switched, reference to the
			//	initial instance is necessary to be captured explicitly.
			if(const auto p = GetValuePtrOf(renv.LookupName(
				"SHBuild_BaseTerminalHook_")))
				if(const auto p_hook = AccessPtr<
					std::function<void(const string&, const string&)>>(*p))
					(*p_hook)(n, val);
		}, term);
	});
	RegisterStrict(renv, "SHBuild_Install_HardLink", [&](TermNode& term){
		CallBinaryAs<const string, const string>(
			[](const string& dst, const string& src){
			InstallHardLink(dst.c_str(), src.c_str());
		}, term);
	});
	// TODO: Implement by derivations instead?
	RegisterStrictUnary<const string>(renv, "SHBuild_QuoteS_",
		[](const string& str){
		if(str.find('\'') == string::npos)
			return ystdex::quote(str, '\'');
		throw NPLException("Error in quoted string.");
	});
	RegisterStrictUnary<const string>(renv, "SHBuild_RaiseError_",
		[](const string& str) YB_ATTR(noreturn){
		throw LoggedEvent(str);
	});
	RegisterStrictBinary<const string, const string>(renv,
		"SHBuild_RemovePrefix_",
		[&](const string& str, const string& pfx){
		return ystdex::begins_with(str, pfx) ? str.substr(pfx.length()) : str;
	});
	RegisterStrictUnary<const string>(renv, "SHBuild_SDot_",
		[](const string& str){
		auto res(str);

		for(auto& c : res)
			if(c == '.')
				c = '_';
		return res;
	});
	RegisterStrictUnary<const string>(renv, "SHBuild_String_absolute_path?_",
		[](const string& str){
		return IO::IsAbsolute(str);
	});
	RegisterStrictUnary<const string>(renv, "SHBuild_TrimOptions_",
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

