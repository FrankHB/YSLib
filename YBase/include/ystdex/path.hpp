/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file path.hpp
\ingroup YStandardEx
\brief 抽象路径模板。
\version r467
\author FrankHB <frankhb1989@gmail.com>
\since build 408
\par 创建时间:
	2013-05-27 02:42:19 +0800
\par 修改时间:
	2013-05-30 03:35 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Path
*/


#ifndef YB_INC_ystdex_path_hpp_
#define YB_INC_ystdex_path_hpp_ 1

#include "string.hpp" // for ystdex::sequence_container_adaptor,
//	ystdex::erase_all_if, ystdex::string_traits, ystdex::to_array;
#include "memory.hpp" // for std::unique_ptr, ystdex::make_unique;
#include <string> // for std::string;
#include <algorithm> // for std::adjencent_find;
#include <typeinfo> // for typeid;

namespace ystdex
{

//! \since build 408
//@{
//! \brief 路径范式。
template<typename _type>
class path_norm
{
public:
	typedef _type value_type;

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

	virtual path_norm*
	clone() = 0;
};


//! \brief 文件路径范式。
template<typename _type>
class file_path_norm;

//! \brief 文件字符串路径范式。
template<>
class file_path_norm<std::string> : public path_norm<std::string>
{
public:
	typedef std::string value_type;

	bool
	is_delimiter(const value_type& s) override
	{
		return s == "/";
	}

	bool
	is_parent(const value_type& s) ynothrow override
	{
		return s == "..";
	}

	bool
	is_root(const value_type& s) ynothrow override
	{
		return s.empty();
	}

	bool
	is_self(const value_type& s) ynothrow override
	{
		return s == ".";
	}

	file_path_norm*
	clone() override
	{
		return new file_path_norm(*this);
	}
};


/*!
\brief 一般路径模板。
\tparam _tSeqContainer 可倒置的序列容器类型。
\pre _tSeqContainer 满足序列容器要求和 LessThanComparable 要求。
\pre _tSeqContainer 成员异常安全性符合一般容器要求，具有 ISO C++11
	表 101 指定的可选操作 back 、 front 、 pop_back 和 push_back 。
\note 满足序列容器、可倒置容器要求，支持容器比较操作。
\note 使用 ISO C++11 容器要求指定的成员顺序声明。
\warning 非虚析构。
\see ISO C++11 17.6.3.1[utility.arg.requirements],
	23.2.1[container.requirements.general] 。
*/
template<class _tSeqCont>
class path : private sequence_container_adaptor<_tSeqCont>
{
private:
	typedef sequence_container_adaptor<_tSeqCont> base;

public:
	typedef typename _tSeqCont::value_type value_type;
	typedef path_norm<value_type> norm;
	typedef typename _tSeqCont::reference reference;
	typedef typename _tSeqCont::const_reference const_reference;
	typedef typename base::size_type size_type;
	typedef typename base::difference_type difference_type;
	typedef typename base::iterator iterator;
	typedef typename base::const_iterator const_iterator;
	typedef typename base::container_type::reverse_iterator reverse_iterator;
	typedef typename base::container_type::const_reverse_iterator
		const_reverse_iterator;

	std::unique_ptr<norm> p_norm;

public:
	path(std::unique_ptr<norm> p = std::unique_ptr<norm>())
		: base(), p_norm(unique_norm(p))
	{}
	explicit
	path(size_type n, std::unique_ptr<norm> p = std::unique_ptr<norm>())
		: base(n), p_norm(unique_norm(p))
	{}
	path(size_type n, value_type root,
		std::unique_ptr<norm> p = std::unique_ptr<norm>())
		: base(n != 0 ? n : 1), p_norm(unique_norm(p))
	{
		this->begin() = std::move(root);
	}
	template<class _tIn>
	path(_tIn first, _tIn last,
		std::unique_ptr<norm> p = std::unique_ptr<norm>())
		: base(first, last), p_norm(unique_norm(p))
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
		: base(il), p_norm(unique_norm(p))
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

	bool
	before(const path& pth) const
	{
		yconstraint(p_norm),
		yconstraint(pth.p_norm);

		return typeid(*p_norm).before(typeid(*pth.p_norm))
			&& static_cast<const base&>(*this) < static_cast<const base&>(pth);
	}

	bool
	equals(const path& pth) const
	{
		yconstraint(p_norm),
		yconstraint(pth.p_norm);

		return typeid(*p_norm) == typeid(*pth.p_norm)
			&& static_cast<const base&>(*this) == static_cast<const base&>(pth);
	}

	void
	filter_self()
	{
		yconstraint(p_norm);

		ystdex::erase_all_if(static_cast<base&>(*this), this->begin(),
			this->end(), [this](const value_type& s){
				p_norm->is_self();
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
		auto i(this->begin());

		while(auto j = std::adjacent_find(i, this->end(),
			[this](const value_type&, const value_type& s){
			return p_norm->is_parent();
		}))
		{
			i = j++;

			yassume(j != this->end());

			++j;
			i = erase(i, j);
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
		return p ? std::move(p) : make_unique<file_path_norm<value_type>>();
	}
};

//! \brief 路径比较操作。
//@{
template<class _tSeqCont>
inline bool
operator==(const path<_tSeqCont>& x, const path<_tSeqCont>& y)
{
	return x.equals(y);
}

template<class _tSeqCont>
bool
operator!=(const path<_tSeqCont>& x, const path<_tSeqCont>& y)
{
	return !(x == y);
}

template<class _tSeqCont>
bool
operator<(const path<_tSeqCont>& x, const path<_tSeqCont>& y)
{
	return x.before(y);
}

template<class _tSeqCont>
bool
operator<=(const path<_tSeqCont>& x, const path<_tSeqCont>& y)
{
	return !(y < x);
}

template<class _tSeqCont>
bool
operator>(const path<_tSeqCont>& x, const path<_tSeqCont>& y)
{
	return y < x;
}

template<class _tSeqCont>
bool
operator>=(const path<_tSeqCont>& x, const path<_tSeqCont>& y)
{
	return !(x < y);
}
//@}

/*!
\ingroup helper_functions
\brief 迭代器包装，用于 range-based for 。
*/
//@{
template<class _tSeqCont>
inline auto
begin(path<_tSeqCont>& pth) -> decltype(pth.begin())
{
	return pth.begin();
}
template<class _tSeqCont>
inline auto
begin(const path<_tSeqCont>& pth) -> decltype(pth.begin())
{
	return pth.begin();
}

template<class _tSeqCont>
inline auto
end(path<_tSeqCont>& pth) -> decltype(pth.end())
{
	return pth.end();
}
template<class _tSeqCont>
inline auto
end(const path<_tSeqCont>& pth) -> decltype(pth.end())
{
	return pth.end();
}
//@}

//! \brief 正规化。
template<class _tSeqCont>
inline void
normalize(path<_tSeqCont>& pth)
{
	pth.filter_self(), pth.merge_parents();
}

//! \brief 交换。
template<class _tSeqCont>
inline void
swap(path<_tSeqCont>& x, path<_tSeqCont>& y)
{
	x.swap(y);
}

//! \brief 取字符串表示。
template<class _tSeqCont>
typename _tSeqCont::value_type
to_string(const path<_tSeqCont>& pth, const typename _tSeqCont::value_type&
	seperator = &to_array<
	typename string_traits<typename _tSeqCont::value_type>::value_type>("/")[0])
{
	if(pth.empty())
		return {};

	auto i(pth.begin());
	typename _tSeqCont::value_type res(*i);

	while(++i != pth.end())
	{
		res += seperator;
		res += *i;
	}
	return std::move(res);
}

//! \brief 取分隔符结尾的字符串表示。
template<class _tSeqCont>
typename _tSeqCont::value_type
to_string_d(const path<_tSeqCont>& pth, typename string_traits<typename
	_tSeqCont::value_type>::value_type delimiter = '/')
{
	typename _tSeqCont::value_type res;

	for(const auto& s : pth)
	{
		res += s;
		res += delimiter;
	}
	return std::move(res);
}
//@}

} // namespace ystdex;

#endif

