/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ybase.h
\ingroup Adaptor
\brief 通用基础设施。
\version 0.2352;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-10-09 09:25:27 +0800;
\par 修改时间:
	2011-04-23 09:05 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Adaptor::YBase;
*/


#ifndef YSL_INC_ADAPTOR_BASE_H_
#define YSL_INC_ADAPTOR_BASE_H_

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


//异常规范宏。

// ythrow = "yielded throwing";
#ifdef YSL_USE_EXCEPTION_SPECIFICATION
#	define ythrow throw
#else
#	define ythrow(...)
#endif

#define ynothrow ythrow()


//接口类型宏。

#define _yInterface struct

#define implements public

#define _yInterfaceHead(_name) { \
	virtual ~_name() {}

//prefix "PDecl" = Pre-declare;
#define PDeclInterface(_name) _yInterface _name;

#define DeclInterface(_name) _yInterface _name _yInterfaceHead(_name)

//对于基接口需要显式指定访问权限和继承方式。
//由于接口定义为 struct 类型，因此通常只需指定是否为 virtual 继承。
// ImplI = Implements Interface;
//抽象实现：保留接口供派生类实现（可以提供接口函数的默认实现）。
// ImplA = Implements Abstractly;
#ifdef __GNUC__
#	define DeclBasedInterface(_name, _base...) \
	_yInterface _name : _base \
	_yInterfaceHead(_name)
#	define ImplI(_interface, ...) virtual
#	define ImplA(_interface, ...)
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#	define DeclBasedInterface(_name, _base, ...) \
	_yInterface _name : _base, __VA_ARGS__ \
	_yInterfaceHead(_name)
#	define ImplI(_interface, ...) virtual
#	define ImplA(_interface, ...)
#else
//注意 ISO/IEC C++ 不支持宏的可变参数列表，因此无法实现接口多继承。
#	define DeclBasedInterface(_name, _base) \
	_yInterface _name : _base \
	_yInterfaceHead(_name)
#	define ImplI(_interface) virtual
#	define ImplA(_interface)
#endif

#define DeclBasedInterface1(_name, _base1) \
	_yInterface _name : _base1 \
	_yInterfaceHead(_name)
#define DeclBasedInterface2(_name, _base1, _base2) \
	_yInterface _name : _base1, _base2 \
	_yInterfaceHead(_name)
#define DeclBasedInterface3(_name, _base1, _base2, _base3) \
	_yInterface _name : _base1, _base2, _base3 \
	_yInterfaceHead(_name)
#define DeclBasedInterface4(_name, _base1, _base2, _base3, _base4) \
	_yInterface _name : _base1, _base2, _base3, _base4 \
	_yInterfaceHead(_name)
#	define ImplI1(_interface1) virtual
#	define ImplI2(_interface1, _interface2) virtual
#	define ImplI3(_interface1, _interface2, _interface3) virtual
#	define ImplI4(_interface1, _interface2, _interface3, _interface4) virtual
#	define ImplA1(_interface1)
#	define ImplA2(_interface1, _interface2)
#	define ImplA3(_interface1, _interface2, _interface3)
#	define ImplA4(_interface1, _interface2, _interface3, _interface4)

//"DeclIEntry" = Declare Interface Entry;
#define DeclIEntry(_signature) virtual _signature = 0;

#define EndDecl };


//函数宏。

//通用函数头定义。
//prefix "PDefH" = Partially Define Head;
#ifdef __GNUC__
#	define PDefH(_type, _name, _paramlist...) \
	_type _name(_paramlist)
#	define PDefHOperator(_type, _op, _paramlist...) \
	_type operator _op(_paramlist)
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#	define PDefH(_type, _name, _paramlist, ...) \
	_type _name(_paramlist, __VA_ARGS__)
#	define PDefHOperator(_type, _op, _paramlist, ...) \
	_type operator _op(_paramlist, __VA_ARGS__)
#else
#	define PDefH(_type, _name, _paramlist) \
	_type _name(_paramlist)
#	define PDefHOperator(_type, _op, _paramlist) \
	_type operator _op(_paramlist)
#endif
#define PDefH0(_type, _name) \
	_type _name()
#define PDefH1(_type, _name, _param1) \
	_type _name(_param1)
#define PDefH2(_type, _name, _param1, _param2) \
	_type _name(_param1, _param2)
#define PDefH3(_type, _name, _param1, _param2, _param3) \
	_type _name(_param1, _param2, _param3)
#define PDefH4(_type, _name, _param1, _param2, _param3, _param4) \
	_type _name(_param1, _param2, _param3, _param4)
#define PDefHOperator0(_type, _op) \
	_type operator _op()
#define PDefHOperator1(_type, _op, _param1) \
	_type operator _op(_param1)
#define PDefHOperator2(_type, _op, _param1, _param2) \
	_type operator _op(_param1, _param2)
#define PDefHOperator3(_type, _op, _param1, _param2, _param3) \
	_type operator _op(_param1, _param2, _param3)
#define PDefHOperator4(_type, _op, _param1, _param2, _param3, _param4) \
	_type operator _op(_param1, _param2, _param3, _param4)
#define PDefConverter(_type) \
	operator _type()


//简单通用函数实现。
//prefix "Impl" = Implement;
#define ImplRet(_expr) \
	{ \
		return _expr; \
	}

//基类同名函数映射和成员同名函数映射实现。
//prefix "Impl" = Implement;
#ifdef __GNUC__
#	define ImplBodyBase(_base, _name, _arglist...) \
	{ \
		return _base::_name(_arglist); \
	}
#	define ImplBodyMember(_member, _name, _arglist...) \
	{ \
		return (_member)._name(_arglist); \
	}
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#	define ImplBodyBase(_base, _name, _arglist, ...) \
	{ \
		return _base::_name(_arglist, __VA_ARGS__); \
	}
#	define ImplBodyMember(_member, _name, _arglist...) \
	{ \
		return (_member)._name(_arglist); \
	}
#else
#	define ImplBodyBase(_base, _name, _arglist) \
	{ \
		return _base::_name(_arglist); \
	}
#	define ImplBodyMember(_member, _name, _arglist) \
	{ \
		return (_member)._name(_arglist); \
	}
#endif
#define ImplBodyBase0(_base, _name) \
	{ \
		return _base::_name(); \
	}
#define ImplBodyBase1(_base, _name, _arg1) \
	{ \
		return _base::_name(_arg1); \
	}
#define ImplBodyBase2(_base, _name, _arg1, _arg2) \
	{ \
		return _base::_name(_arg1, _arg2); \
	}
#define ImplBodyBase3(_base, _name, _arg1, _arg2, _arg3) \
	{ \
		return _base::_name(_arg1, _arg2, _arg3); \
	}
#define ImplBodyBase4(_base, _name, _arg1, _arg2, _arg3, _arg4) \
	{ \
		return _base::_name(_arg1, _arg2, _arg3, _arg4); \
	}
#define ImplBodyMember0(_member, _name) \
	{ \
		return (_member)._name(); \
	}
#define ImplBodyMember1(_member, _name, _arg1) \
	{ \
		return (_member)._name(_arg1); \
	}
#define ImplBodyMember2(_member, _name, _arg1, _arg2) \
	{ \
		return (_member)._name(_arg1, _arg2); \
	}
#define ImplBodyMember3(_member, _name, _arg1, _arg2, _arg3) \
	{ \
		return (_member)._name(_arg1, _arg2, _arg3); \
	}
#define ImplBodyMember4(_member, _name, _arg1, _arg2, _arg3, _arg4) \
	{ \
		return (_member)._name(_arg1, _arg2, _arg3, _arg4); \
	}


//简单通用成员函数定义。
//prefix "Def" = Define;
#define DefEmptyDtor(_type) \
	~_type() \
	{}

#define ImplEmptyDtor(_type) \
	inline _type::DefEmptyDtor(_type)

#define DefConverter(_type, _expr) \
	operator _type() const ynothrow \
	{ \
		return _expr; \
	}
#define DefConverterBase(_type, _base) \
	operator _type() const ynothrow \
	{ \
		return _base::operator _type(); \
	}
#define DefConverterMember(_type, _member) \
	operator _type() const ynothrow \
	{ \
		return (_member).operator _type(); \
	}

#define DefMutableConverter(_type, _expr) \
	operator _type() ynothrow \
	{ \
		return _expr; \
	}
#define DefMutableConverterBase(_type, _base) \
	operator _type() ynothrow \
	{ \
		return _base::operator _type(); \
	}
#define DefMutableConverterMember(_type, _member) \
	operator _type() ynothrow \
	{ \
		return (_member).operator _type(); \
	}

#define DefPredicate(_name, _member) \
	bool _yJOIN(Is, _name)() const ynothrow \
	{ \
		return (_member); \
	}
#define DefPredicateBase(_name, _base) \
	bool _yJOIN(Is, _name)() const ynothrow \
	{ \
		return _base::_yJOIN(Is, _name)(); \
	}
#define DefPredicateMember(_name, _member) \
	bool _yJOIN(Is, _name)() const ynothrow \
	{ \
		return (_member)._yJOIN(Is, _name)(); \
	}

#define DefMutablePredicate(_name, _member) \
	bool _yJOIN(Is, _name)() ynothrow \
	{ \
		return (_member); \
	}
#define DefMutablePredicateBase(_name, _base) \
	bool _yJOIN(Is, _name)() ynothrow \
	{ \
		return _base::_yJOIN(Is, _name)(); \
	}
#define DefMutablePredicateMember(_name, _member) \
	bool _yJOIN(Is, _name)() ynothrow \
	{ \
		return (_member)._yJOIN(Is, _name)(); \
	}

#define DefGetter(_type, _name, _member) \
	_type _yJOIN(Get, _name)() const ynothrow \
	{ \
		return (_member); \
	}
#define DefGetterBase(_type, _name, _base) \
	_type _yJOIN(Get, _name)() const ynothrow \
	{ \
		return _base::_yJOIN(Get, _name)(); \
	}
#define DefGetterMember(_type, _name, _member) \
	_type _yJOIN(Get, _name)() const ynothrow \
	{ \
		return (_member)._yJOIN(Get, _name)(); \
	}

#define DefMutableGetter(_type, _name, _member) \
	_type _yJOIN(Get, _name)() ynothrow \
	{ \
		return (_member); \
	}
#define DefMutableGetterBase(_type, _name, _base) \
	_type _yJOIN(Get, _name)() ynothrow \
	{ \
		return _base::_yJOIN(Get, _name)(); \
	}
#define DefMutableGetterMember(_type, _name, _member) \
	_type _yJOIN(Get, _name)() ynothrow \
	{ \
		return (_member)._yJOIN(Get, _name)(); \
	}

#define DefSetter(_type, _name, _member) \
	void _yJOIN(Set, _name)(_type _tempArgName) \
	{ \
		(_member) = _tempArgName; \
	}
#define DefSetterDe(_type, _name, _member, _defv) \
	void _yJOIN(Set, _name)(_type _tempArgName = _defv) \
	{ \
		(_member) = _tempArgName; \
	}
#define DefSetterBase(_type, _name, _base) \
	void _yJOIN(Set, _name)(_type _tempArgName) \
	{ \
		_base::_yJOIN(Set, _name)(_tempArgName); \
	}
#define DefSetterBaseDe(_type, _name, _base, _defv) \
	void _yJOIN(Set, _name)(_type _tempArgName = _defv) \
	{ \
		_base::_yJOIN(Set, _name)(_tempArgName); \
	}
#define DefSetterMember(_type, _name, _member) \
	void _yJOIN(Set, _name)(_type _tempArgName) \
	{ \
		(_member)._yJOIN(Set, _name)(_tempArgName); \
	}
#define DefSetterMemberDe(_type, _name, _member, _defv) \
	void _yJOIN(Set, _name)(_type _tempArgName = _defv) \
	{ \
		(_member)._yJOIN(Set, _name)(_tempArgName); \
	}
#define DefSetterEx(_type, _name, _member, _tempArgName, _expr) \
	void _yJOIN(Set, _name)(_type _tempArgName) \
	{ \
		(_member) = (_expr); \
	}
#define DefSetterDeEx(_type, _name, _member, _defv, _tempArgName, _expr) \
	void _yJOIN(Set, _name)(_type _tempArgName = _defv) \
	{ \
		(_member) = (_expr); \
	}


//成员函数和模板映射。


#endif

