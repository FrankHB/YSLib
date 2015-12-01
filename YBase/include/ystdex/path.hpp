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
\version r1057
\author FrankHB <frankhb1989@gmail.com>
\since build 408
\par 创建时间:
	2013-05-27 02:42:19 +0800
\par 修改时间:
	2015-11-29 17:04 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Path
*/


#ifndef YB_INC_ystdex_path_hpp_
#define YB_INC_ystdex_path_hpp_ 1

#include "string_view.hpp" // for std::basic_string, string_view,
//	sequence_container_adaptor, totally_ordered, dividable,
//	ystdex::erase_all_if, to_array, string_traits;

namespace ystdex
{

//! \since build 647
//@{
//! \note 允许被按路径语义特化。
//@{
//! \brief 路径特征。
template<typename _type>
class path_traits
{
	using value_type = _type;
};

//! \brief 文件字符串路径特征。
template<typename _tChar, class _tAlloc>
class path_traits<std::basic_string<_tChar, _tAlloc>>
{
public:
	using char_type = _tChar;
	using value_type = std::basic_string<_tChar, _tAlloc>;
	using view_type = basic_string_view<char_type>;

	static yconstfn bool
	is_parent(view_type sv) ynothrow
	{
		return sv.length() == 2 && sv[0] == '.' && sv[1] == '.';
	}

	static yconstfn bool
	is_root(view_type sv) ynothrow
	{
		return sv.empty();
	}

	static yconstfn bool
	is_self(const value_type& str) ynothrow
	{
		return str.length() == 1 && str[0] == '.';
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
*/
template<class _tString, class _tTraits = path_traits<_tString>>
path_category
classify_path(const _tString& name) ynothrow
{
	if(YB_UNLIKELY(name.empty()))
		return path_category::empty;
	if(_tTraits::is_self(name))
		return path_category::self;
	if(_tTraits::is_parent(name))
		return path_category::parent;
	return path_category::node;
}


/*!
\brief 一般路径模板。
\tparam _tSeqCon 可倒置的序列容器类型。
\pre _tSeqCon 满足序列容器要求和 LessThanComparable 要求。
\pre _tSeqCon 成员异常安全性符合一般容器要求，具有 ISO C++11
	表 101 指定的可选操作 back 、 front 、 pop_back 和 push_back 。
\note 满足序列容器、可倒置容器要求，支持容器比较操作。
\note 使用 ISO C++11 容器要求指定的成员顺序声明。
\warning 非虚析构。
\see ISO C++11 17.6.3.1 [utility.arg.requirements],
	23.2.1 [container.requirements.general] 。
*/
template<class _tSeqCon,
	class _tTraits = path_traits<typename _tSeqCon::value_type>>
class path : private sequence_container_adaptor<_tSeqCon>,
	public dividable<path<_tSeqCon, _tTraits>, typename _tSeqCon::value_type>,
	public totally_ordered<path<_tSeqCon, _tTraits>>,
	dividable<path<_tSeqCon, _tTraits>>
{
private:
	//! \since build 473
	//@{
	using base = sequence_container_adaptor<_tSeqCon>;

public:
	using value_type = typename _tSeqCon::value_type;
	//! \since build 647
	using traits_type = _tTraits;
	using reference = typename _tSeqCon::reference;
	using const_reference = typename _tSeqCon::const_reference;
	using size_type = typename base::size_type;
	using difference_type = typename base::difference_type;
	using iterator = typename base::iterator;
	using const_iterator = typename base::const_iterator;
	using reverse_iterator = typename base::container_type::reverse_iterator;
	using const_reverse_iterator
		= typename base::container_type::const_reverse_iterator;

	//! \since build 647
	path() = default;
	//! \since build 647
	using base::base;
	path(const path&) = default;
	path(path&&) = default;

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

	//! \since build 600
	path&
	operator/=(const value_type& s)
	{
		if(!empty() && traits_type::is_parent(s))
		{
			if(!traits_type::is_root(front()) || 1U < size())
			{
				if(!(traits_type::is_parent(back())
					|| traits_type::is_self(back())))
					pop_back();
				else
					push_back(s);
			}
		}
		else if(!traits_type::is_self(s))
			push_back(s);
		return *this;
	}
	//! \since build 600
	path&
	operator/=(const path& pth)
	{
		if(pth.is_relative())
			for(const auto& s : pth)
				*this /= s;
		return *this;
	}

	//! \since build 600
	//@{
	friend bool
	operator==(const path& x, const path& y)
	{
		return static_cast<const base&>(x) == static_cast<const base&>(y);
	}

	friend bool
	operator<(const path& x, const path& y)
	{
		return static_cast<const base&>(x) < static_cast<const base&>(y);
	}
	//@}

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

	void
	filter_self()
	{
		ystdex::erase_all_if(*this, [&](const value_type& s) ynothrow{
			return traits_type::is_self(s);
		});
	}

	value_type
	get_root() const
	{
		return is_absolute() ? front() : value_type();
	}

	bool
	is_absolute() const ynothrow
	{
		return !empty() && traits_type::is_root(front());
	}

	bool
	is_relative() const ynothrow
	{
		return !is_absolute();
	}

	void
	merge_parents()
	{
		for(auto i(this->begin()); i != this->end();)
		{
			auto j(std::adjacent_find(i, this->end(),
				[&](const value_type& x, const value_type& y){
					return !traits_type::is_self(x) && !traits_type::is_parent(x)
						&& traits_type::is_parent(y);
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
		base::swap(static_cast<base&>(pth));
	}
	//@}
};
//@}

/*!
\relates path
\since build 473
*/
//@{
//! \brief 正规化。
template<class _tSeqCon, class _tTraits>
inline void
normalize(path<_tSeqCon, _tTraits>& pth)
{
	pth.filter_self(), pth.merge_parents();
}

//! \brief 交换。
template<class _tSeqCon, class _tTraits>
inline void
swap(path<_tSeqCon, _tTraits>& x, path<_tSeqCon, _tTraits>& y)
{
	x.swap(y);
}

//! \brief 取字符串表示。
template<class _tSeqCon, class _tTraits>
typename _tSeqCon::value_type
to_string(const path<_tSeqCon, _tTraits>& pth,
	const typename _tSeqCon::value_type& seperator = &to_array<
	typename string_traits<typename _tSeqCon::value_type>::value_type>("/")[0])
{
	static_assert(is_object<typename _tSeqCon::value_type>(),
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

//! \brief 取分隔符结尾的字符串表示。
template<class _tSeqCon, class _tTraits>
typename _tSeqCon::value_type
to_string_d(const path<_tSeqCon, _tTraits>& pth, typename string_traits<typename
	_tSeqCon::value_type>::value_type delimiter = '/')
{
	static_assert(is_object<typename _tSeqCon::value_type>(),
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

