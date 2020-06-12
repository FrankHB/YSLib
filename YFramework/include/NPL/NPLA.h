/*
	© 2014-2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA.h
\ingroup NPL
\brief NPLA 公共接口。
\version r7624
\author FrankHB <frankhb1989@gmail.com>
\since build 663
\par 创建时间:
	2016-01-07 10:32:34 +0800
\par 修改时间:
	2020-06-12 21:29 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA
*/


#ifndef NPL_INC_NPLA_h_
#define NPL_INC_NPLA_h_ 1

#include "YModules.h"
#include YFM_NPL_SContext // for YSLib::any_ops, YSLib::NodeLiteral, YSLib::any,
//	YSLib::bad_any_cast, YSLib::enable_shared_from_this, YSLib::in_place_type,
//	YSLib::lref, YSLib::to_string, NPLTag, string, ValueNode, function,
//	NPL::GetNodeNameOf, YSLib::IsPrefixedIndex, TraverseSubnodes, TermNode,
//	YSLib::MakeIndex, std::initializer_list, LoggedEvent, shared_ptr,
//	NPL::Deref, ystdex::isdigit, ystdex::is_nothrow_copy_constructible,
//	ystdex::is_nothrow_copy_assignable, ystdex::is_nothrow_move_constructible,
//	ystdex::is_nothrow_move_assignable, observer_ptr, ystdex::type_id,
//	std::addressof, NPL::make_observer, ystdex::equality_comparable, weak_ptr,
//	ystdex::get_equal_to, pair, ystdex::expand_proxy, NPL::Access,
//	ystdex::ref_eq, ValueObject, NPL::SetContentWith, std::for_each,
//	AccessFirstSubterm, ystdex::less, YSLib::map, pmr, ystdex::copy_and_swap,
//	NoContainer, ystdex::try_emplace, ystdex::try_emplace_hint,
//	ystdex::insert_or_assign, ystdex::type_info, ystdex::expanded_function,
//	YSLib::forward_list, YSLib::allocate_shared, ystdex::enable_if_same_param_t,
//	ystdex::exclude_self_t, ystdex::exchange, ystdex::make_obj_using_allocator;
#include <ystdex/base.h> // for ystdex::derived_entity;

namespace NPL
{

/*!	\defgroup ThunkType Thunk Types
\brief 中间值类型。
\since build 753

标记特定求值策略，储存于 TermNode 的 Value 数据成员中不直接表示宿主语言对象的类型。
*/

//! \since build 851
namespace any_ops = YSLib::any_ops;

//! \since build 600
using YSLib::NodeLiteral;
//! \since build 851
using YSLib::any;
//! \since build 851
using YSLib::bad_any_cast;
//! \since build 788
//@{
using YSLib::enable_shared_from_this;
//! \since build 851
using YSLib::in_place_type;
//! \since build 598
using YSLib::to_string;
//@}


/*!
\brief NPLA 元标签。
\note NPLA 是 NPL 的抽象实现。
\since build 597
*/
struct YF_API NPLATag : NPLTag
{};


/*!
\brief 解码索引节点名称。
\return 解码后的名称。
\since build 852
\sa YSLib::DecodeIndex
\sa std::to_string

若存在解码成功的索引，则结果为其十进制表示；否则，结果同未解码的节点名称。
*/
YB_ATTR_nodiscard YF_API YB_PURE string
DecodeNodeIndex(const string&);


//! \since build 597
using NodeToString = function<string(const ValueNode&)>;

/*!
\brief 转义 NPLA 节点字面量。
\return 调用 EscapeLiteral 转义访问字符串的结果。
\exception bad_any_cast 异常中立：由 NPL::Access 抛出。
\since build 597
*/
YB_ATTR_nodiscard YF_API YB_PURE string
EscapeNodeLiteral(const ValueNode&);

/*!
\brief 转义 NPLA 节点字面量。
\return 参数为控节点则空串，否则调用 Literalize 字面量化 EscapeNodeLiteral 的结果。
\exception bad_any_cast 异常中立：由 EscapeNodeLiteral 抛出。
\sa EscapeNodeLiteral
\since build 598
*/
YB_ATTR_nodiscard YF_API YB_PURE string
LiteralizeEscapeNodeLiteral(const ValueNode&);

/*!
\brief 解析 NPLA 节点字符串。
\since build 508

以 string 类型访问节点，若失败则结果为空串。
*/
YB_ATTR_nodiscard YF_API YB_PURE string
ParseNPLANodeString(const ValueNode&);


//! \since build 597
//@{
//! \brief 生成前缀缩进的函数类型。
using IndentGenerator = function<string(size_t)>;

//! \brief 生成水平制表符为单位的缩进。
YB_ATTR_nodiscard YF_API YB_PURE string
DefaultGenerateIndent(size_t);

//! \exception std::bad_function 异常中立：参数指定的处理器为空。
//@{
/*!
\brief 打印缩进。
\note 若最后一个参数等于零则无副作用。
*/
YF_API void
PrintIndent(std::ostream&, IndentGenerator = DefaultGenerateIndent, size_t = 1);

/*!
\brief 打印容器边界和其中的 NPLA 节点，且在打印边界前调用前置操作。
\since build 851
*/
template<typename _fCallable>
void
PrintContainedNodes(std::ostream& os, function<void()> pre, _fCallable f)
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
\sa TraverseSubnodes
\sa YSLib::IsPrefixedIndex
\since build 851

以第三参数作为边界前置操作，调用 PrintContainedNodes 逐个打印子节点内容。
调用第四参数输出最后一个参数决定的缩进作为前缀，然后打印子节点内容。
对满足 IsPrefixedIndex 的节点调用第四参数作为节点字符串打印；
否则，调用第五参数递归打印子节点，忽略此过程中的 std::out_of_range 异常。
其中，遍历子节点通过调用 TraverseSubnodes 实现。
*/
template<class _tNode, typename _fCallable, typename _fCallable2>
void
TraverseNodeChildAndPrint(std::ostream& os, const _tNode& node,
	function<void()> pre, _fCallable print_node_str,
	_fCallable2 print_term_node)
{
	TraverseSubnodes([&](const _tNode& nd){
		if(YSLib::IsPrefixedIndex(NPL::GetNodeNameOf(nd)))
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

/*!
\brief 打印 NPLA 节点。
\sa DecodeNodeIndex
\sa PrintIdent
\sa PrintNodeChild
\sa PrintNodeString
\sa TraverseNodeChildAndPrint

调用第四参数输出最后一个参数决定的缩进作为前缀和一个空格，然后打印节点内容：
先尝试调用 PrintNodeString 打印节点字符串，若成功直接返回；
否则打印换行，对非空节点调用 TraverseNodeChildAndPrint 打印子节点内容。
其中，使用的边界前置操作为调用第四参数输出最后一个参数决定的缩进作为前缀输出。
打印节点内容中的节点名称时，首先尝试使用 YSLib::DecodeIndex 解码索引。
输出的节点名称使用 DecodeNodeIndex 解码。
*/
YF_API void
PrintNode(std::ostream&, const ValueNode&, NodeToString = EscapeNodeLiteral,
	IndentGenerator = DefaultGenerateIndent, size_t = 0);

/*!
\brief 打印节点字符串。
\return 是否成功访问节点字符串并输出。
\note bad_any_cast 外异常中立。
\sa PrintNode

使用最后一个参数指定的访问节点，打印得到的字符串和换行符。
忽略 bad_any_cast 。
*/
YF_API bool
PrintNodeString(std::ostream&, const ValueNode&,
	NodeToString = EscapeNodeLiteral);
//@}


using NodeInserter = function<void(ValueNode&&, ValueNode::Container&)>;
//@}


//! \since build 852
using TermNodeToString = function<string(const TermNode&)>;

/*!
\brief 解析 NPLA 项节点字符串。
\since build 852

以 string 类型访问节点，若失败则结果为空串。
*/
YB_ATTR_nodiscard YF_API YB_PURE string
ParseNPLATermString(const TermNode&);


/*!
\brief 节点映射操作类型：变换项节点为值节点。
\since buld 501
*/
using NodeMapper = function<ValueNode(const TermNode&)>;

/*!
\brief 映射 NPLA 叶节点。
\return 创建的新节点。
\sa ParseNPLANodeString
\since build 674

创建新节点。若参数为空则返回值为空串的新节点；否则值以 ParseNPLANodeString 取得。
*/
YB_ATTR_nodiscard YF_API YB_PURE ValueNode
MapNPLALeafNode(const TermNode&);

/*!
\brief 变换 NPLA 语法节点为语法分析树的叶节点。
\return 变换得到的节点。
\note 可选参数指定结果名称。
\since build 853
\sa Literalize
\sa ParseNPLANodeString

输入是 NPLA 语法节点可能和其它节点的混合，可能是未完全解析的中间数据。
变换输入为符合 NPLA 语法分析结果表示的项节点。
插入的子节点以 YSLib::NodeSequence 的形式作为变换节点的值而不是子节点，可保持顺序。
对其它叶节点，调用 ParseNPLANodeString 和 Literalize 解析和处理内容，
	作为待插入的子节点。
*/
//@{
YB_ATTR_nodiscard YF_API YB_PURE TermNode
TransformToSyntaxNode(ValueNode&&);
YB_ATTR_nodiscard YB_PURE inline
	PDefH(TermNode, TransformToSyntaxNode, NodeLiteral&& nl)
	ImplRet(TransformToSyntaxNode(std::move(nl.GetNodeRef())))
//@}


//! \since build 852
//@{
/*!
\note 在指定的节点插入节点，可用于语法分析树的输入。
\sa MakeIndex
\sa TransformToSyntaxNode
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
//@}

//! \since build 597
namespace SXML
{

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
	NPLException(string_view sv, YSLib::RecordLevel lv = YSLib::Err)
		: LoggedEvent(sv, lv)
	{}
	//! \since build 844
	DefDeCopyCtor(NPLException)
	//! \brief 虚析构：类定义外默认实现。
	~NPLException() override;

	//! \since build 844
	DefDeCopyAssignment(NPLException)
};


/*!
\since build 834
\todo 捕获并保存类型信息。
*/
//@{
/*!
\brief 类型错误。

类型不匹配的错误。
*/
class YF_API TypeError : public NPLException
{
public:
	//! \since build 692
	using NPLException::NPLException;
	//! \since build 845
	DefDeCopyCtor(TypeError)
	//! \brief 虚析构：类定义外默认实现。
	~TypeError() override;

	//! \since build 845
	DefDeCopyAssignment(TypeError)
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
	//! \since build 845
	DefDeCopyCtor(ValueCategoryMismatch)
	//! \brief 虚析构：类定义外默认实现。
	~ValueCategoryMismatch() override;

	//! \since build 845
	DefDeCopyAssignment(ValueCategoryMismatch)
};


/*!
\brief 列表类型错误。
\note 预期列表或列表引用。

列表类型或非列表类型不匹配的错误。
*/
class YF_API ListTypeError : public TypeError
{
public:
	//! \since build 834
	using TypeError::TypeError;
	//! \since build 845
	DefDeCopyCtor(ListTypeError)
	//! \brief 虚析构：类定义外默认实现。
	~ListTypeError() override;

	//! \since build 845
	DefDeCopyAssignment(ListTypeError)
};
//@}


/*!
\brief 列表规约失败。
\note 预期 ContextHandler 或引用 ContextHandler 的引用值。
\since build 692
\todo 捕获并保存上下文信息。
*/
class YF_API ListReductionFailure : public TypeError
{
public:
	//! \since build 834
	using TypeError::TypeError;
	//! \since build 845
	DefDeCopyCtor(ListReductionFailure)
	//! \brief 虚析构：类定义外默认实现。
	~ListReductionFailure() override;

	//! \since build 845
	DefDeCopyAssignment(ListReductionFailure)
};


/*!
\brief 无效语法。

特定上下文中的语法错误。
*/
class YF_API InvalidSyntax : public NPLException
{
public:
	using NPLException::NPLException;
	//! \since build 845
	DefDeCopyCtor(InvalidSyntax)
	//! \brief 虚析构：类定义外默认实现。
	~InvalidSyntax() override;

	//! \since build 845
	DefDeCopyAssignment(InvalidSyntax)
};


/*!
\brief 参数不匹配。
\since build 771
*/
class YF_API ParameterMismatch : public InvalidSyntax
{
public:
	using InvalidSyntax::InvalidSyntax;
	//! \since build 845
	DefDeCopyCtor(ParameterMismatch)
	//! \brief 虚析构：类定义外默认实现。
	~ParameterMismatch() override;

	//! \since build 845
	DefDeCopyAssignment(ParameterMismatch)
};


/*!
\brief 元数不匹配错误。
\todo 支持范围匹配。

操作数子项个数不匹配的错误。
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
	//! \since build 845
	DefDeCopyCtor(ArityMismatch)
	//! \brief 虚析构：类定义外默认实现。
	~ArityMismatch() override;

	//! \since build 845
	DefDeCopyAssignment(ArityMismatch)

	DefGetter(const ynothrow, size_t, Expected, expected)
	DefGetter(const ynothrow, size_t, Received, received)
};


/*!
\brief 标识符错误。
\since build 726

因在当前上下文中无效或不存在的标识符而引起的错误。
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
	//! \since build 845
	DefDeCopyCtor(BadIdentifier)
	//! \brief 虚析构：类定义外默认实现。
	~BadIdentifier() override;

	//! \since build 845
	DefDeCopyAssignment(BadIdentifier)

	DefGetter(const ynothrow, const string&, Identifier,
		NPL::Deref(p_identifier))
};


/*!
\brief 无效引用。
\since build 822

无效引用错误。
*/
class YF_API InvalidReference : public NPLException
{
public:
	using NPLException::NPLException;
	//! \since build 845
	DefDeCopyCtor(InvalidReference)
	//! \brief 虚析构：类定义外默认实现。
	~InvalidReference() override;

	//! \since build 845
	DefDeCopyAssignment(InvalidReference)
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
YB_ATTR_nodiscard YF_API YB_PURE LexemeCategory
CategorizeBasicLexeme(string_view) ynothrowv;

/*!
\brief 对词素分类。
\sa CategorizeBasicLexeme
*/
YB_ATTR_nodiscard YF_API YB_PURE LexemeCategory
CategorizeLexeme(string_view) ynothrowv;
//@}

/*!
\brief 判断不是非扩展字面量的词素是否为 NPLA 扩展字面量。
\pre 断言：字符串参数的数据指针非空且字符串非空。
\pre 词素不是代码字面量或数据字面量。
\since build 771
*/
YB_ATTR_nodiscard YF_API YB_PURE bool
IsNPLAExtendedLiteral(string_view) ynothrowv;

/*!
\brief 判断字符是否为 NPLA 扩展字面量非数字前缀。
\since build 771
*/
YB_ATTR_nodiscard YB_PURE yconstfn
	PDefH(bool, IsNPLAExtendedLiteralNonDigitPrefix, char c) ynothrow
	ImplRet(c == '#'|| c == '+' || c == '-')

//! \brief 判断字符是否为 NPLA 扩展字面量前缀。
YB_ATTR_nodiscard YB_PURE inline
	PDefH(bool, IsNPLAExtendedLiteralPrefix, char c) ynothrow
	ImplRet(ystdex::isdigit(c) || IsNPLAExtendedLiteralNonDigitPrefix(c))

/*!
\brief 判断词素是否为 NPLA 符号。
\pre 间接断言：字符串参数的数据指针非空且字符串非空。

判断字符串是否为 NPLA 语言规范中约定支持的符号，
	即分类为 LexemeCategory::Symbol 的词素。
*/
YB_ATTR_nodiscard YB_PURE inline
	PDefH(bool, IsNPLASymbol, string_view id) ynothrowv
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
\brief 锚对象指针。
\note 满足可复制和转移且不抛出异常。
\since build 847
*/
using AnchorPtr = yimpl(shared_ptr<const void>);

/*!
\relates AnchorPtr
\since build 858
*/
//@{
static_assert(ystdex::is_nothrow_copy_constructible<AnchorPtr>(),
	"Invalid type found.");
static_assert(ystdex::is_nothrow_copy_assignable<AnchorPtr>(),
	"Invalid type found.");
static_assert(ystdex::is_nothrow_move_assignable<AnchorPtr>(),
	"Invalid type found.");
static_assert(ystdex::is_nothrow_move_constructible<AnchorPtr>(),
	"Invalid type found.");
//@}


/*!
\brief 访问项的值作为记号。
\return 通过访问项的值取得的记号的指针，或空指针表示无法取得名称。
\since build 782
*/
YB_ATTR_nodiscard YB_PURE inline observer_ptr<const TokenValue>
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
YB_ATTR_nodiscard YF_API YB_PURE string
TermToString(const TermNode&);

/*!
\brief 访问项的值并转换为可选带有引用标记的字符串形式。
\note 当前使用前缀 [*] 和空格表示引用项。直接附加字符串，因此通常表示已解析的引用。
\sa TermToString
\since build 840
*/
YB_ATTR_nodiscard YF_API YB_PURE string
TermToStringWithReferenceMark(const TermNode&, bool);
//@}

/*!
\brief 访问项初始化标签。
\since build 857
\sa GetLValueTagsOf
\sa TermTags

若项表示引用值，使用引用值排除 TermTags::Temporary 后的标签；
否则，使用项的标签。
*/
YB_ATTR_nodiscard YF_API YB_PURE TermTags
TermToTags(TermNode&);

/*!
\brief 对列表项抛出指定预期访问值的类型的异常。
\throw ListTypeError 消息中包含由参数指定的预期访问值的类型的异常。
\note 后两个参数传递给 TermToStringWithReferenceMark ，预期用法通常相同。
\sa TermToStringWithReferenceMark
\since build 855
*/
YB_NORETURN YF_API void
ThrowListTypeErrorForInvalidType(const ystdex::type_info&, const TermNode&,
	bool);

/*!
\brief 标记记号节点：递归变换节点，转换其中的词素为记号值。
\note 先变换子节点。
\since build 753
*/
YF_API void
TokenizeTerm(TermNode& term);

//! \exception 异常中立：由项的值数据成员的持有者抛出。
//@{
/*!
\brief 尝试访问项的指定类型对象指针。

尝试访问作为叶节点的 TermNode 。
和 YSLib::AccessPtr 访问 ValueNode 的 Value 数据成员类似，
	但先解析引用重定向到目标，且对持有者异常中立。
*/
//@{
template<typename _type>
YB_ATTR_nodiscard YB_PURE inline observer_ptr<_type>
TryAccessLeaf(TermNode& term)
{
	return term.Value.type() == ystdex::type_id<_type>() ? NPL::make_observer(
		std::addressof(term.Value.GetObject<_type>())) : nullptr; 
}
template<typename _type>
YB_ATTR_nodiscard YB_PURE inline observer_ptr<const _type>
TryAccessLeaf(const TermNode& term)
{
	return term.Value.type() == ystdex::type_id<_type>() ? NPL::make_observer(
		std::addressof(term.Value.GetObject<_type>())) : nullptr; 
}
//@}

/*!
\brief 尝试访问项的指定类型叶节点对象指针。

尝试访问 TermNode 。
类似 NPL::TryAccessLeaf ，但先使用 NPL::IsLeaf 判断叶节点，
	对非叶节点直接返回空指针。
*/
//@{
template<typename _type>
YB_ATTR_nodiscard YB_PURE inline observer_ptr<_type>
TryAccessTerm(TermNode& term)
{
	return IsLeaf(term) ? NPL::TryAccessLeaf<_type>(term) : nullptr;
}
template<typename _type>
YB_ATTR_nodiscard YB_PURE inline observer_ptr<const _type>
TryAccessTerm(const TermNode& term)
{
	return IsLeaf(term) ? NPL::TryAccessLeaf<_type>(term) : nullptr;
}
//@}
//@}

YB_ATTR_nodiscard YB_PURE inline
	PDefH(observer_ptr<const TokenValue>, TermToNamePtr, const TermNode& term)
	ImplRet(NPL::TryAccessTerm<TokenValue>(term))


/*!
\note 因对性能有影响，默认仅调试配置下启用。
\sa EnvironmentReference
*/
//@{
/*!
\brief 环境引用计数检查支持。
\sa Environment
\sa YTraceDe
\since build 856

若定义为 true ，则在 Environment 和 EnvironmentReference 启用引用计数检查支持。
检查在 Environment 的析构函数进行，通过对锚对象剩余引用计数的来源计数判断正常状态。
若引用计数来源都是 Environment 、 EnvironmentReference 或 TermReference ，
	则表示正常；否则，使用 YTraceDe 输出错误消息。
注意绑定析构顺序不确定，可能导致依赖不确定而误报。
因对性能有影响，默认仅调试配置下启用。
*/
#ifndef NPL_NPLA_CheckEnvironmentReferenceCount
#	ifndef NDEBUG
#		define NPL_NPLA_CheckEnvironmentReferenceCount true
#	else
#		define NPL_NPLA_CheckEnvironmentReferenceCount false
#	endif
#endif

/*!
\sa InvalidReference
\since build 876
*/
//@{
/*!
\brief 父环境访问检查支持。
\sa Environment

若定义为 true ，则在默认解析父环境时，检查环境引用是否存在。
*/
#ifndef NPL_NPLA_CheckParentEnvironment
#	ifndef NDEBUG
#		define NPL_NPLA_CheckParentEnvironment true
#	else
#		define NPL_NPLA_CheckParentEnvironment false
#	endif
#endif

/*!
\brief 项引用间接访问检查支持。
\sa TermReference

若定义为 true ，则在访问 TermReference 的值时，检查环境引用是否存在。
*/
#ifndef NPL_NPLA_CheckTermReferenceIndirection
#	ifndef NDEBUG
#		define NPL_NPLA_CheckTermReferenceIndirection true
#else
#		define NPL_NPLA_CheckTermReferenceIndirection false
#	endif
#endif
//@}
//@}

//! \since build 869
class Environment;

/*!
\brief 环境引用。
\sa NPL_NPLA_CheckEnvironmentReferenceCount
\since build 823

可能共享所有权环境的弱引用。
*/
class YF_API EnvironmentReference
	: private ystdex::equality_comparable<EnvironmentReference>
{
private:
	weak_ptr<Environment> p_weak{};
	/*!
	\brief 引用的锚对象指针。
	\since build 847
	*/
	AnchorPtr p_anchor{};

public:
	//! \since build 869
	DefDeCtor(EnvironmentReference)
	//! \brief 构造：使用指定的环境指针和此环境的锚对象指针。
	EnvironmentReference(const shared_ptr<Environment>&);
	/*!
	\brief 构造：使用指定的环境指针和锚对象指针。
	\pre 第二参数表示由环境提供的锚对象指针。
	*/
	template<typename _tParam1, typename _tParam2>
	EnvironmentReference(_tParam1&& arg1, _tParam2&& arg2)
		: p_weak(yforward(arg1)), p_anchor(yforward(arg2))
	{
#if NPL_NPLA_CheckEnvironmentReferenceCount
		ReferenceEnvironmentAnchor();
#endif
	}
#if NPL_NPLA_CheckEnvironmentReferenceCount
	//! \since build 856
	//@{
	EnvironmentReference(const EnvironmentReference& env_ref)
		: p_weak(env_ref.p_weak), p_anchor(env_ref.p_anchor)
	{
		ReferenceEnvironmentAnchor();
	}
	DefDeMoveCtor(EnvironmentReference)

	~EnvironmentReference();

	DefDeCopyAssignment(EnvironmentReference)
	DefDeMoveAssignment(EnvironmentReference)
	//@}
#else
	DefDeCopyMoveCtorAssignment(EnvironmentReference)
#endif

	//! \since build 824
	YB_ATTR_nodiscard YB_PURE friend PDefHOp(bool, ==,
		const EnvironmentReference& x, const EnvironmentReference& y) ynothrow
		ImplRet(x.p_weak.lock() == y.p_weak.lock())

	//! \since build 847
	DefGetter(const ynothrow, const AnchorPtr&, AnchorPtr, p_anchor)
	DefGetter(const ynothrow, const weak_ptr<Environment>&, Ptr, p_weak)

	PDefH(shared_ptr<Environment>, Lock, ) const ynothrow
		ImplRet(p_weak.lock())
#if NPL_NPLA_CheckEnvironmentReferenceCount

private:
	//! \since build 856
	void
	ReferenceEnvironmentAnchor();
#endif
};


/*!
\ingroup ThunkType
\brief 项引用。
\warning 非虚析构。
\since build 800

表示列表项的引用的中间求值结果的项。
项引用可选地关联环境。
当关联的环境不存在时，不提供对引入的对象内存安全的检查。
为避免安全问题，不提供不关联环境的构造函数。若需不关联环境，需显式构造空环境引用。
*/
class YF_API TermReference
	: private ystdex::equality_comparable<TermReference>
{
private:
	//! \since build 842
	lref<TermNode> term_ref;
	/*!
	\brief 引用标签。
	\since build 857
	*/
	TermTags tags = TermTags::Unqualified;
	/*!
	\brief 包含引用的锚对象指针的环境引用。
	\since build 869
	*/
	EnvironmentReference r_env;

public:
	/*!
	\brief 构造：使用参数指定的项并自动判断是否使用引用值初始化。
	\since build 821
	*/
	template<typename _tParam, typename... _tParams>
	inline
	TermReference(TermNode& term, _tParam&& arg, _tParams&&... args)
		: TermReference(TermToTags(term), term, yforward(arg),
		yforward(args)...)
	{}
	//! \since build 857
	//@{
	//! \brief 构造：使用参数指定的标签及引用。
	template<typename _tParam, typename... _tParams>
	inline
	TermReference(TermTags t, TermNode& term, _tParam&& arg, _tParams&&... args)
		: term_ref(term), tags(t),
		r_env(yforward(arg), yforward(args)...)
	{}
	//! \brief 构造：使用参数指定的标签及现有的项引用。
	//@{
	TermReference(TermTags t, const TermReference& ref) ynothrow
		: term_ref(ref.term_ref), tags(t), r_env(ref.r_env)
	{}
	TermReference(TermTags t, TermReference&& ref) ynothrow
		: term_ref(ref.term_ref), tags(t), r_env(std::move(ref.r_env))
	{}
	//@}
	//@}
	//! \since build 855
	DefDeCopyMoveCtorAssignment(TermReference)

	//! \brief 等于：当且仅当引用的项同一时相等。
	YB_ATTR_nodiscard YB_PURE friend
		PDefHOp(bool, ==, const TermReference& x, const TermReference& y)
		ImplRet(ystdex::get_equal_to<>()(x.term_ref, y.term_ref))

	/*!
	\brief 判断被引用的对象是否可通过引用被修改。
	\since build 856
	*/
	DefPred(const ynothrow, Modifiable, !bool(tags & TermTags::Nonmodifying))
	//! \since build 857
	//@{
	//! \brief 判断被引用的对象是否可通过引用值被转移。
	DefPred(const ynothrow, Movable, (tags
		& (TermTags::Unique | TermTags::Nonmodifying)) == TermTags::Unique)
	/*!
	\brief 判断引用值是否表示被引用的被绑定对象左值。

	判断引用值是否具有表示被引用的左值的标签，即排除以下情形：
	消亡值，即标签具有 TermTags::Unique 的引用值；
	引用绑定临时对象的引用值，即标签具有 TermTags::Temporary 的引用值。
	区分临时对象的情形类似宿主语言使用 std::forward<P> 转发 P&& 类型的函数参数：
	当 P&& 类型的函数形式参数绑定右值实际参数，P 为非引用类型函数模板参数；
	当 P&& 类型的函数形式参数绑定左值实际参数，P 为左值引用类型函数模板参数。
	P 为非引用类型时，类似对象语言绑定临时对象，否则类似对象语言中绑定非临时对象。
	这种区分在宿主语言中通过推断模板参数而非函数的实际参数类型或值类别确定，
		结果体现在函数的形式参数类型 P&& 是否为左值引用，等价 P 是否为左值引用。
	作为实际参数的消亡值在宿主语言保证总是推断 P 为非引用类型，这也和对象语言一致，
		允许使用实际参数中的 TermTags::Unique 标签代替形式参数类型编码这种区分；
	但对象语言函数的形式参数不具有显式类型，因此用被绑定对象上的
		TermTags::Temporary 标签标记通过纯右值（即作为临时对象）实际参数初始化
		被绑定对象的情形，对应区分 P 被推断为非左值引用的情形；
	因此，按表达式的值中是否具有 TermTags::Unique 或 TermTags::Temporary 标签，
		即可区分是否按右值转发，而实现类似宿主语言的完美转发（除限定符外）。
	据此，本函数和其它实现转发的一些 API 区分提供的参数中的引用值标签，
		即视带有 TermTags::Unique 或 TermTags::Temporary 标签的引用值为右值。
	在此，被绑定对象具有 TermTags::Temporary 类似宿主语言中推断得到 P&&
		为右值引用类型，并非需被转发的初始化它的实际参数为消亡值。
	本函数不适用在不需要转发对象语言的函数参数的上下文，如判断表达式的值类别
		（右值为带有 TermTags::Unique 的引用的消亡值，或非引用值的纯右值）；
		通常应使用 IsUnique 等代替。
	*/
	DefPred(const ynothrow, ReferencedLValue,
		!(bool(tags & TermTags::Unique) || bool(tags & TermTags::Temporary)))
	//@}
	/*!
	\brief 判断被引用的对象是否指定唯一。
	\since build 856
	*/
	DefPred(const ynothrow, Unique, bool(tags & TermTags::Unique))

	DefGetter(const ynothrow, TermTags, Tags, tags)

	//! \since build 873
	DefSetter(ynothrow, TermNode&, Referent, term_ref)
	//! \since build 873
	DefSetter(ynothrow, TermTags, Tags, tags)

	explicit DefCvtMem(const ynothrow, TermNode&, term_ref)

	/*!
	\brief 取锚对象指针。
	\since build 847
	*/
	DefGetterMem(const ynothrow, const AnchorPtr&, AnchorPtr, r_env)
	/*!
	\brief 取关联环境的引用。
	\since build 869
	*/
	DefGetter(const ynothrow, const EnvironmentReference&, EnvironmentReference,
		r_env)

	/*!
	\brief 取被引用的值。
	\throw NPLException 配置 NPL_NPLA_CheckTermReferenceIndirection 时，环境无效。
	\since build 876
	*/
#if NPL_NPLA_CheckTermReferenceIndirection
	YB_ATTR_nodiscard YB_PURE TermNode&
	get() const;
#else
	YB_ATTR_nodiscard YB_PURE PDefH(TermNode&, get, ) const ynothrow
		ImplRet(term_ref.get())
#endif
};

/*!
\sa TermNode::Tags
\relates TermReference
*/
//@{
/*!
\brief 折叠项引用。
\return 结果引用值及初始化时是否表示引用值。
\since build 857

若被参数引用值引用的对象是否是一个引用值，结果为合并标签后的被后者引用的对象和 true ；
否则，返回自身和 false 。
引用值可包含的非正规表示不被处理，调用时应注意维护子项生存期。
*/
YB_ATTR_nodiscard YF_API pair<TermReference, bool>
Collapse(TermReference);

/*!
\brief 准备折叠项引用。
\sa Environment::MakeTermTags
\since build 876

返回由以下方式确定的引用项：
当第一参数表示项引用时，返回这个引用项；
否则为通过参数初始化的项引用构成的正规表示的项。
初始化项引用时，使用指定环境创建的标签。
这种方式避免初始化引用的引用，除非第一参数已具有未折叠的引用。
使用引用值初始化时保留标签，否则使用项中的标签。
第二参数指定通过参数初始化项引用时关联的环境。
*/
YB_ATTR_nodiscard YF_API TermNode
PrepareCollapse(TermNode&, Environment&);
//@}

/*!
\brief 访问项并取解析 TermReference 间接值后的引用。
\return 若项的 Value 数据成员为 TermReference 则为其中的引用，否则为参数。
\sa TermReference
\since build 854
*/
YB_ATTR_nodiscard YB_PURE inline
	PDefH(TermNode&, ReferenceTerm, TermNode& term)
	ynoexcept_spec(std::declval<TermReference>().get())
	ImplRet(ystdex::invoke_value_or(&TermReference::get,
		NPL::TryAccessLeaf<const TermReference>(term), term))
YB_ATTR_nodiscard YB_PURE inline
	PDefH(const TermNode&, ReferenceTerm, const TermNode& term)
	ynoexcept_spec(std::declval<TermReference>().get())
	ImplRet(ystdex::invoke_value_or(&TermReference::get,
		NPL::TryAccessLeaf<const TermReference>(term), term))


//! \since build 855
//@{
/*!
\brief 表示解析项后作为访问参数的项引用指针。

用于可选参数访问的指针类型，满足：
满足 ISO C++ [nullablepointer.requirements] NullablePointer 要求；
允许隐式转换为 bool ，结果同显式转换为 bool ；
非空值可使用一元 * 和 -> 取指定项引用的 const 内建引用及成员访问；
不对被访问的项引用具所有权。
注意具体类型不应被依赖。使用内建指针实现以在某些 ABI 上提供高效的二进制实现，
	并支持转换为访问参数指针的操作在常量表达式上下文中出现。
这个类型用于解析项时的可选指针参数，其使用详见以下 NPL::ResolveTerm 。
*/
using ResolvedTermReferencePtr = yimpl(const TermReference*);

/*!
\brief 转换项引用指针为项引用的访问参数指针。
\relates ResolvedTermReferencePtr
*/
YB_ATTR_nodiscard YB_PURE yconstfn PDefH(ResolvedTermReferencePtr,
	ResolveToTermReferencePtr, observer_ptr<const TermReference> p) ynothrow
	ImplRet(p.get())
//@}


/*!
\brief 判断解析后的项是否指示可转移项。
\since build 859

判断参数指定的项是否可通过判断项的元数据确定被转移。
参数是项的引用或指向项的指针（支持包括项引用指针和项引用的访问参数指针），
	通常应表示解析项的结果。
*/
//@{
YB_ATTR_nodiscard YB_PURE inline
	PDefH(bool, IsMovable, const TermReference& ref) ynothrow
	ImplRet(ref.IsMovable())
//! \note 空指针指定解析结果中不存在引用值，被解析的项总是视为可转移的右值项。
template<typename _tPointer>
YB_ATTR_nodiscard YB_PURE inline auto
IsMovable(_tPointer p) ynothrow
	-> decltype(!bool(p) || NPL::IsMovable(*p))
{
	return !bool(p) || NPL::IsMovable(*p);
}
//@}


//! \exception 异常中立：由项的值数据成员的持有者抛出。
//@{
//! \brief 尝试访问解析 TermReference 后的项的指定类型对象指针。
//@{
/*!
\sa NPL::TryAccessLeaf
\since build 854
*/
//@{
template<typename _type>
YB_ATTR_nodiscard YB_PURE inline observer_ptr<_type>
TryAccessReferencedLeaf(TermNode& term)
{
	return NPL::TryAccessLeaf<_type>(ReferenceTerm(term));
}
template<typename _type>
YB_ATTR_nodiscard YB_PURE inline observer_ptr<const _type>
TryAccessReferencedLeaf(const TermNode& term)
{
	return NPL::TryAccessLeaf<_type>(ReferenceTerm(term));
}
//@}

/*!
\sa NPL::TryAccessTerm
\since build 858
*/
//@{
template<typename _type>
YB_ATTR_nodiscard YB_PURE inline observer_ptr<_type>
TryAccessReferencedTerm(TermNode& term)
{
	return NPL::TryAccessTerm<_type>(ReferenceTerm(term));
}
template<typename _type>
YB_ATTR_nodiscard YB_PURE inline observer_ptr<const _type>
TryAccessReferencedTerm(const TermNode& term)
{
	return NPL::TryAccessTerm<_type>(ReferenceTerm(term));
}
//@}
//@}

//! \note 使用 NPL::TryAccessLeaf 访问。
//@{
//! \since build 854
//@{
//! \brief 判断项（的值数据成员）是否为引用项。
YB_ATTR_nodiscard YF_API YB_PURE bool
IsReferenceTerm(const TermNode&);

/*!
\brief 判断项（的值数据成员）是否表示被绑定的左值引用。
\sa TermReference::IsReferencedLValue
\since build 871

判断项是否表示引用且 TermReference::IsReferencedLValue 的结果为 true 。
*/
YB_ATTR_nodiscard YF_API YB_PURE bool
IsBoundLValueTerm(const TermNode&);
//@}

/*!
\brief 判断项（的值数据成员）是否表示未折叠的引用。
\since build 869
*/
YB_ATTR_nodiscard YF_API YB_PURE bool
IsUncollapsedTerm(const TermNode&);

/*!
\brief 判断项（的值数据成员）是否表示非引用项或唯一引用。
\sa TermReference::IsUnique
\since build 859
*/
YB_ATTR_nodiscard YF_API YB_PURE bool
IsUniqueTerm(const TermNode&);
//@}

//! \since build 859
//@{
/*!
\brief 解析并间接引用处理可能是引用值的项。
\note 假定项不使用平凡正规表示，不需要对间接值检查 IsBranch 或 IsLeaf 。
\sa NPL::ResolveToTermReferencePtr
\sa ResolveTermHandler
\sa ResolvedTermReferencePtr
\sa TermReference

接受指定解析实现的函数和被解析的项作为参数，尝试访问其中是否具有引用值。
若确定是引用值，则被引用的项是被引用值引用的项；否则，被引用的项是第二参数指定的项。
确定被引用的项后，调用解析函数继续处理。
参数指定解析函数和被解析的项。
解析函数应具有第一参数取被解析的项的引用，还可以具有第二可选参数。后者是
	NPL::ResolvedTermReferencePtr 或 NPL::ResolvedTermReferencePtr 保证支持的
	可隐式转换的目的类型（如 bool ）。
调用解析函数时，指向被解析的项的 NPL::ResolvedTermReferencePtr 类型的值
	被作为实际参数。
和 observer_ptr<const TermReference> 不同，因为这个类型允许隐式转换为 bool ，
	所以解析函数的实现中可直接以 bool 作为第二形式参数。
*/
template<typename _func, class _tTerm>
auto
ResolveTerm(_func do_resolve, _tTerm&& term)
	-> yimpl(decltype(ystdex::expand_proxy<yimpl(void)(_tTerm&&,
	ResolvedTermReferencePtr)>::call(do_resolve, yforward(term),
	ResolvedTermReferencePtr())))
{
	using handler_t = yimpl(void)(_tTerm&&, ResolvedTermReferencePtr);

	// XXX: Assume value representation of %term is not trivially regular.
	if(const auto p = NPL::TryAccessLeaf<const TermReference>(term))
		return ystdex::expand_proxy<handler_t>::call(do_resolve, p->get(),
			NPL::ResolveToTermReferencePtr(p));
	return ystdex::expand_proxy<handler_t>::call(do_resolve,
		yforward(term), ResolvedTermReferencePtr());
}

/*!
\exception ListTypeError 异常中立：项为列表项。
\exception bad_any_cast 异常中立：非列表项类型检查失败。
*/
//@{
template<typename _type, class _tTerm>
void
CheckRegular(_tTerm& term, bool has_ref)
{
	if(YB_UNLIKELY(IsList(term)))
		ThrowListTypeErrorForInvalidType(ystdex::type_id<_type>(), term,
			has_ref);
}

/*!
\brief 访问项的指定类型正规值。
\sa ThrowListTypeErrorForInvalidType

以 NPL::Access 访问调用 NPL::CheckRegular 检查后的项。
*/
template<typename _type, class _tTerm>
YB_ATTR_nodiscard YB_PURE inline auto
AccessRegular(_tTerm& term, bool has_ref)
	-> yimpl(decltype(NPL::Access<_type>(term)))
{
	NPL::CheckRegular<_type>(term, has_ref);
	return NPL::Access<_type>(term);
}

/*!
\brief 访问一次解析引用值后的项的指定类型正规值。
\sa NPL::AccessRegular
\sa NPL::ResolveTerm

以 NPL::AccessRegular 访问调用 NPL::ResolveTerm 解析引用值得到的项。
若遇到正规值为引用值，则进行解引用后继续访问。解引用至多一次。
和 YSLib::Access 访问 ValueNode 类似，但调用 NPL::ResolveTerm
	解析引用重定向到目标，且首先对检查项，若项为列表项使用
	NPL::ThrowListTypeErrorForInvalidType 抛出异常。
*/
template<typename _type, class _tTerm>
YB_ATTR_nodiscard YB_PURE inline auto
ResolveRegular(_tTerm& term) -> yimpl(decltype(NPL::Access<_type>(term)))
{
	return NPL::ResolveTerm([&](_tTerm& nd, bool has_ref)
		-> yimpl(decltype(NPL::Access<_type>(term))){
		return NPL::AccessRegular<_type>(nd, has_ref);
	}, term);
}
//@}
//@}
//@}


//! \since build 801
//@{
/*!
\brief 项引用函数对象操作。
\note 这是 NPL::ReferenceTerm 的函数对象形式。
\sa ReferenceTerm
*/
struct ReferenceTermOp
{
	//! \since build 876
	template<typename _type>
	auto
	operator()(_type&& term) const
		ynoexcept_spec(NPL::ReferenceTerm(yforward(term)))
		-> decltype(NPL::ReferenceTerm(yforward(term)))
	{
		return NPL::ReferenceTerm(yforward(term));
	}
};

/*!
\brief 包装一个非项引用的操作为 NPL::ReferenceTermOp 以支持项引用。
\relates ReferenceTermOp
\since build 802
*/
template<typename _func>
YB_STATELESS auto
ComposeReferencedTermOp(_func f)
	-> yimpl(decltype(ystdex::compose_n(f, ReferenceTermOp())))
{
	return ystdex::compose_n(f, ReferenceTermOp());
}
//@}


/*!
\brief 规约状态：某个项上的一遍规约可能的中间结果。
\note 实现的编码未指定，编码和规约时的判断性能相关。
\since build 730
*/
enum class ReductionStatus : yimpl(size_t)
{
	/*!
	\brief 部分规约：需要继续进行规约。
	\note 一般仅用于异步规约。
	\since build 841
	*/
	Partial = yimpl(0x00),
	/*!
	\brief 中立规约：规约成功终止，且未指定是否需要保留子项。
	\note 一般用于不改变被规约项是否为空项的规约遍。
	\since build 869
	*/
	Neutral = yimpl(0x01),
	/*!
	\brief 纯值规约：规约成功终止，且要求最终清理子项。
	\since build 841
	*/
	Clean = yimpl(0x02),
	/*!
	\brief 非纯值规约：规约成功终止，且需要保留子项。
	\since build 757
	*/
	Retained = yimpl(0x03),
	/*!
	\brief 已取得正规表示的规约：规约成功且已保证正规化，不指定是否需要保留子项。
	\since build 865
	*/
	Regular = yimpl(Retained),
	/*!
	\brief 重规约。
	\since build 757
	*/
	Retrying = yimpl(0x10)
};

/*!
\brief 根据规约状态检查是否可继续规约。
\relates ReductionStatus
\sa YTraceDe
\since build 734

只根据参数确定结果：当且仅当参数规约成功时不视为可继续规约。
在调试配置下，若发现不支持的状态视为不成功，输出警告。
派生实现可使用类似的接口指定多个不同的状态。
*/
YB_ATTR_nodiscard YF_API YB_STATELESS bool
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
\brief 提升项：设置项的内容为参数指定的项或值。
\since build 805

使用第二参数指定的项的内容替换第一个项的内容。
不修改项的子项和值数据成员以外的的内容。
*/
//@{
/*!
\pre 间接断言：第一和第二参数指定不相同的项。
\since build 876
*/
inline PDefH(void, LiftOther, TermNode& term, TermNode& tm)
	ImplExpr(term.MoveContent(std::move(tm)))

/*!
\note 参数相同时作用为空，但可能有额外开销。
\since build 844
*/
inline PDefH(void, LiftTerm, TermNode& term, TermNode& tm)
	ImplExpr(!ystdex::ref_eq<>()(term, tm) ? LiftOther(term, tm) : void())
inline PDefH(void, LiftTerm, ValueObject& tv, ValueObject& vo) ynothrow
	ImplExpr(!ystdex::ref_eq<>()(tv, vo) ? void(tv = std::move(vo)) : void())
inline PDefH(void, LiftTerm, TermNode& term, ValueObject& vo) ynothrow
	ImplExpr(LiftTerm(term.Value, vo))
//@}

/*!
\brief 提升项或项的副本。

提升参数指定的项或设置项。第三参数指定是否直接提升。
*/
//@{
/*!
\pre 间接断言：非直接提升或第一和第二参数指定不相同的项。
\pre 直接提升或第二参数指定的项不共享第一参数的项的子项。
\note 若直接提升，同 LiftOther ，否则同 TermNode::SetContent 。
\since build 876
*/
YF_API void
LiftOtherOrCopy(TermNode&, TermNode&, bool);

/*!
\note 若直接提升，同 LiftTerm ，否则同创建对象副本调用 TermNode::MoveContent 。
\since build 859
*/
YF_API void
LiftTermOrCopy(TermNode&, TermNode&, bool);
//@}

/*!
\brief 提升项的值数据成员或项的值数据成员的副本。
\note 复制引用项引用的对象，复制或转移非引用项的对象。
\note 第三参数指定转移是否被转移。
\brief 提升引用的项的值。
\sa NPL::SetContentWith
\since build 872

提升第二参数引用的项使其中可能包含的引用值以满足返回值的内存安全要求。
提升参数指定的项或设置项。第三参数指定是否直接提升。
若直接提升，同 LiftTerm ，否则同 TermNode::SetContent 。
*/
YF_API void
LiftTermValueOrCopy(TermNode&, TermNode&, bool);


//! \note 不复制项的标签。这适合不保留标签的赋值操作。
//@{
/*!
\brief 提升折叠后的项引用到指定的项。
\sa Collapse
\sa LiftTerm
\since build 858

按参数指定的项提升引用。参数分别为目标、源和项引用。
使用第二参数以支持非正规表示。
假定第三参数引用的项是第二参数；
否则，若目标和源项不相同且引用值具有非正规表示，行为未定义。
*/
YF_API void
LiftCollapsed(TermNode&, TermNode&, TermReference);

/*!
\brief 提升折叠后的第二参数指定的项。
\pre 间接断言：第一和第二参数指定不相同的项。
\sa Collapse
\sa LiftCollapsed
\sa LiftOther
\since build 871

按参数指定的项提升折叠的项。参数分别为目标和源。
当源是引用项时，提升源表示的引用，否则同 LiftOther 。
其中提升操作等效调用 LiftCollapsed 。
*/
YF_API void
MoveCollapsed(TermNode&, TermNode&);
//@}

/*!
\warning 引入的间接值无所有权，应注意在生存期内使用以保证内存安全。
\todo 支持消亡值和复制。
*/
//@{
/*!
\brief 提升项引用：使用第二参数指定的项的内容引用替换第一个项的内容。
\sa ValueObject::MakeIndirect
\since build 747
*/
//@{
//! \since build 799
inline PDefH(void, LiftTermRef, TermNode& term, const TermNode& tm)
	ImplExpr(NPL::SetContentWith(term, tm, &ValueObject::MakeIndirect))
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

若项对象表示引用值则提升项，
否则对 ValueObject 进行基于所有权的生存期检查并取引用这个项的引用值。
*/
YF_API void
LiftToReference(TermNode&, TermNode&);


/*!
\brief 提升引用的项作为转移的项。
\note 复制引用项引用的对象，复制或转移非引用项的对象。
\note 第三参数指定转移是否被转移。

提升第二参数引用的项使其中可能包含的引用值以满足返回值的内存安全要求。
*/
//@{
/*!
\sa LiftTermOrCopy
\since build 873
*/
inline PDefH(void, LiftMoved, TermNode& term, const TermReference& ref,
	bool move)
	ImplExpr(LiftTermOrCopy(term, ref.get(), move))

/*!
\pre 间接断言：非直接提升或第一和第二参数指定不相同的项。
\pre 直接提升或第二参数指定的项不共享第一参数的项的子项。
\sa LiftOtherOrCopy
\since build 876
*/
inline PDefH(void, LiftMovedOther, TermNode& term, const TermReference& ref,
	bool move)
	ImplExpr(LiftOtherOrCopy(term, ref.get(), move))
//@}

/*!
\brief 提升项作为返回值。
\sa LiftMoved
\since build 828

提升项的值数据成员可能包含的引用值以满足返回值的内存安全要求。
提升可能转移或复制对象作为返回值，由一般表达式的值确定可转移：
转移条件等价可转移项，即按 NPL::IsMovable 判断。
若参数是引用项，以项引用作为第二参数调用 NPL::LiftMoved 。
*/
YF_API void
LiftToReturn(TermNode&);
//@}

/*!
\pre 间接断言：参数指定不相同的项。
\note 等价使用 TermReference::IsReferencedLValue 区分左值。
\sa TermNode::MoveContent
\sa TermReference::IsReferencedLValue
*/
//@{
/*!
\brief 提升第二参数指定的右值项作为转发的第一参数指定项中的返回值。
\pre 间接断言：第一和第二参数指定不相同的项。
\since build 872

效果同先提升第二参数到第一参数，然后对结果进行处理以转发：
提升表示右值的项的值数据成员可能包含的引用值以满足返回值的内存安全要求；
转发提升后的值。
转发操作由被转发表达式的值确定可转移：
对左值不进行修改；
否则同 LiftToReturn ，但临时对象的引用值被视为可转移。
同 MoveRValueToReturn ，但转移而非复制临时对象。
可在对象语言中实现类似 C++ 的 std::forward 的转发操作，
	但转移非左值为非引用类型的值。
*/
YF_API void
MoveRValueToForward(TermNode&, TermNode&);

/*!
\brief 提升第二参数指定的右值项作为第一参数指定项中的返回值。
\sa LiftToReturn
\since build 871

效果同先提升第二参数到第一参数，然后对结果进行处理以传递返回值：
提升表示右值的项的值数据成员可能包含的引用值以满足返回值的内存安全要求；
传递提升后的值。
传递操作由一般表达式的值确定可转移：
对左值不进行修改，否则同 LiftToReturn 。
因允许减少提升，实现可较无条件转移参数指定的项后转换返回值高效。
可在对象语言中实现类似 C++ 的 std::forward 的转发操作，
	但复制非左值为非引用类型的值。
*/
YF_API void
MoveRValueToReturn(TermNode&, TermNode&);
//@}

/*!
\brief 对每个子项提升项的值数据成员可能包含的引用值。
\sa LiftToReturn
\since build 830
*/
inline PDefH(void, LiftSubtermsToReturn, TermNode& term)
	ImplExpr(std::for_each(term.begin(), term.end(), LiftToReturn))

//! \pre 断言：参数指定的项是枝节点。
//@{
/*!
\brief 提升首项：使用第一个子项替换项的内容。
\since build 685
*/
inline PDefH(void, LiftFirst, TermNode& term)
	ImplExpr(LiftOther(term, AccessFirstSubterm(term)))

/*!
\brief 提升末项：使用最后一个子项替换项的内容。
\since build 696
*/
inline PDefH(void, LiftLast, TermNode& term)
	ImplExpr(AssertBranch(term), LiftTerm(term, NPL::Deref(term.rbegin())))
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
\brief 规约提升结果。
\return ReductionStatus::Retained 。
\sa LiftToReturn
\since build 855

调用 LiftToReturn 提升结果，再返回规约状态。
*/
inline PDefH(ReductionStatus, ReduceForLiftedResult, TermNode& term)
	// NOTE: This is mainly used to update %LastStatus in the enclosing context.
	ImplRet(LiftToReturn(term), ReductionStatus::Retained)

/*!
\brief 规约第一个非结尾空列表子项。
\return ReductionStatus::Neutral 。
\sa RemoveHead
\since build 774

若项具有不少于一个子项且第一个子项是空列表则移除。
允许空列表作为第一个子项以标记没有操作数的函数应用。
*/
YF_API ReductionStatus
ReduceHeadEmptyList(TermNode&) ynothrow;

/*!
\return 移除项时 ReductionStatus::Retained ，否则 ReductionStatus::Regular 。
\note 返回值总是指示范式。
\sa RemoveHead
*/
//@{
/*!
\brief 规约为列表：对分支列表节点移除第一个子项，保留余下的子项作为列表。
\sa ReduceBranchToList
\since build 774

对分支列表节点移除项，同 NPL::ReduceBranchToList ；否则直接返回规约结果。
*/
inline PDefH(ReductionStatus, ReduceToList, TermNode& term) ynothrow
	ImplRet(IsBranchedList(term) ? ReduceBranchToList(term)
		: ReductionStatus::Regular)

/*!
\brief 规约为列表值：对分支列表节点移除第一个子项，保留余下的子项提升后作为列表的值。
\sa ReduceBranchToListValue
\since build 821

对分支列表节点移除项，同 NPL::ReduceBranchToListValue ；否则直接返回规约结果。
*/
inline PDefH(ReductionStatus, ReduceToListValue, TermNode& term) ynothrow
	ImplRet(IsBranchedList(term) ? ReduceBranchToListValue(term)
		: ReductionStatus::Regular)
//@}

/*!
\pre 间接断言：第一和第二参数指定不相同的项。
\since build 871
*/
//@{
//! \note 第三参数指定的被引用项关联的环境被作为新创建的引用值关联的环境。
//@{
/*!
\brief 规约转发第二参数指定的项表示的引用值作为求值结果。
\pre 间接断言：第二参数指定的项是引用项或第三参数非空。
\pre 间接断言：第三参数空或第一和第二参数指定不相同的项。
\pre 第三参数非空或第二参数指定的项不共享第一参数的项的子项。

若第二参数指定引用项，复制或转移到第一参数，返回 ReductionStatus::Retained ；
否则，以相同参数调用 ReduceToReferenceAt 。
使用引用项复制或转移时，非空的第三参数指定复制，否则指定转移。
*/
YF_API ReductionStatus
ReduceToReference(TermNode&, TermNode&, ResolvedTermReferencePtr);

/*!
\brief 规约转发第二参数指定的项表示的未折叠引用值作为求值结果。
\return ReductionStatus::Clean 。
\pre 间接断言：第三参数非空。

以第二参数指定的项中的对象作为被引用对象，设置第一参数表示的值为其引用值。
*/
YF_API ReductionStatus
ReduceToReferenceAt(TermNode&, TermNode&, ResolvedTermReferencePtr);
//@}

/*!
\brief 规约转发第二参数指定的项表示的值作为求值结果。
\return ReductionStatus::Retained 。
\sa MoveRValueToReturn
*/
inline PDefH(ReductionStatus, ReduceToValue, TermNode& term, TermNode& tm)
	ImplRet(MoveRValueToReturn(term, tm), ReductionStatus::Retained)
//@}


/*!
\brief 判断参数指定的规约结果在合并中是否可被覆盖。
\warning 具体支持的值未指定，结果的稳定性不应被依赖。
\since build 869
*/
YB_ATTR_nodiscard yconstfn YB_STATELESS PDefH(bool,
	IsOverridableReductionResult, ReductionStatus r) ynothrow
	ImplRet(yimpl(r == ReductionStatus::Clean
		|| r == ReductionStatus::Retained))

/*!
\since build 807
\note 一般第一参数用于指定被合并的之前的规约结果，第二参数指定用于合并的结果。
\return 合并后的规约结果。
*/
//@{
/*!
\brief 合并规约结果。

若第二参数显式指定是否保留或不保留子项，则合并后的规约结果为第二参数；
否则为第一参数。
*/
YB_ATTR_nodiscard yconstfn YB_STATELESS PDefH(ReductionStatus,
	CombineReductionResult, ReductionStatus res, ReductionStatus r) ynothrow
	ImplRet(IsOverridableReductionResult(r) ? r : res)

/*!
\brief 合并序列规约结果。
\sa CheckReducible

若第二参数指定可继续规约的项，则合并后的规约结果为第二参数；
	否则同 CombineReductionResult 。
若可继续规约，则指定当前规约的项的规约已终止，不合并第一参数指定的结果。
下一轮序列的规约可能使用或忽略合并后的结果。
*/
YB_ATTR_nodiscard YB_PURE inline
	PDefH(ReductionStatus, CombineSequenceReductionResult, ReductionStatus res,
	ReductionStatus r) ynothrow
	ImplRet(CheckReducible(r) ? r : CombineReductionResult(res, r))
//@}


//! \warning 非虚析构。
//@{
/*!
\brief 环境列表。
\since build 798

指定环境对象引用的有序集合。
*/
using EnvironmentList = vector<ValueObject>;


/*!
\brief 环境。
\warning 避免 shared_ptr 析构方式不兼容的初始化。
\since build 787
*/
class YF_API Environment : private ystdex::equality_comparable<Environment>,
	public enable_shared_from_this<Environment>
{
public:
	// TODO: Wait for %unordered_set to support transparent keys.
	//! \since build 788
	using BindingMap = YSLib::map<string, TermNode, ystdex::less<>>;
	/*!
	\brief 名称解析结果。
	\since build 821

	名称解析的返回结果是环境中的绑定目标的对象指针和直接保存绑定目标的环境的引用。
	*/
	using NameResolution
		= pair<observer_ptr<BindingMap::mapped_type>, lref<Environment>>;
	/*!
	\brief 分配器类型。
	\note 支持 uses-allocator 构造。
	\since build 847
	*/
	using allocator_type = BindingMap::allocator_type;

	//! \since build 788
	mutable BindingMap Bindings;
	/*!
	\brief 父环境：被解释的重定向目标。
	\sa ContextNode::DefaultResolve
	\since build 798
	*/
	ValueObject Parent{};
	/*!
	\brief 冻结状态。
	\sa MakeTermTags
	\since build 871
	*/
	bool Frozen = {};

private:
	/*!
	\brief 锚对象指针：提供被引用计数。
	\since build 869
	*/
	AnchorPtr p_anchor{InitAnchor()};

public:
	//! \since build 845
	//@{
	//! \brief 构造：使用指定的绑定映射分配器初始化空环境。
	Environment(allocator_type a)
		: Bindings(a)
	{}
	//! \brief 构造：使用指定的存储资源构造的绑定映射分配器初始化空环境。
	Environment(pmr::memory_resource& rsrc)
		: Environment(allocator_type(&rsrc))
	{}
	//@}
	/*!
	\brief 构造：使用包含绑定节点的指针。
	\note 不检查绑定的名称。
	*/
	//@{
	//! \since build 845
	explicit
	Environment(const BindingMap& m)
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
	\since build 845
	\todo 使用专用的异常类型。
	*/
	//@{
	//! \since build 847
	//@{
	Environment(const ValueObject& vo, allocator_type a)
		: Bindings(a), Parent((CheckParent(vo), vo))
	{}
	Environment(ValueObject&& vo, allocator_type a)
		: Bindings(a), Parent((CheckParent(vo), std::move(vo)))
	{}
	//@}
	Environment(pmr::memory_resource& rsrc, const ValueObject& vo)
		: Environment(vo, allocator_type(&rsrc))
	{}
	Environment(pmr::memory_resource& rsrc, ValueObject&& vo)
		: Environment(std::move(vo), allocator_type(&rsrc))
	{}
	//@}
	//! \since build 847
	//@{
	Environment(const Environment& e)
		: enable_shared_from_this<Environment>(e),
		Bindings(e.Bindings), Parent(e.Parent)
	{}
	DefDeMoveCtor(Environment)

	PDefHOp(Environment&, =, const Environment& e) ynothrow
		ImplRet(ystdex::copy_and_swap(*this, e))
	/*!
	\brief 析构。
	\note 可检查锚对象中的引用计数。
	\since build 856
	*/
	~Environment();

	DefDeMoveAssignment(Environment)
	//@}

	//! \since build 852
	template<typename _tKey>
	TermNode&
	operator[](_tKey&& k)
	{
		return NPL::Deref(ystdex::try_emplace(Bindings, yforward(k),
			NoContainer).first).second;
	}

	YB_ATTR_nodiscard YB_PURE friend
		PDefHOp(bool, ==, const Environment& x, const Environment& y) ynothrow
		// XXX: This is usd rarely (more often, equality on
		//	%shared_ptr<Environment> is instead), but just keep it simple here
		//	to avoid wrong ("always true") complement like it in %ValueObject.
		ImplRet(ystdex::ref_eq<>()(x, y))

	/*!
	\brief 判断锚对象未被外部引用。
	\since build 830
	*/
	DefPred(const ynothrow, Orphan, p_anchor.use_count() == 1)

	/*!
	\brief 取锚对象指针。
	\since build 856
	*/
	DefGetter(const ynothrow, const AnchorPtr&, AnchorPtr, p_anchor)
	/*!
	\brief 取锚对象指针的引用计数。
	\since build 847
	*/
	DefGetter(const ynothrow, size_t, AnchorCount, size_t(p_anchor.use_count()))
	/*!
	\brief 取名称绑定映射。
	\since build 788
	*/
	DefGetter(const ynothrow, BindingMap&, MapRef, Bindings)

	/*!
	\brief 添加绑定。
	\since build 852
	*/
	//@{
	template<typename _tKey, typename... _tParams>
	inline yimpl(ystdex::enable_if_inconvertible_t)<_tKey&&,
		BindingMap::const_iterator, bool>
	AddValue(_tKey&& k, _tParams&&... args)
	{
		return ystdex::try_emplace(Bindings, yforward(k), NoContainer,
			yforward(args)...).second;
	}
	template<typename _tKey, typename... _tParams>
	inline bool
	AddValue(BindingMap::const_iterator hint, _tKey&& k, _tParams&&... args)
	{
		return ystdex::try_emplace_hint(Bindings, hint, yforward(k),
			NoContainer, yforward(args)...).second;
	}
	//@}

	/*!
	\brief 添加或覆盖绑定。
	\since build 857
	*/
	template<typename _tKey, class _tNode>
	TermNode&
	Bind(_tKey&& k, _tNode&& tm)
	{
		// XXX: %YSLib::map::insert_or_assign does not allows keys not of
		//	%BindingMap::key_type to be directly forwarded.
		return NPL::Deref(ystdex::insert_or_assign(Bindings, yforward(k),
			yforward(tm)).first).second;
	}

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
	//@}

	/*!
	\pre 断言：第一参数的数据指针非空。
	\throw BadIdentifier 非强制调用时发现标识符不存在或冲突。
	\warning 应避免对被替换或移除的值的悬空引用。
	\since build 867
	*/
	//@{
	/*!
	\brief 以字符串为标识符在指定上下文中添加或覆盖定义。
	\note 不检查标识符合法性。
	\note 若定义已存在，覆盖这个定义。覆盖相同的定义无作用。
	*/
	void
	Define(string_view, ValueObject&&);

	/*!
	\brief 以字符串为标识符在指定上下文中定义值。
	\throw BadIdentifier 定义已存在。
	*/
	void
	DefineChecked(string_view, ValueObject&&);

	/*!
	\brief 确保环境指针有效。
	\exception std::invalid_argument 异常中立：由 ThrowForInvalidType 抛出。
	\sa ThrowForInvalidType
	\since build 872
	*/
	static Environment&
	EnsureValid(const shared_ptr<Environment>&);

private:
	/*!
	\brief 初始化锚对象指针。
	\since build 869
	*/
	YB_ATTR_nodiscard YB_PURE AnchorPtr
	InitAnchor() const;

public:
	/*!
	\brief 查找名称。
	\return 查找到的名称，或查找失败时的空值。
	\since build 852

	在环境中查找名称。
	*/
	YB_ATTR_nodiscard YB_PURE NameResolution::first_type
	LookupName(string_view) const;

	/*!
	\brief 根据当前状态创建指定项的引用。
	\note 项通常表示当前环境对象所有的被绑定对象。
	\since build 871
	*/
	PDefH(TermTags, MakeTermTags, const TermNode& term) const
		ImplRet(Frozen ? term.Tags | TermTags::Nonmodifying : term.Tags)

	/*!
	\brief 以字符串为标识符在指定上下文移除定义。
	\return 是否成功移除。
	*/
	bool
	Remove(string_view);
	/*!
	\brief 以字符串为标识符在指定上下文移除定义并检查是否成功。
	\throw BadIdentifier 定义不存在。
	*/
	void
	RemoveChecked(string_view);

	/*!
	\brief 以字符串为标识符在指定上下文的名称查找结果中替换定义。
	\note 若定义不存在则忽略。
	*/
	void
	Replace(string_view, ValueObject&&);

	/*!
	\brief 以字符串为标识符在指定上下文的名称查找结果中替换已存在的定义。
	\throw BadIdentifier 定义不存在。
	*/
	void
	ReplaceChecked(string_view, ValueObject&&);
	//@}

	/*!
	\brief 对不符合环境要求的类型抛出异常。
	\throw TypeError 环境类型检查失败。
	\since build 799
	*/
	YB_NORETURN static void
	ThrowForInvalidType(const ystdex::type_info&);

	/*!
	\brief 抛出无效环境值异常。
	\throw std::invalid_argument 环境宿主值失败。
	\note 参数指定是否用于构造上下文，决定不同的异常消息。
	\note 用于对不符合作为对象语言中有效环境引用的值的宿主值抛出异常。
	\since build 871
	*/
	YB_NORETURN static void
	ThrowForInvalidValue(bool = {});

	//! \since build 746
	friend PDefH(void, swap, Environment& x, Environment& y) ynothrow
		ImplExpr(swap(x.Bindings, y.Bindings), swap(x.Parent, y.Parent),
			swap(x.p_anchor, y.p_anchor))
};


//! \since build 877
class ContextNode;


/*!
\brief 规约动作类型：和绑定上下文以外参数的求值遍的处理器具有等价签名的多态函数。
\warning 假定转移不抛出异常。
\since build 841

对应遍处理器的调用包装类型。
动作有效当前仅当 NPL::Reducer 类型的值能上下文转换为 bool 类型后为真。
*/
yimpl(using) Reducer = ystdex::expanded_function<ReductionStatus(ContextNode&)>;
//@}

/*!
\relates Reducer
\since build 887
*/
//@{
template<class _tAlloc, class _func,
	yimpl(typename = ystdex::enable_if_same_param_t<Reducer, _func>)>
YB_FLATTEN inline _func&&
ToReducer(const _tAlloc&, _func&& f)
{
	return yforward(f);
}
template<class _tAlloc, typename _tParam, typename... _tParams>
inline
	yimpl(ystdex::exclude_self_t)<Reducer, _tParam, Reducer>
ToReducer(const _tAlloc& a, _tParam&& arg, _tParams&&... args)
{
#if true
	return Reducer(std::allocator_arg, a, yforward(arg), yforward(args)...);
#else
	return ystdex::make_obj_using_allocator<Reducer>(a, yforward(arg),
		yforward(args)...);
#endif
}
//@}


/*!
\brief 上下文节点。
\since build 782
*/
class YF_API ContextNode
{
private:
	/*!
	\warning 非虚析构。
	\since build 892
	*/
	//@{
	using ReducerSequenceBase = YSLib::forward_list<Reducer>;

public:
	//! \brief 动作序列类型：蕴含当前上下文的续延。
	class YF_API ReducerSequence : public ReducerSequenceBase
	{
	public:
		using ReducerSequenceBase::ReducerSequenceBase;
		// XXX: This ignores %select_on_container_copy_construction.
		ReducerSequence(const ReducerSequence& act)
			: ReducerSequenceBase(act, act.get_allocator())
		{}
		DefDeMoveCtor(ReducerSequence)
		~ReducerSequence()
		{
			clear();
		}

		DefDeCopyMoveAssignment(ReducerSequence)

		void
		clear() ynothrow
		{
			while(!empty())
				pop_front();
		}
	};

	//! \brief 规约守卫：暂存当前动作序列。
	class YF_API ReductionGuard
	{
	private:
		observer_ptr<ContextNode> p_ctx;
		ReducerSequence::const_iterator i_stacked;

	public:
		ReductionGuard(ContextNode& ctx) ynothrow
			: p_ctx(NPL::make_observer(&ctx)), i_stacked(ctx.stacked.cbegin())
		{
			ctx.stacked.splice_after(ctx.stacked.cbefore_begin(),
				ctx.current);
		}
		ReductionGuard(ReductionGuard&& gd) ynothrow
			: p_ctx(gd.p_ctx), i_stacked(gd.i_stacked)
		{
			gd.p_ctx = {};
		}
		//! \note 恢复暂存的动作序列时不修改当前动作序列。
		~ReductionGuard()
		{
			if(p_ctx)
			{
				auto& ctx(*p_ctx);

				ctx.current.splice_after(ctx.current.cbefore_begin(),
					ctx.stacked, ctx.stacked.cbefore_begin(), i_stacked);
			}
		}
	};
	//@}

private:
	/*!
	\brief 内部存储资源。
	\since build 845
	*/
	lref<pmr::memory_resource> memory_rsrc;
	/*!
	\brief 环境记录指针。
	\invariant p_record 。
	\since build 788
	*/
	shared_ptr<Environment> p_record{YSLib::allocate_shared<Environment>(
		Environment::allocator_type(&memory_rsrc.get()))};

public:
	/*!
	\brief 解析名称：处理保留名称并查找名称。
	\return 查找到的名称，或查找失败时的空值。
	\pre 实现断言：第二参数的数据指针非空。
	\exception NPLException 对实现异常中立的未指定派生类型的异常。
	\note 失败时若抛出异常，条件由实现定义。
	\sa LookupName
	\sa NameResolution
	\sa Environment::Parent
	\since build 883

	解析指定环境中的名称。
	值指定的实现可被改变，应符合以下描述中的要求。
	被解析的环境可能被重定向：解析失败时，尝试提供能进一步解析名称的环境。
	重定向通常在局部解析失败时进行，也可对特定保留名称进行。
	重定向的目标通常是父环境。
	实现名称解析的一般步骤包括：
	局部解析：对被处理的上下文查找名称，若成功则返回；
	否则，若支持重定向，尝试重定向解析：在重定向后的环境中重新查找名称；
	否则，名称解析失败。
	名称解析失败时默认返回空值和未指定的环境引用值。对特定的失败，实现可约定抛出异常。
	只有和名称解析的相关保留名称需被处理。其它保留名称被忽略。
	是否支持重定向由实现指定。若实现支持重定向，应指定确定重定向的目标的规则。
	除非实现另行指定，保留名称为空集；否则，保留名称的集合由实现指定。
	解析名称重定向上下文时，可能找到被重定向的环境或被解析的对象的循环引用。
	不保证检查对反复进入同一重定向环境的循环重定向的环境引用或被解析对象的循环引用。
	调用名称解析接口需注意绑定中的状态不引起循环重定向。
	若引起循环重定向，除非实现另行指定，行为未定义。
	若循环重定向的行为被实现定义，则实现应能确定循环重定向存在；
		此后，名称解析最终应失败或不终止。
	若使用被解析的对象的循环引用，可在对象语言中引起未定义行为。
	*/
	function<Environment::NameResolution(Environment&, string_view)>
		Resolve{DefaultResolve};

private:
	//! \since build 892
	//@{
	/*!
	\brief 上下文中的当前动作：已被激活的规约操作调用后等待规约的动作序列。
	\note 为便于确保资源释放和异常安全，不使用 ystdex::one_shot 。
	*/
	ReducerSequence
		current{ReducerSequence::allocator_type(&memory_rsrc.get())};
	/*!
	\brief 空动作序列缓存。
	\sa ApplyTail
	\sa SetupFront
	*/
	ReducerSequenceBase stashed{current.get_allocator()};
	/*!
	\brief 被调用以先入后出顺序保存的上下文的动作序列。
	\sa MakeCurrentActionGuard
	*/
	ReducerSequence stacked{current.get_allocator()};
	//@}

public:
	/*!
	\brief 最后一次规约状态。
	\sa ApplyTail
	\since build 813
	*/
	ReductionStatus LastStatus = ReductionStatus::Neutral;
	/*!
	\brief 上下文日志追踪。
	\since build 803
	*/
	YSLib::Logger Trace{};

	/*!
	\brief 构造：使用指定的存储资源。
	\since build 845
	*/
	ContextNode(pmr::memory_resource&);
	/*!
	\brief 构造：使用对象副本和环境指针。 
	\throw std::invalid_argument 参数指针为空。
	\sa Environment::ThrowForInvalidValue
	\since build 788

	使用参数指定的对象的部分的子对象值初始化。
	第二参数被检查后转移构造记录指针。
	遍、非缓存动作和日志追踪对象被复制。
	*/
	ContextNode(const ContextNode&, shared_ptr<Environment>&&);
	/*!
	\brief 复制构造。
	\since build 892

	使用参数指定的对象的部分的子对象值初始化。
	内存资源、记录指针、遍、非缓存动作、日志追踪对象和规约状态被复制。
	*/
	ContextNode(const ContextNode&);
	/*!
	\brief 转移构造。
	\post <tt>p_record->Bindings.empty()</tt> 。
	\since build 811
	*/
	ContextNode(ContextNode&&) ynothrow;
	/*!
	\brief 虚析构：顺序移除当前序列中的动作。
	\sa UnwindCurrent
	\since build 842
	*/
	virtual
	~ContextNode();

	DefDeCopyMoveAssignment(ContextNode)

	//! \since build 892
	//@{
	DefPred(const ynothrow, Alive, !current.empty())

	//! \since build 788
	DefGetter(const ynothrow, Environment::BindingMap&, BindingsRef,
		GetRecordRef().GetMapRef())
	DefGetter(const ynothrow, const ReducerSequence&, Current, current)
	DefGetter(const ynothrow
		-> decltype(std::declval<Reducer>().target_type()), auto,
		CurrentActionType, IsAlive() ? current.front().target_type()
		: ystdex::type_id<void>())
	//@}
	/*!
	\brief 取环境记录引用。
	\since build 788
	*/
	DefGetter(const ynothrow, Environment&, RecordRef, *p_record)
	//! \since build 845
	DefGetter(const ynothrow, pmr::memory_resource&, MemoryResourceRef,
		memory_rsrc)

	/*!
	\brief 访问指定类型的当前动作目标。
	\since build 892
	\todo 添加 const 重载。
	*/
	template<typename _type>
	YB_ATTR_nodiscard YB_PURE _type*
	AccessCurrentAs()
	{
		return IsAlive() ? current.front().template target<_type>() : nullptr;
	}

	/*!
	\brief 转移并应用作为尾动作的当前动作，并设置 LastStatus 。
	\note 非强异常安全：当动作调用抛出异常，不恢复已转移的动作。
	\note 不无效化作序列中第一个动作以外的元素。
	\pre 断言：\c IsAlive() 。
	\sa LastStatus
	\since build 810

	转移当前动作序列的第一个动作，然后调用。
	调用前的转移允许调用 SetupCurrent 或 SetupFront 等设置新的尾调用。
	此时，不需要处理已被转移的即已激活的动作。
	*/
	ReductionStatus
	ApplyTail();

	/*!
	\brief 默认的名称解析算法。
	\pre 断言：第二参数的数据指针非空。
	\exception InvalidReference 访问共享重定向环境失败。
	\sa Environment::LookupName
	\sa Environment::Parent
	\sa NPL_NPLA_CheckParentEnvironment
	\sa Resolve
	\since build 869

	按默认环境解析规则解析名称。
	局部解析失败时，重定向解析 Parent 储存的对象作为父环境的引用值。
	不在其它条件下重定向。不重定向到其它目标。
	重定向的候选目标是到有限个不同的环境。
	支持的重定向项的宿主值的类型包括：
	EnvironmentList ：环境列表；
	EnvironmentReference 可能具有共享所有权的重定向环境；
	shared_ptr<Environment> 具有共享所有权的重定向环境。
	若重定向可能具有共享所有权的失败，则表示资源访问错误，如构成循环引用；
		发现环境的循环引用视为失败，抛出异常；其它情形行为未定义。
	以上支持的宿主值类型被依次尝试访问。若成功，则使用此宿主值类型访问环境。
	对列表，使用 DFS （深度优先搜索）依次递归检查其元素。
	循环重定向不终止。
	*/
	YB_ATTR_nodiscard static Environment::NameResolution
	DefaultResolve(Environment&, string_view);

	/*!
	\brief 重写项。
	\pre 间接断言：\c !IsAlive() 。
	\post \c IsAlive() 。
	\exception std::bad_function_call Reducer 参数为空。
	\throw 嵌套 TypeError ：类型不匹配，由 Reduce 抛出的 bad_any_cast 转换。
	\note 不处理重规约。
	\sa ApplyTail
	\sa SetupCurrent
	\since build 810

	调用 IsAlive 判断状态，当可规约时调用 ApplyTail 迭代规约重写。
	因为递归重写平摊到单一的循环 CheckReducible 不用于判断是否需要继续重写循环。
	每次调用当前动作的结果同步到 TailResult 。
	返回值为最后一次当前动作调用结果。
	*/
	YB_FLATTEN ReductionStatus
	Rewrite(Reducer);

	/*!
	\brief 设置当前动作以重规约。
	\pre 断言：\c !IsAlive() 。
	\pre 动作转移无异常抛出。
	\since build 887
	*/
	template<typename... _tParams>
	inline void
	SetupCurrent(_tParams&&... args)
	{
		YAssert(!IsAlive(), "Old continuation is overriden.");
		return SetupFront(yforward(args)...);
	}

	/*!
	\brief 在当前动作序列中添加动作以重规约。
	\pre 动作转移无异常抛出。
	\since build 892
	*/
	template<typename... _tParams>
	inline void
	SetupFront(_tParams&&... args)
	{
		if(!stashed.empty())
		{
			stashed.front()
				= NPL::ToReducer(get_allocator(), yforward(args)...);
			current.splice_after(current.cbefore_begin(), stashed,
				stashed.cbefore_begin());
		}
		else
			current.push_front(
				NPL::ToReducer(get_allocator(), yforward(args)...));
	}

 	/*!
	\brief 切换当前动作序列。
	\since build 892

	交换当前动作序列并返回旧的值。
 	*/
	PDefH(ReducerSequence, Switch, ReducerSequence acts = {})
		ImplRet(ystdex::exchange(current, std::move(acts)))

	/*!
	\brief 切换环境。
	\since build 872
	*/
	//@{
	/*!
	\throw std::invalid_argument 参数指针为空。
	\sa SwitchEnvironmentUnchecked
	*/
	shared_ptr<Environment>
	SwitchEnvironment(const shared_ptr<Environment>&);

	//! \pre 断言：参数指针非空。
	PDefH(shared_ptr<Environment>, SwitchEnvironmentUnchecked,
		const shared_ptr<Environment>& p_env) ynothrowv
		ImplRet(YAssertNonnull(p_env), ystdex::exchange(p_record, p_env))
	//@}

	//! \since build 788
	YB_ATTR_nodiscard PDefH(shared_ptr<Environment>, ShareRecord, ) const
		ImplRet(GetRecordRef().shared_from_this())

	/*!
	\brief 顺序移除当前动作序列中的所有动作。
	\since build 892
	*/
	PDefH(void, UnwindCurrent, ) ynothrow
		ImplExpr(current.clear())

	//! \since build 823
	YB_ATTR_nodiscard PDefH(EnvironmentReference, WeakenRecord, ) const
		// TODO: Blocked. Use C++17 %weak_from_this to get more efficient
		//	implementation.
		ImplRet(ShareRecord())

	/*!
	\brief 取用于初始化环境以外的对象使用的分配器。
	\since build 851
	*/
	YB_ATTR_nodiscard YB_PURE PDefH(pmr::polymorphic_allocator<yimpl(byte)>,
		get_allocator, ) const ynothrow
		ImplRet(pmr::polymorphic_allocator<yimpl(byte)>(
			&GetMemoryResourceRef()))

	/*!
	\brief 缩小存储资源占用：清空缓存。
	\sa stashed
	\since build 892
	*/
	PDefH(void, shrink_to_fit, )
		ImplExpr(stashed.clear())

	/*!
	\pre 分配器相等。
	\since build 788
	*/
	YF_API friend void
	swap(ContextNode&, ContextNode&) ynothrow;
};


/*!
\brief 分配环境。
\relates Environment
\since build 847
*/
//@{
//! \since build 867
template<typename... _tParams>
inline shared_ptr<Environment>
AllocateEnvironment(const Environment::allocator_type& a, _tParams&&... args)
{
	return YSLib::allocate_shared<NPL::Environment>(a, yforward(args)...);
}
template<typename... _tParams>
inline shared_ptr<Environment>
AllocateEnvironment(ContextNode& ctx, _tParams&&... args)
{
	return NPL::AllocateEnvironment(ctx.GetBindingsRef().get_allocator(),
		yforward(args)...);
}
template<typename... _tParams>
inline shared_ptr<Environment>
AllocateEnvironment(TermNode& term, ContextNode& ctx, _tParams&&... args)
{
	const auto a(ctx.GetBindingsRef().get_allocator());

	yunused(term);
	YAssert(a == term.get_allocator(),
		"Allocator mismatch between term and context.");
	return NPL::AllocateEnvironment(a, yforward(args)...);
}
//@}

/*!
\brief 切换到参数指定的新创建的环境。
\relates ContextNode
\since build 838
*/
template<typename... _tParams>
inline shared_ptr<Environment>
SwitchToFreshEnvironment(ContextNode& ctx, _tParams&&... args) ynothrow
{
	return ctx.SwitchEnvironmentUnchecked(NPL::AllocateEnvironment(ctx,
		yforward(args)...));
}


/*!
\brief 构造并向绑定目标添加叶节点值。
\return 对应的值在构造前不存在。
\pre 断言：第二参数的数据指针非空。
\since build 838

设置被绑定对象的值数据成员并清空子项。
可访问 Environment::Bindings 、Environment 或 ContextNode 类型中指定名称的绑定。
因为直接替换被绑定对象，不需要检查目标是否和 TermNode 的正规表示一致。
*/
//@{
template<typename _type, typename... _tParams>
inline bool
EmplaceLeaf(Environment::BindingMap& m, string_view name, _tParams&&... args)
{
	YAssertNonnull(name.data());
	// XXX: The implementation is depended on the fact that %TermNode is simply
	//	an alias of %ValueNode and it is same to %BindingMap currently.
	// XXX: Allocators are not used on %ValueObject for performance in most
	//	cases.
	return ystdex::insert_or_assign(m, name,
		TermNode(std::allocator_arg, m.get_allocator(), NoContainer,
		in_place_type<_type>, yforward(args)...)).second;
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

//! \exception NPLException 访问共享重定向上下文失败。
//@{
/*!
\brief 解析名称：处理保留名称并查找名称。
\pre 断言：第二参数的数据指针非空。
\sa ContextNode::Resolve
\since build 821

解析指定上下文的当前环境中的名称。
解析名称以当前当前环境作为参数调用上下文的 ContextNode::Resolve 实现。
*/
YB_ATTR_nodiscard inline PDefH(Environment::NameResolution, ResolveName,
	const ContextNode& ctx, string_view id)
	ImplRet(YAssertNonnull(id.data()), ctx.Resolve(ctx.GetRecordRef(), id))

/*!
\brief 解析标识符：解析名称并折叠引用。
\pre 间接断言：第二参数的数据指针非空。
\return 标识符指称的对象的引用。
\throw BadIdentifier 标识符未在环境中找到。
\sa PrepareCollapse
\sa ResolveName
\since build 876

解析指定上下文的当前环境中的标识符，若不存在绑定则抛出异常。
调用 ResolveName ，并调用 PrepareCollapse 进行引用折叠。
若无法在指定的环境中找到标识符的绑定，则抛出异常。
解析的结果中的对象以引用在项中表达，以避免使用对象副本。
*/
YB_ATTR_nodiscard YF_API TermNode
ResolveIdentifier(const ContextNode&, string_view);
//@}

/*!
\brief 解析环境。
\return 取得所有权的环境指针及是否具有所有权。
\note 只支持宿主值类型 \c shared_ptr<Environment> 或 \c weak_ptr<Environment> 。
*/
//@{
//! \since build 830
YB_ATTR_nodiscard YF_API pair<shared_ptr<Environment>, bool>
ResolveEnvironment(const ValueObject&);
/*!
\throw ListTypeError 参数是列表节点。
\note 当前使用和 TermToStringWithReferenceMark 相同的方式在异常消息中表示引用项。
\sa TermToStringWithReferenceMark
\since build 840
*/
YB_ATTR_nodiscard YF_API pair<shared_ptr<Environment>, bool>
ResolveEnvironment(const TermNode& term);
//@}


/*!
\brief 环境切换器。
\warning 非虚析构。
\sa Environment::SwitchEnvironmentUnchecked
\since build 821

类 NPL::EnvironmentSwitcher 用于保存切换上下文的当前环境的结果。
配合作用域守卫可用于以异常中立的方式恢复被切换的环境。
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
\brief 异步规约指定动作和上下文中的当前动作。
\return ReductionStatus::Partial 。
\since build 841

异步规约参数指定的动作作为当前动作序列的前缀。
第一和第二参数分别为上下文和捕获的当前动作。
以参数声明的相反顺序捕获参数作为动作，结果以参数声明的顺序析构捕获的动作。
*/
template<typename _fCurrent>
inline ReductionStatus
RelaySwitched(ContextNode& ctx, _fCurrent&& cur)
{
	ctx.SetupFront(yforward(cur));
	return ReductionStatus::Partial;
}

} // namespace NPL;

#endif

