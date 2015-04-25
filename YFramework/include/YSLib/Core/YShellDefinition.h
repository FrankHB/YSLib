/*
	© 2009-2013, 2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YShellDefinition.h
\ingroup Core
\brief 宏定义和类型描述。
\version r1662
\author FrankHB <frankhb1989@gmail.com>
\since build 593
\par 创建时间:
	2009-12-24 15:29:11 +0800
\par 修改时间:
	2015-04-24 06:39 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YShellDefinition
*/


#ifndef YSL_INC_Core_YShellDefinition_h_
#define YSL_INC_Core_YShellDefinition_h_ 1

#include "YModules.h"

#include YFM_YSLib_Adaptor_YAdaptor

namespace YSLib
{

/*!	\defgroup reset Reset Pointers
\brief 安全删除指定引用的句柄指向的对象。
\post 指定引用的句柄值等于 nullptr 。
\since build 209
*/
//@{
template<typename _type>
inline bool
reset(_type*& p) ynothrow
{
	bool b(p);

	ydelete(p);
	p = {};
	return b;
}
//@}

/*!
\brief 比较： shared_ptr 和内建指针类型的相等关系。
\since build 209
*/
template<typename _type>
inline bool
operator==(const shared_ptr<_type>& sp, _type* p)
{
	return sp.get() == p;
}
/*!
\brief 比较： weak_ptr 相等关系。
\note 注意和 shared_ptr 比较托管对象指针的语义不同。
\since build 454
*/
template<typename _type1, typename _type2>
inline bool
operator==(const weak_ptr<_type1>& x, const weak_ptr<_type2>& y)
{
	return !x.owner_before(y) && !y.owner_before(x);
}

/*!
\brief 比较： shared_ptr 左值和内建指针类型的不等关系。
\since build 209
*/
template<typename _type>
inline bool
operator!=(const shared_ptr<_type>& sp, _type* p)
{
	return sp.get() != p;
}
/*!
\brief 比较： weak_ptr 不等关系。
\note 注意和 shared_ptr 比较托管对象指针的语义不同。
\since build 209
*/
template<typename _type1, typename _type2>
inline bool
operator!=(const weak_ptr<_type1>& x, const weak_ptr<_type2>& y)
{
	return !(x == y);
}


/*!
\brief 独占所有权的锁定指针：使用线程模型对应的互斥量和锁以及 unique_ptr 。
\sa ystdex::threading::locked_ptr
\since build 551
*/
template<typename _type, class _tMutex = typename unlock_delete<>::mutex_type,
	class _tLock = typename unlock_delete<_tMutex>::lock_type>
using locked_ptr = unique_ptr<_type, unlock_delete<_tMutex, _tLock>>;

namespace Shells
{
class Shell;
} // namespace Shells;

namespace Text
{
class String;
} // namespace Text;

class Application;

using Shells::Shell;

using Text::String;


//平台无关的类型定义。
//


/*!	\defgroup GlobalObjects Global Objects
\brief 全局对象。
*/

/*!	\defgroup DefaultGlobalConstants Default Global Constants
\ingroup GlobalObjects
\brief 默认全局常量。
*/
//@{
//@}

/*!	\defgroup DefaultGlobalVariables Default Global Variables
\ingroup GlobalObjects
\brief 默认全局变量。
*/
//@{
//@}

/*!	\defgroup DefaultGlobalObjectMapping Default Global Object Mapping
\ingroup GlobalObjects
\brief 默认全局变量映射。

访问全局程序实例对象。
*/
//@{
//@}

} // namespace YSLib;

#endif

