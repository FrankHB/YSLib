// YSLib::Core::YFunc by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-02-14 18:48:44 + 08:00;
// UTime = 2010-11-03 19:53 + 08:00;
// Version = 0.2412;


#ifndef INCLUDED_YFUNC_HPP_
#define INCLUDED_YFUNC_HPP_

// YFunc ：函数对象封装模块。

#include "ysdef.h"
#include <functional>
#include <typeinfo>

YSL_BEGIN

//函数对象基类。
template<typename _tPointer>
class GHBase
{
private:
	mutable _tPointer _ptr; //指针。

protected:
	//********************************
	//名称:		GHBase
	//全名:		YSLib::GHBase<_tPointer>::GHBase
	//可访问性:	protected 
	//返回类型:	
	//修饰符:	
	//形式参数:	_tPointer pf
	//功能概要:	构造：使用指针。
	//备注:		
	//********************************
	inline explicit
	GHBase(_tPointer pf = NULL) : _ptr(pf)
	{}
	//********************************
	//名称:		GHBase
	//全名:		YSLib::GHBase<_tPointer>::GHBase<P>
	//可访问性:	protected 
	//返回类型:	
	//修饰符:	
	//形式参数:	GHBase<P> pf
	//功能概要:	构造：使用其它 GHBase 对象。
	//备注:		
	//********************************
	template<typename P>
	inline explicit
	GHBase(GHBase<P> pf) : _ptr(reinterpret_cast<_tPointer>(P(pf)))
	{}

public:
	//********************************
	//名称:		operator _tPointer
	//全名:		YSLib::GHBase::operator _tPointer
	//可访问性:	public 
	//返回类型:	
	//修饰符:	const
	//功能概要:	转换：指针。
	//备注:		
	//********************************
	inline
	operator _tPointer() const
	{
		return _ptr;
	}

	//********************************
	//名称:		GetPtr
	//全名:		YSLib::GHBase::GetPtr
	//可访问性:	public 
	//返回类型:	_tPointer
	//修饰符:	const
	//功能概要:	取指针。
	//备注:		
	//********************************
	inline _tPointer
	GetPtr() const
	{
		return _ptr;
	}
};


//调用时动态类型检查函数对象模板类。
template<typename _type, typename _tArg, typename _tRet>
class GHDynamicFunction
	: public std::binary_function<_type, _tArg, _tRet>
{
private:
	const union Pointer
	{
	public:
		_tRet (*_f_ptr)(_type&, _tArg);
		_tRet (_type::*_mf_ptr)(_tArg) const;

		//********************************
		//名称:		Pointer
		//全名:		YSLib::GHDynamicFunction<_type, _tArg, _tRet>::Pointer
		//				::Pointer
		//可访问性:	public 
		//返回类型:	
		//修饰符:	
		//形式参数:	_tRet (&_f_)(_type&, _tArg)
		//功能概要:	构造：使用指定函数引用。
		//备注:		
		//********************************
		explicit
		Pointer(_tRet (&_f_)(_type&, _tArg))
			: _f_ptr(&_f_)
		{}
		//********************************
		//名称:		Pointer
		//全名:		YSLib::GHDynamicFunction<_type, _tArg, _tRet>::Pointer
		//				::Pointer
		//可访问性:	public 
		//返回类型:	
		//修饰符:	
		//形式参数:	_tRet(_type::*_mf_ptr_)(_tArg) const
		//功能概要:	构造：使用指定非静态成员函数指针。
		//备注:		
		//********************************
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
	//********************************
	//名称:		GHDynamicFunction
	//全名:		YSLib::GHDynamicFunction<_type, _tArg, _tRet>
	//				::GHDynamicFunction
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	_tRet(&_f)(_type&, _tArg)
	//功能概要:	构造：使用指定函数引用。
	//备注:		
	//********************************
	explicit
	GHDynamicFunction(_tRet(&_f)(_type&, _tArg))
		: _m_ptr(_f), _state(_func)
	{}
	//********************************
	//名称:		GHDynamicFunction
	//全名:		YSLib::GHDynamicFunction<_type, _tArg, _tRet>
	//				::GHDynamicFunction
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	_tRet(_type::*_pf)(_tArg) const
	//功能概要:	构造：使用指定非静态成员函数指针。
	//备注:		
	//********************************
	explicit
	GHDynamicFunction(_tRet(_type::*_pf)(_tArg) const)
		: _m_ptr(_pf), _state(_mem_func)
	{}

	//********************************
	//名称:		operator()
	//全名:		YSLib::GHDynamicFunction<_type, _tArg, _tRet>::operator()
	//可访问性:	public 
	//返回类型:	_tRet
	//修饰符:	const
	//形式参数:	const _type & _r
	//形式参数:	_tArg _x
	//功能概要:	调用：使用 _type 类型参数。
	//备注:		无 dynamic_cast 。
	//********************************
	_tRet
	operator()(_type& _r, _tArg _x) const
	{
		if(_state == _func)
			return _m_ptr._f_ptr(_r, _x);
	//	if(_state == _mem_func && _m_ptr._mf_ptr)
		else if(_m_ptr._mf_ptr)
			return (_r.*_m_ptr._mf_ptr)(_x);
	}
	//********************************
	//名称:		operator()
	//全名:		YSLib::GHDynamicFunction<_type, _tArg, _tRet>
	//				::operator()<_tNew>
	//可访问性:	public 
	//返回类型:	_tRet
	//修饰符:	const
	//形式参数:	const _tNew & _r
	//形式参数:	_tArg _x
	//功能概要:	调用：使用非 _type 类型参数。
	//备注:		有 dynamic_cast 。
	//********************************
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


//********************************
//名称:		ConstructDynamicFunctionWith
//全名:		YSLib::ConstructDynamicFunctionWith<_type, _tArg, _tRet>
//可访问性:	public 
//返回类型:	GHDynamicFunction<_type, _tArg, _tRet>
//修饰符:	
//形式参数:	_tRet (&_f)(_type&, _tArg)
//功能概要:	帮助函数：使用指定函数引用构造
//			GHDynamicFunction<_type, _tArg, _tRet> 对象。
//备注:		
//********************************
template<typename _type, typename _tArg, typename _tRet>
inline GHDynamicFunction<_type, _tArg, _tRet>
ConstructDynamicFunctionWith(_tRet (&_f)(_type&, _tArg))
{
	return GHDynamicFunction<_type, _tArg, _tRet>(_f);
}
//********************************
//名称:		ConstructDynamicFunctionWith
//全名:		YSLib::ConstructDynamicFunctionWith<_tRet, _type, _tArg>
//可访问性:	public 
//返回类型:	
//修饰符:	
//形式参数:	_tRet 
//形式参数:	_type:: * _f
//形式参数:	_tArg
//形式参数:	const
//功能概要:	帮助函数：使用指定非静态成员函数指针构造
//			GHDynamicFunction<_type, _tArg, _tRet> 对象。
//备注:		
//********************************
template<typename _tRet, typename _type, typename _tArg>
inline GHDynamicFunction<_tRet, _type, _tArg>
ConstructDynamicFunctionWith(_tRet (_type::*_f)(_tArg) const)
{
	return GHDynamicFunction<_tRet, _type, _tArg>(_f);
}


//函数对象类：替换非静态成员二元函数的第一个参数。
template<class _type, typename _tRet, typename _tPara, class _tNew = _type>
struct ExpandMemberFirst
{
private:
	_tRet(_type::*_pm)(_tPara);

public:
	//********************************
	//名称:		ExpandMemberFirst
	//全名:		YSLib::Runtime::ExpandMemberFirst<_type, _tRet, _tPara, _tNew>
	//				::ExpandMemberFirst
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	_tRet(_type::*p)(_tPara)
	//功能概要:	构造：使用函数指针。
	//备注:		
	//********************************
	ExpandMemberFirst(_tRet(_type::*p)(_tPara))
		: _pm(p)
	{}

	//********************************
	//名称:		operator==
	//全名:		YSLib::Runtime::ExpandMemberFirst<_type, _tRet, _tPara, _tNew>
	//				::operator==
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	const ExpandMemberFirst & rhs
	//功能概要:	比较：相等关系。
	//备注:		
	//********************************
	bool
	operator==(const ExpandMemberFirst& rhs) const
	{
		return _pm == rhs._pm;
	}

	//********************************
	//名称:		operator()
	//全名:		YSLib::Runtime::ExpandMemberFirst<_type, _tRet, _tPara, _tNew>
	//				::operator()
	//可访问性:	public 
	//返回类型:	_tRet
	//修饰符:	
	//形式参数:	_type & o
	//形式参数:	_tPara arg
	//功能概要:	调用：使用对象引用和参数。
	//备注:		检测空指针。
	//********************************
	_tRet
	operator()(_type& o, _tPara arg)
	{
		if(_pm)
			return o.*_pm(arg);
	}
	//********************************
	//名称:		operator()
	//全名:		YSLib::Runtime::ExpandMemberFirst<_type, _tRet, _tPara, _tNew>
	//				::operator()
	//可访问性:	public 
	//返回类型:	_tRet
	//修饰符:	
	//形式参数:	_tNew & o
	//形式参数:	_tPara arg
	//功能概要:	调用：使用非 _type 类型对象引用和参数。
	//备注:		检测空指针；使用 dynamic_cast 测试类型。
	//********************************
	_tRet
	operator()(_tNew& o, _tPara arg)
	{
		if(_pm)
			try
			{
				return (dynamic_cast<_type&>(o).*_pm)(arg);
			}
			catch(std::bad_cast&)
			{}
	}
};

//逆向柯里化：在参数列表起始添加一个形式参数。
template<class _tFunc, typename _tParm>
struct InversedCurrying
{
	typedef typename _tFunc::Result Result;
	typedef typename _tFunc::Parm1 Parm1;

	_tFunc f;

	//********************************
	//名称:		InversedCurrying
	//全名:		YSLib::Runtime::InversedCurrying<_tFunc, _tParm>
	//				::InversedCurrying
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	_tFunc f_
	//功能概要:	构造：使用函数对象。
	//备注:		
	//********************************
	InversedCurrying(_tFunc f_)
		: f(f_)
	{}

	PDefHOperator(bool, ==, const InversedCurrying& r) const
		ImplRet(f == r.f)

	//********************************
	//名称:		operator()
	//全名:		YSLib::Runtime::InversedCurrying<_tFunc, _tParm>::operator()
	//可访问性:	public 
	//返回类型:	Result
	//修饰符:	const
	//形式参数:	_tParm
	//形式参数:	Parm1 arg1
	//功能概要:	调用：忽略第一个参数。
	//备注:		
	//********************************
	Result
	operator()(_tParm, Parm1 arg1) const
	{
		return f(arg1);
	}
};

YSL_END

#endif

