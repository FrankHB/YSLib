/*
	© 2014-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA.cpp
\ingroup NPL
\brief NPLA 公共接口。
\version r4188
\author FrankHB <frankhb1989@gmail.com>
\since build 663
\par 创建时间:
	2016-01-07 10:32:45 +0800
\par 修改时间:
	2022-07-06 20:50 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA
*/


#include "NPL/YModules.h"
#include YFM_NPL_NPLA // for YSLib::Warning, YSLib::Err, YSLib::RecordLevel,
//	string, YSLib::make_string_view, std::to_string, YSLib::DecodeIndex,
//	std::invalid_argument, ValueNode, Access, EscapeLiteral, Literalize,
//	AccessPtr, ystdex::value_or, ystdex::write, std::bind, TraverseSubnodes,
//	bad_any_cast, std::allocator_arg, YSLib::NodeSequence, ystdex::begins_with,
//	ystdex::sfmt, observer_ptr, ystdex::make_obj_using_allocator, trivial_swap,
//	CountPrefix, make_observer, TermTags, TryAccessLeafAtom, NPL::Deref,
//	YSLib::sfmt, AssertReferentTags, ystdex::call_value_or, ystdex::compose,
//	GetLValueTagsOf, std::mem_fn, IsTyped, ystdex::invoke_value_or, ystdex::ref,
//	PropagateTo, IsSticky, FindSticky, std::distance, ystdex::as_const,
//	TryAccessLeaf, AccessFirstSubterm, YSLib::FilterExceptions, type_id,
//	ystdex::addrof, ystdex::second_of, type_info, std::current_exception,
//	std::rethrow_exception, std::throw_with_nested, ystdex::retry_on_cond,
//	ystdex::id, pair, NPL::IsMovable, YSLib::ExtractException;
#include <ystdex/function.hpp> // for ystdex::unchecked_function;

//! \since build 903
//@{
using YSLib::Warning;
using YSLib::Err;
using YSLib::RecordLevel;
//@}

namespace NPL
{

#ifndef NDEBUG
#	define NPL_Impl_NPLA_CheckStatus true
#else
#	define NPL_Impl_NPLA_CheckStatus false
#endif

string
DecodeNodeIndex(const string& name)
{
	TryRet(string(
		YSLib::make_string_view(std::to_string(YSLib::DecodeIndex(name)))))
	CatchIgnore(std::invalid_argument&);
	return name;
}


string
EscapeNodeLiteral(const ValueNode& node)
{
	return EscapeLiteral(Access<string>(node));
}

string
LiteralizeEscapeNodeLiteral(const ValueNode& node)
{
	return Literalize(EscapeNodeLiteral(node));
}

string
ParseNPLANodeString(const ValueNode& node)
{
	return ystdex::value_or(AccessPtr<string>(node));
}


string
DefaultGenerateIndent(size_t n)
{
	return string(n, '\t');
}

void
PrintIndent(std::ostream& os, IndentGenerator igen, size_t n)
{
	if(YB_LIKELY(n != 0))
		ystdex::write(os, igen(n));
}

void
PrintNode(std::ostream& os, const ValueNode& node, NodeToString node_to_str,
	IndentGenerator igen, size_t depth)
{
	PrintIndent(os, igen, depth);
	os << EscapeLiteral(DecodeNodeIndex(node.GetName())) << ' ';

	const auto print_node_str(std::bind(PrintNodeString, std::ref(os),
		std::placeholders::_1, std::ref(node_to_str)));

	if(!print_node_str(node) && node)
		TraverseSubnodes([&](const ValueNode& nd){
			if(YSLib::IsPrefixedIndex(nd.GetName()))
			{
				PrintIndent(os, igen, depth);
				ystdex::invoke(print_node_str, nd);
			}
			else
				PrintContainedNodes([&](char b){
					PrintIndent(os, igen, depth);
					os << b << '\n';
				}, [&]{
					PrintNode(os, nd, node_to_str, igen, depth + 1);
				});
		}, node);
}

bool
PrintNodeString(std::ostream& os, const ValueNode& node,
	NodeToString node_to_str)
{
	TryRet(os << node_to_str(node) << '\n', true)
	CatchExpr(bad_any_cast&, os << '\n')
	return {};
}


string
ParseNPLATermString(const TermNode& term)
{
	return ystdex::value_or(AccessPtr<string>(term));
}

ValueNode
MapNPLALeafNode(const TermNode& term)
{
	return YSLib::AsNode(ValueNode::allocator_type(term.get_allocator()),
		string(), string(Deliteralize(ParseNPLATermString(term))));
}

TermNode
TransformToSyntaxNode(ValueNode&& node)
{
	const TermNode::allocator_type a(node.get_allocator());
	TermNode res(std::allocator_arg, a, {NPL::AsTermNode(a, std::allocator_arg,
		a, node.GetName())});
	const auto nested_call([&](ValueNode& nd){
		res.Add(TransformToSyntaxNode(std::move(nd)));
	});

	if(node.empty())
	{
		if(const auto p = AccessPtr<YSLib::NodeSequence>(node))
			for(auto& nd : *p)
				nested_call(nd);
		else
			res.emplace(NoContainer, Literalize(ParseNPLANodeString(node)));
	}
	else
		for(auto& nd : node)
			nested_call(nd);
	return res;
}


//! \since build 726
namespace
{

#if NPL_NPLA_CheckParentEnvironment
YB_ATTR_nodiscard YB_PURE bool
IsReserved(string_view id) ynothrowv
{
	YAssertNonnull(id.data());
	return ystdex::begins_with(id, "__");
}
#endif

//! \since build 894
shared_ptr<Environment>
RedirectToShared(string_view id, shared_ptr<Environment> p_env)
{
#if NPL_NPLA_CheckParentEnvironment
	if(p_env)
#else
	yunused(id);
	YAssertNonnull(p_env);
#endif
		return p_env;
#if NPL_NPLA_CheckParentEnvironment
	// XXX: Consider use more concrete semantic failure exception.
	throw InvalidReference(ystdex::sfmt("Invalid reference found for%s name"
		" '%s', probably due to invalid context access by a dangling"
		" reference.",
		IsReserved(id) ? " reserved" : "", id.data()));
#endif
}

//! \since build 869
// XXX: Use other type without overhead of check on call of %operator()?
using Redirector
	= ystdex::unchecked_function<observer_ptr<const ValueObject>()>;

//! \since build 931
observer_ptr<const ValueObject>
RedirectEnvironmentList(Environment::allocator_type a, Redirector& cont,
	EnvironmentList::const_iterator first, EnvironmentList::const_iterator last)
{
	if(first != last)
	{
		cont = ystdex::make_obj_using_allocator<Redirector>(a, trivial_swap,
			std::bind(
			[=, &cont](EnvironmentList::const_iterator i, Redirector& c){
			cont = std::move(c);
			return RedirectEnvironmentList(a, cont, i, last);
		}, std::next(first), std::move(cont)));
		return make_observer(&*first);
	}
	return {};
}

//! \since build 857
YB_ATTR_nodiscard YB_STATELESS TermTags
MergeTermTags(TermTags x, TermTags y) ynothrow
{
	return (((x & ~TermTags::Temporary) | y) & ~TermTags::Unique)
		| (x & y & TermTags::Unique);
}

//! \since build 874
void
MoveRValueFor(TermNode& term, TermNode& tm, bool(TermReference::*pm)() const)
{
	AssertValueTags(tm);
	// XXX: Term tags are currently not respected in prvalues. However, this
	//	should be neutral here due to copy elision in the object language.
	if(const auto p = TryAccessLeafAtom<const TermReference>(tm))
	{
		if(!p->IsReferencedLValue())
			return LiftMovedOther(term, *p, ((*p).*pm)());
	}
	LiftOtherValue(term, tm);
}


/*!
\brief 锚对象使用的共享数据。
\sa EnvironmentReference
\since build 869

共享数据仅被特定的引用访问。
此类引用访问假定 AnchorPtr 实际为 shared_ptr<AnchorData> 。
*/
struct AnchorData final
{
#if NPL_NPLA_CheckEnvironmentReferenceCount
private:
	//! \brief 环境引用计数。
	mutable size_t env_count = 0;
#endif

public:
	DefDeCtor(AnchorData)
	DefDeMoveCtor(AnchorData)

	DefDeMoveAssignment(AnchorData)

#if NPL_NPLA_CheckEnvironmentReferenceCount
	DefGetter(const ynothrow, size_t, Count, env_count)

	/*!
	\brief 转换为锚对象内部数据的引用。
	\pre 断言：参数非空。
	*/
	static PDefH(const AnchorData&, Access, const AnchorPtr& p) ynothrowv
		ImplRet(YAssertNonnull(p),
			NPL::Deref(YSLib::static_pointer_cast<const AnchorData>(p)))

	PDefH(void, AddReference, ) const ynothrow
		ImplExpr(++env_count)

	//! \pre 断言：引用计数大于 0 。
	PDefH(void, RemoveReference, ) const ynothrowv
		ImplExpr(YAssert(env_count > 0, "Invalid zero shared anchor count"
			" for environments found."), --env_count)
#endif
};


//! \since build 921
YB_ATTR_nodiscard YB_PURE std::string
MismatchedTypesToString(const bad_any_cast& e)
{
	// TODO: Use demangled type names?
	return ystdex::sfmt("Mismatched types ('%s', '%s') found.", e.from(),
		e.to());
}

//! \since build 926
YB_NORETURN void
ThrowResolveEnvironmentFailure(const TermNode& term, bool has_ref)
{
	throw ListTypeError(ystdex::sfmt("Invalid environment formed from list '%s'"
		" found.", TermToStringWithReferenceMark(term, has_ref).c_str()));
}

} // unnamed namespace;


LexemeCategory
CategorizeBasicLexeme(string_view id) ynothrowv
{
	const auto c(CheckLiteral(id));

	if(c == '\'')
		return LexemeCategory::Code;
	if(c != char())
		return LexemeCategory::Data;
	return LexemeCategory::Symbol;
}

LexemeCategory
CategorizeLexeme(string_view id) ynothrowv
{
	const auto res(CategorizeBasicLexeme(id));

	return res == LexemeCategory::Symbol && IsNPLAExtendedLiteral(id)
		? LexemeCategory::Extended : res;
}

bool
IsNPLAExtendedLiteral(string_view id) ynothrowv
{
	YAssertNonnull(id.data());

	if(!id.empty())
	{
		const char f(id.front());

		return (id.size() > 1 && IsNPLAExtendedLiteralNonDigitPrefix(f)
			&& !IsAllSignLexeme(id)) || ystdex::isdigit(f);
	}
	return {};
}


string
TermToString(const TermNode& term, size_t n_skip)
{
	if(const auto p = TermToNamePtr(term))
		return *p;

	const bool non_list(!IsList(term));

	// TODO: Use allocator?
	// NOTE: The case for no skip is shown below.
#if false
	if(n_skip == 0)
	{
		const bool is_pair(IsPair(term));

		return IsEmpty(term) ? string("()") : YSLib::sfmt<string>(
			"#<%s{%zu}%s%s>", is_pair ? (non_list ? "improper-list" : "list")
			: "unknown", term.size(), is_pair ? " . " : (non_list ? ":" : ""),
			non_list ? term.Value.type().name() : "");
	}
#endif
	YAssert(n_skip <= CountPrefix(term), "Invalid skip number found.");

	const bool s_is_pair(n_skip < term.size()
		&& IsSticky(std::next(term.begin(), ptrdiff_t(n_skip))->Tags));

	return !non_list && n_skip == term.size() ? string("()")
		: YSLib::sfmt<string>("#<%s{%zu}%s%s>",
		s_is_pair ? (non_list ? "improper-list" : "list") : "unknown",
		term.size() - n_skip, s_is_pair ? " . " : (non_list ? ":" : ""),
		non_list ? term.Value.type().name() : "");
}

string
TermToStringWithReferenceMark(const TermNode& term, bool has_ref, size_t n_skip)
{
	auto term_str(TermToString(term, n_skip));

	return has_ref ? "[*] " + std::move(term_str) : std::move(term_str);
}

TermTags
TermToTags(TermNode& term)
{
	AssertReferentTags(term);
	return ystdex::call_value_or(ystdex::compose(GetLValueTagsOf,
		std::mem_fn(&TermReference::GetTags)),
		TryAccessLeafAtom<const TermReference>(term), term.Tags);
}

void
TokenizeTerm(TermNode& term)
{
	for(auto& child : term)
		TokenizeTerm(child);
	if(const auto p = AccessPtr<string>(term))
		term.Value.emplace<TokenValue>(std::move(*p));
}


#if NPL_NPLA_CheckTermReferenceIndirection
TermNode&
TermReference::get() const
{
	if(!(r_env.GetAnchorPtr() && r_env.GetPtr().expired()))
		return term_ref.get();
	throw InvalidReference("Invalid reference found on indirection, probably"
		" due to invalid context access by a dangling reference.");
}
#endif


pair<TermReference, bool>
Collapse(TermReference ref)
{
	if(const auto p = TryAccessLeafAtom<TermReference>(ref.get()))
	{
		// XXX: Term tags on prvalues are reserved and should be ignored
		//	normally except for future internal use. The only case making the
		//	tags available from the referent of %ref is that
		//	%TermTags::Temporary of the bound object which is the referent of.
		//	Since %MergeTermTags does not keep %TermTags::Temporary of the 1st
		//	argument, this is also the unique source of that tag now.
		ref.SetTags(MergeTermTags(ref.GetTags(), p->GetTags())),
		ref.SetReferent(p->get());
		return {std::move(ref), true};
	}
	return {std::move(ref), {}};
}

TermNode
PrepareCollapse(TermNode& term, const shared_ptr<Environment>& p_env)
{
	return IsTyped<TermReference>(term) ? term
		: NPL::AsTermNode(term.get_allocator(),
		TermReference(p_env->MakeTermTags(term), term, NPL::Nonnull(p_env)));
}


bool
IsReferenceTerm(const TermNode& term)
{
	return bool(TryAccessLeafAtom<const TermReference>(term));
}

bool
IsUniqueTerm(const TermNode& term)
{
	return ystdex::invoke_value_or(&TermReference::IsUnique,
		TryAccessLeafAtom<const TermReference>(term), true);
}

bool
IsModifiableTerm(const TermNode& term)
{
	return ystdex::invoke_value_or(&TermReference::IsModifiable,
		TryAccessLeafAtom<const TermReference>(term),
		!bool(term.Tags & TermTags::Nonmodifying));
}

bool
IsTemporaryTerm(const TermNode& term)
{
	return ystdex::invoke_value_or(&TermReference::IsTemporary,
		TryAccessLeafAtom<const TermReference>(term),
		bool(term.Tags & TermTags::Temporary));
}

bool
IsBoundLValueTerm(const TermNode& term)
{
	return ystdex::invoke_value_or(&TermReference::IsReferencedLValue,
		TryAccessLeafAtom<const TermReference>(term));
}

bool
IsUncollapsedTerm(const TermNode& term)
{
	return ystdex::call_value_or(ystdex::compose(IsReferenceTerm,
		std::mem_fn(&TermReference::get)),
		TryAccessLeafAtom<const TermReference>(term));
}


void
LiftOtherOrCopy(TermNode& term, TermNode& tm, bool move)
{
	if(move)
		LiftOther(term, tm);
	else
		// NOTE: Although %tm is required to be not same to %term, it can still
		//	be an object exclusively owned by %term, e.g. when %term represents
		//	a reference value with irregular representation and %tm is the term
		//	referenced uniquely by %term.Value and owned by a subterm of %term.
		//	Explicit copying is necessary for such cases to prevent invalidating
		//	the subterm (i.e. the owner of %tm) prematurely.
		term.CopyContent(tm);
}

void
LiftTermOrCopy(TermNode& term, TermNode& tm, bool move)
{
	if(move)
		LiftTerm(term, tm);
	else
		term.CopyContent(tm);
}

void
LiftTermValueOrCopy(TermNode& term, TermNode& tm, bool move)
{
	// NOTE: See $2018-02 @ %Documentation::Workflow.
	// NOTE: This is compatible to the irregular representation of subobject
	//	references provided the setting order of %NPL::SetContentWith.
	// TODO: Check the representation is sane?
	// XXX: Term tags are currently not respected in prvalues.
	NPL::SetContentWith(term, tm,
		move ? &ValueObject::MakeMove : &ValueObject::MakeCopy);
	term.Tags = tm.Tags;
}


void
LiftCollapsed(TermNode& term, TermNode& tm, TermReference ref)
{
	auto pr(Collapse(std::move(ref)));

	if(!ystdex::ref_eq<>()(term, tm))
		term.SetContent(TermNode(std::move(tm.GetContainerRef()),
			std::move(pr.first)));
	else if(pr.second)
		yunseq(term.Value = std::move(pr.first),
			term.Tags = TermTags::Unqualified);
}

void
MoveCollapsed(TermNode& term, TermNode& tm)
{
	if(const auto p = TryAccessLeafAtom<TermReference>(tm))
		term.SetContent(TermNode(std::move(tm.GetContainerRef()),
			Collapse(std::move(*p)).first));
	else
		LiftOther(term, tm);
}

void
LiftPropagatedReference(TermNode& term, TermNode& tm, TermTags tags)
{
	// NOTE: Save the reference at first to prevent the possible
	//	invalidation of the object %ref by the modification of the subterms
	//	of %term. See also %LiftOtherOrCopy.
#if YB_IMPL_GNUCPP >= 120000
	// XXX: This is verbose but no more efficient with x86_64-pc-linux G++ 11.1.
	const auto& ref(tm.Value.GetObject<TermReference>());
	TermReference nref(PropagateTo(ref.GetTags(), tags), ref.get(),
		ref.GetEnvironmentReference());

	term.CopyContainer(tm);
	term.Value = std::move(nref);
#else
	term.CopyContent(tm);
	term.Value.GetObject<TermReference>().PropagateFrom(tags);
#endif
}

void
LiftToReference(TermNode& term, TermNode& tm)
{
	if(tm)
	{
		if(IsReferenceTerm(tm))
			// NOTE: Reference collapsed.
			LiftTerm(term, tm);
		else if(tm.Value.OwnsCount() > 1)
			// XXX: This is unsafe and not checkable because the anchor is not
			//	referenced.
			term.SetValue(in_place_type<TermReference>, TermTags::Unqualified,
				tm, EnvironmentReference());
		else
			throw InvalidReference(
				"Value of a temporary shall not be referenced.");
	}
	else
		ystdex::throw_invalid_construction();
}

void
LiftToReturn(TermNode& term)
{
	// XXX: Term tags are currently not respected in prvalues. However, this
	//	should be neutral here due to copy elision in the object language. Note
	//	the operation here is idempotent for qualified expressions.
	// TODO: Detect lifetime escape to perform copy elision?
	if(const auto p = TryAccessLeafAtom<const TermReference>(term))
		// XXX: Using %LiftMovedOther instead of %LiftMoved is safe, because the
		//	referent is not allowed to be same to %term in NPLA.
		LiftMovedOther(term, *p, p->IsMovable());
	AssertValueTags(term);
}

void
MoveRValueToForward(TermNode& term, TermNode& tm)
{
#if true
	MoveRValueFor(term, tm, &TermReference::IsModifiable);
#else
	// NOTE: For exposition only. The optimized implemenation shall be
	//	equivalent to this, except for the copy elision.
	LiftOtherValue(term, tm);
	if(!IsBoundLValueTerm(term))
	{
		if(const auto p = TryAccessLeafAtom<const TermReference>(term))
			LiftMovedOther(term, *p, p->IsModifiable());
	}
#endif
}

void
MoveRValueToReturn(TermNode& term, TermNode& tm)
{
#if true
	MoveRValueFor(term, tm, &TermReference::IsMovable);
#else
	// NOTE: For exposition only. The optimized implemenation shall be
	//	equivalent to this, except for the copy elision.
	LiftOtherValue(term, tm);
	if(!IsBoundLValueTerm(term))
		LiftToReturn(term);
#endif
}

TNIter
LiftPrefixToReturn(TermNode& term, TNCIter it)
{
	YAssert(size_t(std::distance(ystdex::as_const(term).begin(), it))
		<= CountPrefix(term), "Invalid arguments found.");

	auto i(ystdex::cast_mutable(term.GetContainerRef(), it));

	while(i != term.end() && !IsSticky(i->Tags))
	{
		LiftToReturn(*i);
		++i;
	}
	YAssert(term.Value || i == term.end(), "Invalid representation found.");
	return i;
}

void
LiftSuffixToReturn(TermNode& term, TNCIter i)
{
	YAssert(FindSticky(ystdex::as_const(term).begin(),
		ystdex::as_const(term).end()) == i, "Invalid arguments found.");
	// NOTE: As %LiftToReturn without overriding the previous transformed terms.
	if(const auto p = TryAccessLeaf<const TermReference>(term))
	{
		auto& nd(p->get());
		const bool move(p->IsMovable());
		// XXX: As %LiftOtherOrCopy without touching %term.Tags.
		const auto lift_content([&]{
			const auto set_content([&](TermNode::Container con, ValueObject vo){
				term.SetContent(std::move(con), std::move(vo));
			});

			if(move)
				set_content(std::move(nd.GetContainerRef()),
					ValueObject(std::move(nd.Value)));
			else
				set_content(TermNode::Container(nd.GetContainer(),
					nd.get_allocator()), nd.Value);
		});
		auto& con(term.GetContainerRef());

		if(i == con.begin())
			lift_content();
		else
		{
#if false
			// XXX: This is less efficient at least with x86_64-pc-linux G++
			//	12.1.
			const auto lift([&](TermNode::Container tcon, ValueObject vo){
				term.Value = std::move(vo);
				con.splice(i, tcon);
				con.erase(i, con.end());
			});

			if(move)
				lift(TermNode::Container(nd.GetContainer(), nd.get_allocator()),
					nd.Value);
			else
				lift(std::move(nd.GetContainerRef()), std::move(nd.Value));
#else
			// NOTE: Any optimized implemenations shall be equivalent to this.
			TermNode::Container tcon(con.get_allocator());

			tcon.splice(tcon.end(), con);
			lift_content();
			con.splice(con.begin(), tcon);
#endif
		}
	}
	// NOTE: %AssertValueTags shall not be called here because %term.Tags may
	//	have %TermTags::Temporary at the beginning at the call. This is expected
	//	to be cleared later when needed (e.g. by %ClearCombiningTags called
	//	later in %RegularizeTerm).
}


bool
CheckReducible(ReductionStatus status)
{
	switch(status)
	{
	case ReductionStatus::Partial:
	case ReductionStatus::Retrying:
		return true;
	default:
#if NPL_Impl_NPLA_CheckStatus
		// NOTE: Keep away assertions so client code can be diagnosed.
		if(YB_UNLIKELY(status != ReductionStatus::Neutral
			&& status != ReductionStatus::Clean
			&& status != ReductionStatus::Retained))
			YTraceDe(Warning, "Unexpected status found.");
#endif
		break;
	}
	return {};
}

ReductionStatus
RegularizeTerm(TermNode& term, ReductionStatus status) ynothrow
{
	ClearCombiningTags(term);
	// NOTE: Cleanup if and only if necessary.
	if(status == ReductionStatus::Clean)
		term.ClearContainer();
	return status;
}


ReductionStatus
ReduceBranchToList(TermNode& term) ynothrowv
{
	RemoveHead(term);
	// XXX: Not using %EmplaceCallResultOrReturn because this may be called on a
	//	bound object (rather than a call result). Notice in such case, the
	//	retained value is not neccessarily a first-class value.
	return ReductionStatus::Retained;
}

ReductionStatus
ReduceBranchToListValue(TermNode& term) ynothrowv
{
	RemoveHead(term);
	LiftElementsToReturn(term);
	// XXX: Ditto.
	return ReductionStatus::Retained;
}

ReductionStatus
ReduceHeadEmptyList(TermNode& term) ynothrow
{
	if(term.size() > 1 && IsEmpty(AccessFirstSubterm(term)))
		RemoveHead(term);
	return ReductionStatus::Neutral;
}

ReductionStatus
ReduceToReference(TermNode& term, TermNode& tm, ResolvedTermReferencePtr p_ref)
{
	if(const auto p = TryAccessLeafAtom<const TermReference>(tm))
	{
		AssertValueTags(tm);
		// NOTE: Reference collapsed.
		// XXX: As %LiftOtherOrCopy, except that tags specified by %p_ref are
		//	also propagated.
		// XXX: This can be more efficient than %LiftOtherOrCopy with additional
		//	modification on the tags.
		if(!p_ref)
			LiftOther(term, tm);
		else
			LiftPropagatedReference(term, tm, p_ref->GetTags());
		// XXX: The resulted representation can be irregular.
		return ReductionStatus::Retained;
	}
	return ReduceToReferenceAt(term, tm, p_ref);
}

ReductionStatus
ReduceToReferenceAt(TermNode& term, TermNode& tm,
	ResolvedTermReferencePtr p_ref)
{
	AssertValueTags(tm);
	// XXX: Term tags on prvalues are reserved and should be ignored normally
	//	except for future internal use. Since %tm is a term,
	//	%TermTags::Temporary is not expected, %GetLValueTagsOf is also not used.
	term.SetValue(in_place_type<TermReference>, PropagateTo(tm.Tags,
		p_ref->GetTags()), tm, NPL::Deref(p_ref).GetEnvironmentReference());
	return ReductionStatus::Clean;
}


#if NPL_NPLA_CheckEnvironmentReferenceCount
Environment::~Environment()
{
	if(p_anchor)
	{
		const auto acnt(p_anchor.use_count());

		if(acnt >= 1)
		{
			// XXX: Assume this would not wrap.
			const auto ecnt(AnchorData::Access(p_anchor).GetCount() + 1);

			if(ecnt < size_t(acnt))
				YSLib::FilterExceptions([this, acnt, ecnt]{
					const size_t n(Bindings.size());
					size_t i(0);
					string str(Bindings.get_allocator());
					// XXX: The value is heuristic for common cases.
					str.reserve(n * yimpl(8));

					for(const auto& pr : Bindings)
					{
						str += pr.first;
						if(++i != n)
							str += ", ";
					}
					YTraceDe(Err, "Invalid environment destruction: remained"
						" shared anchor count for reference values is nonzero"
						" value %zu, with bindings {%s}.", size_t(acnt) - ecnt,
						str.c_str());
				}, yfsig);
			else if(YB_UNLIKELY(ecnt > size_t(acnt)))
				YTraceDe(Err, "Anchor count for environments %zu is greater"
					" than anchor count %ld.", ecnt, acnt);
		}
		else
			YTraceDe(Err, "Invalid environment use count %ld found.", acnt);
	}
	else
		YTraceDe(Err, "Invalid environment anchor found in destruction.");
}
#else
ImplDeDtor(Environment)
#endif

void
Environment::CheckParent(const ValueObject& vo)
{
	const auto& ti(vo.type());

	if(IsTyped<EnvironmentList>(ti))
	{
		for(const auto& env : vo.GetObject<EnvironmentList>())
			CheckParent(env);
	}
	else if(YB_UNLIKELY(!IsTyped<observer_ptr<const Environment>>(ti)
		&& !IsTyped<EnvironmentReference>(ti)
		&& !IsTyped<shared_ptr<Environment>>(ti)))
		ThrowForInvalidType(ti);
#if NPL_NPLA_CheckParentEnvironment
	if(IsTyped<observer_ptr<const Environment>>(ti))
	{
		if(YB_UNLIKELY(!vo.GetObject<observer_ptr<const Environment>>()))
			// NOTE: See %EnsureValid.
			ThrowForInvalidValue();
	}
	else if(IsTyped<EnvironmentReference>(ti))
		EnsureValid(vo.GetObject<EnvironmentReference>().Lock());
	else if(IsTyped<shared_ptr<Environment>>(ti))
		EnsureValid(vo.GetObject<shared_ptr<Environment>>());
#endif
}

bool
Environment::Deduplicate(BindingMap& dst, const BindingMap& src)
{
	for(const auto& binding : src)
		// XXX: Non-trivially destructible objects is treated same.
		// NOTE: Redirection is not needed here.
		dst.erase(binding.first);
	// NOTE: If the resulted parent environment is empty, it is safe to be
	//	removed.
	return dst.empty();
}

void
Environment::Define(string_view id, ValueObject&& vo)
{
	YAssertNonnull(id.data());
	// XXX: Self overwriting is possible.
	swap((*this)[id].Value, vo);
}

void
Environment::DefineChecked(string_view id, ValueObject&& vo)
{
	YAssertNonnull(id.data());
	if(!AddValue(id, std::move(vo)))
		throw BadIdentifier(id, 2);
}

Environment&
Environment::EnsureValid(const shared_ptr<Environment>& p_env)
{
	if(p_env)
		return *p_env;
	ThrowForInvalidValue();
}

AnchorPtr
Environment::InitAnchor() const
{
	return YSLib::allocate_shared<AnchorData>(Bindings.get_allocator());
}

Environment::NameResolution::first_type
Environment::LookupName(string_view id) const
{
	YAssertNonnull(id.data());
	return make_observer(ystdex::call_value_or<BindingMap::mapped_type*>(
		ystdex::compose(ystdex::addrof<>(), ystdex::second_of<>()),
		Bindings.find(id), {}, Bindings.cend()));
}

bool
Environment::Remove(string_view id)
{
	YAssertNonnull(id.data());
	// XXX: %BindingMap does not have transparent key %erase. This is like
	//	%std::set.
	return ystdex::erase_first(Bindings, id);
}

void
Environment::RemoveChecked(string_view id)
{
	if(!Remove(id))
		throw BadIdentifier(id, 0);
}

bool
Environment::Replace(string_view id, ValueObject&& vo)
{
	if(const auto p = LookupName(id))
	{
		swap(p->Value, vo);
		return true;
	}
	return {};
}

void
Environment::ReplaceChecked(string_view id, ValueObject&& vo)
{
	if(!Replace(id, std::move(vo)))
		throw BadIdentifier(id, 0);
}

void
Environment::ThrowForInvalidType(const type_info& ti)
{
	throw TypeError(
		ystdex::sfmt("Invalid environment type '%s' found.", ti.name()));
}

void
Environment::ThrowForInvalidValue(bool record)
{
	throw std::invalid_argument(record
		? "Invalid environment record pointer found."
		: "Invalid environment found.");
}


#if NPL_NPLA_CheckEnvironmentReferenceCount
EnvironmentReference::~EnvironmentReference()
{
	if(p_anchor)
		AnchorData::Access(p_anchor).RemoveReference();
}

void
EnvironmentReference::ReferenceEnvironmentAnchor()
{
	if(p_anchor)
		AnchorData::Access(p_anchor).AddReference();
}
#endif


ContextNode::ContextNode(pmr::memory_resource& rsrc)
	: memory_rsrc(rsrc)
{}
ContextNode::ContextNode(const ContextNode& ctx,
	shared_ptr<Environment>&& p_rec)
	: memory_rsrc(ctx.memory_rsrc), p_record([&]{
		if(p_rec)
			return std::move(p_rec);
		Environment::ThrowForInvalidValue(true);
	}()),
	Resolve(ctx.Resolve), current(ctx.current), stacked(ctx.stacked),
	Trace(ctx.Trace)
{}
ContextNode::ContextNode(const ContextNode& ctx)
	: memory_rsrc(ctx.memory_rsrc), p_record(ctx.p_record),
	Resolve(ctx.Resolve), current(ctx.current), stacked(ctx.stacked),
	LastStatus(ctx.LastStatus), Trace(ctx.Trace)
{}
ContextNode::ContextNode(ContextNode&& ctx) ynothrow
	: ContextNode(ctx.memory_rsrc)
{
	swap(ctx, *this);
}
ContextNode::~ContextNode()
{
	UnwindCurrent();
}

ReductionStatus
ContextNode::ApplyTail()
{
	// TODO: Add check to avoid native stack overflow when the current action is
	//	called?
	YAssert(IsAlive(), "No tail action found.");
	// NOTE: This also overwrites any previously stored value of %TailAction.
	TailAction = std::move(current.front());
	stashed.splice_after(stashed.cbefore_begin(), current,
		current.cbefore_begin());
	TryExpr(LastStatus = TailAction(*this))
	CatchExpr(..., HandleException(std::current_exception()))
	// NOTE: To make PTC works, %TailAction is not released after the call. It
	//	should be overwritten by the next call to %ApplyTail normally. When
	//	abnormally exited (e.g. in a call to %HandleException), %TailAction is
	//	usually to be cleanup separately.
	return LastStatus;
}

void
ContextNode::DefaultHandleException(std::exception_ptr p)
{
	YAssertNonnull(p);
	TryExpr(std::rethrow_exception(std::move(p)))
	CatchExpr(bad_any_cast& e,
		std::throw_with_nested(TypeError(MismatchedTypesToString(e))))
}

Environment::NameResolution
ContextNode::DefaultResolve(shared_ptr<Environment> p_env, string_view id)
{
	YAssertNonnull(p_env);

	Redirector cont;
	// NOTE: Blocked. Use ISO C++14 deduced lambda return type (cf. CWG 975)
	//	compatible to G++ attribute.
	const auto p_obj(ystdex::retry_on_cond(
		// XXX: Flatten attribute is less efficient with x86_64-pc-linux G++
		//	12.1.
#if YB_IMPL_GNUCPP >= 120000
		[&](Environment::NameResolution::first_type p)
#else
		[&] YB_LAMBDA_ANNOTATE(
		(Environment::NameResolution::first_type p), , flatten)
#endif
	// XXX: This uses G++ extension to work around the compatible issue. See
	//	also %YB_ATTR_LAMBDA_QUAL.
#if !(YB_IMPL_GNUCPP >= 90000)
		-> bool
#endif
	{
		if(!p)
		{
			lref<const ValueObject> cur(p_env->Parent);
			shared_ptr<Environment> p_redirected{};

			ystdex::retry_on_cond(ystdex::id<>(),
				// XXX: This is still more efficient with G++ 12.1.
				[&] YB_LAMBDA_ANNOTATE((), , flatten)
			// XXX: Ditto.
#if !(YB_IMPL_GNUCPP >= 90000)
				-> bool
#endif
			{
				const ValueObject& parent(cur);
				const auto& ti(parent.type());

				if(IsTyped<EnvironmentReference>(ti))
				{
					p_redirected = RedirectToShared(id,
						parent.GetObject<EnvironmentReference>().Lock());
					p_env.swap(p_redirected);
				}
				else if(IsTyped<shared_ptr<Environment>>(ti))
				{
					p_redirected = RedirectToShared(id,
						parent.GetObject<shared_ptr<Environment>>());
					p_env.swap(p_redirected);
				}
				else
				{
					observer_ptr<const ValueObject> p_next{};

					if(IsTyped<EnvironmentList>(ti))
					{
						auto& envs(parent.GetObject<EnvironmentList>());

						p_next = RedirectEnvironmentList(
							p_env->Bindings.get_allocator(), cont,
							envs.cbegin(), envs.cend());
					}
					while(!p_next && bool(cont))
						p_next = ystdex::exchange(cont, Redirector())();
					if(p_next)
					{
						YAssert(!ystdex::ref_eq<>()(cur.get(), *p_next),
							"Cyclic parent found.");
						cur = *p_next;
						return true;
					}
				}
				return false;
			});
			return bool(p_redirected);
		}
		return false;
	}, [&, id]{
		return p_env->LookupName(id);
	}));

	return {p_obj, std::move(p_env)};
}

ReductionStatus
ContextNode::RewriteLoop()
{
	YAssert(IsAlive(), "No action to reduce.");
	// NOTE: Rewrite until no actions remain.
	return ystdex::retry_on_cond(std::bind(&ContextNode::IsAlive, this), [&]{
		return ApplyTail();
	});
}

shared_ptr<Environment>
ContextNode::SwitchEnvironment(const shared_ptr<Environment>& p_env)
{
	if(p_env)
		return SwitchEnvironmentUnchecked(p_env);
	Environment::ThrowForInvalidValue(true);
}

void
swap(ContextNode& x, ContextNode& y) ynothrow
{
	swap(x.p_record, y.p_record),
	swap(x.Resolve, y.Resolve),
	swap(x.current, y.current),
	swap(x.stashed, y.stashed),
	std::swap(x.LastStatus, y.LastStatus),
	swap(x.Trace, y.Trace);
}


TermNode
MoveResolved(const ContextNode& ctx, string_view id)
{
	auto pr(ResolveName(ctx, id));

	if(const auto p = pr.first)
	{
		if(NPL::Deref(pr.second).Frozen)
			return *p;
		return std::move(*p);
	}
	throw BadIdentifier(id);
}

TermNode
ResolveIdentifier(const ContextNode& ctx, string_view id)
{
	auto pr(ResolveName(ctx, id));

	if(pr.first)
		return PrepareCollapse(*pr.first, pr.second);
	throw BadIdentifier(id);
}

pair<shared_ptr<Environment>, bool>
ResolveEnvironment(const ValueObject& vo)
{
	// XXX: Support more environment types?
	if(const auto p = vo.AccessPtr<const EnvironmentReference>())
		return {p->Lock(), {}};
	if(const auto p = vo.AccessPtr<const shared_ptr<Environment>>())
		return {*p, true};
	// TODO: Merge with %Environment::CheckParent?
	Environment::ThrowForInvalidType(vo.type());
}
pair<shared_ptr<Environment>, bool>
ResolveEnvironment(ValueObject& vo, bool move)
{
	// XXX: Ditto.
	if(const auto p = vo.AccessPtr<const EnvironmentReference>())
		return {p->Lock(), {}};
	if(const auto p = vo.AccessPtr<shared_ptr<Environment>>())
		return {move ? std::move(*p) : *p, true};
	// TODO: Ditto.
	Environment::ThrowForInvalidType(vo.type());
}
pair<shared_ptr<Environment>, bool>
ResolveEnvironment(const TermNode& term)
{
	return ResolveTerm([&](const TermNode& nd, bool has_ref){
		if(!IsExtendedList(nd))
			return ResolveEnvironment(nd.Value);
		ThrowResolveEnvironmentFailure(nd, has_ref);
	}, term);
}
pair<shared_ptr<Environment>, bool>
ResolveEnvironment(TermNode& term)
{
	return ResolveTerm([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
		if(!IsExtendedList(nd))
			return ResolveEnvironment(nd.Value, NPL::IsMovable(p_ref));
		ThrowResolveEnvironmentFailure(nd, p_ref);
	}, term);
}


void
TraceException(std::exception& e, YSLib::Logger& trace)
{
	YSLib::ExtractException(
		[&] YB_LAMBDA_ANNOTATE((const char* str, size_t level), , nonnull(2)){
		const auto print([&] YB_LAMBDA_ANNOTATE((RecordLevel lv,
			const char* name, const char* msg), ynothrow, nonnull(3)){
			// XXX: Similar to %YSLib::PrintMessage.
			trace.TraceFormat(lv, "%*s%s<%u>: %s", int(level), "", name,
				unsigned(lv), msg);
		});

		TryExpr(throw)
		// XXX: This clause relies on the source information for meaningful
		//	output. Assume it is used.
		catch(BadIdentifier& ex)
		{
			print(ex.GetLevel(), "BadIdentifier", str);

			const auto& si(ex.Source);

			if(ex.Source.first)
				trace.TraceFormat(ex.GetLevel(), "%*sIdentifier '%s' is at"
					" line %zu, column %zu in %s.", int(level + 1), "",
					ex.GetIdentifier().c_str(), si.second.Line + 1,
					si.second.Column + 1, si.first->c_str());
		}
		CatchExpr(bad_any_cast& ex,
			print(Warning, "TypeError", MismatchedTypesToString(ex).c_str()))
		CatchExpr(LoggedEvent& ex, print(ex.GetLevel(), typeid(ex).name(), str))
		CatchExpr(..., print(Err, "Error", str))
	}, e);
}

} // namespace NPL;

