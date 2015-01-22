/*
	© 2010-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YBaseMacro.h
\ingroup Core
\brief 通用基础设施：宏定义。
\version r2672
\author FrankHB <frankhb1989@gmail.com>
\since build 204
\par 创建时间:
	2010-10-09 09:25:27 +0800
\par 修改时间:
	2015-01-22 18:33 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YBaseMacro
*/


#ifndef YF_INC_YBaseMacro_h_
#define YF_INC_YBaseMacro_h_ 1

#include <ydef.h>

/*!	\defgroup cmacro Macros For Code Compressing
\brief 缩写代码用的宏。
\since 早于 build 132

以下名构词缩写的含义：
Ctor constructor
Cvt converter
De default
Decl declare
Def define
Del deleted
Dtor destructor
Expr expression
Fn function
Fwd forward
H head
I interface
Impl implement
Mem member
Op operator
P partially
PDecl pre-declare
Pred predicate
Ret returning
S statically
Tmpl template

以下宏参数缩写的含义：
_a argument
_alist arguments list
_attr attributes
_b base
_e expression
_i interface
_m member
_n name
_op operator
_p parameter
_plist parameters list
_q qualifier(s)
_sig signature
_t type
*/
//@{

//函数宏。

//! \brief 通用头定义。
#define PDefH(_t, _n, ...) \
	_t \
	_n(__VA_ARGS__)

//! \brief 操作符重载头定义。
#define PDefHOp(_t, _op, ...) \
	PDefH(_t, operator _op, __VA_ARGS__)

//! \brief 转换操作头定义。
#define PDefCvt(_t) \
	operator _t()


//! \brief 简单通用函数实现。
//@{
#define ImplExpr(...) \
	{ \
		(__VA_ARGS__), void(); \
	}
#define ImplRet(...) \
	{ \
		return __VA_ARGS__; \
	}
//! \since build 477
#define ImplThrow(...) \
	{ \
		throw __VA_ARGS__; \
	}
// NOTE: GCC complains about 'void(yunseq(__VA_ARGS__))'.
#define ImplUnseq(...) \
	{ \
		yunused(yunseq(__VA_ARGS__)); \
	}
//@}


//! \since build 547
//@{
//! \brief 指定表达式在 \c try 块中求值。
#define TryExpr(...) \
	try \
	ImplExpr(__VA_ARGS__)
//! \brief 指定表达式在 \c try 块中求值并返回。
#define TryRet(...) \
	try \
	ImplRet(__VA_ARGS__)

//! \brief 指定表达式作为异常处理器。
#define CatchExpr(_ex, ...) \
	catch(_ex) \
	ImplExpr(__VA_ARGS__)
//! \brief 指定异常处理器忽略指定异常。
#define CatchIgnore(_ex) \
	catch(_ex) \
	{}
//! \brief 指定返回语句作为异常处理器。
#define CatchRet(_ex, ...) \
	catch(_ex) \
	ImplRet(__VA_ARGS__)
//! \brief 指定抛出指定异常作为异常处理器。
#define CatchThrow(_ex, ...) \
	catch(_ex) \
	ImplThrow(__VA_ARGS__)
//@}


//! \brief 简单通用成员函数定义。
//@{
/*!
\brief 定义默认析构函数。
\note CWG defect 906 的解决禁止显式默认虚函数，但 CWG defect 1135
	的解决撤销了这一限制。 ISO C++11 最终没有此限制。
\note ISO C++11 不需要显式使用异常规范，可自动推导（参见 ISO C++11 12.4/3 ）。
	显式异常规范导致使用隐式继承需要保证成员函数的异常规范限制。
*/
#define DefDeCtor(_t) \
	_t() = default;
#define DefDelCtor(_t) \
	_t() = delete;

#define DefDeCopyCtor(_t) \
	_t(const _t&) = default;
#define DefDelCopyCtor(_t) \
	_t(const _t&) = delete;

#define DefDeMoveCtor(_t) \
	_t(_t&&) = default;
#define DefDelMoveCtor(_t) \
	_t(_t&&) = delete;

//! \since build 560
#define DefDeCopyMoveCtor(_t) \
	DefDeCopyCtor(_t) \
	DefDeMoveCtor(_t)

#define DefDeDtor(_t) \
	~_t() = default;
#define DefDelDtor(_t) \
	~_t() = delete;

#define ImplEmptyDtor(_t) \
	inline _t::DefDeDtor(_t)

#define DefDeCopyAssignment(_t) \
	_t& operator=(const _t&) = default;
#define DefDelCopyAssignment(_t) \
	_t& operator=(const _t&) = delete;

#define DefDeMoveAssignment(_t) \
	_t& operator=(_t&&) = default;
#define DefDelMoveAssignment(_t) \
	_t& operator=(_t&&) = delete;

//! \since build 560
#define DefDeCopyMoveAssignment(_t) \
	DefDeCopyAssignment(_t) \
	DefDeMoveAssignment(_t)

//! \since build 560
#define DefDeCopyMoveCtorAssignment(_t) \
	DefDeCopyMoveCtor(_t) \
	DefDeCopyMoveAssignment(_t)

#define DefCvt(_q, _t, ...) \
	operator _t() _q \
	ImplRet(__VA_ARGS__)
#define DefCvtBase(_q, _t, _b) \
	DefCvt(_q, _t, _b::operator _t())
#define DefCvtMem(_q, _t, _m) \
	DefCvt(_q, _t, (_m).operator _t())

/*!
\brief 定义表示否定的 \c operator! 。
\pre 表达式 <tt>bool(*this)</tt> 合式且无异常抛出。
\since build 559
*/
#define DefNeg \
	PDefHOp(bool, !, ) const ynothrow \
		ImplRet(!bool(*this))

/*!
\brief 定义表示显式转换 bool 操作符及其否定的 \c operator! 。
\pre 表达式合式且无异常抛出。
\since build 559
*/
#define DefBoolNeg(_spec, ...) \
	DefNeg \
	_spec DefCvt(const ynothrow, bool, __VA_ARGS__)

#define DefPred(_q, _n, ...) \
	bool YPP_Concat(Is, _n)() _q \
	ImplRet(__VA_ARGS__)
#define DefPredBase(_q, _n, _b) \
	DefPred(_q, _n, _b::YPP_Concat(Is, _n)())
#define DefPredMem(_q, _n, _m) \
	DefPred(_q, _n, (_m).YPP_Concat(Is, _n)())

#define DefGetter(_q, _t, _n, ...) \
	_t YPP_Concat(Get, _n)() _q \
	ImplRet(__VA_ARGS__)
#define DefGetterBase(_q, _t, _n, _b) \
	DefGetter(_q, _t, _n, _b::YPP_Concat(Get, _n)())
#define DefGetterMem(_q, _t, _n, _m) \
	DefGetter(_q, _t, _n, (_m).YPP_Concat(Get, _n)())

#define DefSetter(_t, _n, _m) \
	void YPP_Concat(Set, _n)(_t _tempArgName) \
	ImplExpr((_m) = _tempArgName)
#define DefSetterDe(_t, _n, _m, _defv) \
	void YPP_Concat(Set, _n)(_t _tempArgName = _defv) \
	ImplExpr((_m) = _tempArgName)
#define DefSetterBase(_t, _n, _b) \
	void YPP_Concat(Set, _n)(_t _tempArgName) \
	ImplExpr(_b::YPP_Concat(Set, _n)(_tempArgName))
#define DefSetterBaseDe(_t, _n, _b, _defv) \
	void YPP_Concat(Set, _n)(_t _tempArgName = _defv) \
	ImplExpr(_b::YPP_Concat(Set, _n)(_tempArgName))
#define DefSetterMem(_t, _n, _m) \
	void YPP_Concat(Set, _n)(_t _tempArgName) \
	ImplExpr((_m).YPP_Concat(Set, _n)(_tempArgName))
#define DefSetterMemDe(_t, _n, _m, _defv) \
	void YPP_Concat(Set, _n)(_t _tempArgName = _defv) \
	ImplExpr((_m).YPP_Concat(Set, _n)(_tempArgName))
#define DefSetterEx(_t, _n, _m, ...) \
	void YPP_Concat(Set, _n)(_t _tempArgName) \
	ImplExpr((_m) = (__VA_ARGS__))
#define DefSetterDeEx(_t, _n, _m, _defv, ...) \
	void YPP_Concat(Set, _n)(_t _tempArgName = _defv) \
	ImplExpr((_m) = (__VA_ARGS__))
//@}


/*!
\brief 动态复制。
\note 需要在满足 \c CopyConstructible 的类的定义内。
\note 如需要多态复制，需要显示前置 \c virtual 或加入 \c override 等指示符。
\since build 409
*/
#define DefClone(_q, _t) \
	PDefH(_t*, clone, ) _q \
		ImplRet(new _t(*this))


/*!
\brief 交换成员。
\note 需要对应类型具有接受两个左值引用的 swap 成员函数。
\since build 409
*/
#define DefSwap(_q, _t) \
	PDefH(void, swap, _t& _x, _t& _y) _q \
		ImplExpr(_x.swap(_y))


//成员函数和模板映射。


/*!
\brief 传递函数。
\since build 266
*/
#define DefFwdFn(_q, _t, _n, ...) \
	inline _t \
	_n() _q \
	{ \
		return (__VA_ARGS__); \
	}

/*!
\brief 传递模板。
\since build 266
*/
#define DefFwdTmpl(_q, _t, _n, ...) \
	template<typename... _tParams> \
	inline _t \
	_n(_tParams&&... args) _q \
	{ \
		return (__VA_ARGS__); \
	}


/*!	\defgroup InterfaceTypeMacros Interface Type Macros
\brief 接口类型宏。
\since build 161
*/
//@{
//! \since build 562
#define YInterface struct

#define implements public

/*!
\brief 定义接口类型头部。
\sa ImplEmptyDtor
\since build 562
*/
#define YInterfaceHead(_n) { \
protected: \
	DefDeCtor(_n) \
\
public: \
	virtual DefDeDtor(_n)

#define FwdDeclI(_n) YInterface _n;

/*!
\brief 定义接口类型。
\since build 362
*/
#define DeclI(_attr, _n) \
	YInterface _attr _n \
	YInterfaceHead(_n)

/*
\brief 定义派生接口类型。
\note 由于接口定义为 struct 类型，因此通常只需指定是否为 virtual 继承。
\since build 362
*/
#define DeclDerivedI(_attr, _n, ...) \
	YInterface _attr _n : __VA_ARGS__ \
	YInterfaceHead(_n)

//! \note ImplI = Implements Interface 。
#define ImplI(...) override

/*!
\brief 抽象实现：保留接口供派生类实现（可以提供接口函数的默认实现）。
\note ImplA = Implements Abstractly 。
*/
#define ImplA(...)

#define DeclIEntry(_sig) virtual _sig = 0;

#define EndDecl };


/*!
\brief 静态接口。
\since build 266
*/
#define DeclSEntry(...)
/*!
\brief 静态接口实现。
\since build 266
*/
#define ImplS(...)
//@}


/*!
\brief 定义直接派生类。
\note 仅构造函数。不定义成员，避免对象切片或其它成员存储泄漏问题。
\since build 352
*/
#define DefExtendClass(_attr, _n, ...) \
	class _attr _n : __VA_ARGS__ \
	{ \
	public: \
		_n(); \
	};


/*!
\brief 位掩码类型操作。
\note 参见 ISO C++11 17.5.2.1.3[bitmask.types] 。
\since build 270
*/
//@{
#define DefBitmaskAnd(_tBitmask, _tInt) \
	yconstfn _tBitmask operator&(_tBitmask _x, _tBitmask _y) \
		ImplRet(static_cast<_tBitmask>( \
			static_cast<_tInt>(_x) & static_cast<_tInt>(_y)))

#define DefBitmaskOr(_tBitmask, _tInt) \
	yconstfn _tBitmask operator|(_tBitmask _x, _tBitmask _y) \
		ImplRet(static_cast<_tBitmask>( \
			static_cast<_tInt>(_x) | static_cast<_tInt>(_y)))

#define DefBitmaskXor(_tBitmask, _tInt) \
	yconstfn _tBitmask operator^(_tBitmask _x, _tBitmask _y) \
		ImplRet(static_cast<_tBitmask>( \
			static_cast<_tInt>(_x) ^ static_cast<_tInt>(_y)))

#define DefBitmaskNot(_tBitmask, _tInt) \
	yconstfn _tBitmask operator~(_tBitmask _x) \
		ImplRet(static_cast<_tBitmask>(~static_cast<_tInt>(_x)))

#define DefBitmaskAndAssignment(_tBitmask, _tInt) \
	inline _tBitmask& operator&=(_tBitmask& _x, _tBitmask _y) \
		ImplRet(_x = _x & _y)

#define DefBitmaskOrAssignment(_tBitmask, _tInt) \
	inline _tBitmask& operator|=(_tBitmask& _x, _tBitmask _y) \
		ImplRet(_x = _x | _y)

#define DefBitmaskXorAssignment(_tBitmask, _tInt) \
	inline _tBitmask& operator^=(_tBitmask& _x, _tBitmask _y) \
		ImplRet(_x = _x ^ _y)

#define DefBitmaskOperations(_tBitmask, _tInt) \
	DefBitmaskAnd(_tBitmask, _tInt) \
	DefBitmaskOr(_tBitmask, _tInt) \
	DefBitmaskXor(_tBitmask, _tInt) \
	DefBitmaskNot(_tBitmask, _tInt) \
	DefBitmaskAndAssignment(_tBitmask, _tInt) \
	DefBitmaskOrAssignment(_tBitmask, _tInt) \
	DefBitmaskXorAssignment(_tBitmask, _tInt)

/*!
\since build 421
\note 不在类作用域内使用，因此不在类型名前使用 \c typename 。
*/
#define DefBitmaskEnum(_tEnum) \
	DefBitmaskOperations(_tEnum, std::underlying_type<_tEnum>::type)
//@}
//@}

#endif

