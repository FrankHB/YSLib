/*
	© 2012-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file SContext.h
\ingroup NPL
\brief S 表达式上下文。
\version r1502
\author FrankHB <frankhb1989@gmail.com>
\since build 304
\par 创建时间:
	2012-08-03 19:55:41 +0800
\par 修改时间:
	2016-02-27 01:26 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::SContext
*/


#ifndef NPL_INC_SContext_h_
#define NPL_INC_SContext_h_ 1

#include "YModules.h"
#include YFM_NPL_Lexical
#include YFM_YSLib_Core_ValueNode

namespace NPL
{

//! \since build 675
using YSLib::ValueObject;
//! \since build 335
using YSLib::ValueNode;
using TokenList = list<string>;
using TLIter = TokenList::iterator;
using TLCIter = TokenList::const_iterator;
//! \since build 674
//@{
using YSLib::observer_ptr;
using YSLib::LoggedEvent;

//! \brief 项节点：存储语法分析结果的值类型节点。
using TermNode = ValueNode;

using TNIter = TermNode::iterator;
using TNCIter = TermNode::const_iterator;

inline PDefH(TermNode&, MapToTermNode, TermNode& term)
	ImplRet(term)
inline PDefH(const TermNode&, MapToTermNode, const TermNode& term)
	ImplRet(term)

inline PDefH(ValueNode&, MapToValueNode, ValueNode& node)
	ImplRet(node)
inline PDefH(const ValueNode&, MapToValueNode, const ValueNode& node)
	ImplRet(node)
//@}


/*!
\brief 会话：分析指定 NPL 代码。
\since build 304
*/
class YF_API Session
{
public:
	//! \since build 546
	//@{
	using CharParser = std::function<void(LexicalAnalyzer&, char)>;

	//! \since build 592
	LexicalAnalyzer Lexer;

	//! \since build 618
	DefDeCtor(Session)
	//! \throw LoggedEvent 关键失败：无法访问源内容。
	template<typename _tIn>
	Session(_tIn first, _tIn last, CharParser parse = DefaultParseByte)
		: Lexer()
	{
		std::for_each(first, last,
			std::bind(parse, std::ref(Lexer), std::placeholders::_1));
	}
	template<typename _tRange,
		yimpl(typename = ystdex::exclude_self_ctor_t<Session, _tRange>)>
	Session(const _tRange& c, CharParser parse = DefaultParseByte)
		: Session(begin(c), end(c), parse)
	{}
	DefDeCopyMoveCtorAssignment(Session)

	DefGetterMem(const ynothrow, const string&, Buffer, Lexer)
	//@}
	DefGetter(const, TokenList, TokenList, Tokenize(Lexer.Literalize()))

	/*!
	\brief 默认字符解析实现：直接使用 LexicalAnalyzer::ParseByte 。
	\since build 546
	*/
	static void
	DefaultParseByte(LexicalAnalyzer&, char);

	/*!
	\brief 默认字符解析实现：直接使用 LexicalAnalyzer::ParseQuoted 。
	\since build 546
	*/
	static void
	DefaultParseQuoted(LexicalAnalyzer&, char);
};


/*!
\brief S 表达式上下文：处理 S 表达式。
\since build 330
*/
namespace SContext
{

/*!
\brief 遍历记号列表，验证基本合法性：圆括号是否对应。
\param b 起始迭代器。
\param e 终止迭代器。
\pre 迭代器是同一个记号列表的迭代器，其中 b 必须可解引用，且在 e 之前。
\return e 或指向冗余的 ')' 的迭代器。
\throw LoggedEvent 警报：找到冗余的 '(' 。
\since build 335
*/
YF_API TLCIter
Validate(TLCIter b, TLCIter e);

//! \since build 674
//@{
/*!
\brief 遍历规约记号列表，取抽象语法树储存至指定值类型节点。
\param term 项节点。
\param b 起始迭代器。
\param e 终止迭代器。
\pre 迭代器是同一个记号列表的迭代器，其中 b 必须可解引用，且在 e 之前。
\return e 或指向冗余的 ')' 的迭代器。
\throw LoggedEvent 警报：找到冗余的 '(' 。
*/
YF_API TLCIter
Reduce(TermNode& term, TLCIter b, TLCIter e);


/*!
\brief 分析指定源，取抽象语法树储存至指定值类型节点。
\throw LoggedEvent 警报：找到冗余的 ')' 。
*/
//@{
YF_API void
Analyze(TermNode&, const TokenList&);
YF_API void
Analyze(TermNode&, const Session&);
//! \pre 断言：字符串参数的数据指针非空。
YF_API void
Analyze(TermNode&, string_view);
//@}
//! \note 调用 ADL \c Analyze 分析节点。
template<typename _type>
TermNode
Analyze(const _type& arg)
{
	TermNode root;

	Analyze(root, arg);
	return root;
}
//@}

} // namespace SContext;

} // namespace NPL;

#endif

