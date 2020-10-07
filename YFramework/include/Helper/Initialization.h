/*
	© 2009-2016, 2019-2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Initialization.h
\ingroup Helper
\brief 框架初始化。
\version r893
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-10-21 23:15:08 +0800
\par 修改时间:
	2020-10-05 09:19 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::Initialization
*/


#ifndef INC_Helper_Initialization_h_
#define INC_Helper_Initialization_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_ValueNode // for string, ValueNode;
#include YFM_YSLib_Service_FileSystem // for IO::Path;
#include YFM_YSLib_Core_YApplication // for Application;
#include YFM_YSLib_Adaptor_Font // for Drawing::FontCache;
#include YFM_YSLib_Service_ContentType // for MIMEBiMapping;

namespace YSLib
{

#if YCL_DS
/*!
\brief 设置是否显示初始化的诊断消息。
\since build 690
*/
extern bool ShowInitializedLog;
#endif


/*!
\brief 处理最外层边界的异常，若捕获致命错误则在之后终止程序。
\note 可作为 FilterExceptions 的参数，用于统一处理抛出到 GUI 程序的主函数的异常。
\sa FilterExceptions
\since build 691
*/
YF_API void
TraceForOutermost(const std::exception&, RecordLevel) ynothrow;


//! \since build 899
//@{
/*!
\brief 判断指定程序映像的路径字符串指示的位置是否符合局部 FHS 目录布局。
\see https://refspecs.linuxfoundation.org/FHS_3.0/fhs-3.0.html 。

以 POSIX 环境变量语法，预期的局部用户目录布局为：
$PREFIX/
$PREFIX/$BIN/
$PREFIX/lib/
$PREFIX/share/
其中，$PREFIX 为路径前缀，$BIN 是任意非空的目录名。
当参数指定 $PREFIX/$BIN/ 目录，且以上路径以目录存在时，检查通过。
假定检查目录时，布局不改变。
实现可分别检查各项路径，而不要求避免外部并发访问的竞争。
*/
YB_ATTR_nodiscard YF_API YB_PURE bool
CheckLocalFHSLayout(const string&);

/*!
\brief 取根路径字符串对应的局部 FHS 根路径。
\sa FetchRootPathString
\sa GetLocalFHSRootPathOf
*/
YB_ATTR_nodiscard YF_API YB_PURE const IO::Path&
FetchLocalFHSRootPath();

/*!
\brief 取框架根路径字符串。
\return 以平台路径分隔符结尾的目录路径字符串。
*/
YB_ATTR_nodiscard YF_API YB_PURE const string&
FetchRootPathString();

/*!
\brief 取指定程序映像的路径字符串对应的局部 fHS 根路径。
\sa CheckLocalFHSLayout
\sa FetchRootPathString

若框架根路径的 CheckLocalFHSLayout 检查通过，局部 fHS 根路径为对应的 $PREFIX ；
否则，局部 fHS 根路径路径为空。
*/
YB_ATTR_nodiscard YF_API YB_PURE IO::Path
GetLocalFHSRootPathOf(const string&);
//@}


/*!
\brief 载入 NPLA1 配置文件。
\param show_info 是否在标准输出中显示信息。
\pre 间接断言：指针参数非空。
\return 读取的配置。
\note 预设行为、配置文件和配置项参考 Documentation::YSLib 。
\since build 450
*/
YB_ATTR_nodiscard YF_API YB_NONNULL(1, 2) ValueNode
LoadNPLA1File(const char* disp, const char* path, ValueNode(*creator)(),
	bool show_info = {});

/*!
\brief 初始化应用程序组件。
\throw GeneralEvent 嵌套异常：加载失败。
\since build 693
*/
YF_API void
LoadComponents(Application&, const ValueNode&);

/*!
\brief 载入默认配置。
\return 读取的配置。
\sa LoadNPLA1File
\since build 344
*/
YF_API ValueNode
LoadConfiguration(bool = {});

/*!
\brief 保存默认配置。
\throw GeneralEvent 配置文件路径指定的文件不存在或保存失败。\
\since build 344
*/
YF_API void
SaveConfiguration(const ValueNode&);


/*!
\brief 初始化系统字体缓存。
\pre 默认字体缓存已初始化。
\throw FatalError 字体缓存初始化失败。
\since build 398

加载默认字体文件路径和默认字体目录中的字型至默认字体缓存。
*/
YF_API void
InitializeSystemFontCache(Drawing::FontCache&, const string&, const string&);


/*!
\brief 取值类型根节点。
\pre 断言：已初始化。
\since build 688
*/
YF_API ValueNode&
FetchRoot() ynothrow;

/*!
\brief 取默认字体缓存。
\exception FatalError 字体缓存初始化失败。
\since build 425
*/
YF_API Drawing::FontCache&
FetchDefaultFontCache();

/*!
\brief 取 MIME 类型名和文件扩展名双向映射对象。
\since build 450
*/
YF_API MIMEBiMapping&
FetchMIMEBiMapping();

} // namespace YSLib;

#endif

