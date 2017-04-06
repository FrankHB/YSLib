/*
	© 2014-2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA.h
\ingroup NPL
\brief NPLA 公共接口。
\version r1498
\author FrankHB <frankhb1989@gmail.com>
\since build 663
\par 创建时间:
	2016-01-07 10:32:34 +0800
\par 修改时间:
	2017-04-06 23:41 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA
*/


#ifndef NPL_INC_NPLA_h_
#define NPL_INC_NPLA_h_ 1

#include "YModules.h"
#include YFM_NPL_SContext // for string, NPLTag, ValueNode, TermNode,
//	LoggedEvent, ystdex::as_const;
#include <ystdex/base.h> // for ystdex::derived_entity;
#include YFM_YSLib_Core_YEvent // for YSLib::GHEvent, ystdex::fast_any_of,
//	ystdex::indirect, YSLib::GEvent, YSLib::GCombinerInvoker,
//	YSLib::GDefaultLastValueInvoker;
#include <ystdex/any.h> // for ystdex::any;

namespace NPL
{

/*!	\defgroup ThunkType Thunk Types
\brief 中间值类型。
\since build 753

标记特定求值策略，储存于 TermNode 的 Value 数据成员中不直接表示宿主语言对象的类型。
*/

//! \since build 598
using YSLib::pair;
//! \since build 598
using YSLib::to_string;
//! \since build 599
using YSLib::MakeIndex;
//! \since build 600
using YSLib::NodeSequence;
//! \since build 600
using YSLib::NodeLiteral;


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

/*!
\brief 打印 NPLA 节点。
\sa PrintIdent
\sa PrintNodeChild
\sa PrintNodeString

调用第四参数输出最后一个参数决定的缩进作为前缀和一个空格，然后打印节点内容：
先尝试调用 PrintNodeString 打印节点字符串，若成功直接返回；
否则打印换行，然后尝试调用 PrintNodeChild 打印 NodeSequence ；
再次失败则调用 PrintNodeChild 打印子节点。
调用 PrintNodeChild 打印后输出回车。
*/
YF_API void
PrintNode(std::ostream&, const ValueNode&, NodeToString = EscapeNodeLiteral,
	IndentGenerator = DefaultGenerateIndent, size_t = 0);

/*!
\brief 打印作为子节点的 NPLA 节点。
\sa IsPrefixedIndex
\sa PrintIdent
\sa PrintNodeString

调用第四参数输出最后一个参数决定的缩进作为前缀，然后打印子节点内容。
对满足 IsPrefixedIndex 的节点调用 PrintNodeString 作为节点字符串打印；
否则，调用 PrintNode 递归打印子节点，忽略此过程中的 std::out_of_range 异常。
*/
YF_API void
PrintNodeChild(std::ostream&, const ValueNode&, NodeToString
	= EscapeNodeLiteral, IndentGenerator = DefaultGenerateIndent, size_t = 0);

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
	NPLException(const YSLib::string_view sv,
		YSLib::RecordLevel lv = YSLib::Err)
		: LoggedEvent(sv, lv)
	{}
	DefDeCtor(NPLException)

	//! \brief 虚析构：类定义外默认实现。
	~NPLException() override;
};


/*!
\brief 列表规约失败。
\since build 692
\todo 捕获并保存上下文信息。
*/
class YF_API ListReductionFailure : public NPLException
{
public:
	//! \since build 692
	using NPLException::NPLException;
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
\brief 参数匹配失败。
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
class YF_API BadIdentifier : public NPLException
{
private:
	YSLib::shared_ptr<string> p_identifier;

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
//@}


/*!
\brief 规约状态：一遍规约可能的中间结果。
\since build 730
*/
enum class ReductionStatus : yimpl(size_t)
{
	//! \since build 757
	//@{
	//! \brief 规约成功终止且不需要保留子项。
	Clean = 0,
	//! \brief 规约成功但需要保留子项。
	Retained,
	//! \brief 需要重规约。
	Retrying
	//@}
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

//! \since build 674
//@{
//! \brief 上下文节点类型。
using ContextNode = yimpl(ValueNode);

//! \ingroup ThunkType
//@{
//! \brief 上下文处理器类型。
using ContextHandler = YSLib::GHEvent<ReductionStatus(TermNode&, ContextNode&)>;
//! \brief 字面量处理器类型。
using LiteralHandler = YSLib::GHEvent<ReductionStatus(const ContextNode&)>;
//@}

//! \brief 注册上下文处理器。
inline PDefH(void, RegisterContextHandler, ContextNode& node,
	const string& name, ContextHandler f)
	ImplExpr(node[name].Value = std::move(f))

//! \brief 注册字面量处理器。
inline PDefH(void, RegisterLiteralHandler, ContextNode& node,
	const string& name, LiteralHandler f)
	ImplExpr(node[name].Value = std::move(f))
//@}


//! \since build 770
//@{
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


//! \sa LexemeCategory
//@{
/*!
\pre 断言：字符串参数的数据指针非空且字符串非空。
\return 判断的非扩展字面量分类。
*/
//@{
/*!
\brief 对排除扩展字面量的词素分类。
\note 扩展字面量视为非字面量。
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
	ImplRet(std::isdigit(c) || IsNPLAExtendedLiteralNonDigitPrefix(c))

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
\brief 标记记号节点：递归变换节点，转换其中的词素为记号值。
\note 先变换子节点。
\since build 753
*/
YF_API void
TokenizeTerm(TermNode& term);


/*!
\brief 对表示值的 ValueObject 进行基于所有权的生存期检查并取表示其引用的间接值。
\throw NPLException 检查失败：参数具有对象的唯一所有权，不能被外部引用保存。
\throw ystdex::invalid_construction 参数不持有值。
\since build 760
\todo 使用具体的语义错误异常类型。
*/
YF_API ValueObject
ReferenceValue(const ValueObject&);


//! \brief 从指定上下文查找名称对应的节点。
//@{
//! \since build 740
template<typename _tKey>
inline observer_ptr<ValueNode>
LookupName(ContextNode& ctx, const _tKey& id) ynothrow
{
	return YSLib::AccessNodePtr(ctx, id);
}
//! \since build 730
template<typename _tKey>
inline observer_ptr<const ValueNode>
LookupName(const ContextNode& ctx, const _tKey& id) ynothrow
{
	return YSLib::AccessNodePtr(ctx, id);
}
//@}

/*!
\brief 从指定上下文取指定名称指称的值。
\since build 730
*/
template<typename _tKey>
ValueObject
FetchValue(const ContextNode& ctx, const _tKey& name)
{
	return GetValueOf(NPL::LookupName(ctx, name));
}

/*!
\brief 从指定上下文取指定名称指称的值的指针。
\since build 749
*/
template<typename _tKey>
observer_ptr<const ValueObject>
FetchValuePtr(const ContextNode& ctx, const _tKey& name)
{
	return GetValuePtrOf(NPL::LookupName(ctx, name));
}


/*!
\brief 访问项的值作为记号。
\return 通过访问项的值取得的记号的指针，或空指针表示无法取得名称。
\sa TokenValue
\since build 732
*/
YF_API observer_ptr<const string>
TermToNamePtr(const TermNode&);


//! \since build 753
//@{
/*!
\pre 字符串参数的数据指针非空。
\note 最后一个参数表示强制调用。
\warning 应避免对被替换或移除的值的悬空引用。
\throw BadIdentifier 非强制调用时发现标识符不存在或冲突。
\since build 731
*/
//@{
//! \brief 以字符串为标识符在指定上下文中定义值。
YF_API void
DefineValue(ContextNode&, string_view, ValueObject&&, bool);

/*!
\brief 以字符串为标识符在指定上下文中覆盖定义值。
\since build 732
*/
YF_API void
RedefineValue(ContextNode&, string_view, ValueObject&&, bool);

//! \brief 以字符串为标识符在指定上下文移除对象。
YF_API void
RemoveIdentifier(ContextNode&, string_view, bool);
//@}


/*!
\brief 根据规约状态检查是否可继续规约。
\see YTraceDe
\since build 734

只根据输入状态确定结果。当且仅当规约成功时不视为继续规约。
若发现不支持的状态视为不成功，输出警告。
*/
YB_PURE YF_API bool
CheckReducible(ReductionStatus);

/*!
\brief 检查视为范式的节点并提取规约状态。
\since build 769
*/
inline PDefH(ReductionStatus, CheckNorm, const TermNode& term) ynothrow
	ImplRet(IsBranch(term) ? ReductionStatus::Retained : ReductionStatus::Clean)

/*!
\sa CheckReducible
\since build 735
*/
template<typename _func, typename... _tParams>
void
CheckedReduceWith(_func f, _tParams&&... args)
{
	ystdex::retry_on_cond(CheckReducible, f, yforward(args)...);
}


//! \since build 676
//@{
/*!
\brief 遍合并器：逐次调用直至成功。
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
			if(r == ReductionStatus::Retained)
				res = r;
			return r == ReductionStatus::Retrying;
		}) ? ReductionStatus::Retrying : res;
	}
};


/*!
\note 结果表示判断是否应继续规约。
\sa PassesCombiner
*/
//@{
//! \brief 一般合并遍。
template<typename... _tParams>
using GPasses = YSLib::GEvent<ReductionStatus(_tParams...),
	YSLib::GCombinerInvoker<ReductionStatus, PassesCombiner>>;
//! \brief 项合并遍。
using TermPasses = GPasses<TermNode&>;
//! \brief 求值合并遍。
using EvaluationPasses = GPasses<TermNode&, ContextNode&>;
/*!
\brief 字面量合并遍。
\pre 字符串参数的数据指针非空。
\since build 738
*/
using LiteralPasses = GPasses<TermNode&, ContextNode&, string_view>;
//@}


//! \brief 作用域守护类型。
using Guard = ystdex::any;
/*!
\brief 作用域守护遍：用于需在规约例程的入口和出口关联执行的操作。
\todo 支持迭代使用旧值。
*/
using GuardPasses = YSLib::GEvent<Guard(TermNode&, ContextNode&),
	YSLib::GDefaultLastValueInvoker<Guard>>;


//! \brief 调用处理遍：从指定名称的节点中访问指定类型的遍并以指定上下文调用。
//@{
//! \since build 777
template<class _tPasses, typename... _tParams>
typename _tPasses::result_type
InvokePasses(observer_ptr<const ValueNode> p, TermNode& term, ContextNode& ctx,
	_tParams&&... args)
{
	return ystdex::call_value_or([&](const _tPasses& passes){
		// XXX: Blocked. 'yforward' cause G++ 5.3 crash: internal compiler
		//	error: Segmentation fault.
		return passes(term, ctx, std::forward<_tParams>(args)...);
	}, YSLib::AccessPtr<const _tPasses>(p));
}
//! \since build 738
template<class _tPasses, typename... _tParams>
inline typename _tPasses::result_type
InvokePasses(const string& name, TermNode& term, ContextNode& ctx,
	_tParams&&... args)
{
	return NPL::InvokePasses<_tPasses>(YSLib::AccessNodePtr(
		ystdex::as_const(ctx), name), term, ctx, yforward(args)...);
}
//@}


/*!
\brief 调整指定值对象的指针为 TermNode 类型的值。
\return 若成功则为调整后的指针，否则为指向原值对象的指针。
\since build 750
*/
//@{
inline PDefH(observer_ptr<const ValueObject>, AdjustTermValuePtr,
	observer_ptr<const TermNode> p_term)
	ImplRet(ystdex::nonnull_or(GetValuePtrOf(p_term)))
inline PDefH(observer_ptr<const ValueObject>, AdjustTermValuePtr,
	observer_ptr<const ValueObject> p_vo)
	ImplRet(ystdex::nonnull_or(
		AdjustTermValuePtr(YSLib::AccessPtr<const TermNode>(p_vo)), p_vo))
template<typename _tKey>
observer_ptr<const ValueObject>
AdjustTermValuePtr(const ContextNode& ctx, const _tKey& name)
{
	return AdjustTermValuePtr(NPL::FetchValuePtr(ctx, name));
}
//@}

/*!
\brief 附加子项：在第一个参数指定的项的索引后插入子项。
\sa MakeIndex
\since build 779
*/
inline PDefH(void, AppendTerm, TermNode& term, TermNode& tm)
	ImplExpr(term.AddChild(MakeIndex(term), std::move(tm)))

//! \brief 提升项：使用第二个参数指定的项的内容替换第一个项的内容。
//@{
YF_API void
LiftTerm(TermNode&, TermNode&);
//! \since build 745
inline PDefH(void, LiftTerm, ValueObject& term_v, ValueObject& vo)
	ImplExpr(term_v = std::move(vo))
//! \since build 745
inline PDefH(void, LiftTerm, TermNode& term, ValueObject& vo)
	ImplExpr(LiftTerm(term.Value, vo))
//@}

/*!
\brief 提升项：使用第二个参数指定的项的内容引用替换第一个项的内容。
\since build 747
*/
//@{
inline PDefH(void, LiftTermRef, TermNode& term, TermNode& tm)
	ImplExpr(term.SetContentIndirect(tm))
inline PDefH(void, LiftTermRef, ValueObject& term_v, const ValueObject& vo)
	ImplExpr(term_v = vo.MakeIndirect())
inline PDefH(void, LiftTermRef, TermNode& term, const ValueObject& vo)
	ImplExpr(LiftTermRef(term.Value, vo))
//@}
//@}

/*!
\brief 提升延迟项。
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
	ImplExpr(IsBranch(term), LiftTerm(term, Deref(term.begin())))

/*!
\brief 提升末项：使用最后一个子项替换项的内容。
\since build 696
*/
inline PDefH(void, LiftLast, TermNode& term)
	ImplExpr(IsBranch(term), LiftTerm(term, Deref(term.rbegin())))
//@}


/*!
\since build 774
\sa RemoveHead
\note 使用 ADL RemoveHead 。
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

/*!
\brief 规约为列表：对枝节点移除第一个子项，保留余下的子项作为列表。
\return 若成功移除项 ReductionStatus::Retained ，否则为 ReductionStatus::Clean。
*/
YF_API ReductionStatus
ReduceToList(TermNode&) ynothrow;
//@}

} // namespace NPL;

#endif

