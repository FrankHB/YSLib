/*
	© 2015-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file future.hpp
\ingroup YStandardEx
\brief 标准库\c \<future\> 扩展。
\version r120
\author FrankHB <frankhb1989@gmail.com>
\since build 624
\par 创建时间:
	2015-08-18 10:12:00 +0800
\par 修改时间:
	2016-09-21 15:40 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Future
*/


#ifndef YB_INC_ystdex_future_hpp_
#define YB_INC_ystdex_future_hpp_ 1

#include "type_traits.hpp" // for false_, true_, decay_t, result_of_t;
#include <future> // for std::future, std::packaged_task_t;
#include <libdefect/exception.h> // for std::exception_ptr;
#include <memory> // for std::shared_ptr;

namespace ystdex
{

//! \since build 624
//@{
template<typename>
struct is_future_type : false_
{};

template<typename _type>
struct is_future_type<std::future<_type>> : true_
{};


#if !__GLIBCXX__ || (defined(_GLIBCXX_HAS_GTHREADS) \
	&& defined(_GLIBCXX_USE_C99_STDINT_TR1) && (ATOMIC_INT_LOCK_FREE > 1))
template<typename _type>
std::future<decay_t<_type>>
make_ready_future(_type&& val)
{
	using val_t = decay_t<_type>;
	std::promise<val_t> pm;

	pm.set_value(std::forward<val_t>(val));
	return pm.get_future();
}
inline
std::future<void>
make_ready_future()
{
	std::promise<void> pm;

	pm.set_value();
	return pm.get_future();
}
template<typename _type>
std::future<_type>
make_ready_future(std::exception_ptr p)
{
	std::promise<_type> pm;
	pm.set_exception(p);
	return pm.get_future();
}
//@}
#endif


//! \since build 623
template<typename _fCallable, typename... _tParams>
using future_result_t
	= std::future<result_of_t<_fCallable&&(_tParams&&...)>>;

//! \since build 358
template<typename _fCallable, typename... _tParams>
using packed_task_t
	= std::packaged_task<result_of_t<_fCallable&&(_tParams&&...)>()>;


//! \since build 359
//@{
template<typename _fCallable, typename... _tParams>
packed_task_t<_fCallable&&, _tParams&&...>
pack_task(_fCallable&& f, _tParams&&... args)
{
	return packed_task_t<_fCallable&&, _tParams&&...>(
		std::bind(yforward(f), yforward(args)...));
}

template<typename _fCallable, typename... _tParams>
std::shared_ptr<packed_task_t<_fCallable&&, _tParams&&...>>
pack_shared_task(_fCallable&& f, _tParams&&... args)
{
	return std::make_shared<packed_task_t<_fCallable&&,
		_tParams&&...>>(std::bind(yforward(f), yforward(args)...));
}
//@}

} // namespace ystdex;

#endif

