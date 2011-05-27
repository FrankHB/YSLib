/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file util.hpp
\ingroup YCLib
\brief 函数对象、算法和实用程序。
\version 0.1659;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-05-23 06:10:59 +0800; 
\par 修改时间:
	2011-05-23 20:19 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YCLib::YStandardExtend::Utilities;
*/


#ifndef YCL_INC_YSTDEX_UTILITY_HPP_
#define YCL_INC_YSTDEX_UTILITY_HPP_

#include "../ystdex.h"
#include <cstring>
#include <utility>
#include <functional>
#include <algorithm>

namespace ystdex
{
	/*
	\brief 不可复制对象：禁止继承此类的对象调用复制构造函数和复制赋值操作符。
	*/
	class noncopyable
	{
	//保护非多态类。
	protected:
		/*!
		\brief 保护构造：空实现。
		*/
		noncopyable()
		{}
		/*!
		\brief 保护析构：空实现。
		*/
		~noncopyable()
		{}

	public: 
		/*!
		\brief 禁止复制构造。
		*/
		noncopyable(const noncopyable&) = delete;

		/*!
		\brief 禁止赋值复制。
		*/
		noncopyable& operator=(const noncopyable&) = delete;
	};

#ifdef YCL_HAS_BUILTIN_NULLPTR

	using std::nullptr_t;

#else

	/*!
	\brief 空指针类。
	\note 代码参考：
	http://topic.csdn.net/u/20100924/17/ \
		BE0C26F8-5127-46CD-9136-C9A96AAFDA76.html 。
	*/
	const class nullptr_t
	{
	public:
		/*
		\brief 转换任意类型至空非成员或静态成员指针。
		*/
		template<typename T>
		inline operator T*() const
		{
			return 0;
		}

		/*
		\brief 转换任意类型至空非静态成员指针。
		*/
		template<typename C, typename T>
		inline operator T C::*() const
		{
			return 0;
		}
		/*
		\brief 支持关系运算符重载。
		*/
		template<typename T> bool
		equals(T const& rhs) const
		{
			return rhs == 0;
		}

		/*
		\brief 禁止取 nullptr 的指针。
		*/
		void operator&() const = delete;
	} nullptr = {};

	template<typename T>
	inline bool
	operator==(const nullptr_t& lhs, T const& rhs)
	{
		return lhs.equals(rhs);
	}
	template<typename T>
	inline bool
	operator==(T const& lhs, const nullptr_t& rhs)
	{
		return rhs.equals(lhs);
	}

	template<typename T>
	inline bool
	operator!=(const nullptr_t& lhs, T const& rhs)
	{
		return !lhs.equals(rhs);
	}
	template<typename T>
	inline bool
	operator!=(T const& lhs, const nullptr_t& rhs)
	{
		return !rhs.equals(lhs);
	}

#endif


	//根据参数类型使用 std::forward 传递对应参数，保持左值性和常量性。
	#define yforward(_expr) std::forward<typename \
		std::remove_reference<decltype(_expr)>::type>(_expr)


	/*
	\brief 任意非可复制构造的非聚集类型。
	*/
	union no_copy_t
	{
		void* object_ptr;
		const void* const_object_ptr;
		void(*function_ptr)();
		void(no_copy_t::*member_function_pointer)();
	};


	/*
	\brief 任意 POD 类型。
	\note POD 含义参考 ISO C++ 2011 。
	*/
	union any_pod_t
	{
		no_copy_t _unused;
		unsigned char plain_old_data[sizeof(no_copy_t)];

		void*
		access()
		{
			return &plain_old_data[0];
		}
		const void*
		access() const
		{
			return &plain_old_data[0];
		}
		template<typename _type>
		_type&
		access()
		{
			return *static_cast<_type*>(access());
		}
		template<typename _type>
		const _type&
		access() const
		{
			return *static_cast<const _type*>(access());
		}
	};


	/*!	\defgroup Functors General Functors
	\brief 算法。
	*/

	/*!
	\ingroup Functors
	\brief 引用相等关系仿函数。
	*/
	template<typename _type>
	struct ref_eq : public std::binary_function<_type, _type, bool>
	{
		inline bool
		operator()(const _type& _x, const _type& _y) const
		{
			return &_x == &_y;
		}
	};

	/*!
	\ingroup Functors
	\brief 编译期选择自增/自减运算仿函数。
	*/
	template<bool, typename _tScalar>
	struct xcrease_t
	{
		inline _tScalar&
		operator()(_tScalar& _x)
		{
			return ++_x;
		}
	};
	template<typename _tScalar>
	struct xcrease_t<false, _tScalar>
	{
		inline _tScalar&
		operator()(_tScalar& _x)
		{
			return --_x;
		}
	};

	/*!
	\ingroup Functors
	\brief 编译期选择加法/减法复合赋值运算仿函数。
	*/
	//@{
	template<bool, typename _tScalar1, typename _tScalar2>
	struct delta_assignment_t
	{
		inline _tScalar1&
		operator()(_tScalar1& x, _tScalar2 y)
		{
			return x += y;
		}
	};
	template<typename _tScalar1, typename _tScalar2>
	struct delta_assignment_t<false, _tScalar1, _tScalar2>
	{
		inline _tScalar1&
		operator()(_tScalar1& x, _tScalar2 y)
		{
			return x -= y;
		}
	};
	//@}

	/*!
	\ingroup HelperFunction
	\brief 编译期选择自增/自减运算。
	*/
	template<bool _bIsPositive, typename _tScalar>
	_tScalar&
	xcrease(_tScalar& _x)
	{
		return xcrease_t<_bIsPositive, _tScalar>()(_x);
	}

	/*!
	\ingroup HelperFunction
	\brief 编译期选择加法/减法复合赋值运算。
	*/
	template<bool _bIsPositive, typename _tScalar1, typename _tScalar2>
	_tScalar1&
	delta_assignment(_tScalar1& _x, _tScalar2& _y)
	{
		return delta_assignment_t<_bIsPositive, _tScalar1, _tScalar2>()(_x, _y);
	}


	/*!
	\ingroup Functors
	\brief 引用仿函数。
	*/
	template<typename _type>
	struct deref_op : std::unary_function<_type, _type*>
	{
		/*!
		\brief 对指定对象使用 operator& 并返回结果。
		*/
		inline _type*
		operator()(_type& _x) const
		{
			return &_x;
		}
	};


	/*!
	\ingroup Functors
	\brief 常量引用仿函数。
	*/
	template<typename _type>
	struct const_deref_op : std::unary_function<const _type, const _type*>
	{
		/*!
		\brief 对指定 const 对象使用 operator& 并返回结果。
		*/
		inline const _type*
		operator()(const _type& _x) const
		{
			return &_x;
		}
	};


	/*!
	\ingroup Functors
	\brief 间接访问比较仿函数。
	*/
	template<
		typename _type, typename _tPointer = _type*,
		template<typename _type> class _gfCompare = std::less
	>
	struct deref_comp : _gfCompare<_type>
	{
		/*!
		\brief 返回 _gfCompare<_type>::operator()(*_x, *_y) 。
		\note 如有空指针则不进行判断，直接返回 false 。
		*/
		bool
		operator()(const _tPointer& _x, const _tPointer& _y) const
		{
			return _x && _y && _gfCompare<_type>::operator()(*_x, *_y);
		}
	};


	/*!
	\ingroup Functors
	\brief 间接访问字符串比较仿函数。
	*/
	template<
		typename _tChar,
		int (*_lexi_cmp)(const _tChar*, const _tChar*) = std::strcmp,
		class _gfCompare = std::less<int>
	>
	struct deref_str_comp : _gfCompare
	{
		/*!
		\brief 返回 _gfCompare::operator()(_lexi_cmp(_x, _y), 0) 。
		\note 如有空指针则不进行判断，直接返回 false 。
		*/
		bool
		operator()(const _tChar* _x, const _tChar* _y) const
		{
			return _x && _y && _gfCompare::operator()(_lexi_cmp(_x, _y), 0);
		}
	};


	/*!	\defgroup Algorithms Gerneral Algorithms
	\brief 算法。
	*/

	/*!
	\ingroup Algorithms
	\brief 取值类型最小值。
	*/
	template<typename _type>
	inline _type
	vmin(_type a, _type b)
	{
		return b < a ? b : a;
	}
	/*!
	\ingroup Algorithms
	\brief 取值类型最小值。
	\note 使用指定判断操作。
	*/
	template<typename _type, typename _fCompare>
	inline _type
	vmin(_type a, _type b, _fCompare _comp)
	{
		return _comp(b, a) ? b : a;
	}

	/*!
	\ingroup Algorithms
	\brief 取值类型最大值。
	*/
	template<typename _type>
	inline _type
	vmax(_type a, _type b)
	{
		return a < b ? b : a;
	}
	/*!
	\ingroup Algorithms
	\brief 取值类型最大值。
	\note 使用指定判断操作。
	*/
	template<typename _type, typename _fCompare>
	inline _type
	vmax(_type a, _type b, _fCompare _comp)
	{
		return _comp(a, b) ? b : a;
	}


	/*!
	\ingroup Algorithms
	\brief 删除指定标准容器中所有相同元素。
	*/
	template<typename _tContainer>
	typename _tContainer::size_type
	erase_all(_tContainer& _container,
		const typename _tContainer::value_type& _value)
	{
		const auto s(_container.size());

		_container.erase(std::remove(_container.begin(), _container.end(),
			_value), _container.end());

		return s - _container.size();
	}

	/*!
	\ingroup Algorithms
	\brief 删除指定标准容器中所有满足条件元素。
	*/
	template<typename _tContainer, typename _fPredicate>
	typename _tContainer::size_type
	erase_all_if(_tContainer& _container, _fPredicate _pred)
	{
		const auto s(_container.size());

		_container.erase(std::remove_if(_container.begin(), _container.end(),
			_pred), _container.end());

		return s - _container.size();
	}

	/*!
	\ingroup Algorithms
	\brief 按指定键值搜索指定映射。
	\return 一个用于表示结果的 std::pair 对象，其 first 成员为迭代器，
		second 成员表示是否需要插入。
	\note 行为同 std::map::operator[] 。
	*/
	template<class _tMap>
	std::pair<typename _tMap::iterator, bool>
	search_map(_tMap& m, const typename _tMap::key_type& k)
	{
		auto i(m.lower_bound(k));

		return std::make_pair(i, (i == m.end() || m.key_comp()(k, i->first)));
	}
}

#ifndef YCL_HAS_BUILTIN_NULLPTR
using ystdex::nullptr;
#endif

#endif

