/*
	Copyright (C) by Franksoft 2010 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yfunc.hpp
\ingroup Core
\brief 函数调用和仿函数封装。
\version r878
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-02-14 18:48:44 +0800
\par 修改时间:
	2012-09-04 12:14 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YFunc
*/


#ifndef INCLUDED_CORE_YFUNC_HPP_
#define INCLUDED_CORE_YFUNC_HPP_ 1

#include "ysdef.h"
#include <functional>
#include <typeinfo>

YSL_BEGIN

#if 0
/*
\brief 调用时动态类型检查仿函数模板。
*/
template<typename _type, typename _tArg, typename _tRet>
class GHDynamicFunction
{
private:
	const union Pointer
	{
	public:
		_tRet (*_f_ptr)(_type&, _tArg);
		_tRet (_type::*_mf_ptr)(_tArg) const;

		/*!
		\brief 构造：使用指定函数引用。
		*/
		explicit
		Pointer(_tRet (&_f_)(_type&, _tArg))
			: _f_ptr(&_f_)
		{}
		/*!
		\brief 构造：使用指定非静态成员函数指针。
		*/
		explicit
		Pointer(_tRet(_type::*_mf_ptr_)(_tArg) const)
			: _mf_ptr(_mf_ptr_)
		{}
	} _m_ptr;
	const enum
	{
		_func = 1,
		_mem_func = 2
	} _state;

public:
	/*!
	\brief 构造：使用指定函数引用。
	*/
	explicit
	GHDynamicFunction(_tRet(&_f)(_type&, _tArg))
		: _m_ptr(_f), _state(_func)
	{}
	/*!
	\brief 构造：使用指定非静态成员函数指针。
	*/
	explicit
	GHDynamicFunction(_tRet(_type::*_pf)(_tArg) const)
		: _m_ptr(_pf), _state(_mem_func)
	{}

	/*!
	\brief 调用：使用 _type 类型参数。
	\note 无 dynamic_cast 。
	*/
	_tRet
	operator()(_type& _r, _tArg _x) const
	{
		if(_state == _func)
			return _m_ptr._f_ptr(_r, _x);
	//	if(_state == _mem_func && _m_ptr._mf_ptr)
		else if(_m_ptr._mf_ptr)
			return (_r.*_m_ptr._mf_ptr)(_x);
	}
	/*!
	\brief 调用：使用非 _type 类型参数。
	\note 有 dynamic_cast 。
	*/
	template<class _tNew>
	_tRet
	operator()(const _tNew& _r, _tArg _x) const
	{
		if(_state == _mem_func && _m_ptr._mf_ptr)
		{
			try
			{
				return dynamic_cast<_type&>(_r).*_m_ptr._mf_ptr(_x);
			}
			catch(std::bad_cast&)
			{}
		}
	}
};


/*!
\brief 助手函数：使用指定函数引用构造 GHDynamicFunction<_type, _tArg, _tRet> 对象。
*/
template<typename _type, typename _tArg, typename _tRet>
inline GHDynamicFunction<_type, _tArg, _tRet>
ConstructDynamicFunctionWith(_tRet (&_f)(_type&, _tArg))
{
	return GHDynamicFunction<_type, _tArg, _tRet>(_f);
}
/*!
\brief 助手函数：使用指定非静态成员函数指针构造
	GHDynamicFunction<_type, _tArg, _tRet> 对象。
*/
template<typename _tRet, typename _type, typename _tArg>
inline GHDynamicFunction<_tRet, _type, _tArg>
ConstructDynamicFunctionWith(_tRet (_type::*_f)(_tArg) const)
{
	return GHDynamicFunction<_tRet, _type, _tArg>(_f);
}
#endif


/*!
\brief 仿函数：替换非静态成员二元函数的第一个参数。
\since build 167
*/
template<class _type, typename _tRet, typename _tPara, class _tNew = _type>
struct ExpandMemberFirst
{
private:
	_tRet(_type::*_pm)(_tPara);

public:
	/*!
	\brief 构造：使用函数指针。
	*/
	yconstfn
	ExpandMemberFirst(_tRet(_type::*p)(_tPara))
		: _pm(p)
	{}

	/*!
	\brief 比较：相等关系。
	\since build 319
	*/
	yconstfn bool
	operator==(const ExpandMemberFirst& rhs) const ynothrow
	{
		return _pm == rhs._pm;
	}

	/*!
	\brief 调用：使用对象引用和参数。
	\note 检测空指针。
	*/
	_tRet
	operator()(_type& o, _tPara arg)
	{
		if(YB_LIKELY(_pm))
			return o.*_pm(arg);
	}
	/*!
	\brief 调用：使用非 _type 类型对象引用和参数。
	\note 检测空指针；使用 dynamic_cast 测试类型。
	\since build 293
	*/
	_tRet
	operator()(_tNew& o, _tPara&& arg)
	{
		if(YB_LIKELY(_pm))
			try
			{
				return (dynamic_cast<_type&>(o).*_pm)(yforward(arg));
			}
			catch(std::bad_cast&)
			{}
	}
};


/*!
\brief 仿函数：替换非静态成员二元函数的第一个参数并绑定到指定对象。
\since build 171
*/
template<class _type, typename _tRet, typename _tPara, class _tNew = _type>
struct ExpandMemberFirstBinder
{
private:
	_type* _po;
	_tRet(_type::*_pm)(_tPara);

public:
	/*!
	\brief 构造：使用非 _type 类型对象引用和成员函数指针。
	\note 使用 dynamic_cast 测试类型。
	*/
	yconstfn
	ExpandMemberFirstBinder(_tNew& obj, _tRet(_type::*p)(_tPara))
		: _po(dynamic_cast<_type*>(&obj)), _pm(p)
	{}

	/*!
	\brief 比较：相等关系。
	\since build 319
	*/
	yconstfn bool
	operator==(const ExpandMemberFirstBinder& rhs) const ynothrow
	{
		return _po == rhs._po && _pm == rhs._pm;
	}

	/*!
	\brief 调用：使用替换对象引用和参数。
	\note 检测空指针。
	\since build 293
	*/
	_tRet
	operator()(_tPara&& arg)
	{
		if(YB_LIKELY(_po && _pm))
			return (_po->*_pm)(yforward(arg));
	}
	/*!
	\brief 调用：使用替换对象引用和参数。
	\note 检测空指针。
	\since build 293
	*/
	template<class _tN>
	_tRet
	operator()(_tN&, _tPara&& arg)
	{
		if(YB_LIKELY(_po && _pm))
			return (_po->*_pm)(yforward(arg));
	}
};


#if 0
//! \brief 逆向柯里化：在参数列表起始添加一个形式参数。
template<class _fCallable, typename _tParm>
struct InversedCurrying
{
	typedef typename _fCallable::Result Result;
	typedef typename _fCallable::Parm1 Parm1;

	_fCallable f;

	/*!
	\brief 构造：使用函数对象。
	*/
	InversedCurrying(_fCallable f_)
		: f(f_)
	{}

	PDefHOp(bool, ==, const InversedCurrying& r) const
		ImplRet(f == r.f)

	/*!
	\brief 调用：忽略第一个参数。
	*/
	Result
	operator()(_tParm, Parm1 arg1) const
	{
		return f(arg1);
	}
};


//! \brief 多态函数对象基类。
struct PolymorphicFunctorBase
{
	DefDeDtor(PolymorphicFunctorBase)
};


/*
\brief 多态仿函数模板。
\note 继承其它仿函数。
*/
template<class _tFunctor>
class GFunctor : public PolymorphicFunctorBase, public _tFunctor
{
public:
	GFunctor(_tFunctor&& _f)
		: PolymorphicFunctorBase(), _tFunctor(yforward(_f))
	{}

	template<typename... _tArgs>
	GFunctor(_tArgs&&... _args)
		: PolymorphicFunctorBase(),
		_tFunctor(yforward(_args)...)
	{}
};
#endif

YSL_END

#endif

