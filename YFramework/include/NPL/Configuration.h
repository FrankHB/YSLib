/*
	© 2012-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Configuration.h
\ingroup NPL
\brief 配置设置。
\version r336
\author FrankHB <frankhb1989@gmail.com>
\since build 334
\par 创建时间:
	2012-08-27 15:15:08 +0800
\par 修改时间:
	2015-05-24 13:52 +0800
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
	//! \since build 376
	//@{
	Configuration(const ValueNode& node)
		: root(node)
	{}
	Configuration(ValueNode&& node)
		: root(std::move(node))
	{}
	//@}
	//! \since build 596
	//@{
	template<typename _tParam,
		yimpl(typename = ystdex::exclude_self_ctor_t<Configuration, _tParam>)>
	yconstfn
	Configuration(_tParam&& arg)
		: root(0, yforward(arg))
	{}
	template<typename _tParam1, typename _tParam2, typename... _tParams>
	Configuration(_tParam1&& arg1, _tParam2&& arg2, _tParams&&... args)
		: root(0, yforward(arg1), yforward(arg2), yforward(args)...)
	{}
	//@}
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
\brief 输出设置至输出流。
\relates Configuration
\since build 600
*/
YF_API std::ostream&
operator<<(std::ostream&, const Configuration&);


/*!
\brief 从文本文件中读取配置。
\throw GeneralEvent 文件无效或编码非 UTF-8 导致的读取失败。
\since build 344
*/
YF_API ValueNode
ReadConfiguration(TextFile&);

/*!
\brief 写入配置至文本文件。
\throw GeneralEvent 文件无效导致的写入失败。
\since build 344
*/
YF_API void
WriteConfiguration(TextFile&, const ValueNode&);

} // namespace NPL;

#endif

