/*
	© 2012-2016, 2019-2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Configuration.h
\ingroup NPL
\brief 配置设置。
\version r407
\author FrankHB <frankhb1989@gmail.com>
\since build 334
\par 创建时间:
	2012-08-27 15:15:08 +0800
\par 修改时间:
	2021-12-29 01:20 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::Configuration
*/


#ifndef NPL_INC_Configuration_h_
#define NPL_INC_Configuration_h_

#include "YModules.h"
#include YFM_NPL_NPLA1 // for ValueNode, NoContainer,
//	ystdex::exclude_self_params_t;

namespace NPL
{

//! \since build 889
namespace A1
{

/*!
\brief 加载 NPLA1 翻译单元。
\throw LoggedEvent 警告：被加载配置中的实体转换失败。
\since build 665
*/
template<typename _type, typename... _tParams>
YB_ATTR_nodiscard ValueNode
LoadNode(_type&& tree, _tParams&&... args)
{
	TryRet(A1::TransformNode(std::forward<TermNode>(tree), yforward(args)...))
	CatchThrow(bad_any_cast& e, LoggedEvent(YSLib::sfmt(
		"Bad NPLA1 tree found: cast failed from [%s] to [%s] .", e.from(),
		e.to()), YSLib::Warning))
}

} // namespace A1;


/*!
\brief 设置：使用 S 表达式存储外部状态。
\warning 非虚析构。
\since build 334
*/
class YF_API Configuration
{
private:
	ValueNode root;

public:
	DefDeCtor(Configuration)
	//! \since build 376
	//@{
	Configuration(const ValueNode& node)
		: root(node)
	{}
	Configuration(ValueNode&& node)
		: root(std::move(node))
	{}
	//@}
	//! \since build 848
	template<typename... _tParams, yimpl(typename
		= ystdex::exclude_self_params_t<Configuration, _tParams...>)>
	explicit
	Configuration(_tParams&&... args)
		: root(NoContainer, yforward(args)...)
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

