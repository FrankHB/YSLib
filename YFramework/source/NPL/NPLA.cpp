﻿/*
	© 2014-2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA.cpp
\ingroup NPL
\brief NPLA 公共接口。
\version r2622
\author FrankHB <frankhb1989@gmail.com>
\since build 663
\par 创建时间:
	2016-01-07 10:32:45 +0800
\par 修改时间:
	2019-07-23 23:25 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA
*/


#include "NPL/YModules.h"
#include YFM_NPL_NPLA // for YSLib, string, YSLib::DecodeIndex, std::to_string,
//	YSLib::make_string_view, std::invalid_argument, ValueNode, NPL::Access,
//	NPL::DerefEscapeLiteral, Literalize, NPL::AccessPtr, ystdex::value_or,
//	ystdex::write, bad_any_cast, std::allocator_arg, YSLib::NodeSequence,
//	NPL::Deref, ystdex::unimplemented, ystdex::type_id, ystdex::quote,
//	ystdex::call_value_or, ystdex::begins_with, YSLib::get_raw,
//	NPL::make_observer, ystdex::sfmt, NPL::TryAccessTerm, sfmt, GetLValueTagsOf,
//	std::mem_fn, ystdex::compose, ystdex::invoke_value_or, NPL::TryAccessLeaf,
//	NPL::IsMovable, ystdex::ref, YSLib::FilterExceptions, ystdex::retry_on_cond,
//	ystdex::type_info, pair, ystdex::addrof, ystdex::second_of;
#include YFM_NPL_SContext

using namespace YSLib;

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

	if(print_node_str(node))
		return;
	os << '\n';
	if(node)
		TraverseNodeChildAndPrint(os, node, [&]{
			PrintIndent(os, igen, depth);
		}, print_node_str, [&](const ValueNode& nd){
			return PrintNode(os, nd, node_to_str, igen, depth + 1);
		});
}

bool
PrintNodeString(std::ostream& os, const ValueNode& node,
	NodeToString node_to_str)
{
	TryRet(os << node_to_str(node) << '\n', true)
	CatchIgnore(bad_any_cast&)
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
		if(const auto p = NPL::AccessPtr<NodeSequence>(node))
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
		return NPL::Access<string>(NPL::Deref(term.begin()));
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
			if(!term.empty())
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
			ConvertDocumentNode(NPL::Deref(term.begin()), igen, depth), 1);
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

//! \since build 815
YB_NORETURN void
ThrowInvalidEnvironment()
{
	throw std::invalid_argument("Invalid environment record pointer found.");
}


bool
IsReserved(string_view id) ynothrowv
{
	YAssertNonnull(id.data());
	return ystdex::begins_with(id, "__");
}

observer_ptr<Environment>
RedirectToShared(string_view id, const shared_ptr<Environment>& p_shared)
{
	if(p_shared)
		return NPL::make_observer(YSLib::get_raw(p_shared));
	// TODO: Use concrete semantic failure exception.
	throw NPLException(ystdex::sfmt("Invalid reference found for%s name '%s',"
		" probably due to invalid context NPL::Access by dangling reference.",
		IsReserved(id) ? " reserved" : "", id.data()));
}

//! \since build 839
//@{
// XXX: Use other type without overhead of check on call of %operator()?
using Redirector = function<observer_ptr<const Environment>()>;

observer_ptr<const Environment>
RedirectParent(const ValueObject&, string_view, Redirector&);

observer_ptr<const Environment>
RedirectEnvironmentList(EnvironmentList::const_iterator first,
	EnvironmentList::const_iterator last, string_view id, Redirector& cont)
{
	if(first != last)
	{
		cont = std::bind([=, &cont](EnvironmentList::const_iterator i,
			Redirector& c){
			cont = std::move(c);
			return RedirectEnvironmentList(i, last, id, cont);
		}, std::next(first), std::move(cont));
		if(const auto p = RedirectParent(*first, id, cont))
			return p;
	}
	return {};
}

observer_ptr<const Environment>
RedirectParent(const ValueObject& parent, string_view id,
	Redirector& cont)
{
	const auto& tp(parent.type());

	if(tp == ystdex::type_id<EnvironmentList>())
	{
		auto& envs(parent.GetObject<EnvironmentList>());

		return RedirectEnvironmentList(envs.cbegin(), envs.cend(), id, cont);
	}
	if(tp == ystdex::type_id<observer_ptr<const Environment>>())
		return parent.GetObject<observer_ptr<const Environment>>();
	if(tp == ystdex::type_id<EnvironmentReference>())
		return RedirectToShared(id,
			parent.GetObject<EnvironmentReference>().Lock());
	if(tp == ystdex::type_id<shared_ptr<Environment>>())
		return RedirectToShared(id,
			parent.GetObject<shared_ptr<Environment>>());
	return {};
}
//@}

//! \since build 857
TermTags
MergeTermTags(TermTags x, TermTags y) ynothrow
{
	return (x | y) & ~(TermTags::Unique | TermTags::Temporary)
		& (x & y & TermTags::Unique);
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


observer_ptr<const TokenValue>
TermToNamePtr(const TermNode& term)
{
	return NPL::TryAccessTerm<TokenValue>(term);
}

string
TermToString(const TermNode& term)
{
	if(const auto p = TermToNamePtr(term))
		return *p;
	return sfmt<string>("#<unknown{%zu}:%s>", term.size(),
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
ThrowListTypeErrorForInvalidType(const ystdex::type_info& tp,
	const TermNode& term, bool is_ref)
{
	throw ListTypeError(ystdex::sfmt("Expected a value of type '%s', got a list"
		" '%s'.", tp.name(),
		TermToStringWithReferenceMark(term, is_ref).c_str()));
}

void
TokenizeTerm(TermNode& term)
{
	for(auto& child : term)
		TokenizeTerm(child);
	if(const auto p = NPL::AccessPtr<string>(term))
		term.Value.emplace<TokenValue>(std::move(*p));
}


pair<TermReference, bool>
Collapse(TermReference ref)
{
	if(const auto p = NPL::TryAccessLeaf<const TermReference>(ref.get()))
		return {{MergeTermTags(ref.GetTags(), p->GetTags()), p->get(),
			ref.GetAnchorPtr()}, true};
	return {std::move(ref), {}};
}

TermReference
PrepareCollapse(TermNode& term, const AnchorPtr& p_anchor)
{
	if(const auto p = NPL::TryAccessLeaf<const TermReference>(term))
		return *p;
	return {term.Tags, term, p_anchor};
}

TermNode&
ReferenceTerm(TermNode& term) ynothrow
{
	return ystdex::invoke_value_or(&TermReference::get,
		NPL::TryAccessLeaf<const TermReference>(term), term);
}
const TermNode&
ReferenceTerm(const TermNode& term) ynothrow
{
	return ystdex::invoke_value_or(&TermReference::get,
		NPL::TryAccessLeaf<const TermReference>(term), term);
}


bool
IsReferenceTerm(const TermNode& term)
{
	return bool(NPL::TryAccessLeaf<const TermReference>(term));
}

bool
IsLValueTerm(const TermNode& term)
{
	return ystdex::invoke_value_or(&TermReference::IsReferencedLValue,
		NPL::TryAccessLeaf<const TermReference>(term));
}

bool
IsUniqueTerm(const TermNode& term)
{
	return ystdex::invoke_value_or(&TermReference::IsUnique,
		NPL::TryAccessLeaf<const TermReference>(term), true);
}


bool
CheckReducible(ReductionStatus status)
{
	if(status == ReductionStatus::Clean || status == ReductionStatus::Retained)
		return {};
#if NPL_Impl_NPLA_CheckStatus
	// NOTE: Keep away assertions so client code can be diagnosed.
	if(YB_UNLIKELY(status != ReductionStatus::Partial
		&& status != ReductionStatus::Retrying))
		YTraceDe(Warning, "Unexpected status found.");
#endif
	return true;
}

ReductionStatus
RegularizeTerm(TermNode& term, ReductionStatus status) ynothrow
{
	// NOTE: Cleanup if and only if necessary.
	if(status == ReductionStatus::Clean)
		term.ClearContainer();
	return status;
}


void
LiftTermOrCopy(TermNode& term, TermNode& tm, bool move)
{
	if(move)
		LiftTerm(term, tm);
	else
		term.SetContent(tm);
}


void
LiftCollapsed(TermNode& term, TermNode& tm, TermReference ref)
{
	auto pr(Collapse(std::move(ref)));

	if(!ystdex::ref_eq<>()(term, tm))
		term.MoveContent(TermNode(std::move(tm.GetContainerRef()),
			std::move(pr.first)));
	else if(pr.second)
		term.Value = std::move(pr.first);
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
			term.Value = TermReference(TermTags::Unqualified, tm);
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
	// TODO: Detect lifetime escape to perform copy elision?
	// NOTE: Only outermost one level is referenced.
	if(const auto p = NPL::TryAccessLeaf<const TermReference>(term))
		NPL::SetContentWith(term, p->get(), NPL::IsMovable(*p)
			? &ValueObject::MakeMove : &ValueObject::MakeCopy);
	// NOTE: On the other hand, the references captured by vau handlers (which
	//	requries recursive copy of vau handler members if forced) are not
	//	blessed here to avoid leaking abstraction of detailed implementation
	//	of vau handlers; it can be checked by the vau handler itself, if
	//	necessary.
}

void
LiftRValueToReturn(TermNode& term)
{
	if(!IsLValueTerm(term))
		LiftToReturn(term);
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
	if(term.size() > 1 && IsEmpty(NPL::Deref(term.begin())))
		RemoveHead(term);
	return ReductionStatus::Clean;
}


#if NPL_NPLA_CheckEnvironmentReferenceCount
Environment::~Environment()
{
	if(const auto& p_anchor = anchor.Ptr)
	{
		const auto acnt(p_anchor.use_count());

		if(acnt >= 1)
		{
			// XXX: Assume this would not wrap.
			const auto ecnt(AnchorValue::Access(anchor.Ptr).env_count + 1);

			if(ecnt < size_t(acnt))
				YSLib::FilterExceptions([this, acnt, ecnt]{
					const size_t n(Bindings.size());
					size_t i(0);
					// XXX: Allocator type is not available for %string yet.
					string str;
					// XXX: The value is heuristic for common cases.
					str.reserve(n * yimpl(4));

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

Environment::NameResolution
Environment::DefaultResolve(const Environment& e, string_view id)
{
	NameResolution::first_type p;
	auto env_ref(ystdex::ref<const Environment>(e));
	Redirector cont;

	ystdex::retry_on_cond([&](observer_ptr<const Environment> p_env) ynothrow{
		if(p_env)
			env_ref = ystdex::ref(NPL::Deref(p_env));
		return p_env || cont;
	}, [&, id]() -> observer_ptr<const Environment>{
		auto& env(env_ref.get());

		p = env.LookupName(id);
		if(p)
			cont = Redirector();
		else
		{
			const auto p_env(RedirectParent(env.Parent, id, cont));

			// NOTE: This is like asynchronously called %ContextNode::ApplyTail.
			return p_env ? p_env
				: (cont ? ystdex::exchange(cont, Redirector())() : nullptr);
		}
		return {};
	});
	return {p, env_ref};
}

void
Environment::Define(string_view id, ValueObject&& vo, bool forced)
{
	YAssertNonnull(id.data());
	if(forced)
		// XXX: Self overwriting is possible.
		swap((*this)[id].Value, vo);
	else if(!AddValue(id, std::move(vo)))
		throw BadIdentifier(id, 2);
}

Environment::NameResolution::first_type
Environment::LookupName(string_view id) const
{
	YAssertNonnull(id.data());
	return NPL::make_observer(ystdex::call_value_or<BindingMap::mapped_type*>(
		ystdex::compose(ystdex::addrof<>(), ystdex::second_of<>()),
		Bindings.find(id), {}, Bindings.cend()));
}

void
Environment::Redefine(string_view id, ValueObject&& vo, bool forced)
{
	if(const auto p = LookupName(id))
		swap(p->Value, vo);
	else if(!forced)
		throw BadIdentifier(id, 0);
}

bool
Environment::Remove(string_view id, bool forced)
{
	YAssertNonnull(id.data());
	// XXX: %BindingMap does not have transparent key %erase. This is like
	//	%std::set.
	if(ystdex::erase_first(Bindings, id))
		return true;
	if(forced)
		return {};
	throw BadIdentifier(id, 0);
}

void
Environment::ThrowForInvalidType(const ystdex::type_info& tp)
{
	throw TypeError(ystdex::sfmt("Invalid environment type '%s' found.",
		tp.name()));
}


EnvironmentReference::EnvironmentReference(const shared_ptr<Environment>& p_env)
	// TODO: Blocked. Use C++17 %weak_from_this and throw-expression?
	: EnvironmentReference(p_env, p_env ? p_env->GetAnchorPtr() : nullptr)
{}
#if NPL_NPLA_CheckEnvironmentReferenceCount
EnvironmentReference::~EnvironmentReference()
{
	if(p_anchor)
		Environment::AnchorValue::Access(p_anchor).RemoveReference();
}

void
EnvironmentReference::ReferenceEnvironmentAnchor()
{
	if(p_anchor)
		Environment::AnchorValue::Access(p_anchor).AddReference();
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
		ThrowInvalidEnvironment();
	}()),
	Trace(ctx.Trace)
{}
ContextNode::ContextNode(ContextNode&& ctx) ynothrow
	: ContextNode(ctx.memory_rsrc)
{
	swap(ctx, *this);
}
ContextNode::ImplDeDtor(ContextNode)

ReductionStatus
ContextNode::ApplyTail()
{
	// TODO: Avoid stack overflow when current action is called.
	YAssert(bool(Current), "No tail action found.");
	return LastStatus = Switch()();
}

void
ContextNode::Pop() ynothrow
{
	YAssert(!Delimited.empty(), "No continuation is delimited.");
	SetupTail(std::move(Delimited.front()));
	Delimited.pop_front();
}

void
ContextNode::Push(const Reducer& reducer)
{
	YAssert(Current, "No continuation can be captured.");
	Delimited.push_front(reducer);
	std::swap(Current, Delimited.front());
}
void
ContextNode::Push(Reducer&& reducer)
{
	YAssert(Current, "No continuation can be captured.");
	Delimited.push_front(std::move(reducer));
	std::swap(Current, Delimited.front());
}

ReductionStatus
ContextNode::Rewrite(Reducer reduce)
{
	SetupTail(std::move(reduce));
	// NOTE: Rewriting loop until no actions remain.
	return ystdex::retry_on_cond(std::bind(&ContextNode::Transit, this), [&]{
		return ApplyTail();
	});
}

shared_ptr<Environment>
ContextNode::SwitchEnvironment(shared_ptr<Environment> p_env)
{
	if(p_env)
		return SwitchEnvironmentUnchecked(std::move(p_env));
	ThrowInvalidEnvironment();
}

shared_ptr<Environment>
ContextNode::SwitchEnvironmentUnchecked(shared_ptr<Environment> p_env) ynothrowv
{
	YAssertNonnull(p_env);
	return ystdex::exchange(p_record, p_env);
}

bool
ContextNode::Transit() ynothrow
{
	if(!Current)
	{
		if(!Delimited.empty())
			Pop();
		else
			return {};
	}
	return true;
}

void
swap(ContextNode& x, ContextNode& y) ynothrow 
{
	swap(x.p_record, y.p_record),
	swap(x.Delimited, y.Delimited),
	swap(x.Current, y.Current),
	std::swap(x.LastStatus, y.LastStatus),
	swap(x.Trace, y.Trace);
}


Environment::NameResolution
ResolveName(const ContextNode& ctx, string_view id)
{
	YAssertNonnull(id.data());
	return ctx.GetRecordRef().Resolve(id);
}

TermReference
ResolveIdentifier(const ContextNode& ctx, string_view id)
{
	auto pr(ResolveName(ctx, id));

	if(pr.first)
		return PrepareCollapse(*pr.first, pr.second.get().GetAnchorPtr());
	throw BadIdentifier(id);
}

pair<shared_ptr<Environment>, bool>
ResolveEnvironment(const ValueObject& vo)
{
	if(const auto p = vo.AccessPtr<const EnvironmentReference>())
		return {p->Lock(), {}};
	if(const auto p = vo.AccessPtr<const shared_ptr<Environment>>())
		return {*p, true};
	// TODO: Merge with %Environment::CheckParent?
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

} // namespace NPL;

