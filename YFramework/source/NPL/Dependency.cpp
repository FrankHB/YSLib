/*
	© 2015-2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Dependency.cpp
\ingroup NPL
\brief 依赖管理。
\version r2877
\author FrankHB <frankhb1989@gmail.com>
\since build 623
\par 创建时间:
	2015-08-09 22:14:45 +0800
\par 修改时间:
	2019-09-17 05:26 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::Dependency
*/


#include "NPL/YModules.h"
#include YFM_NPL_Dependency // for ystdex::isspace, std::istream,
//	YSLib::unique_ptr, NPL::AllocateEnvironment, std::piecewise_construct,
//	std::forward_as_tuple, NPL::Deref, ystdex::isdigit, Collapse,
//	ystdex::bind1, NPL::IsMovable, NPL::TryAccessReferencedTerm, ystdex::plus,
//	std::placeholders, NPL::ResolveRegular, ystdex::tolower,
//	ystdex::swap_dependent, NPL::Forms functions;
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
#define NPL_Impl_NPLA1_Native_Forms true
// NOTE: For environment primitive native implemantations.
#define NPL_Impl_NPLA1_Native_EnvironmentPrimitives true
// NOTE: For '$vau' in 'id' derivations instead of '$lambda'.
#define NPL_Impl_NPLA1_Use_Id_Vau true
// NOTE: For awareness of strong ownership of environments.
#define NPL_Impl_NPLA1_Use_LockEnvironment true
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
			[&](SmallString& buf, const UnescapeContext& uctx, char) -> bool{
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
		}, [](char ch, SmallString& pfx) -> bool{
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
		[&](SmallString::const_iterator b, SmallString::const_iterator e){
		lst.push_back(string(b, e));
	}, [&](SmallString::const_iterator i){
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
		Remove(dst);
	TryExpr(CreateHardLink(dst, src))
	CatchExpr(..., InstallFile(dst, src))
}

void
InstallSymbolicLink(const char* dst, const char* src)
{
	using namespace YSLib::IO;

	Remove(dst);
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
	// XXX: Allocators are inefficient for short strings here.
	RegisterSequenceContextTransformer(passes, TokenValue(";"), true),
	RegisterSequenceContextTransformer(passes, TokenValue(","));
}

//! \since build 794
//@{
void
CopyEnvironmentDFS(Environment& d, const Environment& e)
{
	auto& m(d.GetMapRef());
	// TODO: Check environments allocator equality.
	const auto a(m.get_allocator());
	// TODO: Support more implementations?
	const auto copy_parent([&](Environment& dst, const Environment& parent){
		auto p_env(NPL::AllocateEnvironment(a));

		CopyEnvironmentDFS(*p_env, parent);
		dst.Parent = std::move(p_env);
	});
	const auto copy_parent_ptr(
		[&](function<Environment&()> mdst, const ValueObject& vo) -> bool{
		if(const auto p = NPL::AccessPtr<EnvironmentReference>(vo))
		{
			if(const auto p_parent = p->Lock())
				copy_parent(mdst(), *p_parent);
			// XXX: Failure of locking is ignored.
			return true;
		}
		else if(const auto p_e = NPL::AccessPtr<shared_ptr<Environment>>(vo))
		{
			if(const auto p_parent = *p_e)
				copy_parent(mdst(), *p_parent);
			// XXX: Empty parent is ignored.
			return true;
		}
		return {};
	});

	copy_parent_ptr([&]() ynothrow -> Environment&{
		return d;
	}, e.Parent);
	for(const auto& b : e.GetMapRef())
		m.emplace(std::piecewise_construct, std::forward_as_tuple(b.first),
			std::forward_as_tuple(b.second.CreateWith(
			[&](const ValueObject& vo) -> ValueObject{
			shared_ptr<Environment> p_env;

			if(copy_parent_ptr([&]() ynothrow -> Environment&{
				p_env = NPL::AllocateEnvironment(a);
				return *p_env;
			}, vo))
				return ValueObject(std::move(p_env));
			return vo;
		}), b.second.Value));
}

void
CopyEnvironment(TermNode& term, ContextNode& ctx)
{
	auto p_env(AllocateEnvironment(term, ctx));

	CopyEnvironmentDFS(*p_env, ctx.GetRecordRef());
	term.Value = ValueObject(std::move(p_env));
}
//@}

//! \since build 854
template<typename _func>
YB_ATTR_nodiscard ReductionStatus
DoIdFunc(_func f, TermNode& term)
{
	RetainN(term);
	LiftTerm(term, NPL::Deref(std::next(term.begin())));
	f(term);
	return ReductionStatus::Retained;
}

//! \since build 859
void
CheckForAssignment(TermNode& nd, ResolvedTermReferencePtr p_ref)
{
	if(p_ref)
	{
		if(p_ref->IsModifiable())
			return;
		ThrowNonmodifiableErrorForAssignee();
	}
	else
		ThrowValueCategoryErrorForFirstArgument(nd);
}

//! \since build 856
template<typename _func>
YB_ATTR_nodiscard ValueToken
DoAssign(_func f, TermNode& x)
{
	ResolveTerm([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
		CheckForAssignment(nd, p_ref);
		f(nd);
	}, x);
	return ValueToken::Unspecified;
}

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
				// NOTE: This is named after '#inert' in Kernel, but essentially
				//	unspecified in NPLA.
				else if(id == "#inert")
					term.Value = ValueToken::Unspecified;
				// XXX: Redundant test?
				else if(IsNPLAExtendedLiteral(id))
					ThrowInvalidSyntaxError(ystdex::sfmt(f != '#'
						? "Unsupported literal prefix found in literal '%s'."
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
				// TODO: Use more specific exception type like
				//	%std::range_error?
				else if(ystdex::isdigit(id.back()))
					ThrowInvalidSyntaxError(ystdex::sfmt("Value of identifier"
						" '%s' is out of the range of the supported integer.",
						id.data()));
				else
					// TODO: Supported literal postfix?
					ThrowInvalidSyntaxError(ystdex::sfmt("Literal postfix is"
						" unsupported in identifier '%s'.", id.data()));
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
	// NOTE: This is like '#ignore' in Kernel, but with the symbol type. An
	//	alternative definition is by evaluating '$def! ignore $quote #ignore'
	//	(see '$def!' and '$quote').
	env.Define("ignore", TokenValue("#ignore"));
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

//! \since build 855
void
LoadObjects(ContextNode& ctx)
{
	RegisterStrictUnary(ctx, "null?", ComposeReferencedTermOp(IsEmpty));
	RegisterStrictUnary(ctx, "nullv?", IsEmpty);
	RegisterStrictUnary(ctx, "reference?", IsReferenceTerm);
	RegisterStrictUnary(ctx, "lvalue?", IsLValueTerm);
	RegisterStrictUnary(ctx, "unique?", IsUniqueTerm);
	RegisterStrict(ctx, "move", [](TermNode& term){
		RetainN(term);
		ResolveTerm([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
			// NOTE: Force move. No %IsMovable check is needed.
			LiftTermOrCopy(term, nd, !p_ref || p_ref->IsModifiable());
			// NOTE: Term tag is not copied.
		}, NPL::Deref(std::next(term.begin())));
		return ReduceForLiftedResult(term);
	});
	RegisterStrict(ctx, "deshare", [](TermNode& term){
		return DoIdFunc([](TermNode& t){
			if(const auto p = NPL::TryAccessLeaf<const TermReference>(t))
				LiftTermRef(t, p->get());
			LiftTermIndirection(t);
		}, term);
	});
	RegisterStrict(ctx, "ref&", [](TermNode& term){
		CallUnary([&](TermNode& tm){
			LiftToReference(term, tm);
		}, term);
		return ReductionStatus::Retained;
	});
	RegisterStrictBinary(ctx, "assign%!", [](TermNode& x, TermNode& y){
		return DoAssign([&](TermNode& nd){
			if(const auto p = NPL::TryAccessLeaf<TermReference>(y))
				LiftCollapsed(nd, y, *p);
			else
				LiftTerm(nd, y);
		}, x);
	});
	RegisterStrictBinary(ctx, "assign@!", [](TermNode& x, TermNode& y){
		return DoAssign(ystdex::bind1(static_cast<void(&)(TermNode&,
			TermNode&)>(LiftTerm), std::ref(y)), x);
	});
}

void
LoadLists(ContextNode& ctx)
{
	// NOTE: Though NPLA does not use cons pairs, corresponding primitives are
	//	still necessary.
	// NOTE: Since NPL has no con pairs, it only added head to existed list.
	RegisterStrict(ctx, "cons", Cons);
	RegisterStrict(ctx, "cons%", ConsRef);
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
			return ResolveIdentifier(c, id);
		});
	});
	// NOTE: This is now be primitive since in NPL environment capture is more
	//	basic than vau.
	RegisterStrict(ctx, "copy-environment", CopyEnvironment);
	RegisterStrictUnary<const EnvironmentReference>(ctx, "lock-environment",
		[](const EnvironmentReference& wenv) ynothrow{
			return wenv.Lock();
		});
	RegisterStrict(ctx, "make-environment", MakeEnvironment);
	RegisterStrictUnary<const shared_ptr<Environment>>(ctx,
		"weaken-environment", [](const shared_ptr<Environment>& p) ynothrow{
		return EnvironmentReference(p);
	});
	// NOTE: Environment mutation is optional in Kernel and supported here.
	// NOTE: For zero overhead principle, the form without recursion (named
	//	'$def!') is preferred. The recursion variant (named '$defrec!') is more
	//	than '$define!' in Kernel because of more native interoperations
	//	(shared object holders) supported, and is used only when necessary.
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
	RegisterStrict(ctx, "wrap", Wrap);
	RegisterStrict(ctx, "wrap%", WrapRef);
	RegisterStrict(ctx, "unwrap", Unwrap);
}

//! \since build 859
void
LoadErrorsAndChecks(ContextNode& ctx)
{
	RegisterStrictUnary<const string>(ctx, "raise-invalid-syntax-error",
		[](const string& str) YB_ATTR_LAMBDA(noreturn){
		ThrowInvalidSyntaxError(str);
	});
	RegisterStrict(ctx, "check-list-reference", CheckListReference);
}

//! \since build 855
void
LoadEncapsulations(ContextNode& ctx)
{
	RegisterStrict(ctx, "make-encapsulation-type", MakeEncapsulationType);
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
	LoadObjects(ctx);
	LoadLists(ctx);
	LoadEnvironments(ctx);
	LoadCombiners(ctx);
	LoadErrorsAndChecks(ctx);
	LoadEncapsulations(ctx);
}

} // namespace Primitive;

namespace Derived
{

//! \since build 855
void
LoadGroundedDerived(REPLContext& context)
{
	auto& renv(context.Root.GetRecordRef());

	// NOTE: Some combiners are provided here as host primitives for
	//	more efficiency and less dependencies.
#if NPL_Impl_NPLA1_Native_Forms || NPL_Impl_NPLA1_Native_EnvironmentPrimitives
	RegisterStrict(renv, "get-current-environment", GetCurrentEnvironment);
	RegisterStrict(renv, "lock-current-environment", LockCurrentEnvironment);
#endif
#if NPL_Impl_NPLA1_Native_Forms || !NPL_Impl_NPLA1_Native_EnvironmentPrimitives
	RegisterForm(renv, "$vau", Vau);
	RegisterForm(renv, "$vau%", VauRef);
#endif
#if NPL_Impl_NPLA1_Native_Forms

	const auto idv([](TermNode& term){
		return DoIdFunc(LiftToReturn, term);
	});

	RegisterForm(renv, "$quote", idv);
	RegisterStrict(renv, "id", [](TermNode& term){
		return DoIdFunc([](TermNode&) ynothrow{}, term);
	});
	RegisterStrict(renv, "idv", idv);
	RegisterStrict(renv, "list", ReduceBranchToListValue);
	RegisterStrict(renv, "list%", ReduceBranchToList);
	// NOTE: Lazy form '$deflazy!' is the basic operation, which may bind
	//	parameter as unevaluated operands. 
	RegisterForm(renv, "$deflazy!", DefineLazy);
	RegisterForm(renv, "$lambda", Lambda);
	RegisterForm(renv, "$lambda%", LambdaRef);
	// NOTE: The sequence operator is also available as infix ';' syntax sugar.
	RegisterForm(renv, "$sequence", Sequence);
	RegisterStrict(renv, "forward", [](TermNode& term){
		return DoIdFunc(LiftRValueToReturn, term);
	});
	RegisterStrictBinary(renv, "assign!", [](TermNode& x, TermNode& y){
		return DoAssign([&](TermNode& nd){
			ResolveTerm([&](TermNode& tm, ResolvedTermReferencePtr p_ref){
				LiftTermOrCopy(nd, tm, NPL::IsMovable(p_ref));
			}, y);
		}, x);
	});
	// NOTE: Like '$set-car!' in Kernel, with no references.
	RegisterStrict(renv, "set-first!", SetFirst);
	// NOTE: Like '$set-car!' in Kernel.
	RegisterStrict(renv, "set-first%!", SetFirstRef);
	// NOTE: Like '$set-car!' in Kernel, with reference not collapsed.
	RegisterStrict(renv, "set-first@!", SetFirstRefAt);
	RegisterStrict(renv, "first", First);
	RegisterStrict(renv, "first&", FirstRef);
	RegisterStrict(renv, "first@", FirstRefAt);
	RegisterStrict(renv, "firstv", FirstVal);
	RegisterStrict(renv, "check-environment", CheckEnvironment);
	RegisterStrict(renv, "apply", Apply);
	RegisterStrict(renv, "list*", ListAsterisk);
	RegisterStrict(renv, "list*%", ListAsteriskRef);
	RegisterForm(renv, "$cond", Cond);
	RegisterForm(renv, "$and?", And);
	RegisterForm(renv, "$or?", Or);
	RegisterStrictUnary(renv, "not?", Not);
#else
#	if NPL_Impl_NPLA1_Native_EnvironmentPrimitives
	context.Perform(R"NPL(
		$def! $vau $vau/e (() get-current-environment) (&formals &ef .&body) d
			eval (cons $vau/e (cons d (cons formals (cons ef body)))) d;
		$def! $vau% $vau (&formals &ef .&body) d
			eval (cons $vau/e% (cons d (cons formals (cons ef body)))) d;
	)NPL");
#	else
	context.Perform(R"NPL(
		$def! get-current-environment (wrap ($vau () d d));
		$def! lock-current-environment (wrap ($vau () d lock-environment d));
	)NPL");
#	endif
	// XXX: The operative '$set!' is same to following derivations.
	context.Perform(R"NPL(
		$def! $quote $vau (&x) #ignore x;
		$def! $set! $vau (&e &formals .&expr) d
			eval (list $def! formals (unwrap eval) expr d) (eval e d);
	)NPL");
#	if NPL_Impl_NPLA1_Use_Id_Vau
	// NOTE: The parameter shall be in list explicitly as '(.x)' to lift
	//	elements by value rather than by reference (as '&x'), otherwise resulted
	//	'list' is wrongly implemented as 'list%' with undefined behavior becuase
	//	it is not guaranteed the operand is alive to access without lifting.
	//	This is not required in Kernel as it does not differentiate lvalues
	//	(first-class referents) from prvalues and all terms can be accessed as
	//	objects with arbitrary longer lifetime.
	context.Perform(R"NPL(
		$def! id wrap ($vau% (%x) #ignore x);
		$def! idv wrap $quote;
		$def! list wrap ($vau (.x) #ignore x);
		$def! list% wrap ($vau &x #ignore x);
	)NPL");
	// XXX: The operative '$defv!' is same to following derivations in
	//	%LoadCore.
	context.Perform(R"NPL(
		$def! $defv! $vau (&$f &formals &ef .&body) d
			eval (list $set! d $f $vau formals ef body) d;
		$defv! $lambda (&formals .&body) d
			wrap (eval (cons $vau (cons formals (cons ignore body))) d);
		$defv! $lambda% (&formals .&body) d
			wrap (eval (cons $vau% (cons formals (cons ignore body))) d);
	)NPL");
#	else
	RegisterForm(renv, "$lambda", Lambda);
	RegisterForm(renv, "$lambda%", LambdaRef);
	context.Perform(R"NPL(
		$def! id $lambda% (%x) x;
		$def! idv $lambda (&x) x;
		$def! list $lambda (.x) x;
		$def! list% $lambda &x x;
	)NPL");
	// XXX: The operative '$defv!' is same to following derivations in
	//	%LoadCore.
	context.Perform(R"NPL(
		$def! $defv! $vau (&$f &formals &ef .&body) d
			eval (list $set! d $f $vau formals ef body) d;
	)NPL");
#	endif
	context.Perform(R"NPL(
		$def! $deflazy! $vau (&definiend .&expr) d
			eval (list $def! definiend $quote expr) d;
		$def! $sequence
			($lambda (&se)
				($lambda #ignore $vau/e% se &body d
					$if (null? body) #inert (eval% (cons% $aux body) d))
				($set! se $aux
					$vau/e% (weaken-environment se) (&head .&tail) d
						$if (null? tail) (eval% head d)
							(($vau% (&t) e ($lambda% #ignore (eval% t e))
								(eval% head d)) (eval% (cons% $aux tail) d))))
			(make-environment (() get-current-environment));
	)NPL");
	// XXX: The operatives '$defl!', '$defl%!', and '$defv%!', as well as the
	//	applicative 'first@' are same to following derivations in %LoadCore.
	context.Perform(R"NPL(
		$defv! $defl! (&f &formals .&body) d
			eval (list $set! d f $lambda formals body) d;
		$defv! $defl%! (&f &formals .&body) d
			eval (list $set! d f $lambda% formals body) d;
		$defl%! forward (%x)
			$if (lvalue? ((unwrap resolve-identifier) x)) x (idv x);
		$defl! assign! (&x &y) assign%! (forward x) (idv y);
		$defl! set-first! (&l x)
			assign@! (first@ (check-list-reference l)) (move x);
		$defl! set-first%! (&l &x)
			assign%! (first@ (check-list-reference l)) (forward x);
		$defl! set-first@! (&l &x)
			assign@! (first@ (check-list-reference l)) (forward x);
		$defl%! first (&l)
			($lambda% ((&x .))
				($if (lvalue? ((unwrap resolve-identifier) l)) id forward) x) l;
		$defl%! first& (&l) ($lambda% ((&x .)) x) (check-list-reference l);
		$defl%! first@ (&l) ($lambda% ((@x .)) x) (check-list-reference l);
		$defl! firstv ((&x .)) x;
		$defl%! check-environment (&e)
			$sequence ($vau/e% e . #ignore) (forward e);
		$defl%! apply (&appv &arg .&opt)
			eval% (cons% () (cons% (unwrap (forward appv)) (forward arg)))
				($if (null? opt) (() make-environment)
					(($lambda ((&e .&eopt))
						$if (null? eopt) e
							(raise-invalid-syntax-error
								"Syntax error in applying form.")) opt));
		$defl! list* (&head .&tail)
			$if (null? tail) head (cons head (apply list* tail));
		$defl%! list*% (&head .&tail) $if (null? tail) (forward head)
			(cons% (forward head) (apply list*% tail));
		$defv! $defv%! (&$f &formals &ef .&body) d
			eval (list $set! d $f $vau% formals ef body) d;
		$defv%! $cond &clauses d $if (null? clauses) #inert
			(apply ($lambda% ((&test .&body) .clauses)
				$if (eval test d) (eval% body d) (apply (wrap $cond) clauses d))
				clauses);
		$defl! not? (&x) eql? x #f;
		$defv%! $and? x d $cond
			((null? x) #t)
			((nullv? (rest& x)) eval% (first (forward x)) d)
			((eval% (first& x) d) apply (wrap $and?) (rest% (forward x)) d)
			(#t #f);
		$defv%! $or? x d $cond
			((null? x) #f)
			((nullv? (rest& x)) eval% (first (forward x)) d)
			((eval% (first& x) d) eval% (first (forward x)) d)
			(#t apply (wrap $or?) (rest% (forward x)) d);
	)NPL");
#endif
}

//! \since build 839
//@{
void
LoadCore(REPLContext& context)
{
	context.Perform(R"NPL(
		$def! $set! $vau (&e &formals .&expr) d
			eval (list $def! formals (unwrap eval) expr d) (eval e d);
		$def! $defv! $vau (&$f &formals &ef .&body) d
			eval (list $set! d $f $vau formals ef body) d;
		$defv! $defv%! (&$f &formals &ef .&body) d
			eval (list $set! d $f $vau% formals ef body) d;
		$defv! $defv/e! (&$f &e &formals &ef .&body) d
			eval (list $set! d $f $vau/e e formals ef body) d;
		$defv! $defv/e%! (&$f &e &formals &ef .&body) d
			eval (list $set! d $f $vau/e% e formals ef body) d;
		$defv! $defl! (&f &formals .&body) d
			eval (list $set! d f $lambda formals body) d;
		$defv! $defl%! (&f &formals .&body) d
			eval (list $set! d f $lambda% formals body) d;
		$defv! $setrec! (&e &formals .&expr) d
			eval (list $defrec! formals (unwrap eval) expr d) (eval e d);
		$defl! rest ((#ignore .x)) x;
		$defl! rest& (&l) ($lambda ((#ignore .&x)) x) (check-list-reference l);
		$defl! rest% ((#ignore .%x)) x;
		$defv! $defw! (&f &formals &ef .&body) d
			eval (list $set! d f wrap (list* $vau formals ef body)) d;
		$defv! $defw%! (&f &formals &ef .&body) d
			eval (list $set! d f wrap (list* $vau% formals ef body)) d;
		$defv! $defw/e! (&f &e &formals &ef .&body) d
			eval (list $set! d f wrap (list* $vau/e e formals ef body)) d;
		$defv! $defw/e%! (&f &e &formals &ef .&body) d
			eval (list $set! d f wrap (list* $vau/e% e formals ef body)) d;
		$defv! $lambda/e (&e &formals .&body) d
			wrap (eval (list* $vau/e e formals ignore body) d);
		$defv! $lambda/e% (&e &formals .&body) d
			wrap (eval (list* $vau/e% e formals ignore body) d);
		$defv! $defl/e! (&f &e &formals .&body) d
			eval (list $set! d f $lambda/e e formals body) d;
		$defv! $defl/e%! (&f &e &formals .&body) d
			eval (list $set! d f $lambda/e% e formals body) d;
	)NPL");
#if NPL_Impl_NPLA1_Use_LockEnvironment
	context.Perform(R"NPL(
		$def! make-standard-environment
			$lambda () () lock-current-environment;
	)NPL");
#else
	context.Perform(R"NPL(
		$def! make-standard-environment
			($lambda (&se &e)
				($lambda #ignore $vau/e se () #ignore (make-environment ce))
				($set! se ce e))
			(make-environment (() get-current-environment))
			(() get-current-environment);
	)NPL");
#endif
	// NOTE: Use of 'eql?' is more efficient than '$if'.
	context.Perform(R"NPL(
		$defv%! $when (&test .&vexpr) d
			$if (eval test d) (eval% (list*% () $sequence vexpr) d);
		$defv%! $unless (&test .&vexpr) d
			$if (not? (eval test d)) (eval% (list*% () $sequence vexpr) d);
		$def! (box% box? unbox) () make-encapsulation-type;
		$defl! box (&x) box% x;
		$defl! first-null? (&l) null? (first l);
		$defl! list-rest% (&x) list% (rest% x);
		$defl%! accl (&l &pred? &base &head &tail &sum) $sequence
			($defl%! aux (&l &base) $if (pred? l) (forward base)
				(aux (tail (forward l)) (sum (head (forward l))
					(forward base))))
			(aux (forward l) (forward base));
		$defl%! accr (&l &pred? &base &head &tail &sum) $sequence
			($defl%! aux (&l) $if (pred? l) (forward base)
				(sum (head (forward l)) (aux (tail (forward l)))))
			(aux (forward l));
		$defl%! foldr1 (&kons &knil &l)
			accr (forward l) null? (forward knil) first rest% kons;
		$defw%! map1 (&appv &l) d
			foldr1 ($lambda (&x &xs) cons% (apply appv (list% x) d) xs) ()
				(forward l);
		$defl! list-concat (&x &y) foldr1 cons% y (forward x);
		$defl! append (.&ls) foldr1 list-concat () (forward ls);
		$defv%! $let (&bindings .&body) d
			eval% (list*% () (list*% $lambda (map1 firstv bindings) (list body))
				(map1 list-rest% bindings)) d;
		$defv%! $let% (&bindings .&body) d
			eval% (list*% () (list*% $lambda% (map1 firstv bindings)
				(list body)) (map1 list-rest% bindings)) d;
		$defv%! $let/d (&bindings &ef .&body) d
			eval% (list*% () (list% wrap (list*% $vau (map1 firstv bindings)
				ef (list body))) (map1 list-rest% bindings)) d;
		$defv%! $let/d% (&bindings &ef .&body) d
			eval% (list*% () (list% wrap (list*% $vau% (map1 firstv bindings)
				ef (list body))) (map1 list-rest% bindings)) d;
		$defv%! $let/e (&e &bindings .&body) d
			eval% (list*% () (list*% $lambda/e e (map1 firstv bindings)
				(list body)) (map1 list-rest% bindings)) d;
		$defv%! $let/e% (&e &bindings .&body) d
			eval% (list*% () (list*% $lambda/e% e (map1 firstv bindings)
				(list body)) (map1 list-rest% bindings)) d;
		$defv%! $let* (&bindings .&body) d
			eval% ($if (null? bindings) (list*% $let bindings (idv body))
				(list% $let (list (first bindings))
				(list*% $let* (rest% bindings) body))) d;
		$defv%! $let*% (&bindings .&body) d
			eval% ($if (null? bindings) (list*% $let* bindings (idv body))
				(list% $let% (list (first bindings))
				(list*% $let*% (rest% bindings) body))) d;
		$defv%! $letrec (&bindings .&body) d
			eval% (list $let () $sequence (list% $def! (map1 firstv bindings)
				(list*% () list (map1 rest% bindings))) body) d;
		$defv%! $letrec% (&bindings .&body) d
			eval% (list $let% () $sequence (list% $def! (map1 firstv bindings)
				(list*% () list (map1 rest% bindings))) body) d;
		$defv! $bindings/p->environment (&parents .&bindings) d $sequence
			($def! res apply make-environment (map1 ($lambda% (x) eval% x d)
				parents))
			(eval% (list% $set! res (map1 firstv bindings)
				(list*% () list (map1 rest% bindings))) d)
			res;
		$defv! $bindings->environment (.&bindings) d
			eval (list*% $bindings/p->environment
				(list (() make-standard-environment)) bindings) d;
		$defv! $provide! (&symbols .&body) d $sequence
			(eval (list% $def! symbols (list $let () $sequence
				(list% ($vau% (&e) d $set! e res (lock-environment d))
					(() get-current-environment)) body (list* () list symbols)))
				d) res;
		$defv! $provide/d! (&symbols &ef .&body) d $sequence
			(eval (list% $def! symbols (list $let/d () ef $sequence
				(list% ($vau% (&e) d $set! e res (lock-environment d))
					(() get-current-environment)) body (list* () list symbols)))
				d) res;
		$defv! $import! (&e .&symbols) d
			eval% (list $set! d symbols (list* () list symbols)) (eval e d);
		$defl! unfoldable? (&l)
			accr l null? (first-null? l) first-null? rest% $or?;
		$def! map-reverse $let ((&se () make-standard-environment)) wrap
			($sequence
				($set! se cxrs $lambda/e (weaken-environment se) (&ls &cxr)
					accl (forward ls) null? () ($lambda (&l) cxr (first l))
						rest cons)
				($vau/e se (&appv .&ls) d accl (forward ls) unfoldable? ()
					($lambda (&ls) cxrs ls first) ($lambda (&ls) cxrs ls rest)
						($lambda (&x &xs) cons (apply appv x d) xs)));
		$defw! for-each-ltr &ls d $sequence (apply map-reverse ls d) #inert;
	)NPL");
}

void
Load(REPLContext& context)
{
	LoadGroundedDerived(context);
	LoadCore(context);
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
		}, NPL::TryAccessReferencedTerm<string>(term));
	});
	context.Perform(R"NPL(
		$defv/e! $binds1? (make-environment
			(() get-current-environment) std.strings) (&e &s) d
			eval (list (unwrap bound?) (symbol->string s)) (eval e d);
	)NPL");
	RegisterStrict(renv, "value-of", ValueOf);
	RegisterStrict(renv, "get-current-repl", [&](TermNode& term){
		RetainN(term, 0);
		term.Value = ValueObject(context, OwnershipTag<>());
	});
	RegisterStrict(renv, "eval-string", EvalString);
	RegisterStrict(renv, "eval-string%", EvalStringRef);
	RegisterStrict(renv, "eval-unit", EvalUnit);
}

void
LoadModule_std_promises(REPLContext& context)
{
	// NOTE: Call of 'set-first%!' does not check cyclic references. This is
	//	kept safe since it can occur only with NPLA1 undefined behavior.
	context.Perform(R"NPL(
		$def! std.promises $provide! (promise? memoize $lazy $lazy/e force)
		(
			$def! (encapsulate promise? decapsulate) () make-encapsulation-type,
			$defl%! memoize (&value)
				encapsulate (list (list% (forward value) ())),
			$defv%! $lazy (.&expr) d encapsulate (list (list expr d)),
			$defv%! $lazy/e (&e .&expr) d
				encapsulate (list (list expr (check-environment (eval e d)))),
			$defl%! force (&x)
				$if (promise? x) (force-promise (decapsulate x)) (forward x),
			$defl%! force-promise (&x) $let ((((&object &env)) x))
				$if (null? env) (forward object)
				(
					$let% ((&y eval% object env)) $cond
						((null? (first (rest& (first& x)))) first& (first& x))
						((promise? y) $sequence
							(set-first%! x (first (decapsulate (forward y))))
							(force-promise x))
						(#t $sequence
							($let (((&o &e) first& x))
								list% (assign! o y) (assign@! e ()))
							(forward y))
				)
		);
	)NPL");
}

void
LoadModule_std_strings(REPLContext& context)
{
	auto& renv(context.Root.GetRecordRef());

	RegisterStrict(renv, "++",
		std::bind(CallBinaryFold<string, ystdex::plus<>>, ystdex::plus<>(),
		string(), std::placeholders::_1));
	RegisterStrictUnary<const string>(renv, "string-empty?",
		[](const string& str) ynothrow{
			return str.empty();
		});
	RegisterStrictBinary(renv, "string<-", [](TermNode& x, TermNode& y){
		ResolveTerm([&](TermNode& nd_x, ResolvedTermReferencePtr p_ref_x){
			if(!p_ref_x || p_ref_x->IsModifiable())
			{
				auto& sx(NPL::AccessRegular<string>(nd_x, p_ref_x));

				ResolveTerm([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
					auto& sy(NPL::AccessRegular<string>(nd, p_ref));

					if(NPL::IsMovable(p_ref))
						sx = std::move(sy);
					else
						sx = sy;
				}, y);
			}
			else
				ThrowNonmodifiableErrorForAssignee();
		}, x);
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
	RegisterStrictUnary(renv, "string->symbol", [](TermNode& term){
		return ResolveTerm([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
			auto& s(NPL::AccessRegular<string>(nd, p_ref));

			return NPL::IsMovable(p_ref) ? StringToSymbol(std::move(s))
				: StringToSymbol(s);
		}, term);
	});
	RegisterStrictUnary<const TokenValue>(renv, "symbol->string",
		SymbolToString);
	RegisterStrictUnary<const string>(renv, "string->regex",
		[](const string& str){
		return std::regex(str);
	});
	RegisterStrict(renv, "regex-match?", [](TermNode& term){
		auto i(std::next(term.begin()));
		const auto& str(NPL::ResolveRegular<const string>(NPL::Deref(i)));
		const auto& r(NPL::ResolveRegular<const std::regex>(NPL::Deref(++i)));

		term.Value = std::regex_match(str, r);
		return ReductionStatus::Clean;
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
				TermNode::AddValueTo(con, in_place_type<string>, arg);
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
	context.Perform(R"NPL(
		$defl/e! env-empty?
			(make-environment (() get-current-environment) std.strings) (&n)
			string-empty? (env-get n);
	)NPL");
	RegisterStrict(renv, "system", CallSystem);
	RegisterStrict(renv, "system-get", [](TermNode& term){
		CallUnaryAs<const string>([&](const string& cmd){
			TermNode::Container con;
			auto res(FetchCommandOutput(cmd.c_str()));

			TermNode::AddValueTo(con, ystdex::trim(std::move(res.first)));
			TermNode::AddValueTo(con, res.second);
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
	renv.DefineChecked("SHBuild_BaseTerminalHook_",
		ValueObject(function<void(const string&, const string&)>(
		[](const string& n, const string& val) ynothrow{
			// XXX: Error from 'std::printf' is ignored.
			std::printf("%s = \"%s\"\n", n.c_str(), val.c_str());
	})));
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
					function<void(const string&, const string&)>>(*p))
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
		[](const string& str) YB_ATTR_LAMBDA(noreturn){
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
				using iter_t = SmallString::const_iterator;

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
					res += string(YSLib::make_string_view(str));
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

