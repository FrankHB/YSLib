/*
	© 2009-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YApplication.h
\ingroup Core
\brief 系统资源和应用程序实例抽象。
\version r1773
\author FrankHB <frankhb1989@gmail.com>
\since build 577
\par 创建时间:
	2009-12-27 17:12:27 +0800
\par 修改时间:
	2016-09-03 10:18 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YApplication
*/


#ifndef YSL_INC_Core_YApplication_h_
#define YSL_INC_Core_YApplication_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YShell // for Shell, stack,
//	std::is_nothrow_copy_constructible, locked_ptr, ystdex::decay_t;
#include <ystdex/any.h> // for ystdex::any, ystdex::unchecked_any_cast;
#include <ystdex/scope_guard.hpp> // for ystdex::unique_guard;

namespace YSLib
{

using Messaging::MessageQueue;

/*!
\brief 程序实例。
\since build 243
*/
class YF_API Application : public Shell
{
private:
	/*!
	\brief 初始化守护。
	\since build 693
	*/
	stack<ystdex::any> on_exit{};
	/*
	\brief 初始化守护互斥锁。
	\since build 725
	*/
	recursive_mutex on_exit_mutex{};
	/*
	\brief 主消息队列互斥锁。
	\since build 551
	*/
	recursive_mutex queue_mutex{};

protected:
	/*
	\brief 主消息队列。
	\since build 481
	*/
	MessageQueue qMain{};
	/*!
	\brief 当前 Shell 句柄：指示当前线程空间中运行的 Shell 。
	\note 全局单线程，生存期与进程相同。
	*/
	shared_ptr<Shell> hShell{};

public:
	//! \brief 无参数构造：默认构造。
	Application();

	//! \brief 析构：释放 Shell 所有权和其它资源。
	~Application() override;

	//! \brief 取得线程空间中当前运行的 Shell 的句柄。
	DefGetter(const ynothrow, shared_ptr<Shell>, ShellHandle, hShell)

	/*!
	\brief 执行消息队列操作。
	\note 线程安全：全局消息队列互斥访问。
	\since build 481
	*/
	template<typename _func>
	auto
	AccessQueue(_func f) -> decltype(f(qMain))
	{
		lock_guard<recursive_mutex> lck(queue_mutex);

		return f(qMain);
	}

	/*!
	\pre 参数调用无异常抛出。
	\note 线程安全：全局互斥访问。
	\since build 693
	*/
	//@{
	template<typename _tParam>
	void
	AddExit(_tParam&& arg)
	{
		lock_guard<recursive_mutex> lck(on_exit_mutex);

		PushExit(yforward(arg));
	}

	template<typename _func>
	void
	AddExitGuard(_func f)
	{
		static_assert(std::is_nothrow_copy_constructible<_func>(),
			"Invalid guard function found.");
		lock_guard<recursive_mutex> lck(on_exit_mutex);

		TryExpr(PushExit(ystdex::unique_guard(f)))
		catch(...)
		{
			f();
			throw;
		}
	}

	/*!
	\brief 锁定添加的初始化守护。
	\note 线程安全：全局初始化守护互斥访问。
	\since build 481
	*/
	template<typename _tParam>
	locked_ptr<ystdex::decay_t<_tParam>, recursive_mutex>
	LockAddExit(_tParam&& arg)
	{
		unique_lock<recursive_mutex> lck(on_exit_mutex);

		PushExit(yforward(arg));
		return {ystdex::unchecked_any_cast<ystdex::decay_t<_tParam>>(
			&on_exit.top()), std::move(lck)};
	}
	//@}

	/*!
	\brief 处理消息：分发消息。
	\pre 断言：当前 Shell 句柄有效。
	\exception 捕获并忽略 Messaging::MessageSignal ，其它异常中立。
	\since build 317
	*/
	void
	OnGotMessage(const Message&) override;

private:
	//! \since build 725
	template<typename _tParam>
	inline void
	PushExit(_tParam&& arg)
	{
		on_exit.push(yforward(arg));
	}

public:
	/*!
	\brief 切换：若参数非空，和线程空间中当前运行的 Shell 的句柄交换。
	\return 参数是否有效。
	\since build 295
	*/
	bool
	Switch(shared_ptr<Shell>&) ynothrow;
	/*!
	\brief 切换：若参数非空，和线程空间中当前运行的 Shell 的句柄交换。
	\return 参数是否有效。
	\warning 空句柄在此处是可接受的，但继续运行可能会导致断言失败。
	\since build 295
	*/
	PDefH(bool, Switch, shared_ptr<Shell>&& h) ynothrow
		ImplRet(Switch(h))
};


/*!
\brief 取应用程序实例。
\throw LoggedEvent 找不到全局应用程序实例或消息发送失败。
\note 保证在平台相关的全局资源初始化之后初始化此实例。
\note 线程安全。
\since build 550
*/
extern YF_API Application&
FetchAppInstance();

/*!
\ingroup helper_functions
\brief 取当前应用程序线程空间中活动的 Shell 句柄。
\warning 应用程序实例初始化前不保证线程安全性。
*/
inline PDefH(shared_ptr<Shell>, FetchShellHandle, ) ynothrow
	ImplRet(FetchAppInstance().GetShellHandle())

/*!
\ingroup helper_functions
\brief 激活 Shell 对象：控制权转移给此对象以维持单线程运行。
\pre 断言： <tt>bool(hShl)</tt> 。
\warning 不保证线程安全性。
*/
inline PDefH(bool, Activate, const shared_ptr<Shell>& hShl)
	ImplRet(FetchAppInstance().Switch(shared_ptr<Shell>(hShl)))


/*!
\brief 默认退出消息优先级。
\since build 696
*/
yconstexpr const Messaging::Priority DefaultQuitPriority(0xF0);

/*!
\brief 全局默认队列消息发送函数。
\exception LoggedEvent 找不到全局应用程序实例或消息发送失败。
\note 线程安全。
\since build 550
*/
//@{
YF_API void
PostMessage(const Message&, Messaging::Priority);
inline PDefH(void, PostMessage, Messaging::ID id, Messaging::Priority prior,
	const ValueObject& vo = {})
	ImplRet(PostMessage(Message(id, vo), prior))
inline PDefH(void, PostMessage, Messaging::ID id, Messaging::Priority prior,
	ValueObject&& c)
	ImplRet(PostMessage(Message(id, std::move(c)), prior))
template<Messaging::MessageID _vID>
inline void
PostMessage(Messaging::Priority prior,
	const typename Messaging::SMessageMap<_vID>::TargetType& target)
{
	PostMessage(_vID, prior, ValueObject(target));
}
//@}

/*!
\brief 以指定错误码和优先级发起 Shell 终止请求。
\since build 696
*/
YF_API void
PostQuitMessage(int = 0, Messaging::Priority p = DefaultQuitPriority);

} // namespace YSLib;

#endif

