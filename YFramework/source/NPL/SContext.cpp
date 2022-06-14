/*
	© 2012-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file SContext.cpp
\ingroup NPL
\brief S 表达式上下文。
\version r2314
\author FrankHB <frankhb1989@gmail.com>
\since build 329
\par 创建时间:
	2012-08-03 19:55:59 +0800
\par 修改时间:
	2022-06-13 01:32 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::SContext
*/


#include "NPL/YModules.h"
#include YFM_NPL_SContext // for vector, NPL::Deref, std::allocator_arg,
//	YSLib::stack, pair, lref;
#include <ystdex/function.hpp> // for ystdex::unchecked_function,
//	ystdex::trivial_swap;
// NOTE: This enables thunked actions in copy for nested call safety, instead of
//	using stacks.
#define NPL_Impl_SContext_Enable_ThunkedActions true
#if NPL_Impl_SContext_Enable_ThunkedActions
// NOTE: If non zero, thunks are only used only for more than certain level of
//	nested calls occur. The value should be small enough to ensure the nested
//	call safety guarantee still works in practice.
#	ifndef NPL_Impl_SContext_Enable_ThunkedThreshold
#		define NPL_Impl_SContext_Enable_ThunkedThreshold yimpl(32U)
#	endif
#else
#	include <ystdex/expanded_function.hpp> // for ystdex::retry_on_cond;
#endif

namespace NPL
{

#if NPL_Impl_SContext_Enable_ThunkedActions
//! \since build 944
namespace
{

struct ConSubThunk final
{
	// XXX: This is more efficient than %function instance with
	//	%ystdex::update_thunk for normal cases.
	using Action = ystdex::unchecked_function<void()>;
	mutable vector<Action> Current;
			
	ConSubThunk(TermNode::allocator_type a) ynothrow
		: Current(a)
	{}

	void
	operator()(TermNode::Container& res, const TermNode::Container& con)
		const
	{
		CopySubterms(res, con.begin(), con.end());
		while(!Current.empty())
		{
			const auto f(std::move(Current.back()));

			Current.pop_back();
			f();
		}
	}

	void
	CopySubterms(TermNode::Container& r, TNCIter first, TNCIter last
#	if NPL_Impl_SContext_Enable_ThunkedThreshold != 0
		, size_t n = 0
#	endif
		) const
	{
		YAssert(first != last, "Invalid range found.");

		auto& dst(r.emplace_back());
		const auto& src(NPL::Deref(first));

		++first;
#	if NPL_Impl_SContext_Enable_ThunkedThreshold != 0
		if(++n < NPL_Impl_SContext_Enable_ThunkedThreshold)
		{
			if(!src.empty())
				CopySubterms(dst.GetContainerRef(), src.begin(), src.end(), n);
			if(first != last)
				CopySubterms(r, first, last, n);
			CopyValueAndTags(dst, src);
			return;
		}
#	endif
		UpdateCopyValueAndTags(dst, src);
		if(first != last)
			UpdateCopySubterms(r, first, last
#	if NPL_Impl_SContext_Enable_ThunkedThreshold != 0
				, n
#	endif
			);
		if(!src.empty())
			Update(
#	if NPL_Impl_SContext_Enable_ThunkedThreshold != 0
				[&, n]{
				CopySubterms(dst.GetContainerRef(), src.begin(), src.end(), n);
#	else
				[&]{
				CopySubterms(dst.GetContainerRef(), src.begin(), src.end());
#	endif
			});
	}

	static PDefH(void, CopyValueAndTags, TermNode& dst, const TermNode& src)
		ImplUnseq(dst.Value = src.Value, dst.Tags = src.Tags)

	template<typename _func>
	void
	Update(_func f) const
	{
		// XXX: Allocators are not used here for performance.
		Current.push_back(std::move(f));
	}

	void
	UpdateCopySubterms(TermNode::Container& r, TNCIter first, TNCIter last
#	if NPL_Impl_SContext_Enable_ThunkedThreshold != 0
		, size_t n = 0
#	endif
		) const
	{
		Update(
#	if NPL_Impl_SContext_Enable_ThunkedThreshold != 0
			[&, first, last, n]{
			CopySubterms(r, first, last, n);
#	else
			[&, first, last]{
			CopySubterms(r, first, last);
#	endif
		});
	}

	// XXX: Making it a separate definition slightly improves the generated code
	//	quality with x86_64-pc-linux G++ 11.1.
	void
	UpdateCopyValueAndTags(TermNode& dst, const TermNode& src) const
	{
		Update([&]{
			CopyValueAndTags(dst, src);
		});
	}
};

} // unnamed namespace;
#endif

void
TermNode::ClearContainer() ynothrow
{
	// NOTE: See %~TermNode.
#if true
	while(!container.empty())
	{
		const auto i(container.begin());

		// NOTE: Flatten the subterms and insert them before the leftmost
		//	recursive subterm.
		container.splice(i, std::move(NPL::Deref(i).container));
		container.erase(i);
	}
#else
	// NOTE: The alternative implementation may be more efficient, but only
	//	for cases with sufficient large amount of subterms.
	for(auto i(container.begin()); i != container.end(); ++i)
		// NOTE: The loop invariant is that no subterms left to 'i' have not
		//	finished the container cleanup (with moved-after state) and the
		//	%Value in these terms are kept in a prefix DFS traverse order in
		//	the resulted sequence. Flatten the subterms and insert them after
		//	'*i' to keep the loop invariant at first.
		container.splice(std::next(i), std::move(i->container));
		// NOTE: Subterms are not erased at once in the loop to prevent the
		//	overhead of internal housekeeping (for the size) in the %container.
	// NOTE: All flattened subterms and remained %Value objects are cleanup.
	container.clear();
#endif
	// XXX: Assertion failure may be caused by undefined behavior or wrong
	//	implmenetation.
	YAssert(IsLeaf(*this), "Failed clearing the container of a term.");
}

// XXX: Simplify with %CreateRecursively?
YB_FLATTEN TermNode::Container
TermNode::ConCons(const ValueNode::Container& con, allocator_type a)
{
	Container res(a);

	for(const auto& item : con)
		res.emplace_back(ConCons(item.GetContainer(), a), item.Value);
	return res;
}
YB_FLATTEN TermNode::Container
TermNode::ConCons(ValueNode::Container&& con, allocator_type a)
{
	Container res(a);

	for(auto& item : con)
		res.emplace_back(ConCons(std::move(item.GetContainerRef()), a),
			std::move(item.Value));
	return res;
}


TermNode::Container
TermNode::ConSub(const Container& con, allocator_type a)
{
	Container res(a);

	if(!con.empty())
	{
#if NPL_Impl_SContext_Enable_ThunkedActions
		// XXX: This works more efficiently when
		//	%NPL_Impl_SContext_Enable_ThunkedThreshold is sufficient large.		
		ConSubThunk{a}(res, con);
#else
		// XXX: Use of the default container (%deque) is more efficient.
		YSLib::stack<pair<lref<TermNode>, lref<const TermNode>>> remained(a),
			tms(a);
	
		// XXX: The order of copying the %Value objects is preserved. Otherwise
		//	the resource allocations and tags modifications are consided pure,
		//	so the order is not necessarily the same in naive implementations
		//	not supporting the nested call safety.
		for(const auto& sub : con)
		{
			res.emplace_back();
			remained.emplace(res.back(), sub),
			tms.emplace(res.back(), sub),
			res.back().Tags = sub.Tags;
			// TODO: Use %not_fn?
			ystdex::retry_on_cond([&]() ynothrow{
				return !remained.empty();
			}, [&]{
				auto& dst_con(remained.top().first.get().GetContainerRef());
				auto& src(remained.top().second.get());

				remained.pop();
				for(auto i(src.rbegin()); i != src.rend(); ++i)
				{
					dst_con.emplace_front();

					auto& nterm(dst_con.front());
					auto& tm(*i);

					remained.emplace(nterm, tm),
					tms.emplace(nterm, tm),
					nterm.Tags = tm.Tags;
				}
			});
			ystdex::retry_on_cond([&]() ynothrow{
				return !tms.empty();
			}, [&]{
				tms.top().first.get().Value = tms.top().second.get().Value;
				tms.pop();
			});
		}
#endif
	}
	return res;
}


void
ReaderState::UpdateLexeme(const string& lexeme)
{
	if(lexeme.length() == 1)
	{
		const char c(lexeme.front());

		if(c == LeftDelimiter)
			++LeftDelimiterCount;
		else if(c == RightDelimiter)
			++RightDelimiterCount;
	}
}

} // namespace NPL;

