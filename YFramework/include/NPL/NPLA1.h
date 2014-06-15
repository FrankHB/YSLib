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
\version r133
\author FrankHB <frankhb1989@gmail.com>
\since build 472
\par 创建时间:
	2014-02-02 17:58:24 +0800
\par 修改时间:
	2014-06-14 22:59 +0800
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

//! \since build 341
using YSLib::string;

//! \since build 335
using YSLib::ValueNode;


//! \since build 472
//@{
/*!
\brief NPLA 元标签。
\note NPLA 是 NPL 的抽象实现。
*/
struct YF_API NPLA : NPL
{};


/*!
\brief 解析 NPLA 节点字符串。
\since build 508

以 string 类型访问节点，若失败则结果为空串。
*/
YF_API string
ParseNPLANodeString(const ValueNode&);


/*!
\brief NPLA1 元标签。
\note NPLA1 是 NPLA 的具体实现。
*/
struct YF_API NPLA1 : NPLA
{};
//@}


//! \since build 501
//@{
//! \brief 节点映射操作类型：变换节点为另一个节点。
using NodeMapper = ValueNode(const ValueNode&);


/*!
\brief 变换 NPLA1 节点 S 表达式抽象语法树为 NPLA1 语义结构。

仅在变换子节点时使用指定的映射操作。映射操作需返回被转换的节点（及子节点）。
默认映射操作递归调用 TransformNPLA1 变换节点。
对非 string 类型的叶节点变换时直接复制名称并转移值，否则不修改原节点的内容。
若映射操作返回节点名称为空则根据当前节点大小加前缀 $ 命名。
*/
YF_API ValueNode
TransformNPLA1(const ValueNode&, std::function<NodeMapper> = {});


//! \brief 加载 NPLA1 翻译单元。
//@{
YF_API ValueNode
LoadNPLA1(ValueNode&&, std::function<NodeMapper> = {});
template<typename _type>
ValueNode
LoadNPLA1(_type&& tree, std::function<NodeMapper> mapper = {})
{
	return LoadNPLA1(tree, mapper);
}
//@}
//@}

} // namespace NPL;

#endif

