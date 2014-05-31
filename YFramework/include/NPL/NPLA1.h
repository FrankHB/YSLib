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
\version r109
\author FrankHB <frankhb1989@gmail.com>
\since build 472
\par 创建时间:
	2014-02-02 17:58:24 +0800
\par 修改时间:
	2014-05-30 10:34 +0800
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

//! \since build 501
//@{
//! \brief 节点映射操作类型：变换节点为另一个节点。
using NodeMapper = ValueNode(const ValueNode&);


//! \brief NPLA1 节点映射。
ValueNode
MapNPLA1Node(const ValueNode&);

/*!
\brief 变换 NPLA1 节点 S 表达式抽象语法树变换为 NPLA1 语义结构。
\note 不修改原节点的内容。
\note 仅在变换子节点时使用映射操作。
\note 若映射操作返回节点名称为空则根据当前节点大小加前缀 $ 命名。
*/
YF_API ValueNode
TransformNPLA1(const ValueNode&, std::function<NodeMapper> = MapNPLA1Node);


//! \brief 加载 NPLA1 翻译单元。
//@{
YF_API ValueNode
LoadNPLA1(ValueNode&&, std::function<NodeMapper> = MapNPLA1Node);
template<typename _type>
ValueNode
LoadNPLA1(_type&& tree, std::function<NodeMapper> mapper = MapNPLA1Node)
{
	return LoadNPLA1(tree, mapper);
}
//@}
//@}

} // namespace NPL;

#endif

