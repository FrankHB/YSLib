/*
	© 2015-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Dependency.h
\ingroup NPL
\brief 依赖管理。
\version r136
\author FrankHB <frankhb1989@gmail.com>
\since build 623
\par 创建时间:
	2015-08-09 22:12:37 +0800
\par 修改时间:
	2016-11-21 02:23 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::Dependency
*/


#ifndef NPL_INC_Dependency_h_
#define NPL_INC_Dependency_h_

#include "YModules.h"
#include YFM_NPL_Lexical // for string, vector;
#include <istream> // for std::istream;

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


//! \since build 623
//@{
//! \brief 从内容为 GCC 兼容编译器 -MMD 选项生成的 Makefile 的流中分解路径。
//@{
//! \since build 742
YF_API vector<string>
DecomposeMakefileDepList(std::streambuf&);
YF_API vector<string>
DecomposeMakefileDepList(std::istream&);
//@}

/*!
\brief 过滤 GCC 兼容编译器 -MMD 选项生成的 Makefile 项列表并选取依赖项。
\return 是否具有冒号项且移除被依赖项和冒号项后非空。
*/
YF_API bool
FilterMakefileDependencies(vector<string>&);
//@}


/*!
\note 第一参数和第二参数分别为目标和源路径。
\see 工具脚本 \c SHBuild-common.sh 。
\since build 651
\todo 检查可能存在于目标路径的旧项的权限；设置权限。
*/
//@{
//@{
/*!
\brief 安装文件：复制单个文件。
\note 复制前检查内容，若相同则不进行冗余复制（类似 rsync -a ）。
*/
//@{
//! \since build 659
YF_API YB_NONNULL(1, 2) void
InstallFile(const char*, const char*);
inline PDefH(void, InstallFile, const string& dst, const string& src)
	ImplExpr(InstallFile(dst.c_str(), src.c_str()))
//@}

//! \brief 安装目录：复制目录树。
//@{
//! \since build 659
YF_API void
InstallDirectory(const string&, const string&);
inline YB_NONNULL(1, 2) PDefH(void, InstallDirectory, const char* dst,
	const char* src)
	ImplExpr(InstallDirectory(string(dst), string(src)))
//@}
//@}

/*!
\brief 安装硬链接：创建硬链接，失败则安装文件。
\throw std::invalid_argument 安装前发现源是有效的目录。
*/
//@{
//! \since build 659
YF_API YB_NONNULL(1, 2) void
InstallHardLink(const char*, const char*);
inline PDefH(void, InstallHardLink, const string& dst, const string& src)
	ImplExpr(InstallHardLink(dst.c_str(), src.c_str()))
//@}

//! \brief 安装符号链接：创建文件符号链接，失败则安装文件。
//@{
//! \since build 659
YF_API YB_NONNULL(1, 2) void
InstallSymbolicLink(const char*, const char*);
inline PDefH(void, InstallSymbolicLink, const string& dst, const string& src)
	ImplExpr(InstallSymbolicLink(dst.c_str(), src.c_str()))
//@}

/*!
\brief 安装可执行文件：安装文件并按需设置可执行权限。
\todo 设置 chmod +x 或 NTFS 可执行权限。
*/
//! \since build 659
//@{
YF_API YB_NONNULL(1, 2) void
InstallExecutable(const char*, const char*);
inline PDefH(void, InstallExecutable, const string& dst, const string& src)
	ImplExpr(InstallExecutable(dst.c_str(), src.c_str()))
//@}
//@}

} // namespace NPL;

#endif

