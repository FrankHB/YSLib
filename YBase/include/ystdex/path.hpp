/*
	© 2013-2016, 2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file path.hpp
\ingroup YStandardEx
\brief 抽象路径模板。
\version r1363
\author FrankHB <frankhb1989@gmail.com>
\since build 408
\par 创建时间:
	2013-05-27 02:42:19 +0800
\par 修改时间:
	2018-08-29 19:33 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Path

这个模块提供通用的、抽象的路径的表示和若干相关操作。
和 std::filesystem::path 专用于表示文件系统路径的目的（参见 WG21 N1975 ）不同，
	设计有许多差异；因此并非 ISO C++17 [fs.class.path] 的直接替代。
但作为路径的表示，一些设计仍然有类似之处，且一些典型用例可被基于本模块的 API 涵盖。
本设计的路径具有以下抽象构造：
路径是元素的容器，其中的元素是能对资源起标识作用的值。
一般地，路径分为顺序的两部分：至多由一个元素表示的根路径(root path) ，
	和之后的不限个数的非空元素表示的限定的无根路径(rootless path) 。
其中，根路径和无根路径都是可选的。
路径和称为路径名(pathname) 的作为外部表示的字符串关联（可能非一一对应）。
路径可能有其它外部表示，但其交互不在本设计中指定。
路径名中使用分隔符(separater) 隔离元素。
典型地，一个分隔字符(delimiter) 可作为分隔符。
基于路径性质的不同，以路径特征(path trait) 参数化，调整不同的路径操作。
路径和外部表示的互操作应通过对元素的操作进行。
部分操作，包括可能的和外部路径之间存在的一些转换，可由按需特化的路径特征指定。
不存在根路径的路径或具有特定被特征另行指定的根的路径是相对路径；其它路径是绝对路径。
判断是否为根路径及是否允许在根路径保留路径名中的任意连续的分隔符由特征间接限定。
若需区分根路径中的组成部分，实现可进一步对根路径留空或置其它特定值，
	其具体组成部分另行约定和表示，由特征间接限定。
抽象的路径和具体文件系统无关，不限 ISO C++17 [filesystems] 指定的功能，不被具体
	文件系统的假设约束，可表示一般的文件系统、网络和其它资源的具有层次的限定名，
	如 UNC 、URL 及 C++ 限定名。
*/


#ifndef YB_INC_ystdex_path_hpp_
#define YB_INC_ystdex_path_hpp_ 1

#include "string.hpp" // for basic_string, string_view,
//	sequence_container_adaptor, dividable, totally_ordered,
//	ystdex::erase_all_if, to_array, string_traits;

namespace ystdex
{

//! \since build 647
//@{
/*!
\note 允许被按路径语义特化。
\note 要求用户特化提供 \c void 特化相同的签名的成员（包括一致的异常规范）。
*/
//@{
/*!
\brief 路径特征。
\sa path

和 WG21 N1975 的 Boost.Filesystem V2 （及之后的 Dinkumware 的实现）不同，
	这里的特征是单独设计的，且默认（非用户特化）不涉及外部表示的具体形式。
这由表示路径的类型的设计的整体设计决定。
判断根名称的参数预期为路径的第一个元素且至少作为第一个参数是已知为根名称，
	否则结果是否表示根名称未指定。
关于路径和 Boost 及 std::filesystem::path 的其它详细设计差异详见 path 模板的说明。
*/
template<typename _type>
struct path_traits;

//! \since build 836
template<>
struct path_traits<void>
{
	template<typename _tString>
	static yconstfn bool
	is_parent(const _tString& str) ynothrow
	{
		using char_value_t = decltype(str[0]);

		return ystdex::string_length(str) == 2 && str[0] == char_value_t('.')
			&& str[1] == char_value_t('.');
	}

	template<typename _tString>
	static yconstfn bool
	is_absolute(const _tString& x) ynothrow
	{
		return ystdex::string_empty(x);
	}

	template<typename _tString>
	static yconstfn bool
	is_self(const _tString& str) ynothrow
	{
		return
			ystdex::string_length(str) == 1 && str[0] == decltype(str[0])('.');
	}

	template<typename _tString>
	static yconstfn bool
	has_root_name(const _tString& x) ynothrow
	{
		return ystdex::string_empty(x);
	}

	template<typename _tString>
	static yconstfn bool
	has_root_path(const _tString& x) ynothrow
	{
		return ystdex::string_empty(x);
	}

	template<typename _tString1, typename _tString2>
	static yconstfn bool
	have_same_root_names(const _tString1& x, const _tString2& y) ynothrow
	{
		return x == y;
	}
};

//! \brief 文件字符串路径特征。
template<typename _tChar, class _tAlloc>
struct path_traits<basic_string<_tChar, _tAlloc>>
{
	using char_type = _tChar;
	using value_type = basic_string<_tChar, _tAlloc>;
	using view_type = basic_string_view<char_type>;

	static yconstfn bool
	is_parent(view_type sv) ynothrow
	{
		return sv.length() == 2 && sv[0] == '.' && sv[1] == '.';
	}

	//! \since build 836
	//@{
	static yconstfn bool
	is_absolute(view_type sv) ynothrow
	{
		return sv.empty();
	}

	static yconstfn bool
	is_self(view_type sv) ynothrow
	{
		return sv.length() == 1 && sv.front() == '.';
	}

	static yconstfn bool
	has_root_name(view_type sv) ynothrow
	{
		return sv.empty();
	}

	static yconstfn bool
	has_root_path(view_type sv) ynothrow
	{
		return sv.empty();
	}

	static yconstfn bool
	have_same_root_names(view_type x, view_type y) ynothrow
	{
		return x == y;
	}
	//@}
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

表示一般路径的模板，是元素类型 value_type 的容器。
初始化路径时可通过路径名解析确定，但作为抽象的路径，
	不直接提供处理具体 value_type 值的接口。
和 ISO C++17 [fs.class.path] 的 std::filesystem::path 的作用和设计有类似之处，
	但有一些显著不同：
	非单一类型而是被特征参数化的类模板。
		使用特征配置包括外部表示检查和转换在内的若干行为。
	表示的是抽象的路径，不依赖文件系统。
		具体路径的处理不一定由 path 模板提供。
		对应的路径名也不限文件系统。
	路径名不在 path 中提供格式转换。
		不提供为特定具体路径适配的格式选项（如 WG21 P430R2 ）。
	无根路径部分对应的路径名假定使用一种分隔符。
		使用不同分隔符的外部表示，如 OpenVMS 本机路径不被作为路径名直接支持。
		路径名以外的外部表示和操作在 path 不特别处理。
	容器对值的表示透明，不指定内部表示及限制，包括编码和允许的字符。
		容器元素的值类型是串类型而不是字符类型。
			因为 value_type 是串类型，不提供 string_type 。
		内部表示不含分隔符。
		特征以外不假定具体分隔符的值。
		在 path 中提供的操作不依赖已在容器内的元素是否为空，也不检查这些元素。
		表示路径的合法值校验需在解析时外部字符串时由外部指定，
			如值是否编码一致且以 NUL 结尾。
		追加(append) 操作 operator/= 对特定的路径类别进行正规化操作，
			参见 operator/= 的说明。
		非解析的追加操作使用 push_back 等容器对元素的通用操作替代。
		不单独设置对元素自身的操作如 operator+= ；可使用对元素的直接操作替代。
	首个元素表示根路径（若存在），内部表示不区分根路径中的根名称和根目录。
		需要区分时通过特征操作，
			参见具有 const path& 参数类型的 operator/= 重载的说明。
*/
template<class _tSeqCon,
	class _tTraits = path_traits<typename _tSeqCon::value_type>>
class path : private sequence_container_adaptor<_tSeqCon>,
	private dividable<path<_tSeqCon, _tTraits>, typename _tSeqCon::value_type>,
	private totally_ordered<path<_tSeqCon, _tTraits>>,
	private dividable<path<_tSeqCon, _tTraits>>
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

	/*!
	\brief 追加路径元素。
	\since build 600

	按特征中的谓词判断路径元素类别并处理路径元素。
	特定类别的路径元素按特定行为修改当前路径：
		谓词 is_parent 指定切换当前路径到父目录，若不能满足则被忽略；
		谓词 is_self 指定不切换当前目录，被无条件忽略。
	没有符合以上谓词的路径元素按容器的 push_back 调用被追加。
	处理 is_parent 谓词的路径时，可能修改现有无根路径中的部分元素，而不仅如
		std::filesystem::path::operator/= 修改全部的相对路径。
	*/
	//@{
	path&
	operator/=(const value_type& s)
	{
		if(!empty() && traits_type::is_parent(s))
		{
			if(has_leaf_nonempty())
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
	path&
	operator/=(value_type&& s) ynothrow
	{
		if(!empty() && traits_type::is_parent(s))
		{
			if(!traits_type::is_absolute(front()) || 1U < size())
			{
				if(!(traits_type::is_parent(back())
					|| traits_type::is_self(back())))
					pop_back();
				else
					push_back(std::move(s));
			}
		}
		else if(!traits_type::is_self(s))
			push_back(std::move(s));
		return *this;
	}
	//@}
	/*!
	\brief 追加路径。
	\sa root_diverged

	当参数为绝对路径或具有和 *this 相同的非空根名称时，替换路径；
		否则，顺序追加路径中的每个元素。
	若需要特征以外的其它比较操作，可由用户代码单独根据根的判断。
	*/
	//@{
	/*!
	\warning 不完全强异常安全：只保证非替换时。
	\since build 600
	*/
	path&
	operator/=(const path& pth)
	{
		if(!root_diverged(pth))
			for(const auto& s : pth)
				*this /= s;
		else
			*this = pth;
		return *this;
	}
	path&
	operator/=(path&& pth) ynothrow
	{
		if(!root_diverged(pth))
			for(auto& s : pth)
				*this /= std::move(s);
		else
			*this = std::move(pth);
		return *this;
	}
	//@}

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

	//! \since build 732
	void
	filter_self() ynothrow
	{
		// NOTE: Lambda is needed to support possibly overloaded function.
		ystdex::erase_all_if(*this, [](const value_type& s) ynothrow{
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
		return !empty() && traits_type::is_absolute(front());
	}

	bool
	is_relative() const ynothrow
	{
		return !is_absolute();
	}

	void
	merge_parents()
	{
		for(auto i(this->begin()); i != this->end(); )
		{
			auto j(std::adjacent_find(i, this->end(),
				[&](const value_type& x, const value_type& y){
					return !traits_type::is_self(x) && !traits_type::is_parent(
						x) && traits_type::is_parent(y);
			}));

			if(j == this->end())
				break;
			i = j++;
			yassume(j != this->end());
			i = erase(i, ++j);
		}
	}

	//! \since build 837
	//@{
	//! \brief 判断移除最后元素是否保持根路径不变。
	//@{
	YB_ATTR_nodiscard bool
	has_leaf() const ynothrow
	{
		return !empty() && has_leaf_nonempty();
	}

	//! \pre 断言：路径非空。
	YB_ATTR_nodiscard bool
	has_leaf_nonempty() const ynothrowv
	{
		yconstraint(!empty());
		return !traits_type::has_root_path(front()) || 1U < size();
	}
	//@}

	/*!
	\brief 重定向路径

	同以路径为参数的 \c operator/= ，但若不替换路径，先调用 remove_leaf 。
	*/
	//@{
	path&
	redirect(const path& pth)
	{
		if(!root_diverged(pth))
		{
			remove_leaf();
			for(const auto& s : pth)
				*this /= s;
		}
		else
			*this = pth;
		return *this;
	}
	path&
	redirect(path&& pth) ynothrow
	{
		if(!root_diverged(pth))
		{
			remove_leaf();
			for(auto& s : pth)
				*this /= std::move(s);
		}
		else
			*this = std::move(pth);
		return *this;
	}
	//@}

	//! \brief 若移除最后元素不会改变根路径，则移除最后的元素。
	bool
	remove_leaf() ynothrow
	{
		if(has_leaf())
		{
			pop_back();
			return true;
		}
		return {};
	}
	//@}

	/*!
	\brief 判断参数指定的路径是否和 \c *this 不同而无法直接作为相对路径追加。
	\sa traits_type::is_absolute
	\sa traits_type::has_root_name
	\sa traits_type::have_same_root_names
	\see WG21 P0492R2 。
	\since build 836

	检查两个路径元素。若参数是绝对路径，或者存在和 \c *this 不相同的根名称，
		则视为相异。
	和 WG21 P0492R2 中对 US 77 和 CA 6 涉及非根路径的追加操作的解决方案类似，
		非相对路径检查相对路径时直接替代。
	根名称比较由特征确定。
	*/
	bool
	root_diverged(const path& pth) const
	{
		return !pth.empty() && (traits_type::is_absolute(pth.front())
			|| (traits_type::has_root_name(pth.front())
			&& !traits_type::have_same_root_names(front(), pth.front())));
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

	if(_tTraits::has_root_path(res))
	{
		auto j(i);

		if(++j != pth.end())
		{
			res += *j;
			i = j;
		}
	}
	while(++i != pth.end())
	{
		res += seperator;
		res += *i;
	}
	return res;
}

//! \brief 取以分隔符结束的字符串表示。
template<class _tSeqCon, class _tTraits>
typename _tSeqCon::value_type
to_string_d(const path<_tSeqCon, _tTraits>& pth, typename string_traits<typename
	_tSeqCon::value_type>::value_type delimiter = '/')
{
	static_assert(is_object<typename _tSeqCon::value_type>(),
		"Invalid type found.");
	auto i(pth.begin());
	typename _tSeqCon::value_type res(*i);

	if(!_tTraits::has_root_path(res))
		res += delimiter;
	while(++i != pth.end())
	{
		res += *i;
		res += delimiter;
	}
	return res;
}
//@}

} // namespace ystdex;

#endif

