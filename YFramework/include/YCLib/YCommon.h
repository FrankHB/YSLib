/*
	© 2009-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YCommon.h
\ingroup YCLib
\brief 平台相关的公共组件无关函数与宏定义集合。
\version r3762
\author FrankHB <frankhb1989@gmail.com>
\since build 561
\par 创建时间:
	2009-11-12 22:14:28 +0800
\par 修改时间:
	2016-07-25 10:25 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::YCommon
*/


#ifndef YCL_INC_ycommon_h_
#define YCL_INC_ycommon_h_ 1

#include "YModules.h"
#include YFM_YCLib_Platform
#include <ystdex/functional.hpp> // for ystdex::decay_t, ystdex::result_of_t,
//	ystdex::retry_on_cond;
#include <ystdex/cassert.h> // yconstraint, yassume for other headers;
#include <ystdex/cwctype.h> // for ystdex::isprint, ystdex::iswprint;
#include <ystdex/cstring.h> // for ystdex::uchar_t, ystdex::replace_cast;
#include YFM_YBaseMacro

//! \brief 默认平台命名空间。
namespace platform
{

/*!
\ingroup Platforms
\since build 652
*/
//@{
//! \brief \c YF_Platform_* 宏替换值的公共类型。
using ID = std::uintmax_t;

//! \brief \c YF_Platform_* 宏替换值的对应的元类型。
template<ID _vN>
using MetaID = ystdex::integral_constant<ID, _vN>;

//! \brief 平台标识的公共标记类型：指定任意平台。
struct IDTagBase
{};

#if YB_IMPL_CLANGPP
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wweak-vtables"
#endif

//! \brief 宿主平台标识的公共标记类型：指定任意宿主平台。
struct HostedIDTagBase : virtual IDTagBase
{};

//! \brief 平台标识的整数标记类型。
template<ID _vN>
struct IDTag : virtual IDTagBase, virtual MetaID<_vN>
{};

//! \brief 指定特定基类类型的 IDTag 特化。
#define YCL_IDTag_Spec(_n, _base) \
	template<> \
	struct IDTag<YF_Platform_##_n> : virtual _base, \
		virtual MetaID<YF_Platform_##_n> \
	{};

//! \brief 指定宿主平台的 IDTag 特化。
#define YCL_IDTag_SpecHost(_n) \
	YCL_IDTag_Spec(_n, HostedIDTagBase)

//! \brief 指定基于特定平台的 IDTag 特化。
#define YCL_IDTag_SpecOnBase(_n, _b) \
	YCL_IDTag_Spec(_n, IDTag<YF_Platform_##_b>)

YCL_IDTag_SpecHost(Win32)
YCL_IDTag_SpecOnBase(Win64, Win32)
YCL_IDTag_SpecOnBase(MinGW32, Win32)
YCL_IDTag_SpecOnBase(MinGW64, Win64)
YCL_IDTag_SpecHost(Linux)
YCL_IDTag_SpecOnBase(Linux_x86, Linux)
YCL_IDTag_SpecOnBase(Linux_x64, Linux)
YCL_IDTag_SpecOnBase(Android, Linux)
YCL_IDTag_SpecOnBase(Android_ARM, Android)

//! \brief 取平台标识的整数标记集合类型。
template<ID... _vN>
struct IDTagSet : virtual IDTag<_vN>...
{};
//@}

#if YB_IMPL_CLANGPP
#	pragma GCC diagnostic pop
#endif

/*!
\ingroup PlatformEmulation
\brief 定义用于平台模拟的传递模板。
\note 使用 ADL 调用第二参数。
\since build 652
*/
#define YCL_DefPlatformFwdTmpl(_n, _fn) \
	DefFwdTmplAuto(_n, _fn(platform::IDTag<YF_Platform>(), yforward(args)...))


/*!
\brief 声明检查存在合式调用的辅助宏。
\sa ystdex::is_detected
\since build 651
*/
//@{
#define YCL_CheckDecl_t(_fn) CheckDecl##_fn##_t
#define YCL_DeclCheck_t(_fn, _call) \
	template<typename... _tParams> \
	using YCL_CheckDecl_t(_fn) \
		= decltype(_call(std::declval<_tParams&&>()...));
//@}


/*!
\brief 异常终止函数。
\since build 319
*/
YB_NORETURN YF_API void
terminate() ynothrow;


/*!
\brief 平台描述空间。
\since build 456
*/
namespace Descriptions
{

/*!
\brief 记录等级。
\since build 432
*/
enum RecordLevel : std::uint8_t
{
	Emergent = 0x00,
	Alert = 0x20,
	Critical = 0x40,
	Err = 0x60,
	Warning = 0x80,
	Notice = 0xA0,
	Informative = 0xC0,
	Debug = 0xE0
};

} // namespace Descriptions;


/*!
\brief 检查默认区域下指定字符是否为可打印字符。
\note MSVCRT 的 isprint/iswprint 实现缺陷的变通。
\sa https://connect.microsoft.com/VisualStudio/feedback/details/799287/isprint-incorrectly-classifies-t-as-printable-in-c-locale
\since build 512
*/
//@{
inline PDefH(bool, IsPrint, char c)
	ImplRet(ystdex::isprint(c))
inline PDefH(bool, IsPrint, wchar_t c)
	ImplRet(ystdex::iswprint(c))
template<typename _tChar>
bool
IsPrint(_tChar c)
{
	return platform::IsPrint(wchar_t(c));
}
//@}


/*!
\warning 注意避免违反 ISO C++11 [basic.lval]/10 以引起未定义行为。
\since build 631
*/
//@{
inline PDefH(ystdex::uchar_t*, ucast, wchar_t* p) ynothrow
	ImplRet(ystdex::replace_cast<ystdex::uchar_t*>(p))
inline PDefH(const ystdex::uchar_t*, ucast, const wchar_t* p) ynothrow
	ImplRet(ystdex::replace_cast<const ystdex::uchar_t*>(p))
template<typename _tChar>
_tChar*
ucast(_tChar* p) ynothrow
{
	return p;
}

inline PDefH(wchar_t*, wcast, ystdex::uchar_t* p) ynothrow
	ImplRet(ystdex::replace_cast<wchar_t*>(p))
inline PDefH(const wchar_t*, wcast, const ystdex::uchar_t* p) ynothrow
	ImplRet(ystdex::replace_cast<const wchar_t*>(p))
template<typename _tChar>
_tChar*
wcast(_tChar* p) ynothrow
{
	return p;
}
//@}


/*!
\brief 循环重复操作。
\since build 625
*/
template<typename _func, typename _tErrorRef,
	typename _tError = ystdex::decay_t<_tErrorRef>,
	typename _type = ystdex::result_of_t<_func&()>>
_type
RetryOnError(_func f, _tErrorRef&& err, _tError e = _tError())
{
	return ystdex::retry_on_cond([&](_type res){
		return res < _type() && _tError(err) == e;
	}, f);
}


/*!
\brief 执行 UTF-8 字符串的环境命令。
\note 使用 std::system 实现；若参数为空则和 std::system 行为一致。
\since build 539
*/
YF_API int
usystem(const char*);


//! \since build 704
//@{
//! \brief 系统配置选项。
enum class SystemOption
{
	/*!
	\brief 内存页面字节数。
	\note 0 表示不支持分页。
	\since build 712
	*/
	PageSize,
	//! \brief 符号链接在路径解析中允许的最大次数。
	MaxSymlinkLoop
};


/*!
\brief 取限制配置。
\return 选项存在且值能被返回类型表示时为转换后的对应选项值，否则为 \c size_t(-1) 。
\note 某些平台调用结果总是翻译时确定的常数，调用此函数无副作用，使用 YB_STATELESS 修饰。
\note 若无 YB_STATELESS 修饰，当找不到项时，可能使用跟踪输出警告。
\note 若无 YB_STATELESS 修饰，当找不到项时，可能设置 errno 为 EINVAL 。
\sa YTraceDe
*/
YF_API
#if YCL_DS
	YB_STATELESS
#endif
size_t
FetchLimit(SystemOption) ynothrow;
//@}

} // namespace platform;

/*!
\brief 平台扩展命名空间。
\note 为便于移植，部分平台扩展接口保留至平台中立文件，依赖性按后者处理。
*/
namespace platform_ex
{

#if YCL_DS
/*!
\brief 设置允许设备进入睡眠的标识状态。
\return 旧状态。
\note 默认状态为 true 。
\warning 非线程安全。
\since build 278
*/
YF_API bool
AllowSleep(bool);
#endif

} // namespace platform_ex;

#endif

