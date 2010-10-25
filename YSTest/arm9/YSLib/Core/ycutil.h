// YSLib::Core::YCoreUtilities by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-05-23 06:10:59 + 08:00;
// UTime = 2010-10-24 16:40 + 08:00;
// Version = 0.2154;


#ifndef INCLUDED_YCUTIL_H_
#define INCLUDED_YCUTIL_H_

// YCoreUtilities ：核心实用模块。

#include "ysdef.h"
#include <cstring>
#include <functional>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Design)

//不可复制对象：继承此类的对象在外部无法调用复制构造函数和复制赋值操作符。
class NonCopyable
{
//保护非多态类。
protected:
	//********************************
	//名称:		NonCopyable
	//全名:		YSLib::Design::NonCopyable::NonCopyable
	//可访问性:	private 
	//返回类型:	
	//修饰符:	
	//功能概要:	保护构造。
	//备注:		空实现。
	//********************************
	NonCopyable()
	{}
	//********************************
	//名称:		~NonCopyable
	//全名:		YSLib::Design::NonCopyable::~NonCopyable
	//可访问性:	protected 
	//返回类型:	
	//修饰符:	
	//功能概要:	保护析构。
	//备注:		空实现。
	//********************************
	~NonCopyable()
	{}

private: 
	//********************************
	//名称:		NonCopyable
	//全名:		YSLib::Design::NonCopyable::NonCopyable
	//可访问性:	private 
	//返回类型:	
	//修饰符:	
	//形式参数:	const NonCopyable &
	//功能概要:	禁止复制构造。
	//备注:		无实现。
	//********************************
	NonCopyable(const NonCopyable&);

	//********************************
	//名称:		operator=
	//全名:		YSLib::Design::NonCopyable::operator=
	//可访问性:	private 
	//返回类型:	NonCopyable&
	//修饰符:	
	//形式参数:	const NonCopyable &
	//功能概要:	禁止赋值复制。
	//备注:		无实现。
	//********************************
	NonCopyable& operator=(const NonCopyable&);
};

YSL_END_NAMESPACE(Design)

using Design::NonCopyable;


//********************************
//名称:		ReferenceEquals
//全名:		YSLib::ReferenceEquals<_type>
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	const _type & a
//形式参数:	const _type & b
//功能概要:	判断引用相等关系。
//备注:		
//********************************
template<typename _type>
inline bool
ReferenceEquals(const _type& a, const _type& b)
{
	return &a == &b;
}


//********************************
//名称:		GetZeroElement
//全名:		YSLib::GetZeroElement
//可访问性:	public 
//返回类型:	int
//修饰符:	
//功能概要:	取整数类型的零元素。
//备注:		
//********************************
inline int
GetZeroElement()
{
	return 0;
}
//********************************
//名称:		GetZeroElement
//全名:		YSLib::GetZeroElement<_type>
//可访问性:	public 
//返回类型:	_type
//修饰符:	
//功能概要:	取指定类型的零元素。
//备注:		
//********************************
template<typename _type>
inline _type
GetZeroElement()
{
	return _type();
}


//********************************
//名称:		vmin
//全名:		YSLib::vmin<_type>
//可访问性:	public 
//返回类型:	_type
//修饰符:	
//形式参数:	_type a
//形式参数:	_type b
//功能概要:	取值类型最小值。
//备注:		
//********************************
template<typename _type>
inline _type
vmin(_type a, _type b)
{
	return b < a ? b : a;
}
//********************************
//名称:		vmax
//全名:		YSLib::vmax<_type>
//可访问性:	public 
//返回类型:	_type
//修饰符:	
//形式参数:	_type a
//形式参数:	_type b
//功能概要:	取值类型最大值。
//备注:		
//********************************
template<typename _type>
inline _type
vmax(_type a, _type b)
{
	return a < b ? b : a;
}
//********************************
//名称:		vmin
//全名:		YSLib::vmin<_type, _fCompare>
//可访问性:	public 
//返回类型:	_type
//修饰符:	
//形式参数:	_type a
//形式参数:	_type b
//形式参数:	_fCompare _comp
//功能概要:	取值类型最小值。
//备注:		使用指定判断操作。
//********************************
template<typename _type, typename _fCompare>
inline _type
vmin(_type a, _type b, _fCompare _comp)
{
	return _comp(b, a) ? b : a;
}
//********************************
//名称:		vmax
//全名:		YSLib::vmax<_type, _fCompare>
//可访问性:	public 
//返回类型:	_type
//修饰符:	
//形式参数:	_type a
//形式参数:	_type b
//形式参数:	_fCompare _comp
//功能概要:	取值类型最大值。
//备注:		使用指定判断操作。
//********************************
template<typename _type, typename _fCompare>
inline _type
vmax(_type a, _type b, _fCompare _comp)
{
	return _comp(a, b) ? b : a;
}


//********************************
//名称:		sgn
//全名:		YSLib::sgn
//可访问性:	public 
//返回类型:	s8
//修饰符:	
//形式参数:	int a
//形式参数:	int b
//功能概要:	整数类型符号函数。
//备注:		当 a < b 时返回 -1 ，否则当 a = b 时返回 0 ，否则返回 1 。
//********************************
inline s8
sgn(int a, int b = 0)
{
	return a < b ? -1 : !(a == b);
}
//********************************
//名称:		sgn
//全名:		YSLib::sgn<_type>
//可访问性:	public 
//返回类型:	s8
//修饰符:	
//形式参数:	const _type & a
//形式参数:	const _type & b
//功能概要:	符号函数。
//备注:		当 a < b 时返回 -1 ，否则当 a = b 时返回 0 ，否则返回 1 。
//********************************
template<typename _type>
s8
sgn(const _type& a, const _type& b = GetZeroElement<_type>())
{
	return a < b ? -1 : !(a == b);
}

//********************************
//名称:		sgnInterval
//全名:		YSLib::sgnInterval
//可访问性:	public 
//返回类型:	int
//修饰符:	
//形式参数:	int d
//形式参数:	int a
//形式参数:	int b
//功能概要:	判断整数 d 和以 [a, b](a ≤ b) 或 [b, a](a > b) 区间的关系。
//备注:		无精度修正。
//			返回值意义——
//			< 0 ：d 在区间外；
//			= 0 ：d 在区间端点上；
//			> 0 ：d 在区间内。
//********************************
inline int
sgnInterval(int d, int a, int b)
{
	return sgn(a, d) * sgn(d, b);
}
//********************************
//名称:		sgnInterval
//全名:		YSLib::sgnInterval<_type>
//可访问性:	public 
//返回类型:	int
//修饰符:	
//形式参数:	const _type & d
//形式参数:	const _type & a
//形式参数:	const _type & b
//功能概要:	判断 d 和以 [a, b](a ≤ b) 或 [b, a](a > b) 区间的关系。
//备注:		无精度修正。
//			返回值意义——
//			< 0 ：d 在区间外；
//			= 0 ：d 在区间端点上；
//			> 0 ：d 在区间内。
//********************************
template<typename _type>
int
sgnInterval(const _type& d, const _type& a, const _type& b)
{
	return sgn(a, d) * sgn(d, b);
}

//********************************
//名称:		isInIntervalRegular
//全名:		YSLib<_type>::isInIntervalRegular
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	_type i
//形式参数:	_type b
//功能概要:	判断 i 是否在左闭右开区间 [GetZeroElement<_type>(), b) 中。
//前置条件:	断言：GetZeroElement<_type>() < b 。
//备注:		
//********************************
template<typename _type>
inline bool
isInIntervalRegular(_type i, _type b)
{
	YAssert(GetZeroElement<_type>() < b,
		"In function \"template<typename _type>\n"
		"inline bool\n"
		"isInIntervalRegular(_type i, _type b)\":\n"
		"Zero element as lower bound is not less than upper bound.");

	return !(i < GetZeroElement<_type>()) && i < b;
}
//********************************
//名称:		isInIntervalRegular
//全名:		YSLib<_type>::isInIntervalRegular
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	_type i
//形式参数:	_type a
//形式参数:	_type b
//功能概要:	判断 i 是否在左闭右开区间 [a, b) 中。
//前置条件:	断言：a < b 。
//备注:		
//********************************
template<typename _type>
inline bool
isInIntervalRegular(_type i, _type a, _type b)
{
	YAssert(a < b,
		"In function \"template<typename _type>\n"
		"inline bool\n"
		"isInIntervalRegular(_type i, _type a, _type b)\":\n"
		"Lower bound is not less than upper bound.");

	return !(i < a) && i < b;
}

//********************************
//名称:		isInIntervalStrict
//全名:		YSLib<_type>::isInIntervalStrict
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	_type i
//形式参数:	_type b
//功能概要:	判断 i 是否在开区间 (GetZeroElement<_type>(), b) 内。
//前置条件:	断言：GetZeroElement<_type>() < b 。
//备注:		
//********************************
template<typename _type>
inline bool
isInIntervalStrict(_type i, _type b)
{
	YAssert(GetZeroElement<_type>() < b,
		"In function \"template<typename _type>\n"
		"inline bool\n"
		"isInIntervalStrict(_type i, _type b)\":\n"
		"Zero element as lower bound is not less than upper bound.");

	return i > GetZeroElement<_type>() && i < b;
}
//********************************
//名称:		isInIntervalStrict
//全名:		YSLib<_type>::isInIntervalStrict
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	_type i
//形式参数:	_type a
//形式参数:	_type b
//功能概要:	判断 i 是否在开区间 (a, b) 内。
//前置条件:	断言：a < b 。
//备注:		
//********************************
template<typename _type>
inline bool
isInIntervalStrict(_type i, _type a, _type b)
{
	YAssert(a < b,
		"In function \"template<typename _type>\n"
		"inline bool\n"
		"isInIntervalStrict(_type i, _type a, _type b)\":\n"
		"Lower bound is not less than upper bound.");

	return i > a && i < b;
}

//********************************
//名称:		restrictInIntervalRegular
//全名:		YSLib<_type>::restrictInIntervalRegular
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	_type & i
//形式参数:	int a
//形式参数:	int b
//功能概要:	约束整数 i 在左闭右开区间 [a, b) 中。
//前置条件:	断言：a < b 。
//后置条件:	a <= i && i < b 。
//备注:		
//********************************
template<typename _type>
void
restrictInIntervalRegular(_type& i, int a, int b)
{
	YAssert(a < b,
		"In function \"template<typename _type>\n"
		"void\n"
		"restrictInIntervalRegular(_type& i, int a, int b)\":\n"
		"Lower bound is not less than upper bound.");

	if(i < a)
		i = a;
	else if(b < i)
		i = b - 1;
}

//********************************
//名称:		restrictUnsigned
//全名:		YSLib<_type>::restrictUnsigned
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	_type & u
//形式参数:	unsigned b
//功能概要:	约束无符号整数 u 在区间上界 b 内。
//后置条件:	u <= b。
//备注:		
//********************************
template<typename _type>
void
restrictUnsigned(_type& u, unsigned b)
{
	if(b < u)
		u = b;
}

//********************************
//名称:		restrictUnsignedRegular
//全名:		YSLib<_type>::restrictUnsignedRegular
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	_type & u
//形式参数:	unsigned b
//功能概要:	约束无符号整数 u 在左闭右开区间 [0, b) 中。
//前置条件:	断言：b != GetZeroElement<type>() 。
//后置条件:	GetZeroElement<_type>() <= u && u < b 。
//备注:		
//********************************
template<typename _type>
void
restrictUnsignedRegular(_type& u, unsigned b)
{
	YAssert(b != GetZeroElement<_type>(),
		"In function \"template<typename _type>\n"
		"void\n"
		"restrictUnsignedRegular(_type& u, unsigned b)\":\n"
		"Upper bound is zero.");

	if(!(u < b))
		u = b - 1;
}

//********************************
//名称:		restrictLessEqual
//全名:		YSLib<_type>::restrictLessEqual
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	_type & a
//形式参数:	_type & b
//功能概要:	约束关系： a ≤ b 。
//后置条件:	a <= b 。
//备注:		
//********************************
template<typename _type>
inline void
restrictLessEqual(_type& a, _type& b)
{
	if(b < a)
		std::swap(a, b);
}


//********************************
//名称:		ClearSequence
//全名:		YSLib<_type>::ClearSequence
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	_type * dst
//形式参数:	std::size_t n
//功能概要:	清除指定的连续区域。
//备注:		
//********************************
template<typename _type>
void
ClearSequence(_type* dst, std::size_t n)
{
	if(dst && n)
		memset(dst, 0, sizeof(_type) * n);
}


// delete 仿函数。
struct delete_obj
{
	//********************************
	//名称:		operator()
	//全名:		YSLib::delete_obj::operator()<_type>
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	_type * _ptr
	//功能概要:	删除指针指向的对象。
	//备注:		
	//********************************
	template<typename _type>
	inline void
	operator()(_type* _ptr)
	{
		delete _ptr;
	}
};


//带置空指针操作的 delete 仿函数。
struct safe_delete_obj
{
	//********************************
	//名称:		operator()
	//全名:		YSLib::safe_delete_obj::operator()<_type>
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	_type * & _ptr
	//功能概要:	删除指针指向的对象，并置指针为空值。
	//备注:		
	//********************************
	template<typename _type>
	inline void
	operator()(_type*& _ptr)
	{
		delete _ptr;
		_ptr = NULL;
	}
};


//引用仿函数。
template<typename _type>
struct deref_op : std::unary_function<_type, _type*>
{
	//********************************
	//名称:		operator()
	//全名:		YSLib::deref_op<_type>::operator()
	//可访问性:	public 
	//返回类型:	_type*
	//修饰符:	const
	//形式参数:	_type & _x
	//功能概要:	对指定对象使用 operator& 并返回结果。
	//备注:		
	//********************************
	inline _type*
	operator()(_type& _x) const
	{
		return &_x;
	}
};


//常量引用仿函数。
template<typename _type>
struct const_deref_op : std::unary_function<const _type, const _type*>
{
	//********************************
	//名称:		operator()
	//全名:		YSLib::const_deref_op<_type>::operator()
	//可访问性:	public 
	//返回类型:	const _type*
	//修饰符:	const
	//形式参数:	const _type & _x
	//功能概要:	对指定 const 对象使用 operator& 并返回结果。
	//备注:		
	//********************************
	inline const _type*
	operator()(const _type& _x) const
	{
		return &_x;
	}
};


//间接访问比较仿函数。
template<
	typename _type,
	template<typename _type> class _fCompare = std::less
>
struct deref_comp : _fCompare<_type>
{
	//********************************
	//名称:		operator()
	//全名:		YSLib::deref_comp<_type, _type, _fCompare>::operator()
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	_type * const & _x
	//形式参数:	_type * const & _y
	//功能概要:	返回 _fCompare<_type>::operator()(*_x, *_y) 。
	//备注:		如有空指针则不进行判断，直接返回 false 。
	//********************************
	bool
	operator()(_type* const& _x, _type* const& _y) const
	{
		return _x != NULL && _y != NULL && _fCompare<_type>::operator()(*_x, *_y);
	}
};


//间接访问字符串比较仿函数。
template<
	typename _tChar,
	int (*_lexi_cmp)(const _tChar*, const _tChar*) = std::strcmp,
	class _fCompare = std::less<int>
>
struct deref_str_comp : _fCompare
{
	//********************************
	//名称:		operator()
	//全名:		YSLib::deref_str_comp<_tChar, _tChar, _lexi_cmp, _fCompare>::operator()
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	const _tChar * _x
	//形式参数:	const _tChar * _y
	//功能概要:	返回 _fCompare::operator()(_lexi_cmp(_x, _y), 0) 。
	//备注:		如有空指针则不进行判断，直接返回 false 。
	//********************************
	bool
	operator()(const _tChar* _x, const _tChar* _y) const
	{
		return _x && _y && _fCompare::operator()(_lexi_cmp(_x, _y), 0);
	}
};


//********************************
//名称:		erase_all
//全名:		YSLib::erase_all<_tContainer>
//可访问性:	public 
//返回类型:	typename _tContainer::size_type
//修饰符:	
//形式参数:	_tContainer & _container
//形式参数:	const typename _tContainer::value_type & _value
//功能概要:	泛型算法：删除指定标准容器中所有相同元素。
//备注:		
//********************************
template<typename _tContainer>
typename _tContainer::size_type
erase_all(_tContainer& _container, const typename _tContainer::value_type& _value)
{
	int n(0);
	typename _tContainer::iterator i;

	while((i = std::find(_container.begin(), _container.end(), _value)) != _container.end())
	{
		_container.erase(i);
		++n;
	}
	return n;
}

//********************************
//名称:		erase_all_if
//全名:		YSLib<_tContainer, _fPredicate>::erase_all_if
//可访问性:	public 
//返回类型:	typename _tContainer::size_type
//修饰符:	
//形式参数:	_tContainer & _container
//形式参数:	const typename _tContainer::value_type & _pred
//功能概要:	泛型算法：删除指定标准容器中所有满足条件元素。
//备注:		
//********************************
template<typename _tContainer, typename _fPredicate>
typename _tContainer::size_type
erase_all_if(_tContainer& _container, const typename _tContainer::value_type& _pred)
{
	int n(0);
	typename _tContainer::iterator i;

	while((i = std::find(_container.begin(), _container.end(), _pred)) != _container.end())
		if(_pred(*i))
		{
			_container.erase(i);
			++n;
		}
	return n;
}

YSL_END

#endif

