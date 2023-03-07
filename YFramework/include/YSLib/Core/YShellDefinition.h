/*
	© 2009-2013, 2015, 2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YShellDefinition.h
\ingroup Core
\brief 宏定义和类型描述。
\version r1721
\author FrankHB <frankhb1989@gmail.com>
\since build 593
\par 创建时间:
	2009-12-24 15:29:11 +0800
\par 修改时间:
	2023-03-08 05:16 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YShellDefinition
*/


#ifndef YSL_INC_Core_YShellDefinition_h_
#define YSL_INC_Core_YShellDefinition_h_ 1

#include "YModules.h"
#include YFM_YSLib_Adaptor_YAdaptor // for YPP_Stringize;

//! \since build 969
//!@{
//! \brief YSLib 主版本号。
#define YSL_Version_Major 0
//! \brief YSLib 次版本号。
#define YSL_Version_Minor 9
//! \brief YSLib 修订版本号。
#ifndef YSL_Version_Patch
#	define YSL_Version_Patch 0
#endif
//! \brief YSLib 主版本号字符串。
#define YSL_VerStr_Major YPP_Stringize(YSL_Version_Major)
//! \brief YSLib 次版本号字符串。
#define YSL_VerStr_Minor YPP_Stringize(YSL_Version_Minor)
//! \brief YSLib 修订版本号字符串。
#define YSL_VerStr_Patch YPP_Stringize(YSL_Version_Patch)
//! \brief YSLib 短语义版本号字符串。
#define YSL_SemVerStr_Short YSL_VerStr_Major "." YSL_VerStr_Minor
//! \brief YSLib 全语义版本号字符串。
#define YSL_SemVerStr_Full YSL_SemVerStr_Short "." YSL_VerStr_Patch
//!@}

namespace YSLib
{

/*!	\defgroup reset Reset Pointers
\brief 安全删除指定引用的句柄指向的对象。
\post 指定引用的句柄值等于 nullptr 。
\since build 209
*/
//!@{
template<typename _type>
inline bool
reset(_type*& p) ynothrow
{
	bool b(p);

	ydelete(p);
	p = {};
	return b;
}
//!@}

/*!
\brief 比较：shared_ptr 和内建指针类型的相等关系。
\since build 209
*/
template<typename _type>
inline bool
operator==(const shared_ptr<_type>& sp, _type* p)
{
	return sp.get() == p;
}
/*!
\brief 比较：weak_ptr 相等关系。
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
\brief 比较：shared_ptr 左值和内建指针类型的不等关系。
\since build 209
*/
template<typename _type>
inline bool
operator!=(const shared_ptr<_type>& sp, _type* p)
{
	return sp.get() != p;
}
/*!
\brief 比较：weak_ptr 不等关系。
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

//! \since build 969
namespace Consoles
{

//! \brief 颜色选项。
enum class ColorOption
{
	//! \brief 从不使用。
	Never = 0,
	//! \brief 总是使用。
	Always,
	//! \brief 自动判断。
	Auto
};

/*!
\ingroup tags
\brief 颜色选项标签类型。
\relates ColorOption
*/
template<ColorOption _vOpt = ColorOption::Auto>
using ColorOptionTag = std::integral_constant<ColorOption, _vOpt>;


FwdDeclI(IOutputTerminal)

} // namespace Consoles;

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


/*!	\defgroup GlobalObjects Global Objects
\brief 全局对象。
*/

/*!	\defgroup DefaultGlobalConstants Default Global Constants
\ingroup GlobalObjects
\brief 默认全局常量。
*/
//!@{
//!@}

/*!	\defgroup DefaultGlobalVariables Default Global Variables
\ingroup GlobalObjects
\brief 默认全局变量。
*/
//!@{
//!@}

/*!	\defgroup DefaultGlobalObjectMapping Default Global Object Mapping
\ingroup GlobalObjects
\brief 默认全局变量映射。

访问全局程序实例对象。
*/
//!@{
//!@}

} // namespace YSLib;

#endif

