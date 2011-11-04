/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ybasemac.h
\ingroup Core
\brief 通用基础设施：宏定义。
\version r2961;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-10-09 09:25:27 +0800;
\par 修改时间:
	2011-11-04 19:30 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YBaseMacro;
*/


#ifndef YSL_INC_ADAPTOR_YBASEMAC_H_
#define YSL_INC_ADAPTOR_YBASEMAC_H_

//! \todo 检查语言实现的必要支持：可变参数宏。

/*
 ISO/IEC C++ 未确定宏定义内 # 和 ## 操作符求值顺序。
 GCC 中，宏定义内 ## 操作符修饰的形式参数为宏时，该宏不会被展开。
详见：http://gcc.gnu.org/onlinedocs/cpp/Concatenation.html 。
解决方案来源：
 https://www.securecoding.cert.org/confluence/display/cplusplus/ \
 PRE05-CPP.+Understand+macro+replacement+when+concatenating \
 +tokens+or+performing+stringification 。
*/
#define _yJOIN(x, y) x ## y


// YSLib 命名空间宏。

#define YSL_BEGIN	namespace YSLib {
#define YSL_END		}
#define YSL_			::YSLib::
#define YSL			::YSLib

#define YSL_BEGIN_NAMESPACE(s)	namespace s {
#define YSL_END_NAMESPACE(s)	}


/*
 ybase.h 中以下宏参数缩写的含义：
_a argument
_alist arguments list
_b base
_e expression
_i interface
_m member
_n name
_op operator
_p parameter
_plist paramaters list
_sig signature
_t type
*/
//接口类型宏。

#define _yInterface struct

#define implements public

#define _yInterfaceHead(_n) { \
	virtual ~_n() {}

//prefix "PDecl" = Pre-declare;
#define PDeclInterface(_n) _yInterface _n;

#define DeclInterface(_n) _yInterface _n _yInterfaceHead(_n)

//对于基接口需要显式指定访问权限和继承方式。
//由于接口定义为 struct 类型，因此通常只需指定是否为 virtual 继承。
// ImplI = Implements Interface;
//抽象实现：保留接口供派生类实现（可以提供接口函数的默认实现）。
// ImplA = Implements Abstractly;
#define DeclBasedInterface(_n, _b, ...) \
	_yInterface _n : _b, __VA_ARGS__ \
	_yInterfaceHead(_n)
#define ImplI(...) virtual
#define ImplA(...)

#define DeclBasedInterface1(_n, _b1) \
	_yInterface _n : _b1 \
	_yInterfaceHead(_n)
#define DeclBasedInterface2(_n, _b1, _b2) \
	_yInterface _n : _b1, _b2 \
	_yInterfaceHead(_n)
#define DeclBasedInterface3(_n, _b1, _b2, _b3) \
	_yInterface _n : _b1, _b2, _b3 \
	_yInterfaceHead(_n)
#define DeclBasedInterface4(_n, _b1, _b2, _b3, _b4) \
	_yInterface _n : _b1, _b2, _b3, _b4 \
	_yInterfaceHead(_n)

//"DeclIEntry" = Declare Interface Entry;
#define DeclIEntry(_sig) virtual _sig = 0;

#define EndDecl };


/*!
\brief 定义直接派生类。
\note 仅构造函数。不定义成员，避免对象切片或其它成员存储泄漏问题。
*/
#define DefExtendClass1(_n, _b, _a) \
	class _n : _a _b \
	{ \
	public: \
		_n(); \
	};


//函数宏。

//通用头定义。
//prefix "PDef" = Partially Define;
//prefix "PDefH" = prefix "PDef" Head;
//prefix "PDefTH" = prefix "PDef" Template Head;
#define PDefH(_t, _n, ...) \
	_t _n(__VA_ARGS__)
#define PDefHOperator(_t, _op, ...) \
	_t operator _op(__VA_ARGS__)

#define PDefConverter(_t) \
	operator _t()

#define PDefTH1(_t1) \
	template<typename _t1>
#define PDefTH2(_t1, _t2) \
	template<typename _t1, typename _t2>
#define PDefTH3(_t1, _t2, _t3) \
	template<typename _t1, typename _t2, typename _t3>
#define PDefTH4(_t1, _t2, _t3, _t4) \
	template<typename _t1, typename _t2, typename _t3, typename _t4>


//简单通用函数实现。
//prefix "Impl" = Implementation;
#define ImplExpr(_e) \
	{ \
		static_cast<void>(_e); \
	}
#define ImplRet(_e) \
	{ \
		return _e; \
	}
#define ImplUnsequenced(...) \
	{ \
		static_cast<void>(yunsequenced(__VA_ARGS__)); \
	}

//基类同名函数映射和成员同名函数映射实现。
//prefix "Impl" = Implement;
#define ImplBodyBase(_b, _n, ...) \
	{ \
		return _b::_n(__VA_ARGS__); \
	}
#define ImplBodyMember(_m, _n, ...) \
	{ \
		return (_m)._n(__VA_ARGS__); \
	}


//简单通用成员函数定义。
//prefix "Def" = Define;
#define DefEmptyDtor(_t) \
	~_t() \
	{}
#define ImplEmptyDtor(_t) \
	inline _t::DefEmptyDtor(_t)

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

#define DefDeDtor(_t) \
	~_t() = default;
#define DefDelDtor(_t) \
	~_t() = delete;

#define DefDeCopyAssignment(_t) \
	_t& operator=(const _t&) = default;
#define DefDelCopyAssignment(_t) \
	_t& operator=(const _t&) = delete;

#define DefDeMoveAssignment(_t) \
	_t& operator=(_t&&) = default;
#define DefDelMoveAssignment(_t) \
	_t& operator=(_t&&) = delete;

#define DefConverter(_t, _e) \
	operator _t() const ynothrow \
	{ \
		return _e; \
	}
#define DefConverterBase(_t, _b) \
	operator _t() const ynothrow \
	{ \
		return _b::operator _t(); \
	}
#define DefConverterMember(_t, _m) \
	operator _t() const ynothrow \
	{ \
		return (_m).operator _t(); \
	}

#define DefMutableConverter(_t, _e) \
	operator _t() ynothrow \
	{ \
		return _e; \
	}
#define DefMutableConverterBase(_t, _b) \
	operator _t() ynothrow \
	{ \
		return _b::operator _t(); \
	}
#define DefMutableConverterMember(_t, _m) \
	operator _t() ynothrow \
	{ \
		return (_m).operator _t(); \
	}

#define DefPredicate(_n, _m) \
	bool _yJOIN(Is, _n)() const ynothrow \
	{ \
		return (_m); \
	}
#define DefPredicateBase(_n, _b) \
	bool _yJOIN(Is, _n)() const ynothrow \
	{ \
		return _b::_yJOIN(Is, _n)(); \
	}
#define DefPredicateMember(_n, _m) \
	bool _yJOIN(Is, _n)() const ynothrow \
	{ \
		return (_m)._yJOIN(Is, _n)(); \
	}

#define DefMutablePredicate(_n, _m) \
	bool _yJOIN(Is, _n)() ynothrow \
	{ \
		return (_m); \
	}
#define DefMutablePredicateBase(_n, _b) \
	bool _yJOIN(Is, _n)() ynothrow \
	{ \
		return _b::_yJOIN(Is, _n)(); \
	}
#define DefMutablePredicateMember(_n, _m) \
	bool _yJOIN(Is, _n)() ynothrow \
	{ \
		return (_m)._yJOIN(Is, _n)(); \
	}

#define DefGetter(_t, _n, _m) \
	_t _yJOIN(Get, _n)() const ynothrow \
	{ \
		return (_m); \
	}
#define DefGetterBase(_t, _n, _b) \
	_t _yJOIN(Get, _n)() const ynothrow \
	{ \
		return _b::_yJOIN(Get, _n)(); \
	}
#define DefGetterMember(_t, _n, _m) \
	_t _yJOIN(Get, _n)() const ynothrow \
	{ \
		return (_m)._yJOIN(Get, _n)(); \
	}

#define DefMutableGetter(_t, _n, _m) \
	_t _yJOIN(Get, _n)() ynothrow \
	{ \
		return (_m); \
	}
#define DefMutableGetterBase(_t, _n, _b) \
	_t _yJOIN(Get, _n)() ynothrow \
	{ \
		return _b::_yJOIN(Get, _n)(); \
	}
#define DefMutableGetterMember(_t, _n, _m) \
	_t _yJOIN(Get, _n)() ynothrow \
	{ \
		return (_m)._yJOIN(Get, _n)(); \
	}

#define DefSetter(_t, _n, _m) \
	void _yJOIN(Set, _n)(_t _tempArgName) \
	{ \
		(_m) = _tempArgName; \
	}
#define DefSetterDe(_t, _n, _m, _defv) \
	void _yJOIN(Set, _n)(_t _tempArgName = _defv) \
	{ \
		(_m) = _tempArgName; \
	}
#define DefSetterBase(_t, _n, _b) \
	void _yJOIN(Set, _n)(_t _tempArgName) \
	{ \
		_b::_yJOIN(Set, _n)(_tempArgName); \
	}
#define DefSetterBaseDe(_t, _n, _b, _defv) \
	void _yJOIN(Set, _n)(_t _tempArgName = _defv) \
	{ \
		_b::_yJOIN(Set, _n)(_tempArgName); \
	}
#define DefSetterMember(_t, _n, _m) \
	void _yJOIN(Set, _n)(_t _tempArgName) \
	{ \
		(_m)._yJOIN(Set, _n)(_tempArgName); \
	}
#define DefSetterMemberDe(_t, _n, _m, _defv) \
	void _yJOIN(Set, _n)(_t _tempArgName = _defv) \
	{ \
		(_m)._yJOIN(Set, _n)(_tempArgName); \
	}
#define DefSetterEx(_t, _n, _m, _tempArgName, _e) \
	void _yJOIN(Set, _n)(_t _tempArgName) \
	{ \
		(_m) = (_e); \
	}
#define DefSetterDeEx(_t, _n, _m, _defv, _tempArgName, _e) \
	void _yJOIN(Set, _n)(_t _tempArgName = _defv) \
	{ \
		(_m) = (_e); \
	}


/*!
\brief 动态复制。
\note 需要在满足 CopyConstructible 的类的定义内。
*/
#define DefClone(_t, _n) \
	_t* \
	_n() const \
	{ \
		return new _t(*this); \
	}


//成员函数和模板映射。

//传递构造函数。
#define DefForwardCtorT1(_tClass, _t1, _m1) \
	PDefTH1(_t1) \
	_tClass(_t1&& _m1##a_) \
		: _m1(yforward(_m1##a_)) \
	{}
#define DefForwardCtorT2(_tClass, _t1, _m1, _t2, _m2) \
	PDefTH2(_t1, _t2) \
	_tClass(_t1&& _m1##a_, _t2&& _m2##a_) \
		: _m1(yforward(_m1##a_)), _m2(yforward(_m2##a_)) \
	{}
#define DefForwardCtorT3(_tClass, _t1, _m1, _t2, _m2, _t3, _m3) \
	PDefTH3(_t1, _t2, _t3) \
	_tClass(_t1&& _m1##a_, _t2&& _m2##a_, _t3&& _m3##a_) \
		: _m1(yforward(_m1##a_)), _m2(yforward(_m2##a_)), \
		_m3(yforward(_m3##a_)) \
	{}
#define DefForwardCtorT4(_tClass, _t1, _m1, _t2, _m2, _t3, _m3, _t4, _m4) \
	PDefTH4(_t1, _t2, _t3, _t4) \
	_tClass(_t1&& _m1##a_, _t2&& _m2##a_, _t3&& _m3##a_, _t4&& _m4##a_) \
		: _m1(yforward(_m1##a_)), _m2(yforward(_m2##a_)), \
		_m3(yforward(_m3##a_)), _m4(yforward(_m4##a_)) \
	{}

#endif

