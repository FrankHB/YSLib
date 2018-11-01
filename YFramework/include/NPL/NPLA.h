/*
	© 2014-2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA.h
\ingroup NPL
\brief NPLA 公共接口。
\version r4317
\author FrankHB <frankhb1989@gmail.com>
\since build 663
\par 创建时间:
	2016-01-07 10:32:34 +0800
\par 修改时间:
	2018-10-25 14:32 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA
*/


#ifndef NPL_INC_NPLA_h_
#define NPL_INC_NPLA_h_ 1

#include "YModules.h"
#include YFM_NPL_SContext // for NPLTag, ValueNode, TermNode, string,
//	LoggedEvent, ystdex::isdigit, YSLib::lref, ystdex::any,
//	ystdex::equality_comparable, shared_ptr, ystdex::type_info, weak_ptr,
//	ystdex::get_equal_to, ystdex::exchange;
#include <ystdex/base.h> // for ystdex::derived_entity;
#include YFM_YSLib_Core_YEvent // for ystdex::indirect, ystdex::fast_any_of,
//	YSLib::GHEvent, YSLib::GEvent, YSLib::GCombinerInvoker,
//	YSLib::GDefaultLastValueInvoker;
#include <algorithm> // for std::for_each;
#include <ystdex/any.h> // for ystdex::any;

namespace NPL
{

/*!	\defgroup ThunkType Thunk Types
\brief 中间值类型。
\since build 753

标记特定求值策略，储存于 TermNode 的 Value 数据成员中不直接表示宿主语言对象的类型。
*/

//! \since build 599
using YSLib::MakeIndex;
//! \since build 600
using YSLib::NodeSequence;
//! \since build 600
using YSLib::NodeLiteral;
//! \since build 788
//@{
using YSLib::enable_shared_from_this;
//! \since build 842
using YSLib::lref;
using YSLib::make_shared;
using YSLib::make_weak;
using YSLib::observer_ptr;
//! \since build 598
using YSLib::pair;
//! \since build 598
using YSLib::to_string;
using YSLib::shared_ptr;
using YSLib::weak_ptr;
//@}


/*!
\brief NPLA 元标签。
\note NPLA 是 NPL 的抽象实现。
\since build 597
*/
struct YF_API NPLATag : NPLTag
{};


/*!
\brief 插入语法节点。
\since build 599

在指定的节点插入以节点大小字符串为名称的节点，可用于语法分析树。
*/
//@{
template<class _tNodeOrCon, typename... _tParams>
ValueNode::iterator
InsertSyntaxNode(_tNodeOrCon&& node_or_con,
	std::initializer_list<ValueNode> il, _tParams&&... args)
{
	return node_or_con.emplace_hint(node_or_con.end(), ValueNode::Container(il),
		MakeIndex(node_or_con), yforward(args)...);
}
template<class _tNodeOrCon, typename _type, typename... _tParams>
ValueNode::iterator
InsertSyntaxNode(_tNodeOrCon&& node_or_con, _type&& arg, _tParams&&... args)
{
	return node_or_con.emplace_hint(node_or_con.end(), yforward(arg),
		MakeIndex(node_or_con), yforward(args)...);
}
//@}


/*!
\brief 节点映射操作类型：变换节点为另一个节点。
\since buld 501
*/
using NodeMapper = std::function<ValueNode(const TermNode&)>;

//! \since buld 597
//@{
using NodeToString = std::function<string(const ValueNode&)>;

template<class _tCon>
using GNodeInserter = std::function<void(TermNode&&, _tCon&)>;

using NodeInserter = GNodeInserter<TermNode::Container&>;

using NodeSequenceInserter = GNodeInserter<NodeSequence>;
//@}


//! \return 创建的新节点。
//@{
/*!
\brief 映射 NPLA 叶节点。
\sa ParseNPLANodeString
\since build 674

创建新节点。若参数为空则返回值为空串的新节点；否则值以 ParseNPLANodeString 取得。
*/
YF_API ValueNode
MapNPLALeafNode(const TermNode&);

/*!
\brief 变换节点为语法分析树叶节点。
\note 可选参数指定结果名称。
\since build 598
*/
YF_API ValueNode
TransformToSyntaxNode(const ValueNode&, const string& = {});
//@}

/*!
\brief 转义 NPLA 节点字面量。
\return 调用 EscapeLiteral 转义访问字符串的结果。
\exception ystdex::bad_any_cast 异常中立：由 Access 抛出。
\since build 597
*/
YF_API string
EscapeNodeLiteral(const ValueNode&);

/*!
\brief 转义 NPLA 节点字面量。
\return 参数为控节点则空串，否则调用 Literalize 字面量化 EscapeNodeLiteral 的结果。
\exception ystdex::bad_any_cast 异常中立：由 EscapeNodeLiteral 抛出。
\sa EscapeNodeLiteral
\since build 598
*/
YF_API string
LiteralizeEscapeNodeLiteral(const ValueNode&);

/*!
\brief 解析 NPLA 节点字符串。
\since build 508

以 string 类型访问节点，若失败则结果为空串。
*/
YF_API string
ParseNPLANodeString(const ValueNode&);


/*!
\brief 插入语法子节点。
\since build 599

在指定的节点插入以节点大小字符串为名称的节点，可用于语法分析树。
*/
//@{
//! \since build 666
template<class _tNodeOrCon>
ValueNode::iterator
InsertChildSyntaxNode(_tNodeOrCon&& node_or_con, ValueNode& child)
{
	return InsertSyntaxNode(yforward(node_or_con), child.GetContainerRef());
}
template<class _tNodeOrCon>
ValueNode::iterator
InsertChildSyntaxNode(_tNodeOrCon&& node_or_con, ValueNode&& child)
{
	return InsertSyntaxNode(yforward(node_or_con),
		std::move(child.GetContainerRef()));
}
template<class _tNodeOrCon>
ValueNode::iterator
InsertChildSyntaxNode(_tNodeOrCon&& node_or_con, const NodeLiteral& nl)
{
	return
		InsertChildSyntaxNode(yforward(node_or_con), TransformToSyntaxNode(nl));
}
//@}


//! \since build 597
//@{
//! \brief 生成前缀缩进的函数类型。
using IndentGenerator = std::function<string(size_t)>;

//! \brief 生成水平制表符为单位的缩进。
YF_API string
DefaultGenerateIndent(size_t);

//! \exception std::bad_function 异常中立：参数指定的处理器为空。
//@{
/*!
\brief 打印缩进。
\note 若最后一个参数等于零则无副作用。
*/
YF_API void
PrintIndent(std::ostream&, IndentGenerator = DefaultGenerateIndent, size_t = 1);

//! \since build 803
//@{
/*!
\brief 遍历子节点。
\note 使用 ADL AccessPtr 。

遍历节点容器中的子节点。
首先调用 AccessPtr 尝试访问 NodeSequence ，否则直接作为节点容器访问。
*/
template<typename _fCallable, class _type>
void
TraverseSubnodes(_fCallable f, const _type& node)
{
	using YSLib::AccessPtr;

	// TODO: Null coalescing or variant value?
	if(const auto p = AccessPtr<NodeSequence>(node))
		for(const auto& nd : *p)
			ystdex::invoke(f, nd);
	else
		for(const auto& nd : node)
			ystdex::invoke(f, nd);
}

//! \brief 打印容器边界和其中的 NPLA 节点，且在打印边界前调用前置操作。
template<typename _fCallable>
void
PrintContainedNodes(std::ostream& os, std::function<void()> pre, _fCallable f)
{
	pre();
	os << '(' << '\n';
	TryExpr(ystdex::invoke(f))
	CatchIgnore(std::out_of_range&)
	pre();
	os << ')' << '\n';
}

/*!
\brief 打印有索引前缀的节点或遍历子节点并打印。
\note 使用 ADL IsPrefixedIndex 。
\sa IsPrefixedIndex
\sa TraverseSubnodes

以第三参数作为边界前置操作，调用 PrintContainedNodes 逐个打印子节点内容。
调用第四参数输出最后一个参数决定的缩进作为前缀，然后打印子节点内容。
对满足 IsPrefixedIndex 的节点调用第四参数作为节点字符串打印；
否则，调用第五参数递归打印子节点，忽略此过程中的 std::out_of_range 异常。
其中，遍历子节点通过调用 TraverseSubnodes 实现。
*/
template<typename _fCallable, typename _fCallable2>
void
TraverseNodeChildAndPrint(std::ostream& os, const ValueNode& node,
	std::function<void()> pre, _fCallable print_node_str,
	_fCallable2 print_term_node)
{
	using YSLib::IsPrefixedIndex;

	TraverseSubnodes([&](const ValueNode& nd){
		if(IsPrefixedIndex(nd.GetName()))
		{
			pre();
			ystdex::invoke(print_node_str, nd);
		}
		else
			PrintContainedNodes(os, pre, [&]{
				ystdex::invoke(print_term_node, nd);
			});
	}, node);
}
//@}

/*!
\brief 打印 NPLA 节点。
\sa PrintIdent
\sa PrintNodeChild
\sa PrintNodeString
\sa TraverseNodeChildAndPrint

调用第四参数输出最后一个参数决定的缩进作为前缀和一个空格，然后打印节点内容：
先尝试调用 PrintNodeString 打印节点字符串，若成功直接返回；
否则打印换行，对非空节点调用 TraverseNodeChildAndPrint 打印子节点内容。
其中，使用的边界前置操作为调用第四参数输出最后一个参数决定的缩进作为前缀输出。
*/
YF_API void
PrintNode(std::ostream&, const ValueNode&, NodeToString = EscapeNodeLiteral,
	IndentGenerator = DefaultGenerateIndent, size_t = 0);

/*!
\brief 打印节点字符串。
\return 是否成功访问节点字符串并输出。
\note ystdex::bad_any_cast 外异常中立。
\sa PrintNode

使用最后一个参数指定的访问节点，打印得到的字符串和换行符。
忽略 ystdex::bad_any_cast 。
*/
YF_API bool
PrintNodeString(std::ostream&, const ValueNode&,
	NodeToString = EscapeNodeLiteral);
//@}


namespace SXML
{

//! \since build 674
//@{
/*!
\brief 转换 SXML 节点为 XML 属性字符串。
\throw LoggedEvent 没有子节点。
\note 当前不支持 annotation ，在超过 2 个子节点时使用 YTraceDe 警告。
*/
YF_API string
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
YF_API string
ConvertDocumentNode(const TermNode&, IndentGenerator = DefaultGenerateIndent,
	size_t = 0, ParseOption = ParseOption::Normal);

/*!
\brief 转换 SXML 节点为被转义的 XML 字符串。
\sa EscapeXML
*/
YF_API string
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


//! \since build 599
//@{
//! \brief 构造 SXML 文档顶级节点。
//@{
template<typename... _tParams>
ValueNode
MakeTop(const string& name, _tParams&&... args)
{
	return YSLib::AsNodeLiteral(name,
		{{MakeIndex(0), "*TOP*"}, NodeLiteral(yforward(args))...});
}
inline PDefH(ValueNode, MakeTop, )
	ImplRet(MakeTop({}))
//@}

/*!
\brief 构造 SXML 文档 XML 声明节点。
\note 第一个参数指定节点名称，其余参数指定节点中 XML 声明的值：版本、编码和独立性。
\note 最后两个参数可选为空值，此时结果不包括对应的属性。
\warning 不对参数合规性进行检查。
*/
YF_API ValueNode
MakeXMLDecl(const string& = {}, const string& = "1.0",
	const string& = "UTF-8", const string& = {});

/*!
\brief 构造包含 XML 声明的 SXML 文档节点。
\sa MakeTop
\sa MakeXMLDecl
*/
YF_API ValueNode
MakeXMLDoc(const string& = {}, const string& = "1.0",
	const string& = "UTF-8", const string& = {});

//! \brief 构造 SXML 属性标记字面量。
//@{
inline PDefH(NodeLiteral, MakeAttributeTagLiteral,
	std::initializer_list<NodeLiteral> il)
	ImplRet({"@", il})
template<typename... _tParams>
NodeLiteral
MakeAttributeTagLiteral(_tParams&&... args)
{
	return SXML::MakeAttributeTagLiteral({NodeLiteral(yforward(args)...)});
}
//@}

//! \brief 构造 XML 属性字面量。
//@{
inline PDefH(NodeLiteral, MakeAttributeLiteral, const string& name,
	std::initializer_list<NodeLiteral> il)
	ImplRet({name, {MakeAttributeTagLiteral(il)}})
template<typename... _tParams>
NodeLiteral
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
//@}


/*!
\ingroup exceptions
\since build 691
*/
//@{
//! \brief NPL 异常基类。
class YF_API NPLException : public LoggedEvent
{
public:
	YB_NONNULL(2)
	NPLException(const char* str = "", YSLib::RecordLevel lv = YSLib::Err)
		: LoggedEvent(str, lv)
	{}
	NPLException(const string_view sv, YSLib::RecordLevel lv = YSLib::Err)
		: LoggedEvent(sv, lv)
	{}
	DefDeCtor(NPLException)

	//! \brief 虚析构：类定义外默认实现。
	~NPLException() override;
};


/*!
\since build 834
\todo 捕获并保存类型信息。
*/
//@{
//! \brief 类型错误。
class YF_API TypeError : public NPLException
{
public:
	//! \since build 692
	using NPLException::NPLException;
	DefDeCtor(TypeError)

	//! \brief 虚析构：类定义外默认实现。
	~TypeError() override;
};


/*!
\brief 值类别不匹配。
\note 预期左值或右值。
*/
class YF_API ValueCategoryMismatch : public TypeError
{
public:
	//! \since build 834
	using TypeError::TypeError;
	DefDeCtor(ValueCategoryMismatch)

	//! \brief 虚析构：类定义外默认实现。
	~ValueCategoryMismatch() override;
};


/*!
\brief 列表类型错误。
\note 预期列表或列表引用。
*/
class YF_API ListTypeError : public TypeError
{
public:
	//! \since build 834
	using TypeError::TypeError;
	DefDeCtor(ListTypeError)

	//! \brief 虚析构：类定义外默认实现。
	~ListTypeError() override;
};
//@}


/*!
\brief 列表规约失败。
\note 预期 ContextHandler 或引用。
\since build 692
\todo 捕获并保存上下文信息。
*/
class YF_API ListReductionFailure : public TypeError
{
public:
	//! \since build 834
	using TypeError::TypeError;
	DefDeCtor(ListReductionFailure)

	//! \brief 虚析构：类定义外默认实现。
	~ListReductionFailure() override;
};


//! \brief 语法错误。
class YF_API InvalidSyntax : public NPLException
{
public:
	using NPLException::NPLException;
	DefDeCtor(InvalidSyntax)

	//! \brief 虚析构：类定义外默认实现。
	~InvalidSyntax() override;
};


/*!
\brief 参数不匹配。
\since build 771
*/
class YF_API ParameterMismatch : public InvalidSyntax
{
public:
	using InvalidSyntax::InvalidSyntax;
	DefDeCtor(ParameterMismatch)

	//! \brief 虚析构：类定义外默认实现。
	~ParameterMismatch() override;
};


/*!
\brief 元数不匹配错误。
\todo 支持范围匹配。
*/
class YF_API ArityMismatch : public ParameterMismatch
{
private:
	size_t expected;
	size_t received;

public:
	DefDeCtor(ArityMismatch)
	/*!
	\note 前两个参数表示期望和实际的元数。
	\since build 726
	*/
	ArityMismatch(size_t, size_t, YSLib::RecordLevel = YSLib::Err);

	//! \brief 虚析构：类定义外默认实现。
	~ArityMismatch() override;

	DefGetter(const ynothrow, size_t, Expected, expected)
	DefGetter(const ynothrow, size_t, Received, received)
};


/*!
\brief 标识符错误。
\since build 726
*/
class YF_API BadIdentifier : public InvalidSyntax
{
private:
	shared_ptr<string> p_identifier;

public:
	/*!
	\brief 构造：使用作为标识符的字符串、已知实例数和和记录等级。
	\pre 间接断言：第一参数的数据指针非空。

	不检查第一参数内容作为标识符的合法性，直接视为待处理的标识符，
	初始化表示标识符错误的异常对象。
	实例数等于 0 时表示未知标识符；
	实例数等于 1 时表示非法标识符；
	实例数大于 1 时表示重复标识符。
	*/
	YB_NONNULL(2)
	BadIdentifier(const char*, size_t = 0, YSLib::RecordLevel = YSLib::Err);
	BadIdentifier(string_view, size_t = 0, YSLib::RecordLevel = YSLib::Err);
	DefDeCtor(BadIdentifier)

	//! \brief 虚析构：类定义外默认实现。
	~BadIdentifier() override;

	DefGetter(const ynothrow, const string&, Identifier,
		YSLib::Deref(p_identifier))
};


/*!
\brief 无效引用错误。
\since build 822
*/
class YF_API InvalidReference : public NPLException
{
public:
	using NPLException::NPLException;
	DefDeCtor(InvalidReference)

	//! \brief 虚析构：类定义外默认实现。
	~InvalidReference() override;
};
//@}


/*!
\brief 字面量类别。
\since build 734
*/
enum class LexemeCategory
{
	//! \brief 符号：非字面量。
	Symbol,
	//! \brief 代码字面量。
	Code,
	//! \brief 数据字面量。
	Data,
	//! \brief 扩展字面量：由 NPLA 定义的其它字面量类别。
	Extended
};


//! \since build 770
//@{
//! \sa LexemeCategory
//@{
/*!
\pre 断言：字符串参数的数据指针非空。
\return 判断的非扩展字面量分类。
*/
//@{
/*!
\brief 对排除扩展字面量的词素分类。
\note 空字符串和扩展字面量视为非字面量。
*/
YF_API LexemeCategory
CategorizeBasicLexeme(string_view) ynothrowv;

/*!
\brief 对词素分类。
\sa CategorizeBasicLexeme
*/
YF_API LexemeCategory
CategorizeLexeme(string_view) ynothrowv;
//@}

/*!
\brief 判断不是非扩展字面量的词素是否为 NPLA 扩展字面量。
\pre 断言：字符串参数的数据指针非空且字符串非空。
\pre 词素不是代码字面量或数据字面量。
\since build 771
*/
YF_API bool
IsNPLAExtendedLiteral(string_view) ynothrowv;

/*!
\brief 判断字符是否为 NPLA 扩展字面量非数字前缀。
\since build 771
*/
yconstfn PDefH(bool, IsNPLAExtendedLiteralNonDigitPrefix, char c) ynothrow
	ImplRet(c == '#'|| c == '+' || c == '-')

//! \brief 判断字符是否为 NPLA 扩展字面量前缀。
inline PDefH(bool, IsNPLAExtendedLiteralPrefix, char c) ynothrow
	ImplRet(ystdex::isdigit(c) || IsNPLAExtendedLiteralNonDigitPrefix(c))

/*!
\brief 判断词素是否为 NPLA 符号。
\pre 间接断言：字符串参数的数据指针非空且字符串非空。
*/
inline PDefH(bool, IsNPLASymbol, string_view id) ynothrowv
	ImplRet(CategorizeLexeme(id) == LexemeCategory::Symbol)
//@}
//@}


/*!
\ingroup ThunkType
\brief 记号值。
\invariant 值应保证非空。
\note 和被求值的字符串不同的包装类型。
\warning 非空析构。
\since build 756
*/
using TokenValue = ystdex::derived_entity<string, NPLATag>;


/*!
\brief 访问项的值作为记号。
\return 通过访问项的值取得的记号的指针，或空指针表示无法取得名称。
\since build 782
*/
YF_API observer_ptr<const TokenValue>
TermToNamePtr(const TermNode&);

/*!
\return 转换得到的字符串。
\sa TermToString

访问项的值作为名称转换为字符串，若失败则提取值的类型和子项数作为构成值的表示。
除名称外的外部表示方法未指定；结果可能随实现变化。
*/
//@{
/*!
\brief 访问项的值并转换为字符串形式的外部表示。
\since build 801
*/
YF_API string
TermToString(const TermNode&);

/*!
\brief 访问项的值并转换为可选带有引用标记的字符串形式。
\note 当前使用前缀 [*] 和空格表示引用项。
\sa TermToString
\since build 840
*/
YF_API string
TermToStringWithReferenceMark(const TermNode&, bool);
//@}

/*!
\brief 对列表项抛出指定预期访问值的类型的异常。
\throw ListTypeError 消息中包含由参数指定的预期访问值的类型的异常。
\sa IsReferenceTerm
\sa TermToStringWithReferenceMark
\since build 840
*/
YB_NORETURN YF_API void
ThrowListTypeErrorForInvalidType(const ystdex::type_info&, const TermNode&);

/*!
\brief 标记记号节点：递归变换节点，转换其中的词素为记号值。
\note 先变换子节点。
\since build 753
*/
YF_API void
TokenizeTerm(TermNode& term);


/*!
\brief 规约状态：某个项上的一遍规约可能的中间结果。
\since build 730
*/
enum class ReductionStatus : yimpl(size_t)
{
	/*!
	\brief 部分规约：需要继续进行规约。
	\note 一般仅用于异步规约。
	\since build 841
	*/
	Partial,
	/*!
	\brief 规约成功终止且不需要保留子项。
	\since build 841
	*/
	Clean,
	/*!
	\brief 规约成功但需要保留子项。
	\since build 757
	*/
	Retained,
	/*!
	\brief 需要重规约。
	\since build 757
	*/
	Retrying,
};


/*!
\ingroup ThunkType
\brief 延迟求值项。
\note 和被延迟求值的项及其它节点是不同的包装类型。
\warning 非空析构。
\since build 752

直接作为项的值对象包装被延迟求值的项。
*/
using DelayedTerm = ystdex::derived_entity<TermNode, NPLATag>;


//! \since build 840
class Environment;


//! \since build 828
//@{
//! \brief 判断项是否为引用项。
YF_API bool
IsReferenceTerm(const TermNode&) ynothrow;

//! \brief 判断项是否表示左值引用。
YF_API bool
IsLValueTerm(const TermNode&) ynothrow;
//@}


/*!
\ingroup ThunkType
\brief 项引用。
\warning 非虚析构。
\since build 800

表示列表项的引用的中间求值结果的项。
*/
class YF_API TermReference
{
private:
	//! \since build 842
	lref<TermNode> term_ref;
	/*!
	\brief 指定是否以引用值初始化。
	\since build 828
	*/
	bool is_ref;
	/*!
	\brief 引用的锚对象指针。
	\since build 823
	*/
	shared_ptr<const void> p_anchor{};

public:
	//! \brief 构造：使用参数指定的引用和空锚对象并自动判断是否使用引用值初始化。
	TermReference(TermNode& term)
		: TermReference(IsReferenceTerm(term), term)
	{}
	/*!
	\brief 构造：使用参数指定的引用和锚对象并自动判断是否使用引用值初始化。
	\since build 821
	*/
	template<typename _tParam, typename... _tParams>
	TermReference(TermNode& term, _tParam&& arg, _tParams&&... args)
		: TermReference(IsReferenceTerm(term), term, yforward(arg),
		yforward(args)...)
	{}
	//! \since build 828
	//@{
	//! \brief 构造：使用参数指定的是否使用引用值初始化的标记及指定引用和空锚对象。
	TermReference(bool r, TermNode& term)
		: term_ref(term), is_ref(r)
	{}
	//! \brief 构造：使用参数指定的是否使用引用值初始化的标记及指定引用和指定锚对象。
	template<typename _tParam, typename... _tParams>
	TermReference(bool r, TermNode& term, _tParam&& arg, _tParams&&... args)
		: term_ref(term), is_ref(r),
		p_anchor(yforward(arg), yforward(args)...)
	{}
	//! \brief 构造：使用参数指定的是否使用引用值初始化的标记及现有的项引用。
	TermReference(bool r, TermReference t_ref)
		: term_ref(t_ref.term_ref), is_ref(r), p_anchor(t_ref.p_anchor)
	{}
	//@}
	DefDeCopyCtor(TermReference)

	DefDeCopyAssignment(TermReference)

	//! \brief 等于：当且仅当引用的项同一时相等。
	friend PDefHOp(bool, ==, const TermReference& x, const TermReference& y)
		ImplRet(ystdex::get_equal_to<>()(x.term_ref, y.term_ref))

	/*!
	\brief 判断被引用项在初始化时是否表示引用值。
	\since build 828
	*/
	DefPred(const ynothrow, TermReferenced, is_ref)

	explicit DefCvtMem(const ynothrow, TermNode&, term_ref)

	//! \since build 823
	DefGetter(const ynothrow, const shared_ptr<const void>&, AnchorPtr,
		p_anchor)

	PDefH(TermNode&, get, ) const ynothrow
		ImplRet(term_ref.get())
};

/*!
\brief 折叠项引用。
\return 引用值及初始化时是否表示引用值。
\note 可选提供环境关联锚对象指针。
\relates TermReference
\since build 829

返回引用由以下方式确定：
当参数的 Value 表示项引用时，返回值；否则为通过参数初始化的项引用。
这种方式避免初始化引用的引用。
*/
//@{
YF_API pair<TermReference, bool>
Collapse(TermNode&);
YF_API pair<TermReference, bool>
Collapse(TermNode&, const Environment&);
//@}

//! \since build 800
//@{
/*!
\brief 访问项并取解析 TermReference 间接值后的引用。
\return 若项的 Value 数据成员为 TermReference 则为其中的引用，否则为参数。
\sa TermReference
*/
YF_API TermNode&
ReferenceTerm(TermNode&);
YF_API const TermNode&
ReferenceTerm(const TermNode&);

/*!
\brief 解析并间接引用处理可能是引用值的项。
\note 假定项符合正规表示，不需要对间接值检查 IsLeaf 。
\since build 840
*/
//@{
template<typename _func>
auto
ResolveTerm(_func do_resolve, TermNode& term)
	-> decltype(do_resolve(term, bool()))
{
	// XXX: Assume value representation of %term is regular.
	if(const auto p = YSLib::AccessPtr<const TermReference>(term))
		return do_resolve(p->get(), true);
	return do_resolve(term, {});
}
template<typename _func>
auto
ResolveTerm(_func do_resolve, const TermNode& term)
	-> decltype(do_resolve(term, bool()))
{
	// XXX: Assume value representation of %term is regular.
	if(const auto p = YSLib::AccessPtr<const TermReference>(term))
		return do_resolve(p->get(), true);
	return do_resolve(term, {});
}
//@}

/*!
\brief 访问解析 TermReference 后的项的指定类型对象。
\exception ListTypeError 异常中立：项为列表项。
\exception std::bad_cast 异常中立：非列表项类型检查失败。
\sa ThrowListTypeErrorForInvalidType
*/
//@{
template<typename _type>
_type&
AccessTerm(TermNode& term)
{
	auto& tm(ReferenceTerm(term));

	if(!IsList(tm))
		return tm.Value.Access<_type>();
	ThrowListTypeErrorForInvalidType(ystdex::type_id<_type>(), term);
}
template<typename _type>
const _type&
AccessTerm(const TermNode& term)
{
	const auto& tm(ReferenceTerm(term));

	if(!IsList(tm))
		return tm.Value.Access<_type>();
	ThrowListTypeErrorForInvalidType(ystdex::type_id<_type>(), term);
}
//@}

//! \brief 访问解析 TermReference 后的项的指定类型对象指针。
//@{
template<typename _type>
inline observer_ptr<_type>
AccessTermPtr(TermNode& term) ynothrow
{
	return ReferenceTerm(term).Value.AccessPtr<_type>();
}
template<typename _type>
inline observer_ptr<const _type>
AccessTermPtr(const TermNode& term) ynothrow
{
	return ReferenceTerm(term).Value.AccessPtr<_type>();
}
//@}
//@}


//! \since build 801
//@{
//! \brief 引用项操作。
struct ReferenceTermOp
{
	template<typename _type>
	auto
	operator()(_type&& term) const -> decltype(ReferenceTerm(yforward(term)))
	{
		return ReferenceTerm(yforward(term));
	}
};

/*!
\brief 包装一个非引用项的操作为 NPL::ReferenceTermOp 以支持项引用。
\relates ReferenceTermOp
\since build 802
*/
template<typename _func>
auto
ComposeReferencedTermOp(_func f)
	-> yimpl(decltype(ystdex::compose_n(f, ReferenceTermOp())))
{
	return ystdex::compose_n(f, ReferenceTermOp());
}
//@}


/*!
\brief 检查视为范式的节点并提取规约状态。
\since build 769
*/
inline PDefH(ReductionStatus, CheckNorm, const TermNode& term) ynothrow
	ImplRet(IsBranch(term) ? ReductionStatus::Retained : ReductionStatus::Clean)

/*!
\brief 根据规约状态检查是否可继续规约。
\sa ReductionStatus::Partial
\sa ReductionStatus::Retrying
\sa YTraceDe
\since build 734

只根据输入状态确定结果。当且仅当规约成功时不视为继续规约。
若发现不支持的状态视为不成功，输出警告。
派生实现可使用类似的接口指定多个不同的状态。
*/
YF_API YB_PURE bool
CheckReducible(ReductionStatus);

/*!
\sa CheckReducible
\since build 841
*/
template<typename _func, typename... _tParams>
auto
CheckedReduceWith(_func f, _tParams&&... args)
	-> decltype(ystdex::retry_on_cond(CheckReducible, f, yforward(args)...))
{
	return ystdex::retry_on_cond(CheckReducible, f, yforward(args)...);
}

/*!
\brief 按规约结果正规化项。
\return 第二参数。
\since build 841
*/
YF_API ReductionStatus
RegularizeTerm(TermNode&, ReductionStatus) ynothrow;


/*!
\brief 提升项：使用第二个参数指定的项的内容替换第一个项的内容。
\since build 805
*/
//@{
//! \note 参数相同时作用为空，但可能有额外开销。
inline PDefH(void, LiftTerm, TermNode& term, TermNode& tm) ynothrow
	ImplExpr(term.MoveContent(std::move(tm)))
inline PDefH(void, LiftTerm, ValueObject& term_v, ValueObject& vo) ynothrow
	ImplExpr(term_v = std::move(vo))
inline PDefH(void, LiftTerm, TermNode& term, ValueObject& vo) ynothrow
	ImplExpr(LiftTerm(term.Value, vo))
//@}

/*!
\brief 提升项间接引用：复制或转移间接引用项使项不含间接值。
\since build 834
*/
inline PDefH(void, LiftTermIndirection, TermNode& term)
	// NOTE: See $2018-02 @ %Documentation::Workflow::Annual2018.
	ImplExpr(YSLib::SetContentWith(term, std::move(term),
		&ValueObject::MakeMoveCopy))

/*!
\warning 引入的间接值无所有权，应注意在生存期内使用以保证内存安全。
\since build 800
\todo 支持消亡值和复制。
*/
//@{
/*!
\note 用于支持实现对象语言中的左值到右值转换。
\sa LiftTerm
\sa LiftTermRef
\sa TermReference
*/
//@{
/*!
\brief 提升引用项。
\return 项为引用项。
\since build 828

项的 Value 数据成员为 TermReference 类型的值时调用 LiftTermRef 。
*/
YF_API bool
LiftTermOnRef(TermNode&, TermNode&);

/*!
\brief 提升自身引用项。
\sa LiftTermOnRef
\since build 828

作用同以相同参数调用 LiftTermOnRef 。
*/
inline PDefH(bool, LiftTermRefToSelf, TermNode& term)
	ImplRet(LiftTermOnRef(term, term))
//@}

/*!
\brief 提升项引用：使用第二个参数指定的项的内容引用替换第一个项的内容。
\sa ValueObject::MakeIndirect
\since build 747
*/
//@{
//! \since build 799
inline PDefH(void, LiftTermRef, TermNode& term, const TermNode& tm)
	ImplExpr(YSLib::SetContentWith(term, tm, &ValueObject::MakeIndirect))
inline PDefH(void, LiftTermRef, ValueObject& term_v, const ValueObject& vo)
	ImplExpr(term_v = vo.MakeIndirect())
inline PDefH(void, LiftTermRef, TermNode& term, const ValueObject& vo)
	ImplExpr(LiftTermRef(term.Value, vo))
//@}

/*!
\brief 提升项对象为引用。
\throw InvalidReference 被提升的值非引用。
\throw ystdex::invalid_construction 参数不持有值。
\sa LiftTerm
\since build 800
\todo 使用具体的语义错误异常类型。
*/
YF_API void
LiftToReference(TermNode&, TermNode&);

/*!
\brief 提升自身引用项后提升间接引用项以满足返回值的内存安全要求。
\sa LiftTermRefToSelf
\sa LiftTermIndirection
\since build 828
*/
YF_API void
LiftToReturn(TermNode&);

/*!
\brief 递归提升项及其子项或递归创建项和子项对应的包含间接值的引用项到自身。
\note 先提升项的值再提升子项以确保子项表示引用值时被提升。
\sa LiftTermRefToSelf
*/
YF_API void
LiftToSelf(TermNode&);

/*!
\brief 递归提升项及其子项或递归创建项和子项对应的包含间接值的间接引用项到自身。
\sa LiftToSelf
\sa LiftTermIndirection
\since build 821

调用 LiftToSelf ，然后递归地以相同参数调用 LiftTermIndirection 复制或转移自身。
*/
YF_API void
LiftToSelfSafe(TermNode&);

/*!
\brief 递归提升项及其子项或递归创建项和子项对应的包含间接值的引用项到其它项。
\sa LiftTerm
\sa LiftToSelf

以第二参数调用 LiftToSelf 后再调用 LiftTerm 。
*/
YF_API void
LiftToOther(TermNode&, TermNode&);
//@}

/*!
\brief 对每个子项调用 LiftToReturn 。
\since build 830
*/
inline PDefH(void, LiftSubtermsToReturn, TermNode& term)
	ImplExpr(std::for_each(term.begin(), term.end(), LiftToReturn))

/*!
\brief 对每个子项调用 LiftToSelfSafe 。
\since build 822
*/
inline PDefH(void, LiftSubtermsToSelfSafe, TermNode& term)
	ImplExpr(std::for_each(term.begin(), term.end(), LiftToSelfSafe))

/*!
\brief 提升延迟求值项的引用。
\since build 752
*/
inline PDefH(void, LiftDelayed, TermNode& term, DelayedTerm& tm)
	ImplExpr(LiftTermRef(term, tm))


//! \pre 断言：参数指定的项是枝节点。
//@{
/*!
\brief 提升首项：使用首个子项替换项的内容。
\since build 685
*/
inline PDefH(void, LiftFirst, TermNode& term)
	ImplExpr(AssertBranch(term), LiftTerm(term, Deref(term.begin())))

/*!
\brief 提升末项：使用最后一个子项替换项的内容。
\since build 696
*/
inline PDefH(void, LiftLast, TermNode& term)
	ImplExpr(AssertBranch(term), LiftTerm(term, Deref(term.rbegin())))
//@}


/*!
\pre 间接断言：参数为枝节点。
\return ReductionStatus::Retained 。
\since build 823
*/
//@{
//! \brief 规约为列表：对枝节点移除第一个子项，保留余下的子项作为列表。
YF_API ReductionStatus
ReduceBranchToList(TermNode&) ynothrowv;

/*!
\brief 规约为列表值：对枝节点移除第一个子项，保留余下的子项提升后作为列表的值。
\sa LiftSubtermsToReturn
*/
YF_API ReductionStatus
ReduceBranchToListValue(TermNode&) ynothrowv;
//@}

/*!
\since build 774
\sa RemoveHead
*/
//@{
/*!
\brief 规约第一个非结尾空列表子项。
\return ReductionStatus::Clean 。

若项具有不少于一个子项且第一个子项是空列表则移除。
允许空列表作为第一个子项以标记没有操作数的函数应用。
*/
YF_API ReductionStatus
ReduceHeadEmptyList(TermNode&) ynothrow;

//! \return 移除项时 ReductionStatus::Retained ，否则 ReductionStatus::Clean。
//@{
/*!
\brief 规约为列表：对枝节点移除第一个子项，保留余下的子项作为列表。
\sa ReduceBranchToList
*/
YF_API ReductionStatus
ReduceToList(TermNode&) ynothrow;

/*!
\brief 规约为列表值：对枝节点移除第一个子项，保留余下的子项提升后作为列表的值。
\sa ReduceBranchToListValue
\since build 821
*/
YF_API ReductionStatus
ReduceToListValue(TermNode&) ynothrow;
//@}
//@}


/*!
\brief 规约闭包结果处理：提升结果。
\return 根据规约后剩余项确定的规约结果。
\sa CheckNorm
\sa LiftToReturn
\sa RegularizeTerm
\since build 841

对规约闭包结果进行处理，依次进行以下操作：
调用 RegularizeTerm 根据第二参数指定的规约结果对项进行正规化；
调用 LiftToReturn 提升最外一级的引用项后递归提升间接值；
最后调用 CheckNorm 确定返回值。
*/
YF_API ReductionStatus
ReduceForClosureResult(TermNode&, ReductionStatus);


/*!
\since build 807
\note 一般第一参数用于指定被合并的之前的规约结果，第二参数指定用于合并的结果。
\return 合并后的规约结果。
*/
/*!
\brief 合并规约结果。

若第二参数指定保留项，则合并后的规约结果为第二参数；否则为第一参数。
*/
yconstfn PDefH(ReductionStatus, CombineReductionResult, ReductionStatus res,
	ReductionStatus r) ynothrow
	ImplRet(r == ReductionStatus::Retained ? r : res)

/*!
\brief 合并序列规约结果。
\sa CheckReducible

若第二参数指定可继续规约的项，则合并后的规约结果为第二参数；
	否则同 CombineReductionResult 。
若可继续规约，则指定当前规约的项的规约已终止，不合并第一参数指定的结果。
下一轮序列的规约可能使用或忽略合并后的结果。
*/
inline PDefH(ReductionStatus, CombineSequenceReductionResult,
	ReductionStatus res, ReductionStatus r) ynothrow
	ImplRet(CheckReducible(r) ? r : CombineReductionResult(res, r))


//! \since build 676
//@{
/*!
\brief 遍合并器：逐次调用序列中的遍直至成功。
\note 合并遍结果用于表示及早判断是否应继续规约，可在循环中实现再次规约一个项。
*/
struct PassesCombiner
{
	/*!
	\note 对遍调用异常中立。
	\since build 764
	*/
	template<typename _tIn>
	ReductionStatus
	operator()(_tIn first, _tIn last) const
	{
		auto res(ReductionStatus::Clean);

		return ystdex::fast_any_of(first, last, [&](ReductionStatus r) ynothrow{
			res = CombineReductionResult(res, r);
			// XXX: Currently %CheckReducible is not used.
			return r == ReductionStatus::Retrying;
		}) ? ReductionStatus::Retrying : res;
	}
};


/*!
\brief 环境列表。
\since build 798

指定环境对象引用的有序集合。
*/
using EnvironmentList = vector<ValueObject>;


/*!
\brief 环境。
\warning 非虚析构。
\warning 避免 shared_ptr 析构方式不兼容的初始化。
\since build 787
*/
class YF_API Environment : private ystdex::equality_comparable<Environment>,
	public enable_shared_from_this<Environment>
{
public:
	//! \since build 788
	using BindingMap = ValueNode;
	/*!
	\brief 名称解析结果。
	\since build 821

	名称解析的返回结果是环境中的绑定目标的对象指针和直接保存绑定目标的环境的引用。
	*/
	using NameResolution
		= pair<observer_ptr<ValueNode>, lref<const Environment>>;

private:
	/*!
	\brief 锚对象类型：提供被引用计数。
	\since build 821
	*/
	struct SharedAnchor final
	{
		shared_ptr<const void> Ptr{make_shared<uintptr_t>()};

		DefDeCtor(SharedAnchor)
		SharedAnchor(const SharedAnchor&) ynothrow
			: SharedAnchor()
		{}
		SharedAnchor(SharedAnchor&& a) ynothrow
			: Ptr(a.Ptr)
		{}

		PDefHOp(SharedAnchor&, =, const SharedAnchor& a)
			ImplRet(ystdex::copy_and_swap(*this, a))
		PDefHOp(SharedAnchor&, =, SharedAnchor&& a)
			ImplRet(ystdex::move_and_swap(*this, a))

		friend PDefH(void, swap, SharedAnchor& x, SharedAnchor& y) ynothrow
			ImplRet(swap(x.Ptr, y.Ptr))
	};

public:
	//! \since build 788
	mutable BindingMap Bindings{};
	/*!
	\brief 解析名称：处理保留名称并查找名称。
	\return 查找到的名称，或查找失败时的空值。
	\pre 实现断言：第二参数的数据指针非空。
	\exception NPLException 对实现异常中立的未指定派生类型的异常。
	\note 失败时若抛出异常，条件由实现定义。
	\sa LookupName
	\sa NameResolution
	\sa Parent
	\since build 821

	解析指定环境中的名称。
	被解析的环境可重定向：解析失败时，尝试提供能进一步解析名称的环境。
	重定向通常在局部解析失败时进行，也可对特定保留名称进行。
	重定向的目标通常是父环境。
	实现名称解析的一般步骤包括：
	局部解析：对被处理的上下文查找名称，若成功则返回；
	否则，若支持重定向，尝试重定向解析：在重定向后的环境中重新查找名称；
	否则，名称解析失败。
	名称解析失败时默认返回空值和未指定的环境引用值。对特定的失败，实现可约定抛出异常。
	保留名称的集合和是否支持重定向由实现约定。
	只有和名称解析的相关保留名称被处理。其它保留名称被忽略。
	不保证对循环重定向进行检查。
	*/
	std::function<NameResolution(string_view)> Resolve{
		std::bind(DefaultResolve, std::cref(*this), std::placeholders::_1)};
	/*!
	\brief 父环境：被解释的重定向目标。
	\sa DefaultResolve
	\since build 798
	*/
	ValueObject Parent{};

private:
	/*!
	\brief 锚对象指针：提供被引用计数。
	\since build 821
	*/
	SharedAnchor anchor{};

public:
	//! \brief 无参数构造：初始化空环境。
	DefDeCtor(Environment)
	/*!
	\brief 构造：使用包含绑定节点的指针。
	\note 不检查绑定的名称。
	*/
	//@{
	explicit
	Environment(const BindingMap&& m)
		: Bindings(m)
	{}
	explicit
	Environment(BindingMap&& m)
		: Bindings(std::move(m))
	{}
	//@}
	/*!
	\brief 构造：使用父环境。
	\exception NPLException 异常中立：由 CheckParent 抛出。
	\todo 使用专用的异常类型。
	*/
	//@{
	explicit
	Environment(const ValueObject& vo)
		: Parent((CheckParent(vo), vo))
	{}
	explicit
	Environment(ValueObject&& vo)
		: Parent((CheckParent(vo), std::move(vo)))
	{}
	//@}
	DefDeCopyMoveCtorAssignment(Environment)

	friend PDefHOp(bool, ==, const Environment& x, const Environment& y)
		ynothrow
		ImplRet(x.Bindings == y.Bindings)

	/*!
	\brief 判断锚对象未被外部引用。
	\since build 830
	*/
	DefPred(const ynothrow, Orphan, anchor.Ptr.use_count() == 1)

	/*!
	\brief 取名称绑定映射。
	\since build 788
	*/
	DefGetter(const ynothrow, BindingMap&, MapRef, Bindings)
	//! \since build 825
	DefGetter(const ynothrow, const shared_ptr<const void>&, AnchorPtr,
		anchor.Ptr)

	/*!
	\brief 引用锚对象。
	\since build 821
	*/
	PDefH(shared_ptr<const void>, Anchor, ) const
		ImplRet(anchor.Ptr)

	//! \since build 798
	//@{
	/*!
	\brief 检查可作为父环境的宿主对象。
	\note 若存在父环境，首先对父环境递归检查。
	\exception NPLException 异常中立：由 ThrowForInvalidType 抛出。
	\todo 使用专用的异常类型。
	*/
	static void
	CheckParent(const ValueObject&);

	/*!
	\brief 移除第一参数中名称和第二参数中重复的绑定项。
	\return 移除后的目的结果中没有绑定。
	\since build 825
	*/
	static bool
	Deduplicate(BindingMap&, const BindingMap&);

	/*!
	\brief 解析名称：处理保留名称并查找名称。
	\pre 断言：第二参数的数据指针非空。
	\exception NPLException 访问共享重定向环境失败。
	\sa Lookup
	\sa Parent
	\sa Resolve
	\since build 821

	按默认环境解析规则解析名称。
	局部解析失败时，重定向解析 Parent 储存的对象作为父环境的引用值。
	不在其它条件下重定向。不重定向到其它目标。
	重定向的候选目标是到有限个不同的环境。
	支持的重定向项的宿主值的类型包括：
	EnvironmentList ：环境列表；
	observer_ptr<const Environment> 无所有权的重定向环境；
	EnvironmentReference 可能具有共享所有权的重定向环境；
	shared_ptr<Environment> 具有共享所有权的重定向环境。
	若重定向可能具有共享所有权的失败，则表示资源访问错误，如构成循环引用；
		可能涉及未定义行为。
	以上支持的宿主值类型被依次检查。若检查成功，则使用此宿主值类型访问环境。
	对列表，使用 DFS （深度优先搜索）依次递归检查其元素。
	*/
	static NameResolution
	DefaultResolve(const Environment&, string_view);
	//@}

	/*!
	\pre 字符串参数的数据指针非空。
	\throw BadIdentifier 非强制调用时发现标识符不存在或冲突。
	\note 最后一个参数表示强制调用。
	\warning 应避免对被替换或移除的值的悬空引用。
	\since build 787
	*/
	//@{
	//! \brief 以字符串为标识符在指定上下文中定义值。
	void
	Define(string_view, ValueObject&&, bool);

	/*!
	\brief 查找名称。
	\pre 断言：第二参数的数据指针非空。
	\return 查找到的名称，或查找失败时的空值。
	\since build 798

	在环境中查找名称。
	*/
	observer_ptr<ValueNode>
	LookupName(string_view) const;

	//! \pre 间接断言：第一参数的数据指针非空。
	//@{
	//! \brief 以字符串为标识符在指定上下文中覆盖定义值。
	void
	Redefine(string_view, ValueObject&&, bool);

	/*
	\brief 以字符串为标识符在指定上下文移除对象。
	\return 是否成功移除。
	*/
	bool
	Remove(string_view, bool);
	//@}
	//@}

	/*!
	\brief 对不符合环境要求的类型抛出异常。
	\throw TypeError 环境类型检查失败。
	\since build 799
	*/
	YB_NORETURN static void
	ThrowForInvalidType(const ystdex::type_info&);
};


/*!
\brief 环境引用。
\since build 823

可能共享所有权环境的引用。
*/
class YF_API EnvironmentReference
	: private ystdex::equality_comparable<EnvironmentReference>
{
private:
	weak_ptr<Environment> p_weak;
	//! \brief 引用的锚对象指针。
	shared_ptr<const void> p_anchor;

public:
	//! \brief 构造：使用指定的环境指针和此环境的锚对象指针。
	EnvironmentReference(const shared_ptr<Environment>&);
	//! \brief 构造：使用指定的环境指针和锚对象指针。
	//@{
	template<typename _tParam1, typename _tParam2>
	EnvironmentReference(_tParam1&& arg1, _tParam2&& arg2)
		: p_weak(yforward(arg1)), p_anchor(yforward(arg2))
	{}
	//@}
	DefDeCopyMoveCtorAssignment(EnvironmentReference)

	DefGetter(const ynothrow, const weak_ptr<Environment>&, Ptr, p_weak)
	DefGetter(const ynothrow, const shared_ptr<const void>&, AnchorPtr,
		p_anchor)

	PDefH(shared_ptr<Environment>, Lock, ) const ynothrow
		ImplRet(p_weak.lock())

	//! \since build 824
	friend PDefHOp(bool, ==, const EnvironmentReference& x,
		const EnvironmentReference& y) ynothrow
		ImplRet(x.p_weak.lock() == y.p_weak.lock())
};


/*!
\brief 规约函数类型：和绑定所有参数的求值遍的处理器等价。
\warning 假定转移不抛出异常。
\warning 非虚析构。
\since build 841
*/
class YF_API Reducer : private YSLib::GHEvent<ReductionStatus()>
{
public:
	using BaseType = YSLib::GHEvent<ReductionStatus()>;

	DefDeCtor(Reducer)
	using BaseType::BaseType;
	DefDeCopyMoveCtorAssignment(Reducer)

	DefGetter(const ynothrow, const BaseType&, Base, *this)
	DefGetter(ynothrow, BaseType&, BaseRef, *this)

	using BaseType::operator bool;

	friend PDefHOp(bool, ==, const Reducer& x, const Reducer& y) ynothrow
		ImplRet(ystdex::ref_eq<>()(x, y))

	using BaseType::operator();

	friend DefSwap(ynothrow, Reducer,
		ystdex::swap_dependent(_x.GetBaseRef(), _y.GetBaseRef()))

	using BaseType::target;

	using BaseType::target_type;
};


/*!
\brief 上下文节点。
\since build 782
*/
class YF_API ContextNode
{
private:
	/*!
	\brief 环境记录指针。
	\invariant p_record 。
	\since build 788
	*/
	shared_ptr<Environment> p_record{make_shared<Environment>()};

public:
	/*!
	\brief 当前动作。
	\note 为便于确保资源释放和异常安全，不使用 ystdex::one_shot 。
	\sa Switch
	\since build 806
	*/
	Reducer Current{};
	/*!
	\brief 定界动作：边界外的剩余动作。
	\since build 810
	*/
	YSLib::deque<Reducer> Delimited{};
	/*!
	\brief 最后一次规约状态。
	\sa ApplyTail
	\since build 813
	*/
	ReductionStatus LastStatus = ReductionStatus::Clean;
	/*!
	\brief 上下文日志追踪。
	\since build 803
	*/
	YSLib::Logger Trace{};

	DefDeCtor(ContextNode)
	/*!
	\throw std::invalid_argument 参数指针为空。
	\note 遍和日志追踪对象被复制。
	\since build 788
	*/
	ContextNode(const ContextNode&, shared_ptr<Environment>&&);
	DefDeCopyCtor(ContextNode)
	/*!
	\brief 转移构造。
	\post <tt>p_record->Bindings.empty()</tt> 。
	\since build 811
	*/
	ContextNode(ContextNode&&) ynothrow;
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 842
	*/
	virtual
	~ContextNode();

	DefDeCopyMoveAssignment(ContextNode)

	//! \since build 788
	DefGetter(const ynothrow, Environment::BindingMap&, BindingsRef,
		GetRecordRef().GetMapRef())
	//! \since build 788
	DefGetter(const ynothrow, Environment&, RecordRef, *p_record)

	/*!
	\brief 转移并应用尾调用。
	\note 调用前切换 Current 以允许调用 SetupTail 设置新的尾调用。
	\pre 断言： \c Current 。
	\sa LastStatus
	\since build 810
	*/
	ReductionStatus
	ApplyTail();

	/*!
	\sa Delimited
	\sa SetupTail
	\sa Current
	\since build 810
	*/
	//@{
	/*!
	\brief 转移首个定界动作为当前动作。
	\pre 断言： \c !Delimited.empty() 。
	\pre 间接断言： \c !Current 。
	\post \c Current 。
	\sa Delimited
	\sa SetupTail
	\since build 810
	*/
	void
	Pop() ynothrow;

	/*!
	\brief 转移当前动作为首个定界动作。
	\pre 断言： \c Current 。
	*/
	//@{
	//! \since build 812
	//@{
	void
	Push(const Reducer&);
	void
	Push(Reducer&&);
	//@}
	//! \pre \c !Current 。
	PDefH(void, Push, )
		ImplExpr(Push(Reducer()))
	//@}
	//@}

	//! \exception std::bad_function_call Reducer 参数为空。
	//@{
	/*!
	\brief 重写项。
	\pre 间接断言：\c !Current 。
	\post \c Current 。
	\sa ApplyTail
	\sa SetupTail
	\sa Transit
	\note 不处理重规约。
	\since build 810

	调用 Transit 转变状态，当可规约时调用 ApplyTail 迭代规约重写。
	因为递归重写平摊到单一的循环， CheckReducible 不用于判断是否需要继续重写循环。
	每次调用 Current 的结果同步到 TailResult 。
	返回值为最后一次 Current 调用结果。
	*/
	ReductionStatus
	Rewrite(Reducer);
	//@}

	/*!
	\brief 设置当前动作以重规约。
	\pre 断言：\c !Current 。
	\pre 动作转移无异常抛出。
	\since build 809
	*/
	template<typename _func>
	void
	SetupTail(_func&& f)
	{
#if false
		// XXX: Not applicable for functor with %Reducer is captured.
		using func_t = ystdex::decay_t<_func> ;
		static_assert(ystdex::or_<std::is_same<func_t, Reducer>,
			ystdex::is_nothrow_move_constructible<func_t>>(),
			"Invalid type found.");
#endif

		YAssert(!Current, "Old continuation is overriden.");
		Current = yforward(f);
	}

	/*!
	\brief 切换当前动作。
	\since build 807
	*/
	PDefH(Reducer, Switch, Reducer f = {})
		ImplRet(ystdex::exchange(Current, std::move(f)))

	/*!
	\brief 切换环境。
	\since build 815
	*/
	//@{
	/*!
	\throw std::invalid_argument 参数指针为空。
	\sa SwitchEnvironmentUnchecked
	*/
	shared_ptr<Environment>
	SwitchEnvironment(shared_ptr<Environment>);

	//! \pre 断言：参数指针非空。
	shared_ptr<Environment>
	SwitchEnvironmentUnchecked(shared_ptr<Environment>) ynothrowv;
	//@}

	/*!
	\brief 按需转移首个定界动作为当前动作。
	\pre 间接断言： \c !Current 。
	\return 是否可继续规约。
	\sa Pop
	\since build 811
	*/
	bool
	Transit() ynothrow;

	//! \since build 788
	//@{
	PDefH(shared_ptr<Environment>, ShareRecord, ) const
		ImplRet(GetRecordRef().shared_from_this())

	//! \since build 823
	PDefH(EnvironmentReference, WeakenRecord, ) const
		// TODO: Use C++17 %weak_from_this to get more efficient implementation.
		ImplRet(ShareRecord())

	friend PDefH(void, swap, ContextNode& x, ContextNode& y) ynothrow
		ImplExpr(swap(x.p_record, y.p_record), swap(x.Trace, y.Trace))
	//@}
};

/*!
\brief 切换到参数指定的新创建的环境。
\relates ContextNode
\since build 838
*/
template<typename... _tParams>
inline shared_ptr<Environment>
SwitchToFreshEnvironment(ContextNode& ctx, _tParams&&... args) ynothrow
{
	return ctx.SwitchEnvironmentUnchecked(
		make_shared<Environment>(yforward(args)...));
}


/*!
\brief 构造并向绑定目标添加叶节点值。
\return 对应的值在构造前不存在。
\pre 断言：第二参数的数据指针非空。
\since build 838
*/
//@{
template<typename _type, typename... _tParams>
inline bool
EmplaceLeaf(Environment::BindingMap& m, string_view name, _tParams&&... args)
{
	YAssertNonnull(name.data());
	return m.insert_or_assign(name, TermNode(YSLib::NoContainer, name,
		ystdex::in_place_type<_type>, yforward(args)...)).second;
	// NOTE: The following code is incorrect because the subterms are not
	//	cleared, as well as lacking of %bool return value of insertion result.
//	m[name].Value.emplace<_type>(yforward(args)...);
}
template<typename _type, typename... _tParams>
inline bool
EmplaceLeaf(Environment& env, string_view name, _tParams&&... args)
{
	return NPL::EmplaceLeaf<_type>(env.Bindings, name, yforward(args)...);
}
template<typename _type, typename... _tParams>
inline bool
EmplaceLeaf(ContextNode& ctx, string_view name, _tParams&&... args)
{
	return NPL::EmplaceLeaf<_type>(ctx.GetRecordRef(), name, yforward(args)...);
}
//@}

//! \since build 788
//@{
//! \brief 从指定环境查找名称对应的节点。
//@{
template<typename _tKey>
inline observer_ptr<ValueNode>
LookupName(Environment& ctx, const _tKey& id) ynothrow
{
	return YSLib::AccessNodePtr(ctx.GetMapRef(), id);
}
template<typename _tKey>
inline observer_ptr<const ValueNode>
LookupName(const Environment& ctx, const _tKey& id) ynothrow
{
	return YSLib::AccessNodePtr(ctx.GetMapRef(), id);
}
//@}

//! \brief 从指定环境取指定名称指称的值。
template<typename _tKey>
ValueObject
FetchValue(const Environment& env, const _tKey& name)
{
	return GetValueOf(NPL::LookupName(env, name));
}

//! \brief 从指定上下文取指定名称指称的值的指针。
template<typename _tKey>
observer_ptr<const ValueObject>
FetchValuePtr(const Environment& env, const _tKey& name)
{
	return GetValuePtrOf(NPL::LookupName(env, name));
}
//@}

//! \exception NPLException 访问共享重定向上下文失败。
//@{
/*!
\brief 解析名称：处理保留名称并查找名称。
\pre 断言：第二参数的数据指针非空。
\sa Environment::ResolveName
\since build 821

解析指定上下文中的名称。
*/
YF_API Environment::NameResolution
ResolveName(const ContextNode&, string_view);

/*!
\brief 解析标识符：解析名称并折叠引用。
\pre 间接断言：第二参数的数据指针非空。
\return 标识符指称的实体的引用及初始化时是否表示引用值。
\throw BadIdentifier 标识符未在环境中找到。
\sa Collapse
\sa ResolveName
\since build 829

解析指定上下文中的标识符，若不存在绑定则抛出异常。
*/
YF_API pair<TermReference, bool>
ResolveIdentifier(const ContextNode&, string_view);
//@}

/*!
\brief 解析环境。
\return 取得所有权的环境指针及是否具有所有权。
\note 只支持宿主值类型 \c shared_ptr<Environment> 或 \c weak_ptr<Environment> 。
*/
//@{
//! \since build 830
YF_API pair<shared_ptr<Environment>, bool>
ResolveEnvironment(const ValueObject&);
/*!
\throw ListTypeError 参数是列表节点。
\note 当前使用和 TermToStringWithReferenceMark 相同的方式在异常消息中表示引用项。
\sa TermToStringWithReferenceMark
\since build 840
*/
YF_API pair<shared_ptr<Environment>, bool>
ResolveEnvironment(const TermNode& term);
//@}


/*!
\brief 环境切换器。
\warning 非虚析构。
\since build 821
*/
struct EnvironmentSwitcher
{
	//! \since build 842
	lref<ContextNode> Context;
	mutable shared_ptr<Environment> SavedPtr;

	EnvironmentSwitcher(ContextNode& ctx,
		shared_ptr<Environment>&& p_saved = {})
		: Context(ctx), SavedPtr(std::move(p_saved))
	{}
	DefDeMoveCtor(EnvironmentSwitcher)

	DefDeMoveAssignment(EnvironmentSwitcher)

	void
	operator()() const ynothrowv
	{
		if(SavedPtr)
			Context.get().SwitchEnvironmentUnchecked(std::move(SavedPtr));
	}
};


/*!
\brief 组合动作。
\note 不直接使用 lambda 表达式，以避免对捕获的动作的未指定的析构顺序。
\note 相对 pair 值作为捕获的 lambda 表达式预期有更好的翻译性能。
\since build 841
*/
template<typename _fCurrent, typename _fNext>
struct GComposedAction final
{
	// NOTE: Lambda is not used to avoid unspecified destruction order of
	//	captured component and better performance (compared to the case of
	//	%pair used to keep the order).
	lref<ContextNode> Context;
	// NOTE: The destruction order of captured component is significant.
	//! \since build 821
	//@{
	mutable _fNext Next;
	// XXX: To support function objects like %std::bind result with mutable
	//	bound parameters.
	mutable _fCurrent Current;

	template<typename _tParam1, typename _tParam2>
	GComposedAction(ContextNode& ctx, _tParam1&& cur, _tParam2&& next)
		: Context(ctx), Next(yforward(next)), Current(yforward(cur))
	{}
	//@}
	// XXX: Copy is not intended used directly, but for well-formness.
	DefDeCopyMoveCtor(GComposedAction)

	DefDeMoveAssignment(GComposedAction)

	ReductionStatus
	operator()() const
	{
		RelaySwitched(Context, std::move(Next));
		return Current();
	}
};


/*!
\note 参数分别为上下文、捕获的当前动作和捕获的后继动作。
\note 以参数声明的相反顺序捕获参数作为动作，结果以参数声明的顺序析构捕获的动作。
*/
//@{
/*!
\brief 组合规约动作：创建指定上下文中的连续异步规约当前和后继动作的规约动作。
\note 若当前动作为空，则直接使用后继动作作为结果。
\sa GComposedAction
\since build 841
*/
//@{
template<typename _fCurrent, typename _fNext>
inline yimpl(ystdex::enable_if_t)<ystdex::is_decayed<_fCurrent, Reducer>::value,
	Reducer>
ComposeActions(ContextNode& ctx, _fCurrent&& cur, _fNext&& next)
{
	return cur ? GComposedAction<ystdex::remove_cvref_t<_fCurrent>,
		ystdex::remove_cvref_t<_fNext>>(ctx, yforward(cur), yforward(next))
		: std::move(next);
}
template<typename _fCurrent, typename _fNext, yimpl(typename
	= ystdex::enable_if_t<!ystdex::is_decayed<_fCurrent, Reducer>::value>)>
inline Reducer
ComposeActions(ContextNode& ctx, _fCurrent&& cur, _fNext&& next)
{
	return GComposedAction<ystdex::remove_cvref_t<_fCurrent>,
		ystdex::remove_cvref_t<_fNext>>(ctx, yforward(cur), yforward(next));
}
//@}

/*!
\return ReductionStatus::Partial 。
\since build 841
*/
//@{
/*!
\brief 异步规约当前和后继动作。
\sa ComposeActions
*/
template<typename _fCurrent, typename _fNext>
ReductionStatus
RelayNext(ContextNode& ctx, _fCurrent&& cur, _fNext&& next)
{
	ctx.SetupTail(ComposeActions(ctx, yforward(cur), yforward(next)));
	return ReductionStatus::Partial;
}

/*!
\brief 异步规约指定动作和非空的当前动作。
\pre 断言： \tt ctx.Current 。
*/
template<typename _fCurrent>
inline ReductionStatus
RelaySwitchedUnchecked(ContextNode& ctx, _fCurrent&& cur)
{
	YAssert(ctx.Current, "No action found to be the next action.");
	return RelayNext(ctx, yforward(cur), ctx.Switch());
}

/*!
\brief 异步规约指定动作和当前动作。
\sa RelaySwitchedUnchecked
*/
template<typename _fCurrent>
inline ReductionStatus
RelaySwitched(ContextNode& ctx, _fCurrent&& cur)
{
	if(ctx.Current)
		return RelaySwitchedUnchecked(ctx, yforward(cur));
	ctx.SetupTail(yforward(cur));
	return ReductionStatus::Partial;
}
//@}
//@}

/*!
\brief 转移动作到当前动作及定界动作。
\since build 812
*/
inline PDefH(void, MoveAction, ContextNode& ctx, Reducer&& act)
	ImplExpr(!ctx.Current ? ctx.SetupTail(std::move(act))
		: ctx.Push(std::move(act)))

} // namespace NPL;

#endif

