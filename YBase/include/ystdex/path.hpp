/*
	© 2013-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file path.hpp
\ingroup YStandardEx
\brief 抽象路径模板。
\version r733
\author FrankHB <frankhb1989@gmail.com>
\since build 408
\par 创建时间:
	2013-05-27 02:42:19 +0800
\par 修改时间:
	2015-03-21 13:49 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Path
*/


#ifndef YB_INC_ystdex_path_hpp_
#define YB_INC_ystdex_path_hpp_ 1

#include "base.h" // for ystdex::cloneable;
#include "string.hpp" // for ystdex::sequence_container_adaptor,
//	ystdex::erase_all_if, ystdex::string_traits, ystdex::to_array;
#include "memory.hpp" // for std::unique_ptr, ystdex::make_unique;
#include <string> // for std::string;
#include <algorithm> // for std::adjencent_find;
#include <typeinfo> // for typeid;

namespace ystdex
{

/*!
\note 允许被按路径语义特化。
\since build 408
*/
//@{
//! \brief 路径范式。
template<typename _type>
class path_norm : public cloneable
{
public:
	using value_type = _type;

	//! \since build 586
	path_norm() = default;
	//! \since build 586
	path_norm(const path_norm&) = default;
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 450
	*/
	~path_norm() override;

	virtual bool
	is_compatible_with(const path_norm&) const ynothrow
	{
		return true;
	}

	virtual bool
	is_delimiter(const value_type&) = 0;

	virtual bool
	is_parent(const value_type&) ynothrow = 0;

	virtual bool
	is_root(const value_type&) ynothrow = 0;

	virtual bool
	is_self(const value_type&) ynothrow = 0;

//	virtual bool
//	is_wildcard(const value_type&) ynothrow = 0;

	//! \since build 475
	virtual path_norm*
	clone() const override = 0;
};

template<typename _type>
path_norm<_type>::~path_norm() = default;


//! \brief 文件路径范式。
template<typename _type>
class file_path_norm;

/*!
\brief 文件字符串路径范式。
\since build 409
*/
template<typename _tChar, class _tAlloc>
class file_path_norm<std::basic_string<_tChar, _tAlloc>>
	: public path_norm<std::string>
{
public:
	using value_type = std::basic_string<_tChar, _tAlloc>;

	bool
	is_delimiter(const value_type& str) override
	{
		return str.length() == 1 && str[0] == '/';
	}

	bool
	is_parent(const value_type& str) ynothrow override
	{
		return str.length() == 2 && str[0] == '.' && str[1] == '.';
	}

	bool
	is_root(const value_type& str) ynothrow override
	{
		return str.empty();
	}

	bool
	is_self(const value_type& str) ynothrow override
	{
		return str.length() == 1 && str[0] == '.';
	}

	//! \since build 475
	file_path_norm*
	clone() const override
	{
		return new file_path_norm(*this);
	}
};
//@}


/*!
\brief 路径类别。
\since build 543
*/
enum class path_category : yimpl(size_t)
{
	empty,
	self,
	parent,
	node
};

/*!
\brief 路径分类。
\relates path_category
\since build 543
*/
template<typename _tNorm, class _tString>
ystdex::path_category
classify_path(const _tString& name, _tNorm&& norm = _tNorm())
{
	if(YB_UNLIKELY(name.empty()))
		return ystdex::path_category::empty;
	if(norm.is_self(name))
		return ystdex::path_category::self;
	if(norm.is_parent(name))
		return ystdex::path_category::parent;
	return ystdex::path_category::node;
}


//! \since build 473
//@{
/*!
\brief 一般路径模板。
\tparam _tSeqCon 可倒置的序列容器类型。
\pre _tSeqCon 满足序列容器要求和 LessThanComparable 要求。
\pre _tSeqCon 成员异常安全性符合一般容器要求，具有 ISO C++11
	表 101 指定的可选操作 back 、 front 、 pop_back 和 push_back 。
\note 满足序列容器、可倒置容器要求，支持容器比较操作。
\note 使用 ISO C++11 容器要求指定的成员顺序声明。
\warning 非虚析构。
\see ISO C++11 17.6.3.1[utility.arg.requirements],
	23.2.1[container.requirements.general] 。
*/
template<class _tSeqCon,
	class _tNorm = ystdex::path_norm<typename _tSeqCon::value_type>>
class path : private sequence_container_adaptor<_tSeqCon>
{
private:
	using base = sequence_container_adaptor<_tSeqCon>;

public:
	using value_type = typename _tSeqCon::value_type;
	using norm = _tNorm;
	//! \since build 473
	using default_norm = ystdex::conditional_t<std::is_default_constructible<
		norm>::value, norm, file_path_norm<value_type>>;
	using reference = typename _tSeqCon::reference;
	using const_reference = typename _tSeqCon::const_reference;
	using size_type = typename base::size_type;
	using difference_type = typename base::difference_type;
	using iterator = typename base::iterator;
	using const_iterator = typename base::const_iterator;
	using reverse_iterator = typename base::container_type::reverse_iterator;
	using const_reverse_iterator
		= typename base::container_type::const_reverse_iterator;

	std::unique_ptr<norm> p_norm;

public:
	path(std::unique_ptr<norm> p = std::unique_ptr<norm>())
		: path(base(), std::move(p))
	{}
	//! \since build 433
	explicit
	path(base&& b, std::unique_ptr<norm> p = std::unique_ptr<norm>())
		: base(std::move(b)), p_norm(unique_norm(p))
	{}
	explicit
	path(size_type n, std::unique_ptr<norm> p = std::unique_ptr<norm>())
		: path(base(n), std::move(p))
	{}
	path(size_type n, value_type root,
		std::unique_ptr<norm> p = std::unique_ptr<norm>())
		: path(n != 0 ? n : 1, std::move(p))
	{
		this->begin() = std::move(root);
	}
	template<typename _tIn>
	path(_tIn first, _tIn last,
		std::unique_ptr<norm> p = std::unique_ptr<norm>())
		: path(base(first, last), std::move(p))
	{}
	//! \since build 433
	template<typename... _tParams>
	path(std::unique_ptr<norm> p, _tParams&&... args)
		: path(base(yforward(args)...), std::move(p))
	{}
	path(const path& pth)
		: base(pth), p_norm(pth.get_norm().clone())
	{}
	path(path&& pth) ynothrow
		: path()
	{
		pth.swap(*this);
	}
	path(std::initializer_list<value_type> il,
		std::unique_ptr<norm> p = std::unique_ptr<norm>())
		: path(base(il), std::move(p))
	{}

	path&
	operator=(const path& pth)
	{
		return *this = path(pth);
	}
	path&
	operator=(path&& pth) ynothrow
	{
		pth.swap(*this);
		return *this;
	}
	path&
	operator=(std::initializer_list<value_type> il)
	{
		return *this = path(il);
	}

	using base::begin;

	using base::end;

	using base::cbegin;

	using base::cend;

	using base::container_type::rbegin;

	using base::container_type::rend;

	using base::container_type::crbegin;

	using base::container_type::crend;

	using base::size;

	using base::max_size;

	using base::empty;

	using base::back;

//	using base::emplace;

//	using base::emplace_back;

	using base::front;

	using base::pop_back;

	using base::push_back;

	using base::insert;

	using base::erase;

	using base::clear;

	using base::assign;

	//! \since build 559
	using base::get_container;

	bool
	before(const path& pth) const
	{
		return typeid(get_norm()).before(typeid(pth.get_norm()))
			&& static_cast<const base&>(*this) < static_cast<const base&>(pth);
	}

	bool
	equals(const path& pth) const
	{
		return typeid(get_norm()) == typeid(pth.get_norm())
			&& static_cast<const base&>(*this) == static_cast<const base&>(pth);
	}

	void
	filter_self()
	{
		auto& nm(get_norm());

		ystdex::erase_all_if(*this, [&](const value_type& s){
			return nm.is_self(s);
		});
	}

	norm&
	get_norm() const ynothrow
	{
		yconstraint(p_norm);
		return *p_norm;
	}

	value_type
	get_root() const
	{
		return is_absolute() ? front() : value_type();
	}

	bool
	is_absolute() const ynothrow
	{
		return !empty() && get_norm().is_root(front());
	}

	bool
	is_relative() const ynothrow
	{
		return !is_absolute();
	}

	void
	merge_parents()
	{
		auto& nm(get_norm());

		for(auto i(this->begin()); i != this->end();)
		{
			auto j(std::adjacent_find(i, this->end(),
				[&](const value_type& x, const value_type& y){
					return !nm.is_self(x) && !nm.is_parent(x)
						&& nm.is_parent(y);
			}));

			if(j == this->end())
				break;
			i = j++;
			yassume(j != this->end());
			i = erase(i, ++j);
		}
	}

	void
	swap(path& pth) ynothrow
	{
		base::swap(static_cast<base&>(pth)),
		p_norm.swap(pth.p_norm);
	}

private:
	static std::unique_ptr<norm>
	unique_norm(std::unique_ptr<norm>& p)
	{
		return p ? std::move(p) : make_unique<default_norm>();
	}
};

/*!
\brief 路径比较操作。
\relates path
*/
//@{
template<class _tSeqCon, class _tNorm>
inline bool
operator==(const path<_tSeqCon, _tNorm>& x, const path<_tSeqCon, _tNorm>& y)
{
	return x.equals(y);
}

template<class _tSeqCon, class _tNorm>
bool
operator!=(const path<_tSeqCon, _tNorm>& x, const path<_tSeqCon, _tNorm>& y)
{
	return !(x == y);
}

template<class _tSeqCon, class _tNorm>
bool
operator<(const path<_tSeqCon, _tNorm>& x, const path<_tSeqCon, _tNorm>& y)
{
	return x.before(y);
}

template<class _tSeqCon, class _tNorm>
bool
operator<=(const path<_tSeqCon, _tNorm>& x, const path<_tSeqCon, _tNorm>& y)
{
	return !(y < x);
}

template<class _tSeqCon, class _tNorm>
bool
operator>(const path<_tSeqCon, _tNorm>& x, const path<_tSeqCon, _tNorm>& y)
{
	return y < x;
}

template<class _tSeqCon, class _tNorm>
bool
operator>=(const path<_tSeqCon, _tNorm>& x, const path<_tSeqCon, _tNorm>& y)
{
	return !(x < y);
}
//@}

/*!
\brief 正规化。
\relates path
*/
template<class _tSeqCon, class _tNorm>
inline void
normalize(path<_tSeqCon, _tNorm>& pth)
{
	pth.filter_self(), pth.merge_parents();
}

/*!
\brief 交换。
\relates path
*/
template<class _tSeqCon, class _tNorm>
inline void
swap(path<_tSeqCon, _tNorm>& x, path<_tSeqCon, _tNorm>& y)
{
	x.swap(y);
}

/*!
\brief 取字符串表示。
\relates path
*/
template<class _tSeqCon, class _tNorm>
typename _tSeqCon::value_type
to_string(const path<_tSeqCon, _tNorm>& pth,
	const typename _tSeqCon::value_type& seperator = &to_array<
	typename string_traits<typename _tSeqCon::value_type>::value_type>("/")[0])
{
	static_assert(is_object<typename _tSeqCon::value_type>::value,
		"Invalid type found.");

	if(pth.empty())
		return {};

	auto i(pth.begin());
	typename _tSeqCon::value_type res(*i);

	while(++i != pth.end())
	{
		res += seperator;
		res += *i;
	}
	return res;
}

/*!
\brief 取分隔符结尾的字符串表示。
\relates path
*/
template<class _tSeqCon, class _tNorm>
typename _tSeqCon::value_type
to_string_d(const path<_tSeqCon, _tNorm>& pth, typename string_traits<typename
	_tSeqCon::value_type>::value_type delimiter = '/')
{
	static_assert(is_object<typename _tSeqCon::value_type>::value,
		"Invalid type found.");
	typename _tSeqCon::value_type res;

	for(const auto& s : pth)
	{
		res += s;
		res += delimiter;
	}
	return res;
}
//@}

} // namespace ystdex;

#endif

