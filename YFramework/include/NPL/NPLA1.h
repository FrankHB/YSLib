/*
	© 2014-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA1.h
\ingroup NPLA1
\brief NPLA1 公共接口。
\version r328
\author FrankHB <frankhb1989@gmail.com>
\since build 472
\par 创建时间:
	2014-02-02 17:58:24 +0800
\par 修改时间:
	2015-05-12 17:13 +0800
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
#include <ystdex/functional.hpp> // for ystdex::id_func_clr_t;

namespace NPL
{

//! \since build 341
using YSLib::string;

//! \since build 335
using YSLib::ValueNode;


/*!
\brief NPLA 元标签。
\note NPLA 是 NPL 的抽象实现。
\since build 597
*/
struct YF_API NPLATag : NPLTag
{};


/*!
\brief 节点映射操作类型：变换节点为另一个节点。
\since buld 501
*/
using NodeMapper = std::function<ystdex::id_func_clr_t<ValueNode>>;

//! \since buld 597
//@{
using NodeToString = std::function<string(const ValueNode&)>;

template<class _tCon>
using GNodeInserter = std::function<void(ValueNode&&, _tCon&)>;

using NodeInserter = GNodeInserter<ValueNode::Container&>;

/*!
\brief 节点序列容器。

除分配器外满足和 std::vector 相同的要求的模板的一个实例，元素为 ValueNode 类型。
*/
using NodeSequence = yimpl(YSLib::vector)<ValueNode>;

using NodeSequenceInserter = GNodeInserter<NodeSequence>;
//@}


/*!
\brief 映射 NPLA 叶节点。
\since build 597
\sa ParseNPLANodeString

创建新节点。若参数为空则返回值为空串的新节点；否则值以 ParseNPLANodeString 取得。
*/
YF_API ValueNode
MapNPLALeafNode(const ValueNode&);

/*!
\brief 转义 NPLA 节点字面量。
\return 调用 EscapeLiteral 转义 ParseNPLANodeString 的结果。
\since build 597
*/
YF_API string
EscapeNodeLiteral(const ValueNode&);

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
\since build 597
*/
struct YF_API NPLA1Tag : NPLATag
{};


//! \since build 597
//@{
//! \brief 插入 NPLA1 子节点。
//@{
/*!
\note 插入后按名称排序顺序。

第一参数指定的变换结果插入第二参数指定的容器。
若映射操作返回节点名称为空则根据当前容器内子节点数量加前缀 $ 命名以避免重复。
*/
YF_API void
InsertNPLA1Child(ValueNode&&, ValueNode::Container&);

/*!
\note 保持顺序。

直接插入 NPLA1 子节点到序列容器末尾。
*/
YF_API void
InsertNPLA1SequenceChild(ValueNode&&, NodeSequence&);
//@}

/*!
\brief 变换 NPLA1 节点 S 表达式抽象语法树为 NPLA1 语义结构。
\exception std::bad_function_call 第三至五参数为空。
\return 变换后的新节点（及子节点）。

第一参数指定源节点，其余参数指定部分变换规则。
当被调用的第二至第四参数不修改传入的节点参数时变换不修改源节点。
过程如下：
若源节点为叶节点，直接返回使用第三参数创建映射的节点。
若源节点只有一个子节点，直接返回这个子节点的变换结果。
否则，使用第四参数从第一个子节点取作为变换结果名称的字符串。
	若名称非空则忽略第一个子节点，只变换剩余子节点。
		当剩余一个子节点（即源节点有两个子节点）时，直接递归变换这个节点并返回。
		若变换后的结果名称非空，则作为结果的值；否则，结果作为容器内单一的值。
	否则，新建节点容器，遍历并变换剩余的节点插入这个容器，返回以这个容器构造的节点。
		第二参数指定此时的映射操作，若为空则默认使用递归 TransformNPLA1 调用。
		调用第五参数插入映射的结果到容器。
*/
YF_API ValueNode
TransformNPLA1(const ValueNode&, NodeMapper = {}, NodeMapper = MapNPLALeafNode,
	NodeToString = ParseNPLANodeString, NodeInserter = InsertNPLA1Child);

/*!
\brief 变换 NPLA1 节点 S 表达式抽象语法树为 NPLA1 序列语义结构。
\exception std::bad_function_call 第三至五参数为空。
\return 变换后的新节点（及子节点）。
\sa TransformNPLA1

和 TransformNPLA1 变换规则相同，
但插入的子节点以 NodeSequence 的形式作为变换节点的值而不是子节点，可保持顺序。
*/
YF_API ValueNode
TransformNPLA1Sequence(const ValueNode&, NodeMapper = {},
	NodeMapper = MapNPLALeafNode, NodeToString = ParseNPLANodeString,
	NodeSequenceInserter = InsertNPLA1SequenceChild);


/*!
\brief 加载 NPLA1 翻译单元。
\throw YSLib::LoggedEvent 警告：被加载配置中的实体转换失败。
*/
//@{
template<typename _type, typename... _tParams>
ValueNode
LoadNPLA1(_type&& tree, _tParams&&... args)
{
	TryRet(NPL::TransformNPLA1(std::forward<ValueNode&&>(tree),
		yforward(args)...))
	CatchThrow(ystdex::bad_any_cast& e, YSLib::LoggedEvent(YSLib::sfmt(
		"Bad NPLA1 tree found: cast failed from [%s] to [%s] .", e.from(),
		e.to()), YSLib::Warning))
}

template<typename _type, typename... _tParams>
ValueNode
LoadNPLA1Sequence(_type&& tree, _tParams&&... args)
{
	TryRet(NPL::TransformNPLA1Sequence(std::forward<ValueNode&&>(tree),
		yforward(args)...))
	CatchThrow(ystdex::bad_any_cast& e, YSLib::LoggedEvent(YSLib::sfmt(
		"Bad NPLA1 tree found: cast failed from [%s] to [%s] .", e.from(),
		e.to()), YSLib::Warning))
}
//@}


//! \brief 生成前缀缩进的函数类型。
using IndentGenerator = std::function<string(size_t)>;

//! \brief 生成水平制表符为单位的缩进。
YF_API string
DefaultGenerateIndent(size_t);

/*!
\brief 打印缩进。
\note 若最后一个参数等于零则无副作用。
*/
YF_API void
PrintIndent(std::ostream&, IndentGenerator = DefaultGenerateIndent, size_t = 1);

/*!
\brief 打印 NPLA1 节点。
\sa PrintIdent
\sa PrintNodeChild
\sa PrintNodeString

打印缩进前缀后递归遍历，打印自身内容，按需调用 PrintNodeChild 打印子节点内容。
先尝试打印节点字符串；若失败则尝试调用 PrintNodeChild 打印 NodeSequence ；
再次失败则调用 PrintNodeChild 打印子节点。
*/
YF_API void
PrintNode(std::ostream&, const ValueNode&, NodeToString = EscapeNodeLiteral,
	IndentGenerator = DefaultGenerateIndent, size_t = 0);

/*!
\brief 打印作为子节点的 NPLA1 节点。
\sa IsPrefixedIndex
\sa PrintIdent
\sa PrintNodeString

打印缩进前缀后打印节点内容。
对满足 IsPrefixedIndex 判断的节点调用 PrintNodeString 作为节点字符串打印；
否则，递归打印子节点。
*/
YF_API void
PrintNodeChild(std::ostream&, const ValueNode&, NodeToString
	= EscapeNodeLiteral, IndentGenerator = DefaultGenerateIndent, size_t = 0);

/*!
\brief 打印节点字符串。
\return 是否成功访问节点字符串并输出。
\note ystdex::bad_any_cast 外异常中立。

使用最后一个参数指定的访问节点，打印得到的字符串和换行符。
*/
YF_API bool
PrintNodeString(std::ostream&, const ValueNode&,
	NodeToString = EscapeNodeLiteral);


namespace SXML
{

/*!
\brief 转换 SXML 节点为 XML 属性字符串。
\throw LoggedEvent 没有子节点。
\note 当前不支持 annotation ，在超过 2 个子节点时使用 YTraceDe 警告。
*/
YF_API string
ConvertAttributeNodeString(const ValueNode&);

/*!
\brief 转换 SXML 文档节点为 XML 字符串。
\throw LoggedEvent 不符合最后一个参数约定的内容被解析。
\throw ystdex::unimplemented 指定 ParseOption::Strict 时解析未实现内容。
\sa ConvertStringNode
\see http://okmij.org/ftp/Scheme/SXML.html#Annotations 。
\todo 支持 *ENTITY* 和 *NAMESPACES* 标签。

转换 SXML 文档节点为 XML 。
尝试使用 ConvertStringNode 转换字符串节点，若失败作为非叶子节点递归转换。
因为当前 SXML 规范未指定注解(annotation) ，所以直接忽略。
*/
YF_API string
ConvertDocumentNode(const ValueNode&, IndentGenerator = DefaultGenerateIndent,
	size_t = 0, ParseOption = ParseOption::Normal);

/*!
\brief 转换 SXML 节点为被转义的 XML 字符串。
\sa EscapeXML
*/
YF_API string
ConvertStringNode(const ValueNode&);

/*!
\brief 打印 SContext::Analyze 分析取得的 SXML 语法树节点并刷新流。
\see ConvertDocumentNode
\see SContext::Analyze
\see Session

参数节点中取第一个节点作为 SXML 文档节点调用 ConvertStringNode 输出并刷新流。
*/
YF_API void
PrintSyntaxNode(std::ostream& os, const ValueNode&,
	IndentGenerator = DefaultGenerateIndent, size_t = 0);

} // namespace SXML;
//@}

} // namespace NPL;

#endif

