/*
	© 2009-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file TextFile.h
\ingroup Service
\brief 平台无关的文本文件抽象。
\version r936
\author FrankHB <frankhb1989@gmail.com>
\since build 473
\par 创建时间:
	2009-11-24 23:14:41 +0800
\par 修改时间:
	2015-08-04 14:12 +0800
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
\brief 验证流的编码。
\note 第二参数和第三参数指定缓冲区，第四参数指定最大文本长度，
	第五参数参数指定验证的编码。
\since build 621
*/
//@{
YB_NONNULL(1, 2) Text::Encoding
VerifyEncoding(std::FILE*, char*, size_t, size_t, Encoding = CS_Default);
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

/*!
\brief 检查缓冲区是否具有指定的 BOM 。
\since build 619
*/
//@{
inline PDefH(bool, CheckBOM, const char* buf, const char* str, size_t n)
	ImplRet(std::char_traits<char>::compare(buf, str, n) == 0)
template<size_t _vN>
inline bool
CheckBOM(const char* buf, const char(&str)[_vN])
{
	return CheckBOM(buf, str, _vN - 1);
}
//@}

//! \brief 探测 BOM 和编码。
//@{
/*!
\pre 参数指定的缓冲区至少具有 4 个字节可读。
\return 检查的编码和 BOM 长度，若失败为 <tt>{CharSet::Null, 0}</tt> 。
\since build 619
*/
YF_API pair<Encoding, size_t>
DetectBOM(const char*);
/*!
\sa VerifyEncoding
\since build 621

设置流读位置为起始位置。当流大小大于 1 时试验读取前 4 字节并检查 BOM 。
若没有发现 BOM ，调用 VerifyEncoding 按前 64 字节探测编码。
读取 BOM 时若遇到流无效，直接返回 <tt>{CharSet::Null, 0}</tt> 。
*/
YF_API pair<Encoding, size_t>
DetectBOM(std::istream&, size_t, Encoding = CS_Default);
//@}

/*!
\brief 写入指定编码的 BOM 。
\return 写入的 BOM 的长度。
\note 只写入 UTF-16 、 UTF-8 或 UTF-32 的 BOM 。
\since build 617
*/
YF_API size_t
WriteBOM(std::ostream&, Text::Encoding);

} // namespace Text;


//! \brief 文本文件类。
class YF_API TextFile : public File
{
public:
	/*!
	\brief 编码。
	\warning 修改编码可能会造成读取错误。
	\since build 290
	*/
	Text::Encoding Encoding;

private:
	//! \brief BOM 大小。
	size_t bl = 0;

public:
	/*!
	\brief 构造：使用指定文件名、编码和模式初始化文本文件对象。
	\note 当打开文件大小为零且以可写方式打开时按编码写入 BOM 。
	\since build 412
	*/
	explicit
	TextFile(const char*, std::ios_base::openmode = std::ios_base::in,
		Text::Encoding = Text::CS_Default);
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 615
	*/
	~TextFile() override;

	/*!
	\brief 取 BOM 大小。
	\since build 520
	*/
	DefGetter(const ynothrow, size_t, BOMSize, bl)
	DefGetter(const ynothrow, size_t, TextSize, GetSize() - GetBOMSize()) \
		//!< 取文本区段大小。

	/*!
	\brief 定位：设置文件读位置。
	\since build 586
	*/
	void
	Locate(size_t) const;
};

} // namespace YSLib;

#endif

