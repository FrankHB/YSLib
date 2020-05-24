/*
	© 2015-2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Dependency.cpp
\ingroup NPL
\brief 依赖管理。
\version r3598
\author FrankHB <frankhb1989@gmail.com>
\since build 623
\par 创建时间:
	2015-08-09 22:14:45 +0800
\par 修改时间:
	2020-05-22 14:17 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::Dependency
*/


#include "NPL/YModules.h"
#include YFM_NPL_Dependency // for set, string, ystdex::isspace, std::istream,
//	YSLib::unique_ptr, TokenValue, ystdex::bind1, ValueObject,
//	NPL::AllocateEnvironment, std::piecewise_construct, std::forward_as_tuple,
//	LiftOther, Collapse, LiftOtherOrCopy, NPL::IsMovable, LiftTermOrCopy,
//	ResolveTerm, LiftTermValueOrCopy, NPL::TryAccessReferencedTerm,
//	ystdex::plus, std::placeholders, NPL::ResolveRegular, ystdex::tolower,
//	ystdex::swap_dependent, LiftTermRef, LiftTerm, NPL::Deref;
#include YFM_NPL_NPLA1Forms // for NPL::Forms functions;
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
	Session sess{};
	auto& lexer(sess.Lexer);
	string cbuf{};

	yunused(sess.Process(s_it_t(&sb), s_it_t(), [&](char ch){
		lexer.ParseQuoted(ch, cbuf,
			[&](string& buf, char c, UnescapeContext& uctx, char) -> bool{
			// NOTE: See comments in %munge function of 'mkdeps.c' from libcpp
			//	of GCC.
			if(uctx.Length == 1)
			{
				uctx.VerifyBufferLength(buf.length());

				const auto i(uctx.Start);

				YAssert(i == buf.length() - 1, "Invalid buffer found.");
				if(buf[i] == '\\')
					switch(c)
					{
					case ' ':
						spaces.insert(buf.size());
						YB_ATTR_fallthrough;
					case '\\':
					case '#':
						buf[i] = c;
						uctx.Clear();
						return true;
					case '\n':
						buf.pop_back();
						uctx.Clear();
						return true;
					default:
						uctx.Clear();
						buf += c;
						return {};
					}
				if(buf[i] == '$' && c == '$')
				{
					uctx.Clear();
					return true;
				}
			}
			buf += c;
			return {};
		}, [](string_view buf, UnescapeContext& uctx) -> bool{
			YAssert(!buf.empty(), "Invalid buffer found.");
			if(!uctx.IsHandling())
			{
				if(buf.back() == '\\' || buf.back() == '$')
				{
					yunseq(uctx.Start = buf.length() - 1,
						uctx.Length = 1);
					return true;
				}
			}
			return {};
		});
	}));

	vector<string> lst;

	ystdex::split_if(cbuf.begin(), cbuf.end(), ystdex::isspace,
		[&](string::const_iterator b, string::const_iterator e){
		lst.push_back({b, e});
	}, [&](string::const_iterator i) YB_PURE{
		return !ystdex::exists(spaces, size_t(i - cbuf.cbegin()));
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

//! \since build 872
YB_ATTR_nodiscard ReductionStatus
DoMoveOrTransfer(void(&f)(TermNode&, TermNode&, bool), TermNode& term)
{
	return Forms::CallResolvedUnary(
		[&](TermNode& tm, ResolvedTermReferencePtr p_ref){
		// NOTE: Force move. No %IsMovable check is needed.
		// XXX: Term tags are currently not respected in prvalues.
		f(term, tm, !p_ref || p_ref->IsModifiable());
		// NOTE: Term tags are not copied.
		return ReductionStatus::Retained;
	}, term);
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

//! \since build 869
template<typename _func>
YB_ATTR_nodiscard ReductionStatus
DoIdFunc(_func f, TermNode& term)
{
	return Forms::CallRawUnary([&](TermNode& tm){
		LiftOther(term, tm);
		return f(term);
	}, term);
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
	RegisterUnary<>(ctx, "null?", ComposeReferencedTermOp(IsEmpty));
	RegisterUnary<>(ctx, "nullv?", IsEmpty);
	RegisterUnary<>(ctx, "reference?", IsReferenceTerm);
	RegisterUnary<>(ctx, "bound-lvalue?", IsBoundLValueTerm);
	RegisterUnary<>(ctx, "uncollapsed?", IsUncollapsedTerm);
	RegisterUnary<>(ctx, "unique?", IsUniqueTerm);
	RegisterStrict(ctx, "deshare", [](TermNode& term){
		return Forms::CallRawUnary([&](TermNode& tm){
			if(const auto p = NPL::TryAccessLeaf<const TermReference>(tm))
				LiftTermRef(tm, p->get());
			NPL::SetContentWith(term, std::move(tm),
				&ValueObject::MakeMoveCopy);
			return ReductionStatus::Retained;
		}, term);
	});
	RegisterStrict(ctx, "expire", [](TermNode& term){
		return Forms::CallRawUnary([&](TermNode& tm){
			if(const auto p = NPL::TryAccessLeaf<TermReference>(tm))
				p->SetTags(p->GetTags() | TermTags::Unique);
			LiftTerm(term, tm);
			return ReductionStatus::Retained;
		}, term);
	});
	RegisterStrict(ctx, "move!", std::bind(DoMoveOrTransfer,
		std::ref(LiftOtherOrCopy), std::placeholders::_1));
	RegisterStrict(ctx, "transfer!", std::bind(DoMoveOrTransfer,
		std::ref(LiftTermValueOrCopy), std::placeholders::_1));
	RegisterStrict(ctx, "ref&", [](TermNode& term){
		CallUnary([&](TermNode& tm){
			LiftToReference(term, tm);
		}, term);
		return ReductionStatus::Retained;
	});
	RegisterBinary<>(ctx, "assign@!", [](TermNode& x, TermNode& y){
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
	RegisterForm(ctx, "$resolve-identifier",
		[](TermNode& term, const ContextNode& c){
		Forms::CallRegularUnaryAs<const TokenValue>([&](string_view id){
			term = CheckSymbol(id, [&]{
				return ResolveIdentifier(c, id);
			});
		}, term);
		return ReductionStatus::Retained;
	});
	// NOTE: This is now be primitive since in NPL environment capture is more
	//	basic than vau.
	RegisterStrict(ctx, "copy-environment", CopyEnvironment);
	RegisterUnary<Strict, const EnvironmentReference>(ctx, "lock-environment",
		[](const EnvironmentReference& wenv) ynothrow{
		return wenv.Lock();
	});
	RegisterUnary<>(ctx, "freeze-environment!", [](TermNode& term){
		Environment::EnsureValid(ResolveEnvironment(term).first).Frozen = true;
		return ValueToken::Unspecified;
	});
	RegisterStrict(ctx, "make-environment", MakeEnvironment);
	RegisterUnary<Strict, const shared_ptr<Environment>>(ctx,
		"weaken-environment", [](const shared_ptr<Environment>& p_env) ynothrow{
		return EnvironmentReference(p_env);
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
	RegisterUnary<Strict, const string>(ctx, "raise-invalid-syntax-error",
		[](const string& str) YB_ATTR(noreturn){
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
	See $2017-02 @ %Documentation::Workflow.
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
		return DoIdFunc(ReduceForLiftedResult, term);
	});

	RegisterForm(renv, "$quote", idv);
	RegisterStrict(renv, "id", [](TermNode& term){
		return DoIdFunc(
			[](TermNode&) YB_ATTR_LAMBDA_QUAL(ynothrow, YB_STATELESS){
			return ReductionStatus::Retained;
		}, term);
	});
	RegisterStrict(renv, "idv", idv);
	RegisterStrict(renv, "list", ReduceBranchToListValue);
	RegisterStrict(renv, "list%", ReduceBranchToList);
	// NOTE: Lazy form '$deflazy!' is the basic operation, which may bind
	//	parameter as unevaluated operands. 
	RegisterForm(renv, "$deflazy!", DefineLazy);
	RegisterForm(renv, "$set!", SetWithNoRecursion);
	RegisterForm(renv, "$setrec!", SetWithRecursion);
	RegisterForm(renv, "$lambda", Lambda);
	RegisterForm(renv, "$lambda%", LambdaRef);
	// NOTE: The sequence operator is also available as infix ';' syntax sugar.
	RegisterForm(renv, "$sequence", Sequence);
	RegisterStrict(renv, "collapse", [](TermNode& term){
		return Forms::CallRawUnary([&](TermNode& tm){
			MoveCollapsed(term, tm);
			return ReductionStatus::Retained;
		}, term);
	});
	RegisterStrict(renv, "forward", [](TermNode& term){
		return Forms::CallRawUnary(std::bind(ReduceToValue, std::ref(term),
			std::placeholders::_1), term);
	});
	RegisterStrict(renv, "forward!", [](TermNode& term){
		return Forms::CallRawUnary([&](TermNode& tm){
			MoveRValueToForward(term, tm);
			return ReductionStatus::Retained;
		}, term);
	});
	RegisterBinary<>(renv, "assign%!", [](TermNode& x, TermNode& y){
		return DoAssign(ystdex::bind1(MoveCollapsed, std::ref(y)), x);
	});
	RegisterBinary<>(renv, "assign!", [](TermNode& x, TermNode& y){
		return DoAssign([&](TermNode& nd_x){
			ResolveTerm([&](TermNode& nd_y, ResolvedTermReferencePtr p_ref_y){
				LiftTermOrCopy(nd_x, nd_y, NPL::IsMovable(p_ref_y));
			}, y);
		}, x);
	});
	RegisterStrict(renv, "apply", Apply);
	RegisterStrict(renv, "list*", ListAsterisk);
	RegisterStrict(renv, "list*%", ListAsteriskRef);
	RegisterStrict(renv, "forward-list-first%", ForwardListFirst);
	RegisterStrict(renv, "first", First);
	RegisterStrict(renv, "first@", FirstAt);
	RegisterStrict(renv, "first&", FirstRef);
	RegisterStrict(renv, "firstv", FirstVal);
	// NOTE: Like 'set-car!' in Kernel, with no references.
	RegisterStrict(renv, "set-first!", SetFirst);
	// NOTE: Like 'set-car!' in Kernel, with reference not collapsed.
	RegisterStrict(renv, "set-first@!", SetFirstAt);
	// NOTE: Like 'set-car!' in Kernel, with reference collapsed.
	RegisterStrict(renv, "set-first%!", SetFirstRef);
	RegisterStrict(renv, "check-environment", CheckEnvironment);
	RegisterForm(renv, "$cond", Cond);
	RegisterForm(renv, "$when", When);
	RegisterForm(renv, "$unless", Unless);
	RegisterUnary<>(renv, "not?", Not);
	RegisterForm(renv, "$and?", And);
	RegisterForm(renv, "$or?", Or);
#else
#	if NPL_Impl_NPLA1_Native_EnvironmentPrimitives
	context.Perform(R"NPL(
		$def! $vau $vau/e (() get-current-environment) (&formals &ef .&body) d
			eval (cons $vau/e (cons d (cons formals (cons ef (move! body))))) d;
		$def! $vau% $vau (&formals &ef .&body) d
			eval (cons $vau/e% (cons d (cons formals (cons ef (move! body)))))
				d;
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
	)NPL");
	// NOTE: The function 'id' does not initialize new objects from the operand.
	// XXX: The implementation of 'id' relies on the fact that an object other
	//	than a reference (i.e. represented by a prvalue) cannot have qualifiers
	//	implying %TermTags::Nonmodifying. Otherwise, a new primitive (e.g. to
	//	cast away the tag) is needed to replace the plain use of 'move!', or it
	//	can just lift the operand as the native %LiftTerm does.
#	if NPL_Impl_NPLA1_Use_Id_Vau
	// NOTE: The parameter shall be in list explicitly as '(.x)' to lift
	//	elements by value rather than by reference (as '&x'), otherwise resulted
	//	'list' is wrongly implemented as 'list%' with undefined behavior becuase
	//	it is not guaranteed the operand is alive to access without lifting.
	//	This is not required in Kernel as it does not differentiate lvalues
	//	(first-class referents) from prvalues and all terms can be accessed as
	//	objects with arbitrary longer lifetime.
	context.Perform(R"NPL(
		$def! id wrap ($vau% (%x) #ignore $if (bound-lvalue? x) x (move! x));
		$def! idv wrap $quote;
		$def! list wrap ($vau (.x) #ignore x);
		$def! list% wrap ($vau &x #ignore x);
	)NPL");
#	else
	RegisterForm(renv, "$lambda", Lambda);
	RegisterForm(renv, "$lambda%", LambdaRef);
	context.Perform(R"NPL(
		$def! id $lambda% (%x) $if (bound-lvalue? x) x (move! x);
		$def! idv $lambda (&x) x;
		$def! list $lambda (.x) x;
		$def! list% $lambda &x x;
	)NPL");
#	endif
	// XXX: The operative '$defv!' is same to following derivations in
	//	%LoadCore.
	context.Perform(R"NPL(
		$def! $deflazy! $vau (&definiend .&expr) d
			eval (list $def! definiend $quote expr) d;
		$def! $set! $vau (&e &formals .&expr) d
			eval (list $def! formals (unwrap eval%) expr d) (eval e d);
		$def! $defv! $vau (&$f &formals &ef .&body) d
			eval (list $set! d $f $vau formals ef (move! body)) d;
		$defv! $setrec! (&e &formals .&expr) d
			eval (list $defrec! formals (unwrap eval%) expr d) (eval e d);
	)NPL");
#	if NPL_Impl_NPLA1_Use_Id_Vau
	context.Perform(R"NPL(
		$defv! $lambda (&formals .&body) d wrap
			(eval (cons $vau (cons formals (cons ignore (move! body)))) d);
		$defv! $lambda% (&formals .&body) d wrap
			(eval (cons $vau% (cons formals (cons ignore (move! body)))) d);
	)NPL");
#	endif
	// XXX: The operatives '$defl!', '$defl%!', '$defw%!', and '$defv%!', as
	//	well as the applicatives 'rest&' and 'rest%' are same to following
	//	derivations in %LoadCore.
	// NOTE: Use of 'eql?' is more efficient than '$if'.
	context.Perform(R"NPL(
		$def! $sequence
			($lambda (&se)
				($lambda #ignore $vau/e% se &exprseq d
					$if (null? exprseq) #inert (eval% (cons% $aux exprseq) d))
				($set! se $aux
					$vau/e% (weaken-environment se) (&head .&tail) d
						$if (null? tail) (eval% head d)
							(($vau% (&t) e ($lambda% #ignore eval% t e)
								(eval% head d)) (eval% (cons% $aux tail) d))))
			(make-environment (() get-current-environment));
		$defv! $defl! (&f &formals .&body) d
			eval (list $set! d f $lambda formals (move! body)) d;
		$defv! $defl%! (&f &formals .&body) d
			eval (list $set! d f $lambda% formals (move! body)) d;
		$defl%! collapse (%x)
			$if (uncollapsed? ($resolve-identifier x)) (idv x) x;
		$defl%! forward (%x)
			$if (bound-lvalue? ($resolve-identifier x)) x (idv x);
		$defl%! forward! (%x)
			$if (bound-lvalue? ($resolve-identifier x)) x (move! x);
		$defl! assign! (&x &y) assign@! (forward! x) (idv (collapse y));
		$defl! assign%! (&x &y) assign@! (forward! x) (forward! (collapse y));
		$defl%! apply (&appv &arg .&opt)
			eval% (cons% () (cons% (unwrap (forward! appv)) (forward! arg)))
				($if (null? opt) (() make-environment)
					(($lambda ((&e .&eopt))
						$if (null? eopt) e
							(raise-invalid-syntax-error
								"Syntax error in applying form.")) opt));
		$defl! list* (&head .&tail)
			$if (null? tail) head (cons head (apply list* tail));
		$defl%! list*% (&head .&tail) $if (null? tail) (forward! head)
			(cons% (forward! head) (apply list*% tail));
		$defv! $defw%! (&f &formals &ef .&body) d
			eval (list $set! d f wrap (list* $vau% formals ef (move! body))) d;
		$defw%! forward-list-first% (&list-appv &appv &l) d
			($lambda% ((&x .))
				eval% (list% (forward! appv) ($resolve-identifier x)) d)
				(eval% (list% (forward! list-appv) l) d);
		$defl%! first@ (&l) ($lambda% ((@x .)) x) (check-list-reference l);
		$defl%! first (%l)
			($lambda% (fwd) forward-list-first% fwd forward! l)
				($if (bound-lvalue? ($resolve-identifier l)) id expire);
		$defl%! first& (&l) ($lambda% ((&x .)) x) (check-list-reference l);
		$defl! firstv ((&x .)) x;
		$defl! rest& (&l) ($lambda ((#ignore .&x)) x) (check-list-reference l);
		$defl! rest% ((#ignore .%x)) x;
		$defl! set-first! (&l x)
			assign@! (first@ (check-list-reference l)) (move! x);
		$defl! set-first@! (&l &x)
			assign@! (first@ (check-list-reference l)) (forward! x);
		$defl! set-first%! (&l &x)
			assign%! (first@ (check-list-reference l)) (forward! x);
		$defl%! check-environment (&e)
			$sequence ($vau/e% e . #ignore) (forward! e);
		$defv! $defv%! (&$f &formals &ef .&body) d
			eval (list $set! d $f $vau% formals ef (move! body)) d;
		$defv%! $cond &clauses d $if (null? clauses) #inert
			(apply ($lambda% ((&test .&body) .&clauses)
				$if (eval test d) (eval% (move! body) d)
					(apply (wrap $cond) (move! clauses) d)) (move! clauses));
		$defv%! $when (&test .&exprseq) d
			$if (eval test d) (eval% (list*% () $sequence exprseq) d);
		$defv%! $unless (&test .&exprseq) d
			$if (eval test d) #inert (eval% (list*% () $sequence exprseq) d);
		$defl! not? (&x) eql? x #f;
		$defv%! $and? &x d $cond
			((null? x) #t)
			((nullv? (rest& x)) eval% (first (forward! x)) d)
			((eval% (first& x) d) apply (wrap $and?) (rest% (forward! x)) d)
			(#t #f);
		$defv%! $or? &x d $cond
			((null? x) #f)
			((nullv? (rest& x)) eval% (first (forward! x)) d)
			((eval% (first& x) d) eval% (first (forward! x)) d)
			(#t apply (wrap $or?) (rest% (forward! x)) d);
	)NPL");
#endif
}

//! \since build 839
//@{
void
LoadCore(REPLContext& context)
{
	context.Perform(R"NPL(
		$def! $defv! $vau (&$f &formals &ef .&body) d
			eval (list $set! d $f $vau formals ef (move! body)) d;
		$defv! $defv%! (&$f &formals &ef .&body) d
			eval (list $set! d $f $vau% formals ef (move! body)) d;
		$defv! $defv/e! (&$f &e &formals &ef .&body) d
			eval (list $set! d $f $vau/e e formals ef (move! body)) d;
		$defv! $defv/e%! (&$f &e &formals &ef .&body) d
			eval (list $set! d $f $vau/e% e formals ef (move! body)) d;
		$defv! $defl! (&f &formals .&body) d
			eval (list $set! d f $lambda formals (move! body)) d;
		$defv! $defl%! (&f &formals .&body) d
			eval (list $set! d f $lambda% formals (move! body)) d;
		$defl! restv ((#ignore .x)) x;
		$defl! rest& (&l) ($lambda ((#ignore .&x)) x) (check-list-reference l);
		$defl! rest% ((#ignore .%x)) x;
		$defv! $defw! (&f &formals &ef .&body) d
			eval (list $set! d f wrap (list* $vau formals ef (move! body))) d;
		$defv! $defw%! (&f &formals &ef .&body) d
			eval (list $set! d f wrap (list* $vau% formals ef (move! body))) d;
		$defv! $defw/e! (&f &e &formals &ef .&body) d
			eval (list $set! d f wrap (list* $vau/e e formals ef (move! body)))
				d;
		$defv! $defw/e%! (&f &e &formals &ef .&body) d
			eval (list $set! d f wrap (list* $vau/e% e formals ef (move! body)))
				d;
		$defv! $lambda/e (&e &formals .&body) d
			wrap (eval (list* $vau/e e formals ignore (move! body)) d);
		$defv! $lambda/e% (&e &formals .&body) d
			wrap (eval (list* $vau/e% e formals ignore (move! body)) d);
		$defv! $defl/e! (&f &e &formals .&body) d
			eval (list $set! d f $lambda/e e formals (move! body)) d;
		$defv! $defl/e%! (&f &e &formals .&body) d
			eval (list $set! d f $lambda/e% e formals (move! body)) d;
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
	context.Perform(R"NPL(
		$def! (box% box? unbox) () make-encapsulation-type;
		$defl! box (&x) box% x;
		$defl! first-null? (&l) null? (first l);
		$defl! list-rest% (&x) list% (rest% x);
		$defw%! accl (&l &pred? &base &head &tail &sum) d
			$if (apply pred? (list% l) d) (forward! base)
				(eval% (list% accl (list% tail l) pred? (list% sum
					(list% head l) (list% forward! base)) head tail sum) d);
		$defw%! accr (&l &pred? &base &head &tail &sum) d
			$if (apply pred? (list% l) d) (forward! base)
				(eval% (list% sum (list% head l) (list% accr (list% tail l)
					pred? (list% forward! base) head tail sum)) d);
		$defw%! foldr1 (&kons &knil &l) d apply accr
			(list% (forward! l) null? (forward! knil) first rest% kons) d;
		$defw%! map1 (&appv &l) d
			foldr1 ($lambda (&x &xs) cons%
				(apply appv (list% (forward! x)) d) xs) () (forward! l);
		$defl! list-concat (&x &y) foldr1 cons% y (forward! x);
		$defl! append (.&ls) foldr1 list-concat () (move! ls);
		$defv%! $let (&bindings .&body) d
			eval% (list*% () (list*% $lambda (map1 firstv bindings)
				(list (move! body))) (map1 list-rest% bindings)) d;
		$defv%! $let% (&bindings .&body) d
			eval% (list*% () (list*% $lambda% (map1 firstv bindings)
				(list (move! body))) (map1 list-rest% bindings)) d;
		$defv%! $let/d (&bindings &ef .&body) d
			eval% (list*% () (list% wrap (list*% $vau (map1 firstv bindings)
				ef (list (move! body)))) (map1 list-rest% bindings)) d;
		$defv%! $let/d% (&bindings &ef .&body) d
			eval% (list*% () (list% wrap (list*% $vau% (map1 firstv bindings)
				ef (list (move! body)))) (map1 list-rest% bindings)) d;
		$defv%! $let/e (&e &bindings .&body) d
			eval% (list*% () (list*% $lambda/e e (map1 firstv bindings)
				(list (move! body))) (map1 list-rest% bindings)) d;
		$defv%! $let/e% (&e &bindings .&body) d
			eval% (list*% () (list*% $lambda/e% e (map1 firstv bindings)
				(list (move! body))) (map1 list-rest% bindings)) d;
		$defv%! $let* (&bindings .&body) d
			eval% ($if (null? bindings) (list*% $let bindings (move! body))
				(list% $let (list (first bindings))
				(list*% $let* (rest% bindings) (move! body)))) d;
		$defv%! $let*% (&bindings .&body) d
			eval% ($if (null? bindings) (list*% $let* bindings (move! body))
				(list% $let% (list (first bindings))
				(list*% $let*% (rest% bindings) (move! body)))) d;
		$defv%! $letrec (&bindings .&body) d
			eval% (list $let () $sequence (list% $def! (map1 firstv bindings)
				(list*% () list (map1 rest% bindings))) (move! body)) d;
		$defv%! $letrec% (&bindings .&body) d
			eval% (list $let% () $sequence (list% $def! (map1 firstv bindings)
				(list*% () list (map1 rest% bindings))) (move! body)) d;
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
					(() get-current-environment)) (move! body)
					(list* () list symbols)))
				d) res;
		$defv! $provide/d! (&symbols &ef .&body) d $sequence
			(eval (list% $def! symbols (list $let/d () ef $sequence
				(list% ($vau% (&e) d $set! e res (lock-environment d))
					(() get-current-environment)) (move! body)
					(list* () list symbols)))
				d) res;
		$defv! $import! (&e .&symbols) d
			eval% (list $set! d symbols (list* () list symbols)) (eval e d);
		$defl! unfoldable? (&l)
			accr l null? (first-null? l) first-null? rest% $or?;
		$defl%! list-extract (&l &extr) accr l null? ()
			($lambda% (&l) forward-list-first% expire extr l) rest% cons%;
		$defl%! list-extract-first (&l) list-extract l first;
		$defl%! list-extract-rest% (&l) list-extract l rest%;
		$defw%! map-reverse (&appv .&ls) d
			accl (move! ls) unfoldable? () list-extract-first
				list-extract-rest%
				($lambda (&x &xs) cons% (apply appv (forward! x) d) xs);
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
	auto& renv(rctx.GetRecordRef());

	LoadObjects(renv);
	Primitive::Load(rctx);
	Derived::Load(context);
	// NOTE: Prevent the ground environment from modification.
	renv.Frozen = true;
}

} // namespace Ground;
//@}

} // unnamed namespace;

void
LoadGroundContext(REPLContext& context)
{
	// NOTE: Dynamic separator handling is lifted to %REPLContext::Preprocess.
	//	See $2020-02 @ %Documentation::Workflow.
	Ground::Load(context);
}

void
LoadModule_std_environments(REPLContext& context)
{
	auto& renv(context.Root.GetRecordRef());

	RegisterUnary<>(renv, "bound?",
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
				encapsulate (list (list% (forward! value) ())),
			$defv%! $lazy (.&expr) d encapsulate (list (list expr d)),
			$defv%! $lazy/e (&e .&expr) d
				encapsulate (list (list expr (check-environment (eval e d)))),
			$defl%! force (&x)
				$if (promise? x) (force-promise (decapsulate x)) (forward! x),
			$defl%! force-promise (&x) $let ((((&object &env)) x))
				$if (null? env) (forward! object)
				(
					$let% ((&y eval% object env)) $cond
						((null? (first (rest& (first& x)))) first& (first& x))
						((promise? y) $sequence
							(set-first%! x (first (decapsulate (forward! y))))
							(force-promise x))
						(#t $sequence
							($let (((&o &e) first& x))
								list% (assign! o y) (assign@! e ()))
							(forward! y))
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
	RegisterUnary<Strict, const string>(renv, "string-empty?",
		[](const string& str) ynothrow{
			return str.empty();
		});
	RegisterBinary<>(renv, "string<-", [](TermNode& x, TermNode& y){
		ResolveTerm([&](TermNode& nd_x, ResolvedTermReferencePtr p_ref_x){
			if(!p_ref_x || p_ref_x->IsModifiable())
			{
				auto& str_x(NPL::AccessRegular<string>(nd_x, p_ref_x));

				ResolveTerm(
					[&](TermNode& nd_y, ResolvedTermReferencePtr p_ref_y){
					auto& str_y(NPL::AccessRegular<string>(nd_y, p_ref_y));

					if(NPL::IsMovable(p_ref_y))
						str_x = std::move(str_y);
					else
						str_x = str_y;
				}, y);
			}
			else
				ThrowNonmodifiableErrorForAssignee();
		}, x);
		return ValueToken::Unspecified;
	});
	RegisterBinary<Strict, string, string>(renv, "string-contains-ci?",
		[](string x, string y){
		// TODO: Extract 'strlwr'.
		const auto to_lwr([](string& str) ynothrow{
			for(auto& c : str)
				c = ystdex::tolower(c);
		});

		to_lwr(x),
		to_lwr(y);
		return x.find(y) != string::npos;
	});
	RegisterUnary<>(renv, "string->symbol", [](TermNode& term){
		return ResolveTerm([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
			auto& s(NPL::AccessRegular<string>(nd, p_ref));

			return NPL::IsMovable(p_ref) ? StringToSymbol(std::move(s))
				: StringToSymbol(s);
		}, term);
	});
	RegisterUnary<Strict, const TokenValue>(renv, "symbol->string",
		SymbolToString);
	RegisterUnary<Strict, const string>(renv, "string->regex",
		[](const string& str){
		return std::regex(str);
	});
	RegisterStrict(renv, "regex-match?", [](TermNode& term){
		RetainN(term, 2);

		auto i(std::next(term.begin()));
		const auto& str(NPL::ResolveRegular<const string>(NPL::Deref(i)));
		const auto& r(NPL::ResolveRegular<const std::regex>(NPL::Deref(++i)));

		term.Value = std::regex_match(str, r);
		return ReductionStatus::Clean;
	});
}

void
LoadModule_std_io(REPLContext& context)
{
	auto& renv(context.Root.GetRecordRef());

	RegisterUnary<Strict, const string>(renv, "puts", [](const string& str){
		// FIXME: Use %EncodeArg?
		// XXX: Error from 'std::puts' is ignored.
		std::puts(str.c_str());
		return ValueToken::Unspecified;
	});
	RegisterUnary<Strict, const string>(renv, "load",
		[&](const string& filename){
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
	RegisterUnary<Strict, const string>(renv, "env-get", [](const string& var){
		string res;

		FetchEnvironmentVariable(res, var.c_str());
		return res;
	});
	RegisterBinary<Strict, const string, const string>(renv, "env-set",
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
	RegisterUnary<Strict, const string>(renv, "system-quote",
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
	RegisterUnary<Strict, const string>(renv, "SHBuild_BuildGCH_existed_",
		[](const string& str) -> bool{
		if(IO::UniqueFile
			file{uopen(str.c_str(), IO::omode_convb(std::ios_base::in))})
			return file->GetSize() > 0;
		return {};
	});
	RegisterUnary<Strict, const string>(renv, "SHBuild_EnsureDirectory_",
		[](const string& str){
		EnsureDirectory(IO::Path(str));
	});
	RegisterUnary<Strict, const string>(renv, "SHBuild_BuildGCH_mkpdirp_",
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
	RegisterUnary<Strict, const string>(renv, "SHBuild_QuoteS_",
		[](const string& str){
		if(str.find('\'') == string::npos)
			return ystdex::quote(str, '\'');
		throw NPLException("Error in quoted string.");
	});
	RegisterUnary<Strict, const string>(renv, "SHBuild_RaiseError_",
		[](const string& str) YB_ATTR(noreturn){
		throw LoggedEvent(str);
	});
	RegisterBinary<Strict, const string, const string>(renv,
		"SHBuild_RemovePrefix_",
		[&](const string& str, const string& pfx){
		return ystdex::begins_with(str, pfx) ? str.substr(pfx.length()) : str;
	});
	RegisterUnary<Strict, const string>(renv, "SHBuild_SDot_",
		[](const string& str){
		auto res(str);

		for(auto& c : res)
			if(c == '.')
				c = '_';
		return res;
	});
	RegisterUnary<Strict, const string>(renv, "SHBuild_String_absolute_path?_",
		[](const string& str){
		return IO::IsAbsolute(str);
	});
	RegisterUnary<Strict, const string>(renv, "SHBuild_TrimOptions_",
		[](const string& src, const ContextNode& ctx){
		string res(src.get_allocator());
		const auto a(ctx.get_allocator());
		Session sess(a);
		set<size_t> left_qset(a);
		vector<size_t> qlist(a);
		typename string::size_type l(0);
		const auto decomp([&](string_view sv){
			using iter_t = string_view::const_iterator;

			// XXX: As %DelimitedByteParser::DecomposeString.
			if(sv.front() != '\'' && sv.front() != '"')
				ystdex::split_l(sv.cbegin(), sv.cend(), ystdex::isspace,
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
				res += string(YSLib::make_string_view(sv));
				res += ' ';
			}
			l += sv.length();
		});
		auto& lexer(sess.Lexer);
		string cbuf(a);
		size_t i(0);

		yunused(sess.Process(src, [&](char c){
			if(lexer.FilterChar(c, cbuf, NPLUnescape, IgnorePrefix))
			{
				if((c == '\'' || c == '"') && lexer.GetDelimiter() == char())
					left_qset.insert(cbuf.length() - 1);
				YAssert(!cbuf.empty(), "Invalid buffer found.");
				if(lexer.UpdateBack(cbuf.back(), c))
				{
					// NOTE: As %BufferedByteParserBase::QueryLastDelimited.
					const auto s(cbuf.length()
						- (lexer.GetDelimiter() != char() ? 1 : 0));

					if(s != i)
					{
						decomp(string_view(&cbuf[i], s - i));
						i = s;
					}
				}
			}
		}));
		if(cbuf.size() != i)
			decomp(string_view(&cbuf[i], cbuf.size() - i));
		if(!res.empty())
			res.pop_back();
		return res;
	});
}

} // namespace Forms;

} // namespace A1;

} // namespace NPL;

