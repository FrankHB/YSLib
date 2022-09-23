/*
	© 2012-2016, 2019-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Configuration.h
\ingroup NPL
\brief 配置设置。
\version r468
\author FrankHB <frankhb1989@gmail.com>
\since build 334
\par 创建时间:
	2012-08-27 15:15:08 +0800
\par 修改时间:
	2022-09-23 01:04 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::Configuration
*/


#ifndef NPL_INC_Configuration_h_
#define NPL_INC_Configuration_h_

#include "YModules.h"
#include YFM_NPL_NPLA1 // for YSLib::default_allocator, byte, ValueNode,
//	TermNode, ContextState, GlobalState, shared_ptr, Environment, std::istream,
//	string_view, ystdex::exclude_self_params_t, NoContainer;

namespace NPL
{

//! \since build 889
namespace A1
{

/*!
\brief 节点加载器。
\since build 956
*/
class YF_API NodeLoader
{
private:
	mutable ContextState cs;

public:
	NodeLoader(const GlobalState&);

	YB_ATTR_nodiscard YB_PURE ValueNode
	LoadNode(const TermNode&) const;
	template<typename _type,
		yimpl(typename = ystdex::exclude_self_t<TermNode, _type>)>
	YB_ATTR_nodiscard YB_PURE inline ValueNode
	LoadNode(_type&& obj) const
	{
		return LoadNode(cs.Global.get().ReadFrom(GlobalState::LoadOptionTag<
			GlobalState::NoSourceInformation>(), yforward(obj), cs));
	}

	DefGetter(const ynothrow, ContextState&, Context, cs)
};

} // namespace A1;


/*!
\brief 设置：使用 S 表达式存储外部状态。
\warning 非虚析构。
\since build 334
*/
class YF_API Configuration
{
public:
	//! \since build 955
	using allocator_type = YSLib::default_allocator<yimpl(byte)>;

private:
	ValueNode root;

public:
	//! \brief 无参数构造：默认构造。
	DefDeCtor(Configuration)
	/*!
	\brief 构造：使用分配器。
	\since build 955
	*/
	Configuration(allocator_type a)
		: root(a)
	{}
	//! \since build 376
	Configuration(const ValueNode& node)
		: root(node)
	{}
	//! \since build 376
	Configuration(ValueNode&& node)
		: root(std::move(node))
	{}
	//! \since build 848
	template<typename... _tParams, yimpl(typename
		= ystdex::exclude_self_params_t<Configuration, _tParams...>)>
	explicit inline
	Configuration(_tParams&&... args)
		: root(NoContainer, yforward(args)...)
	{}
	//! \since build 955
	template<typename... _tParams>
	explicit inline
	Configuration(std::allocator_arg_t, allocator_type a, _tParams&&... args)
		: root(std::allocator_arg, a, NoContainer, yforward(args)...)
	{}
	DefDeCopyMoveCtorAssignment(Configuration)

	DefGetter(const ynothrow, const ValueNode&, Root, root)

	/*!
	\brief 从文件输入设置。
	\throw GeneralEvent 文件无效。
	\throw LoggedEvent 文件内容读取失败。
	\since build 619
	*/
	YF_API friend std::istream&
	operator>>(std::istream&, Configuration&);

	//! \since build 344
	DefGetter(const ynothrow, const ValueNode&, Node, root)
	/*!
	\brief 取设置节点的右值引用。
	\since build 341
	*/
	DefGetter(ynothrow, ValueNode&&, NodeRRef, std::move(root))

	//! \since build 955
	YB_ATTR_nodiscard YB_PURE
		PDefH(allocator_type, get_allocator, ) const ynothrow
		ImplRet(root.get_allocator())
};

/*!
\brief 输出设置至输出流。
\relates Configuration
\since build 600
*/
YF_API std::ostream&
operator<<(std::ostream&, const Configuration&);

} // namespace NPL;

#endif

