/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Configuration.h
\ingroup NPL
\brief 配置设置。
\version r150;
\author FrankHB<frankhb1989@gmail.com>
\since build 334 。
\par 创建时间:
	2012-08-27 15:15:08 +0800;
\par 修改时间:
	2012-09-02 20:03 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	NPL::Configuration;
*/


#ifndef NPL_INC_CONFIGURATION_H_
#define NPL_INC_CONFIGURATION_H_

#include "YSLib/Core/yobject.h"
#include "YSLib/Service/yftext.h"

YSL_BEGIN_NAMESPACE(NPL)

//! \since build 335 。
//@{
using YSLib::File;
using YSLib::TextFile;
using YSLib::ValueNode;
//@}


/*!
\brief 设置：使用 S 表达式存储外部状态。
\since build 334 。
*/
class Configuration
{
private:
	ValueNode root;

public:
	DefDeCtor(Configuration)
	template<typename... _tParams>
	Configuration(_tParams&&... args)
		: root(yforward(args)...)
	{}
	DefDeCopyCtor(Configuration)
	DefDeMoveCtor(Configuration)

	DefDeCopyAssignment(Configuration)
	DefDeMoveAssignment(Configuration)

	DefGetter(const ynothrow, const ValueNode&, Root, root)

	/*!
	\brief 从文件输入设置。
	\since build 334 。
	*/
	friend TextFile&
	operator>>(TextFile&, Configuration&);
};

/*!
\brief 输出设置至文件。
\since build 334 。
*/
File&
operator<<(File&, const Configuration&);


/*!
\brief 转换设置： S 表达式抽象语法树转换为 S 表达式设置。
\since build 334 。
*/
ValueNode
TransformConfiguration(const ValueNode&);

YSL_END_NAMESPACE(NPL)

#endif

