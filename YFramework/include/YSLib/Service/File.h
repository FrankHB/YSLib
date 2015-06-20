/*
	© 2009-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file File.h
\ingroup Service
\brief 平台无关的文件抽象。
\version r1187
\author FrankHB <frankhb1989@gmail.com>
\since build 473
\par 创建时间:
	2009-11-24 23:14:41 +0800
\par 修改时间:
	2015-06-17 00:36 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::File
*/


#ifndef YSL_INC_Service_File_h_
#define YSL_INC_Service_File_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YCoreUtilities
#include <cwctype>
#include YFM_YSLib_Adaptor_YTextBase
#include <sstream> // for std::ostringstream;

namespace YSLib
{

/*!
\brief 文件基类。
\since build 206
*/
class YF_API File : private noncopyable
{
private:
	//! \since build 341
	std::FILE* fp; //!< 默认文件指针。
	//! \since build 341
	size_t fsize; //!< 文件大小。

public:
	/*!
	\brief 构造。
	\note 得到空对象。
	*/
	File();
	/*!
	\brief 构造：使用指定文件路径初始化对象。
	\note 自动打开文件。
	\since build 326
	*/
	//@
	//! \since build 412
	explicit
	File(const char*, const char* = "rb");
	//! \since build 412
	File(const char*, std::ios_base::openmode);
	explicit
	File(const String&, const ucs2_t* = u"rb");
	File(const String&, std::ios_base::openmode);
	//@}
	/*!
	\brief 析构。
	\note 自动关闭文件。
	*/
	virtual
	~File();

	/*!
	\brief 判断无效或有效性。
	\since build 319
	*/
	DefBoolNeg(explicit, fp)

	DefGetter(const ynothrow, FILE*, Ptr, fp) //!< 取文件指针。
	DefGetter(const ynothrow, size_t, Size, fsize) //!< 取文件大小。
	/*!
	\brief 取文件指针的位置。
	\return 成功时同 std::ftell 。
	\throw LoggedEvent 失败。
	\since build 587
	*/
	size_t
	GetPosition();

	/*!
	\brief 设置文件指针位置。
	\note 参数和返回值语义同 \c std::fseek 。
	\since build 273
	*/
	PDefH(int, Seek, ptrdiff_t offset, int whence) const
		ImplRet(std::fseek(fp, long(offset), whence))

	/*!
	\brief 检测文件结束符。
	\note 参数和返回值语义同 \c std::feof() 。
	*/
	PDefH(int, CheckEOF, ) const
		ImplRet(std::feof(fp))

private:
	/*!
	\brief 检查文件有效长度后读位置返回文件起始。
	\since build 305
	*/
	void
	CheckSize();

public:
	/*!
	\brief 关闭文件。
	\note 语义同 \c std::fclose 。
	\note 清除文件指针。
	*/
	void
	Close();

	/*!
	\brief 刷新流。
	\return 若成功则为 0 ，否则为 \c EOF 。
	\note 语义同 \c std::fflush 。
	\warning 刷新输入流或最近操作为输入的流引起未定义行为。
	\see ISO C11 7.21.5.2 。
	\since build 329
	*/
	PDefH(int, Flush, ) const
		ImplRet(std::fflush(fp))

	/*!
	\brief 以指定方式打开指定路径的文件。
	\note 语义同 \c std::fopen 。
	\note 对输入 \c openmode ，使用 ystdex::openmode_conv 转换。
	\see ISO C11 7.21.5.3 。
	\since build 326
	*/
	//@{
	//! \since build 412
	bool
	Open(const char*, const char* = "rb");
	//! \since build 412
	bool
	Open(const char*, std::ios_base::openmode);
	bool
	Open(const String&, const ucs2_t* = u"rb");
	bool
	Open(const String&, std::ios_base::openmode);
	//@}

	/*!
	\brief 连续读 \c nmemb 个大小为 \c size 文件块到 \c ptr 中。
	\return 返回成功读取的文件块数。
	\note 语义同 \c std::fread 。
	\see ISO C11 7.21.8.1 。
	\since build 290
	*/
	PDefH(size_t, Read, void* ptr, size_t size = 1U, size_t nmemb = 1U) const
		ImplRet(std::fread(ptr, size, nmemb, fp))

	/*!
	\brief 文件指针返回到文件头，语义同 std::rewind 。
	*/
	PDefH(void, Rewind, ) const
		ImplExpr(std::rewind(fp))

	/*!
	\brief 截断文件。
	\sa platform::truncate
	\note 派生类可能覆盖默认行为。
	\since build 341
	*/
	virtual bool
	Truncate(size_t) const;

	/*!
	\brief 连续写 \c nmemb 个大小为 \c size 文件块到 \c ptr 中。
	\return 返回成功写入的文件块数。
	\note 语义同 \c std::fwrite 。
	\see ISO C11 7.21.8.2 。
	\since build 329
	*/
	PDefH(size_t, Write, void* ptr, size_t size = 1U, size_t nmemb = 1U) const
		ImplRet(std::fwrite(ptr, size, nmemb, fp))
};

/*!
\param f 文件。
\pre <tt>bool(f)</tt> 。
\relates File
*/
//@{
/*!
\brief 从指定文件读字符。
\since build 607
*/
template<typename _tChar, yimpl(typename _type2 = _tChar,
	typename = ystdex::enable_if_t<
	std::is_integral<typename std::char_traits<_type2>::char_type>::value>)>
File&
operator>>(File& f, _tChar& c)
{
	YAssert(bool(f), "Invalid file found.");

	c = std::fgetc(f.GetPtr());
	return f;
}
/*!
\brief 从指定文件读空白符分隔的字符串。
\since build 326
*/
template<typename _tString,
	yimpl(typename = ystdex::enable_for_string_class_t<_tString>)>
File&
operator>>(File& f, _tString& str)
{
	YAssert(bool(f), "Invalid file found.");

	int c;

	while((c = std::fgetc(f.GetPtr())) > 0 && !std::isspace(c))
		str += c;
	return f;
}

/*!
\brief 向指定文件写字符。
\since build 326
*/
inline PDefHOp(File&, <<, File& f, char c)
	ImplRet(YAssert(bool(f), "Invalid file found."), std::fputc(c, f.GetPtr()),
		f)
//! \brief 向指定文件写字符串。
//@{
//! \since build 326
inline PDefHOp(File&, <<, File& f, const char* str)
	ImplRet(YAssert(bool(f), "Invalid file found."),
		std::fputs(str, f.GetPtr()), f)

//! \since build 600
inline PDefHOp(File&, <<, File& f, const string& str)
	ImplRet(f << str.c_str())
//@}
//! \since build 600
template<typename _type>
File&
operator<<(File& f, const _type& obj)
{
	std::ostringstream oss;

	oss << obj;
	f << oss.str();
	return f;
}
//@}

} // namespace YSLib;

#endif

