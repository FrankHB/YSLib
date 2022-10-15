/*
	© 2017-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA1Internals.h
\ingroup NPL
\brief NPLA1 内部接口。
\version r22721
\author FrankHB <frankhb1989@gmail.com>
\since build 882
\par 创建时间:
	2020-02-15 13:20:08 +0800
\par 修改时间:
	2022-10-11 01:52 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	NPL::NPLA1Internals
*/


#ifndef NPL_INC_NPLA1Internals_h_
#define NPL_INC_NPLA1Internals_h_ 1

#include "YModules.h"
#include YFM_NPL_NPLA1 // for shared_ptr, ContextNode, YSLib::allocate_shared,
//	NPL::Deref, NPLException, TermNode, ReductionStatus, Reducer, YSLib::map,
//	size_t, lref, Environment, set, IsTyped, EnvironmentList,
//	EnvironmentReference, pair, YSLib::forward_list, tuple, std::declval,
//	EnvironmentGuard, NPL::get, A1::NameTypedContextHandler, MoveKeptGuard,
//	TermReference, TermTags, TryAccessLeafAtom, ThrowTypeErrorForInvalidType,
//	type_id, TermToNamePtr, IsIgnore, ParameterMismatch, IsPair, IsEmpty,
//	IsList, NPL::AsTermNode, NPL::AsTermNodeTagged;
#include <ystdex/compose.hpp> // for ystdex::get_less;
#include <ystdex/scope_guard.hpp> // for ystdex::unique_guard;
#include <ystdex/optional.h> // for ystdex::optional;
#include <ystdex/utility.hpp> // for ystdex::exchange;
#include <ystdex/ref.hpp> // for std::reference_wrapper, std::ref,
//	ystdex::unref;
#include <ystdex/bind.hpp> // for ystdex::bind1, std::placeholders::_2;
#include <ystdex/function.hpp> // for ystdex::unchecked_function;
#include <ystdex/function_adaptor.hpp> // for ystdex::update_thunk;
#include <iterator> // for std::next;

namespace NPL
{

namespace A1
{

/*!
\brief 内部接口。
\since build 882

用于实现 NPLA1Forms 和其它 NPL 模块的非公开 API 。
*/
inline namespace Internals
{

// NOTE: The inlined synchronous calls are more effecient than asynchronous
//	ones. It is intended to prevent unneeded uncapturable continuation being
//	construced by reduction action composition calls (e.g. of %RelaySwitched).
//	However, they are only safe where they are not directly called by
//	asynchronous implementations which expect the actions in these direct calls
//	are always strictly asynchronous. In thunked implementations, they shall
//	also be cared to avoid direct recursion of synchronous calls which make the
//	thunks potentially useless to prevent the host stack overflow.
#define NPL_Impl_NPLA1_Enable_InlineDirect true

// NOTE: The following options provide documented alternative implementations.
//	They are for exposition only. The code without TCO or asynchronous thunked
//	calls works without several guarantees in the specification (notably,
//	support of PTC), which is not conforming. Other documented cases not
//	supporting PTC are noted in implementations separatedly.
#define NPL_Impl_NPLA1_Enable_TCO true
#define NPL_Impl_NPLA1_Enable_Thunked true
// NOTE: If non zero, thunks are only used only for more than certain level of
//	nested calls occur. The value should be small enough to ensure the nested
//	call safety guarantee still works in practice. Currently only enabled for
//	thunked separator passes (see below).
#ifndef NPL_Impl_NPLA1_Enable_ThunkedThreshold
#	define NPL_Impl_NPLA1_Enable_ThunkedThreshold yimpl(16U)
#endif
#define NPL_Impl_NPLA1_Enable_ThunkedSeparatorPass NPL_Impl_NPLA1_Enable_Thunked

//! \since build 820
static_assert(!NPL_Impl_NPLA1_Enable_TCO || NPL_Impl_NPLA1_Enable_Thunked,
	"Invalid combination of build options found.");

//! \since build 842
inline PDefH(void, SetupNextTerm, ContextNode& ctx, TermNode& term)
	ImplExpr(ContextState::Access(ctx).SetNextTermRef(term))

// NOTE: See $2018-09 @ %Documentation::Workflow for rationale of the
//	implementation of PTC.

/*!
\brief 源代码名称恢复器。
\since build 947
*/
class SourceNameRecoverer final
{
private:
	observer_ptr<SourceName> p_current{};
	SourceName Saved{};

public:
	DefDeCtor(SourceNameRecoverer)
	SourceNameRecoverer(SourceName& cur, SourceName name)
		: p_current(&cur), Saved(name)
	{}
	DefDeCopyMoveCtorAssignment(SourceNameRecoverer)

	void
	operator()() const ynothrow
	{
		if(p_current)
			*p_current = std::move(Saved);
	}
};

// XXX: First-class continuations are not implemented completely yet, due to
//	lack of term replacement mechanism in captured continuation. Kernel-style
//	continuation interception is also unsupported because no reference for
//	parantage is maintained in the context currently.

/*!
\brief 一次续延检查器。
\since build 943
*/
class OneShotChecker final
{
private:
	shared_ptr<bool> p_shot;

public:
	OneShotChecker(ContextNode& ctx)
		: p_shot(YSLib::allocate_shared<bool>(ctx.get_allocator()))
	{}
	//! \since build 947
	DefDeCopyMoveCtorAssignment(OneShotChecker)

	void
	operator()() const ynothrow
	{
		// XXX: To allow moving %OneShotChecker, check of %p_shot is necessary.
		if(p_shot)
			NPL::Deref(p_shot) = true;
	}

	void
	Check() const
	{
		auto& shot(NPL::Deref(p_shot));

		if(!shot)
			shot = true;
		else
			throw NPLException("One-shot continuation expired.");
	}
};


#if NPL_Impl_NPLA1_Enable_Thunked
#	if false
//! \since build 821
YB_ATTR_nodiscard inline
#		if true
PDefH(ReductionStatus, RelayTail, ContextNode& ctx, Reducer& cur)
	ImplRet(RelaySwitched(ctx, std::move(cur)))
#		else
// NOTE: For exposition only. This does not hold guarantee of TCO in unbounded
//	recursive cases.
PDefH(ReductionStatus, RelayTail, ContextNode&, const Reducer& cur)
	ImplRet(cur())
#		endif
#	endif

//! \since build 840
template<typename _func>
void
SetupTailAction(ContextNode& ctx, _func&& act)
{
	ctx.SetupFront(std::move(act));
}

#	if NPL_Impl_NPLA1_Enable_TCO
//! \since build 827
struct RecordCompressor final
{
	using RecordInfo
		= YSLib::map<lref<Environment>, size_t, ystdex::get_less<>>;
	using ReferenceSet = set<lref<Environment>, ystdex::get_less<>>;

	//! \since build 894
	weak_ptr<Environment> RootPtr;
	ReferenceSet Reachable, NewlyReachable;
	RecordInfo Universe;

	//! \since build 894
	RecordCompressor(const shared_ptr<Environment>& p_root)
		: RecordCompressor(p_root, NPL::Deref(p_root).Bindings.get_allocator())
	{}
	//! \since build 894
	RecordCompressor(const shared_ptr<Environment>& p_root,
		Environment::allocator_type a)
		: RootPtr(p_root), Reachable({NPL::Deref(p_root)}, a),
		NewlyReachable(a), Universe(a)
	{
		AddParents(NPL::Deref(p_root));
	}

	// XXX: All checks rely on recursive calls which do not respect nested
	//	safety currently.
	//! \since build 860
	void
	AddParents(Environment& e)
	{
		Traverse(e, e.Parent,
			[this](const shared_ptr<Environment>& p_dst, const Environment&){
			return Universe.emplace(NPL::Deref(p_dst),
				CountReferences(p_dst)).second;
		});
	}

	void
	Compress();

	//! \since build 894
	YB_ATTR_nodiscard YB_PURE static size_t
	CountReferences(const shared_ptr<Environment>& p) ynothrowv
	{
		const auto acnt(NPL::Deref(p).GetAnchorCount());

		YAssert(acnt > 0, "Zero anchor count found for environment.");
		return CountStrong(p) + size_t(acnt) - 2;
	}

	//! \since build 894
	YB_ATTR_nodiscard YB_PURE static size_t
	CountStrong(const shared_ptr<Environment>& p) ynothrowv
	{
		const long scnt(p.use_count());

		YAssert(scnt > 0, "Zero shared count found for environment.");
		return size_t(scnt);
	}

	//! \since build 882
	template<typename _fTracer>
	static void
	Traverse(Environment& e, ValueObject& parent, const _fTracer& trace)
	{
		const auto& ti(parent.type());

		if(IsTyped<EnvironmentList>(ti))
		{
			for(auto& vo : parent.GetObject<EnvironmentList>())
				Traverse(e, vo, trace);
		}
		else if(IsTyped<EnvironmentReference>(ti))
		{
			if(auto p = parent.GetObject<EnvironmentReference>().Lock())
				TraverseForSharedPtr(e, parent, trace, p);
		}
		else if(IsTyped<shared_ptr<Environment>>(ti))
		{
			if(auto p = parent.GetObject<shared_ptr<Environment>>())
				TraverseForSharedPtr(e, parent, trace, p);
		}
	}

private:
	//! \since build 894
	template<typename _fTracer>
	static void
	TraverseForSharedPtr(Environment& e, ValueObject& parent,
		const _fTracer& trace, shared_ptr<Environment>& p)
	{
		if(ystdex::expand_proxy<void(const shared_ptr<Environment>&,
			Environment&, ValueObject&)>::call(trace, p, e, parent))
		{
			auto& dst(*p);

			// NOTE: The shared pointer should not be locked to ensure it
			//	neutral to nested calls.
			p.reset();
			Traverse(dst, dst.Parent, trace);
		}
	}
};


/*!
\brief 帧记录索引。
\note 顺序保持和 FrameRecord 的元素对应一致。
\since build 842
*/
enum RecordFrameIndex : size_t
{
	/*!
	\brief 活动环境。
	
	作为尾上下文的求值的组成活动记录的环境指针。
	环境指针在求值中是可选的，但只有最终为非空值时，活动的帧记录被保留。
	若环境指针为空值，最终所在的帧记录被移除。
	*/
	ActiveEnvironmentPtr,
	/*!
	\brief 活动合并子。

	在活动环境中调用的对象。
	因为尾调用不都是函数合并，合并子是可选的。
	当前直接转移从合并项转移。左值可带有源信息。
	*/
	ActiveCombiner
};

/*!
\brief 帧记录。
\note 成员顺序和 RecordFrameIndex 中的项对应。
\note 成员析构的顺序是确定的。
\since build 842
\sa RecordFrameIndex
*/
using FrameRecord = pair<shared_ptr<Environment>, ValueObject>;


/*!
\brief 帧记录列表。
\sa FrameRecord
\since build 946

表示帧记录的序列。
随子过程调用的添加到序列起始位置。
一般地，基类不应被依赖。若需确定的资源释放顺序，直接使用这个类而非其基类。
*/
class YF_API FrameRecordList : public yimpl(YSLib::forward_list)<FrameRecord>
{
private:
	using Base = yimpl(YSLib::forward_list)<FrameRecord>;

public:
#	if YB_IMPL_CLANGPP || YB_IMPL_GNUCPP >= 100000
	// XXX: See ContextNode::ReducerSequence.
	FrameRecordList() ynoexcept_spec(Base())
		: Base()
	{}
#	elif __cpp_inheriting_constructors < 201511L
	DefDeCtor(FrameRecordList)
#	endif
	using Base::Base;
	// XXX: This ignores %select_on_container_copy_construction.
	FrameRecordList(const FrameRecordList& l)
		: Base(l, l.get_allocator())
	{}
	DefDeMoveCtor(FrameRecordList)
	~FrameRecordList()
	{
		clear();
	}

	DefDeCopyMoveAssignment(FrameRecordList)

	void
	clear() ynothrow
	{
		// NOTE: The order is significant, as the destruction order of
		//	components of %FrameRecord is unspecified.
		// XXX: Different to %ContextNode::ReducerSequence::clear, this is a bit
		//	more efficient.
		while(!empty())
			// NOTE: The order of destruction of the members in each frame is
			//	implied by %FrameRecord.
			pop_front();
	}
};


//! \since build 818
class TCOAction final
{
private:
	// NOTE: Specialized guard type (instead of using %ystdex::unique_guard) is
	//	more efficient here.
	// XXX: More specialized guard type without %ystdex::unique_guard works, but
	//	it is acually less efficient, at least on x86_64-pc-linux G++ 10.2.
	//! \since build 910
	struct GuardFunction final
	{
		//! \brief 当前项引用。
		lref<TermNode> TermRef;

		PDefHOp(void, (), ) const ynothrow
			ImplExpr(TermRef.get().Clear())
	};
	//! \since build 947
	//@{
	/*!
	\brief 附加信息索引。
	\note 顺序保持和 ExtraInfo 的元素对应一致。
	*/
	enum ExtraInfoIndex : size_t
	{
		RecoverSourceName,
		CheckOneShot
	};

	/*!
	\brief 附加信息记录。
	\note 成员顺序和 ExtraInfoIndex 中的项对应。
	\note 成员析构的顺序是未指定的。
	\sa ExtraInfoIndex
	*/
	using ExtraInfo = tuple<ystdex::guard<SourceNameRecoverer>,
		ystdex::optional<ystdex::guard<OneShotChecker>>>;
	//@}

	//! \since build 910
	mutable size_t req_lift_result = 0;
	//! \since build 946
	mutable FrameRecordList record_list;
	// NOTE: The guard to swap external environment pointer is separated from
	//	the frame record list to allow less resources kept in the list.
	// NOTE: See $2022-06 @ %Documentation::Workflow for details.
	//! \since build 946
	mutable EnvironmentGuard env_guard;
	//! \since build 909
	mutable decltype(ystdex::unique_guard(std::declval<GuardFunction>()))
		term_guard;
	/*!
	\brief 可选的附加信息。
	\since build 947

	保存不是每个 TCO 动作都需被使用的信息，避免频繁创建的性能开销。
	*/
	mutable ystdex::optional<ExtraInfo> opt_extra_info{};

public:
	/*!
	\warning 若不满足 NPLA1 规约函数上下文状态类型要求，行为未定义。
	\since build 819
	*/
	TCOAction(ContextNode& ctx, TermNode& term, bool lift)
		: req_lift_result(lift ? 1 : 0), record_list(ctx.get_allocator()),
		// NOTE: The external environment pointer is not saved in the guard yet,
		//	and it should be set later if any environment switch is needed.
		env_guard(ctx), term_guard(ystdex::unique_guard(GuardFunction{term}))
		// XXX: Do not call %AssertValueTags on %term, as it can be (and is
		//	usually) a combiniation instead of the representation of some
		//	object language value.
	{}
	// XXX: Not used, but provided for well-formness.
	//! \since build 819
	TCOAction(const TCOAction& a)
		// XXX: Some members are moved. This is only safe when newly constructed
		//	object always live longer than the older one.
		: req_lift_result(a.req_lift_result),
		record_list(a.record_list.get_allocator()),
		env_guard(std::move(a.env_guard)), term_guard(std::move(a.term_guard))
	{}
	DefDeMoveCtor(TCOAction)
	// XXX: Out-of-line destructor here is inefficient.

	DefDeMoveAssignment(TCOAction)

	//! \since build 877
	ReductionStatus
	operator()(ContextNode&) const;

private:
	//! \since build 947
	YB_ATTR_nodiscard ExtraInfo&
	GetExtraInfoRef()
	{
		if(!opt_extra_info)
			opt_extra_info.emplace();
		return *opt_extra_info;
	}

public:
	//! \since build 958
	DefGetter(const ynothrow, ContextNode&, ContextRef, env_guard.func.Context)
	//! \since build 913
	DefGetter(const ynothrow, TermNode&, TermRef, term_guard.func.func.TermRef)
	//! \since build 957
	//@{
	DefGetter(const ynothrow, FrameRecordList&, FrameRecordList, record_list)

	// NOTE: The allocation of the active combiner should normally be done
	//	before the assignment of the environment pointer, because the object
	//	should be stable (i.e. pinned) before the environment can be determined. 
	//! \post \c AssertAttched() 。
	YB_ATTR(hot) void
	AddOperator(ValueObject& op) const
	{
		// NOTE: Rather than only the target %TokenValue value, the whole
		//	%ValueObject is needed, since it can have the source information
		//	in its holder and %QuerySourceInformation requires an object of
		//	%ValueObject.
		record_list.emplace_front(nullptr, std::move(op));
		// XXX: After the move, the value is unspecified. This should be no
		//	longer used, so it is irrelavant.
	}

	/*!
	\pre 断言：帧记录列表非空。
	\pre 断言：帧记录中的环境指针为空值。
	\since build 958
	*/
	void
	AssertAttached() const ynothrowv
	{
		YAssert(!record_list.empty(), "No entry found in the record list");
		YAssert(!NPL::get<ActiveEnvironmentPtr>(record_list.front()),
			"Missing the fresh frame in the record list.");
	}

	//! \post \c AssertAttched() 。
	YB_ATTR_nodiscard ValueObject&
	Attach(ValueObject& op) const
	{
		AddOperator(op);
		// TODO: Blocked. Use ISO C++17 return value of %forward_list as the
		//	call to %emplace_front implied in %AddOperator.
		return NPL::get<ActiveCombiner>(record_list.front());
	}
	//@}

	//! \since build 910
	//@{
	void
	CompressFrameList();

	// NOTE: See $2018-06 @ %Documentation::Workflow and $2019-06 @
	//	%Documentation::Workflow for details.
	void
	CompressForContext(ContextNode& ctx)
	{
		CompressFrameList();
		RecordCompressor(ctx.GetRecordPtr()).Compress();
	}

	/*!
	\pre 断言：第二参数中的上下文和 TCO 动作保存的上下文一致。
	\pre 断言：第二参数中的环境非空。
	*/
	YB_ATTR(hot) void
	CompressForGuard(ContextNode& ctx, EnvironmentGuard&& gd)
	{
		// NOTE: Operand saving is performed whether the frame compression is
		//	needed, once there is a saved environment.
		auto& p_saved(gd.func.SavedPtr);

		// NOTE: The guard must reference the same context here.
		YAssert(ystdex::ref_eq<>()(GetContextRef(), ctx),
			"Invalid context found.");
		YAssert(ystdex::ref_eq<>()(gd.func.Context.get(), ctx),
			"Invalid guard found.");
		// NOTE: An empty saved pointer is allowed by the interface (for an
		//	object %EnvironmentSwitcher initialized without an environment) but
		//	unsupported here.
		YAssert(p_saved, "Invalid guard found.");
		// NOTE: If there is no environment set in %act.env_guard yet, there is
		//	ideally no need to save the components to the frame record list
		//	for recursive calls.
		if(env_guard.func.SavedPtr)
		{
			// NOTE: The environment is saved in the frame record list as a new
			//	entry if necessary. The existence of the entry shall be checked
			//	because it may be missing or not from a combiner (e.g. by
			//	'eval'). The view of active combiners eases the comsumer (e.g.
			//	%TraceBacktrace) to get all active combiners of %TCOAction.
			if(!record_list.empty()
				&& !NPL::get<ActiveEnvironmentPtr>(record_list.front()))
				NPL::get<ActiveEnvironmentPtr>(record_list.front())
					= std::move(p_saved);
			else
				record_list.emplace_front(std::move(p_saved), ValueObject(
					record_list.get_allocator()));
			// NOTE: If the external pointer (saved in %env_guard) is reused, it
			//	will be compressed and the active combiner will be abandoned.
			CompressForContext(ctx);
		}
		else
			env_guard.func.SavedPtr = std::move(gd.func.SavedPtr);
		// NOTE: Each further operation making potentionally calling to
		//	%AddOperator (e.g. in %ReduceCombined) should always go into this
		//	function (unless some exception is thrown) and that time there
		//	should be a new active combiner, or the current active combiner is
		//	reused for the new environment.
	}
	//@}

	//! \since build 943
	YB_ATTR_nodiscard YB_ATTR(cold) OneShotChecker
	MakeOneShotChecker()
	{
		auto& one_shot_guard(NPL::get<CheckOneShot>(GetExtraInfoRef()));

		if(!one_shot_guard)
			// XXX: The context is only used to determine the allocator, which
			//	is an implementation detail. Usually the context in caller
			//	should be the same, though.
			one_shot_guard.emplace(GetContextRef());
		return one_shot_guard->func;
	}

	/*!
	\pre 间接断言：满足 AssertAttached 的要求。
	\since build 957
	*/
	void
	PopTopFrame() const
	{
		// XXX: At current, no additional environment pointer is allowed. This
		//	is intended to be used for the vau handler before the call to
		//	%RelayForCall (which would set the pointer).
		AssertAttached();
		record_list.pop_front();
	}

	/*!
	\pre \c NPL::get<CheckOneShot>(GetExtraInfoRef()) 。
	\since build 945
	*/
	void
	ReleaseOneShotGuard()
	{
		auto& one_shot_guard(NPL::get<CheckOneShot>(GetExtraInfoRef()));

		YAssert(one_shot_guard, "One-shot guard is not initialized properly.");
		return one_shot_guard.reset();
	}

	/*!
	\brief 保存尾上下文中的源代码名称。
	\since build 947
	*/
	YB_ATTR(cold) void
	SaveTailSourceName(SourceName& cur, SourceName name)
	{
		NPL::get<RecoverSourceName>(GetExtraInfoRef())
			= SourceNameRecoverer(cur, std::move(name));
	}

	//! \since build 911
	//@{
	//! \brief 设置提升请求。
	void
	SetupLift() const
	{
		// NOTE: The flag indicates a request for handling during next time
		//	before %TCOAction is finished, handled in %operator().
		++req_lift_result;
		if(YB_UNLIKELY(req_lift_result == 0))
			throw NPLException(
				"TCO action lift request count overflow detected.");
	}
	/*
	\brief 按参数设置提升请求。
	\param lift 指定规约后提升结果。
	*/
	void
	SetupLift(bool lift) const
	{
		if(lift)
			SetupLift();
	}
	//@}
};

//! \since build 886
YB_ATTR_nodiscard YB_PURE inline
	PDefH(TCOAction*, AccessTCOAction, ContextNode& ctx) ynothrow
	ImplRet(ctx.AccessCurrentAs<TCOAction>())

//! \since build 942
YB_ATTR_nodiscard YB_PURE inline
	PDefH(TCOAction*, AccessTCOActionUnchecked, ContextNode& ctx) ynothrowv
	ImplRet(ctx.AccessCurrentAsUnchecked<TCOAction>())

// NOTE: There is no need to check term like
//	'if(&p->GetTermPtr().get() == &term)'. It should be same to saved enclosing
//	term unless a nested TCO action is needed explicitly (by following
//	%SetupTailAction rather than %EnsureTCOAction).

//! \since build 840
YB_ATTR_nodiscard YB_FLATTEN TCOAction&
EnsureTCOAction(ContextNode& ctx, TermNode& term);

/*!
\pre 当前动作是 TCO 动作。
\since build 840
*/
YB_ATTR_nodiscard inline PDefH(TCOAction&, RefTCOAction, ContextNode& ctx)
	// NOTE: The TCO action should have been created by a previous call of
	//	%EnsureTCOAction, typically in the call of %CombinerReturnThunk in
	//	calling a combiner from %ReduceCombinedBranch.
	ImplRet(NPL::Deref(AccessTCOAction(ctx)))

//! \since build 886
inline
	PDefH(void, SetupTailTCOAction, ContextNode& ctx, TermNode& term, bool lift)
	ImplExpr(SetupTailAction(ctx, TCOAction(ctx, term, lift)))


// XXX: It is accidentally close to ECMAScript 6's %PrepareForTailCall in both
//	name and semantics, but actually independent. In particular, ES6's "pop"
//	wording indicates it only operate on one side of the activation records in
//	relavant contexts, which differs than the %TCOAction here.
/*!
\brief 准备 TCO 求值。
\param ctx 规约上下文。
\param term 被求值项。
\param gd 环境守卫。
\pre 断言：当前动作是 TCO 动作，且其中的当前项和被规约的项相同。
\pre 间接断言：第三参数中的环境非空且 TCO 动作中的状态满足 CompressForGuard 。
\sa TCOAction::CompressForGuard
\since build 878

访问现有的 TCO 动作进行操作压缩，以复用其中已被分配的资源。
*/
inline TCOAction&
PrepareTCOEvaluation(ContextNode& ctx, TermNode& term, EnvironmentGuard&& gd)
{
	auto& act(RefTCOAction(ctx));

	YAssert(&act.GetTermRef() == &term,
		"Invalid term for target TCO action found.");
	yunused(term);
	act.CompressForGuard(ctx, std::move(gd));
	return act;
}
#	endif
#endif


//! \since build 909
inline void
AssertNextTerm(ContextNode& ctx, TermNode& term)
{
	yunused(ctx),
	yunused(term);
#if NPL_Impl_NPLA1_Enable_Thunked
	YAssert(ystdex::ref_eq<>()(term, ContextState::Access(
		ctx).GetNextTermRef()), "Invalid current term found.");
#endif
}


//! \since build 879
//@{
#if NPL_Impl_NPLA1_Enable_Thunked
// NOTE: Normally these overloads do not need %trivial_swap_t because
//	both %Continuation and %std::reference_wrapper instances are specialized
//	enough to make the %Reducer constructor behave expectedly. However, to
//	simplify the caller sites which does not differentiate these types (e.g.
//	%TailCall::RelayNextGuardedLifted below), overloads with %trivial_swap_t are
//	provided anyway.
YB_ATTR_always_inline inline ReductionStatus
RelayCurrent(ContextNode& ctx, Continuation&& cur)
{
	return RelaySwitched(ctx, std::move(cur));
}
//! \since build 929
YB_ATTR_always_inline inline ReductionStatus
RelayCurrent(ContextNode& ctx, trivial_swap_t, Continuation&& cur)
{
	return RelaySwitched(ctx, std::move(cur));
}
YB_ATTR_always_inline inline ReductionStatus
RelayCurrent(ContextNode& ctx, std::reference_wrapper<Continuation> cur)
{
	return RelaySwitched(ctx, cur);
}
//! \since build 929
YB_ATTR_always_inline inline ReductionStatus
RelayCurrent(ContextNode& ctx, trivial_swap_t,
	std::reference_wrapper<Continuation> cur)
{
	return RelaySwitched(ctx, cur);
}
template<typename _fCurrent>
YB_ATTR_always_inline inline auto
RelayCurrent(ContextNode& ctx, _fCurrent&& cur)
	-> decltype(cur(std::declval<TermNode&>(), ctx))
{
	return A1::RelayCurrent(ctx, Continuation(yforward(cur), ctx));
}
//! \since build 926
template<typename _fCurrent>
YB_ATTR_always_inline inline auto
RelayCurrent(ContextNode& ctx, trivial_swap_t, _fCurrent&& cur)
	-> decltype(cur(std::declval<TermNode&>(), ctx))
{
	return A1::RelayCurrent(ctx,
		Continuation(trivial_swap, yforward(cur), ctx));
}
#endif
//@}

//! \since build 878
//@{
#if !NPL_Impl_NPLA1_Enable_Thunked || NPL_Impl_NPLA1_Enable_InlineDirect
template<typename _fCurrent>
YB_ATTR_always_inline inline ReductionStatus
RelayDirect(ContextNode& ctx, _fCurrent&& cur)
{
	return cur(ctx);
}
// XXX: This fails to exclude ill-formed overloads not qualified to
//	'cur(term, ctx)' with 'std::bind' result from libstdc++, either with G++ or
//	Clang++. Make sure %_fCurrent accept two proper parameters.
template<typename _fCurrent>
YB_ATTR_always_inline inline auto
RelayDirect(ContextNode& ctx, _fCurrent&& cur, TermNode& term)
	-> decltype(cur(term, ctx))
{
	// XXX: This works around %std::reference_wrapper in libstdc++ to function
	//	type in C++2a mode with Clang++. See
	//	https://gcc.gnu.org/bugzilla/show_bug.cgi?id=93470.
	return ystdex::unref(cur)(term, ctx);
}
#endif
YB_ATTR_always_inline inline ReductionStatus
RelayDirect(ContextNode& ctx, const Continuation& cur, TermNode& term)
{
	return cur.Handler(term, ctx);
}

template<typename _fCurrent>
YB_ATTR_always_inline inline ReductionStatus
RelayCurrentOrDirect(ContextNode& ctx, _fCurrent&& cur, TermNode& term)
{
#	if !NPL_Impl_NPLA1_Enable_Thunked || NPL_Impl_NPLA1_Enable_InlineDirect
	return A1::RelayDirect(ctx, yforward(cur), term);
#	else
	yunused(term);
	return A1::RelayCurrent(ctx, yforward(cur));
#	endif
}
//! \since build 926
template<typename _fCurrent>
YB_ATTR_always_inline inline ReductionStatus
RelayCurrentOrDirect(ContextNode& ctx, trivial_swap_t, _fCurrent&& cur,
	TermNode& term)
{
#	if !NPL_Impl_NPLA1_Enable_Thunked || NPL_Impl_NPLA1_Enable_InlineDirect
	return A1::RelayDirect(ctx, yforward(cur), term);
#	else
	yunused(term);
	return A1::RelayCurrent(ctx, trivial_swap, yforward(cur));
#	endif
}
//@}

//! \since build 910
template<typename _fCurrent, typename _fNext>
inline ReductionStatus
RelayCurrentNext(ContextNode& ctx, TermNode& term, _fCurrent&& cur,
	_fNext&& next)
{
#if NPL_Impl_NPLA1_Enable_Thunked
	NPL::RelaySwitched(ctx, yforward(next));
	return A1::RelayCurrentOrDirect(ctx, yforward(cur), term);
#else
	A1::RelayDirect(ctx, yforward(cur), term);
	return ystdex::expand_proxy<ReductionStatus(ContextNode&)>::call(
		yforward(next), ctx);
#endif
}
//! \since build 926
template<typename _fCurrent, typename _fNext>
inline ReductionStatus
RelayCurrentNext(ContextNode& ctx, TermNode& term, _fCurrent&& cur,
	trivial_swap_t, _fNext&& next)
{
#if NPL_Impl_NPLA1_Enable_Thunked
	NPL::RelaySwitched(ctx, trivial_swap, yforward(next));
	return A1::RelayCurrentOrDirect(ctx, yforward(cur),
		term);
#else
	return A1::RelayCurrentNext(ctx, term, yforward(cur), yforward(next));
#endif
}
//! \since build 926
template<typename _fCurrent, typename _fNext>
inline ReductionStatus
RelayCurrentNext(ContextNode& ctx, TermNode& term, trivial_swap_t,
	_fCurrent&& cur, _fNext&& next)
{
#if NPL_Impl_NPLA1_Enable_Thunked
	NPL::RelaySwitched(ctx, yforward(next));
	return A1::RelayCurrentOrDirect(ctx, trivial_swap, yforward(cur),
		term);
#else
	return A1::RelayCurrentNext(ctx, term, yforward(cur), yforward(next));
#endif
}
//! \since build 926
template<typename _fCurrent, typename _fNext>
inline ReductionStatus
RelayCurrentNext(ContextNode& ctx, TermNode& term, trivial_swap_t,
	_fCurrent&& cur, trivial_swap_t, _fNext&& next)
{
#if NPL_Impl_NPLA1_Enable_Thunked
	NPL::RelaySwitched(ctx, trivial_swap, yforward(next));
	return A1::RelayCurrentOrDirect(ctx, trivial_swap, yforward(cur),
		term);
#else
	return A1::RelayCurrentNext(ctx, term, yforward(cur), yforward(next));
#endif
}

//! \since build 916
template<typename _fCurrent, typename _fNext>
inline ReductionStatus
RelayCurrentNextThunked(ContextNode& ctx, TermNode& term, _fCurrent&& cur,
	_fNext&& next)
{
#if NPL_Impl_NPLA1_Enable_Thunked
	yunused(term);
	NPL::RelaySwitched(ctx, yforward(next));
	return A1::RelayCurrent(ctx, yforward(cur));
#else
	return A1::RelayCurrentNext(ctx, term, yforward(cur), yforward(next));
#endif
}
//! \since build 926
template<typename _fCurrent, typename _fNext>
inline ReductionStatus
RelayCurrentNextThunked(ContextNode& ctx, TermNode& term, _fCurrent&& cur,
	trivial_swap_t, _fNext&& next)
{
#if NPL_Impl_NPLA1_Enable_Thunked
	yunused(term);
	NPL::RelaySwitched(ctx, trivial_swap, yforward(next));
	return A1::RelayCurrent(ctx, yforward(cur));
#else
	return A1::RelayCurrentNext(ctx, term, yforward(cur), yforward(next));
#endif
}
//! \since build 926
template<typename _fCurrent, typename _fNext>
inline ReductionStatus
RelayCurrentNextThunked(ContextNode& ctx, TermNode& term, trivial_swap_t,
	_fCurrent&& cur, _fNext&& next)
{
#if NPL_Impl_NPLA1_Enable_Thunked
	yunused(term);
	NPL::RelaySwitched(ctx, yforward(next));
	return A1::RelayCurrent(ctx, trivial_swap, yforward(cur));
#else
	return A1::RelayCurrentNext(ctx, term, yforward(cur), yforward(next));
#endif
}
//! \since build 926
template<typename _fCurrent, typename _fNext>
inline ReductionStatus
RelayCurrentNextThunked(ContextNode& ctx, TermNode& term, trivial_swap_t,
	_fCurrent&& cur, trivial_swap_t, _fNext&& next)
{
#if NPL_Impl_NPLA1_Enable_Thunked
	yunused(term);
	NPL::RelaySwitched(ctx, trivial_swap, yforward(next));
	return A1::RelayCurrent(ctx, trivial_swap, yforward(cur));
#else
	return A1::RelayCurrentNext(ctx, term, yforward(cur), yforward(next));
#endif
}

//! \since build 926
template<typename... _tParams>
// XXX: This is a workaround for G++'s LTO bug.
#if YB_IMPL_GNUCPP >= 100000 || !NPL_Impl_NPLA1_Enable_Thunked \
	|| NPL_Impl_NPLA1_Enable_TCO
YB_FLATTEN
#endif
inline ReductionStatus
ReduceCurrentNext(TermNode& term, ContextNode& ctx, _tParams&&... args)
{
	SetupNextTerm(ctx, term);
	return A1::RelayCurrentNext(ctx, term, yforward(args)...);
}

//! \since build 926
template<typename... _tParams>
#if YB_IMPL_GNUCPP >= 100000 || !NPL_Impl_NPLA1_Enable_Thunked \
	|| NPL_Impl_NPLA1_Enable_TCO
YB_FLATTEN
#endif
inline ReductionStatus
ReduceCurrentNextThunked(TermNode& term, ContextNode& ctx, _tParams&&... args)
{
	SetupNextTerm(ctx, term);
	return A1::RelayCurrentNextThunked(ctx, term, yforward(args)...);
}


//! \since build 821
//@{
//! \pre 最后一个参数的类型退化后可平凡交换。
//@{
template<typename _fNext>
inline ReductionStatus
RelaySubsequent(ContextNode& ctx, TermNode& term, _fNext&& next)
{
	// XXX: The %std::reference_wrapper instance is specialized enough without
	//	%trivial_swap.
	return A1::RelayCurrentNext(ctx, term,
		std::ref(ContextState::Access(ctx).ReduceOnce), trivial_swap,
		yforward(next));
}

//! \since build 945
template<typename _fNext>
inline ReductionStatus
ReduceSubsequent(TermNode& term, ContextNode& ctx, _fNext&& next)
{
	SetupNextTerm(ctx, term);
	return A1::RelaySubsequent(ctx, term, yforward(next));
}
//@}

//! \note 同 ReduceSubsequent ，但不要求最后一个参数的类型退化后满足可平凡交换。
template<typename _fNext>
inline ReductionStatus
ReduceSubsequentPinned(TermNode& term, ContextNode& ctx, _fNext&& next)
{
	return A1::ReduceCurrentNext(term, ctx,
		std::ref(ContextState::Access(ctx).ReduceOnce), yforward(next));
}
//@}


//! \since build 911
//@{
struct NonTailCall final
{
#if NPL_Impl_NPLA1_Enable_Thunked
	//! \since build 947
	YB_ATTR_nodiscard YB_FLATTEN static Reducer
	MakeLiftResult(TermNode& term, ContextNode& ctx)
	{
		// NOTE: The term to be lifted is fixed regardless of continuation
		//	capture, so this needs no setup of the next term and the reducer
		//	does not need to be a %Continuation.
		// XXX: However, %Containuation is actually more efficient. Whether
		//	%Continuation is used, %NPL::ToReducer here would be a bit
		//	inefficient.
#if true
		return Continuation(NameTypedContextHandler([&]{
			return ReduceForLiftedResult(term);
		}, "eval-lift-result"), ctx);
#else
		yunused(ctx);
		return NameTypedContextHandler([&]{
			return ReduceForLiftedResult(term);
		}, "eval-lift-result");
#endif
	}
#endif

	/*!
	\pre 最后一个参数的类型退化后可平凡交换。
	\pre 断言：第三参数中的环境非空。
	*/
	//@{
	template<typename _fCurrent>
	YB_FLATTEN static inline ReductionStatus
	RelayNextGuarded(ContextNode& ctx, TermNode& term, EnvironmentGuard&& gd,
		_fCurrent&& cur)
	{
		YAssert(gd.func.SavedPtr, "Invalid guard found.");
		// XXX: For thunked code, %cur shall likely be a %Continuation before
		//	being captured and it is not capturable here. No %SetupNextTerm
		//	needs to be called here. Otherwise, %cur is not a %Contiuation and
		//	it shall still handle the capture of the term by itself. The %term
		//	is optionally used in direct calls instead of the next term setup,
		//	while they shall be equivalent.
#if NPL_Impl_NPLA1_Enable_Thunked
		// TODO: Blocked. Use C++14 lambda initializers to simplify the
		//	implementation.
		return A1::RelayCurrentNext(ctx, term, yforward(cur), trivial_swap,
			MoveKeptGuard(gd));
#else
		yunused(gd);
		return A1::RelayDirect(ctx, cur, term);
#endif
	}

	template<typename _fCurrent>
	static inline ReductionStatus
	RelayNextGuardedLifted(ContextNode& ctx, TermNode& term,
		EnvironmentGuard&& gd, _fCurrent&& cur)
	{
		YAssert(gd.func.SavedPtr, "Invalid guard found.");
		// XXX: See %RelayNextGuarded.
#if NPL_Impl_NPLA1_Enable_Thunked
		auto act(MoveKeptGuard(gd));

		// XXX: Call of %SetupNextTerm is not needed because the next term is
		//	guaranteed not changed whether %cur is a continuation.
		RelaySwitched(ctx, trivial_swap, std::move(act));
		// XXX: %Reducer is specialized enough without %trivial_swap.
		return A1::RelayCurrentNext(ctx, term, trivial_swap, yforward(cur),
			MakeLiftResult(term, ctx));
#else
		yunused(gd);
		RelayDirect(ctx, cur, term);
		return ReduceForLiftedResult(term);
#endif
	}

	template<typename _fCurrent>
	static ReductionStatus
	RelayNextGuardedProbe(ContextNode& ctx, TermNode& term,
		EnvironmentGuard&& gd, bool lift, _fCurrent&& cur)
	{
		YAssert(gd.func.SavedPtr, "Invalid guard found.");
		// XXX: See %RelayNextGuarded.
#if NPL_Impl_NPLA1_Enable_Thunked
		// TODO: Blocked. Use C++14 lambda initializers to simplify the
		//	implementation.
		auto act(MoveKeptGuard(gd));

		if(lift)
		{
			// XXX: As %RelayNextGuardedLifted.
			RelaySwitched(ctx, trivial_swap, std::move(act));
			// XXX: As %RelayNextGuardedLifted.
			return A1::RelayCurrentNext(ctx, term, trivial_swap, yforward(cur),
				MakeLiftResult(term, ctx));
		}
		return A1::RelayCurrentNext(ctx, term, trivial_swap, yforward(cur),
			trivial_swap, std::move(act));
#else
		yunused(gd);

		const auto res(RelayDirect(ctx, cur, term));

		return lift ? ReduceForLiftedResult(term) : res;
#endif
	}
	//@}

	//! \pre TCO 实现：当前动作非 TCO 动作。
	static void
	SetupForNonTail(ContextNode& ctx, TermNode& term)
	{
#if NPL_Impl_NPLA1_Enable_TCO
		YAssert(!AccessTCOAction(ctx), "Non-tail context expected.");
#endif
		// XXX: Assume the next action is not a %TCOAction.
		ctx.LastStatus = ReductionStatus::Neutral;
		// NOTE: This does not rely on the existence of the %TCOAction, as it
		//	will call %EnsureTCOAction. Since the call would rely on %TCOAction,
		//	anyway, creating the TCO action here instead of a new action
		//	to restore the environment is more efficient.
#if NPL_Impl_NPLA1_Enable_TCO
		SetupTailTCOAction(ctx, term, {});
#else
		yunused(term);
#endif
	}
};


struct TailCall final
{
	/*!
	\pre 最后一个参数的类型退化后可平凡交换。
	\pre 断言：第三参数中的环境非空。
	\pre TCO 实现：同 PrepareTCOEvaluation 。
	*/
	//@{
	template<typename _fCurrent>
	YB_FLATTEN static inline ReductionStatus
	RelayNextGuarded(ContextNode& ctx, TermNode& term, EnvironmentGuard&& gd,
		_fCurrent&& cur)
	{
		// XXX: See %NonTailCall::RelayNextGuarded.
#if NPL_Impl_NPLA1_Enable_TCO
		PrepareTCOEvaluation(ctx, term, std::move(gd));
		return A1::RelayCurrentOrDirect(ctx, trivial_swap, yforward(cur), term);
#else
		YAssert(gd.func.SavedPtr, "Invalid guard found.");
		return NonTailCall::RelayNextGuarded(ctx, term, std::move(gd),
			yforward(cur));
#endif
	}

	template<typename _fCurrent>
	static inline ReductionStatus
	RelayNextGuardedLifted(ContextNode& ctx, TermNode& term,
		EnvironmentGuard&& gd, _fCurrent&& cur)
	{
		// XXX: See %NonTailCall::RelayNextGuardedLifted.
#if NPL_Impl_NPLA1_Enable_TCO
		PrepareTCOEvaluation(ctx, term, std::move(gd)).SetupLift();
		return A1::RelayCurrentOrDirect(ctx, trivial_swap, yforward(cur), term);
#else
		YAssert(gd.func.SavedPtr, "Invalid guard found.");
		return NonTailCall::RelayNextGuardedLifted(ctx, term, std::move(gd),
			yforward(cur));
#endif
	}

	template<typename _fCurrent>
	static inline ReductionStatus
	RelayNextGuardedProbe(ContextNode& ctx, TermNode& term,
		EnvironmentGuard&& gd, bool lift, _fCurrent&& cur)
	{
		// XXX: See %TailCall::RelayNextGuarded.
#if NPL_Impl_NPLA1_Enable_TCO
		PrepareTCOEvaluation(ctx, term, std::move(gd)).SetupLift(lift);
		return A1::RelayCurrentOrDirect(ctx, trivial_swap, yforward(cur), term);
#else
		YAssert(gd.func.SavedPtr, "Invalid guard found.");
		return NonTailCall::RelayNextGuardedProbe(ctx, term, std::move(gd),
			lift, yforward(cur));
#endif
	}
	//@}

	static void
	SetupForNonTail(ContextNode&, TermNode&) ynothrow
	{}
};


template<class _tTraits>
struct Combine final
{
	// XXX: A combination of an operative and its operand shall always be
	//	evaluated in a tail context. However, for a combiner call, sometimes it
	//	is not in the tail context in the enclosing context. If the call needs
	//	no creation of %TCOAction (e.g. fully implemented native code), avoiding
	//	calling %PrepareTCOEvaluation needs no precondition of the existence of
	//	%TCOAction, and this (with %_tTraits as %TailCall) can be an
	//	optimization.
	//! \pre TCO 实现：当前动作是 TCO 动作，且其中的当前项和被规约的项相同。
	//@{
	// XXX: Do not use %YB_FLATTEN here for LTO compiling performance.
	//! \pre 断言：第三参数中的环境非空。
	static ReductionStatus
	RelayEnvSwitch(ContextNode& ctx, TermNode& term, EnvironmentGuard gd)
	{
		// NOTE: %ReduceFirst and other passes would not be called again. The
		//	unwrapped combiner should have been evaluated and it would not be
		//	wrongly evaluated again.
		// XXX: Consider optimization when the combiner subterm is known
		//	regular.
		// NOTE: Term is set in %_tTraits::RelayNextGuarded, even for direct
		//	inlining call of %ReduceCombinedBranch.
		// NOTE: The precondition is similar to the last call in
		//	%EvalImplUnchecked in the presense of the assumption that %term is
		//	from the current term saved in the context.
		// XXX: The %std::reference_wrapper instance is specialized enough
		//	without %trivial_swap.
		return _tTraits::RelayNextGuarded(ctx, term, std::move(gd),
			std::ref(ReduceCombinedBranch));
	}
	//! \pre 间接断言：环境指针参数非空。
	static ReductionStatus
	RelayEnvSwitch(ContextNode& ctx, TermNode& term,
		shared_ptr<Environment> p_env)
	{
		return RelayEnvSwitch(ctx, term, EnvironmentGuard(ctx,
			ctx.SwitchEnvironmentUnchecked(std::move(p_env))));
	}

	// NOTE: A %_tGuardOrEnv is either a guard or a strong reference to an
	//	environment supported by %RelayEnvSwitch respectively. A strong
	//	reference is not an empty pointer.
	template<class _tGuardOrEnv>
	// XXX: Do not use %YB_FLATTEN here for LTO compiling performance.
	static inline ReductionStatus
	ReduceEnvSwitch(TermNode& term, ContextNode& ctx, _tGuardOrEnv&& gd_or_env)
	{
		_tTraits::SetupForNonTail(ctx, term);
		// NOTE: The next term is set here unless the direct inlining call in
		//	%ReduceCombinedBranch is used.
#if !NPL_Impl_NPLA1_Enable_InlineDirect
		SetupNextTerm(ctx, term);
#endif
		return RelayEnvSwitch(ctx, term, yforward(gd_or_env));
	}

	// NOTE: As %ReduceSubsequent.
	template<typename _fNext, class _tGuardOrEnv>
	static ReductionStatus
	ReduceCallSubsequent(TermNode& term, ContextNode& ctx,
		_tGuardOrEnv&& gd_or_env, _fNext&& next)
	{
		return A1::ReduceCurrentNext(term, ctx,
			// TODO: Blocked. Use C++14 lambda initializers to simplify the
			//	implementation.
			ystdex::bind1([](TermNode& t, ContextNode& c, _tGuardOrEnv& g_e){
			return ReduceEnvSwitch(t, c, std::move(g_e));
		}, std::placeholders::_2, std::move(gd_or_env)), yforward(next));
	}
	//@}
};
//@}


//! \since build 881
using Action = ystdex::unchecked_function<void()>;


//! \since build 897
YB_ATTR_nodiscard YB_PURE inline
	PDefH(TermReference, EnsureLValueReference, const TermReference& ref)
	// XXX: Use %TermReference::SetTags is not efficient here.
	ImplRet(TermReference(ref.GetTags() & ~TermTags::Unique, ref))
//! \since build 869
YB_ATTR_nodiscard YB_PURE inline
	PDefH(TermReference, EnsureLValueReference, TermReference&& ref)
	// XXX: Use %TermReference::SetTags is not efficient here.
	ImplRet(TermReference(ref.GetTags() & ~TermTags::Unique, std::move(ref)))


//! \since build 920
inline PDefH(void, SetEvaluatedReference, TermNode& term, const TermNode& bound,
	const TermReference& ref)
	// XXX: It is assumed that %term is not an ancestor of %bound. The source
	//	term tags are ignored.
	ImplExpr(YAssert(TryAccessLeafAtom<const TermReference>(bound).get()
		== &ref, "Invalid term or reference value found."), term.SetContent(
		bound.GetContainer(), EnsureLValueReference(TermReference(ref))))

/*!
\pre 第三参数非空。
\since build 944
*/
inline PDefH(void, SetEvaluatedValue, TermNode& term, TermNode& bound,
	const shared_ptr<Environment>& p_env)
	ImplExpr(term.Value = [&](Environment& env){
		return ValueObject(std::allocator_arg, term.get_allocator(),
			in_place_type<TermReference>,
			env.MakeTermTags(bound) & ~TermTags::Unique, bound,
			EnvironmentReference(p_env, env.GetAnchorPtr()));
	}(NPL::Deref(p_env)))


//! \since build 939
YB_NORETURN void
ThrowNestedParameterTreeMismatch();

//! \since build 949
YB_NORETURN inline PDefH(void, ThrowFormalParameterTypeError,
	const TermNode& term, bool has_ref, size_t n_skip = 0)
	ImplExpr(
	ThrowTypeErrorForInvalidType(type_id<TokenValue>(), term, has_ref, n_skip))

//! \since build 917
char
ExtractSigil(string_view&);

//! \since build 942
template<typename _func>
YB_FLATTEN inline void
HandleOrIgnore(_func f, const TermNode& t, bool t_has_ref)
{
	if(const auto p = TermToNamePtr(t))
		f(*p);
	else if(!IsIgnore(t))
		ThrowFormalParameterTypeError(t, t_has_ref);
}


//! \since build 917
template<typename _fBindValue>
class GParameterValueMatcher final
{
	//! \since build 949
	using MEntry = pair<TNCIter, TNCIter>;

public:
	_fBindValue BindValue;

private:
	//! \since build 949
	mutable YSLib::stack<MEntry, vector<MEntry>> remained;

public:
	//! \since build 949
	template<typename... _tParams>
	inline
	GParameterValueMatcher(TermNode::allocator_type a, _tParams&&... args)
		: BindValue(yforward(args)...), remained(a)
	{}

	void
	operator()(const TermNode& t) const
	{
		// As %ParameterCheck::WrapCall in NPLA1.cpp.
		try
		{
			Match(t, {});
			// NOTE: This is a trampoline to eliminate the call depth
			//	limitation.
			while(!remained.empty())
			{
				auto& e(remained.top());

				if(e.first == e.second)
					remained.pop();
				else
					Match(NPL::Deref(e.first++), {});
			}
		}
		CatchExpr(ParameterMismatch&, throw)
		CatchExpr(..., ThrowNestedParameterTreeMismatch())
	}

private:
	void
	Match(const TermNode& t, bool indirect) const
	{
		if(IsPair(t))
			MatchPair(t);
		else if(!IsEmpty(t))
		{
			if(const auto p_t = TryAccessLeafAtom<const TermReference>(t))
			{
				auto& nd(p_t->get());

				if(IsPair(nd))
					MatchPair(nd);
				else if(!IsEmpty(nd))
					MatchLeaf(nd, true);
			}
			else
				MatchLeaf(t, indirect);
		}
	}

	//! \since build 951
	void
	MatchLeaf(const TermNode& t, bool indirect) const
	{
		YAssert(!IsList(t), "Invalid term found.");
		HandleOrIgnore(std::ref(BindValue), t, indirect);
	}

	//! \since build 951
	void
	MatchPair(const TermNode& t) const
	{
		YAssert(IsPair(t), "Invalid term found.");
		if(IsBranch(t))
			remained.emplace(t.begin(), t.end());
	}
};

/*!
\relates GParameterValueMatcher
\since build 949
*/
template<typename _fBindValue>
YB_ATTR_nodiscard inline GParameterValueMatcher<_fBindValue>
MakeParameterValueMatcher(TermNode::allocator_type a, _fBindValue bind_value)
{
	return GParameterValueMatcher<_fBindValue>(a, std::move(bind_value));
}


// NOTE: This implements binding tags of temporary objects, to allow distinguish
//	bound xvalues from lvalues by inspecting the result of evaluating an
//	identifier (which will never be an xvalue, cf. %EvaluateIdentifier in
//	NPLA1.cpp).
//! \since build 897
YB_ATTR_nodiscard YB_STATELESS yconstfn
	PDefH(TermTags, BindReferenceTags, TermTags ref_tags) ynothrow
	ImplRet(bool(ref_tags & TermTags::Unique) ? ref_tags | TermTags::Temporary
		: ref_tags)
//! \since build 876
YB_ATTR_nodiscard YB_PURE inline
	PDefH(TermTags, BindReferenceTags, const TermReference& ref) ynothrow
	ImplRet(BindReferenceTags(GetLValueTagsOf(ref.GetTags())))


//! \since build 859
class RefContextHandler final
	: private ystdex::equality_comparable<RefContextHandler>
{
private:
#if NPL_NPLA_CheckEnvironmentReferenceCount
	//! \since build 876
	EnvironmentReference environment_ref;
#elif YB_IMPL_GNUCPP < 120000
	// XXX: The anchor pointer here is for more efficient native code generation
	//	(at least with x86_64-pc-linux G++ 9.2, but not G++ 12.1), though there
	//	are still more than necessary memory allocations should have been better
	//	avoided.
	AnchorPtr anchor_ptr;
#endif

public:
	lref<const ContextHandler> HandlerRef;

	//! \since build 869
	RefContextHandler(const ContextHandler& h,
		const EnvironmentReference& env_ref) ynothrow
#if NPL_NPLA_CheckEnvironmentReferenceCount
		: environment_ref(env_ref), HandlerRef(h)
#elif YB_IMPL_GNUCPP < 120000
		: anchor_ptr(env_ref.GetAnchorPtr()), HandlerRef(h)
#else
		: HandlerRef((yunused(env_ref), h))
#endif
	{}
	DefDeCopyMoveCtorAssignment(RefContextHandler)

	YB_ATTR_nodiscard YB_PURE friend PDefHOp(bool, ==,
		const RefContextHandler& x, const RefContextHandler& y)
		ImplRet(x.HandlerRef.get() == y.HandlerRef.get())

	PDefHOp(ReductionStatus, (), TermNode& term, ContextNode& ctx) const
		// NOTE: The referent is uncontionally treated as an lvalue to prevent
		//	the unexpected move of the referenced resources in %HandlerRef (e.g.
		//	in %VauHandler in NPLA1Forms.cpp) for prvalues. See also
		//	%ReduceCombinedBranch in NPLA1.cpp for the introducing of the tag.
		ImplRet(ClearCombiningTags(term), HandlerRef.get()(term, ctx))

// XXX: This is currently unused and Clang++ would complain with
//	[-Wunused-function].
#if !YB_IMPL_CLANGPP
#	if NPL_NPLA_CheckEnvironmentReferenceCount
	friend DefSwap(ynothrow, RefContextHandler,
		(std::swap(_x.environment_ref, _y.environment_ref),
		std::swap(_x.HandlerRef, _y.HandlerRef)))
#	elif YB_IMPL_GNUCPP < 120000
	friend DefSwap(ynothrow, RefContextHandler,
		(std::swap(_x.anchor_ptr, _y.anchor_ptr),
		std::swap(_x.HandlerRef, _y.HandlerRef)))
#	else
	friend DefSwap(ynothrow, RefContextHandler,
		(std::swap(_x.HandlerRef, _y.HandlerRef)))
#	endif
#endif
};


//! \since build 950
//@{
template<class _tAlloc, typename... _tParams>
YB_ATTR_nodiscard YB_ATTR_always_inline inline shared_ptr<TermNode>
AllocateSharedTerm(const _tAlloc& a, _tParams&&... args)
{
	return YSLib::allocate_shared<TermNode>(a, yforward(args)...);
}

template<class _tAlloc, typename... _tParams>
YB_ATTR_nodiscard YB_ATTR_always_inline inline shared_ptr<TermNode>
AllocateSharedTermValue(const _tAlloc& a, _tParams&&... args)
{
	return A1::AllocateSharedTerm(a, NPL::AsTermNode(a, yforward(args)...));
}

YB_ATTR_nodiscard inline PDefH(TermNode, MakeSubobjectReferent,
	TermNode::allocator_type a, shared_ptr<TermNode> p_sub)
	ImplRet(NPL::AsTermNodeTagged(a, TermTags::Sticky, std::move(p_sub)))
//@}


//! \since build 953
ReductionStatus
ReduceAsSubobjectReference(TermNode&, shared_ptr<TermNode>,
	const EnvironmentReference&, TermTags);

//! \since build 913
ReductionStatus
ReduceForCombinerRef(TermNode&, const TermReference&, const ContextHandler&,
	size_t);

} // inline namespace Internals;

} // namesapce A1;

} // namespace NPL;

#endif

