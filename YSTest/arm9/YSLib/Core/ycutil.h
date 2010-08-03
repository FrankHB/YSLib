// YSLib::Core::YCoreUtilities by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-5-23 6:10:59;
// UTime = 2010-8-2 13:57;
// Version = 0.1958;


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
	NonCopyable()
	{}
	~NonCopyable()
	{}

private: 
	NonCopyable(const NonCopyable&);
	NonCopyable& operator=(const NonCopyable&);
};

YSL_END_NAMESPACE(Design)

using Design::NonCopyable;


//判断引用相等关系。
template<typename _type>
inline bool
ReferenceEquals(const _type& a, const _type& b)
{
	return &a == &b;
}


//取指定类型的零元素。
inline int
GetZeroElement()
{
	return 0;
}
template<typename _type>
inline _type
GetZeroElement()
{
	return _type();
}


//取值类型最值。
template<typename _type>
inline _type
vmin(_type a, _type b)
{
	return b < a ? b : a;
}
template<typename _type>
inline _type
vmax(_type a, _type b)
{
	return a < b ? b : a;
}
template<typename _type, typename _cmpType>
inline _type
vmin(_type a, _type b, _cmpType _comp)
{
	return _comp(b < a) ? b : a;
}
template<typename _type, typename _cmpType>
inline _type
vmax(_type a, _type b, _cmpType _comp)
{
	return _comp(a < b) ? b : a;
}


//符号函数，当 a < b 时返回 -1 ，否则当 a = b 时返回 0 ，否则返回 1 。
inline s8
sgn(int a, int b = 0)
{
	return a < b ? -1 : !(a == b);
}
template<typename _type>
s8
sgn(const _type& a, const _type& b = GetZeroElement<_type>())
{
	return a < b ? -1 : !(a == b);
}

//判断 d 和以 [a, b](a ≤ b) 或 [b, a](a > b) 区间的关系，无精度修正。
//返回值意义—— < 0 ：d 在区间外； = 0 ：d 在区间端点上；> 0 ：d 在区间内。
inline int
sgnInterval(int d, int a, int b)
{
	return sgn(a, d) * sgn(d, b);
}
template<typename _type>
int
sgnInterval(const _type& d, const _type& a, const _type& b)
{
	return sgn(a, d) * sgn(d, b);
}

//判断是否在左闭右开区间中。
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

//判断是否在开区间内。
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

//约束整数在左闭右开区间中。
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

//约束无符号整数在区间上界内。
template<typename _type>
void
restrictUnsigned(_type& u, unsigned b)
{
	if(b < u)
		u = b;
}
template<typename _type>
void
restrictUnsignedRegular(_type& u, unsigned b)
{
	YAssert(b,
		"In function \"template<typename _type>\n"
		"void\n"
		"restrictUnsignedRegular(_type& u, unsigned b)\":\n"
		"Upper bound is zero.");

	if(!(u < b))
		u = b - 1;
}

//约束关系： a ≤ b 。
template<typename _type>
inline void
restrictLessEqual(_type& a, _type& b)
{
	if(b < a)
		std::swap(a, b);
}


//清除指定的连续区域。
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
	template<typename _type>
	inline void
	operator()(_type*& _ptr)
	{
		delete _ptr;
		_ptr = NULL;
	}
};


//解引用仿函数。
template<typename _type>
struct deref_op : std::unary_function<_type, _type*>
{
	inline _type*
	operator()(_type& _x) const
	{
		return &_x;
	}
};


//常量解引用仿函数。
template<typename _type>
struct const_deref_op : std::unary_function<const _type, const _type*>
{
	inline const _type*
	operator()(const _type& _x) const
	{
		return &_x;
	}
};


//间接访问（解引用）比较仿函数。
template<
	typename _type,
	template<typename _type> class _cmpType = std::less
>
struct deref_comp : _cmpType<_type>
{
	bool
	operator()(_type* const& _x, _type* const& _y) const
	{
		return _x && _y && _cmpType<_type>::operator()(*_x, *_y);
	}
};


//间接访问字符串（解引用）比较仿函数。
template<
	typename _charType,
	int (*_lexi_cmp)(const _charType*, const _charType*) = std::strcmp,
	class _cmpType = std::less<int>
>
struct deref_str_comp : _cmpType
{
	bool
	operator()(const _charType* _x, const _charType* _y) const
	{
		return _x && _y && _cmpType::operator()(_lexi_cmp(_x, _y), 0);
	}
};


//删除指定标准容器中所有相同元素算法。
template<typename _containerType>
typename _containerType::size_type
erase_all(_containerType& _container, const typename _containerType::value_type& _value)
{
	int n(0);
	typename _containerType::iterator i;

	while((i = std::find(_container.begin(), _container.end(), _value)) != _container.end())
	{
		_container.erase(i);
		++n;
	}
	return n;
}

//删除指定标准容器中所有满足条件元素算法。
template<typename _containerType, typename _predicateType>
typename _containerType::size_type
erase_all_if(_containerType& _container, const typename _containerType::value_type& _pred)
{
	int n(0);
	typename _containerType::iterator i;

	while((i = std::find(_container.begin(), _container.end(), _pred)) != _container.end())
	{
		_container.erase(i);
		++n;
	}
	return n;
}

YSL_END

#endif

