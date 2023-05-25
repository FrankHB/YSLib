/*
	© 2016-2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA1Forms.cpp
\ingroup NPL
\brief NPLA1 语法形式。
\version r34098
\author FrankHB <frankhb1989@gmail.com>
\since build 882
\par 创建时间:
	2020-02-15 11:19:51 +0800
\par 修改时间:
	2023-05-16 08:24 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA1Forms
*/


#include "NPL/YModules.h"
#include YFM_NPL_NPLA1Forms // for IsSticky, NPL::Deref, ReferenceTerm,
//	ystdex::update_thunk, trivial_swap, ContextHandler,
//	ResolvedTermReferencePtr, string, IsNPLASymbol, ystdex::fast_all_of,
//	ystdex::isdigit, std::strchr, ystdex::call_value_or, LiftToReturn,
//	TryAccessReferencedTerm, TryAccessLeafAtom, TermReference, in_place_type,
//	LiftTerm, TypeError;
#include "NPLA1Internals.h" // for A1::Internals API;

namespace NPL
{

namespace A1
{

//! \since build 685
namespace
{

//! \since build 928
void
EqualSubterm(bool& r, Action& act, TermNode::allocator_type a, TNCIter first1,
	TNCIter first2, TNCIter last1)
{
	if(first1 != last1)
	{
		YAssert(!IsSticky(NPL::Deref(first1).Tags),
			"Invalid representation found."),
		YAssert(!IsSticky(NPL::Deref(first2).Tags),
			"Invalid representation found.");

		auto& x(ReferenceTerm(NPL::Deref(first1)));
		auto& y(ReferenceTerm(NPL::Deref(first2)));

		if(TermUnequal(x, y))
			r = {};
		else
		{
			yunseq(++first1, ++first2);
			ystdex::update_thunk(act, a, trivial_swap,
				[&, a, first1, first2, last1]{
				if(r)
					EqualSubterm(r, act, a, first1, first2, last1);
			});
			ystdex::update_thunk(act, a, trivial_swap, [&, a]{
				if(r)
					EqualSubterm(r, act, a, x.begin(), y.begin(), x.end());
			});
		}
	}
}

} // unnamed namespace;

ReductionStatus
ReduceForwardHandler(TermNode& term, ContextHandler& h,
	ResolvedTermReferencePtr p_ref)
{
	// XXX: Allocators are not used on %FormContextHandler for performance in
	//	most cases.
	return ReduceMakeForm(term, MakeValueOrMove(p_ref, [&]{
		return h;
	}, [&]{
		return std::move(h);
	}), 1);
}


bool
IsSymbol(const string& id) ynothrow
{
	return !id.empty() && IsNPLASymbol(id) && ystdex::fast_all_of(id.begin(),
		id.end(), [] YB_LAMBDA_ANNOTATE((char c), ynothrow, const){
			// XXX: Conservatively only check the basic character set. There
			//	could be false negative results.
			return ystdex::isdigit(c) || bool(std::strchr(
				"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
				"!$%&*./:<=>?@^_~+-", c));
		});
}


bool
Encapsulation::Equal(const TermNode& x, const TermNode& y)
{
	if(TermUnequal(x, y))
		return {};

	// NOTE: This is a trampoline to eliminate the call depth limitation.
	// XXX: This is not like %SeparatorPass in %NPLA1 inserting subnodes for
	//	each turn of the search which is not friendly to short-curcuit
	//	evaluation.
	bool r(true);
	Action act{};

	EqualSubterm(r, act, x.get_allocator(), x.begin(), y.begin(), x.end());
	while(act)
	{
		const auto a(std::move(act));

		a();
	}
	return r;
}


ReductionStatus
Encapsulate::operator()(TermNode& term) const
{
	return Forms::CallUnary([this] YB_LAMBDA_ANNOTATE((TermNode& tm), , pure){
		return Encapsulation(GetType(), std::move(tm));
	}, term);
}


ReductionStatus
EncapsulateValue::operator()(TermNode& term) const
{
	return Forms::CallUnary([this](TermNode& tm){
		LiftToReturn(tm);
		return Encapsulation(GetType(), std::move(tm));
	}, term);
}


ReductionStatus
Encapsulated::operator()(TermNode& term) const
{
	return Forms::CallUnary([this] YB_LAMBDA_ANNOTATE((TermNode& tm), , pure){
		return bool(ystdex::call_value_or([this] YB_LAMBDA_ANNOTATE(
			(const Encapsulation& enc), ynothrow, pure){
			return Get() == enc.Get();
		}, TryAccessReferencedTerm<Encapsulation>(tm)));
	}, term);
}


ReductionStatus
Decapsulate::operator()(TermNode& term) const
{
	return Forms::CallRegularUnaryAs<const Encapsulation>(
		[&](const Encapsulation& enc, ResolvedTermReferencePtr p_ref){
		if(Get() == enc.Get())
		{
			auto& tm(enc.TermRef);

			return MakeValueOrMove(p_ref, [&]() -> ReductionStatus{
				// NOTE: As an lvalue reference, the object in %tm cannot be
				//	destroyed.
				if(const auto p = TryAccessLeafAtom<const TermReference>(tm))
					term.SetContent(tm.GetContainer(), *p);
				else
				{
					// XXX: Subterms cleanup is deferred.
					// XXX: Allocators are not used here for performance.
					term.SetValue(in_place_type<TermReference>, tm,
						p_ref->GetEnvironmentReference());
					return ReductionStatus::Clean;
				}
				return ReductionStatus::Retained;
			}, [&]{
				LiftTerm(term, tm);
				return ReductionStatus::Retained;
			});
		}
		throw TypeError("Mismatched encapsulation type found.");
	}, term);
}

namespace Forms
{

} // namespace Forms;

} // namesapce A1;

} // namespace NPL;

