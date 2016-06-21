/*
	© 2010-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YFunc.hpp
\ingroup Core
\brief 函数调用和仿函数封装。
\version r1285
\author FrankHB <frankhb1989@gmail.com>
\since build 560
\par 创建时间:
	2010-02-14 18:48:44 +0800
\par 修改时间:
	2016-06-21 04:15 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YFunc
*/


#ifndef YSL_INC_Core_yfunc_hpp_
#define YSL_INC_Core_yfunc_hpp_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YShellDefinition
#include YFM_YSLib_Adaptor_YTextBase // for string;

namespace YSLib
{

/*!
\brief 递归调用上下文。
\note 用于构建依赖项。
\since build 545
*/
template<typename _tKey, typename _tValue>
class GRecursiveCallContext
{
public:
	using KeyType = _tKey;
	using ValueType = _tValue;
	using CallerType = std::function<ValueType(const GRecursiveCallContext&)>;

private:
	std::function<CallerType(const KeyType&)> build;

public:
	template<typename _func, yimpl(typename
		= ystdex::exclude_self_t<GRecursiveCallContext, _func>)>
	GRecursiveCallContext(_func f)
		: build(f)
	{}
	DefDeCopyMoveCtorAssignment(GRecursiveCallContext)

	PDefHOp(ValueType, (), const KeyType& key) const
		ImplRet(build(key)(*this))
};


/*!
\brief 注册处理器抽象模板：供派生类加载一个或多个键和指定类型关联的例程。
\note 加载的键的数量和类型的数量需要保持一致。
\warning 非虚析构。
\since build 494
*/
template<class _tDerived, typename _tKey, typename _fHandler>
struct GHandlerRegisterBase
{
private:
	unordered_map<_tKey, _fHandler> registered_map{};

public:
	//! \since build 495
	PDefH(bool, Contains, const _tKey& key) const
		ImplRet(registered_map.count(key) != 0)

	DeclSEntry(template<_type, _fHandler> _fHandler GetRegister() const)

	template<typename... _tParams>
	auto
	Call(const _tKey& key, _tParams&&... args)
		-> ystdex::result_of_t<_fHandler&(_tParams&&...)>
	{
		if(const auto f = registered_map[key])
		{
			// TODO: Do right trace.
		//	YTraceDe(Notice, "Found registered handler: %s.",
		//		to_string(key).c_str());

			return f(yforward(args)...);
		}
		return ystdex::result_of_t<_fHandler&(_tParams&&...)>();
	}

	template<class _type>
	void
	Register(const _tKey& key)
	{
		registered_map.emplace(key, static_cast<_tDerived*>(this)
			->template GetRegister<_type, _fHandler>(key));
	}
	template<typename _tIn, class _type, class _tTuple>
	void
	Register(_tIn first, _tIn last)
	{
		YAssert(first != last && std::distance(first, last)
			== decltype(std::distance(first, last))(
			std::tuple_size<_tTuple>::value + 1), "Wrong range found.");
		Register<_type>(*first);
		++first;
		YAssert((first == last) == (std::tuple_size<_tTuple>::value == 0),
			"Wrong number of parameters found.");
	//	static_if(std::tuple_size<_tTuple>::value != 0)
	//		RegisterTail<_tIn, std::tuple_element<0, _tTuple>,
	//			typename tuple_split<_tTuple>::tail>(first, last);
		RegisterTail<_tIn>(ystdex::identity<_tTuple>(), first, last);
	}
	template<class _type, class... _types>
	void
	Register(std::initializer_list<string> il)
	{
		YAssert(il.size() == sizeof...(_types) + 1,
			"Wrong size of initializer list found.");
		Register<std::initializer_list<string>::const_iterator, _type,
			tuple<_types...>>(il.begin(), il.end());
	}

private:
	//! \since build 670
	template<typename _tIn>
	void
	RegisterTail(ystdex::identity<tuple<>>, _tIn first, _tIn last)
	{
		YAssert(first == last, "Wrong size of initializer list found.");
		yunused(first),
		yunused(last);
	}
	//! \since build 670
	template<typename _tIn, class _type, class... _types>
	void
	RegisterTail(ystdex::identity<tuple<_type, _types...>>, _tIn first,
		_tIn last)
	{
		Register<_tIn, _type, tuple<_types...>>(first, last);
	}
};

} // namespace YSLib;

#endif

