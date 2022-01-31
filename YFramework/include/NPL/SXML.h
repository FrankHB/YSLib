/*
	© 2015-2016, 2019, 2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file SXML.h
\ingroup NPL
\brief NPL SXML 实现。
\version r11576
\author FrankHB <frankhb1989@gmail.com>
\since build 936
\par 创建时间:
	2022-01-20 17:41:14 +0800
\par 修改时间:
	2022-01-24 02:30 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::SXML
*/


#ifndef NPL_INC_SXML_h_
#define NPL_INC_SXML_h_ 1

#include "YModules.h"
#include YFM_NPL_NPLA // for std::initializer_list, TermNode, string,
//	IndentGenerator, MakeIndex;

namespace NPL
{

/*!
\note 在指定的节点插入节点，可用于语法分析树的输入。
\sa MakeIndex
\sa TransformToSyntaxNode
\since build 852
*/
//@{
//! \brief 插入语法节点。
//@{
template<class _tNodeOrCon, typename... _tParams>
TNIter
InsertSyntaxNode(_tNodeOrCon&& node_or_con,
	std::initializer_list<TermNode> il, _tParams&&... args)
{
	return node_or_con.emplace(TermNode::Container(il), yforward(args)...);
}
template<class _tNodeOrCon, typename _type, typename... _tParams>
TNIter
InsertSyntaxNode(_tNodeOrCon&& node_or_con, _type&& arg, _tParams&&... args)
{
	return node_or_con.emplace(yforward(arg), yforward(args)...);
}
//@}

//! \brief 插入语法子节点。
//@{
template<class _tNodeOrCon>
TNIter
InsertChildSyntaxNode(_tNodeOrCon&& node_or_con, TermNode& child)
{
	return NPL::InsertSyntaxNode(yforward(node_or_con),
		child.GetContainerRef());
}
template<class _tNodeOrCon>
TNIter
InsertChildSyntaxNode(_tNodeOrCon&& node_or_con, TermNode&& child)
{
	return NPL::InsertSyntaxNode(yforward(node_or_con),
		std::move(child.GetContainerRef()));
}
template<class _tNodeOrCon>
TNIter
InsertChildSyntaxNode(_tNodeOrCon&& node_or_con, NodeLiteral&& nl)
{
	return NPL::InsertChildSyntaxNode(yforward(node_or_con),
		TransformToSyntaxNode(std::move(nl.GetNodeRef())));
}
//@}
//@}

/*!
\brief SXML 表示。
\see http://okmij.org/ftp/Scheme/SXML.html 。
\since build 597
*/
namespace SXML
{

//! \brief 限定解析选项。
enum class ParseOption
{
	Normal,
	Strict,
	String,
	Attribute
};


//! \since build 674
//@{
/*!
\brief 转换 SXML 节点为 XML 属性字符串。
\throw LoggedEvent 没有子节点。
\note 当前不支持 annotation ，在超过 2 个子节点时使用 YTraceDe 警告。
*/
YB_ATTR_nodiscard YF_API YB_PURE string
ConvertAttributeNodeString(const TermNode&);

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
YB_ATTR_nodiscard YF_API string
ConvertDocumentNode(const TermNode&, IndentGenerator = DefaultGenerateIndent,
	size_t = 0, ParseOption = ParseOption::Normal);

/*!
\brief 转换 SXML 节点为被转义的 XML 字符串。
\sa EscapeXML
*/
YB_ATTR_nodiscard YF_API YB_PURE string
ConvertStringNode(const TermNode&);

/*!
\brief 打印 SContext::Analyze 分析取得的 SXML 语法树节点并刷新流。
\see ConvertDocumentNode
\see SContext::Analyze
\see Session

参数节点中取第一个节点作为 SXML 文档节点调用 ConvertStringNode 输出并刷新流。
*/
YF_API void
PrintSyntaxNode(std::ostream& os, const TermNode&,
	IndentGenerator = DefaultGenerateIndent, size_t = 0);
//@}


//! \since build 852
//@{
template<typename _tString, typename _tLiteral = NodeLiteral>
YB_ATTR_nodiscard inline YB_PURE TermNode
NodeLiteralToTerm(_tString&& name, std::initializer_list<_tLiteral> il)
{
	return TermNode(YSLib::AsNodeLiteral(yforward(name), il));
}

//! \brief 构造 SXML 文档顶级节点。
//@{
template<typename... _tParams>
YB_ATTR_nodiscard YB_PURE TermNode
MakeTop(const string& name, _tParams&&... args)
{
	return SXML::NodeLiteralToTerm(name,
		{{MakeIndex(0), "*TOP*"}, NodeLiteral(yforward(args))...});
}
YB_ATTR_nodiscard YB_PURE inline PDefH(TermNode, MakeTop, )
	ImplRet(MakeTop({}))
//@}

/*!
\brief 构造 SXML 文档 XML 声明节点。
\note 第一参数指定节点名称，其余参数指定节点中 XML 声明的值：版本、编码和独立性。
\note 最后两个参数可选为空值，此时结果不包括对应的属性。
\warning 不对参数合规性进行检查。
*/
YB_ATTR_nodiscard YF_API YB_PURE TermNode
MakeXMLDecl(const string& = {}, const string& = "1.0",
	const string& = "UTF-8", const string& = {});

/*!
\brief 构造包含 XML 声明的 SXML 文档节点。
\sa MakeTop
\sa MakeXMLDecl
*/
YB_ATTR_nodiscard YF_API YB_PURE TermNode
MakeXMLDoc(const string& = {}, const string& = "1.0",
	const string& = "UTF-8", const string& = {});
//@}

//! \since build 599
//@{
//! \brief 构造 SXML 属性标记字面量。
//@{
YB_ATTR_nodiscard YB_PURE inline PDefH(NodeLiteral, MakeAttributeTagLiteral,
	std::initializer_list<NodeLiteral> il)
	ImplRet({"@", il})
template<typename... _tParams>
YB_ATTR_nodiscard YB_PURE NodeLiteral
MakeAttributeTagLiteral(_tParams&&... args)
{
	return SXML::MakeAttributeTagLiteral({NodeLiteral(yforward(args)...)});
}
//@}

//! \brief 构造 XML 属性字面量。
//@{
YB_ATTR_nodiscard YB_PURE inline PDefH(NodeLiteral, MakeAttributeLiteral,
	const string& name, std::initializer_list<NodeLiteral> il)
	ImplRet({name, {MakeAttributeTagLiteral(il)}})
template<typename... _tParams>
YB_ATTR_nodiscard YB_PURE NodeLiteral
MakeAttributeLiteral(const string& name, _tParams&&... args)
{
	return {name, {SXML::MakeAttributeTagLiteral(yforward(args)...)}};
}
//@}

//! \brief 插入只有 XML 属性节点到语法节点。
//@{
template<class _tNodeOrCon>
inline void
InsertAttributeNode(_tNodeOrCon&& node_or_con, const string& name,
	std::initializer_list<NodeLiteral> il)
{
	InsertChildSyntaxNode(node_or_con, MakeAttributeLiteral(name, il));
}
template<class _tNodeOrCon, typename... _tParams>
inline void
InsertAttributeNode(_tNodeOrCon&& node_or_con, const string& name,
	_tParams&&... args)
{
	InsertChildSyntaxNode(node_or_con,
		SXML::MakeAttributeLiteral(name, yforward(args)...));
}
//@}
//@}

} // namespace SXML;

} // namespace NPL;

#endif

