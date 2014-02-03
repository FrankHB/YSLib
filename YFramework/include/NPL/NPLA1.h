/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA1.h
\ingroup NPLA1
\brief NPLA1 公共接口。
\version r89
\author FrankHB <frankhb1989@gmail.com>
\since build 472
\par 创建时间:
	2014-02-02 17:58:24 +0800
\par 修改时间:
	2014-02-02 18:20 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA1
*/


#ifndef NPL_INC_NPLA1_h_
#define NPL_INC_NPLA1_h_ 1

#include "YModules.h"
#include YFM_NPL_NPL
#include YFM_YSLib_Core_ValueNode

namespace NPL
{

//! \since build 472
//@{
/*!
\brief NPLA 元标签。
\note NPLA 是 NPL 的抽象实现。
*/
struct YF_API NPLA : NPL
{};


/*!
\brief NPLA1 元标签。
\note NPLA1 是 NPLA 的具体实现。
*/
struct YF_API NPLA1 : NPLA
{};
//@}


//! \since build 341
using YSLib::string;

//! \since build 335
using YSLib::ValueNode;


/*!
\brief 转换设置： S 表达式抽象语法树变换为 NPLA1 语义结构。
\since build 472
*/
YF_API ValueNode
TransformNPLA1(const ValueNode&);


/*!
\brief 读取 NPLA1 翻译单元。
\since build 449
*/
//@{
YF_API ValueNode
LoadNPLA1(ValueNode&&);
template<typename _type>
ValueNode
LoadNPLA1(_type&& tree)
{
	return LoadNPLA1(tree);
}
//@}

} // namespace NPL;

#endif

