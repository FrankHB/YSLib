/*
	© 2018-2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA1Internals.cpp
\ingroup NPL
\brief NPLA1 内部接口。
\version r20282
\author FrankHB <frankhb1989@gmail.com>
\since build 473
\par 创建时间:
	2020-02-15 13:20:08 +0800
\par 修改时间:
	2020-07-18 18:37 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	NPL::NPLA1Internals
*/


#include "NPL/YModules.h"
#include "NPLA1Internals.h"

using namespace YSLib;

namespace NPL
{

namespace A1
{

inline namespace Internals
{

#if NPL_Impl_NPLA1_Enable_Thunked
#	if NPL_Impl_NPLA1_Enable_TCO
ReductionStatus
TCOAction::operator()(ContextNode& ctx) const
{
	YAssert(ystdex::ref_eq<>()(EnvGuard.func.Context.get(), ctx),
		"Invalid context found.");

	// NOTE: Lifting is optional, but is shall be performed before release
	//	of guards. See also $2018-02 @ %Documentation::Workflow.
	const auto res(HandleResultRequests(TermRef, ctx));

	// NOTE: The order here is significant. The environment in the guards
	//	should be hold until lifting is completed.
	{
		const auto egd(std::move(EnvGuard));
	}
	while(!xgds.empty())
		xgds.pop_back();
	// NOTE: This should be after the guards to ensure there is no term
	//	references the environment.
	while(!RecordList.empty())
	{
		// NOTE: The order is significant, as %FrameRecord destruction now
		//	is unspecified, and temporary objects have dependencies on
		//	environments.
		auto& front(RecordList.front());

		get<ActiveCombiner>(front) = {};
		RecordList.pop_front();
	}
	return res;
}


TCOAction&
EnsureTCOAction(ContextNode& ctx, TermNode& term)
{
	auto p_act(AccessTCOAction(ctx));

	if(!p_act)
	{
		SetupTailTCOAction(ctx, term, {});
		p_act = AccessTCOAction(ctx);
	}
	return NPL::Deref(p_act);
}


void
RecordCompressor::Compress()
{
	// NOTE: This is need to keep the root as external reference.
	const auto p_root(NPL::Nonnull(RootPtr.lock()));

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
	for(ReferenceSet rs; !NewlyReachable.empty();
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

	// TODO: Full support of PTC by direct DFS traverse.
	ReferenceSet accessed;

	ystdex::retry_on_cond(ystdex::id<>(), [&]() -> bool{
		bool collected = {};

		Traverse(*p_root, p_root->Parent,
			[&](const shared_ptr<Environment>& p_dst, Environment& src,
			ValueObject& parent) -> bool{
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
#	endif
#endif


EnvironmentReference
FetchTailEnvironmentReference(const TermReference& ref, ContextNode& ctx)
{
	auto r_env(ref.GetEnvironmentReference());

	// NOTE: Unsafe term reference is enforced to be safe with current
	//	environment as the holder.
	return
		r_env.GetAnchorPtr() ? r_env : EnvironmentReference(ctx.GetRecordPtr());
}

ReductionStatus
ReduceForCombinerRef(TermNode& term, ContextNode& ctx,
	const TermReference& ref, const ContextHandler& h, size_t n)
{
	const auto& r_env(FetchTailEnvironmentReference(ref, ctx));
	const auto a(term.get_allocator());
	auto p_sub(YSLib::allocate_shared<TermNode>(a, TermNode(std::allocator_arg,
		a, NoContainer, ContextHandler(FormContextHandler(RefContextHandler(h,
		r_env), n)))));
	auto& sub(NPL::Deref(p_sub));
	TermNode tm(std::allocator_arg, a, {{std::allocator_arg, a, NoContainer,
		std::move(p_sub)}}, std::allocator_arg, a, TermReference(sub, r_env));

	term.SetContent(std::move(tm));
	return ReductionStatus::Retained;
}

} // inline namespace Internals;

} // namesapce A1;

} // namespace NPL;

