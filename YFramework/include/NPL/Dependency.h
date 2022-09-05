/*
	© 2015-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Dependency.h
\ingroup NPL
\brief 依赖管理。
\version r617
\author FrankHB <frankhb1989@gmail.com>
\since build 623
\par 创建时间:
	2015-08-09 22:12:37 +0800
\par 修改时间:
	2022-09-05 08:50 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::Dependency
*/


#ifndef NPL_INC_Dependency_h_
#define NPL_INC_Dependency_h_

#include "YModules.h"
#include YFM_NPL_NPLA1 // for string, vector, REPLContext, YSLib::unique_ptr;
#include <istream> // for std::istream;
#include <ystdex/scope_guard.hpp> // for ystdex::guard;

namespace NPL
{

//! \since build 624
enum class DepsEventType : yimpl(size_t)
{
	General,
	Search,
	Build,
	Command,
	DepsCheck,
	Max
};


//! \brief 从内容为 GCC 兼容编译器 -MMD 选项生成的 Makefile 的流中分解路径。
//@{
//! \since build 742
YF_API vector<string>
DecomposeMakefileDepList(std::streambuf&);
//! \since build 623
YF_API vector<string>
DecomposeMakefileDepList(std::istream&);
//@}

/*!
\brief 过滤 GCC 兼容编译器 -MMD 选项生成的 Makefile 项列表并选取依赖项。
\return 是否具有冒号项且移除被依赖项和冒号项后非空。
\since build 623
*/
YF_API bool
FilterMakefileDependencies(vector<string>&);

//! \since build 758
namespace A1
{

/*!
\brief 打开指定路径的文件作为 NPL 输入流。
\throw 嵌套异常：文件打开失败。
\note 编码假定为 UTF-8 。允许可选的 BOM 。以共享只读方式打开。
\since build 839
*/
YB_ATTR_nodiscard YF_API YB_NONNULL(1) YSLib::unique_ptr<std::istream>
OpenFile(const char*);

//! \since build 899
//@{
//! \brief 打开指定路径的文件作为 NPL 输入流并在上下文设置源代码名称。
YB_ATTR_nodiscard YF_API YSLib::unique_ptr<std::istream>
OpenUnique(REPLContext&, string);


/*!
\brief 在 REPL 上下文中加载指定名称的外部翻译单元。
\note 使用跳板，不直接改变上下文中的续延。
\note 加载时在上下文中设置源代码名称。
\sa OpenUnique
\sa TryLoadSource
*/
YB_NONNULL(2) YF_API void
PreloadExternal(REPLContext&, const char*);

//! \note 以参数指定的项的第一个子项作为 string 类型的名称指定翻译单元。
//@{
/*!
\brief 规约加载外部翻译单元。
\pre 要求同命名空间 Forms 中的规约函数。
\sa Forms
\sa OpenUnique
*/
YF_API ReductionStatus
ReduceToLoadExternal(TermNode&, ContextNode&, REPLContext&);

/*!
\brief 异步规约加载外部翻译单元。
\note 这个函数可用于实现外部的复合加载操作。
\sa ReduceToLoadExternal
*/
YF_API ReductionStatus
RelayToLoadExternal(ContextNode&, TermNode&, REPLContext&);
//@}
//@}

namespace Forms
{

/*!
\note 支持的具体语法形式参考 Documentation::NPL 。
\since build 839
*/
//@{
/*!
\brief 加载 NPLA1 基础上下文。

加载序列中缀分隔符和 NPLA1 基础环境使用的公共语法形式。
支持的语法形式包括预定义对象、基本操作和定义在基础环境中的派生操作。
其中，派生操作包括基本派生操作和核心库函数。
加载的基础环境被冻结。
当前派生实现：求值合并子调用前已加载字符串模块或等价方式初始化为模块 std.strings 。
*/
YF_API void
LoadGroundContext(REPLContext&);

/*!
\pre 已加载基础 NPLA1 上下文或等价方式初始化。
\exception NPLException 异常中立：初始化失败。
\warning 若不满足加载的前置条件，可能调用失败，抛出异常。
\sa LoadGroundContext
*/
//@{
/*!
\brief 加载续延模块。
\since build 943

加载一等续延和相关操作。
*/
YF_API void
LoadModule_std_continuations(REPLContext&);

/*!
\brief 加载代理求值模块。
\since build 856

加载 promise 类型和延迟求值等操作。
*/
YF_API void
LoadModule_std_promises(REPLContext&);

/*!
\brief 加载数学功能模块。
\since build 930

加载数学功能相关操作。
*/
YF_API void
LoadModule_std_math(REPLContext&);

/*!
\brief 加载字符串模块。

加载字符串库操作。
*/
YF_API void
LoadModule_std_strings(REPLContext&);

/*!
\pre 当前派生实现：已加载和初始化依赖的模块，在当前环境可访问的指定的模块名称。
\exception NPLException 违反加载模块的前置条件而无法成功初始化。
*/
//@{
/*!
\brief 加载输入/输出模块。
\pre 断言：第二参数非空。
\note 第二参数指定基础环境。
\since build 942

加载输入/输出库操作。
派生实现依赖模块：
字符串模块 std.strings 。
*/
YF_API void
LoadModule_std_io(REPLContext&, const shared_ptr<Environment>&);

/*!
\brief 加载系统模块。
\sa LoadModule_std_strings

加载系统库操作。
派生实现依赖模块：
字符串模块 std.strings 。
*/
YF_API void
LoadModule_std_system(REPLContext&);

/*!
\brief 加载模块管理模块。
\pre 断言：第二参数非空。
\note 第二参数指定基础环境。
\since build 942

加载模块管理操作。
派生实现依赖模块：
字符串模块 std.strings ；
输入/输出模块 std.io ；
代理求值模块 std.promises ；
系统模块 std.system 。
*/
YF_API void
LoadModule_std_modules(REPLContext&, const shared_ptr<Environment>&);
//@}

/*!
\brief 加载 SHBuild 使用的内部模块。

加载 SHBuild 自举使用的其它公共语法形式。
用于内部使用。加载的环境的具体内容未指定。
*/
YF_API void
LoadModule_SHBuild(REPLContext&);
//@}
//@}

/*!
\brief 加载 NPLA1 基础上下文和标准模块。
\sa LoadGroundContext
\sa LoadModule_std_io
\sa LoadModule_std_math
\sa LoadModule_std_modules
\sa LoadModule_std_promises
\sa LoadModule_std_strings
\sa LoadModule_std_system
\since build 898

调用 LoadGroundContext 并加载标准库模块。
加载的标准库模块名称符合 NPLA1 参考实现扩展环境约定。
模块初始化调用的顺序未指定，但满足必要的内部逻辑顺序。
加载的标准库模块被冻结。
*/
YF_API void
LoadStandardContext(REPLContext&);

} // namespace Forms;

} // namespace A1;

} // namespace NPL;

#endif

