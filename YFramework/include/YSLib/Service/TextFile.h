/*
	© 2009-2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file TextFile.h
\ingroup Service
\brief 平台无关的文本文件抽象。
\version r1070
\author FrankHB <frankhb1989@gmail.com>
\since build 473
\par 创建时间:
	2009-11-24 23:14:41 +0800
\par 修改时间:
	2018-08-20 07:54 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::TextFile
*/


#ifndef YSL_INC_Service_TextFile_h_
#define YSL_INC_Service_TextFile_h_ 1

#include "YModules.h"
#include YFM_YSLib_Service_File // for string, File;

namespace YSLib
{

namespace Text
{

/*!
\brief 验证编码。
\note 第二参数和第三参数指定验证用的缓冲区，第四参数指定最大文本长度，
	第五参数参数指定验证的编码。
\note 假定调用前流状态正常。
\since build 621
*/
//@{
YB_NONNULL(1, 2) Text::Encoding
VerifyEncoding(std::FILE*, char*, size_t, size_t, Encoding = CS_Default);
//! \since build 743
YB_NONNULL(2) Text::Encoding
VerifyEncoding(std::streambuf&, char*, size_t, size_t, Encoding = CS_Default);
YB_NONNULL(2) Text::Encoding
VerifyEncoding(std::istream&, char*, size_t, size_t, Encoding = CS_Default);
//@}


/*!
\brief Unicode 编码模式标记。

Unicode Encoding Scheme Signatures BOM（byte-order mark ，字节顺序标记）常量。
\note 在 Unicode 3.2 前作为零宽无间断空格字符在对应字符集的编码单字节序列。
\note 适用于未明确字节序或字符集的流。
\since build 297
*/
//@{
yconstexpr const char BOM_UTF_16LE[]{"\xFF\xFE"};
yconstexpr const char BOM_UTF_16BE[]{"\xFE\xFF"};
yconstexpr const char BOM_UTF_8[]{"\xEF\xBB\xBF"};
yconstexpr const char BOM_UTF_32LE[]{"\xFF\xFE\x00\x00"};
yconstexpr const char BOM_UTF_32BE[]{"\x00\x00\xFE\xFF"};
//@}

//! \brief 检查第一参数是否具有指定的 BOM 。
//@{
/*!
\pre 间接断言：指针参数非空。
\since build 619
*/
//@{
YB_NONNULL(1, 2) inline PDefH(bool, CheckBOM, const char* buf, const char* str,
	size_t n)
	ImplRet(std::char_traits<char>::compare(Nonnull(buf), str, n) == 0)
template<size_t _vN>
YB_NONNULL(1) inline bool
CheckBOM(const char* buf, const char(&str)[_vN])
{
	return CheckBOM(buf, str, _vN - 1);
}
//@}
//! \since build 724
//@{
//! \pre 间接断言：参数的数据指针非空。
template<size_t _vN>
inline bool
CheckBOM(string_view sv, const char(&str)[_vN])
{
	return !(sv.length() < _vN - 1) && CheckBOM(sv.data(), str, _vN - 1);
}
/*!
\pre 断言：第二参数表示的字符数不超过最长的 BOM 长度。
\note 不检查读写位置，直接读取流然后检查读取的内容。
*/
//@{
//! \pre 断言：参数的数据指针非空。
YF_API bool
CheckBOM(std::istream&, string_view);
template<size_t _vN>
inline bool
CheckBOM(std::istream& is, const char(&str)[_vN])
{
	return CheckBOM(is, {str, _vN - 1});
}
//@}
//@}
//@}

//! \brief 探测 BOM 和编码。
//@{
/*!
\pre 参数指定的缓冲区至少具有 4 个字节可读。
\pre 断言：参数非空。
\return 检查的编码和 BOM 长度，若失败为 <tt>{CharSet::Null, 0}</tt> 。
\since build 619
*/
YF_API YB_NONNULL(1) pair<Encoding, size_t>
DetectBOM(const char*);
/*!
\pre 断言：参数的数据指针非空。
\return 检查的编码和 BOM 长度，若失败为 <tt>{CharSet::Null, 0}</tt> 。
\since build 694
*/
YF_API pair<Encoding, size_t>
DetectBOM(string_view);
/*!
\exception LoggedEvent 流在检查 BOM 时候读取的字符数不是非负数。
\sa VerifyEncoding
\since build 621

设置读位置为起始位置。当流大小大于 1 时试验读取前 4 字节并检查 BOM 。
若没有发现 BOM ，调用 VerifyEncoding 按前 64 字节探测编码。
读取 BOM 时若遇到流无效，直接返回 <tt>{CharSet::Null, 0}</tt> 。
*/
//@{
//! \since build 743
YF_API pair<Encoding, size_t>
DetectBOM(std::streambuf&, size_t, Encoding = CS_Default);
YF_API pair<Encoding, size_t>
DetectBOM(std::istream&, size_t, Encoding = CS_Default);
//@}
//@}

/*!
\brief 写入指定编码的 BOM 。
\return 写入的 BOM 的长度。
\note 只写入 UTF-16 、 UTF-8 或 UTF-32 的 BOM 。
\since build 617
*/
YF_API size_t
WriteBOM(std::ostream&, Text::Encoding);


/*!
\brief 跳过可选的 BOM 的流。
\sa Text::CheckBOM
\since build 805
*/
template<class _type, typename _tBOM>
void
SkipBOM(_type& is, const _tBOM& bom)
{
	if(bool(is))
	{
		if(!Text::CheckBOM(is, bom))
			// NOTE: EOF bit is cleared implicitly since WG21 N3168.
			is.seekg(0);
		else
			is.clear();
	}
}

/*!
\brief 打开跳过可选的 BOM 的流。
\sa Text::CheckBOM
\since build 805
*/
template<class _type, typename _tBOM, typename... _tParams>
unique_ptr<_type>
OpenSkippedBOMtream(const _tBOM& bom, _tParams&&... args)
{
	auto p(make_unique<_type>(yforward(args)...));

	SkipBOM(*p, bom);
	return p;
}

} // namespace Text;

} // namespace YSLib;

#endif

