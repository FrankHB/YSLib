/*
	© 2012-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Lexical.h
\ingroup NPL
\brief NPL 词法处理。
\version r1413
\author FrankHB <frankhb1989@gmail.com>
\since build 335
\par 创建时间:
	2012-08-03 23:04:28 +0800
\par 修改时间:
	2014-10-15 09:28 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::Lexical
*/


#ifndef YF_INC_NPL_Lexical_h_
#define YF_INC_NPL_Lexical_h_ 1

#include "YModules.h"
#include YFM_YSLib_Adaptor_YContainer
#include <cctype> // for std::isgraph;

namespace NPL
{

//! \since build 329
//@{
using ystdex::byte;
using YSLib::list;
using YSLib::string;
//@}
//! \since build 545
using YSLib::vector;
//! \since build 546
using YSLib::begin;
//! \since build 546
using YSLib::end;


/*!
\brief 反转义上下文。
\since build 545
*/
class YF_API UnescapeContext
{
public:
	//! \brief 转义序列前缀。
	string Prefix;

private:
	//! \brief 有效转义序列。
	string sequence;

public:
	DefDeCtor(UnescapeContext)
	DefDeCopyCtor(UnescapeContext)
	DefDeMoveCtor(UnescapeContext)

	DefDeCopyAssignment(UnescapeContext)
	DefDeMoveAssignment(UnescapeContext)

	DefPred(const ynothrow, Handling, !Prefix.empty())
	PDefH(bool, IsHandling, const string& prefix) const
		ImplRet(Prefix == prefix)

	DefGetter(const ynothrow, const string&, Sequence, sequence)

	PDefH(void, Clear, ) ynothrow
		ImplExpr(Prefix.clear(), sequence.clear())

	string
	Done();

	PDefH(bool, PopIf, byte uc)
		ImplRet(PopIf(char(uc)))
	PDefH(bool, PopIf, char c)
		ImplRet(!sequence.empty() && sequence.back() == c
			? (sequence.pop_back(), true) : false)
	PDefH(void, Push, byte uc)
		ImplExpr(Push(char(uc)))
	PDefH(void, Push, char c)
		ImplExpr(sequence += c)
};


/*!
\brief 设置反斜杠转义前缀：当输入 '\\' 时设置前缀为 "\\" 。
\sa LexicalAnalyzer::PrefixHandler
\since build 546
*/
YF_API bool
HandleBackslashPrefix(char, string&);

/*!
\brief NPL 转义匹配算法。
\sa LexicalAnalyzer::Unescaper
\since build 545

支持转义序列为 "\\" 、 "\a" 、 "\b" 、 "\f" 、 "\n" 、 "\r" 、 "\t" 和 "\v" 。
除以下说明外，转义序列语义参见 ISO C++11 （排除 raw-string-literal ）；
	其它转义序列由派生实现定义。
除了反斜杠转义外，其它转义仅适用于字面量。
反斜杠转义：连续两个反斜杠被替换为一个反斜杠；
引号转义：反斜杠之后紧接单引号或双引号时，反斜杠会被删除。
*/
YF_API bool
NPLUnescape(string&, const UnescapeContext&, char);


/*!
\brief 词法分析器。
\pre 输入兼容单字节的基本字符集的编码，如基本字符集为 ASCII 时分析 UTF-8 文本。
\post 解析中间结果除了字面量外不存在空格以外的空白符，但可能有连续的空格。
\since build 329

以字节为基本单位的词法分析器。
接受字节输入迭代器的输入，结果存放于 string 中。
基本字符集的字符的值保证在区间 [0, 0x7F) 内。
可接受的单字符词法分隔符都保证在基本字符集内。
输出规则（按优先顺序）：
断行连接：反斜杠之后紧接换行符的双字符序列视为续行符，会被删除；
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
public:
	/*!
	\brief 转移序列前缀处理器。
	\note 参数为当前处理的字符和反转义上下文的转义序列前缀引用。
	\note 返回值表示是否修改了前缀。
	\since build 546
	*/
	using PrefixHandler = std::function<bool(char, string&)>;
	/*!
	\brief 指定匹配转义序列的反转义算法：解析转义序列并按需修改指定缓存。
	\note 参数表示输出缓存、反转义上下文和当前正在处理的边界字符（如引号）。
	\note 返回值表示是否修改了输出缓存。
	\since build 545
	*/
	using Unescaper
		= std::function<bool(string&, const UnescapeContext&, char)>;

private:
	/*!
	\brief 指示当前续行符状态。
	\since build 545
	*/
	char line_concat = {};
	/*!
	\brief 反转移上下文：储存反转义中间结果。
	\note 若前缀非空表示正在处理反转义。
	\since build 545
	*/
	UnescapeContext unescape_context;
	/*!
	\brief 字面分隔符状态：表示正在处理字面量中的有效字符。
	\note 值为 NUL 时表示当前不处理字面量。
	*/
	char ld;
	/*!
	\brief 字符解析中间结果。
	*/
	string cbuf;
	/*!
	\brief 字符解析中间结果中非转义的引号出现的位置的有序列表。
	\since build 545
	*/
	vector<size_t> qlist;

public:
	LexicalAnalyzer();
	//! \since build 546
	//@{
	DefDeCopyCtor(LexicalAnalyzer)
	DefDeMoveCtor(LexicalAnalyzer)

	DefDeCopyAssignment(LexicalAnalyzer)
	DefDeMoveAssignment(LexicalAnalyzer)
	//@}

	DefGetter(const ynothrow, const string&, Buffer, cbuf)
	//! \since build 545
	//@{
	DefGetter(const ynothrow, const vector<size_t>&, Quotes, qlist)

private:
	bool
	CheckEscape(byte, Unescaper);

	bool
	CheckLineConcatnater(char, char = '\\', char = '\n');
	//@}

	//! \since build 546
	bool
	FilterForParse(char, Unescaper, PrefixHandler);

public:
	/*
	\warning 在同一个分析器对象上混用多种反转义算法的结果未指定。
	\warning 在同一个分析器对象上混用 ParseByte 和 ParseQuoted 的结果未指定。
	\since build 546
	*/
	//@{
	//! \brief 解析单个字符并添加至字符解析结果。
	void
	ParseByte(byte, Unescaper = NPLUnescape,
		PrefixHandler = HandleBackslashPrefix);

	//! \brief 解析单个字面量字符并添加至字符解析结果：反转义以外无视边界字符。
	void
	ParseQuoted(byte, Unescaper = NPLUnescape,
		PrefixHandler = HandleBackslashPrefix);
	//@}

	/*!
	\brief 根据中间结果取字符串列表。
	\note 其中每一项是完整的字面量或非字面量。
	*/
	list<string>
	Literalize() const;
};


/*!
\brief 检查指定字符串是否为字面量。
\return 若为字面量（首尾字符都为 '\'' 或 '"' 之一），则为首字符，否则为 char() 。
\since build 304
\todo 实现 UTF-8 字符串末尾兼容性。
*/
YF_API char
CheckLiteral(const string&);

/*!
\brief 去除字面量边界分隔符。
\return 若首尾字符都为 '\'' 或 '"' 之一，则为去除首尾字符之后的副本；否则为原串。
\since build 343
*/
YF_API string
Deliteralize(const string&);

/*!
\brief 反转义字符串：替换指定字符串中的可转义字符为转义序列。
\sa LexicalAnalyzer
\since build 545
*/
YF_API string
Escape(const string&);

/*!
\brief 反转义字符串字面量。
\return 若参数是字符串字面量时转义其中的内容，否则为原串。
\note 使用 Escape 转义。
\note 若转义后最后一个字符为 '\\' 则添加一个 '\\' 以避免转义末尾分隔符。
\sa LexicalAnalyzer
\since build 545
*/
YF_API string
EscapeLiteral(const string&);


/*!
\brief 判断是否为 NPL 图形分隔符。
\since build 331
*/
yconstfn bool
IsGraphicalDelimeter(char c)
{
//	return std::ispunct(c);
	return c == '(' || c == ')' || c == ':' || c == ',' || c == ';'
		|| c == '#' || c == '%' || c == '!';
}

/*!
\brief 判断是否为 NPL 分隔符。
\since build 331
*/
yconstfn bool
IsDelimeter(char c)
{
	return byte(c) < 0x80 && (!std::isgraph(c) || IsGraphicalDelimeter(c));
}


/*!
\brief 分解字符串为记号。
\since build 335
*/
YF_API list<string>
Decompose(const string&);

/*!
\brief 记号化：提取字符串列表中的记号。
\note 排除字面量，分解其余字符串为记号列表。
\since build 301
*/
YF_API list<string>
Tokenize(const list<string>&);

} // namespace NPL;

#endif

