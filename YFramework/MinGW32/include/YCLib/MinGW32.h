/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file MinGW32.h
\ingroup YCLib
\ingroup MinGW32
\brief YCLib MinGW32 平台扩展公共头文件。
\version r187
\author FrankHB <frankhb1989@gmail.com>
\since build 412
\par 创建时间:
	2012-06-08 17:57:49 +0800
\par 修改时间:
	2013-09-23 19:04 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(MinGW32)::MinGW32
*/


#ifndef YCL_MinGW32_INC_MinGW32_h_
#define YCL_MinGW32_INC_MinGW32_h_ 1

#include "YCLib/ycommon.h"
#include <YCLib/NativeAPI.h>
#include "YSLib/Core/yexcept.h" // for YSLib::LoggedEvent;

#if !YCL_MinGW32
#	error "This file is only for MinGW32."
#endif

/*!
\ingroup name_collision_workarounds
\brief 禁止使用 GetObject 宏。
\since build 412
*/
#undef GetObject

namespace platform_ex
{

/*!
\ingroup exception_types
\brief 宿主异常。
\since build 426
*/
class YF_API Exception : public YSLib::LoggedEvent
{
public:
	//! \since build 432
	Exception(const std::string& = "unknown host exception",
		LevelType = {}) ynothrow;
};

/*!
\brief Windows 平台扩展接口。
\since build 427
*/
inline namespace Windows
{

/*!
\ingroup exception_types
\brief Win32 错误引起的宿主异常。
\since build 426
*/
class YF_API Win32Exception : public Exception
{
public:
	//! \since build 435
	//@{
	using ErrorCode = ::DWORD;

private:
	ErrorCode err;

public:
	/*!
	\pre 错误码不等于 0 。
	\warning 初始化参数时可能会改变 ::GetLastError() 的结果。
	*/
	Win32Exception(ErrorCode, const std::string& = "Win32 exception",
		LevelType = {}) ynothrow;

	DefGetter(const ynothrow, ErrorCode, ErrorCode, err)
	//! \since build 437
	DefGetter(const ynothrow, std::string, Message, FormatMessage(err))

	explicit DefCvt(const ynothrow, ErrorCode, err)

	/*!
	\brief 格式化错误消息字符串。
	\return 若发生异常则结果为空，否则为区域固定为 en-US 的系统消息字符串。
	*/
	static std::string
	FormatMessage(ErrorCode) ynothrow;
	//@}
};

/*!
\brief 按 ::GetLastError 的结果和指定参数抛出 MinGW32::Win32Exception 对象。
\since build 426
*/
#	define YF_Raise_Win32Exception(...) \
	{ \
		const auto err(::GetLastError()); \
	\
		throw platform_ex::Windows::Win32Exception(err, __VA_ARGS__); \
	}


/*!
\brief 判断是否在 Wine 环境下运行。
\note 检查 HKEY_CURRENT_USER 和 HKEY_LOCAL_MACHINE 下的 Software\Wine 键实现。
\since build 435
*/
YF_API bool
CheckWine();


// TODO: Add more Windows specific APIs.

/*!	\defgroup native_encoding_conv Native Encoding Conversion
\brief 本机文本编码转换。
\since build 431
*/
//@{
YF_API std::string
UTF8ToMBCS(const char*, std::size_t, int = CP_ACP);
inline std::string
UTF8ToMBCS(const char* str, int cp = CP_ACP)
{
	return UTF8ToMBCS(str, ystdex::ntctslen(str), cp);
}
inline std::string
UTF8ToMBCS(const std::string& str, int cp = CP_ACP)
{
	return UTF8ToMBCS(str.c_str(), str.length(), cp);
}

YF_API std::string
WCSToMBCS(const wchar_t*, std::size_t, int = CP_ACP);
inline std::string
WCSToMBCS(const wchar_t* str, int cp = CP_ACP)
{
	return WCSToMBCS(str, ystdex::ntctslen(str), cp);
}
inline std::string
WCSToMBCS(const std::wstring& str, int cp = CP_ACP)
{
	return WCSToMBCS(str.c_str(), str.length(), cp);
}

YF_API std::wstring
MBCSToWCS(const char*, std::size_t, int = CP_ACP);
inline std::wstring
MBCSToWCS(const char* str, int cp = CP_ACP)
{
	return MBCSToWCS(str, ystdex::ntctslen(str), cp);
}
inline std::wstring
MBCSToWCS(const std::string& str, int cp = CP_ACP)
{
	return MBCSToWCS(str.c_str(), str.length(), cp);
}
//@}


/*!
\brief 注册表键。
\since build 435
\todo 增加和实现查询值等功能接口。
*/
class YF_API RegisterKey
{
private:
	::HKEY h_key;

public:
	RegisterKey(::HKEY h_parent, const wchar_t* key)
	{
		if(!::RegOpenKeyEx(h_parent, key, 0, KEY_READ, &h_key))
			YF_Raise_Win32Exception("RegOpenKeyEx");
	}
	~RegisterKey()
	{
		::RegCloseKey(h_key);
	}

	DefGetter(const ynothrow, ::HKEY, Key, h_key)
};

} // namespace Windows;

} // namespace platform_ex;

#endif

