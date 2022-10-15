/*
	© 2015-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Dependency.cpp
\ingroup NPL
\brief 依赖管理。
\version r7402
\author FrankHB <frankhb1989@gmail.com>
\since build 623
\par 创建时间:
	2015-08-09 22:14:45 +0800
\par 修改时间:
	2022-10-12 18:05 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::Dependency
*/


#include "NPL/YModules.h"
#include YFM_NPL_Dependency // for set, string, UnescapeContext, string_view,
//	ystdex::isspace, std::istream, YSLib::unique_ptr, std::throw_with_nested,
//	std::invalid_argument, ystdex::sfmt, YSLib::share_move, GlobalState,
//	RelaySwitched, trivial_swap, std::bind, SourceName, RetainN,
//	NPL::ResolveRegular, NPL::Deref, A1::NameTypedReducerHandler, std::ref,
//	Forms::CallResolvedUnary, EnsureValueTags, ResolvedTermReferencePtr,
//	TryAccessLeafAtom, TermReference, LiftTerm, MoveResolved, Environment,
//	NPL::AllocateEnvironment, function, ValueObject, AccessPtr,
//	EnvironmentReference, shared_ptr, std::piecewise_construct,
//	NPL::forward_as_tuple, LiftOther, ThrowNonmodifiableErrorForAssignee,
//	ThrowValueCategoryError, ValueToken, ResolveTerm, TokenValue,
//	CheckVariadicArity, A1::AsForm, ystdex::bind1, std::placeholders,
//	NPL::CollectTokens, Strict, LiftOtherOrCopy, IsEmpty,
//	ComposeReferencedTermOp, IsBranch, IsTypedRegular, ReferenceTerm,
//	IsReferenceTerm, IsBoundLValueTerm, IsUniqueTerm, IsModifiableTerm,
//	IsTemporaryTerm, IsUncollapsedTerm, LiftTermRef, NPL::SetContentWith,
//	LiftTermValueOrCopy, ResolveName, ResolveIdentifier,
//	Environment::EnsureValid, NPLException, ReduceToReferenceList,
//	MoveCollapsed, NPL::IsMovable, LiftTermOrCopy, IsBranchedList,
//	AccessFirstSubterm, ThrowInsufficientTermsError, Retain, NPL::AsTermNode,
//	ystdex::fast_any_of, A1::Perform, Ensigil, YSLib::ufexists,
//	YSLib::to_std_string, AssertValueTags, ClearCombiningTags,
//	EmplaceCallResultOrReturn, RemoveHead, TryAccessTerm, ystdex::plus,
//	ystdex::tolower, ReduceReturnUnspecified, YSLib::IO::StreamPut,
//	YSLib::OwnershipTag, YSLib::FetchEnvironmentVariable,
//	YSLib::SetEnvironmentVariable, YSLib::uremove, YSLib::allocate_shared,
//	tuple, YSLib::IO::UniqueFile, ystdex::throw_error;
#include <ystdex/container.hpp> // for ystdex::exists, ystdex::search_map,
//	ystdex::emplace_hint_in_place;
#include YFM_NPL_NPLA1Forms // for EncapsulateValue, Encapsulate, Encapsulated,
//	Decapsulate, NPL::Forms functions, StringToSymbol, SymbolToString;
#include YFM_NPL_NPLAMath // for NumerLeaf, NumberNode, NPL math functions;
#include YFM_YSLib_Service_FileSystem // for YSLib::IO::Path,
//	YSLib::Deployment::InstallHardLink;
#include <ystdex/iterator.hpp> // for std::istreambuf_iterator,
//	ystdex::make_transform;
#include YFM_YSLib_Service_TextFile // for
//	YSLib::IO::SharedInputMappedFileStream, YSLib::Text;
#include <regex> // for std::regex, std::regex_replace, std::regex_match;
#include <ostream> // for std::endl;
#include "NPLA1Internals.h" // for NPL_Impl_NPLA1_Enable_Thunked,
//	ReduceSubsequent, A1::RelayCurrentNext, MoveKeptGuard;
#include YFM_YSLib_Core_YCoreUtilities // for YSLib::LockCommandArguments,
//	YSLib::FetchCommandOutput, YSLib::RandomizeTemplatedString;
#include <ystdex/string.hpp> // for ystdex::begins_with;
#include <ystdex/cstdio.h> // for ystdex::fexists;
#include <cerrno> // for errno, EEXIST, EPERM;

namespace NPL
{

// NOTE: The following options provide documented alternative implementations.
//	Replacing each group of implementations in either native implementations or
//	source derivations shall behaves the same, except the differences allowed by
//	the document. Functions whose definitions provided by a same top-level
//	(relatively to the ground environment) statement are in the same group. The
//	native implementation of forms should provide better performance in general,
//	thus it is enabled by default.
// NOTE: Several derivations are known to result different unspecified values to
//	the native implementations as the effects of the invocations, which are
//	conforming to the specifications. Currently such cases include: '$let*' and
//	'$let*%' move non-empty rvalues bindings differently.

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
	}, [&] YB_LAMBDA_ANNOTATE((string::const_iterator i), , pure){
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

namespace A1
{

YSLib::unique_ptr<std::istream>
OpenFile(const char* filename)
{
	TryRet(YSLib::Text::OpenSkippedBOMtream<
		YSLib::IO::SharedInputMappedFileStream>(YSLib::Text::BOM_UTF_8,
		filename))
	CatchExpr(..., std::throw_with_nested(std::invalid_argument(
		ystdex::sfmt("Failed opening file '%s'.", filename))))
}

YSLib::unique_ptr<std::istream>
OpenUnique(ContextState& cs, string filename)
{
	auto p_is(A1::OpenFile(filename.c_str()));

	// NOTE: Swap guard for %cs.CurrentSource is not used. It is up to the
	//	caller to support PTC or not.
	cs.CurrentSource = YSLib::share_move(filename);
	return p_is;
}


void
PreloadExternal(ContextState& cs, const char* filename)
{
	const auto& global(cs.Global.get());

	TryLoadSource(cs, filename,
		*OpenUnique(cs, string(filename, global.Allocator)));
}

//! \since build 923
namespace
{

//! \since build 955
ReductionStatus
ReduceToLoadFile(TermNode& term, ContextNode& ctx, string filename)
{
	auto& cs(ContextState::Access(ctx));
	const auto& global(cs.Global.get());

	// NOTE: This is explicitly not same to klisp. This is also friendly to PTC.
	// XXX: Same to %A1::ReduceOnce, without setup the next term.
#if NPL_Impl_NPLA1_Enable_Thunked
#	if NPL_Impl_NPLA1_Enable_TCO
	RefTCOAction(ctx).SaveTailSourceName(cs.CurrentSource,
		std::move(cs.CurrentSource));
#	else
	RelaySwitched(ctx, trivial_swap,
		A1::NameTypedReducerHandler(std::bind([&](SourceName& saved_src){
		cs.CurrentSource = std::move(saved_src);
		return ctx.LastStatus;
	}, std::move(cs.CurrentSource)), "restore-source-name"));
#	endif
	term = global.Load(cs, std::move(filename));
	global.Preprocess(term);
	return ContextState::Access(ctx).ReduceOnce.Handler(term, ctx);
#else
	auto saved_src(std::move(cs.CurrentSource));

	term = global.Load(cs, std::move(filename));
	global.Preprocess(term);

	const auto res(ContextState::Access(ctx).ReduceOnce.Handler(term, ctx));

	cs.CurrentSource = std::move(saved_src);
	return res;
#endif
}

} // unnamed namespace;

ReductionStatus
ReduceToLoadExternal(TermNode& term, ContextNode& ctx)
{
	RetainN(term);
	return ReduceToLoadFile(term, ctx, string(NPL::ResolveRegular<const string>(
		NPL::Deref(std::next(term.begin()))), term.get_allocator()));
}

ReductionStatus
RelayToLoadExternal(ContextNode& ctx, TermNode& term)
{
	return RelaySwitched(ctx, trivial_swap,
		A1::NameTypedReducerHandler(std::bind(ReduceToLoadExternal,
		std::ref(term), std::ref(ctx)), "load-external"));
}

namespace Forms
{

//! \since build 758
namespace
{

//! \since build 928
YB_ATTR_nodiscard ReductionStatus
DoMoveOrTransfer(TermNode& term, void(&f)(TermNode&, TermNode&, bool))
{
	return Forms::CallResolvedUnary(
		[&](TermNode& nd, ResolvedTermReferencePtr p_ref){
		// NOTE: Force move. No %IsMovable check is needed.
		// XXX: Term tags are currently not respected in prvalues.
		f(term, nd, !p_ref || p_ref->IsModifiable());
		EnsureValueTags(term.Tags);
		return ReductionStatus::Retained;
	}, term);
}

//! \since build 914
YB_ATTR_nodiscard ReductionStatus
Qualify(TermNode& term, TermTags tag_add)
{
	return Forms::CallRawUnary([&](TermNode& tm){
		if(const auto p = TryAccessLeafAtom<TermReference>(tm))
			p->AddTags(tag_add);
		LiftTerm(term, tm);
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
	const auto copy_parent([&](Environment& dst, const Environment& parent){
		auto p_env(NPL::AllocateEnvironment(a));

		CopyEnvironmentDFS(*p_env, parent);
		dst.Parent = std::move(p_env);
	});
	const auto copy_parent_ptr(
		[&](function<Environment&()> mdst, const ValueObject& vo) -> bool{
		if(const auto p = AccessPtr<EnvironmentReference>(vo))
		{
			if(const auto p_parent = p->Lock())
				copy_parent(mdst(), *p_parent);
			// XXX: Failure of locking is ignored.
			return true;
		}
		else if(const auto p_e = AccessPtr<shared_ptr<Environment>>(vo))
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
		m.emplace(std::piecewise_construct, NPL::forward_as_tuple(b.first),
			NPL::forward_as_tuple(b.second.CreateWith(
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
	term.SetValue(std::move(p_env));
}
//@}

#if NPL_Impl_NPLA1_Native_Forms
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
#endif

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
	ThrowValueCategoryError(nd);
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

//! \since build 897
YB_ATTR_nodiscard YB_FLATTEN ReductionStatus
DoResolve(TermNode(&f)(const ContextNode&, string_view), TermNode& term,
	const ContextNode& c)
{
	Forms::CallRegularUnaryAs<const TokenValue>(
		[&] YB_LAMBDA_ANNOTATE((string_view id), , flatten){
		term = f(c, id);
		EnsureValueTags(term.Tags);
	}, term);
	return ReductionStatus::Retained;
}

#if NPL_Impl_NPLA1_Native_Forms
//! \since build 921
//@{
ReductionStatus
DefineCombiner(TermNode& term, ContextNode& ctx, size_t n,
	ReductionStatus(&op)(TermNode&, ContextNode&))
{
	CheckVariadicArity(term, n);

	auto& con(term.GetContainerRef());
	const auto a(con.get_allocator());
	auto i(con.begin());

	con.insert(++++i, A1::AsForm(a, op));
	// NOTE: There should be one subterm exactly, but the check is still
	//	required as the derivations, so just deferred it in
	//	%Forms::DefineWithNoRecursion, but not some combiner with
	//	%BindSymbol.
	// XXX: Assume the implementation of %Forms::DefineWithNoRecursion is
	//	synchronous.
	return Forms::DefineWithNoRecursion(term, ctx);
}

inline auto
BindDefComb(ReductionStatus(&op)(TermNode&, ContextNode&), size_t n)
	-> decltype(ystdex::bind1(DefineCombiner, std::placeholders::_2, n,
	std::ref(op)))
{
	return ystdex::bind1(DefineCombiner, std::placeholders::_2, n,
		std::ref(op));
}

#if false
// NOTE: The following is an alternative native implementation with less
//	efficiency.
//! \since build 921
YB_NONNULL(2) void
AddDefineFunction(ContextNode& ctx, const char* fn,
	std::initializer_list<const char*> il1,
	std::initializer_list<const char*> il2)
{
	const auto a(ctx.get_allocator());
	TermNode term(a);

	term.emplace(A1::AsForm(a, DefineWithNoRecursion));
	NPL::AddToken(term, fn);
	term.emplace(A1::AsForm(a, Vau));

	auto& t0(*term.emplace(NPL::CollectTokens(il1, a)));

	NPL::AddToken(t0, ".&body");
	NPL::AddToken(term, "d");
	term.emplace(A1::AsForm(a, Eval, Strict));

	auto& t1(*term.emplace());

	t1.emplace(A1::AsForm(a, ListAsterisk, Strict));
	t1.emplace(A1::AsForm(a, DefineWithNoRecursion));
	NPL::AddTokens(t1, il2);

	auto& t2(*t1.emplace());

	t2.emplace(A1::AsForm(a,
		ystdex::bind1(DoMoveOrTransfer, std::ref(LiftOtherOrCopy)), Strict));
	NPL::AddToken(t2, "body");
	NPL::AddToken(term, "d");
	Reduce(term, ctx);
#	if false
	// NOTE: Usage:
	AddDefineFunction(ctx, "$defv!", {"&$f", "&formals", "&ef"},
		{"$f", "$vau", "formals", "ef"});
	AddDefineFunction(ctx, "$defv%!", {"&$f", "&formals", "&ef"},
		{"$f", "$vau%", "formals", "ef"});
	AddDefineFunction(ctx, "$defv/e!", {"&$f", "&p", "&formals", "&ef"},
		{"$f", "$vau/e", "p", "formals", "ef"});
	AddDefineFunction(ctx, "$defv/e%!", {"&$f", "&p", "&formals", "&ef"},
		{"$f", "$vau/e%", "p", "formals", "ef"});
	AddDefineFunction(ctx, "$defw!", {"&f", "&formals", "&ef"},
		{"f", "$wvau", "formals", "ef"});
	AddDefineFunction(ctx, "$defw%!", {"&f", "&formals", "&ef"},
		{"f", "$wvau%", "formals", "ef"});
	AddDefineFunction(ctx, "$defw/e!", {"&f", "&p", "&formals", "&ef"},
		{"f", "$wvau/e", "p", "formals", "ef"});
	AddDefineFunction(ctx, "$defw/e%!", {"&f", "&p", "&formals", "&ef"},
		{"f", "$wvau/e%", "p", "formals", "ef"});
	AddDefineFunction(ctx, "$defl!", {"&f", "&formals"},
		{"f", "$lambda", "formals"});
	AddDefineFunction(ctx, "$defl%!", {"&f", "&formals"},
		{"f", "$lambda%", "formals"});
	AddDefineFunction(ctx, "$defl/e!", {"&f", "&p", "&formals"},
		{"f", "$lambda/e", "p", "formals"});
	AddDefineFunction(ctx, "$defl/e%!", {"&f", "&p", "&formals"},
		{"f", "$lambda/e%", "p", "formals"});
#	endif
}
#endif
//@}

/*!
\brief 创建环境弱引用作为单一父环境的新环境。
\return 新创建环境的非空指针。
\sa NPL::AllocateEnvironment
\since build 942
*/
YB_ATTR_nodiscard YB_FLATTEN shared_ptr<Environment>
CreateEnvironmentWithParent(const Environment::allocator_type& a,
	const EnvironmentReference& r_env)
{
	// XXX: Simlar to %MakeEnvironment.
	ValueObject parent;

	parent.emplace<EnvironmentReference>(r_env);
	return NPL::AllocateEnvironment(a, std::move(parent));
}
#endif

//! \since build 834
//@{
namespace Ground
{

#if false
void
LoadObjects(Environment& env)
{}
#endif

namespace Primitive
{

void
LoadEquals(ContextNode& ctx)
{
	RegisterStrict(ctx, "eq?", Eq);
	RegisterStrict(ctx, "eql?", EqLeaf);
	RegisterStrict(ctx, "eqr?", EqReference);
	RegisterStrict(ctx, "eqv?", EqValue);
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
	RegisterUnary(ctx, "null?", ComposeReferencedTermOp(IsEmpty));
	RegisterUnary(ctx, "nullv?", IsEmpty);
	RegisterUnary(ctx, "branch?", ComposeReferencedTermOp(IsBranch));
	RegisterUnary(ctx, "branchv?", IsBranch);
	RegisterUnary(ctx, "pair?", ComposeReferencedTermOp(IsPair));
	RegisterUnary(ctx, "pairv?", IsPair);
	RegisterUnary(ctx, "symbol?",
		[] YB_LAMBDA_ANNOTATE((const TermNode& x), ynothrow, pure){
		return IsTypedRegular<TokenValue>(ReferenceTerm(x));
	});
	RegisterUnary(ctx, "reference?", IsReferenceTerm);
	RegisterUnary(ctx, "unique?", IsUniqueTerm);
	RegisterUnary(ctx, "modifiable?", IsModifiableTerm);
	RegisterUnary(ctx, "temporary?", IsTemporaryTerm);
	RegisterUnary(ctx, "bound-lvalue?", IsBoundLValueTerm);
	RegisterUnary(ctx, "uncollapsed?", IsUncollapsedTerm);
	RegisterStrict(ctx, "deshare", [](TermNode& term){
		return CallRawUnary([&](TermNode& tm){
			if(const auto p = TryAccessLeafAtom<const TermReference>(tm))
				LiftTermRef(tm, p->get());
			NPL::SetContentWith(term, std::move(tm),
				&ValueObject::MakeMoveCopy);
			return ReductionStatus::Retained;
		}, term);
	});
	RegisterStrict(ctx, "as-const", trivial_swap,
		ystdex::bind1(Qualify, TermTags::Nonmodifying));
	RegisterStrict(ctx, "expire", trivial_swap,
		ystdex::bind1(Qualify, TermTags::Unique));
	RegisterStrict(ctx, "move!", trivial_swap,
		ystdex::bind1(DoMoveOrTransfer, std::ref(LiftOtherOrCopy)));
	RegisterStrict(ctx, "transfer!", trivial_swap,
		ystdex::bind1(DoMoveOrTransfer, std::ref(LiftTermValueOrCopy)));
	RegisterStrict(ctx, "ref&", [](TermNode& term){
		CallUnary([&](TermNode& tm){
			LiftToReference(term, tm);
		}, term);
		return ReductionStatus::Retained;
	});
	RegisterBinary(ctx, "assign@!", [](TermNode& x, TermNode& y){
		return DoAssign(ystdex::bind1(static_cast<void(&)(TermNode&,
			TermNode&)>(LiftOther), std::ref(y)), x);
	});
}

void
LoadLists(ContextNode& ctx)
{
	RegisterStrict(ctx, "cons", Cons);
	RegisterStrict(ctx, "cons%", ConsRef);
	// NOTE: Like '$set-cdr!' in Kernel, with no references.
	RegisterStrict(ctx, "set-rest!", SetRest);
	// NOTE: Like '$set-cdr!' in Kernel.
	RegisterStrict(ctx, "set-rest%!", SetRestRef);
}

//! \since build 908
void
LoadSymbols(ContextNode& ctx)
{
	RegisterUnary<Strict, const TokenValue>(ctx, "desigil", [](TokenValue s){
		return TokenValue(!s.empty() && (s.front() == '&' || s.front() == '%')
			? s.substr(1) : std::move(s));
	});
}

void
LoadEnvironments(ContextNode& ctx)
{
	using namespace std::placeholders;

	// NOTE: The applicative 'copy-es-immutable' is unsupported currently due to
	//	different implementation of control primitives.
	RegisterStrict(ctx, "eval", Eval);
	RegisterStrict(ctx, "eval%", EvalRef);
	RegisterUnary<Strict, const string>(ctx, "bound?",
		[](const string& id, ContextNode& c){
		return bool(ResolveName(c, id).first);
	});
	RegisterForm(ctx, "$resolve-identifier", trivial_swap,
		std::bind(DoResolve, std::ref(ResolveIdentifier), _1, _2));
	RegisterForm(ctx, "$move-resolved!", trivial_swap,
		std::bind(DoResolve, std::ref(MoveResolved), _1, _2));
	// NOTE: This is now primitive since in NPL environment capture is more
	//	basic than vau.
	RegisterStrict(ctx, "copy-environment", CopyEnvironment);
	RegisterUnary<Strict, const EnvironmentReference>(ctx, "lock-environment",
		[](const EnvironmentReference& r_env) ynothrow{
		return r_env.Lock();
	});
	RegisterUnary(ctx, "freeze-environment!", [](TermNode& x){
		Environment::EnsureValid(ResolveEnvironment(x).first).Frozen = true;
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
	RegisterUnary<Strict, const string>(ctx, "raise-error",
		[] YB_LAMBDA_ANNOTATE((const string& str), , noreturn){
		throw NPLException(str);
	});
	RegisterUnary<Strict, const string>(ctx, "raise-invalid-syntax-error",
		[] YB_LAMBDA_ANNOTATE((const string& str), , noreturn){
		ThrowInvalidSyntaxError(str);
	});
	RegisterUnary<Strict, const string>(ctx, "raise-type-error",
		[] YB_LAMBDA_ANNOTATE((const string& str), , noreturn){
		throw TypeError(str);
	});
	RegisterStrict(ctx, "check-list-reference", CheckListReference);
	RegisterStrict(ctx, "check-pair-reference", CheckPairReference);
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
	LoadSymbols(ctx);
	LoadEnvironments(ctx);
	LoadCombiners(ctx);
	LoadErrorsAndChecks(ctx);
	LoadEncapsulations(ctx);
}

} // namespace Primitive;

namespace Derived
{

//! \since build 955
//@{
void
LoadBasicDerived(ContextState& cs)
{
	auto& renv(cs.GetRecordRef());

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

	using namespace std::placeholders;
	const auto idv([](TermNode& term){
		return DoIdFunc(ReduceForLiftedResult, term);
	});

	RegisterForm(renv, "$quote", idv);
	RegisterStrict(renv, "id", [](TermNode& term){
		return DoIdFunc([] YB_LAMBDA_ANNOTATE((TermNode&), ynothrow, const){
			return ReductionStatus::Retained;
		}, term);
	});
	RegisterStrict(renv, "idv", idv);
	RegisterStrict(renv, "list", ReduceBranchToListValue);
	RegisterForm(renv, "$lvalue-identifier?",
		[](TermNode& term, ContextNode& ctx){
		Forms::CallRegularUnaryAs<const TokenValue>([&](string_view id){
			auto pr(ResolveName(ctx, id));

			term.Value = [&]() -> bool{
				if(pr.first)
				{
					auto& tm(*pr.first);

					if(const auto p
						= TryAccessLeafAtom<const TermReference>(tm))
						return p->IsReferencedLValue();
					return !(bool(tm.Tags & TermTags::Unique)
						|| bool(tm.Tags & TermTags::Temporary));
				}
				throw BadIdentifier(id);
			}();
		}, term);
	});
	RegisterStrict(renv, "forward!", [](TermNode& term){
		return Forms::CallRawUnary([&](TermNode& tm){
			MoveRValueToForward(term, tm);
			return ReductionStatus::Retained;
		}, term);
	});
	// NOTE: The list operator is also available as infix ',' syntax sugar. See
	//	%SeparatorPass in NPLA1.cpp for details.
	RegisterStrict(renv, "list%", ReduceBranchToList);
	RegisterStrict(renv, "rlist", [](TermNode& term, ContextNode& ctx){
		return Forms::CallRawUnary([&](TermNode& tm){
			return ReduceToReferenceList(term, ctx, tm);
		}, term);
	});
	RegisterForm(renv, "$remote-eval", RemoteEval);
	RegisterForm(renv, "$remote-eval%", RemoteEvalRef);
	// NOTE: Conceptionally the lazy form '$deflazy!' is a more basic operation
	//	than other environment mutation operations, which may bind parameter as
	//	unevaluated operands.
	RegisterForm(renv, "$deflazy!", DefineLazy);
	RegisterForm(renv, "$set!", SetWithNoRecursion);
	RegisterForm(renv, "$setrec!", SetWithRecursion);
	RegisterForm(renv, "$wvau", WVau);
	RegisterForm(renv, "$wvau%", WVauRef);
	RegisterForm(renv, "$wvau/e", WVauWithEnvironment);
	RegisterForm(renv, "$wvau/e%", WVauWithEnvironmentRef);
	RegisterForm(renv, "$lambda", Lambda);
	RegisterForm(renv, "$lambda%", LambdaRef);
	RegisterForm(renv, "$lambda/e", LambdaWithEnvironment);
	RegisterForm(renv, "$lambda/e%", LambdaWithEnvironmentRef);
	RegisterUnary(renv, "list?", ComposeReferencedTermOp(IsList));
	RegisterStrict(renv, "apply", Apply);
	RegisterStrict(renv, "apply-list", ApplyList);
	// NOTE: The sequence operator is also available as infix ';' syntax sugar.
	//	See %SeparatorPass in NPLA1.cpp for details.
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
	RegisterBinary(renv, "assign%!", [](TermNode& x, TermNode& y){
		return DoAssign(ystdex::bind1(MoveCollapsed, std::ref(y)), x);
	});
	RegisterBinary(renv, "assign!", [](TermNode& x, TermNode& y){
		return DoAssign([&](TermNode& nd_x){
			ResolveTerm([&](TermNode& nd_y, ResolvedTermReferencePtr p_ref_y){
				// NOTE: Self-assignment is not checked directly. This allows
				//	copy assignment to fail as expected. Anyway, the destination
				//	is not modified if the source copy fails.
				LiftTermOrCopy(nd_x, nd_y, NPL::IsMovable(p_ref_y));
			}, y);
		}, x);
	});
	RegisterStrict(renv, "list*", ListAsterisk);
	RegisterStrict(renv, "list*%", ListAsteriskRef);
	RegisterForm(renv, "$defv!", trivial_swap, BindDefComb(Vau, 2));
	RegisterForm(renv, "$defv%!", trivial_swap, BindDefComb(VauRef, 2));
	RegisterForm(renv, "$defv/e!", trivial_swap,
		BindDefComb(VauWithEnvironment, 3));
	RegisterForm(renv, "$defv/e%!", trivial_swap,
		BindDefComb(VauWithEnvironmentRef, 3));
	RegisterForm(renv, "$defw!", trivial_swap, BindDefComb(WVau, 2));
	RegisterForm(renv, "$defw%!", trivial_swap, BindDefComb(WVauRef, 2));
	RegisterForm(renv, "$defw/e!", trivial_swap,
		BindDefComb(WVauWithEnvironment, 3));
	RegisterForm(renv, "$defw/e%!", trivial_swap,
		BindDefComb(WVauWithEnvironmentRef, 3));
	RegisterForm(renv, "$defl!", trivial_swap, BindDefComb(Lambda, 1));
	RegisterForm(renv, "$defl%!", trivial_swap, BindDefComb(LambdaRef, 1));
	RegisterForm(renv, "$defl/e!", trivial_swap,
		BindDefComb(LambdaWithEnvironment, 2));
	RegisterForm(renv, "$defl/e%!", trivial_swap,
		BindDefComb(LambdaWithEnvironmentRef, 2));
	RegisterStrict(renv, "forward-first%", ForwardFirst);
	RegisterStrict(renv, "first", First);
	RegisterStrict(renv, "first@", FirstAt);
	RegisterStrict(renv, "first%", FirstFwd);
	RegisterStrict(renv, "first&", FirstRef);
	RegisterStrict(renv, "firstv", FirstVal);
	RegisterStrict(renv, "rest%", RestFwd);
	RegisterStrict(renv, "rest&", RestRef);
	RegisterStrict(renv, "restv", RestVal);
	// NOTE: Like 'set-car!' in Kernel, with no references.
	RegisterStrict(renv, "set-first!", SetFirst);
	// NOTE: Like 'set-car!' in Kernel, with reference not collapsed.
	RegisterStrict(renv, "set-first@!", SetFirstAt);
	// NOTE: Like 'set-car!' in Kernel, with reference collapsed.
	RegisterStrict(renv, "set-first%!", SetFirstRef);
	RegisterStrict(renv, "equal?", EqualTermValue);
	RegisterStrict(renv, "check-environment", CheckEnvironment);
	RegisterStrict(renv, "check-parent", CheckParent);
	RegisterForm(renv, "$cond", Cond);
	RegisterForm(renv, "$when", When);
	RegisterForm(renv, "$unless", Unless);
	RegisterUnary(renv, "not?", Not);
	RegisterForm(renv, "$and", And);
	RegisterForm(renv, "$or", Or);
	RegisterStrict(renv, "accl", AccL);
	RegisterStrict(renv, "accr", AccR);
	RegisterStrict(renv, "foldr1", FoldR1);
	RegisterStrict(renv, "map1", Map1);
	RegisterUnary<Strict, ResolvedArg<>>(renv, "first-null?",
		[](ResolvedArg<>&& x){
		if(IsBranchedList(x.first))
			return IsEmpty(ReferenceTerm(AccessFirstSubterm(x.first)));
		ThrowInsufficientTermsError(x.first, x.second);
	});
	RegisterStrict(renv, "rulist", [](TermNode& term){
		return Forms::CallRawUnary([&](TermNode& tm){
			return ReduceToReferenceUList(term, tm);
		}, term);
	});
	RegisterStrict(renv, "list-concat", ListConcat);
	RegisterStrict(renv, "append", Append);
	RegisterStrict(renv, "list-extract-first", ListExtractFirst);
	RegisterStrict(renv, "list-extract-rest%", ListExtractRestFwd);
	RegisterStrict(renv, "list-push-front!", ListPushFront);
	RegisterForm(renv, "$let", Let);
	RegisterForm(renv, "$let%", LetRef);
	RegisterForm(renv, "$let/e", LetWithEnvironment);
	RegisterForm(renv, "$let/e%", LetWithEnvironmentRef);
	RegisterForm(renv, "$let*", LetAsterisk);
	RegisterForm(renv, "$let*%", LetAsteriskRef);
	RegisterForm(renv, "$letrec", LetRec);
	RegisterForm(renv, "$letrec%", LetRecRef);
	RegisterStrict(renv, "make-standard-environment", trivial_swap,
		// NOTE: The weak reference of the ground environment is saved and it
		//	shall not be moved after being called.
		// TODO: Blocked. Use C++14 lambda initializers to simplify the
		//	implementation.
		ystdex::bind1([](TermNode& term, const EnvironmentReference& ce){
		RetainN(term, 0);
		term.SetValue(CreateEnvironmentWithParent(term.get_allocator(), ce));
	}, cs.WeakenRecord()));
	RegisterStrict(renv, "derive-current-environment",
		[] YB_LAMBDA_ANNOTATE((TermNode& term, ContextNode& ctx), , flatten){
		Retain(term);
		term.emplace(NPL::AsTermNode(term.get_allocator(), ctx.WeakenRecord()));
		return MakeEnvironment(term);
	});
	RegisterStrict(renv, "derive-environment", trivial_swap,
		// NOTE: As 'make-standard-environment'.
		// TODO: Blocked. Use C++14 lambda initializers to simplify the
		//	implementation.
		ystdex::bind1([] YB_LAMBDA_ANNOTATE(
		(TermNode& term, const EnvironmentReference& ce), , flatten){
		Retain(term);
		term.emplace(NPL::AsTermNode(term.get_allocator(), ce));
		return MakeEnvironment(term);
	}, cs.WeakenRecord()));
	RegisterForm(renv, "$as-environment", AsEnvironment);
	RegisterForm(renv, "$bindings/p->environment",
		BindingsWithParentToEnvironment);
	RegisterForm(renv, "$bindings->environment", BindingsToEnvironment);
	RegisterStrict(renv, "symbols->imports", SymbolsToImports);
	RegisterForm(renv, "$provide/let!", ProvideLet);
	RegisterForm(renv, "$provide!", Provide);
	RegisterForm(renv, "$import!", Import);
	RegisterForm(renv, "$import&!", ImportRef);
	RegisterUnary<Strict, ResolvedArg<>>(renv, "nonfoldable?",
		[](ResolvedArg<>&& x){
		auto& nd(x.get());

		if(IsList(nd))
			return ystdex::fast_any_of(nd.begin(), nd.end(),
				ComposeReferencedTermOp(IsEmpty));
		// XXX: This is from 'first-null?' in the alternative derivation.
		ThrowInsufficientTermsError(nd, x.second);
	});
	RegisterStrict(renv, "assq", Assq);
	RegisterStrict(renv, "assv", Assv);
	{
		const auto a(cs.get_allocator());
		// NOTE: As %MakeEncapsulationType.
		shared_ptr<void> p_type(new yimpl(byte));

		renv.Define("box", A1::MakeForm(a, EncapsulateValue(p_type), Strict)),
		renv.Define("box%", A1::MakeForm(a, Encapsulate(p_type), Strict)),
		renv.Define("box?", A1::MakeForm(a, Encapsulated(p_type), Strict)),
		renv.Define("unbox", A1::MakeForm(a, Decapsulate(p_type), Strict));
	}
#else
	cs.ShareCurrentSource("<root:basic-derived>");
	A1::Perform(cs,
#	if NPL_Impl_NPLA1_Native_EnvironmentPrimitives
	R"NPL(
$def! forward! wrap
	($vau/e% (() get-current-environment) (%x) #ignore
		$if (bound-lvalue? ($resolve-identifier x)) x (move! x));
$def! $vau $vau/e (() get-current-environment) (&formals &ef .&body) d
	eval (cons $vau/e (cons d (cons% (forward! formals)
		(cons% ef (forward! body))))) d;
$def! $vau% $vau (&formals &ef .&body) d
	eval (cons $vau/e% (cons d (cons% (forward! formals)
		(cons% ef (forward! body))))) d;
	)NPL"
#	else
	R"NPL(
$def! get-current-environment (wrap ($vau () d d));
$def! lock-current-environment (wrap ($vau () d lock-environment d));
	)NPL"
#	endif
	R"NPL(
$def! $quote $vau% (x) #ignore $move-resolved! x;
	)NPL"
	// NOTE: The function 'id' does not initialize new objects from the operand.
	// XXX: The implementation of 'id' relies on the fact that an object other
	//	than a reference (i.e. represented by a prvalue) cannot have qualifiers
	//	implying %TermTags::Nonmodifying. Otherwise, a new primitive (e.g. to
	//	cast away the tag) is needed to replace the plain use of 'move!', or it
	//	can just lift the operand as the native %LiftTerm does.
#	if NPL_Impl_NPLA1_Use_Id_Vau
	// NOTE: The parameter shall be in a list explicitly as '(.x)' to lift
	//	elements by value rather than by reference (as '&x'), otherwise the
	//	resulted 'list' is wrongly implemented as 'list%' with undefined
	//	behavior becuase it is not guaranteed the operand is alive to access.
	//	This is not the same in Kernel as it does not differentiate lvalues
	//	(first-class referents) from prvalues and all terms can be accessed as
	//	objects with arbitrary longer lifetime.
	R"NPL(
$def! id wrap ($vau% (%x) #ignore $move-resolved! x);
$def! idv wrap $quote;
$def! list wrap ($vau (.x) #ignore move! x);
	)NPL"
#	else
	);
	RegisterForm(renv, "$lambda", Lambda);
	RegisterForm(renv, "$lambda%", LambdaRef);
	cs.ShareCurrentSource("<root:basic-derived-1>");
	A1::Perform(cs, R"NPL(
$def! id $lambda% (%x) $move-resolved! x;
$def! idv $lambda% (x) $move-resolved! x;
$def! list $lambda (.x) move! x;
	)NPL"
#	endif
	R"NPL(
$def! $lvalue-identifier? $vau (&s) d
	eval (list bound-lvalue? (list $resolve-identifier s)) d;
	)NPL"
#	if NPL_Impl_NPLA1_Use_Id_Vau
#		if !NPL_Impl_NPLA1_Native_EnvironmentPrimitives
	R"NPL(
$def! forward! wrap
	($vau% (%x) #ignore $if ($lvalue-identifier? x) x (move! x));
	)NPL"
#		endif
	R"NPL(
$def! list% wrap ($vau &x #ignore forward! x);
$def! rlist wrap ($vau ((.&x)) #ignore forward! (check-list-reference x));
	)NPL"
#	else
#		if !NPL_Impl_NPLA1_Native_EnvironmentPrimitives
	R"NPL(
$def! forward! $lambda% (%x) $if ($lvalue-identifier? x) x (move! x);
	)NPL"
#		endif
	R"NPL(
$def! list% $lambda &x forward! x;
$def! rlist $lambda ((.&x)) forward! (check-list-reference x);
	)NPL"
#	endif
	R"NPL(
$def! $remote-eval $vau (&o &e) d eval (forward! o) (eval e d);
$def! $remote-eval% $vau% (&o &e) d eval% (forward! o) (eval e d);
$def! $deflazy! $vau (&definiend .&body) d
	eval (list% $def! (forward! definiend) $quote (forward! body)) d;
$def! $set! $vau (&e &formals .&body) d
	eval (list% $def! (forward! formals) (unwrap eval%) (forward! body) d)
		(eval e d);
$def! $setrec! $vau (&e &formals .&body) d
	eval (list% $defrec! (forward! formals) (unwrap eval%) (forward! body) d)
		(eval e d);
$def! $wvau $vau (&formals &ef .&body) d
	wrap (eval (cons $vau (cons% (forward! formals) (cons% ef (forward! body))))
		d);
$def! $wvau% $vau (&formals &ef .&body) d
	wrap (eval
		(cons $vau% (cons% (forward! formals) (cons% ef (forward! body)))) d);
$def! $wvau/e $vau (&p &formals &ef .&body) d
	wrap (eval (cons $vau/e
		(cons p (cons% (forward! formals) (cons% ef (forward! body))))) d);
$def! $wvau/e% $vau (&p &formals &ef .&body) d
	wrap (eval (cons $vau/e%
		(cons p (cons% (forward! formals) (cons% ef (forward! body))))) d);
	)NPL"
#	if NPL_Impl_NPLA1_Use_Id_Vau
	R"NPL(
$def! $lambda $vau (&formals .&body) d
	wrap (eval (cons $vau
		(cons% (forward! formals) (cons% #ignore (forward! body)))) d);
$def! $lambda% $vau (&formals .&body) d
	wrap (eval (cons $vau%
		(cons% (forward! formals) (cons% #ignore (forward! body)))) d);
	)NPL"
#	endif
	// NOTE: Use of 'eqv?' is more efficient than '$if'.
	R"NPL(
$def! $lambda/e $vau (&p &formals .&body) d
	wrap (eval (cons $vau/e
		(cons p (cons% (forward! formals) (cons% #ignore (forward! body))))) d);
$def! $lambda/e% $vau (&p &formals .&body) d
	wrap (eval (cons $vau/e%
		(cons p (cons% (forward! formals) (cons% #ignore (forward! body))))) d);
$def! list? $lambda (&o) eql? o ();
$def! apply $lambda% (&appv &arg .&opt)
	eval% (cons% () (cons% (unwrap (forward! appv)) (forward! arg)))
		($if (null? opt) (() make-environment)
			(($lambda ((&e .&eopt))
				$if (null? eopt) e
					(raise-invalid-syntax-error
						"Syntax error in applying form.")) opt));
$def! apply-list $lambda% (&appv &arg .&opt)
	eval% (cons% ($if (list? arg) () $if) (cons% (unwrap (forward! appv))
		(forward! arg)))
		($if (null? opt) (() make-environment)
			(($lambda ((&e .&eopt))
				$if (null? eopt) e
					(raise-invalid-syntax-error
						"Syntax error in applying form.")) opt));
$def! $sequence
	($lambda (&se)
		($lambda #ignore $vau/e% se &exprseq d
			$if (null? exprseq) #inert
				(apply-list (wrap $aux) (forward! exprseq) d))
		($set! se $aux
			$vau/e% (weaken-environment se) (&head .&tail) d
				$if (null? tail) (eval% (forward! head) d)
					(($vau% (&t) e ($lambda% #ignore eval% t e)
						(eval% (forward! head) d))
						(eval% (cons% $aux (forward! tail)) d))))
	(make-environment (() get-current-environment));
$def! collapse $lambda% (%x)
	$if (uncollapsed? x) (($if ($lvalue-identifier? x) ($lambda% (%x) x) id)
		($if (modifiable? x) (idv x) (as-const (idv x)))) ($move-resolved! x);
$def! forward $lambda% (%x) $if ($lvalue-identifier? x) x (idv x);
$def! assign! $lambda (&x &y) assign@! (forward! x) (idv (collapse y));
$def! assign%! $lambda (&x &y) assign@! (forward! x) (forward! (collapse y));
$def! list* $lambda (&head .&tail)
	$if (null? tail) (forward! head)
		(cons (forward! head) (apply-list list* (forward! tail)));
$def! list*% $lambda% (&head .&tail)
	$if (null? tail) (forward! head)
		(cons% (forward! head) (apply-list list*% (forward! tail)));
$def! $defv! $vau (&$f &formals &ef .&body) d
	eval (list*% $def! $f $vau (forward! formals) ef (forward! body)) d;
$defv! $defv%! (&$f &formals &ef .&body) d
	eval (list*% $def! $f $vau% (forward! formals) ef (forward! body)) d;
$defv! $defv/e! (&$f &p &formals &ef .&body) d
	eval (list*% $def! $f $vau/e p (forward! formals) ef (forward! body)) d;
$defv! $defv/e%! (&$f &p &formals &ef .&body) d
	eval (list*% $def! $f $vau/e% p (forward! formals) ef (forward! body)) d;
$defv! $defw! (&f &formals &ef .&body) d
	eval (list*% $def! f $wvau (forward! formals) ef (forward! body)) d;
$defv! $defw%! (&f &formals &ef .&body) d
	eval (list*% $def! f $wvau% (forward! formals) ef (forward! body)) d;
$defv! $defw/e! (&f &p &formals &ef .&body) d
	eval (list*% $def! f $wvau/e p (forward! formals) ef (forward! body)) d;
$defv! $defw/e%! (&f &p &formals &ef .&body) d
	eval (list*% $def! f $wvau/e% p (forward! formals) ef (forward! body)) d;
$defv! $defl! (&f &formals .&body) d
	eval (list*% $def! f $lambda (forward! formals) (forward! body)) d;
$defv! $defl%! (&f &formals .&body) d
	eval (list*% $def! f $lambda% (forward! formals) (forward! body)) d;
$defv! $defl/e! (&f &p &formals .&body) d
	eval (list*% $def! f $lambda/e p (forward! formals) (forward! body)) d;
$defv! $defl/e%! (&f &p &formals .&body) d
	eval (list*% $def! f $lambda/e% p (forward! formals) (forward! body)) d;
$defw%! forward-first% (&appv (&x .)) d
	apply (forward! appv) (list% ($move-resolved! x)) d;
$defl%! first (&pr)
	$if ($lvalue-identifier? pr) (($lambda% ((@x .)) $if (uncollapsed? x)
		($if (modifiable? x) (idv x) (as-const (idv x))) x) pr)
		(forward-first% idv (expire pr));
$defl%! first@ (&pr) ($lambda% ((@x .))
	$if (unique? ($resolve-identifier pr)) (expire x) x)
	($if (unique? ($resolve-identifier pr)) pr
		(check-pair-reference (forward! pr)));
$defl%! first% (&pr)
	($lambda (fwd (@x .)) fwd x) ($if ($lvalue-identifier? pr) id expire) pr;
$defl%! first& (&pr)
	($lambda% ((@x .)) $if (uncollapsed? x)
		($if (modifiable? pr) (idv x) (as-const (idv x)))
		($if (unique? ($resolve-identifier pr)) (expire x) x))
	($if (unique? ($resolve-identifier pr)) pr
		(check-pair-reference (forward! pr)));
$defl! firstv ((&x .)) $move-resolved! x;
$defl%! rest% ((#ignore .%xs)) $move-resolved! xs;
$defl%! rest& (&pr)
	($lambda% ((#ignore .&xs)) $if (unique? ($resolve-identifier pr))
		(expire xs) ($resolve-identifier xs))
	($if (unique? ($resolve-identifier pr)) pr
		(check-pair-reference (forward! pr)));
$defl! restv ((#ignore .xs)) $move-resolved! xs;
$defl! set-first! (&pr x) assign@! (first@ (forward! pr)) (move! x);
$defl! set-first@! (&pr &x) assign@! (first@ (forward! pr)) (forward! x);
$defl! set-first%! (&pr &x) assign%! (first@ (forward! pr)) (forward! x);
$defl! equal? (&x &y)
	$if ($if (pair? x) (pair? y) #f)
		($if (equal? (first& x) (first& y)) (equal? (rest& x) (rest& y)) #f)
		(eqv? x y);
$defl%! check-environment (&e) $sequence (eval% #inert e) (forward! e);
$defl%! check-parent (&p) $sequence ($vau/e% p . #ignore) (forward! p);
$defv%! $cond &clauses d
	$if (null? clauses) #inert
		(apply-list ($lambda% ((&test .&body) .&clauses)
			$if (eval test d) (eval% (forward! body) d)
			(apply (wrap $cond) (forward! clauses) d)) (forward! clauses));
$defv%! $when (&test .&exprseq) d
	$if (eval test d) (eval% (list* () $sequence (forward! exprseq)) d);
$defv%! $unless (&test .&exprseq) d
	$if (eval test d) #inert
		(eval% (list* () $sequence (forward! exprseq)) d);
$defl! not? (&x) eqv? x #f;
$defv%! $and &x d
	$cond
		((null? x) #t)
		((null? (rest& x)) eval% (first (forward! x)) d)
		((eval% (first& x) d) eval% (cons% $and (rest% (forward! x))) d)
		(#t #f);
$defv%! $or &x d
	$cond
		((null? x) #f)
		((null? (rest& x)) eval% (first (forward! x)) d)
		(#t ($lambda% (&r) $if r (forward! r) (eval%
			(cons% $or (rest% (forward! x))) d)) (eval% (move! (first& x)) d));
$defw%! accl (&l &pred? &base &head &tail &sum) d
	$if (apply pred? (list% l) d) (forward! base)
		(apply accl (list% (apply tail (list% l) d) pred?
			(apply sum (list% (apply head (list% l) d)
			(forward! base)) d) head tail sum) d);
$defw%! accr (&l &pred? &base &head &tail &sum) d
	$if (apply pred? (list% l) d) (forward! base)
		(apply sum (list% (apply head (list% l) d)
			(apply accr (list% (apply tail (list% l) d)
			pred? (forward! base) head tail sum) d)) d);
$defw%! foldr1 (&kons &knil &l) d
	apply accr (list% (($lambda ((.@xs)) xs) (check-list-reference l)) null?
		(forward! knil) ($if ($lvalue-identifier? l) ($lambda (&l) first% l)
		($lambda (&l) expire (first% l))) rest% kons) d;
$defw%! map1 (&appv &l) d
	foldr1 ($lambda (%x &xs) cons%
		(apply appv (list% ($move-resolved! x)) d) (move! xs)) () (forward! l);
$defl! first-null? (&l) null? (first l);
$defl%! rulist (&l)
	$if ($lvalue-identifier? l)
		(accr (($lambda ((.@xs)) xs) (check-list-reference l)) null? ()
			($lambda% (%l) $sequence ($def! %x idv (first@ l))
				(($if (uncollapsed? x) idv expire) (expire x))) rest%
			($lambda (%x &xs) (cons% ($resolve-identifier x) (move! xs))))
		(idv (forward! (check-list-reference l)));
$defl! list-concat (&x &y) foldr1 cons% (forward! y) (forward! x);
$defl! append (.&ls) foldr1 list-concat () (forward! ls);
$defl%! list-extract-first (&l) map1 first (forward! l);
$defl%! list-extract-rest% (&l) map1 rest% (forward! l);
$defl! list-push-front! (&l &x) $if (modifiable? (check-list-reference l))
	(assign! l (cons% (forward! x) (idv l)))
	(raise-type-error "Modifiable object expected.");
$def! ($let $let% $let/e $let/e% $let* $let*% $letrec $letrec%) ($lambda (&ce)
(
	$def! mods () ($lambda/e ce ()
	(
		$defv%! $lqual (&ls) d
			($if (eval (list $lvalue-identifier? ls) d) id rulist) (eval% ls d);
		$defv%! $lqual* (&x) d
			($if (eval (list $lvalue-identifier? x) d) id expire) (eval% x d);
		$defl%! mk-let ($ctor &bindings &body)
			list* () (list* $ctor (list-extract-first bindings)
				(list% (forward! body))) (list-extract-rest% bindings);
		$defl%! mk-let/e ($ctor &p &bindings &body)
			list* () (list* $ctor p (list-extract-first bindings)
				(list% (forward! body))) (list-extract-rest% bindings);
		$defl%! mk-let* ($let $let* &bindings &body)
			$if (null? bindings) (list* $let () (forward! body))
				(list $let (list (first% ($lqual* bindings)))
				(list* $let* (rest% ($lqual* bindings)) (forward! body)));
		$defl%! mk-letrec ($let &bindings &body)
			list $let () $sequence (list $def! (list-extract-first bindings)
				(list* () list (list-extract-rest% bindings))) (forward! body);
		() lock-current-environment
	));
	$defv/e%! $let mods (&bindings .&body) d
		eval% (mk-let $lambda ($lqual bindings) (forward! body)) d;
	$defv/e%! $let% mods (&bindings .&body) d
		eval% (mk-let $lambda% ($lqual bindings) (forward! body)) d;
	$defv/e%! $let/e mods (&p &bindings .&body) d
		eval% (mk-let/e $lambda/e p ($lqual bindings) (forward! body)) d;
	$defv/e%! $let/e% mods (&p &bindings .&body) d
		eval% (mk-let/e $lambda/e% p ($lqual bindings) (forward! body)) d;
	$defv/e%! $let* mods (&bindings .&body) d
		eval% (mk-let* $let $let* ($lqual* bindings) (forward! body)) d;
	$defv/e%! $let*% mods (&bindings .&body) d
		eval% (mk-let* $let% $let*% ($lqual* bindings) (forward! body)) d;
	$defv/e%! $letrec mods (&bindings .&body) d
		eval% (mk-letrec $let ($lqual bindings) (forward! body)) d;
	$defv/e%! $letrec% mods (&bindings .&body) d
		eval% (mk-letrec $let% ($lqual bindings) (forward! body)) d;
	map1 move! (list% $let $let% $let/e $let/e% $let* $let*% $letrec $letrec%)
)) (() get-current-environment);
$defw! derive-current-environment (.&envs) d
	apply make-environment (append envs (list d)) d;
	)NPL"
#	if NPL_Impl_NPLA1_Use_LockEnvironment
	R"NPL(
$defl! make-standard-environment () () lock-current-environment;
	)NPL"
#		if true
	R"NPL(
$def! derive-environment ()
	($vau () d $lambda/e (() lock-current-environment) (.&envs)
		() ($lambda/e (append envs (list d)) () () lock-current-environment));
	)NPL"
#		else
	// XXX: This is also correct, but less efficient.
	R"NPL(
$def! derive-environment ()
	($vau () d eval (list $lambda/e (() lock-current-environment)
		((unwrap list) .&envs) () (list $lambda/e ((unwrap list) append envs
			(list d)) () () lock-current-environment)) d);
	)NPL"
#		endif
#	else
	// XXX: Ground environment is passed by 'ce'.
	R"NPL(
$def! make-standard-environment
	($lambda (&se &e) ($lambda #ignore $lambda/e se () make-environment ce)
		($set! se ce e))
	(make-environment (() get-current-environment))
	(() get-current-environment);
$def! derive-environment
	($lambda (&se &e)
		($lambda #ignore
			$lambda/e se (.&envs)
				apply make-environment (append envs (list ce)))
		($set! se ce e))
	(make-environment (() get-current-environment))
	(() get-current-environment);
	)NPL"
#	endif
	R"NPL(
$defv! $as-environment (.&body) d
	eval (list $let () (list% $sequence (forward! body)
		(list () lock-current-environment))) d;
$defv! $bindings/p->environment (&parents .&bindings) d $sequence
	($def! (res bref) list (apply make-environment
		(map1 ($lambda% (x) eval% x d) parents)) (rulist bindings))
	(eval% (list $set! res (list-extract-first bref)
		(list* () list (list-extract-rest% bref))) d)
	res;
$defv! $bindings->environment (.&bindings) d
	eval (list* $bindings/p->environment () (forward! bindings)) d;
$defl! symbols->imports (&symbols)
	list* () list% (map1 ($lambda (&s) list forward! (desigil s))
		(forward! symbols));
$defv! $provide/let! (&symbols &bindings .&body) d
	eval% (list% $let (forward! bindings) $sequence
		(forward! body) (list% $set! d (append symbols ((unwrap list%) .))
		(symbols->imports symbols)) (list () lock-current-environment)) d;
$defv! $provide! (&symbols .&body) d
	eval (list*% $provide/let! (forward! symbols) () (forward! body)) d;
$defv! $import! (&e .&symbols) d
	eval% (list $set! d (append symbols ((unwrap list%) .))
		(symbols->imports symbols)) (eval e d);
$defv! $import&! (&e .&symbols) d
	eval% (list $set! d (append (map1 ensigil symbols)
		((unwrap list%) .)) (symbols->imports symbols)) (eval e d);
$defl! nonfoldable? (&l)
	$if (null? l) #f ($if (first-null? l) #t (nonfoldable? (rest& l)));
$defl%! assq (&x &alist) $cond ((null? alist))
	((eq? x (first& (first& alist))) first% alist)
	(#t assq (forward! x) (rest& alist));
$defl%! assv (&x &alist) $cond ((null? alist))
	((eqv? x (first& (first& alist))) first% alist)
	(#t assv (forward! x) (rest% alist));
$def! (box% box? unbox) () make-encapsulation-type;
$defl! box (x) box% (move! x);
	)NPL"
	);
#endif
}

void
LoadStandardDerived(ContextState& cs)
{
#if NPL_Impl_NPLA1_Native_Forms
	// XXX: Using %cs instead of 'renv' is occasionally more efficient.
	RegisterUnary<Strict, const TokenValue>(cs, "ensigil", Ensigil);
	RegisterForm(cs, "$binds1?", [](TermNode& term, ContextNode& ctx){
		RetainN(term, 2);

		auto i(term.begin());
		auto& eterm(*++i);
		const auto& id(NPL::ResolveRegular<const TokenValue>(*++i));

		return ReduceSubsequent(eterm, ctx, A1::NameTypedReducerHandler([&]{
			auto p_env(ResolveEnvironment(eterm).first);

			Environment::EnsureValid(p_env);
				term.Value = bool(ctx.Resolve(std::move(p_env), id).first);
			return ReductionStatus::Clean;
		}, "eval-$binds1?-env"));
	});
#else
	// XXX: The derivations depends on %std.strings and core functions
	//	derivations available later in the bodies. Otherwise, they are not
	//	relied on. To avoid cyclic dependencies, the derivation of %ensigl
	//	further avoids specific core functions.
	cs.ShareCurrentSource("<root:standard-derived>");
	A1::Perform(cs, R"NPL(
$def! ensigil $lambda (&s)
	$let/e (derive-current-environment std.strings) ()
		$let ((&str symbol->string s))
			$if (string-empty? str) s
				(string->symbol (++ "&" (symbol->string (desigil s))));
$def! $binds1? $vau (&e &s) d
	$let/e (derive-current-environment std.strings) ()
		eval (list (unwrap bound?) (symbol->string s)) (eval e d);
	)NPL");
#endif
}

void
LoadCore(ContextState& cs)
{
	cs.ShareCurrentSource("<root:core>");
	A1::Perform(cs, R"NPL(
$defw%! map-reverse (&appv .&ls) d
	accl (forward! (check-list-reference ls)) nonfoldable? () list-extract-first
		list-extract-rest%
		($lambda (&x &xs) cons% (apply appv (forward! x) d) (forward! xs));
$defw! for-each-ltr &ls d $sequence (apply map-reverse (forward! ls) d) #inert;
	)NPL");
}

void
Load(ContextState& cs)
{
	LoadBasicDerived(cs);
	LoadStandardDerived(cs);
	LoadCore(cs);
}
//@}

} // namespace Derived;

//! \since build 955
void
Load(ContextState& cs)
{
//	LoadObjects(cs.GetRecordRef());
	Primitive::Load(cs);
	Derived::Load(cs);
	// NOTE: Prevent the ground environment from modification.
	cs.GetRecordRef().Frozen = true;
}

} // namespace Ground;
//@}

#if NPL_Impl_NPLA1_Native_Forms
//! \since build 923
void
CheckRequirement(const string& req)
{
	if(YB_UNLIKELY(req.empty()))
		throw NPLException("Empty requirement name found.");
}

//! \since build 941
YB_ATTR_nodiscard string
FindValidRequirementIn(const vector<string>& specs, const string& req)
{
	YAssert(!req.empty(), "Invalid requirement found.");
	const std::regex re("\\?");

	for(const auto& spec : specs)
	{
		string path(std::regex_replace(spec, re, req));

		if(YSLib::ufexists(path.c_str()))
			return path;
	}
	throw NPLException("No module for requirement '" + YSLib::to_std_string(req)
		+ "' found.");
}


//! \since build 926
//@{
class Promise final
{
private:
	//! \invariant 非空 。
	EnvironmentReference r_env;
	TermNode tm_obj;
	TermNode tm_env;
	observer_ptr<Promise> p_back = {};
	bool eval_ref = true;

public:
	// XXX: Keep parameters of 'TermNode' object type, as it is more efficient,
	//	at least in code generation by x86_64-pc-linux G++ 11.1.
	//! \pre 间接断言：第二参数的标签可表示一等对象的值。
	Promise(ContextNode& ctx, TermNode tm) ynothrow
		: r_env(ctx.WeakenRecord()), tm_obj(std::move(tm)),
		tm_env(tm.get_allocator())
	{
		AssertValueTags(tm_obj);
	}
	//! \pre 间接断言：第三参数的标签可表示一等对象的值。
	Promise(ContextNode& ctx, TermNode tm_e, TermNode tm, bool no_lift) ynothrow
		: r_env(ctx.WeakenRecord()), tm_obj(std::move(tm)),
		tm_env(std::move(tm_e)), eval_ref(no_lift)
	{
		AssertValueTags(tm_obj);
	}
	DefDeCopyMoveCtorAssignment(Promise)

	YB_ATTR_nodiscard YB_PURE friend
		PDefHOp(bool, ==, const Promise& x, const Promise& y) ynothrow
		ImplRet(Encapsulation::Equal(x.GetEnvironment(), y.GetEnvironment())
			&& Encapsulation::Equal(x.GetObject(), y.GetObject()))

	DefPred(const ynothrow, Ready, IsEmpty(GetEnvironment()))
	DefPred(const ynothrow, Lifting, !eval_ref)

	DefGetter(const ynothrow, const TermNode&, Environment, Resolve().tm_env)
	DefGetter(const ynothrow, const TermNode&, Object, Resolve().tm_obj)

	void
	Iterate(Promise&, ResolvedTermReferencePtr, ContextNode&);

	PDefH(void, LiftTo, TermNode& term, bool move)
		ImplExpr(LiftOtherOrCopy(term, Resolve().tm_obj, move))

	void
	Memoize(TermNode&);

	ReductionStatus
	ReduceToResult(TermNode&, ResolvedTermReferencePtr);

	YB_ATTR_nodiscard YB_PURE PDefH(Promise&, Resolve, ) ynothrow
		ImplRet(p_back ? *p_back : *this)
	YB_ATTR_nodiscard YB_PURE PDefH(const Promise&, Resolve, ) const ynothrow
		ImplRet(p_back ? *p_back : *this)
};

void
Promise::Iterate(Promise& prom, ResolvedTermReferencePtr p_ref,
	ContextNode& ctx)
{
	if(prom.p_back)
		tm_obj.Clear(), tm_env.Clear(),
			yunseq(r_env = prom.r_env, p_back = prom.p_back);
	else if(p_ref)
		tm_obj.Clear(), tm_env.Clear(),
		yunseq(r_env = p_ref->GetEnvironmentReference(),
			p_back = make_observer(&prom));
	else
		yunseq(tm_obj = std::move(prom.tm_obj),
			tm_env = std::move(prom.tm_env),
			r_env = ctx.WeakenRecord()), p_back.reset();
}

void
Promise::Memoize(TermNode& tm)
{
	auto& resolved(Resolve());

	MoveCollapsed(resolved.tm_obj, tm);
	resolved.tm_env.Clear();
}

ReductionStatus
Promise::ReduceToResult(TermNode& term, ResolvedTermReferencePtr p_ref)
{
	auto& nd(Resolve().tm_obj);
	// XXX: As %ReduceToFirst in NPLA1Forms without %TermTags::Nonmodifying
	//	propagation and value tags adjustment from the soruce since the source
	//	is trustable to have only the tags of modifiable first-class object.
	const bool list_not_move(!NPL::IsMovable(p_ref) || p_back);

	if(const auto p = TryAccessLeafAtom<const TermReference>(nd))
	{
		if(list_not_move)
		{
			term.CopyContent(nd);
			return ReductionStatus::Retained;
		}
		if(!p->IsReferencedLValue())
		{
			LiftOtherOrCopy(term, p->get(), p->IsMovable());
			return ReductionStatus::Retained;
		}
	}
	else if(list_not_move)
	{
		// XXX: Allocators are not used here for performance.
		term.SetValue(term.get_allocator(), in_place_type<TermReference>,
			nd.Tags, nd, p_ref ? p_ref->GetEnvironmentReference() : r_env);
		return ReductionStatus::Clean;
	}
	LiftOther(term, nd);
	return ReductionStatus::Retained;
}


ReductionStatus
LazyImpl(TermNode& term, ContextNode& ctx, bool no_lift)
{
	Retain(term);
	RemoveHead(term);
	// XXX: This implies %EnsureValueTags.
	ClearCombiningTags(term);
	return EmplaceCallResultOrReturn(term, Promise(ctx, NPL::AsTermNode(
		term.get_allocator(), ctx.WeakenRecord()), std::move(term), no_lift));
}

ReductionStatus
LazyWithDynamic(TermNode& term, ContextNode& ctx, bool no_lift)
{
	CheckVariadicArity(term, 1);
	RemoveHead(term);

	const auto i(term.begin());

	return ReduceSubsequent(*i, ctx,
		A1::NameTypedReducerHandler([&, i]{

		// NOTE: As %CheckEnvironment.
		Environment::EnsureValid(
			ResolveEnvironment(ystdex::as_const(*i)).first);

		auto tm(std::move(*i));

		term.erase(i);
		// XXX: This implies %EnsureValueTags.
		ClearCombiningTags(term);
		return EmplaceCallResultOrReturn(term,
			Promise(ctx, std::move(tm), std::move(term), no_lift));
	}, "eval-lazy-parent"));
}

ReductionStatus
ForcePromise(TermNode& term, ContextNode& ctx, Promise& prom, TermNode& nd,
	ResolvedTermReferencePtr p_ref, bool lift)
{
	if(prom.IsReady())
		return prom.ReduceToResult(term, p_ref);

	// XXX: Assume the 1st subterm is free to use. It is sufficient to save the
	//	expression to be evaluated. Even if not, a subterm can be emplaced at
	//	the front position and then all subterms to save temporary objects would
	//	be destroyed on the lifting, and it should be safe since the order is
	//	guaranteed left-to-right by the destructor of %TermNode.
	auto& nterm(*term.begin());
	auto p_env_saved([&](const TermNode& tm_env){
		// XXX: The 'const' on 'tm_env' is vital, because it cannot be moved,
		//	since %Promise::IsReady relies on the term not being empty (as the
		//	current move-after state).
		return ResolveEnvironment(tm_env).first;
	}(prom.GetEnvironment()));

	// XXX: This may invalidate %prom.
	prom.LiftTo(nterm, NPL::IsMovable(p_ref));
	return A1::ReduceCurrentNext(nterm, ctx, trivial_swap,
		// TODO: Blocked. Use C++14 lambda initializers to simplify the
		//	implementation.
		ystdex::bind1([&, lift](TermNode& t, ContextNode& c,
		shared_ptr<Environment>& p_env){
		return NonTailCall::RelayNextGuardedProbe(c, t,
			EnvironmentGuard(c, c.SwitchEnvironment(std::move(p_env))), lift,
			std::ref(ContextState::Access(c).ReduceOnce));
	}, std::placeholders::_2, std::move(p_env_saved)),
		trivial_swap, A1::NameTypedReducerHandler([&, p_ref]{
		// NOTE: Different to [RnRK], the promise object may be assigned and
		//	%prom may be invalidated during the evaluation on %nterm, so it is
		//	necessary to access %nd again. In this implementation, %prom from
		//	the recursive call to %ForcePromise also reuses %nterm to hold the
		//	old %nprom for nested promise evaluation, so lifetimes of %prom and
		//	%nprom should not overlap.
		auto& nprom(AccessRegular<Promise>(nd, p_ref));

		// NOTE: This is necessary to handle the promise forced during the
		//	evaluation on %nterm, see [RnRK].
		if(!nprom.IsReady())
		{
			if(ResolveTerm(
				[&](TermNode& nd_n, ResolvedTermReferencePtr p_ref_n) -> bool{
				if(const auto p_inner = TryAccessTerm<Promise>(nd_n))
				{
					nprom.Iterate(*p_inner, p_ref_n, ctx);
					lift = lift || p_inner->IsLifting();
					return true;
				}
				return {};
			}, nterm))
				return ForcePromise(term, ctx, nprom, nd, p_ref, lift);
			nprom.Memoize(nterm);
		}
		// XXX: This may invalidate %nprom.
		return nprom.ReduceToResult(term, p_ref);
	}, "promise-handle-result"));
}
//@}


//! \since build 955
template<typename _func>
ReductionStatus
ReduceToLoadGuarded(TermNode& term, ContextNode& ctx,
	shared_ptr<Environment> p_env, _func reduce)
{
#	if NPL_Impl_NPLA1_Enable_Thunked
	return A1::RelayCurrentNext(ctx, term, trivial_swap, std::bind(
		std::move(reduce), std::ref(term), std::ref(ctx)),
		trivial_swap, MoveKeptGuard(EnvironmentGuard(ctx,
		ctx.SwitchEnvironmentUnchecked(std::move(p_env)))));
#	else
	const EnvironmentGuard gd(ctx, ctx.SwitchEnvironmentUnchecked(p_env));

	return reduce(term, ctx);
#	endif
}
#endif


/*!
\ingroup functors
\since build 930
*/
struct LeafPred
{
	template<typename _func>
	YB_ATTR_nodiscard YB_PURE bool
	operator()(const TermNode& nd, _func f) const ynoexcept_spec(f(nd.Value))
	{
		return IsLeaf(nd) && f(nd.Value);
	}
};

} // unnamed namespace;

void
LoadGroundContext(ContextState& cs)
{
	// NOTE: Dynamic separator handling is lifted to %GlobalState::Preprocess.
	//	See $2020-02 @ %Documentation::Workflow.
	Ground::Load(cs);
}

void
LoadModule_std_continuations(ContextState& cs)
{
	auto& renv(cs.GetRecordRef());

	RegisterStrict(renv, "call/1cc", Call1CC);
	RegisterStrict(renv, "continuation->applicative",
		ContinuationToApplicative);
#if NPL_Impl_NPLA1_Native_Forms
	RegisterStrict(renv, "apply-continuation", ApplyContinuation);
#else
	cs.ShareCurrentSource("<lib:std.continuations>");
	A1::Perform(cs, R"NPL(
$defl! apply-continuation (&k &arg)
	apply (continuation->applicative (forward! k)) (forward! arg);
	)NPL");
#endif
}

void
LoadModule_std_promises(ContextState& cs)
{
#if NPL_Impl_NPLA1_Native_Forms
	auto& renv(cs.GetRecordRef());

	RegisterUnary(renv, "promise?",
		[] YB_LAMBDA_ANNOTATE((const TermNode& x), ynothrow, pure){
		return IsTypedRegular<Promise>(ReferenceTerm(x));
	});
	RegisterUnary(renv, "memoize",
		[] YB_LAMBDA_ANNOTATE((TermNode& x, ContextNode& ctx), , pure){
		return Promise(ctx, std::move(x));
	});
	RegisterForm(renv, "$lazy", [](TermNode& term, ContextNode& ctx){
		return LazyImpl(term, ctx, {});
	});
	RegisterForm(renv, "$lazy%", [](TermNode& term, ContextNode& ctx){
		return LazyImpl(term, ctx, true);
	});
	RegisterForm(renv, "$lazy/d", [](TermNode& term, ContextNode& ctx){
		return LazyWithDynamic(term, ctx, {});
	});
	RegisterForm(renv, "$lazy/d%", [](TermNode& term, ContextNode& ctx){
		return LazyWithDynamic(term, ctx, true);
	});
	RegisterStrict(renv, "force", [](TermNode& term, ContextNode& ctx){
		return CallRawUnary([&](TermNode& tm){
			return
				ResolveTerm([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
				if(const auto p = TryAccessTerm<Promise>(nd))
					return
						ForcePromise(term, ctx, *p, nd, p_ref, p->IsLifting());
				LiftOther(term, tm);
				return ReductionStatus::Retained;
			}, tm);
		}, term);
	});
#else
	cs.ShareCurrentSource("<lib:std.promises>");
	// NOTE: The call to 'set-first%!' does not check cyclic references. This is
	//	kept safe since it can occur only with NPLA1 undefined behavior.
	// XXX: The internal construct uses lists instead of pairs as [RnRK] even
	//	after pairs are supported, since the assignment to the referent of the
	//	value introduced by trailing sequence has unspecified behavior in the
	//	object language (and it will not change the shared referent in the
	//	current implementation)..
	A1::Perform(cs, R"NPL(
$provide/let! (promise? memoize $lazy $lazy% $lazy/d $lazy/d% force)
((mods $as-environment (
	$def! (encapsulate% promise? decapsulate) () make-encapsulation-type;
	$defl%! do-force (&prom fwd) $let% ((((&o &env) evf) decapsulate prom))
		$if (null? env) (first (first (decapsulate (fwd prom))))
		(
			$let*% ((&y evf (fwd o) env) (&x decapsulate prom)
				(((&o &env) &evf) x))
				$cond
					((null? env) first (first (decapsulate (fwd prom))))
					((promise? y) $sequence
						($if (eqv? (firstv (rest& (decapsulate y))) eval)
							(assign! evf eval))
						(set-first%! x (first (decapsulate (forward! y))))
						(do-force prom fwd))
					(#t $sequence
						(list% (assign%! o (forward! y)) (assign@! env ()))
						(first (first (decapsulate (fwd prom)))))
		)
)))
(
	$import! mods &promise?,
	$defl/e%! &memoize mods (&x)
		encapsulate% (list (list% (forward! x) ()) #inert),
	$defv/e%! &$lazy mods (.&body) d
		encapsulate% (list (list (forward! body) d) eval),
	$defv/e%! &$lazy% mods (.&body) d
		encapsulate% (list (list (forward! body) d) eval%),
	$defv/e%! &$lazy/d mods (&e .&body) d
		encapsulate%
			(list (list (forward! body) (check-environment (eval e d))) eval),
	$defv/e%! &$lazy/d% mods (&e .&body) d
		encapsulate%
			(list (list (forward! body) (check-environment (eval e d))) eval%),
	$defl/e%! &force mods (&x)
		($lambda% (fwd) $if (promise? x) (do-force x fwd) (fwd x))
			($if ($lvalue-identifier? x) id move!)
);
	)NPL");
#endif
}

void
LoadModule_std_math(ContextState& cs)
{
	auto& renv(cs.GetRecordRef());

	RegisterUnary(renv, "number?", trivial_swap,
		ComposeReferencedTermOp(ystdex::bind1(LeafPred(), IsNumberValue)));
	// NOTE: Currently all numbers are complex numbers.
	RegisterUnary(renv, "complex?", trivial_swap,
		ComposeReferencedTermOp(ystdex::bind1(LeafPred(), IsNumberValue)));
	// NOTE: Currently all numbers are real numbers.
	RegisterUnary(renv, "real?", trivial_swap,
		ComposeReferencedTermOp(ystdex::bind1(LeafPred(), IsNumberValue)));
	RegisterUnary(renv, "rational?", trivial_swap,
		ComposeReferencedTermOp(ystdex::bind1(LeafPred(), IsRationalValue)));
	RegisterUnary(renv, "integer?", trivial_swap,
		ComposeReferencedTermOp(ystdex::bind1(LeafPred(), IsIntegerValue)));
	// NOTE: Currently all exact numbers are integers. This should actually be
	//	the union of fixnum and bignum.
	RegisterUnary(renv, "exact-integer?", trivial_swap,
		ComposeReferencedTermOp(ystdex::bind1(LeafPred(), IsExactValue)));
	RegisterUnary(renv, "fixnum?", trivial_swap,
		ComposeReferencedTermOp(ystdex::bind1(LeafPred(), IsFixnumValue)));
	RegisterUnary(renv, "flonum?", trivial_swap,
		ComposeReferencedTermOp(ystdex::bind1(LeafPred(), IsFlonumValue)));
	RegisterUnary<Strict, const NumberLeaf>(renv, "exact?", IsExactValue);
	RegisterUnary<Strict, const NumberLeaf>(renv, "inexact?", IsInexactValue);
	RegisterUnary<Strict, const NumberLeaf>(renv, "finite?", IsFinite);
	RegisterUnary<Strict, const NumberLeaf>(renv, "infinite?", IsInfinite);
	RegisterUnary<Strict, const NumberLeaf>(renv, "nan?", IsNaN);
	RegisterBinary<Strict, const NumberLeaf, const NumberLeaf>(renv, "=?",
		Equal);
	RegisterBinary<Strict, const NumberLeaf, const NumberLeaf>(renv, "<?",
		Less);
	RegisterBinary<Strict, const NumberLeaf, const NumberLeaf>(renv, ">?",
		Greater);
	RegisterBinary<Strict, const NumberLeaf, const NumberLeaf>(renv, "<=?",
		LessEqual);
	RegisterBinary<Strict, const NumberLeaf, const NumberLeaf>(renv, ">=?",
		GreaterEqual);
	RegisterUnary<Strict, const NumberLeaf>(renv, "zero?", IsZero);
	RegisterUnary<Strict, const NumberLeaf>(renv, "positive?", IsPositive);
	RegisterUnary<Strict, const NumberLeaf>(renv, "negative?", IsNegative);
	RegisterUnary<Strict, const NumberLeaf>(renv, "odd?", IsOdd);
	RegisterUnary<Strict, const NumberLeaf>(renv, "even?", IsEven);
	RegisterBinary<Strict, NumberNode, NumberNode>(renv, "max", Max);
	RegisterBinary<Strict, NumberNode, NumberNode>(renv, "min", Min);
	RegisterUnary<Strict, NumberNode>(renv, "add1", Add1);
	RegisterBinary<Strict, NumberNode, NumberNode>(renv, "+", Plus);
	RegisterUnary<Strict, NumberNode>(renv, "sub1", Sub1);
	RegisterBinary<Strict, NumberNode, NumberNode>(renv, "-", Minus);
	RegisterBinary<Strict, NumberNode, NumberNode>(renv, "*", Multiplies);
	RegisterBinary<Strict, NumberNode, NumberNode>(renv, "/", Divides);
	RegisterUnary<Strict, NumberNode>(renv, "abs", Abs);
	RegisterBinary<Strict, NumberNode, NumberNode>(renv, "floor/",
		FloorDivides);
	RegisterBinary<Strict, NumberNode, NumberNode>(renv, "floor-quotient",
		FloorQuotient);
	RegisterBinary<Strict, NumberNode, NumberNode>(renv, "floor-remainder",
		FloorRemainder);
	RegisterBinary<Strict, NumberNode, NumberNode>(renv, "truncate/",
		TruncateDivides);
	RegisterBinary<Strict, NumberNode, NumberNode>(renv, "truncate-quotient",
		TruncateQuotient);
	RegisterBinary<Strict, NumberNode, NumberNode>(renv, "truncate-remainder",
		TruncateRemainder);
	RegisterUnary<Strict, NumberNode>(renv, "inexact", Inexact);
}

void
LoadModule_std_strings(ContextState& cs)
{
	auto& renv(cs.GetRecordRef());

	RegisterUnary(renv, "string?",
		[] YB_LAMBDA_ANNOTATE((const TermNode& x), ynothrow, pure){
		return IsTypedRegular<string>(ReferenceTerm(x));
	});
	RegisterStrict(renv, "++", trivial_swap,
		std::bind(CallBinaryFold<string, ystdex::plus<>>,
		ystdex::plus<>(), string(), std::placeholders::_1));
	RegisterUnary<Strict, const string>(renv, "string-empty?",
		[](const string& str) ynothrow{
		return str.empty();
	});
	RegisterBinary<Strict, ResolvedArg<>, ResolvedArg<>>(renv, "string<-",
		[](ResolvedArg<>&& x, ResolvedArg<>&& y){
		if(x.IsModifiable())
		{
			auto& str_x(AccessRegular<string>(x.get(), x.second));
			auto& str_y(AccessRegular<string>(y.get(), y.second));

			if(y.IsMovable())
				str_x = std::move(str_y);
			else
				str_x = str_y;
		}
		else
			ThrowNonmodifiableErrorForAssignee();
		return ValueToken::Unspecified;
	});
	RegisterStrict(renv, "string-split", [](TermNode& term){
		return CallBinaryAs<string, const string>(
			[&](string& x, const string& y) -> ReductionStatus{
			if(!x.empty())
			{
				TermNode::Container con(term.get_allocator());
				const auto len(y.length());

				if(len != 0)
				{
					string::size_type pos(0), orig(0);

					while((pos = x.find(y, pos)) != string::npos)
					{
						TermNode::AddValueTo(con, x.substr(orig, pos - orig));
						orig = (pos += len);
					}
					TermNode::AddValueTo(con, x.substr(orig, pos - orig));
				}
				else
					TermNode::AddValueTo(con, std::move(x));
				con.swap(term.GetContainerRef());
				return ReductionStatus::Retained;
			}
			return ReductionStatus::Clean;
		}, term);
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
	RegisterUnary<Strict, ResolvedArg<string>>(renv, "string->symbol",
		[](ResolvedArg<string>&& x){
		return x.IsMovable() ? StringToSymbol(std::move(x.first))
			: StringToSymbol(x.first);
	});
	RegisterUnary<Strict, const TokenValue>(renv, "symbol->string",
		SymbolToString);
	RegisterUnary<Strict, const string>(renv, "string->regex",
		[](const string& str){
		return std::regex(str);
	});
	RegisterBinary<Strict, const string, const std::regex>(renv, "regex-match?",
		[](const string& str, const std::regex& re){
		return std::regex_match(str, re);
	});
	RegisterStrict(renv, "regex-replace", [](TermNode& term){
		RetainN(term, 3);

		auto i(term.begin());
		const auto& str(NPL::ResolveRegular<const string>(NPL::Deref(++i)));
		const auto& re(NPL::ResolveRegular<const std::regex>(NPL::Deref(++i)));

		return EmplaceCallResultOrReturn(term, string(std::regex_replace(str,
			re, NPL::ResolveRegular<const string>(NPL::Deref(++i)))));
	});
}

void
LoadModule_std_io(ContextState& cs)
{
	auto& renv(cs.GetRecordRef());

	RegisterUnary<Strict, const string>(renv, "readable-file?",
		[](const string& str) ynothrow{
		return YSLib::ufexists(str.c_str());
	});
	RegisterUnary<Strict, const string>(renv, "writable-file?",
		[](const string& str) ynothrow{
		return YSLib::ufexists(str.c_str(), true);
	});
	RegisterStrict(renv, "newline", trivial_swap,
		[&](TermNode& term, ContextNode& ctx){
		RetainN(term, 0);
		if(auto&
			os{ContextState::Access(ctx).Global.get().GetOutputStreamRef()})
			os << std::endl;
		return ReduceReturnUnspecified(term);
	});
	RegisterUnary<Strict, const string>(renv, "put", trivial_swap,
		[&](const string& str, ContextNode& ctx){
		YSLib::IO::StreamPut(ContextState::Access(ctx).Global.get()
			.GetOutputStreamRef(), str.c_str());
		return ValueToken::Unspecified;
	});
#if NPL_Impl_NPLA1_Native_Forms
	RegisterUnary<Strict, const string>(renv, "puts", trivial_swap,
		[&](const string& str, ContextNode& ctx){
		auto& os(ContextState::Access(ctx).Global.get().GetOutputStreamRef());

		YSLib::IO::StreamPut(os, str.c_str());
		if(os)
			os << std::endl;
		return ValueToken::Unspecified;
	});
#else
	cs.ShareCurrentSource("<lib:std.io>");
	A1::Perform(cs, R"NPL(
$defl! puts (&s) $sequence (put s) (() newline);
	)NPL");
#endif
	// NOTE: Since %load disallows additional barrier of catching the inner
	//	exceptions and the reset of the source name, %TryLoadSource or
	//	%PreloadExternal is not applicable here.
	RegisterStrict(renv, "load", ReduceToLoadExternal);
#if NPL_Impl_NPLA1_Native_Forms
	RegisterStrict(renv, "get-module", trivial_swap,
		ystdex::bind1([&](TermNode& term, ContextNode& ctx,
		const EnvironmentReference& r_ground){
		CheckVariadicArity(term, 0);

		const auto size(term.size());

		if(size <= 3)
		{
			// XXX: As 'make-standard-environment'.
			auto p_env(
				CreateEnvironmentWithParent(term.get_allocator(), r_ground));

			switch(size)
			{
			case 3:
				{
					auto& con(term.GetContainerRef());

					p_env->Bind("module-parameters", NPL::AsTermNode(
						ResolveEnvironment(std::move(*con.rbegin())).first));
					// XXX: This is needed for the call to %ReduceToLoadExternal
					//	later.
					term.GetContainerRef().pop_back();
				}
				YB_ATTR_fallthrough;
			case 2:
#	if NPL_Impl_NPLA1_Enable_Thunked
				RelaySwitched(ctx, trivial_swap, A1::NameTypedReducerHandler(
					std::bind([&](shared_ptr<Environment> p_res){
					term.SetValue(std::move(p_res));
					return ReductionStatus::Clean;
				}, p_env), "get-module-return"));
				return ReduceToLoadGuarded(term, ctx, std::move(p_env),
					ReduceToLoadExternal);
#	else
				ReduceToLoadGuarded(term, ctx, std::move(p_env),
					ReduceToLoadExternal);
				term.SetValue(std::move(p_env));
				return ReductionStatus::Clean;
#	endif
			}
			YAssert(false, "Invalid state found.");
		}
		ThrowInvalidSyntaxError("Syntax error in get-module.");
	}, std::placeholders::_2, cs.WeakenRecord()));
#else
	cs.ShareCurrentSource("<lib:std.io-1>");
	A1::Perform(cs, R"NPL(
$defl! get-module (&filename .&opt)	
	$let ((env $if (null? opt) (() make-standard-environment)
		($let (((&e .&eopt) opt)) $if (null? eopt)
			($let ((env () make-standard-environment)) $sequence
				($set! env module-parameters (check-environemnt e)) env)
			(raise-invalid-syntax-error "Syntax error in get-module."))))
		$sequence (eval% (list load filename) env) env;
	)NPL");
#endif
}

void
LoadModule_std_system(ContextState& cs)
{
	auto& renv(cs.GetRecordRef());

	RegisterStrict(renv, "get-current-repl", trivial_swap,
		[&](TermNode& term, ContextNode& ctx){
		RetainN(term, 0);
		term.Value.assign(ContextState::Access(ctx).Global.get(),
			YSLib::OwnershipTag<>());
	});
	RegisterStrict(renv, "eval-string", EvalString);
	RegisterStrict(renv, "eval-string%", EvalStringRef);
	RegisterStrict(renv, "eval-unit", EvalUnit);
	RegisterStrict(renv, "cmd-get-args", [](TermNode& term){
		RetainN(term, 0);
		{
			const auto p_cmd_args(YSLib::LockCommandArguments());
			TermNode::Container con(term.get_allocator());

			for(const auto& arg : p_cmd_args->Arguments)
				TermNode::AddValueTo(con, in_place_type<string>, arg);
			con.swap(term.GetContainerRef());
		}
		return ReductionStatus::Retained;
	});
	RegisterUnary<Strict, const string>(renv, "env-get", [](const string& var){
		string res(var.get_allocator());

		YSLib::FetchEnvironmentVariable(res, var.c_str());
		return res;
	});
	RegisterBinary<Strict, const string, const string>(renv, "env-set",
		[](const string& var, const string& val){
		YSLib::SetEnvironmentVariable(var.c_str(), val.c_str());
	});
	cs.ShareCurrentSource("<lib:std.system>");
	A1::Perform(cs, R"NPL(
$defl/e! env-empty? (derive-current-environment std.strings) (&n)
	string-empty? (env-get n);
	)NPL");
	RegisterStrict(renv, "system", CallSystem);
	RegisterStrict(renv, "system-get", [](TermNode& term){
		CallUnaryAs<const string>([&](const string& cmd){
			TermNode::Container con(term.get_allocator());
			auto res(YSLib::FetchCommandOutput(cmd.c_str()));

			TermNode::AddValueTo(con, ystdex::trim(std::move(res.first)));
			TermNode::AddValueTo(con, res.second);
			con.swap(term.GetContainerRef());
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
	RegisterUnary<Strict, const string>(renv, "remove-file",
		[](const string& filename){
		return YSLib::uremove(filename.c_str());
	});
}

void
LoadModule_std_modules(ContextState& cs)
{
#if NPL_Impl_NPLA1_Native_Forms
	using namespace std::placeholders;
	using YSLib::to_std_string;
	auto& renv(cs.GetRecordRef());
	const auto a(renv.Bindings.get_allocator());
	const auto p_registry(YSLib::allocate_shared<YSLib::map<string,
		pair<TermNode, shared_ptr<Environment>>>>(a));
	auto& registry(*p_registry);
	const auto p_specs([&]{
		auto p_vec(YSLib::allocate_shared<vector<string>>(a));
		string x(a);

		YSLib::FetchEnvironmentVariable(x, "NPLA1_PATH");
		if(x.empty())
		{
			p_vec->push_back("./?");
			p_vec->push_back("./?.txt");
		}
		else
		{
			string::size_type pos(0), orig(0);

			while((pos = x.find('?', pos)) != string::npos)
			{
				p_vec->push_back(x.substr(orig, pos - orig));
				orig = ++pos;
			}
			p_vec->push_back(x.substr(orig, pos - orig));
		}
		return p_vec;
	}());
	auto& specs(*p_specs);

	RegisterUnary<Strict, const string>(renv, "registered-requirement?",
		// TODO: Blocked. Use C++14 lambda initializers to optimize the
		//	implementation.
		// XXX: Not using binding to prevent overloading ambiguity.
		[p_registry](const string& req){
		if(!req.empty())
			return ystdex::exists(NPL::Deref(p_registry), req);
		throw NPLException("Empty requirement name found.");
	});
	RegisterStrict(renv, "register-requirement!", trivial_swap,
		ystdex::bind1([&](TermNode& term, const EnvironmentReference& r_ground){
		auto i(term.begin());
		const auto& req(NPL::ResolveRegular<const string>(NPL::Deref(++i)));

		CheckRequirement(req);

		const auto pr(ystdex::search_map(registry, req));

		if(pr.second)
		{
			term.Value.emplace<EnvironmentReference>(
				ystdex::emplace_hint_in_place(registry, pr.first, req,
				std::piecewise_construct, tuple<>(), NPL::forward_as_tuple([&]{
				return
					CreateEnvironmentWithParent(term.get_allocator(), r_ground);
			}()))->second.second);
			return ReductionStatus::Clean;
		}
		throw NPLException("Requirement '" + to_std_string(req)
			+ "' is already registered.");
	}, cs.WeakenRecord()));
	RegisterUnary<Strict, const string>(renv, "unregister-requirement!",
		trivial_swap, [&](const string& req){
		CheckRequirement(req);
		if(YB_UNLIKELY(registry.erase(req) == 0))
			throw NPLException("Requirement '" + to_std_string(req)
				+ "' is not registered.");
	});
	RegisterUnary<Strict, const string>(renv, "find-requirement-filename",
		// TODO: Blocked. Use C++14 lambda initializers to optimize the
		//	implementation.
		// XXX: Not using binding to prevent overloading ambiguity.
		trivial_swap, [p_specs](const string& req){
		CheckRequirement(req);
		return FindValidRequirementIn(NPL::Deref(p_specs), req);
	});
	RegisterStrict(renv, "require", trivial_swap,
		ystdex::bind1([&](TermNode& term, ContextNode& ctx,
		const EnvironmentReference& r_ground) -> ReductionStatus{
		CheckVariadicArity(term, 0);

		auto i(term.begin());
		const auto& req(NPL::ResolveRegular<const string>(NPL::Deref(++i)));

		CheckRequirement(req);

		auto pr(ystdex::search_map(registry, req));
		const auto reduce_to_res([&](TermNode& tm){
			// XXX: As %ReduceToReference for modifiable lvalues (with default
			//	tags).
			AssertValueTags(tm);
			if(const auto p = TryAccessLeafAtom<const TermReference>(tm))
			{
				// XXX: Since %tm is not shared with %term, it is safe to use
				//	%TermNode::SetContent instead of %TermNode::CopyContent.
				term.SetContent(tm);
				return ReductionStatus::Retained;
			}
			term.SetValue(term.get_allocator(), in_place_type<TermReference>,
				TermTags::Unqualified, tm, ctx.WeakenRecord());
			return ReductionStatus::Clean;
		});

		if(pr.second)
		{
			auto filename(FindValidRequirementIn(specs, req));

			pr.first = ystdex::emplace_hint_in_place(registry, pr.first, req,
				std::piecewise_construct, tuple<>(), NPL::forward_as_tuple([&]{
				return CreateEnvironmentWithParent(term.get_allocator(),
					r_ground);
			}()));

			auto& val(pr.first->second);
			auto& con(term.GetContainerRef());

			// XXX: As 'get-module'.
			switch(con.size())
			{
			case 3:
				val.second->Bind("module-parameters", NPL::AsTermNode(
					ResolveEnvironment(std::move(*con.rbegin())).first));
				con.pop_back();
				YB_ATTR_fallthrough;
			case 2:
				// TODO: Blocked. Use C++14 lambda initializers to optimize
				//	the implementation.
				return A1::RelayCurrentNext(ctx, term, trivial_swap,
					ystdex::bind1([&](TermNode& t, ContextNode& c,
					string& fname, shared_ptr<Environment>& p_env){
					return ReduceToLoadGuarded(t, c, std::move(p_env),
						ystdex::bind1(
						[&](TermNode& t_0, ContextNode& c_0, string& fname_0){
						return
							ReduceToLoadFile(t_0, c_0, std::move(fname_0));
					}, _2, std::move(fname)));
				}, _2, std::move(filename), val.second),
					A1::NameTypedReducerHandler([&, reduce_to_res]{
					MoveCollapsed(val.first, term);
					return reduce_to_res(val.first);
				}, "require-return"));
			}
			ThrowInvalidSyntaxError("Syntax error in require.");
		}
		return reduce_to_res(pr.first->second.first);
	}, _2, cs.WeakenRecord()));
#else
	context.ShareCurrentSource("<lib:std.modules>");
	// XXX: Thread-safety is not respected currently.
	context.Perform(R"NPL(
$provide/let! (registered-requirement? register-requirement!
	unregister-requirement! find-requirement-filename require)
((mods $as-environment (
	$import! std.strings &string-empty? &++ &string->symbol;

	$defl! requirement-error ()
		raise-error "Empty requirement name found.",
	(
	$def! registry () make-environment;
	$defl! bound-name? (&req)
		$and (eval (list bound? req) registry)
			(not? (null? (eval (string->symbol req) registry))),
	$defl%! get-cell% (&req) first& (eval% (string->symbol req) registry),
	$defl! set-value! (&req &v)
		eval (list $def! (string->symbol req) $quote (forward! v)) registry
	),
	$def! prom_pathspecs ($remote-eval% $lazy std.promises)
		$let ((spec ($remote-eval% env-get std.system) "NPLA1_PATH"))
			$if (string-empty? spec) (list "./?" "./?.txt")
				(($remote-eval% string-split std.strings) spec ";"),
	(
	$def! placeholder ($remote-eval% string->regex std.strings) "\?",
	$defl! get-requirement-filename (&specs &req)
		$if (null? specs)
			(raise-error (++ "No module for requirement '" req
				"' found."))
			(
				$let* ((spec first& specs) (path ($remote-eval% regex-replace
					std.strings) spec placeholder req))
					$if (($remote-eval% readable-file? std.io) path) path
						(get-requirement-filename (rest& specs) req)
			)
	)
)))
(
	$defl/e! &registered-requirement? mods (&req)
		$if (string-empty? req) (() requirement-error) (bound-name? req),
	$defl/e! &register-requirement! mods (&req)
		$if (string-empty? req) (() requirement-error)
			($if (bound-name? req) (raise-error
				(++ "Requirement '" req "' is already registered."))
				($let ((env () make-standard-environment))
					$sequence (set-value! req (list () env))
						(weaken-environment (move! env)))),
	$defl/e! &unregister-requirement! mods (&req)
		$if (string-empty? req) (() requirement-error)
			($if (bound-name? req) (set-value! req ()) (raise-error
				(++ "Requirement '" req "' is not registered."))),
	$defl/e! &find-requirement-filename mods (&req)
		get-requirement-filename
			(($remote-eval% force std.promises) prom_pathspecs) req;
	$defl/e%! require mods (&req .&opt)
		$if (registered-requirement? req) (get-cell% (forward! req))
			($let*% ((filename find-requirement-filename req)
				(env register-requirement! req) (&res get-cell% (forward! req)))
				$sequence
					($unless (null? opt)
						($set! env module-parameters $let (((&e .&eopt) opt))
							$if (null? eopt) (check-environemnt e)
								(raise-invalid-syntax-error
									"Syntax error in require.")))
					(assign%! res (eval% (list ($remote-eval% load std.io)
						(move! filename)) (move! env)))
					res);
);
	)NPL");
#endif
}

void
LoadModule_SHBuild(ContextState& cs)
{
	using namespace YSLib;
	auto& renv(cs.GetRecordRef());
	const auto& global(cs.Global.get());

	// NOTE: SHBuild builtins.
	renv.DefineChecked("SHBuild_BaseTerminalHook_",
		ValueObject(function<void(const string&, const string&)>(
		[&](const string& n, const string& val) ynothrow{
		auto& os(global.GetOutputStreamRef());

		IO::StreamPut(os,
			YSLib::sfmt("%s = \"%s\"", n.c_str(), val.c_str()).c_str());
		if(os)
			os << std::endl;
	})));
	RegisterUnary<Strict, const string>(renv, "SHBuild_BuildGCH_existed_",
		[](const string& str) -> bool{
		if(IO::UniqueFile file{uopen(str.c_str(), {}, std::ios_base::in)})
			return file->GetSize() > 0;
		return {};
	});
	RegisterUnary<Strict, const string>(renv, "SHBuild_BuildGCH_mkpdirp_",
		[](const string& str){
		IO::Path pth(str);

		if(!pth.empty())
		{
			pth.pop_back();
			EnsureDirectory(pth);
		}
		return ValueToken::Unspecified;
	});
	RegisterUnary<Strict, const string>(renv, "SHBuild_DirectoryOf_",
		[](const string& str){
		IO::Path pth(str);

		if(!pth.empty())
			pth.pop_back();
		return string(pth);
	});
	RegisterUnary<Strict, const string>(renv, "SHBuild_EnsureDirectory_",
		[](const string& str){
		EnsureDirectory(IO::Path(str));
		return ValueToken::Unspecified;
	});
	RegisterStrict(renv, "SHBuild_EchoVar", trivial_swap,
		[&](TermNode& term){
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
			return ValueToken::Unspecified;
		}, term);
	});
	RegisterBinary<Strict, const string, const string>(renv,
		"SHBuild_Install_HardLink", [](const string& dst, const string& src){
		Deployment::InstallHardLink(dst.c_str(), src.c_str());
		return ValueToken::Unspecified;
	});
	// TODO: Implement by derivations instead?
	RegisterUnary<Strict, const string>(renv, "SHBuild_QuoteS_",
		[](const string& str){
		if(str.find('\'') == string::npos)
			return ystdex::quote(str, '\'');
		throw NPLException("Error in quoted string.");
	});
	RegisterBinary<Strict, const string, const string>(renv,
		"SHBuild_RemovePrefix_",
		[](const string& str, const string& pfx){
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
				res += sv;
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
	RegisterBinary<Strict, const string, const string>(renv,
		"SHBuild_MakeTempFilename",
		[](const string& pfx, const string& sfx){
		// NOTE: POSIX functions are not used for compatibility and for retrying
		//	issues (see below). On the other hand, Win32's %::GetTempPathW is
		//	not reliable across different Windows versions. See
		//	https://reviews.llvm.org/D14231.
		// XXX: This mask length of 6 is same to %::mkstemps used by
		//	%make_temp_file_with_prefix in libiberty, which in turns used by
		//	GCC driver. The mask '%' and the template string below are similar
		//	to the implementation in the LLVM support library used by Clang
		//	driver.
		const string_view tmpl("0123456789abcdef");

#if YF_Hosted
		// NOTE: Value of %TMP_MAX varies. It is also obsolescent in POSIX. See
		//	https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/stdio.h.html.
		// NOTE: Common implementation may try too many times, e.g. glibc's
		//	implementation retry 26^3 times. LLVM tries 128 times. Here it is
		//	more conservative, and the number is documented explicitly.
		for(size_t n(16); n != 0; --n)
#endif
		{
			string str("%%%%%%", pfx.get_allocator());

#if YF_Hosted
			str = YSLib::RandomizeTemplatedString(str, '%', tmpl);
#else
			str = YSLib::RandomizeTemplatedString(std::move(str), '%',
				tmpl);
#endif

			str = pfx + std::move(str) + sfx;
			// TODO: Check file access with read-write permissions using
			//	%YSLib::uopen. This requires the exposure of the open flags in
			//	YSLib since NPL does not use %YCLib::NativeAPI.
			// XXX: This is like %YSLib::ufexists, but the UTF-8 support is not
			//	needed.
			if(ystdex::fexists(str.c_str(), true))
				return str;
#if YF_Hosted

			const int err(errno);

			// NOTE: On Windows %EPERM can occur on trying to open a file marked
			//	for deletion.
			if(err != EEXIST && err != EPERM)
				continue;
#endif
		}

		// NOTE: This is nearly fatal. It is not intended to be handled in
		//	the object language in general. Thus, currently it is not
		//	wrapped in %NPLException.
		int err(errno);

		ystdex::throw_error(err, "Failed opening temporary file with the"
			" prefix '" + to_std_string(pfx) + '\'');
	});
}

void
LoadStandardContext(ContextState& cs)
{
	LoadGroundContext(cs);
	// XXX: A lambda-expression is OK and the generated code can be equally
	//	efficient, but not succinct as using macro in the invocation sites. 
#define NPL_Impl_LoadStd(_name) \
	LoadModuleChecked(cs, "std." #_name, LoadModule_std_##_name, cs)
	NPL_Impl_LoadStd(continuations),
	NPL_Impl_LoadStd(promises);
	NPL_Impl_LoadStd(math),
	NPL_Impl_LoadStd(strings);
	NPL_Impl_LoadStd(io);
	NPL_Impl_LoadStd(system);
	NPL_Impl_LoadStd(modules);
#undef NPL_Impl_LoadStd
}

} // namespace Forms;

} // namespace A1;

} // namespace NPL;

