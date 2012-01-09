/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cstring.h
\ingroup YStandardEx
\brief YCLib ISO C 标准字符串扩展。
\version r2421;
\author FrankHB<frankhb1989@gmail.com>
\since build 245 。
\par 创建时间:
	2009-12-27 17:31:14 +0800;
\par 修改时间:
	2012-01-07 21:07 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YStandardEx::CString;
*/


#ifndef YCL_INC_YSTRING_H_
#define YCL_INC_YSTRING_H_

#include "../ydef.h"
#include <cstdlib>
#include <type_traits>
#include <cstring>
#include <cctype>

namespace ystdex
{
	//非 ISO C/C++ 扩展库函数。
	//

	/*!
	\brief 带空指针检查的字符串长度计算。
	\return 当字符指针非空时为 std::strlen 计算的串长，否则为 0 。
	*/
	std::size_t
	strlen_n(const char*);

	/*!
	\brief 带空指针检查的字符串复制。
	\return 成功时为复制的字符串，失败时为空指针。

	当目标字符串和源字符串都非空时用 std::strcpy 复制字符串。
	*/
	char*
	strcpy_n(char*, const char*);

	/*!
	\brief 带空指针检查的字符串连接。
	\return 返回目标参数。

	对传入参数进行非空检查后串接指定的两个字符串，结果复制至指定位置。
	*/
	char*
	strcpycat(char*, const char*, const char*);

	/*!
	\brief 带空指针检查的字符串连接复制。
	\return 目标参数。

	对传入参数进行非空检查后串接指定的两个字符串，
	结果复制至用指定分配函数（默认为 std::malloc）新分配的空间。
	*/
	char*
	strcatdup(const char*, const char*, void*(*)(std::size_t) = std::malloc);


	/*!
	\defgroup NTCTSUtil null-terminated character string utilities
	\brief 简单 NTCTS 操作。
	\note NTCTS(null-terminated character string) 即 NUL 字符标记结束的字符串，
		除了结束字符外没有 NUL 字符。
	\note 简单指不包括 NTMBS(null-terminated mutibyte string) ，按等宽字符考虑。
	\note 参见 ISO/IEC 14882:2003 (17.1.12, 17.3.2.1.3.2) 。
	*/

	/*!
	\ingroup NTCTSUtil
	\brief 计算简单 NTCTS 长度。
	\pre 断言： <tt>str</tt> 。
	*/
	template<typename _tChar>
	size_t
	sntctslen(const _tChar* str)
	{
		assert(str);

		const _tChar* p(str);

		while(*p != 0)
			++p;
		return p - str;
	}

	/*!
	\ingroup NTCTSUtil
	\brief 按字典序比较简单 NTCTS 。
	\pre 断言： <tt>str1 && str2</tt> 。
	*/
	template<typename _tChar>
	wint_t
	sntctscmp(const _tChar* str1, const _tChar* str2)
	{
		assert(str1 && str2);

		wint_t d(0);

		while(!(d = *str1 - *str2))
		{
			yunseq(++str1, ++str2);
		}
		return d;
	}

	/*!
	\ingroup NTCTSUtil
	\brief 按字典序比较简单 NTCTS （忽略大小写）。
	\pre 断言： <tt>str1 && str2</tt> 。
	*/
	template<typename _tChar>
	wint_t
	sntctsicmp(const _tChar* str1, const _tChar* str2)
	{
		assert(str1 && str2);

		wint_t d(0);

		while(!(d = std::tolower(*str1) - std::tolower(*str2)))
		{
			yunseq(++str1, ++str2);
		}
		return d;
	}
}

#endif

