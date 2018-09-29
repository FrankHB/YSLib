﻿/*
	© 2014-2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA.cpp
\ingroup NPL
\brief NPLA 公共接口。
\version r2022
\author FrankHB <frankhb1989@gmail.com>
\since build 663
\par 创建时间:
	2016-01-07 10:32:45 +0800
\par 修改时间:
	2018-09-25 08:31 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA
*/


#include "NPL/YModules.h"
#include YFM_NPL_NPLA // for ystdex::value_or, ystdex::write,
//	ystdex::bad_any_cast, ystdex::unimplemented, ystdex::type_id, ystdex::quote,
//	ystdex::call_value_or, ystdex::begins_with, ystdex::sfmt, ystdex::ref,
//	ystdex::retry_on_cond, ystdex::type_info, pair;
#include YFM_NPL_SContext

using namespace YSLib;

namespace NPL
{

ValueNode
MapNPLALeafNode(const TermNode& term)
{
	return AsNode(string(),
		string(Deliteralize(ParseNPLANodeString(MapToValueNode(term)))));
}

ValueNode
TransformToSyntaxNode(const ValueNode& node, const string& name)
{
	ValueNode::Container con{AsIndexNode(size_t(), node.GetName())};
	const auto nested_call([&](const ValueNode& nd){
		con.emplace(TransformToSyntaxNode(nd, MakeIndex(con)));
	});

	if(node.empty())
	{
		if(const auto p = AccessPtr<NodeSequence>(node))
			for(auto& nd : *p)
				nested_call(nd);
		else
			con.emplace(NoContainer, MakeIndex(1),
				Literalize(ParseNPLANodeString(node)));
	}
	else
		for(auto& nd : node)
			nested_call(nd);
	return {std::move(con), name};
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
	os << EscapeLiteral(node.GetName()) << ' ';
	if(PrintNodeString(os, node, node_to_str))
		return;
	os << '\n';
	if(node)
		TraverseNodeChildAndPrint(os, node, [&]{
			PrintIndent(os, igen, depth);
		}, [&](const ValueNode& nd){
			return PrintNodeString(os, nd, node_to_str);
		}, [&](const ValueNode& nd){
			return PrintNode(os, nd, node_to_str, igen, depth + 1);
		});
}

bool
PrintNodeString(std::ostream& os, const ValueNode& node,
	NodeToString node_to_str)
{
	TryRet(os << node_to_str(node) << '\n', true)
	CatchIgnore(ystdex::bad_any_cast&)
	return {};
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
			const auto& n(Access<string>(Deref(i)));

			return n + '=' + Access<string>(Deref(++i));
		}
		YB_ATTR_fallthrough;
	case 1:
		return Access<string>(Deref(term.begin()));
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

			const auto& con(term.GetContainer());

			if(!con.empty())
				try
				{
					auto i(con.cbegin());
					const auto& str(Access<string>(Deref(i)));

					++i;
					if(str == "@")
					{
						for(; i != con.cend(); ++i)
							res += ' ' + ConvertAttributeNodeString(Deref(i));
						return res;
					}
					if(opt == ParseOption::Attribute)
						throw LoggedEvent("Invalid non-attribute term found.");
					if(str == "*PI*")
					{
						res = "<?";
						for(; i != con.cend(); ++i)
							res += string(Deliteralize(ConvertDocumentNode(
								Deref(i), igen, depth, ParseOption::String)))
								+ ' ';
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
						if(i != con.end())
						{
							if(!Deref(i).empty()
								&& (i->begin())->Value == string("@"))
							{
								head += string(
									Deliteralize(ConvertDocumentNode(*i, igen,
									depth, ParseOption::Attribute)));
								if(++i == con.cend())
									return head + " />";
							}
							head += '>';
						}
						else
							return head + " />";
						for(; i != con.cend(); ++i)
						{
							nl = Deref(i).Value.type()
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
				CatchExpr(ystdex::bad_any_cast& e, YTraceDe(Warning,
					"Conversion failed: <%s> to <%s>.", e.from(), e.to()))
		}
		return res;
	}
	throw LoggedEvent("Empty term found.", Warning);
}

string
ConvertStringNode(const TermNode& term)
{
	return ystdex::call_value_or(EscapeXML, AccessPtr<string>(term));
}

void
PrintSyntaxNode(std::ostream& os, const TermNode& term, IndentGenerator igen,
	size_t depth)
{
	if(IsBranch(term))
		ystdex::write(os,
			ConvertDocumentNode(Deref(term.begin()), igen, depth), 1);
	os << std::flush;
}


ValueNode
MakeXMLDecl(const string& name, const string& ver, const string& enc,
	const string& sd)
{
	auto decl("version=\"" + ver + '"');

	if(!enc.empty())
		decl += " encoding=\"" + enc + '"';
	if(!sd.empty())
		decl += " standalone=\"" + sd + '"';
	return AsNodeLiteral(name, {{MakeIndex(0), "*PI*"}, {MakeIndex(1), "xml"},
		{MakeIndex(2), decl + ' '}});
}

ValueNode
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
		return make_observer(get_raw(p_shared));
	// TODO: Use concrete semantic failure exception.
	throw NPLException(ystdex::sfmt("Invalid reference found for%s name '%s',"
		" probably due to invalid context access by dangling reference.",
		IsReserved(id) ? " reserved" : "", id.data()));
}

//! \since build 839
//@{
// XXX: Use other type without overhead of check on call of %operator()?
using Redirector = std::function<observer_ptr<const Environment>()>;

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
	return AccessPtr<TokenValue>(term);
}

string
TermToString(const TermNode& term)
{
	if(const auto p = TermToNamePtr(term))
		return *p;
	return sfmt("#<unknown{%zu}:%s>", term.size(), term.Value.type().name());
}

void
TokenizeTerm(TermNode& term)
{
	for(auto& child : term)
		TokenizeTerm(child);
	if(const auto p = AccessPtr<string>(term))
		term.Value.emplace<TokenValue>(std::move(*p));
}


bool
IsReferenceTerm(const TermNode& term) ynothrow
{
	return term.Value.type() == ystdex::type_id<TermReference>();
}

bool
IsLValueTerm(const TermNode& term) ynothrow
{
	return ystdex::call_value_or(std::mem_fn(&TermReference::IsTermReferenced),
		AccessPtr<const TermReference>(term));
}

TermNode&
ReferenceTerm(TermNode& term)
{
	return ystdex::call_value_or(
		[&](const TermReference& term_ref) ynothrow -> TermNode&{
		return term_ref.get();
	}, AccessPtr<const TermReference>(term), term);
}
const TermNode&
ReferenceTerm(const TermNode& term)
{
	return ystdex::call_value_or(
		[&](const TermReference& term_ref) ynothrow -> const TermNode&{
		return term_ref.get();
	}, AccessPtr<TermReference>(term), term);
}


bool
CheckReducible(ReductionStatus status)
{
	if(status == ReductionStatus::Clean || status == ReductionStatus::Retained)
		return {};
	if(YB_UNLIKELY(status != ReductionStatus::Retrying))
		YTraceDe(Warning, "Unexpected status found.");
	return true;
}

ReductionStatus
RegularizeTerm(TermNode& term, ReductionStatus res)
{
	// NOTE: Cleanup if and only if necessary.
	if(res == ReductionStatus::Clean)
		term.ClearContainer();
	return res;
}


bool
LiftTermOnRef(TermNode& term, TermNode& tm)
{
	if(const auto p = AccessPtr<const TermReference>(tm))
	{
		LiftTermRef(term, p->get());
		return true;
	}
	return {};
}

void
LiftToReference(TermNode& term, TermNode& tm)
{
	if(tm)
	{
		if(!LiftTermOnRef(term, tm))
		{
			if(!tm.Value.OwnsUnique())
				LiftTerm(term, tm);
			else
				throw InvalidReference(
					"Value of a temporary shall not be referenced.");
		}
	}
	else
		ystdex::throw_invalid_construction();
}

void
LiftToReturn(TermNode& term)
{
	// TODO: Detect lifetime escape to perform copy elision?
	// NOTE: Only outermost one level is referenced.
	LiftTermRefToSelf(term);
	// NOTE: To keep lifetime of objects referenced by references introduced in
	//	%EvaluateIdentifier sane, %ValueObject::MakeMoveCopy is not enough
	//	because it will not copy objects referenced in holders of
	//	%YSLib::RefHolder instances). On the other hand, the references captured
	//	by vau handlers (which requries recursive copy of vau handler members if
	//	forced) are not blessed here to avoid leaking abstraction of detailed
	//	implementation of vau handlers; it can be checked by the vau handler
	//	itself, if necessary.
	LiftTermIndirection(term);
}

void
LiftToSelf(TermNode& term)
{
	// NOTE: The order is significant.
	LiftTermRefToSelf(term);
	for(auto& child : term)
		LiftToSelf(child);
}

void
LiftToSelfSafe(TermNode& term)
{
	LiftToSelf(term);
	LiftTermIndirection(term);
}

void
LiftToOther(TermNode& term, TermNode& tm)
{
	LiftTermRefToSelf(tm);
	LiftTerm(term, tm);
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
	if(term.size() > 1 && IsEmpty(Deref(term.begin())))
		RemoveHead(term);
	return ReductionStatus::Clean;
}

ReductionStatus
ReduceToList(TermNode& term) ynothrow
{
	return IsBranch(term) ? ReduceBranchToList(term) : ReductionStatus::Clean;
}

ReductionStatus
ReduceToListValue(TermNode& term) ynothrow
{
	return IsBranch(term) ? ReduceBranchToListValue(term)
		: ReductionStatus::Clean;
}


ReductionStatus
ReduceForClosureResult(TermNode& term, const ContextNode& ctx)
{
	RegularizeTerm(term, ctx.LastStatus);
	LiftToReturn(term);
	return CheckNorm(term);
}


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
		dst.Remove(binding.GetName());
	// NOTE: If the resulted parent environment is empty, it is safe to be
	//	removed.
	return dst.empty();
}

Environment::NameResolution
Environment::DefaultResolve(const Environment& e, string_view id)
{
	observer_ptr<ValueNode> p;
	auto env_ref(ystdex::ref<const Environment>(e));
	Redirector cont;

	ystdex::retry_on_cond([&](observer_ptr<const Environment> p_env) ynothrow{
		if(p_env)
			env_ref = ystdex::ref(Deref(p_env));
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
		swap(Bindings[id].Value, vo);
	else if(!Bindings.AddValue(id, std::move(vo)))
		throw BadIdentifier(id, 2);
}

observer_ptr<ValueNode>
Environment::LookupName(string_view id) const
{
	YAssertNonnull(id.data());
	return AccessNodePtr(Bindings, id);
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
	if(Bindings.Remove(id))
		return true;
	if(forced)
		return {};
	throw BadIdentifier(id, 0);
}

void
Environment::ThrowForInvalidType(const ystdex::type_info& tp)
{
	throw NPLException(ystdex::sfmt("Invalid environment type '%s' found.",
		tp.name()));
}


EnvironmentReference::EnvironmentReference(const shared_ptr<Environment>& p_env)
	// TODO: Blocked. Use C++1z %weak_from_this and throw-expression?
	: EnvironmentReference(p_env, p_env ? p_env->Anchor() : nullptr)
{}


pair<TermReference, bool>
Collapse(TermNode& node)
{
	if(const auto p_tref = AccessPtr<const TermReference>(node))
		return {*p_tref, true};
	return {node, {}};
}
pair<TermReference, bool>
Collapse(TermNode& node, const Environment& env)
{
	if(const auto p_tref = AccessPtr<const TermReference>(node))
		return {*p_tref, true};
	return {{node, env.Anchor()}, {}};
}


ContextNode::ContextNode(const ContextNode& ctx,
	shared_ptr<Environment>&& p_rec)
	: p_record([&]{
		if(p_rec)
			return std::move(p_rec);
		ThrowInvalidEnvironment();
	}()),
	EvaluateLeaf(ctx.EvaluateLeaf), EvaluateList(ctx.EvaluateList),
	EvaluateLiteral(ctx.EvaluateLiteral), Trace(ctx.Trace)
{}
ContextNode::ContextNode(ContextNode&& ctx) ynothrow
	: ContextNode()
{
	swap(ctx, *this);
}

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

ReductionStatus
ContextNode::RewriteGuarded(TermNode& term, Reducer reduce)
{
	const auto gd(Guard(term, *this));

	return Rewrite(reduce);
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


Environment::NameResolution
ResolveName(const ContextNode& ctx, string_view id)
{
	YAssertNonnull(id.data());
	return ctx.GetRecordRef().Resolve(id);
}

pair<TermReference, bool>
ResolveIdentifier(const ContextNode& ctx, string_view id)
{
	auto pr(ResolveName(ctx, id));

	if(pr.first)
		return Collapse(*pr.first, pr.second);
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



Reducer
ComposeActions(ContextNode& ctx, Reducer&& cur, Reducer&& next)
{
	// NOTE: Lambda is not used to avoid unspecified destruction order of
	//	captured component and better performance (compared to the case of
	//	%pair used to keep the order).
	struct Action final
	{
		lref<ContextNode> Context;
		// NOTE: The destruction order of captured component is significant.
		//! \since build 821
		//@{
		mutable Reducer Next;
		Reducer Current;

		Action(ContextNode& ctx, Reducer& cur, Reducer& next)
			: Context(ctx), Next(std::move(next)), Current(std::move(cur))
		{}
		//@}
		// XXX: Copy is not intended used directly, but for well-formness.
		DefDeCopyMoveCtor(Action)

		DefDeMoveAssignment(Action)

		ReductionStatus
		operator()() const
		{
			RelaySwitched(Context, std::move(Next));
			return Current();
		}
	};

	return cur ? Action(ctx, cur, next) : std::move(next);
}

ReductionStatus
RelayNext(ContextNode& ctx, Reducer&& cur, Reducer&& next)
{
	ctx.SetupTail(ComposeActions(ctx, std::move(cur), std::move(next)));
	return ReductionStatus::Retrying;
}

} // namespace NPL;

