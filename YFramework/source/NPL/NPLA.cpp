﻿/*
	© 2014-2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA.cpp
\ingroup NPL
\brief NPLA 公共接口。
\version r3580
\author FrankHB <frankhb1989@gmail.com>
\since build 663
\par 创建时间:
	2016-01-07 10:32:45 +0800
\par 修改时间:
	2021-07-05 00:28 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA
*/


#include "NPL/YModules.h"
#include YFM_NPL_NPLA // for YSLib::Warning, YSLib::err, string,
//	YSLib::DecodeIndex, std::to_string, YSLib::make_string_view,
//	std::invalid_argument, ValueNode, NPL::Access, EscapeLiteral, Literalize,
//	NPL::AccessPtr, ystdex::value_or, ystdex::write, TraverseSubnodes,
//	bad_any_cast, std::allocator_arg, YSLib::NodeSequence, NPL::Deref,
//	AccessFirstSubterm, ystdex::unimplemented, ystdex::type_id, ystdex::quote,
//	ystdex::call_value_or, ystdex::begins_with, ystdex::sfmt,
//	NPL::make_observer, YSLib::sfmt, GetLValueTagsOf, std::mem_fn,
//	ystdex::compose, ystdex::invoke_value_or, NPL::TryAccessLeaf,
//	NPL::IsMovable, ystdex::ref, PropagateTo, YSLib::FilterExceptions,
//	ystdex::id, ystdex::retry_on_cond, ystdex::type_info, pair, ystdex::addrof,
//	ystdex::second_of, std::rethrow_exception, std::throw_with_nested,
//	YSLib::ExtractException;
#include YFM_NPL_SContext

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
	return EscapeLiteral(NPL::Access<string>(node));
}

string
LiteralizeEscapeNodeLiteral(const ValueNode& node)
{
	return Literalize(EscapeNodeLiteral(node));
}

string
ParseNPLANodeString(const ValueNode& node)
{
	return ystdex::value_or(NPL::AccessPtr<string>(node));
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
	return ystdex::value_or(NPL::AccessPtr<string>(term));
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
		if(const auto p = NPL::AccessPtr<YSLib::NodeSequence>(node))
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

namespace SXML
{

string
ConvertAttributeNodeString(const TermNode& term)
{
	switch(term.size())
	{
	default:
		YTraceDe(Warning, "Invalid term with more than 2 children found.");
		YB_ATTR_fallthrough;
	case 2:
		{
			auto i(term.begin());
			const auto& n(NPL::Access<string>(NPL::Deref(i)));

			return n + '=' + NPL::Access<string>(NPL::Deref(++i));
		}
		YB_ATTR_fallthrough;
	case 1:
		return NPL::Access<string>(AccessFirstSubterm(term));
	case 0:
		break;
	}
	throw LoggedEvent("Invalid term with less than 1 children found.", Warning);
}

string
ConvertDocumentNode(const TermNode& term, IndentGenerator igen, size_t depth,
	ParseOption opt)
{
	if(term)
	{
		string res(ConvertStringNode(term));

		if(res.empty())
		{
			if(opt == ParseOption::String)
				throw LoggedEvent("Invalid non-string term found.");
			if(IsBranch(term))
				try
				{
					auto i(term.begin());
					const auto& str(NPL::Access<string>(NPL::Deref(i)));

					++i;
					if(str == "@")
					{
						for(; i != term.end(); ++i)
							res += ' '
								+ ConvertAttributeNodeString(NPL::Deref(i));
						return res;
					}
					if(opt == ParseOption::Attribute)
						throw LoggedEvent("Invalid non-attribute term found.");
					if(str == "*PI*")
					{
						res = "<?";
						for(; i != term.end(); ++i)
							res += string(Deliteralize(
								ConvertDocumentNode(NPL::Deref(i), igen, depth,
								ParseOption::String))) + ' ';
						if(!res.empty())
							res.pop_back();
						return res + "?>";
					}
					if(str == "*ENTITY*" || str == "*NAMESPACES*")
					{
						if(opt == ParseOption::Strict)
							throw ystdex::unimplemented();
					}
					else if(str == "*COMMENT*")
						;
					else if(!str.empty())
					{
						const bool is_content(str != "*TOP*");
						string head('<' + str);
						bool nl{};

						if(YB_UNLIKELY(!is_content && depth > 0))
							YTraceDe(Warning, "Invalid *TOP* found.");
						if(i != term.end())
						{
							if(!NPL::Deref(i).empty()
								&& (i->begin())->Value == string("@"))
							{
								head += string(
									Deliteralize(ConvertDocumentNode(*i, igen,
									depth, ParseOption::Attribute)));
								if(++i == term.end())
									return head + " />";
							}
							head += '>';
						}
						else
							return head + " />";
						for(; i != term.end(); ++i)
						{
							nl = NPL::Deref(i).Value.type()
								!= ystdex::type_id<string>();
							if(nl)
								res += '\n' + igen(depth + size_t(is_content));
							else
								res += ' ';
							res += string(Deliteralize(ConvertDocumentNode(*i,
								igen, depth + size_t(is_content))));
						}
						if(res.size() > 1 && res.front() == ' ')
							res.erase(0, 1);
						if(!res.empty() && res.back() == ' ')
							res.pop_back();
						if(is_content)
						{
							if(nl)
								res += '\n' + igen(depth);
							return head + res + ystdex::quote(str, "</", '>');
						}
					}
					else
						throw LoggedEvent("Empty item found.", Warning);
				}
				CatchExpr(bad_any_cast& e, YTraceDe(Warning,
					"Conversion failed: <%s> to <%s>.", e.from(), e.to()))
		}
		return res;
	}
	throw LoggedEvent("Empty term found.", Warning);
}

string
ConvertStringNode(const TermNode& term)
{
	return ystdex::call_value_or(EscapeXML, NPL::AccessPtr<string>(term));
}

void
PrintSyntaxNode(std::ostream& os, const TermNode& term, IndentGenerator igen,
	size_t depth)
{
	if(IsBranch(term))
		ystdex::write(os,
			ConvertDocumentNode(AccessFirstSubterm(term), igen, depth), 1);
	os << std::flush;
}


TermNode
MakeXMLDecl(const string& name, const string& ver, const string& enc,
	const string& sd)
{
	auto decl("version=\"" + ver + '"');

	if(!enc.empty())
		decl += " encoding=\"" + enc + '"';
	if(!sd.empty())
		decl += " standalone=\"" + sd + '"';
	return SXML::NodeLiteralToTerm(name, {{MakeIndex(0), "*PI*"},
		{MakeIndex(1), "xml"}, {MakeIndex(2), decl + ' '}});
}

TermNode
MakeXMLDoc(const string& name, const string& ver, const string& enc,
	const string& sd)
{
	auto doc(MakeTop(name));

	doc.Add(MakeXMLDecl(MakeIndex(1), ver, enc, sd));
	return doc;
}

} // namespace SXML;


//! \since build 726
namespace
{

//! \since build 731
string
InitBadIdentifierExceptionString(string&& id, size_t n)
{
	return (n != 0 ? (n == 1 ? "Bad identifier: '" : "Duplicate identifier: '")
		: "Unknown identifier: '") + std::move(id) + "'.";
}


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
using Redirector = function<observer_ptr<const ValueObject>()>;

//! \since build 884
observer_ptr<const ValueObject>
RedirectEnvironmentList(EnvironmentList::const_iterator first,
	EnvironmentList::const_iterator last, string_view id, Redirector& cont)
{
	if(first != last)
	{
		cont = std::bind(
			[=, &cont](EnvironmentList::const_iterator i, Redirector& c){
			cont = std::move(c);
			return RedirectEnvironmentList(i, last, id, cont);
		}, std::next(first), std::move(cont));
		return NPL::make_observer(&*first);
	}
	return {};
}

//! \since build 857
TermTags
MergeTermTags(TermTags x, TermTags y) ynothrow
{
	return (x | y) & ~(TermTags::Unique | TermTags::Temporary)
		& (x & y & TermTags::Unique);
}

//! \since build 874
void
MoveRValueFor(TermNode& term, TermNode& tm, bool(TermReference::*pm)() const)
{
	// XXX: Term tags are currently not respected in prvalues. However, this
	//	should be neutral here due to copy elision in the object language.
	if(const auto p = NPL::TryAccessLeaf<const TermReference>(tm))
	{
		if(!p->IsReferencedLValue())
			return LiftMovedOther(term, *p, ((*p).*pm)());
	}
	LiftOther(term, tm);
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

} // unnamed namespace;


ImplDeDtor(NPLException)


ImplDeDtor(TypeError)


ImplDeDtor(ValueCategoryMismatch)


ImplDeDtor(ListTypeError)


ImplDeDtor(ListReductionFailure)


ImplDeDtor(InvalidSyntax)


ImplDeDtor(ParameterMismatch)


ArityMismatch::ArityMismatch(size_t e, size_t r, RecordLevel lv)
	: ParameterMismatch(
	ystdex::sfmt("Arity mismatch: expected %zu, received %zu.", e, r), lv),
	expected(e), received(r)
{}
ImplDeDtor(ArityMismatch)


BadIdentifier::BadIdentifier(const char* id, size_t n, RecordLevel lv)
	: InvalidSyntax(InitBadIdentifierExceptionString(id, n), lv),
	p_identifier(make_shared<string>(id))
{}
BadIdentifier::BadIdentifier(string_view id, size_t n, RecordLevel lv)
	: InvalidSyntax(InitBadIdentifierExceptionString(string(id), n), lv),
	p_identifier(make_shared<string>(id))
{}
ImplDeDtor(BadIdentifier)


ImplDeDtor(InvalidReference)


LexemeCategory
CategorizeBasicLexeme(string_view id) ynothrowv
{
	YAssertNonnull(id.data());

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
			&& id.find_first_not_of("+-") != string_view::npos)
			|| ystdex::isdigit(f);
	}
	return {};
}


string
TermToString(const TermNode& term)
{
	if(const auto p = TermToNamePtr(term))
		return *p;
	return YSLib::sfmt<string>("#<unknown{%zu}:%s>", term.size(),
		term.Value.type().name());
}

string
TermToStringWithReferenceMark(const TermNode& term, bool has_ref)
{
	auto term_str(TermToString(term));

	return has_ref ? "[*] " + std::move(term_str) : std::move(term_str);
}

TermTags
TermToTags(TermNode& term)
{
	return ystdex::call_value_or(ystdex::compose(GetLValueTagsOf,
		std::mem_fn(&TermReference::GetTags)),
		NPL::TryAccessLeaf<const TermReference>(term), term.Tags);
}

void
ThrowInsufficientTermsError(const TermNode& term, bool has_ref)
{
	throw ParameterMismatch(ystdex::sfmt(
		"Insufficient subterms found in '%s' for the list parameter.",
		TermToStringWithReferenceMark(term, has_ref).c_str()));
}

void
ThrowListTypeErrorForInvalidType(const ystdex::type_info& tp,
	const TermNode& term, bool has_ref)
{
	throw ListTypeError(ystdex::sfmt("Expected a value of type '%s', got a list"
		" '%s'.", tp.name(),
		TermToStringWithReferenceMark(term, has_ref).c_str()));
}

void
ThrowListTypeErrorForNonlist(const TermNode& term, bool has_ref)
{
	throw ListTypeError(ystdex::sfmt("Expected a list, got '%s'.",
		TermToStringWithReferenceMark(term, has_ref).c_str()));
}

void
ThrowTypeErrorForInvalidType(const ystdex::type_info& tp, const TermNode& term,
	bool has_ref)
{
	throw TypeError(ystdex::sfmt("Expected a value of type '%s', got '%s'.",
		tp.name(), TermToStringWithReferenceMark(term, has_ref).c_str()));
}

void
TokenizeTerm(TermNode& term)
{
	for(auto& child : term)
		TokenizeTerm(child);
	if(const auto p = NPL::AccessPtr<string>(term))
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
	if(const auto p = NPL::TryAccessLeaf<TermReference>(ref.get()))
	{
		ref.SetTags(MergeTermTags(ref.GetTags(), p->GetTags())),
		ref.SetReferent(p->get());
		return {std::move(ref), true};
	}
	return {std::move(ref), {}};
}

TermNode
PrepareCollapse(TermNode& term, const shared_ptr<Environment>& p_env)
{
	if(term.Value.type() == ystdex::type_id<const TermReference>())
		return term;
	return NPL::AsTermNode(term.get_allocator(),
		TermReference(p_env->MakeTermTags(term), term, NPL::Nonnull(p_env)));
}


bool
IsReferenceTerm(const TermNode& term)
{
	return bool(NPL::TryAccessLeaf<const TermReference>(term));
}

bool
IsBoundLValueTerm(const TermNode& term)
{
	return ystdex::invoke_value_or(&TermReference::IsReferencedLValue,
		NPL::TryAccessLeaf<const TermReference>(term));
}

bool
IsUncollapsedTerm(const TermNode& term)
{
	return ystdex::call_value_or(ystdex::compose(IsReferenceTerm,
		std::mem_fn(&TermReference::get)),
		NPL::TryAccessLeaf<const TermReference>(term));
}

bool
IsUniqueTerm(const TermNode& term)
{
	return ystdex::invoke_value_or(&TermReference::IsUnique,
		NPL::TryAccessLeaf<const TermReference>(term), true);
}

bool
IsModifiableTerm(const TermNode& term)
{
	return ystdex::invoke_value_or(&TermReference::IsModifiable,
		NPL::TryAccessLeaf<const TermReference>(term),
		!bool(term.Tags & TermTags::Nonmodifying));
}

bool
IsTemporaryTerm(const TermNode& term)
{
	return ystdex::invoke_value_or(&TermReference::IsTemporary,
		NPL::TryAccessLeaf<const TermReference>(term),
		bool(term.Tags & TermTags::Temporary));
}


void
LiftOtherOrCopy(TermNode& term, TermNode& tm, bool move)
{
	// XXX: Term tags are currently not respected in prvalues.
	if(move)
		LiftOther(term, tm);
	else
		// NOTE: Although %tm is required to be not same to %term, it can still
		//	be an object exclusively owned by %term, e.g. when %term represents
		//	a reference value with irregular representation and %tm is the term
		//	referenced uniquely by %term.Value. Explicit copy is necessary for
		//	such cases to avoid invalidate subterm reference prematurely.
		term.CopyContent(tm);
}

void
LiftTermOrCopy(TermNode& term, TermNode& tm, bool move)
{
	// XXX: Term tags are currently not respected in prvalues.
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
	if(const auto p = NPL::TryAccessLeaf<TermReference>(tm))
		term.SetContent(TermNode(std::move(tm.GetContainerRef()),
			Collapse(std::move(*p)).first));
	else
		LiftOther(term, tm);
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
			// XXX: Allocators are not used here for performance in most cases.
			term.Value = TermReference(TermTags::Unqualified, tm,
				EnvironmentReference());
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
	if(const auto p = NPL::TryAccessLeaf<const TermReference>(term))
		// XXX: Using %LiftMovedOther instead of %LiftMoved is safe, because the
		//	referent is not allowed to be same to %term in NPLA.
		LiftMovedOther(term, *p, p->IsMovable());
}

void
MoveRValueToForward(TermNode& term, TermNode& tm)
{
#if true
	MoveRValueFor(term, tm, &TermReference::IsModifiable);
#else
	// NOTE: For exposition only. The optimized implemenation shall be
	//	equivalent to this, except for the copy elision.
	LiftOther(term, tm);
	if(!IsBoundLValueTerm(term))
	{
		if(const auto p = NPL::TryAccessLeaf<const TermReference>(term))
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
	LiftOther(term, tm);
	if(!IsBoundLValueTerm(term))
		LiftToReturn(term);
#endif
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
	// NOTE: Cleanup if and only if necessary.
	if(status == ReductionStatus::Clean)
		term.ClearContainer();
	return status;
}


ReductionStatus
ReduceBranchToList(TermNode& term) ynothrowv
{
	RemoveHead(term);
	return ReductionStatus::Retained;
}

ReductionStatus
ReduceBranchToListValue(TermNode& term) ynothrowv
{
	RemoveHead(term);
	LiftSubtermsToReturn(term);
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
	if(const auto p = NPL::TryAccessLeaf<const TermReference>(tm))
	{
		// NOTE: Reference collapsed.
		// XXX: As %LiftOtherOrCopy, except that tags spcfified %p_ref are also
		//	Propagated.
		if(!p_ref)
			LiftOther(term, tm);
		else
		{
			// XXX: Both can be even more efficient than %LiftOtherOrCopy.
#if true
			// XXX: As %LiftTermOtherOrCopy.
			term.CopyContent(tm);

			auto& ref(term.Value.GetObject<TermReference>());

			ref.SetTags(PropagateTo(ref.GetTags(), p_ref->GetTags()));
#else
			term.CopyContainer(tm);
			term.Value = TermReference(PropagateTo(p->GetTags(),
				p_ref->GetTags()), p->get(),
				NPL::Deref(p_ref).GetEnvironmentReference());
#endif
		}
		// XXX: The resulted representation can be irregular.
		return ReductionStatus::Retained;
	}
	return ReduceToReferenceAt(term, tm, p_ref);
}

ReductionStatus
ReduceToReferenceAt(TermNode& term, TermNode& tm,
	ResolvedTermReferencePtr p_ref)
{
	// XXX: Term tags on prvalues are reserved and should be ignored normally
	//	except for future internal use. Since %term is a term, %Tags::Temporary
	//	is not expected, the %GetLValueTagsOf is also not used.
	// XXX: Allocators are not used here for performance in most cases.
	term.Value = TermReference(PropagateTo(tm.Tags, p_ref->GetTags()), tm,
		NPL::Deref(p_ref).GetEnvironmentReference());
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
	const auto& tp(vo.type());

	if(tp == ystdex::type_id<EnvironmentList>())
	{
		for(const auto& env : vo.GetObject<EnvironmentList>())
			CheckParent(env);
	}
	else if(YB_UNLIKELY(tp != ystdex::type_id<observer_ptr<const Environment>>()
		&& tp != ystdex::type_id<EnvironmentReference>()
		&& tp != ystdex::type_id<shared_ptr<Environment>>()))
		ThrowForInvalidType(tp);
#if NPL_NPLA_CheckParentEnvironment
	if(tp == ystdex::type_id<observer_ptr<const Environment>>())
	{
		if(YB_UNLIKELY(!vo.GetObject<observer_ptr<const Environment>>()))
			// NOTE: See %EnsureValid.
			ThrowForInvalidValue();
	}
	else if(tp == ystdex::type_id<EnvironmentReference>())
		EnsureValid(vo.GetObject<EnvironmentReference>().Lock());
	else if(tp == ystdex::type_id<shared_ptr<Environment>>())
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
	return NPL::make_observer(ystdex::call_value_or<BindingMap::mapped_type*>(
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
Environment::ThrowForInvalidType(const ystdex::type_info& tp)
{
	throw TypeError(
		ystdex::sfmt("Invalid environment type '%s' found.", tp.name()));
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
	// TODO: Add check to avoid stack overflow when the current action is
	//	called?
	YAssert(IsAlive(), "No tail action found.");
	TailAction = std::move(current.front());
	stashed.splice_after(stashed.cbefore_begin(), current,
		current.cbefore_begin());
	TryExpr(LastStatus = TailAction(*this))
	CatchExpr(..., HandleException(std::current_exception()))
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
	const auto p_obj(ystdex::retry_on_cond([&] YB_LAMBDA_ANNOTATE(
		(Environment::NameResolution::first_type p), , flatten)
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
				[&] YB_LAMBDA_ANNOTATE((), , flatten)
			// XXX: Ditto.
#if !(YB_IMPL_GNUCPP >= 90000)
				-> bool
#endif
			{
				const ValueObject& parent(cur);
				const auto& tp(parent.type());

				if(tp == ystdex::type_id<EnvironmentReference>())
				{
					p_redirected = RedirectToShared(id,
						parent.GetObject<EnvironmentReference>().Lock());
					p_env.swap(p_redirected);
				}
				else if(tp == ystdex::type_id<shared_ptr<Environment>>())
				{
					p_redirected = RedirectToShared(id,
						parent.GetObject<shared_ptr<Environment>>());
					p_env.swap(p_redirected);
				}
				else
				{
					observer_ptr<const ValueObject> p_next{};

					if(tp == ystdex::type_id<EnvironmentList>())
					{
						auto& envs(parent.GetObject<EnvironmentList>());

						p_next = RedirectEnvironmentList(envs.cbegin(),
							envs.cend(), id, cont);
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
		throw ListTypeError(
			ystdex::sfmt("Invalid environment formed from list '%s' found.",
			TermToStringWithReferenceMark(nd, has_ref).c_str()));
	}, term);
}
pair<shared_ptr<Environment>, bool>
ResolveEnvironment(TermNode& term)
{
	return ResolveTerm([&](TermNode& nd, ResolvedTermReferencePtr p_ref){
		if(!IsExtendedList(nd))
			return ResolveEnvironment(nd.Value, NPL::IsMovable(p_ref));
		throw ListTypeError(
			ystdex::sfmt("Invalid environment formed from list '%s' found.",
			TermToStringWithReferenceMark(nd, p_ref).c_str()));
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

