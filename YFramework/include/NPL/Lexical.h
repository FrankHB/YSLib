/*
	© 2012-2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Lexical.h
\ingroup NPL
\brief NPL 词法处理。
\version r1859
\author FrankHB <frankhb1989@gmail.com>
\since build 335
\par 创建时间:
	2012-08-03 23:04:28 +0800
\par 修改时间:
	2020-03-27 13:21 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::Lexical
*/


#ifndef YF_INC_NPL_Lexical_h_
#define YF_INC_NPL_Lexical_h_ 1

#include "YModules.h"
#include YFM_NPL_NPL // for std::string, byte, CHAR_MIN;
#include YFM_YSLib_Adaptor_YTextBase // for YSLib::begin, YSLib::end,
//	YSLib::function, YSLib::list, YSLib::set, YSLib::string, YSLib::string_view,
//	YSLib::vector, YSLib::make_string_view;
#include <cctype> // for std::isgraph;

namespace NPL
{

//! \since build 861
namespace pmr = YSLib::pmr;
//! \since build 546
using YSLib::begin;
//! \since build 546
using YSLib::end;
//! \since build 851
using YSLib::function;
//! \since build 329
using YSLib::list;
//! \since build 806
using YSLib::set;
//! \since build 329
using YSLib::string;
//! \since build 659
using YSLib::string_view;
//! \since build 545
using YSLib::vector;

/*!
\brief 记号列表：不需要提供分配器的短字符串列表。
\since build 304
*/
using TokenList = list<yimpl(std::string)>;
/*!
\brief 记号视图列表：保存字符串中间结果的列表。
\since build 886
*/
using TokenViewList = vector<string_view>;


/*!
\brief 反转义上下文。
\note 因为通常情形使用短字符串，不提供分配器接口。
\since build 545
*/
class YF_API UnescapeContext
{
public:
	/*!
	\note 上下文处理不直接修改，一般由转移序列前缀处理器设置。
	\since build 886
	\sa LexicalAnalyzer
	*/
	//@{
	/*!
	\brief 转义序列起始位置。
	\note 若转义序列长度为 0 ，则值未指定；否则指定外部缓冲区中前缀起始的位置。
	*/
	size_t Start = size_t(-1);
	//! \brief 转义序列长度。
	size_t Length = 0;
	//@}

public:
	//! \post <tt>Prefix.empty() && sequence.empty()</tt> 。
	DefDeCtor(UnescapeContext)
	DefDeCopyMoveCtorAssignment(UnescapeContext)

	DefPred(const ynothrow, Handling, Length != 0)

	//! \brief 清除反转义上下文中的转义序列状态。
	PDefH(void, Clear, ) ynothrow
		ImplExpr(Length = 0)

	//! \since build 886
	PDefH(void, VerifyBufferLength, size_t len) const
		ImplExpr(yunused(len), YAssert(Length == 0 || Start + Length <= len,
			"Invalid unescaping state found."))
};


//! \since build 886
//@{
/*!
\pre 断言：第一参数的数据指针非空。
\sa LexicalAnalyzer
*/
//@{
//! \brief 设置反斜杠转义前缀：当输入 '\\' 时设置前缀为 "\\" 。
YB_ATTR_nodiscard YF_API bool
HandleBackslashPrefix(string_view, UnescapeContext&);

//! \brief 忽略前缀。
YB_ATTR_nodiscard YB_PURE yconstfn
	PDefH(bool, IgnorePrefix, string_view buf, UnescapeContext&) ynothrow
	ImplRet(yunused(buf), YAssert(!buf.empty(), "Invalid buffer found."), false)
//@}

/*!
\brief NPL 转义匹配算法。
\pre 断言：第一参数非空。
\sa LexicalAnalyzer

假定转义序列前缀为反斜杠，但不检查内容。
支持转义序列为 "\\" 、 "\a" 、 "\b" 、 "\f" 、 "\n" 、 "\r" 、 "\t" 和 "\v" ，
	以及断行连接。
除以下说明外，转义序列语义参见 ISO C++11 （排除 raw-string-literal ）；
	其它转义序列由派生实现定义。
反斜杠转义：连续两个反斜杠被替换为一个反斜杠；
引号转义：反斜杠之后紧接单引号或双引号时，反斜杠被删除。
断行连接：反斜杠之后紧接换行符的双字符序列视为续行符，被删除使分隔的行组成逻辑行。
注意断行连接和 ISO C++ 单独先行处理不同，不支持连接在不同行的转义序列。
*/
YB_ATTR_nodiscard YF_API bool
NPLUnescape(string&, char, UnescapeContext&, char);
//@}


/*!
\brief 词法分析器。
\pre 输入兼容单字节的基本字符集的编码，如基本字符集为 ASCII 时分析 UTF-8 文本。
\post 解析中间结果除了字面量外不存在空格以外的空白符，但可能有连续的空格。
\since build 329

以字节为基本单位的词法分析器。
接受字节输入迭代器的输入，结果存放于字符串中。
基本字符集的字符的值保证在区间 [0, 0x7F) 内。
可接受的单字符词法分隔符都保证在基本字符集内。
输出规则（按优先顺序）：
转义：转义序列替换为被转义字符；
字面量：分析至未被转义的单引号或双引号后进入字面量分析状态，无视以下规则，
	直接逐字节输出原始输入，直至遇到对应的另一个引号。
窄字符空白符替换：单字节空格、水平/垂直制表符、换行符
	被替换为单一空格；回车符会被忽略；
原始输出：其它字符序列逐字节输出。
支持配置转义算法。默认实现参见 NPLUnescape 。
*/
class YF_API LexicalAnalyzer
{
private:
	/*!
	\brief 反转移上下文：储存反转义中间结果。
	\note 若前缀非空表示正在处理反转义。
	\since build 545
	*/
	UnescapeContext unescape_context{};
	/*!
	\brief 字面分隔符状态：表示正在处理字面量中的有效字符。
	\note 值为空字符时表示当前不处理字面量。
	*/
	char ld = {};
	/*!
	\brief 字符解析中间结果。
	\since build 886
	*/
	string cbuf{};
	/*!
	\brief 字符解析中间结果中非转义的引号出现的位置的有序列表。
	\since build 545
	*/
	vector<size_t> qlist{};

public:
	LexicalAnalyzer();
	//! \since build 861
	LexicalAnalyzer(pmr::memory_resource&);
	//! \since build 546
	DefDeCopyMoveCtorAssignment(LexicalAnalyzer)

	//! \since build 886
	DefGetter(const ynothrow, const string&, Buffer, cbuf)
	//! \since build 886
	DefGetter(const ynothrow, char, Delimiter, ld)
	//! \since build 545
	DefGetter(const ynothrow, const vector<size_t>&, Quotes, qlist)

	/*!
	\brief 过滤解析的字符。
	\pre 参数满足以下签名和行为的要求的可调用对象。
	\return 字符未被转义处理。
	\since build 886

	检查转义序列并处理转义序列前缀。
	第二和第三参数分别指定反转义算法和转义序列前缀处理器；
		省略时，默认分别对应 NPLUnescape 和 HandleBackslashPrefix 。
	第二参数指定匹配转义序列的反转义算法：解析转义序列并按需修改指定缓存。
	第二参数满足调用签名 bool(string&, char, UnescapeContext&, char) ，其中：
		参数表示输出缓存、当前字符、反转义上下文和当前正在处理的边界字符（如引号）；
		返回值表示是否匹配转义序列。
	反转义算法按反转义上下文指定的转义序列前缀长度确定是否需要匹配转义序列，
		进行反转义。
	若需要反转义，使用第二参数指定的字符作为结尾构成新的转义序列：
		若新的转义序列完整匹配，则替换反转义结果到缓冲区，
			并清除反转义上下文中的转义序列状态；
		否则，向缓冲区结尾追加字符，
			并在新的转义序列完全不匹配时清除反转义上下文中的转义序列状态；
	否则，直接向缓冲区结尾追加字符。
	清除反转义上下文中的转义序列状态允许重新匹配新的转义序列。
	第三参数指定转义序列前缀处理器。
	第三参数满足调用签名 bool(string_view, UnescapeContext&) ，其中：
		参数为当前处理的缓冲区和反转义上下文；
		返回值表示缓冲区结尾是否更新了前缀。
	序列前缀处理器处理缓冲区的序列前缀，以反转义上下文中的前缀长度确定是否存在前缀：
		若不存在前缀，则判断缓冲区结尾是否是否构成前缀；
		若发现前缀，则更新反转义上下文中的前缀起始位置和长度。
	序列前缀处理器的参数指定的缓冲区满足：
		数据指针非空；
		数据指定空串，或其结尾应已经添加前缀字符。
	*/
	template<typename _fUnescape = decltype(NPLUnescape),
		typename _fHandlerPrefix = decltype(HandleBackslashPrefix)>
	YB_ATTR_nodiscard bool
	FilterChar(char c, _fUnescape unescape = NPLUnescape,
		_fHandlerPrefix handle_prefix = HandleBackslashPrefix)
	{
		// XXX: Incomplete escape sequence at end is ignored at current.
		if(!unescape_context.IsHandling())
		{
			cbuf += c;
			return !handle_prefix(cbuf, unescape_context);
		}
		unescape(cbuf, c, unescape_context, ld);
		return {};
	}

	/*!
	\note 参数指定反转义算法。
	\warning 在同一个分析器对象上混用不等价的多种解析方法或反转义算法的结果未指定。
	\since build 588
	*/
	//@{
	/*!
	\brief 解析单个字符并添加至字符解析结果。
	\note 记录引号并忽略空字符。
	*/
	template<typename... _tParams>
	inline void
	ParseByte(char c, _tParams&&... args)
	{
		if(FilterChar(c, yforward(args)...))
			ReplaceBack(c);
	}

	//! \brief 解析单个字面量字符并添加至字符解析结果：反转义以外无视边界字符。
	template<typename... _tParams>
	inline void
	ParseQuoted(char c, _tParams&&... args)
	{
		yunused(FilterChar(c, yforward(args)...));
	}

	/*!
	\brief 直接添加字符。
	\since build 763
	*/
	PDefH(void, ParseRaw, char c)
		ImplExpr(cbuf += c);
	//@}

	/*!
	\brief 根据中间结果取字符串列表。
	\note 其中每一项是引用字符解析中间结果的完整的字面量或非字面量。
	\warning 结果应在修改字符解析中间结果的操作前使用，否则失效访问引起未定义行为。
	\since build 886
	*/
	TokenViewList
	Literalize() const;

	/*!
	\brief 替换缓冲区最后的单个字节的字符解析结果。
	\pre 断言：缓冲区非空。
	\note 假定缓冲区最后一个字符等于参数。
	\note 解析时处理引号和空白符。空格和回车符以外的空白符解析为空格。
	\since build 886
	*/
	void
	ReplaceBack(char);
};


/*!
\pre 断言：字符串参数的数据指针非空。
\since build 659
*/
//@{
/*!
\brief 检查指定字符串是否为字面量。
\return 若为字面量（首尾字符都为 '\'' 或 '"' 之一），则为首字符，否则为 char() 。
*/
YB_ATTR_nodiscard YF_API YB_PURE char
CheckLiteral(string_view) ynothrowv;

/*!
\brief 去除字面量边界分隔符。
\pre 断言：参数的数据指针非空。
\return 若标识符是字面量，则为去除首尾字符之后的副本；否则为原串。
*/
//@{
//! \note 使用 CheckLiteral 判断。
YB_ATTR_nodiscard YF_API string_view
Deliteralize(string_view) ynothrowv;

//! \since build 731
YB_ATTR_nodiscard inline
	PDefH(string_view, DeliteralizeUnchecked, string_view sv) ynothrowv
	ImplRet(YAssertNonnull(sv.data()), ystdex::get_mid(sv))
//@}

/*!
\brief 编码转义字符串：替换指定字符串中的可转义字符为转义序列。
\sa LexicalAnalyzer
*/
YB_ATTR_nodiscard YF_API string
Escape(string_view);

/*!
\brief 编码转义字符串字面量。
\return 若参数是字符串字面量时转义其中的内容，否则为原串。
\note 使用 Escape 转义。
\note 若转义后最后一个字符为 '\\' 则添加一个 '\\' 以避免转义末尾分隔符。
\sa LexicalAnalyzer
*/
YB_ATTR_nodiscard YF_API string
EscapeLiteral(string_view);

/*!
\brief 编码 XML 字符串。
\see http://www.w3.org/TR/2006/REC-xml11-20060816/#charsets 。

允许 XML 1.1 字符，仅对空字符使用 YTraceDe 进行警告。
仅对 XML 1.0 和 XML 1.1 规定的有条件使用字符 \c & 、 \c < 和 \c > 生成转义序列。
*/
YB_ATTR_nodiscard YF_API string
EscapeXML(string_view);

/*!
\brief 修饰字符串为字面量。
\return 若首尾字符都为 '\'' 或 '"' 之一或第二参数为空字符则为原串，
	否则为首尾加上第二参数字符的串。
\since build 597
*/
YB_ATTR_nodiscard YF_API string
Literalize(string_view, char = '"');
//@}


//! \since build 825
//@{
//! \brief 判断参数是否为 NPL 图形分隔符。
YB_ATTR_nodiscard yconstfn PDefH(bool, IsGraphicalDelimiter, char c) ynothrow
	ImplRet(c == '(' || c == ')' || c == ',' || c == ';')

//! \brief 判断参数是否为 NPL 分隔符。
YB_ATTR_nodiscard inline PDefH(bool, IsDelimiter, char c) ynothrow
#if CHAR_MIN < 0
	ImplRet(c >= 0 && (!std::isgraph(c) || IsGraphicalDelimiter(c)))
#else
	ImplRet(!std::isgraph(c) || IsGraphicalDelimiter(c))
#endif
//@}


/*!
\brief 分解字符串为记号。
\pre 断言：字符串参数的数据指针非空。
\post 结果中字符串两端不包括 "C" 区域 \tt std::isspace 返回非零的字符。
\since build 886
*/
YB_ATTR_nodiscard YF_API TokenList
Decompose(string_view, TokenList::allocator_type = {});

/*!
\brief 记号化：提取字符串列表中的记号。
\note 排除字面量，分解其余字符串为记号列表。
\since build 886
*/
YB_ATTR_nodiscard YF_API TokenList
Tokenize(const TokenViewList&);

} // namespace NPL;

#endif

