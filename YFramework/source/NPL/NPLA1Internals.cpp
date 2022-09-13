/*
	© 2018-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA1Internals.cpp
\ingroup NPL
\brief NPLA1 内部接口。
\version r20663
\author FrankHB <frankhb1989@gmail.com>
\since build 473
\par 创建时间:
	2020-02-15 13:20:08 +0800
\par 修改时间:
	2022-09-05 22:12 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	NPL::NPLA1Internals
*/


#include "NPL/YModules.h"
#include "NPLA1Internals.h" // for NPL::Deref, Environment, ystdex::dismiss,
//	shared_ptr, std::make_move_iterator, NPL::get, ActiveEnvironmentPtr,
//	std::throw_with_nested, ParameterMismatch, std::allocator_arg,
//	NPL::AsTermNode;

namespace NPL
{

namespace A1
{

inline namespace Internals
{

#if NPL_Impl_NPLA1_Enable_Thunked
#	if NPL_Impl_NPLA1_Enable_TCO
void
RecordCompressor::Compress()
{
	// NOTE: This is need to keep the root as external reference.
	const auto p_root(NPL::Nonnull(RootPtr.lock()));
	const auto a(NPL::Deref(p_root).Bindings.get_allocator());

	// NOTE: Trace.
	for(auto& pr : Universe)
	{
		auto& e(pr.first.get());

		Traverse(e, e.Parent, [this](const shared_ptr<Environment>& p_dst){
			auto& count(Universe.at(NPL::Deref(p_dst)));

			YAssert(count > 0, "Invalid count found in trace record.");
			--count;
			return false;
		});
	}
	for(auto i(Universe.cbegin()); i != Universe.cend(); )
		if(i->second > 0)
		{
			// TODO: Blocked. Use ISO C++17 container node API for efficient
			//	implementation.
			NewlyReachable.insert(i->first);
			i = Universe.erase(i);
		}
		else
			++i;
	for(ReferenceSet rs(a); !NewlyReachable.empty();
		NewlyReachable = std::move(rs))
	{
		for(const auto& e : NewlyReachable)
			Traverse(e, e.get().Parent,
				[&](const shared_ptr<Environment>& p_dst) ynothrowv{
				auto& dst(NPL::Deref(p_dst));

				rs.insert(dst);
				Universe.erase(dst);
				return false;
			});
		Reachable.insert(std::make_move_iterator(NewlyReachable.begin()),
			std::make_move_iterator(NewlyReachable.end()));
		for(auto i(rs.cbegin()); i != rs.cend(); )
			if(ystdex::exists(Reachable, *i))
				i = rs.erase(i);
			else
				++i;
	}

	// XXX: Need full support of PTC by direct DFS traverse?
	ReferenceSet accessed(a);

	ystdex::retry_on_cond(ystdex::id<>(), [&]() -> bool{
		bool collected = {};

		Traverse(*p_root, p_root->Parent, [&](const shared_ptr<Environment>&
			p_dst, Environment& src, ValueObject& parent) -> bool{
			auto& dst(NPL::Deref(p_dst));

			if(accessed.insert(src).second)
			{
				if(!ystdex::exists(Universe, ystdex::ref(dst)))
					return true;
				// NOTE: Variable %parent can be a single parent in a list
				//	of parent environments not equal to %src.Parent.
				// XXX: The envionment would be finally collected after the last
				//	reference is destroyed. The order of destruction of
				//	environments is unspecified but determined by the parentage.
				parent = dst.Parent;
				collected = true;
			}
			return {};
		});
		return collected;
	});
}


ReductionStatus
TCOAction::operator()(ContextNode& ctx) const
{
	YAssert(ystdex::ref_eq<>()(env_guard.func.Context.get(), ctx),
		"Invalid context found.");

	// NOTE: Many orders are siginificant, see %Documentation::NPL. For example,
	//	lifting (if any) shall be performed before release of guards (see also
	//	$2018-02 @ %Documentation::Workflow). The comments below only specify
	//	the implementation-dependent ones.
	// NOTE: If this is called properly, %ctx.LastStatus should be maintained to
	//	refer to the reduction status of the right term by
	//	%PushedAction::operator() in NPLA1.cpp.
	const auto res([&]() -> ReductionStatus{
		// NOTE: This implies the call of %RegularizeTerm before lifting.
		// XXX: Since the call of %RegularizeTerm is idempotent without term
		//	modification before the next reduction of term if it does not
		//	represent an uncollapsed reference value, there is no need to call
		//	%RegularizeTerm if the lift is not needed. However, this is not
		//	easily provable, so leave it as-is.
		// XXX: When %req_lift_result is 0, this is only needed on a real call
		//	from the evaluation is reentered. However, whether here is the
		//	reentrant call is not easily provable.
		RegularizeTerm(GetTermRef(), ctx.LastStatus);
		if(req_lift_result != 0)
		{
			for(; req_lift_result != 0; --req_lift_result)
				LiftToReturn(GetTermRef());
			return ReductionStatus::Retained;
		}
		// TODO: Anything necessary to prepare for invocation of first-class
		//	continuations?
		return ctx.LastStatus;
	}());

	ystdex::dismiss(term_guard);
	// XXX: The following operations are fine, but since there are no other
	//	user-defined operations to be invoked in %TCOAction before them, it
	//	should be more efficient to just delay them to the action rewriting (of
	//	%ContextNode::TailAction) in %ContextNode::ApplyTail.
#if false
	{
		const auto egd(std::move(env_guard));
	}
	record_list.clear();
#endif
	return res;
}

void
TCOAction::CompressFrameList()
{
	ystdex::retry_on_cond(ystdex::id<>(), [&]() -> bool{
		bool removed = {};

		// NOTE: The following code searches the frames to be removed, in the
		//	order from new to old. After merging, the guard slot %env_guard owns
		//	the resources of the expression (and its enclosed subexpressions)
		//	being TCO'd.
		// TODO: Use %FrameRecordList implementation having %remove_if with a
		//	non-void return type compatible to ISO C++20?
		record_list.remove_if([&](const FrameRecord& r) ynothrowv -> bool{
			const auto& p_frame_env_ref(NPL::get<ActiveEnvironmentPtr>(r));

			// NOTE: The whole frame is to be removed. The function prvalue is
			//	expected to live only in the subexpression evaluation, whose
			//	owned static environments (if any) share nothing with the
			//	environment (which is a local environment not owning the static
			//	environments) in the same record entry, so the whole entry is
			//	safe to be removed when the count is not 1. This has equivalent
			//	effects of evlis tail recursion.
			if(p_frame_env_ref.use_count() != 1
				// XXX: %NPL::Deref is safe because this can be null only when
				//	the use count is 0.
				|| NPL::Deref(p_frame_env_ref).IsOrphan())
			{
				removed = true;
				return true;
			}
			return {};
		});
		return removed;
	});
}

TCOAction&
EnsureTCOAction(ContextNode& ctx, TermNode& term)
{
	auto p_act(AccessTCOAction(ctx));

	if(YB_UNLIKELY(!p_act))
	{
		SetupTailTCOAction(ctx, term, {});
		p_act = AccessTCOActionUnchecked(ctx);
	}
	return NPL::Deref(p_act);
}
#	endif
#endif


void
ThrowNestedParameterTreeMismatch()
{
	std::throw_with_nested(ParameterMismatch("Failed initializing the operand"
		" in a parameter tree (expected a list, a symbol or '#ignore')."));
}

char
ExtractSigil(string_view& id)
{
	if(!id.empty())
	{
		char sigil(id.front());

		if(sigil != '&' && sigil != '%' && sigil != '@')
			sigil = char();
		else
			id.remove_prefix(1);
		return sigil;
	}
	return char();
}


ReductionStatus
ReduceAsSubobjectReference(TermNode& term, shared_ptr<TermNode> p_sub,
	const EnvironmentReference& r_env, TermTags tags)
{
	YAssert(p_sub, "Invalid subterm to form a subobject reference found.");

	// NOTE: Irregular representation is constructed for the subobject
	//	reference.
	auto& con(term.GetContainerRef());
	auto i(con.begin());

	// XXX: Set %Value first. Although this is not strongly exception-safe,
	//	it guarantees no unexpected copies of user-defined objects remained even
	//	if the following operations exit via exception. The order of setting of
	//	%Tags is insignificant, though.
	term.SetValue(TermReference(tags, NPL::Deref(p_sub), r_env)),
	term.Tags = TermTags::Unqualified;
	// XXX: Not using %ystdex::prefix_eraser because there is known 1 subterm to
	//	be inserted.
	con.insert(i,
		A1::MakeSubobjectReferent(con.get_allocator(), std::move(p_sub)));		
	con.erase(i, con.end());
	return ReductionStatus::Retained;
}

ReductionStatus
ReduceForCombinerRef(TermNode& term, const TermReference& ref,
	const ContextHandler& h, size_t n)
{
	// NOTE: Irregular representation is constructed for the combiner subobject
	//	reference.
	const auto& r_env(ref.GetEnvironmentReference());
	const auto a(term.get_allocator());

	// XXX: Allocators are not used on %FormContextHandler for performance in
	//	most cases.
	return ReduceAsSubobjectReference(term,
		A1::AllocateSharedTermValue(a, ContextHandler(std::allocator_arg, a,
		FormContextHandler(RefContextHandler(h, r_env), n))), r_env,
		ref.GetTags());
}

} // inline namespace Internals;

} // namesapce A1;

} // namespace NPL;

