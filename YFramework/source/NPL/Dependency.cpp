/*
	© 2015-2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Dependency.cpp
\ingroup NPL
\brief 依赖管理。
\version r4817
\author FrankHB <frankhb1989@gmail.com>
\since build 623
\par 创建时间:
	2015-08-09 22:14:45 +0800
\par 修改时间:
	2021-05-06 19:39 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::Dependency
*/


#include "NPL/YModules.h"
#include YFM_NPL_Dependency // for set, string, UnescapeContext, string_view,
//	ystdex::isspace, std::istream, YSLib::unique_ptr, YSLib::share_move,
//	A1::NameTypedReducerHandler, std::ref, RelaySwitched, NPL::Deref,
//	NPL::ResolveRegular, A1::ReduceOnce, ResolvedTermReferencePtr,
//	Forms::CallResolvedUnary, NPL::AllocateEnvironment, function, ValueObject,
//	EnvironmentReference, std::piecewise_construct, NPL::forward_as_tuple,
//	LiftOther, ThrowNonmodifiableErrorForAssignee, ThrowValueCategoryError,
//	ValueToken, IsNPLASymbol, ThrowInvalidTokenError, ResolveTerm, TokenValue,
//	IsEmpty, ComposeReferencedTermOp, IsBranch, IsReferenceTerm,
//	IsBoundLValueTerm, IsUncollapsedTerm, IsUniqueTerm, IsModifiableTerm,
//	IsTemporaryTerm, NPL::TryAccessLeaf, LiftTermRef, NPL::SetContentWith,
//	Forms::CallRawUnary, LiftTerm, LiftOtherOrCopy, ystdex::bind1,
//	std::placeholders, LiftTermValueOrCopy, MoveResolved, ResolveIdentifier,
//	ReduceToReferenceList, NPL::IsMovable, LiftTermOrCopy, IsBranchedList,
//	AccessFirstSubterm, ReferenceTerm, ThrowInsufficientTermsError,
//	ystdex::exchange, ystdex::plus, ystdex::tolower, YSLib::IO::StreamPut,
//	FetchEnvironmentVariable, ystdex::swap_dependent, YSLib::IO::UniqueFile,
//	YSLib::IO::omode_convb, YSLib::uremove, ystdex::throw_error;
#include YFM_NPL_NPLA1Forms // for NPL::Forms functions;
#include YFM_YSLib_Service_FileSystem // for YSLib::IO::Path;
#include <ystdex/iterator.hpp> // for std::istreambuf_iterator,
//	ystdex::make_transform;
#include YFM_YSLib_Service_TextFile // for
//	YSLib::IO::SharedInputMappedFileStream, YSLib::Text;
#include <cerrno> // for errno, ERANGE;
#include <regex> // for std::regex, std::regex_match;
#include <ostream> // for std::endl;
#include "NPLA1Internals.h" // for NPL_Impl_NPLA1_Enable_Thunked;
#include YFM_YSLib_Core_YCoreUtilities // for FetchCommandOutput,
//	YSLib::RandomizeTemplatedString, to_std_string;
#include <ystdex/string.hpp> // for ystdex::begins_with;
#include <ystdex/cstdio.h> // for ystdex::fexists;
#include <cerrno> // for errno, EEXIST, EPERM;

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

YSLib::unique_ptr<std::istream>
OpenFile(const char* filename)
{
	return YSLib::Text::OpenSkippedBOMtream<
		YSLib::IO::SharedInputMappedFileStream>(YSLib::Text::BOM_UTF_8,
		filename);
}

YSLib::unique_ptr<std::istream>
OpenUnique(REPLContext& context, string filename)
{
	auto p_is(A1::OpenFile(filename.c_str()));

	// NOTE: Swap guard for %Context.CurrentSource is not used to support PTC.
	context.CurrentSource = YSLib::share_move(filename);
	return p_is;
}


void
PreloadExternal(REPLContext& context, const char* filename)
{
	TryLoadSource(context, filename,
		*OpenUnique(context, string(filename, context.Allocator)));
}

ReductionStatus
ReduceToLoadExternal(TermNode& term, ContextNode& ctx, REPLContext& context)
{
	Forms::RetainN(term);
	term = context.Load(context, ctx,
		NPL::ResolveRegular<string>(NPL::Deref(std::next(term.begin()))));
	// NOTE: This is explicitly not same to klisp. This is also friendly to PTC.
	// XXX: Same to %A1::ReduceOnce, without setup the next term.
	return ContextState::Access(ctx).ReduceOnce.Handler(term, ctx);
}

ReductionStatus
RelayToLoadExternal(ContextNode& ctx, TermNode& term, REPLContext& context)
{
	return RelaySwitched(ctx,
		A1::NameTypedReducerHandler(std::bind(ReduceToLoadExternal,
		std::ref(term), std::ref(ctx), std::ref(context)), "load-external"));
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
		[&](TermNode& nd, ResolvedTermReferencePtr p_ref){
		// NOTE: Force move. No %IsMovable check is needed.
		// XXX: Term tags are currently not respected in prvalues.
		f(term, nd, !p_ref || p_ref->IsModifiable());
		// NOTE: Term tags are not copied.
		return ReductionStatus::Retained;
	}, term);
}

//! \since build 914
YB_ATTR_nodiscard ReductionStatus
Qualify(TermNode& term, TermTags tag_add)
{
	return Forms::CallRawUnary([&](TermNode& tm){
		if(const auto p = NPL::TryAccessLeaf<TermReference>(tm))
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

//! \since build 917
template<typename _func>
auto
CheckSymbol(string_view id, _func f) -> decltype(f())
{
	if(IsNPLASymbol(id))
		return f();
	ThrowInvalidTokenError(id);
}

//! \since build 897
YB_ATTR_nodiscard YB_FLATTEN ReductionStatus
DoResolve(TermNode(&f)(const ContextNode&, string_view), TermNode& term,
	const ContextNode& c)
{
	Forms::CallRegularUnaryAs<const TokenValue>(
		[&] YB_LAMBDA_ANNOTATE((string_view id), , flatten){
		term = CheckSymbol(id, std::bind(f, std::ref(c), id));
	}, term);
	return ReductionStatus::Retained;
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
	RegisterUnary<>(ctx, "null?", ComposeReferencedTermOp(IsEmpty));
	RegisterUnary<>(ctx, "nullv?", IsEmpty);
	RegisterUnary<>(ctx, "branch?", ComposeReferencedTermOp(IsBranch));
	RegisterUnary<>(ctx, "branchv?", IsBranch);
	RegisterUnary<>(ctx, "reference?", IsReferenceTerm);
	RegisterUnary<>(ctx, "bound-lvalue?", IsBoundLValueTerm);
	RegisterUnary<>(ctx, "uncollapsed?", IsUncollapsedTerm);
	RegisterUnary<>(ctx, "unique?", IsUniqueTerm);
	RegisterUnary<>(ctx, "modifiable?", IsModifiableTerm);
	RegisterUnary<>(ctx, "temporary?", IsTemporaryTerm);
	RegisterStrict(ctx, "deshare", [](TermNode& term){
		return CallRawUnary([&](TermNode& tm){
			if(const auto p = NPL::TryAccessLeaf<const TermReference>(tm))
				LiftTermRef(tm, p->get());
			NPL::SetContentWith(term, std::move(tm),
				&ValueObject::MakeMoveCopy);
			return ReductionStatus::Retained;
		}, term);
	});
	RegisterStrict(ctx, "as-const",
		ystdex::bind1(Qualify, TermTags::Nonmodifying));
	RegisterStrict(ctx, "expire",
		ystdex::bind1(Qualify, TermTags::Unique));
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
	RegisterForm(ctx, "$resolve-identifier",
		std::bind(DoResolve, std::ref(ResolveIdentifier), _1, _2));
	RegisterForm(ctx, "$move-resolved!",
		std::bind(DoResolve, std::ref(MoveResolved), _1, _2));
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
		[] YB_LAMBDA_ANNOTATE((const string& str), , noreturn){
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
	LoadSymbols(ctx);
	LoadEnvironments(ctx);
	LoadCombiners(ctx);
	LoadErrorsAndChecks(ctx);
	LoadEncapsulations(ctx);
}

} // namespace Primitive;

namespace Derived
{

//! \since build 909
void
LoadBasicDerived(REPLContext& context)
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
			term.Value = CheckSymbol(id, [&]() -> bool{
				auto pr(ResolveName(ctx, id));

				if(pr.first)
				{
					auto& tm(*pr.first);

					if(const auto p
						= NPL::TryAccessLeaf<const TermReference>(tm))
						return p->IsReferencedLValue();
					return !(bool(tm.Tags & TermTags::Unique)
						|| bool(tm.Tags & TermTags::Temporary));
				}
				throw BadIdentifier(id);
			});
		}, term);
	});
	RegisterStrict(renv, "forward!", [](TermNode& term){
		return Forms::CallRawUnary([&](TermNode& tm){
			MoveRValueToForward(term, tm);
			return ReductionStatus::Retained;
		}, term);
	});
	RegisterStrict(renv, "list%", ReduceBranchToList);
	RegisterStrict(renv, "rlist", [](TermNode& term, ContextNode& ctx){
		return Forms::CallRawUnary([&](TermNode& tm){
			return ReduceToReferenceList(term, ctx, tm);
		}, term);
	});
	// NOTE: Lazy form '$deflazy!' is the basic operation, which may bind
	//	parameter as unevaluated operands.
	RegisterForm(renv, "$deflazy!", DefineLazy);
	RegisterForm(renv, "$set!", SetWithNoRecursion);
	RegisterForm(renv, "$setrec!", SetWithRecursion);
	RegisterForm(renv, "$lambda", Lambda);
	RegisterForm(renv, "$lambda%", LambdaRef);
	RegisterForm(renv, "$lambda/e", LambdaWithEnvironment);
	RegisterForm(renv, "$lambda/e%", LambdaWithEnvironmentRef);
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
	RegisterBinary<>(renv, "assign%!", [](TermNode& x, TermNode& y){
		return DoAssign(ystdex::bind1(MoveCollapsed, std::ref(y)), x);
	});
	RegisterBinary<>(renv, "assign!", [](TermNode& x, TermNode& y){
		return DoAssign([&](TermNode& nd_x){
			ResolveTerm([&](TermNode& nd_y, ResolvedTermReferencePtr p_ref_y){
				// NOTE: Self-assignment is not checked directly. This allows
				//	copy assignment to fail as expected. Anyway, the destination
				//	is not modified if the source copy fails.
				LiftTermOrCopy(nd_x, nd_y, NPL::IsMovable(p_ref_y));
			}, y);
		}, x);
	});
	RegisterStrict(renv, "apply", Apply);
	RegisterStrict(renv, "list*", ListAsterisk);
	RegisterStrict(renv, "list*%", ListAsteriskRef);
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
	RegisterUnary<>(renv, "not?", Not);
	RegisterForm(renv, "$and?", And);
	RegisterForm(renv, "$or?", Or);
	RegisterStrict(renv, "accl", AccL);
	RegisterStrict(renv, "accr", AccR);
	RegisterStrict(renv, "foldr1", FoldR1);
	RegisterStrict(renv, "map1", Map1);
	RegisterUnary<>(renv, "first-null?", [](TermNode& x){
		return ResolveTerm(
			[&](TermNode& nd, ResolvedTermReferencePtr p_ref){
			if(IsBranchedList(nd))
				return IsEmpty(ReferenceTerm(AccessFirstSubterm(nd)));
			ThrowInsufficientTermsError(nd, p_ref);
		}, x);
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
	RegisterForm(renv, "$let", Let);
	RegisterForm(renv, "$let%", LetRef);
	RegisterForm(renv, "$let/e", LetWithEnvironment);
	RegisterForm(renv, "$let/e%", LetWithEnvironmentRef);
	RegisterForm(renv, "$let*", LetAsterisk);
	RegisterForm(renv, "$let*%", LetAsteriskRef);
	RegisterForm(renv, "$letrec", LetRec);
	RegisterForm(renv, "$letrec%", LetRecRef);
	RegisterStrict(renv, "make-standard-environment",
		// NOTE: The weak reference of the ground environment is saved and it
		//	shall not be moved after being called.
		// TODO: Blocked. Use C++14 lambda initializers to simplify the
		//	implementation.
		ystdex::bind1([] YB_LAMBDA_ANNOTATE(
		(TermNode& term, const EnvironmentReference& ce), , flatten){
		RetainN(term, 0);

		// XXX: Simlar to %MakeEnvironment.
		ValueObject parent;

		parent.emplace<EnvironmentReference>(ce);
		term.Value
			= NPL::AllocateEnvironment(term.get_allocator(), std::move(parent));
	}, context.Root.WeakenRecord()));
	RegisterStrict(renv, "derive-current-environment",
		[] YB_LAMBDA_ANNOTATE((TermNode& term, ContextNode& ctx), , flatten){
		Retain(term);
		term.emplace(NPL::AsTermNode(term.get_allocator(), ctx.WeakenRecord()));
		return MakeEnvironment(term);
	});
	RegisterStrict(renv, "derive-environment",
		// NOTE: As 'make-standard-environment'.
		// TODO: Blocked. Use C++14 lambda initializers to simplify the
		//	implementation.
		ystdex::bind1([] YB_LAMBDA_ANNOTATE(
		(TermNode& term, const EnvironmentReference& ce), , flatten){
		Retain(term);
		term.emplace(NPL::AsTermNode(term.get_allocator(), ce));
		return MakeEnvironment(term);
	}, context.Root.WeakenRecord()));
	RegisterForm(renv, "$as-environment", AsEnvironment);
	RegisterForm(renv, "$bindings/p->environment",
		BindingsWithParentToEnvironment);
	RegisterForm(renv, "$bindings->environment", BindingsToEnvironment);
	RegisterStrict(renv, "symbols->imports", SymbolsToImports);
#else
	context.ShareCurrentSource("<root:basic-derived>");
	context.Perform(
#	if NPL_Impl_NPLA1_Native_EnvironmentPrimitives
	R"NPL(
		$def! $vau $vau/e (() get-current-environment) (&formals &ef .&body) d
			eval (cons $vau/e (cons d (cons formals (cons ef (move! body))))) d;
		$def! $vau% $vau (&formals &ef .&body) d
			eval (cons $vau/e% (cons d (cons formals (cons ef (move! body)))))
				d;
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
	context.ShareCurrentSource("<root:basic-derived-1>");
	context.Perform(R"NPL(
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
	R"NPL(
		$def! forward! wrap
			($vau% (%x) #ignore $if ($lvalue-identifier? x) x (move! x));
		$def! list% wrap ($vau &x #ignore forward! x);
		$def! rlist wrap ($vau ((.&x)) #ignore move! x);
	)NPL"
#	else
	R"NPL(
		$def! forward! $lambda% (%x) $if ($lvalue-identifier? x) x (move! x);
		$def! list% $lambda &x forward! x;
		$def! rlist $lambda ((.&x)) move! x;
	)NPL"
#	endif
	// XXX: The operative '$defv!' is same to following derivations in
	//	%LoadCore.
	R"NPL(
		$def! $deflazy! $vau (&definiend .&body) d
			eval (list $def! definiend $quote body) d;
		$def! $set! $vau (&e &formals .&body) d
			eval (list $def! formals (unwrap eval%) (move! body) d) (eval e d);
		$def! $defv! $vau (&$f &formals &ef .&body) d
			eval (list $set! d $f $vau formals ef (move! body)) d;
		$defv! $setrec! (&e &formals .&body) d
			eval (list $defrec! formals (unwrap eval%) (move! body) d)
				(eval e d);
	)NPL"
#	if NPL_Impl_NPLA1_Use_Id_Vau
	R"NPL(
		$defv! $lambda (&formals .&body) d
			wrap (eval (cons $vau (cons formals (cons ignore (move! body)))) d);
		$defv! $lambda% (&formals .&body) d
			wrap
				(eval (cons $vau% (cons formals (cons ignore (move! body)))) d);
	)NPL"
#	endif
	// XXX: The operatives '$defl!', '$defl%!', '$defw%!', '$defv%!',
	//	'$defw!', and '$defv/e%!' are same to following derivations in
	//	%LoadCore.
	// NOTE: Use of 'eqv?' is more efficient than '$if'.
	R"NPL(
		$defv! $lambda/e (&e &formals .&body) d
			wrap (eval (list* $vau/e e formals ignore (move! body)) d);
		$defv! $lambda/e% (&e &formals .&body) d
			wrap (eval (list* $vau/e% e formals ignore (move! body)) d);
		$def! $sequence
			($lambda (&se)
				($lambda #ignore $vau/e% se &exprseq d
					$if (null? exprseq) #inert
						(eval% (cons% $aux (move! exprseq)) d))
				($set! se $aux
					$vau/e% (weaken-environment se) (&head .&tail) d
						$if (null? tail) (eval% (forward! head) d)
							(($vau% (&t) e ($lambda% #ignore eval% t e)
								(eval% (forward! head) d))
								(eval% (cons% $aux (move! tail)) d))))
			(make-environment (() get-current-environment));
		$defv! $defl! (&f &formals .&body) d
			eval (list $set! d f $lambda formals (move! body)) d;
		$defv! $defl%! (&f &formals .&body) d
			eval (list $set! d f $lambda% formals (move! body)) d;
		$defl%! collapse (%x)
			$if (uncollapsed? ($resolve-identifier x)) (idv x) x;
		$defl%! forward (%x) $if ($lvalue-identifier? x) x (idv x);
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
			$if (null? tail) (forward! head)
				(cons (forward! head) (apply list* (move! tail)));
		$defl! list*% (&head .&tail)
			$if (null? tail) (forward! head)
				(cons% (forward! head) (apply list*% (move! tail)));
		$defv! $defw%! (&f &formals &ef .&body) d
			eval (list $set! d f wrap (list* $vau% formals ef (move! body))) d;
		$defw%! forward-first% (&appv (&x .)) d
			apply (forward! appv) (list% ($move-resolved! x)) d;
		$defl%! first (%l)
			($lambda% (fwd) forward-first% forward! (fwd l))
				($if ($lvalue-identifier? l) id expire);
		$defl%! first@ (&l)
			($lambda% ((@x .)) x) (check-list-reference (forward! l));
		$defl%! first% (%l)
			($lambda (fwd (@x .)) fwd x)
				($if ($lvalue-identifier? l) id expire) l;
		$defl%! first& (&l)
			($lambda% ((&x .)) x) (check-list-reference (forward! l));
		$defl! firstv ((&x .)) x;
		$defl! rest% ((#ignore .%xs)) move! xs;
		$defl%! rest& (&l)
			($lambda% ((#ignore .&xs)) xs) (check-list-reference (forward! l));
		$defl! restv ((#ignore .xs)) move! xs;
		$defl! set-first! (&l x) assign@! (first@ (forward! l)) (move! x);
		$defl! set-first@! (&l &x) assign@! (first@ (forward! l)) (forward! x);
		$defl! set-first%! (&l &x) assign%! (first@ (forward! l)) (forward! x);
		$defl! equal? (&x &y)
			$if ($if (branch? x) (branch? y) #f)
				($if (equal? (first& x) (first& y))
					(equal? (rest& x) (rest& y)) #f)
					(eqv? x y);
		$defl%! check-environment (&e)
			$sequence (eval% #ignore e) (forward! e);
		$defl%! check-parent (&e)
			$sequence ($vau/e% e . #ignore) (forward! e);
		$defv! $defv%! (&$f &formals &ef .&body) d
			eval (list $set! d $f $vau% formals ef (move! body)) d;
		$defv%! $cond &clauses d
			$if (null? clauses) #inert
				(apply ($lambda% ((&test .&body) .&clauses)
					$if (eval test d) (eval% (move! body) d)
						(apply (wrap $cond) (move! clauses) d))
						(move! clauses));
		$defv%! $when (&test .&exprseq) d
			$if (eval test d) (eval% (list* () $sequence (move! exprseq)) d);
		$defv%! $unless (&test .&exprseq) d
			$if (eval test d) #inert
				(eval% (list* () $sequence (move! exprseq)) d);
		$defl! not? (&x) eqv? x #f;
		$defv%! $and? &x d
			$cond
				((null? x) #t)
				((null? (rest& x)) eval% (first (forward! x)) d)
				((eval% (first& x) d) apply (wrap $and?) (rest% (forward! x)) d)
				(#t #f);
		$defv%! $or? &x d
			$cond
				((null? x) #f)
				((null? (rest& x)) eval% (first (forward! x)) d)
				(#t ($lambda% (&r) $if r (forward! r) (apply (wrap $or?)
					(rest% (forward! x)) d)) (eval% (move! (first& x)) d));
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
			apply accr (list% (($lambda ((.@xs)) xs) l) null? (forward! knil)
				($if ($lvalue-identifier? l) ($lambda (&l) first% l)
				($lambda (&l) expire (first% l))) rest% kons) d;
		$defw%! map1 (&appv &l) d
			foldr1 ($lambda (%x &xs) cons%
				(apply appv (list% ($move-resolved! x)) d) (move! xs)) ()
				(forward! l);
		$defl! first-null? (&l) null? (first l);
		$defl%! rulist (&l)
			$if ($lvalue-identifier? l)
				(accr (($lambda ((.@xs)) xs) l) null? ()
					($lambda% (%l) $sequence ($def! %x idv (first@ l))
						(($if (uncollapsed? x) idv expire) (expire x))) rest%
					($lambda (%x &xs)
						(cons% ($resolve-identifier x) (move! xs))))
				(idv (forward! l));
		$defl! list-concat (&x &y) foldr1 cons% (forward! y) (forward! x);
		$defl! append (.&ls) foldr1 list-concat () (move! ls);
		$defl%! list-extract-first (&l) map1 first l;
		$defl%! list-extract-rest% (&l) map1 rest% l;
		$defv! $defv/e%! (&$f &e &formals &ef .&body) d
			eval (list $set! d $f $vau/e% e formals ef (move! body)) d;
		$def! ($let $let% $let/e $let/e% $let* $let*% $letrec $letrec%)
			($lambda (&ce)
		(
			$def! mods () ($lambda/e ce ()
			(
				$defv%! $lqual (&ls) d
					($if (eval (list $lvalue-identifier? ls) d) as-const rulist)
						(eval% ls d);
				$defv%! $lqual* (&x) d
					($if (eval (list $lvalue-identifier? x) d) as-const expire)
						(eval% x d);
				$defl%! mk-let ($ctor &bindings &body)
					list* () (list* $ctor (list-extract-first bindings)
						(list (move! body))) (list-extract-rest% bindings);
				$defl%! mk-let/e ($ctor &e &bindings &body)
					list* () (list* $ctor e (list-extract-first bindings)
						(list (move! body))) (list-extract-rest% bindings);
				$defl%! mk-let* ($let $let* &bindings &body)
					$if (null? bindings) (list* $let () (move! body))
						(list $let (list (first% ($lqual* bindings)))
						(list* $let* (rest% ($lqual* bindings)) (move! body)));
				$defl%! mk-letrec ($let &bindings &body)
					list $let () $sequence (list $def! (list-extract-first
						bindings) (list* () list (list-extract-rest% bindings)))
						(move! body);
				() lock-current-environment
			));
			$defv/e%! $let mods (&bindings .&body) d
				eval% (mk-let $lambda ($lqual bindings) (move! body)) d;
			$defv/e%! $let% mods (&bindings .&body) d
				eval% (mk-let $lambda% ($lqual bindings) (move! body)) d;
			$defv/e%! $let/e mods (&e &bindings .&body) d
				eval% (mk-let/e $lambda/e e ($lqual bindings) (move! body)) d;
			$defv/e%! $let/e% mods (&e &bindings .&body) d
				eval% (mk-let/e $lambda/e% e ($lqual bindings) (move! body)) d;
			$defv/e%! $let* mods (&bindings .&body) d
				eval% (mk-let* $let $let* ($lqual* bindings) (move! body)) d;
			$defv/e%! $let*% mods (&bindings .&body) d
				eval% (mk-let* $let% $let*% ($lqual* bindings) (move! body)) d;
			$defv/e%! $letrec mods (&bindings .&body) d
				eval% (mk-letrec $let ($lqual bindings) (move! body)) d;
			$defv/e%! $letrec% mods (&bindings .&body) d
				eval% (mk-letrec $let% ($lqual bindings) (move! body)) d;
			map1 move!
				(list% $let $let% $let/e $let/e% $let* $let*% $letrec $letrec%)
		)) (() get-current-environment);
		$defv! $defw! (&f &formals &ef .&body) d
			eval (list $set! d f wrap (list* $vau formals ef (move! body))) d;
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
				() ($lambda/e (append envs (list d)) ()
					() lock-current-environment));
	)NPL"
#		else
	// XXX: This is also correct, but less efficient.
	R"NPL(
		$def! derive-environment ()
			($vau () d eval (list $lambda/e (() lock-current-environment)
				((unwrap list) .&envs) () (list $lambda/e ((unwrap list) append
					envs (list d)) () () lock-current-environment)) d);
	)NPL"
#		endif
#	else
	// XXX: Ground environment is passed by 'ce'.
	R"NPL(
		$def! make-standard-environment
			($lambda (&se &e)
				($lambda #ignore $lambda/e se () make-environment ce)
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
			eval (list $let () (list $sequence (move! body)
				(list () lock-current-environment))) d;
		$defv! $bindings/p->environment (&parents .&bindings) d $sequence
			($def! (res bref) list (apply make-environment
				(map1 ($lambda% (x) eval% x d) parents)) (rulist bindings))
			(eval% (list $set! res (list-extract-first bref)
				(list* () list (list-extract-rest% bref))) d)
			res;
		$defv! $bindings->environment (.&bindings) d
			eval (list* $bindings/p->environment () (move! bindings)) d;
		$defl! symbols->imports (&symbols)
			list* () list% (map1 ($lambda (&s) list forward! (desigil s))
				(forward! symbols));
	)NPL"
	);
#endif
}

//! \since build 909
void
LoadStandardDerived(REPLContext& context)
{
#if NPL_Impl_NPLA1_Native_Forms
	auto& renv(context.Root.GetRecordRef());

	RegisterUnary<Strict, const TokenValue>(renv, "ensigil",
		[](TokenValue s) -> TokenValue{
		if(!s.empty() && s.front() != '&')
		{
			if(s.front() != '%')
				return '&' + s;
			s.front() = '&';
		}
		return s;
	});
#else
	// XXX: %ensigil depends on %std.strings but not some core functions, to
	//	avoid cyclic dependencies.
	context.ShareCurrentSource("<root:standard-derived>");
	context.Perform(R"NPL(
		$def! ensigil $lambda (&s)
			$let/e (derive-current-environment std.strings) ()
				$let ((&str symbol->string s))
					$if (string-empty? str) s
						(string->symbol (++ "&" (symbol->string (desigil s))));
	)NPL");
#endif
}

//! \since build 839
//@{
void
LoadCore(REPLContext& context)
{
	// XXX: Call multiple %context.Perform in different places. Different to
	//	%LoadBasicDerived, this way is a slightly more efficient whether
	//	%NPL_Impl_NPLA1_Native_Forms is set.
	context.ShareCurrentSource("<root:core>");
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
		$defv! $defl/e! (&f &e &formals .&body) d
			eval (list $set! d f $lambda/e e formals (move! body)) d;
		$defv! $defl/e%! (&f &e &formals .&body) d
			eval (list $set! d f $lambda/e% e formals (move! body)) d;
		$def! (box% box? unbox) () make-encapsulation-type;
		$defl! box (&x) box% x;
		$defl%! assv (&x &alist) $cond ((null? alist) ())
			((eqv? x (first& (first& alist))) first alist)
			(#t assv (forward! x) (rest% alist));
		$defv! $provide/let! (&symbols &bindings .&body) d
			eval% (list% $let (forward! bindings) $sequence
				(move! body) (list% $set! d symbols (symbols->imports symbols))
				(list () lock-current-environment)) d;
		$defv! $provide! (&symbols .&body) d
			eval (list*% $provide/let! (forward! symbols) () (move! body)) d;
		$defv! $import! (&e .&symbols) d
			eval% (list $set! d symbols (symbols->imports symbols)) (eval e d);
		$defv! $import&! (&e .&symbols) d
			eval% (list $set! d (map1 ensigil symbols)
				(symbols->imports symbols)) (eval e d);
		$defl! nonfoldable? (&l)
			$if (null? l) #f ($if (first-null? l) #t (nonfoldable? (rest& l)));
		$defl! list-push-front! (&l &x)
			assign! l (cons% (forward! x) (move! l));
		$defw%! map-reverse (&appv .&ls) d
			accl (move! ls) nonfoldable? () list-extract-first
				list-extract-rest%
				($lambda (&x &xs) cons% (apply appv (forward! x) d) xs);
		$defw! for-each-ltr &ls d $sequence (apply map-reverse ls d) #inert;
	)NPL");
}

void
Load(REPLContext& context)
{
	LoadBasicDerived(context);
	LoadStandardDerived(context);
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

	RegisterUnary<Strict, const string>(renv, "bound?",
		[](const string& id, ContextNode& ctx){
		return CheckSymbol(id, [&]{
			return bool(ResolveName(ctx, id).first);
		});
	});
	context.ShareCurrentSource("<lib:std.environments>");
	context.Perform(R"NPL(
		$defv/e! $binds1? (derive-current-environment std.strings) (&e &s) d
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
	context.ShareCurrentSource("<lib:std.promises>");
	context.Perform(R"NPL(
		$provide/let! (promise? memoize $lazy $lazy/e force)
		((mods $as-environment (
			$def! (encapsulate% promise? decapsulate)
				() make-encapsulation-type;
			$defl%! force-promise (&x) $let ((((&o &env)) x))
				$if (null? env) (forward! o)
				(
					$let% ((&y eval% o env))
						$cond
							((null? (first (rest& (first& x))))
								first& (first& x))
							((promise? y) $sequence
								(set-first%! x
									(first (decapsulate (forward! y))))
								(force-promise x))
							(#t $sequence
								($let (((&o &e) first& x))
									list% (assign! o y) (assign@! e ()))
								(forward! y))
				)
		)))
		(
			$import! mods &promise?;

			$defl/e%! &memoize mods (&x)
				encapsulate% (list (list% (forward! x) ())),
			$defv/e%! &$lazy mods (.&body) d
				encapsulate% (list (list (move! body) d)),
			$defv/e%! &$lazy/e mods (&e .&body) d
				encapsulate%
					(list (list (move! body) (check-parent (eval e d)))),
			$defl/e%! &force mods (&x)
				$if (promise? x) (force-promise (decapsulate x)) (forward! x)
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
	RegisterUnary<>(renv, "string->symbol", [](TermNode& x){
		return ResolveTerm([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
			auto& s(NPL::AccessRegular<string>(nd, p_ref));

			return NPL::IsMovable(p_ref) ? StringToSymbol(std::move(s))
				: StringToSymbol(s);
		}, x);
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

	RegisterUnary<Strict, const string>(renv, "puts", [&](const string& str){
		auto& os(context.GetOutputStreamRef());

		YSLib::IO::StreamPut(os, str.c_str());
		if(os)
			os << std::endl;
		return ValueToken::Unspecified;
	});
#if true
	RegisterStrict(renv, "load", [&](TermNode& term, ContextNode& ctx){
		return ReduceToLoadExternal(term, ctx, context);
	});
#else
	RegisterUnary<Strict, string>(renv, "load", [&](string& filename){
		// NOTE: This is not applicable since %load has no additional barrier
		//	here.
		// XXX: As %PreloadExternal.
		const auto p_is(OpenUnique(context, std::move(filename)));

		TryLoadSource(context, context.CurrentSource->c_str(), *p_is);
		return ValueToken::Unspecified;
	});
#endif
}

void
LoadModule_std_system(REPLContext& context)
{
	auto& renv(context.Root.GetRecordRef());

	RegisterStrict(renv, "cmd-get-args", [](TermNode& term){
		RetainN(term, 0);
		{
			const auto p_cmd_args(LockCommandArguments());
			TermNode::Container con{};

			for(const auto& arg : p_cmd_args->Arguments)
				TermNode::AddValueTo(con, in_place_type<string>, arg);
			con.swap(term.GetContainerRef());
		}
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
	context.ShareCurrentSource("<lib:std.system>");
	context.Perform(R"NPL(
		$defl/e! env-empty? (derive-current-environment std.strings) (&n)
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
	RegisterUnary<Strict, const string>(renv, "remove-file",
		[](const string& filename){
		return YSLib::uremove(filename.c_str());
	});
}

void
LoadModule_SHBuild(REPLContext& context)
{
	using namespace YSLib;
	auto& renv(context.Root.GetRecordRef());

	// NOTE: SHBuild builtins.
	renv.DefineChecked("SHBuild_BaseTerminalHook_",
		ValueObject(function<void(const string&, const string&)>(
		[&](const string& n, const string& val) ynothrow{
		auto& os(context.GetOutputStreamRef());

		IO::StreamPut(os,
			YSLib::sfmt("%s = \"%s\"", n.c_str(), val.c_str()).c_str());
		if(os)
			os << std::endl;
	})));
	RegisterUnary<Strict, const string>(renv, "SHBuild_BuildGCH_existed_",
		[](const string& str) -> bool{
		if(IO::UniqueFile
			file{uopen(str.c_str(), IO::omode_convb(std::ios_base::in))})
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
			return ValueToken::Unspecified;
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
		[] YB_LAMBDA_ANNOTATE((const string& str), , noreturn){
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
		// NOTE: Value of %TMP_MAX varies. It is also obsolescent in POSIX. See https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/stdio.h.html.
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
			// XXX: This is like %YSLib::ufexists, but UTF-8 support is not
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
		ystdex::throw_error(errno, "Failed opening temporary file with the"
			" prefix '" + to_std_string(pfx) + '\'');
	});
}

void
LoadStandardContext(REPLContext& context)
{
	LoadGroundContext(context);

	auto& rctx(context.Root);
	const auto load_std_module([&](string_view module_name,
		void(&load_module)(REPLContext&)){
		LoadModuleChecked(rctx, "std." + string(module_name),
			std::bind(load_module, std::ref(context)));
	});

	load_std_module("strings", LoadModule_std_strings);
	load_std_module("environments", LoadModule_std_environments),
	load_std_module("io", LoadModule_std_io),
	load_std_module("system", LoadModule_std_system),
	load_std_module("promises", LoadModule_std_promises);
}

} // namespace Forms;

} // namespace A1;

} // namespace NPL;

