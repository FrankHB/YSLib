/*
	© 2012-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Configuration.h
\ingroup NPL
\brief 配置设置。
\version r298
\author FrankHB <frankhb1989@gmail.com>
\since build 334
\par 创建时间:
	2012-08-27 15:15:08 +0800
\par 修改时间:
	2014-12-14 21:50 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::Configuration
*/


#ifndef NPL_INC_Configuration_h_
#define NPL_INC_Configuration_h_

#include "YModules.h"
#include YFM_NPL_NPLA1
#include YFM_YSLib_Service_TextFile

namespace NPL
{

//! \since build 335
//@{
using YSLib::File;
using YSLib::TextFile;
//@}


/*!
\brief 设置：使用 S 表达式存储外部状态。
\since build 334
*/
class YF_API Configuration
{
private:
	ValueNode root;

public:
	DefDeCtor(Configuration)
	//! \since build 432
	Configuration(ValueNode& node)
		: root(static_cast<const ValueNode&>(node))
	{}
	//! \since build 376
	//@{
	Configuration(const ValueNode& node)
		: root(node)
	{}
	Configuration(ValueNode&& node)
		: root(std::move(node))
	{}
	Configuration(Configuration& conf)
		: Configuration(static_cast<const Configuration&>(conf))
	{}
	//@}
	template<typename... _tParams>
	Configuration(_tParams&&... args)
		: root(0, yforward(args)...)
	{}
	DefDeCopyMoveCtorAssignment(Configuration)

	DefGetter(const ynothrow, const ValueNode&, Root, root)

	/*!
	\brief 从文件输入设置。
	\since build 403
	*/
	YF_API friend TextFile&
	operator>>(TextFile&, Configuration&);

	//! \since build 344
	DefGetter(const ynothrow, const ValueNode&, Node, root)
	/*!
	\brief 取设置节点的右值引用。
	\since build 341
	*/
	DefGetter(ynothrow, ValueNode&&, NodeRRef, std::move(root))
};

/*!
\brief 输出设置至文件。
\relates Configuration
\since build 334
*/
YF_API File&
operator<<(File&, const Configuration&);

} // namespace NPL;

#endif

