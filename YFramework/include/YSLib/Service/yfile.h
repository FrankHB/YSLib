/*
	Copyright by FrankHB 2009 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yfile.h
\ingroup Service
\brief 平台无关的文件抽象。
\version r1083
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-24 23:14:41 +0800
\par 修改时间:
	2013-04-24 00:35 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YFile
*/


#ifndef YSL_INC_Core_yfile_h_
#define YSL_INC_Core_yfile_h_ 1

#include "../Core/ycutil.h"
#include <cwctype>
#include "../Adaptor/YTextBase.h"

YSL_BEGIN

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
	explicit
	File(const_path_t, const char* = "rb");
	File(const_path_t, std::ios_base::openmode);
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
	\brief 判断无效性。
	\since build 319
	*/
	PDefHOp(bool, !, ) const ynothrow
		ImplRet(!bool(*this))

	/*!
	\brief 判断有效性。
	\since build 319
	*/
	explicit DefCvt(const ynothrow, bool, fp)

	DefGetter(const ynothrow, FILE*, Ptr, fp) //!< 取文件指针。
	DefGetter(const ynothrow, size_t, Size, fsize) //!< 取文件大小。
	/*!
	\brief 取文件指针的位置，返回值语义同 std::ftell 。
	*/
	DefGetter(const ynothrow, ptrdiff_t, Position, std::ftell(fp))

	/*!
	\brief 设置文件指针位置。
	\note 参数和返回值语义同 \c std::fseek 。
	\since build 273
	*/
	PDefH(int, Seek, ptrdiff_t offset, int whence) const
		ImplRet(std::fseek(fp, offset, whence))

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
	\warning 刷新输入流或最近操作为输入的流导致未定义行为。
	\see ISO C11 7.21.5.2 。
	\since build 329
	*/
	PDefH(int, Flush, ) const
		ImplRet(std::fflush(fp))

	/*!
	\brief 以指定方式打开指定路径的文件。
	\note 语义同 \c std::fopen 。
	\note 对于输入 \c openmode ，使用 ystdex::openmode_conv 转换。
	\see ISO C11 7.21.5.3 。
	\since build 326
	*/
	//@{
	bool
	Open(const_path_t, const char* = "rb");
	bool
	Open(const_path_t, std::ios_base::openmode);
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
		ImplRet(std::rewind(fp))

	/*!
	\brief 截断文件。
	\see platform::truncate 。
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
\brief 从指定文件读字符。
\param f 文件。
\pre <tt>bool(f)</tt> 。
\since build 326
*/
template<typename _tChar>
File&
operator>>(File& f, typename std::char_traits<_tChar>::char_type& c)
{
	YAssert(bool(f), "Invalid file found.");

	const auto fp(f.GetPtr());

	c = std::fgetc(fp);
	return f;
}
/*!
\brief 从指定文件读空白符分隔的字符串。
\param f 文件。
\pre <tt>bool(f)</tt> 。
\since build 326
*/
template<typename _tString>
File&
operator>>(File& f, _tString& str)
{
	YAssert(bool(f), "Invalid file found.");

	const auto fp(f.GetPtr());
	int c;

	while((c = std::fgetc(fp)) > 0 && !std::iswspace(c))
		str += c;
	return f;
}


/*!
\brief 向指定文件写字符。
\param f 文件。
\pre <tt>bool(f)</tt> 。
\since build 326
*/
inline File&
operator<<(File& f, char c)
{
	YAssert(bool(f), "Invalid file found.");

	std::fputc(c, f.GetPtr());
	return f;
}
/*!
\brief 向指定文件写字符串。
\param f 文件。
\pre <tt>bool(f)</tt> 。
\since build 326
*/
inline File&
operator<<(File& f, const char* str)
{
	YAssert(bool(f), "Invalid file found.");

	std::fputs(str, f.GetPtr());
	return f;
}
/*!
\brief 向指定文件写字符串。
\param f 文件。
\pre <tt>bool(f)</tt> 。
\since build 326
\todo 支持非 char 元素字符串。
*/
template<typename _tString>
File&
operator<<(File& f, const _tString& str)
{
	YAssert(bool(f), "Invalid file found.");

	std::fputs(reinterpret_cast<const char*>(str.c_str()), f.GetPtr());
	return f;
}

YSL_END

#endif

