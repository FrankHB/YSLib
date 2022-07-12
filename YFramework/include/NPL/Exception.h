/*
	© 2016-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Exception.h
\ingroup NPL
\brief NPL 异常。
\version r11508
\author FrankHB <frankhb1989@gmail.com>
\since build 663
\par 创建时间:
	2022-01-21 01:59:22 +0800
\par 修改时间:
	2022-07-05 03:54 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::Exception
*/


#ifndef NPL_INC_Exception_h_
#define NPL_INC_Exception_h_ 1

#include "YModules.h"
#include YFM_NPL_SContext // for LoggedEvent, YSLib::RecordLevel, std::string,
//	shared_ptr, NPL::Deref, TermNode, type_info;

namespace NPL
{

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
	//! \since build 921
	NPLException(const std::string& str, YSLib::RecordLevel lv = YSLib::Err)
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
\brief 列表类型错误。
\note 预期列表或列表引用。
\since build 834
\todo 捕获并保存类型信息。

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


/*!
\brief 参数不匹配。
\since build 771
*/
class YF_API ParameterMismatch : public ListTypeError
{
public:
	//! \since build 902
	using ListTypeError::ListTypeError;
	//! \since build 845
	DefDeCopyCtor(ParameterMismatch)
	//! \brief 虚析构：类定义外默认实现。
	~ParameterMismatch() override;

	//! \since build 845
	DefDeCopyAssignment(ParameterMismatch)
};


/*!
\brief 元数不匹配。
\todo 支持范围匹配。

操作数子项个数不匹配的错误。
*/
class YF_API ArityMismatch : public ParameterMismatch
{
private:
	size_t expected;
	size_t received;

public:
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
	\brief 标识符关联的源代码信息。
	\since build 893
	*/
	SourceInformation Source{};

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
\note 非字符串的后三个参数传递给 TermToStringWithReferenceMark ，预期用法通常相同。
\sa TermToStringWithReferenceMark
\since build 949
*/
//@{
/*!
\brief 抛出缺少项的异常。
\throw ParameterMismatch 缺少项的错误。
*/
YB_NORETURN YF_API void
ThrowInsufficientTermsError(const TermNode&, bool, size_t = 0);

/*!
\brief 抛出对非有序对值预期列表类型的异常。
\throw ListTypeError 值不是有序对。
*/
YB_NORETURN YF_API void
ThrowListTypeErrorForAtom(const TermNode&, bool, size_t = 0);

/*!
\brief 对列表项抛出指定预期访问值的类型的异常。
\throw ListTypeError 消息中包含由参数指定的预期访问值的类型的异常。
*/
//@{
YB_NORETURN YF_API YB_NONNULL(1) void
ThrowListTypeErrorForInvalidType(const char*, const TermNode&, bool,
	size_t = 0);
YB_NORETURN YF_API void
ThrowListTypeErrorForInvalidType(const type_info&, const TermNode&, bool,
	size_t = 0);
//@}

/*!
\brief 抛出对非列表值预期列表类型的异常。
\throw ListTypeError 值不是列表。
*/
YB_NORETURN YF_API void
ThrowListTypeErrorForNonList(const TermNode&, bool, size_t = 0);

/*!
\brief 对项抛出指定预期访问值的类型的异常。
\throw TypeError 消息中包含由参数指定的预期访问值的类型的异常。
*/
//@{
YB_NORETURN YF_API YB_NONNULL(1,2) void
ThrowTypeErrorForInvalidType(const char*, const char*);
YB_NORETURN YF_API YB_NONNULL(1) void
ThrowTypeErrorForInvalidType(const char*, const TermNode&, bool, size_t = 0);
YB_NORETURN YF_API void
ThrowTypeErrorForInvalidType(const type_info&, const TermNode&, bool,
	size_t = 0);
//@}
//@}

} // namespace NPL;

#endif

